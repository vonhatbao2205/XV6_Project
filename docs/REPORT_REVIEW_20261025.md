# Nhận xét báo cáo CS333 Lab 2

Ngày rà soát: 25/10/2026.

## 1. Đúng bản và phạm vi kiểm tra

- Báo cáo được đọc: [report/Report.pdf](../report/Report.pdf), 9 trang (bản tương ứng `Report (3).pdf` được cung cấp).
- File trùng từng byte với `report/Report.pdf` tại commit
  `85f3581522c83ac3665f3dc85b689e219b8b5e35` trên nhánh `syscall`.
- SHA-256: `d4d438fdd0d64975ffc1e25a92af7e330676a7261656e4f4c80731c056e1f717`.
- Đối chiếu với đề CS333 trên Google Docs, mã nguồn và script chấm của chính
  commit trên. Đọc toàn bộ source LaTeX và xem trực quan cả 9 trang PDF;
  kiểm tra thêm trang 8 bằng Poppler ở 160 dpi.
- Chạy lại `make grade` trên checkout riêng của commit này trong WSL Ubuntu
  24.04: **60/60**. Đây là kiểm chứng mới, không chỉ lặp lại kết quả ngày 20/10.
- Chạy lại `python3 test-sandbox-audit.py` trên cùng commit: **2/2**, gồm
  6 nhóm kiểm tra sandbox và 10 lần boot riêng cho bài tái sử dụng bộ nhớ.
- Chạy thêm hai tình huống để phân biệt một lần thử và hai lần thử của `attack`.
- Kiểm tra GitHub Actions của chính commit báo cáo: Linux thất bại ở kiểm tra
  định dạng; macOS build thành công. Không dùng kết quả build macOS để suy ra
  đã chạy kiểm thử chức năng trên macOS.

Bản rà soát này chỉ bổ sung nhận xét và minh chứng; không sửa PDF hoặc mã nguồn bài làm.
Những số liệu về NixOS và instrumentation được đánh giá theo tài liệu nhóm
cung cấp; không được ghi thành kết quả độc lập đã đo lại trên NixOS.

## 2. Đánh giá tổng thể

Đây là một báo cáo tốt về nội dung kỹ thuật và có thể tiếp tục hoàn thiện để
nộp, không cần viết lại toàn bộ. Đủ các thành phần bắt buộc theo đề, nằm trong
giới hạn 10 trang, giải thích được cơ chế chứ không chỉ đưa ảnh hoặc chép code.

Điểm mạnh nhất là phần sandbox: trình bày trường dữ liệu, vòng đời trạng thái,
fork/exec, vị trí kiểm tra trước handler, buffer pathname và quy tắc không nới
quyền khá chặt chẽ. Phần GDB bám phiên chạy và source thực tế. Phần 3.4 đã suy
ra đúng offset và chủ động nêu giới hạn của cách tìm kiếm.

Các điểm nên sửa tập trung vào: diễn giải đúng kết quả script chấm; khả năng
đọc ảnh bằng chứng; liên kết các số liệu đo với log/phiên đo; và hạn chế những
câu khẳng định rộng hơn điều mã hoặc thí nghiệm chứng minh được.

## 3. Đối chiếu yêu cầu báo cáo

| Yêu cầu của đề | Vị trí trong PDF | Nhận xét |
|---|---|---|
| Thông tin nhóm, phân công, tỷ lệ đóng góp | Trang 1–2, Bảng 1 | Có đủ tên, MSSV, công việc và 50%/50%. |
| Tự đánh giá mức hoàn thành và kết quả make grade | Trang 2 và 8–9 | Có đủ, nhưng cần giải thích giới hạn của điểm script. |
| Ảnh toàn bộ kết quả chấm | Trang 8, Hình 8 | Có các test và tổng điểm, nhưng ảnh thu nhỏ nhiều. |
| Sáu câu GDB kèm ảnh | Trang 3–4, Hình 1–3 | Có đủ; số liệu quan trọng phù hợp ảnh và source. |
| Các file sửa, các trường mới trong proc | Trang 4–5, Bảng 4–5 | Đầy đủ và khớp code. |
| Đường đi của interpose và syscall bị từ chối | Trang 5, Hình 4 | Đúng, trực quan, có cả chiều trả về user. |
| Giải thích vì sao attack hoạt động, cách xác định offset | Trang 7–8 | Công thức đúng; phần hành trình trang cần bổ sung căn cứ đo. |
| Khó khăn, hạn chế, tham khảo | Trang 9 | Có nội dung thực chất, không chỉ nêu chung chung. |
| Khai báo dùng AI và công việc AI hỗ trợ | Trang 9, mục [6] | Đã khai báo Claude Code và Codex với phạm vi cụ thể. |
| Tối đa 10 trang | Toàn file | Đạt: 9 trang kể cả bìa. |

