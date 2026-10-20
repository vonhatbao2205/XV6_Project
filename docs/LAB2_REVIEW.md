# Rà soát và bàn giao Lab 2 — 20/10/2026

Nhóm: Võ Nhất Bảo (24120166), Phạm Lê Huy Hoàng (24120182).
Phân công trong báo cáo hiện có: Bảo làm 3.1–3.2; Hoàng làm 3.3–3.4.
Mốc rà soát phần đã có: commit `26ecdec` trên nhánh `syscall`.

## Kết quả

`make grade`: **60/60**. Tất cả test của script trong repo đều pass.
Đây là điểm của script, không phải điểm môn học; báo cáo và vấn đáp còn
được chấm riêng. `time.txt` chứa `7` theo số giờ người dùng cung cấp.

Kiểm tra bổ sung: 6 nhóm trường hợp biên sandbox và 10 phiên khởi động QEMU
riêng cho bài tái sử dụng bộ nhớ đều pass. Chuỗi thử có độ dài 1, 8, 16, 24,
bắt đầu bằng chữ số, chữ hoa hoặc chữ thường; seed 24120182.

Môi trường: WSL Ubuntu 24.04, GCC RISC-V 13.3.0, QEMU 8.2.2,
GDB 15.1. Log tóm tắt và GDB nằm trong `docs/verification/`.
Log build đầy đủ còn ở `D:\XV6_Verification\grade-final.log`.
Các file C sửa/thêm đã qua kiểm tra định dạng clang-format 23.1.3.
Bản clang-format 18 của Ubuntu 24.04 không đọc được tùy chọn mới
`ReflowComments: Never` trong cấu hình gốc; không sửa cấu hình dự án.
Các thay đổi định dạng sau lần chạy test chỉ là khoảng trắng.

## Phần của Bảo

- Phần 3.1 đúng về các kết quả chính: `usertrap()` gọi `syscall()`;
  syscall đầu tiên là `open`, số 15; SPP bằng 0; process `init`, pid 1.
- Chạy lại độc lập trên bản `26ecdec`: `sstatus = 0x200000022`.
  Khi chủ động thay phép đọc syscall number bằng phép đọc địa chỉ 0 trên
  bản kiểm tra riêng, GDB xác nhận `scause = 13`, `stval = 0`,
  lệnh `lw a3,0(zero)` ở `0x80001d6e`. Địa chỉ khác phiên ban đầu vì
  compiler/build khác; không thay các giá trị lịch sử trong câu trả lời.
- Sửa đường dẫn minh chứng trong `answers-syscall.txt`: ảnh thực tế nằm
  trong `report/images`, còn đường dẫn `evidence/lab2` cũ không có trong repo.
  Làm rõ SPIE lưu trạng thái SIE trước trap.
- Phần 3.2 đã nối đầy đủ syscall, kiểm tra mask trước handler và sao chép mask
  khi fork. Trường được đặt lại ở `freeproc`; process table ban đầu được BSS
  khởi tạo bằng 0. Không thấy lỗi chức năng trong phần này qua rà soát và test.

## Phần 3.3

Thêm `char sbpath[MAXPATH]` vào `struct proc`, dùng `uint` cho `sbmask`.
`sys_interpose()` lấy pathname bằng `argstr` vào buffer cục bộ trước khi
thay đổi trạng thái. Con trỏ sai, chuỗi thiếu NUL hoặc dài từ MAXPATH byte
trở lên trả -1 và giữ nguyên cả mask lẫn pathname hiện có.

Trong `syscall()`, syscall hợp lệ được kiểm tra trước khi gọi handler.
Call không bị mask chạy bình thường. Call bị mask trả -1, ngoại trừ
`open`/`exec` có pathname bằng chính xác `sbpath` và `sbpath` không phải `-`.
So sánh chuỗi chính xác: `README` khác `./README`; `---` là pathname thông
thường, chỉ chuỗi `-` là ký hiệu không cho phép pathname nào.

