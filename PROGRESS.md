# Tiến độ Lab 2 — 20/10/2026

Thư mục làm việc: `D:\XV6_Project`, nhánh `syscall`.

## Đã kiểm chứng

- Đọc đề CS333 và rà soát các commit của Võ Nhất Bảo.
- Viết phần 3.3 (pathname sandbox) và 3.4 (`user/attack.c`).
- Chạy thật `make grade` trong WSL Ubuntu 24.04, QEMU RISC-V.
- Kết quả cuối: **60/60**. Tất cả 8 test sandbox, test attack và kiểm tra
  sự tồn tại của answers-syscall.txt đều OK. Kiểm tra answers của grader
  không thay thế chấm nội dung sáu câu trả lời bằng tay.
- `time.txt = 7` theo số giờ người dùng cung cấp.
- Log đầy đủ: `D:\XV6_Verification\grade-final.log`.
- Kiểm tra bổ sung: 6 nhóm trường hợp biên sandbox và 10 lượt chạy phần
  3.4 đều pass. Log: `docs/verification/audit-final.txt`.
- Đã kiểm chứng GDB độc lập cho phần 3.1 trên bản commit gốc của Bảo.
  Kết quả chính khớp; địa chỉ lệnh thay đổi theo compiler/build.

## Hoàn tất mã nguồn

- Mã nguồn đã hoàn tất và được commit; kiểm thử chính và bổ sung đều đạt.
- Đã kiểm tra định dạng các file C sửa/thêm bằng clang-format 23.1.3.
- Nhánh làm việc: `syscall`. Xem `git log -3 --oneline` để đối chiếu commit
  và `git status --short` để kiểm tra thay đổi chưa commit.

## Báo cáo

Giữ nguyên báo cáo trong repo vì nhóm còn bản mới chưa upload.
Thông tin nhóm: Võ Nhất Bảo — 24120166; Phạm Lê Huy Hoàng — 24120182.
Ghi chú kỹ thuật/kết quả kiểm thử: `docs/LAB2_REVIEW.md` và
`docs/verification/`, dùng để ghép vào báo cáo.

File này ghi trạng thái tại lần cập nhật gần nhất, không phải chỉ báo
tiến trình chạy nền. Nếu phiên chat bị ngắt, không mặc định công việc
vẫn đang được thực hiện.