## 4. Những chỉnh sửa nên ưu tiên trước khi nộp

### 4.1. Trang 8: tách kiểm tra tự động answers-syscall.txt khỏi chấm nội dung

Bảng 7 ghi: “Tệp trả lời đủ nội dung (chấm tay)” bên cạnh kết quả 5/5 của
`make grade`. Cách đặt này dễ khiến người đọc hiểu script đã xác nhận sáu câu
trả lời đúng hoặc việc chấm tay đã được thực hiện.

Thực tế, `gradelib.py::check_answers(file, n=10)` chỉ mở file và kiểm tra nội
dung sau strip dài ít nhất 10 ký tự. Nó không đọc hiểu các câu trả lời và
không kiểm tra ảnh GDB. Nội dung vẫn phải do người chấm đánh giá.

Đề xuất sửa ô mô tả:

> Kiểm tra file tồn tại và có tối thiểu nội dung; sáu câu trả lời và ảnh minh
> chứng được chấm thủ công theo rubric.

Thêm một câu ngay dưới bảng tổng điểm:

> 60/60 là kết quả của script kiểm thử đi kèm. Điểm đồ án còn phụ thuộc nội
> dung trả lời GDB, báo cáo, code review và phần vấn đáp của từng thành viên.

Việc tự đánh giá mức “Xuất sắc” theo rubric không tự nó sai; nên ghi rõ đó
là tự đánh giá của nhóm, không phải điểm đã được giảng viên xác nhận.

### 4.2. Trang 8: tăng kích thước Hình 8 và làm chính xác chú thích

Hình 8 nằm trong cột 27% chiều rộng trang, trong khi ảnh chứa toàn bộ tên test,
kết quả, cảnh báo linker và exit code. Có thể đọc khi phóng lớn PDF, nhưng
khó kiểm chứng nhanh khi in A4 hoặc xem ở tỷ lệ 100%.

Hình 3 ở trang 4 và một số code hai cột tại trang 5–6 cũng cần phóng lớn mới
đọc thoải mái. Bố cục không tràn lề hoặc chồng chữ; vấn đề là mật độ thông tin.

Đề xuất:

1. Dành chiều ngang lớn hơn cho ảnh chấm, hoặc chia thành hai ảnh nối tiếp.
2. Giữ đủ tên test, kết quả, tổng điểm và exit code. Có thể thêm log text riêng
   để tra cứu; log text không thay ảnh mà đề yêu cầu.
3. Chú thích “Toàn bộ kết quả các test của make grade” chính xác hơn
   “Toàn bộ output” khi phần build đã được rút gọn/cắt.
4. Sửa ghi chú “RWX ... bị cắt trong ảnh”: ảnh hiện tại vẫn nhìn thấy thông
   báo `LOAD segment with RWX permissions`, dù đường dẫn đầu dòng bị cắt.

Báo cáo đang có 9 trang; có thể dùng một phần trang thứ 10 để làm minh chứng
dễ đọc hơn, thay vì cố nén mọi thứ.

### 4.3. Trang 7, Bảng 6: bổ sung log cho hành trình trang nhớ

Bảng này có giá trị giải thích, nhưng đưa ra nhiều số liệu thực nghiệm rất cụ
thể: địa chỉ vật lý `0x87f25000`, 13 trang ở trên secret, fork lấy 11 trang,
exec lấy 10 trang, vị trí index 19 rồi index 15. Source của `secret.c` chỉ đủ
chứng minh công thức offset bên trong trang; không tự chứng minh các con số
cấp phát/giải phóng cụ thể của một phiên chạy.

Trong repo hiện không tìm thấy script instrumentation hoặc raw log làm căn
cứ cho bảng này. Điều đó không chứng minh số liệu sai, nhưng làm người đọc
khó kiểm tra và tái lập.