`kfork()` sao chép cả mask và pathname; `freeproc()` xóa trạng thái để không
rò hạn chế sang process khác. `exec` giữ nguyên trạng thái sandbox.

Giữ quy tắc tích lũy hạn chế của phần 3.2: gọi lại có thể thêm bit hoặc
bỏ ngoại lệ bằng `-`. Khi đã mask open/exec, không được đổi ngoại lệ sang
pathname khác hoặc khôi phục ngoại lệ đã bỏ. Đây là lựa chọn bảo thủ để
một buffer pathname không làm nới quyền cũ; không mô hình hóa nhiều ngoại lệ.

Luồng gọi: prototype trong `user.h` → stub sinh bởi `usys.pl` → `ecall` →
`uservec` lưu trapframe → `usertrap` → `syscall` kiểm tra sandbox → handler
nếu được phép → giá trị trả về trong trapframe a0 → trở về user.

## Phần 3.4

Chỉ sửa `user/attack.c` cho task này. Giữ nguyên `user/secret.c`,
`kernel/kalloc.c`, `kernel/vm.c` và hai script chấm gốc.

Trong cấu hình LAB_SYSCALL, các trang cấp phát eager không được xóa nội
dung cũ. Sau khi `secret` thoát, các trang được trả lại free list LIFO;
process mới có thể nhận lại một số trang đó khi gọi `sbrk` eager.

Theo chính `secret.c`, marker `Here it is: ` nằm ở offset
`32 + (unsigned char)secret[0]` tính từ đầu trang, và secret bắt đầu sau
marker 12 byte. Vì vậy offset secret là `44 + (unsigned char)secret[0]`.
Đây là offset suy ra từ source, không phải số dò ngẫu nhiên.

Chương trình cấp 32 trang và tìm marker ở các offset 32..287 trong từng
trang. Nó kiểm tra chuỗi chữ/số có NUL trong cùng trang và kiểm tra lại
quan hệ offset với ký tự đầu trước khi in đúng một dòng. Số trang 32 là
giới hạn tìm kiếm thực nghiệm cho source/toolchain này: page table và exec
cũng lấy trang từ free list, nên không giả định một số thứ tự trang ảo cố
định và không tuyên bố đúng cho mọi cách bố trí allocator khác.

## Chạy lại

Trong WSL, từ `/mnt/d/XV6_Project`:

```sh
make grade
python3 test-sandbox-audit.py
```

Test bổ sung tự bật `SANDBOX_AUDIT=1` để thêm `sbaudit` vào filesystem;
build thông thường không thêm chương trình này. Test bao gồm: buffer sai
và biên 127/128 byte, cập nhật atomic, không nới hạn chế, copy buffer riêng,
chuỗi qua biên trang, giữ policy qua exec, open bị chặn không tạo file,
bit cao của mask và 72 lượt fork để kiểm tra tái sử dụng process slot.

## Báo cáo và nộp bài

Báo cáo trong `report/` được giữ nguyên theo yêu cầu vì nhóm có bản mới
chưa upload. Cần đưa nội dung 3.3–3.4, ảnh kết quả chấm đầy đủ và các kết quả
kiểm chứng vào bản báo cáo cuối, tối đa 10 trang. Các file log ở đây là text,
không thay thế ảnh chụp được đề yêu cầu.

Khai báo AI trong báo cáo: Codex hỗ trợ rà soát, viết phần 3.3–3.4,
viết test bổ sung, chạy kiểm chứng GDB/QEMU và chuẩn bị ghi chú này.
Hai thành viên cần hiểu toàn bộ bài để vấn đáp.

Sau khi commit, `make zipball` tạo `lab.zip`. Bài nộp cuối là
`24120166_24120182.zip`, chứa `lab.zip` và `Report.pdf` đã hoàn thiện.
