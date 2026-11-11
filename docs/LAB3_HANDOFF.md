# Lab 3 (pgtbl): bàn giao Task 4 và 5 cho Hoàng

Bảo đã làm xong Task 1, 2, 3 trên nhánh `pgtbl`. Trên NixOS (gcc 15.3.0,
QEMU 10.2.4), `make grade` được 26/46: `vmprint_test`, `pgaccess`,
`answers-pgtbl.txt`, `time` đạt; `usertests -q` in `ALL TESTS PASSED`.
`ugetpid` và `superpg` còn lỗi vì là Task 4 và 5.

## Đã có trong repo

| Tệp | Nội dung |
| --- | --- |
| `kernel/riscv.h` | `PTE_A` (bit 6), `PTE_D` (bit 7) |
| `kernel/vm.c` | `vmprint()` và hàm đệ quy `vmprint_level()` |
| `kernel/sysproc.c` | `sys_pgaccess()`; `sys_vmprint`/`sys_pgaccess` trả `uint64` |
| `user/pgaudit.c`, `Makefile` | 23 ca kiểm thử thêm cho `pgaccess` |
| `answers-pgtbl.txt` | Q1, Q2 xong; Q3, Q4 còn dòng `[Task 4, ...]`, `[Task 5, ...]` |
| `report/Report.tex` | Mục 1 đến 5 xong; phần của Hoàng nằm trong hộp màu cam |
| `time.txt` | 10 (Bảo chốt: 2 giờ cho mỗi task, 5 task) |

## Những chỗ Task 4 và 5 chạm vào phần đã làm

- `vmprint_level()` đã nhận lá bằng `R|W|X` ở mọi mức và không đệ quy vào
  lá, nên superpage ở mức 1 đã in ra một dòng có quyền. Rubric T5a cần
  `vmprint` đánh dấu superpage: thêm dấu hiệu ở nhánh lá khi `level > 0`.
- Q1 và Q2 (cả trong báo cáo) đã ghi trước rằng sau Task 4, dòng
  `0x3FFFFFD000` của `print_pgtbl` có `perm 0x13` (V|R|U) và `vmprint` có
  thêm lá `0x0000003fffffd000 ... RU`. Nếu Task 4 dùng quyền khác, sửa
  câu đó ở `answers-pgtbl.txt` (Q1, Q2) và Bảng 3 của báo cáo.
- `pgaccess` chấp nhận mọi trang có `PTE_V|PTE_U`, nên trang `USYSCALL`
  (có `U`) đọc được bit A qua `pgaccess`. Không cần sửa gì.
- Regex `vmprint_test` của grader vẫn khớp khi có thêm lá `USYSCALL`.

## Việc cần làm

1. Code Task 4 và Task 5 (không sửa `grade-lab-pgtbl`, `gradelib.py`,
   `user/pgtbltest.c`).
2. Thay hai dòng giữ chỗ Q3, Q4 trong `answers-pgtbl.txt`.
3. Báo cáo: điền mục 6, 7, các ô `\hoang{...}` trong bảng 1, 2, 5, 6, 7,
   mục Hạn chế và khai báo AI ở mục 10, rồi xóa các hộp `hoangbox`.
   Hiện báo cáo 8 trang, giới hạn là 10.
4. Kiểm tra sau khi merge: `make grade` (mong đợi 46/46), `pgtbltest` in
   `pgtbltest: all tests succeeded`, `pgaudit` vẫn 23/23.
5. Chụp lại ảnh: `vmprint.png`, `print-pgtbl.png` (có `USYSCALL`) và
   `grade.png`. Bảo chạy `scripts/capture_lab3.py` trên máy mình (lệnh ở
   đầu script) và cắt ảnh như cũ, hoặc Hoàng chụp tay.