Nên giữ một trace ngắn có: thao tác alloc/free, địa chỉ trang, caller, process,
thứ tự sự kiện; kèm commit, toolchain và chuỗi lệnh của phiên đo. Có thể lưu
ngoài lab.zip và dẫn từ chú thích bảng. Phải giữ kernel và secret.c của bản
nộp không có instrumentation.

Nếu không còn trace gốc, nên nói rõ phần nào suy luận từ source, phần nào
được quan sát trước đây; tránh trình bày toàn bảng như phép đo còn truy xuất
được nếu không có căn cứ để đối chiếu.

### 4.4. Trang 7: đổi cách nói về index cố định

Câu “Con số này cố định với mã nguồn và toolchain này” còn mạnh. Thứ tự
free list còn phụ thuộc lịch sử lệnh, trạng thái heap và chuỗi cấp phát trước
đó. Có cùng compiler không đủ để bảo đảm cùng index trong mọi phiên.

Đề xuất:

> Trong phiên đo với môi trường và chuỗi lệnh nêu trên, nhóm quan sát trang
> chứa secret ở index 19 trong lần attack thứ nhất và index 15 trong lần thứ
> hai. Các index này mô tả phiên đo, không phải hằng số của xv6; thuật toán
> quét 32 trang thay vì mã hóa cứng vị trí.

Giữ công thức `32 + c` và `44 + c`: đó là công thức suy ra trực tiếp từ
`secret.c`, có cơ sở khác hẳn index trang quan sát bằng thực nghiệm.

### 4.5. Trang 9: mô tả đầy đủ tình huống secret cũ, kể cả lần thử thứ hai

Báo cáo đã trung thực nêu:

```text
secret xyzzy
attack        -> xyzzy
secret 0Kq9
attack        -> xyzzy
```

Tôi tái hiện được đúng tình huống này trong WSL. Sau đó thử thêm một lần
`attack` theo quyền thử hai lần mà đề cho phép, kết quả là:

```text
secret xyzzy
attack        -> xyzzy
secret 0Kq9
attack        -> xyzzy
attack        -> 0Kq9
```

Vì đề chấp nhận một trong hai lần attack tìm được secret, ca thử đầu tiên
không đủ để kết luận bài vi phạm yêu cầu lab. Không nên tự hạ điểm chỉ vì
ca một lần thử này; cũng không nên lấy 10 lần boot mới để chứng minh luôn
trả secret mới nhất ngay lần đầu trong một phiên có nhiều secret.

Nên bổ sung kết quả cả hai lần thử vào mục 8.2 và nói rõ: thuật toán không
xác định tuổi của dữ liệu; nó trả ứng viên đầu tiên thỏa điều kiện. Đây là
giới hạn về hành vi, còn kết quả bộ test của lab vẫn là đạt.

### 4.6. Trang 8: điều kiện kiểm tra marker không chứng minh nguồn gốc/độ mới

Câu “kiểm tra chéo ... để loại chuỗi đánh dấu không do secret ghi” nên thu
hẹp thành:

> Kiểm tra marker, tập ký tự, NUL và quan hệ offset giúp giảm nhận nhầm ứng
> viên. Các điều kiện này không xác định ứng viên thuộc lần chạy secret nào.

Điều này nhất quán với giới hạn secret cũ mà nhóm đã nêu.

### 4.7. Trang 2–3: bổ sung các file công cụ được nhắc trong báo cáo

Báo cáo viện dẫn `shell.nix` và `scripts/capture_lab2.py`, nhưng hai file này
không có trong cây Git tại commit được rà soát.

Đề không bắt buộc nộp script chụp hình, nên đây không phải thiếu file nộp bài
bắt buộc. Tuy nhiên, khi báo cáo dùng tên đường dẫn cụ thể để giải thích môi
trường và nguồn ảnh, nên đính kèm file, dẫn tới vị trí truy cập được, hoặc nói
rõ đó là công cụ nội bộ nằm ngoài gói nộp.

Thêm commit đã kiểm thử bên cạnh commit gốc `99161db`, để người đọc biết
kết quả 60/60 ứng với bản bài làm nào. Trong lần rà soát này, source ở
`85f3581` được thử trực tiếp.

### 4.8. Trang 6 và 9: điều kiện thành công của argstr cần nói đủ

