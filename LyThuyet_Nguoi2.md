# Lý thuyết về Bubble Sort và Insertion Sort (Người 2)

Tài liệu này chứa thông tin lý thuyết về hai thuật toán **Bubble Sort** (Sắp xếp nổi bọt) và **Insertion Sort** (Sắp xếp chèn) để phục vụ cho việc làm Slide thuyết trình và báo cáo.

---

## 1. Bubble Sort (Sắp xếp nổi bọt)

### a. Độ phức tạp thuật toán (Big O)
* **Thời gian tốt nhất (Best Case):** $O(N)$
  * *Trường hợp xảy ra:* Mảng đã được sắp xếp từ trước. Khi áp dụng cải tiến (dùng biến cờ hiệu `swapped`), thuật toán chỉ cần duyệt qua mảng một lần duy nhất để kiểm tra và dừng lại khi không có hoán đổi nào xảy ra.
* **Thời gian trung bình (Average Case):** $O(N^2)$
  * *Trường hợp xảy ra:* Các phần tử phân bố ngẫu nhiên.
* **Thời gian xấu nhất (Worst Case):** $O(N^2)$
  * *Trường hợp xảy ra:* Mảng bị sắp xếp ngược (ví dụ cần sắp xếp tăng dần nhưng đầu vào lại là giảm dần). Mỗi phần tử đều phải so sánh và hoán đổi tối đa.
* **Không gian (Space Complexity):** $O(1)$
  * *Giải thích:* Thuật toán sắp xếp tại chỗ (In-place), chỉ sử dụng một vài biến phụ trợ để lưu giá trị tạm thời khi hoán đổi.

### b. Giải thích cách hoạt động (bằng lời)
1. Thuật toán bắt đầu từ đầu dãy số, so sánh từng cặp phần tử kề nhau (phần tử thứ $j$ và $j+1$).
2. Nếu phần tử trước lớn hơn phần tử sau (đối với sắp xếp tăng dần), ta thực hiện hoán đổi vị trí của chúng.
3. Sau một lượt duyệt qua toàn bộ dãy (từ $0$ đến $N-1$), phần tử lớn nhất sẽ được đưa về đúng vị trí ở cuối dãy (vị trí $N-1$). Vị trí này coi như đã được cố định.
4. Lặp lại quá trình trên với dãy con từ đầu đến vị trí $N-2$, đưa phần tử lớn thứ nhì về vị trí kế cuối.
5. Quá trình này tiếp tục cho đến khi không còn phần tử nào cần so sánh nữa.
* **Cải tiến tối ưu:** Sử dụng một biến cờ hiệu `swapped` để ghi nhận xem có xảy ra hoán đổi nào trong lượt duyệt hiện tại hay không. Nếu đi hết một lượt duyệt mà không có bất kỳ cặp nào cần hoán đổi, nghĩa là toàn bộ dãy đã được sắp xếp, thuật toán có thể kết thúc ngay lập tức mà không cần chạy tiếp các vòng lặp ngoài.

### c. Ưu điểm và Nhược điểm
* **Ưu điểm:**
  * Cực kỳ đơn giản, trực quan, dễ hiểu và dễ cài đặt đối với người mới học.
  * Không tốn thêm bộ nhớ ngoài ($O(1)$ phụ trợ).
  * Là thuật toán sắp xếp **ổn định (Stable)**: Giữ nguyên thứ tự tương đối của các phần tử có giá trị bằng nhau.
  * Nhận biết được mảng đã sắp xếp rất nhanh nhờ biến cờ hiệu ($O(N)$).
* **Nhược điểm:**
  * Hiệu năng rất kém trên các bộ dữ liệu lớn do độ phức tạp thời gian trung bình và xấu nhất là $O(N^2)$.
  * Số lượng phép so sánh và hoán đổi quá nhiều so với các thuật toán nâng cao khác.

---

## 2. Insertion Sort (Sắp xếp chèn)

### a. Độ phức tạp thuật toán (Big O)
* **Thời gian tốt nhất (Best Case):** $O(N)$
  * *Trường hợp xảy ra:* Mảng đã được sắp xếp. Trong mỗi bước duyệt, phần tử hiện tại chỉ cần so sánh đúng 1 lần với phần tử ngay trước nó và không cần dịch chuyển bất kỳ vị trí nào.
* **Thời gian trung bình (Average Case):** $O(N^2)$
  * *Trường hợp xảy ra:* Các phần tử ngẫu nhiên.
* **Thời gian xấu nhất (Worst Case):** $O(N^2)$
  * *Trường hợp xảy ra:* Mảng bị sắp xếp ngược. Mỗi phần tử mới ở vị trí $i$ đều phải so sánh và dịch chuyển qua tất cả $i$ phần tử đã sắp xếp trước đó.
* **Không gian (Space Complexity):** $O(1)$
  * *Giải thích:* Thuật toán sắp xếp tại chỗ (In-place), chỉ cần một biến tạm thời để lưu giá trị phần tử đang được chèn (`key`).

### b. Giải thích cách hoạt động (bằng lời)
1. Thuật toán coi phần tử đầu tiên (chỉ số 0) là một mảng đã được sắp xếp gồm 1 phần tử.
2. Duyệt từ phần tử thứ hai (chỉ số $1$) đến phần tử cuối cùng của mảng. Gọi phần tử đang xét ở bước hiện tại là khóa (`key`).
3. So sánh `key` với các phần tử nằm trước nó trong phần mảng đã sắp xếp (theo thứ tự từ phải sang trái, tức là từ $i-1$ lùi về $0$).
4. Nếu một phần tử trong phần đã sắp xếp lớn hơn `key`, ta dịch chuyển phần tử đó sang bên phải một vị trí để tạo khoảng trống.
5. Tiếp tục so sánh và dịch chuyển cho đến khi tìm thấy phần tử nhỏ hơn hoặc bằng `key`, hoặc khi chạm đầu mảng.
6. Chèn giá trị `key` vào khoảng trống trống vừa được tạo ra.
7. Lặp lại quá trình này cho đến khi duyệt hết tất cả các phần tử.

### c. Ưu điểm và Nhược điểm
* **Ưu điểm:**
  * Đơn giản, dễ cài đặt.
  * Hiệu quả vượt trội đối với các mảng có kích thước nhỏ.
  * Rất hiệu quả đối với các mảng đã **gần như được sắp xếp** (gần sorted) hoặc khi cần chèn trực tiếp các phần tử mới vào một mảng đã sắp xếp sẵn.
  * Là thuật toán sắp xếp **ổn định (Stable)** và chạy tại chỗ **(In-place)**.
  * Trong thực tế, chạy nhanh hơn và thực hiện ít phép so sánh hơn so với Bubble Sort và Selection Sort.
* **Nhược điểm:**
  * Không phù hợp với các bộ dữ liệu lớn vì độ phức tạp trung bình và xấu nhất vẫn là $O(N^2)$.
  * Số lần dịch chuyển dữ liệu (hoán đổi/ghi đè) vẫn ở mức cao khi gặp mảng sắp xếp ngược.
