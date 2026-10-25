# Kiểm chứng trên NixOS — 25/10/2026

Các phép đo làm căn cứ cho báo cáo (mục 6.2, 8.1, 8.2), chạy trên mã nguồn bài
làm ở commit `e969443`. Thư mục `docs/` không nằm trong `lab.zip`.

Môi trường: NixOS, `shell.nix` của nhóm (nằm ngoài repo), riscv64-none-elf-gcc
15.3.0, GNU Binutils 2.46, QEMU 10.2.4 với 3 CPU. Mỗi phép đo boot QEMU một lần
bằng `make qemu` và gõ lệnh qua `run_cmds.py` / `kalloc-trace/run_trace.py`
(pexpect).

| Tệp | Nội dung |
|---|---|
| `grade-and-audit.txt` | `make grade` 60/60 và `python3 test-sandbox-audit.py` 2/2 trên `e969443`. |
| `stale-secret.log` | `secret xyzzy; attack; secret 0Kq9; attack; attack`: lần attack đầu sau `secret 0Kq9` in `xyzzy`, lần sau in `0Kq9`. |
| `interpose-null-probe.patch`, `interpose-null-probe.log` | Lệnh in tạm trong `sys_interpose()`: với `sandbox 32768 - sbtest escape`, `interpose(0, 0)` cho `argstr = 6` (chuỗi 6 byte mã máy ở VA 0 của `sbtest`, byte 0 ở offset 6) và bị quy tắc không nới quyền từ chối vì `sbmask = 0x8000`. |
| `kalloc-trace/` | Trace cấp phát trang cho Bảng 6 của báo cáo (xem dưới). |

## kalloc-trace

`ktrace.patch` (chỉ dùng để đo, không commit vào kernel) in một dòng cho mỗi
`kalloc()`/`kfree()` sau khi boot: thao tác, địa chỉ trang, địa chỉ trả về của
hàm gọi, pid, tên process, và `SECRET` nếu trang có marker `"Here it is: "` ở
offset `32 + byte đầu của secret`. `uvmalloc()` in thêm khoảng `oldsz..newsz` và
địa chỉ ảo của từng trang được map.

Tái lập:

```sh
git worktree add /tmp/kt e969443 && cd /tmp/kt
git apply <repo>/docs/verification/nixos-20261025/kalloc-trace/ktrace.patch
make kernel/kernel fs.img
python3 <...>/kalloc-trace/run_trace.py trace.log         # secret xyzzy; attack; attack
python3 <...>/kalloc-trace/analyze_trace.py trace.log kernel/kernel
```

`analyze_trace.py` dịch địa chỉ hàm gọi bằng `addr2line`, kiểm tra mỗi lần
`kalloc()` lấy đúng trang được `kfree()` gần nhất (LIFO), đếm số trang nằm trên
trang secret sau mỗi nhóm sự kiện, và tính index của trang secret trong vùng
`sbrk(32 * PGSIZE)` của `attack`. Kết quả trong `analysis.txt`: trang vật lý
`0x87f25000`, index 19 ở lần attack thứ nhất và 15 ở lần thứ hai. Hai lần boot
cho cùng chuỗi sự kiện. Đây là số liệu của phiên đo này, không phải hằng số
của xv6.