Địa chỉ 0 có thể hợp lệ trong user page table của xv6. Nhưng chỉ việc địa chỉ
bắt đầu hợp lệ chưa đủ để khẳng định `argstr` thành công: vẫn phải đọc được
toàn chuỗi và gặp NUL trước giới hạn MAXPATH.

Đề xuất diễn đạt:

> Trong bản sbtest được thử, argstr tại địa chỉ 0 đọc được một chuỗi kết thúc
> trong giới hạn; vì vậy interpose(0, 0) không thể được xem là phép thử con trỏ
> không hợp lệ. Ca con trỏ sai cần dùng địa chỉ ngoài không gian user hoặc
> chuỗi không kết thúc trong giới hạn.

Quy tắc không nới quyền của implementation vẫn đúng; nhận xét này chỉ làm
giải thích chặt chẽ hơn.

## 5. Nhận xét từng phần kỹ thuật

### Task 3.1 — tốt, nên giữ cách bám source thực tế

- Q1: `usertrap()` gọi `syscall()`, giải thích frame trampoline có cơ sở.
- Q2: số 15 là SYS_open trong source đang dùng; giải thích không áp dụng
  máy móc initcode.S của phiên bản cũ là đúng. Nên ghi “gợi ý trong đề CS333”
  cho rõ; trang MIT 2026 hiện dẫn tới `user/init.c` và `user/init.asm`.
- Q3: SPP = 0 chỉ previous mode là user; các bit SIE/SPIE và UXL được giải
  thích phù hợp. Nếu giữ nhãn UXL = 10 trên hình, nên ghi rõ đó là nhị phân.
- Q4–Q6: mô tả `lw a3,0(zero)`, scause=13, stval=0, process init pid 1
  hợp lý, có ảnh minh chứng. Đúng khi ghi địa chỉ phụ thuộc bản build.

Chưa thấy sai kiến thức cốt lõi trong sáu câu; cần cải thiện kích thước ảnh.

### Task 3.2 — phần vững nhất

Giải thích đúng chuỗi prototype → stub → ecall → trap → dispatcher → handler,
đường trả về qua a0; syscall bị từ chối không chạy handler. Trình bày OR mask,
không cho escape, kế thừa qua fork, giữ qua exec và xóa trạng thái process
slot là đầy đủ. Hình 4 minh họa cụ thể nên giữ.

### Task 3.3 — chính xác và có phân tích giới hạn

Mô tả buffer tạm trước khi cập nhật trạng thái, giữ nguyên policy nếu đọc
pathname lỗi, exact string matching, phân biệt '-' với '---', sao chép cho
con và giữ qua exec đều khớp code. Giải thích hai lần đọc pathname dựa trên
mô hình process hiện tại của xv6 là phù hợp; không suy rộng nó sang hệ có
threads dùng chung address space.

Quy tắc thay đổi pathname sau khi đã mask được nêu là lựa chọn bảo thủ;
đây là cách trình bày tốt, tránh khiến người đọc hiểu nó là quy định tường
minh chung cho mọi hệ sandbox.

### Task 3.4 — công thức đúng, số liệu đo cần căn cứ và phạm vi rõ

Marker dài 12 byte, offset marker `32+c`, offset secret `44+c`, ví dụ x=120
cho 152/164 đều đúng. Phân biệt sbrk eager với lazy là cần thiết vì nhánh
vmfault lazy vẫn có xử lý zero trang.

Quét có giới hạn và không mã hóa cứng index là hợp lý trong phạm vi bài.
Nên giữ riêng ba lớp: công thức từ source; index quan sát trong một trace;
kết quả thực nghiệm qua các lần test. Không dùng một lớp thay cho lớp kia.

## 6. GitHub Actions: thông tin ngoài báo cáo cần phân biệt

Run của commit hiện tại:
https://github.com/vonhatbao2205/XV6_Project/actions/runs/39449504711

- Linux FAIL ở `make fmt && git diff --exit-code`.
- Log lần này cho thấy formatter tạo diff trong `kernel/kalloc.c`,
  `kernel/syscall.h`, `kernel/vm.c`, `user/secret.c`; đây là kiểm tra định
  dạng, không phải kết quả make grade.
- Không quy lỗi lần này cho thông báo `invalid boolean` của một formatter
  cũ: log hiện tại có các diff cụ thể, cần mô tả theo đúng lần chạy.
- macOS SUCCESS ở build; job này không chạy make grade.

Báo cáo không nói CI xanh, vì vậy không có mâu thuẫn trực tiếp. Nhưng khi
mô tả độ hoàn thiện chung, nên tách rõ local grade 60/60 và CI formatting.
Tôi không sửa workflow trong lượt nhận xét báo cáo này.

## 7. Trình bày và tài liệu tham khảo

Bìa, hệ màu, header/footer, bảng phân công và bảng file/trường dữ liệu nhất
quán. Đủ 9 trang; không phát hiện tràn chữ ra ngoài trang hay chữ/bảng chồng
nhau khi kiểm tra trực quan. Trang 2 và 9 khá dày; trang 5–6 có code nhỏ.

Nên giảm một phần code chép nguyên đoạn để dành diện tích cho minh chứng.
Ưu tiên đoạn thể hiện quyết định thiết kế, thay vì đặt thêm nhiều dòng đã
có đầy đủ trong source. Bố cục nên giúp người chấm kiểm chứng được điều nhóm
khẳng định ngay tại chỗ.

Các đường dẫn MIT, sách xv6 và repo đặc tả RISC-V đều mở được tại thời điểm
rà soát. Nên thêm trích dẫn ngay trong thân bài: mục GDB/đặc quyền dẫn [3],
[4]; cơ chế lab dẫn [1], [2]; Bảng 6 dẫn trace của nhóm. Mục [5] nên có
đường dẫn commit mã nguồn cụ thể của nhóm, thay vì chỉ một danh sách file.

Đề CS333 là nguồn quyết định cách đóng gói và rubric của môn. Không lấy quy
trình nộp Gradescope của MIT để thay yêu cầu nộp zip chứa Report.pdf của CS333.

## 8. Thứ tự chỉnh sửa thực tế

1. Sửa mô tả test answers-syscall.txt; thêm câu phân biệt 60/60 với điểm môn.
2. Phóng lớn Hình 8/Hình 3; sửa chú thích output và ghi chú RWX.
3. Bổ sung trace và ngữ cảnh đo cho Bảng 6; bỏ khẳng định index cố định.
4. Thêm kết quả attack lần hai vào mục hạn chế; không suy rộng khả năng
   nhận diện secret mới nhất từ các test boot mới.
5. Sửa hai câu về argstr và độ chắc chắn của kiểm tra marker.
6. Bổ sung vị trí shell.nix/script chụp, commit đã thử và trích dẫn tại chỗ.

Sau các chỉnh sửa này, trọng tâm chuẩn bị là vấn đáp: cả hai thành viên nên
giải thích được tại sao chặn ở dispatcher, vì sao cần sao chép policy khi fork,
vì sao exec giữ policy, vì sao offset secret không phải index trang và vì sao
một kiểm tra marker không xác định tuổi của dữ liệu.

## 9. Bằng chứng kèm bản nhận xét

Các file sau nằm trong [verification/report-review-20261025](verification/report-review-20261025/):

- [grade.txt](verification/report-review-20261025/grade.txt): kết quả toàn bộ test của make grade trên commit được review, 60/60. Đây là phần kết quả test trích từ log build đầy đủ.
- [extra-tests.txt](verification/report-review-20261025/extra-tests.txt): kiểm tra bổ sung trên cùng commit, 2/2.
- [repeated-secret-one-attempt.txt](verification/report-review-20261025/repeated-secret-one-attempt.txt): transcript tái hiện việc lần thử đầu trả secret cũ.
- [repeated-secret-two-attempts.txt](verification/report-review-20261025/repeated-secret-two-attempts.txt): transcript lần thử thứ hai trả đúng 0Kq9.

Ảnh render từng trang chỉ được dùng để kiểm tra bố cục; xem PDF gốc để đối chiếu các nhận xét theo trang. Log CI được tra trực tiếp tại [GitHub Actions](https://github.com/vonhatbao2205/XV6_Project/actions/runs/39449504711).

Nguồn đề CS333:
https://docs.google.com/document/d/1eC_qlWX9naIL3oKWk0cKQoCJeXKtipb327C4srIyBAE/edit

Nguồn MIT đối chiếu quy tắc thử attack hai lần:
https://pdos.csail.mit.edu/6.828/2026/labs/syscall.html
