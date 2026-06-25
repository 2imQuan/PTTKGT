import sys

def bubble_sort_trace(arr):
    comparisons = 0
    swaps = 0
    n = len(arr)
    print(f"\nMảng ban đầu: {arr}")
    print("-" * 50)
    
    for i in range(n - 1):
        swapped = False
        print(f"\n--- Vòng lặp ngoài thứ {i+1} ---")
        for j in range(n - i - 1):
            comparisons += 1
            comp_msg = f"So sánh {arr[j]} và {arr[j+1]}"
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
                swaps += 1
                swapped = True
                print(f"{comp_msg:<20} -> Lớn hơn -> Hoán đổi -> Mảng: {arr}")
            else:
                print(f"{comp_msg:<20} -> Nhỏ hơn hoặc bằng -> Giữ nguyên")
                
        if not swapped:
            print("Mảng đã được sắp xếp xong trước thời hạn (không có hoán đổi nào ở vòng này).")
            break
            
    print("-" * 50)
    return comparisons, swaps

def insertion_sort_trace(arr):
    comparisons = 0
    swaps = 0
    n = len(arr)
    print(f"\nMảng ban đầu: {arr}")
    print("-" * 50)
    
    for i in range(1, n):
        key = arr[i]
        j = i - 1
        print(f"\n--- Bước {i}: Chèn khóa key = {key} vào phần đã sắp xếp {arr[:i]} ---")
        
        while True:
            if j < 0:
                print(f"Chạm đầu mảng. Đặt key = {key} tại vị trí 0")
                break
            
            comparisons += 1
            comp_msg = f"So sánh {arr[j]} > {key}"
            
            if arr[j] > key:
                arr[j + 1] = arr[j]
                swaps += 1
                print(f"{comp_msg:<20} -> Đúng -> Dịch {arr[j]} sang phải -> Mảng: {arr}")
                j -= 1
            else:
                print(f"{comp_msg:<20} -> Sai -> Dừng dịch chuyển")
                break
                
        arr[j + 1] = key
        swaps += 1
        print(f"Đặt key = {key} vào vị trí thích hợp -> Mảng: {arr}")
        
    print("-" * 50)
    return comparisons, swaps

def main():
    print("=" * 60)
    print("  CÔNG CỤ CHẠY THỬ VÀ TRỰC QUAN HÓA THUẬT TOÁN (NGƯỜI 2)")
    print("=" * 60)
    
    # Nhập dữ liệu
    try:
        user_input = input("Nhập dãy số cần sắp xếp (cách nhau bằng khoảng trắng, ví dụ: 5 2 9 1 7): ")
        arr = [int(x) for x in user_input.strip().split()]
        if not arr:
            raise ValueError
    except ValueError:
        print("Dữ liệu nhập không hợp lệ, sử dụng mảng mặc định: [24, 12, 35, 2, 89, 45, 6, 17]")
        arr = [24, 12, 35, 2, 89, 45, 6, 17]
        
    print("\nChọn thuật toán để chạy thử:")
    print("1. Bubble Sort (Sắp xếp nổi bọt)")
    print("2. Insertion Sort (Sắp xếp chèn)")
    choice = input("Nhập lựa chọn của bạn (1 hoặc 2): ").strip()
    
    if choice == '1':
        comps, swps = bubble_sort_trace(arr)
        print(f"\nKẾT QUẢ CUỐI CÙNG (BUBBLE SORT):")
        print(f" - Mảng đã sắp xếp: {arr}")
        print(f" - Tổng số lần so sánh: {comps}")
        print(f" - Tổng số lần hoán đổi: {swps}")
    elif choice == '2':
        comps, swps = insertion_sort_trace(arr)
        print(f"\nKẾT QUẢ CUỐI CÙNG (INSERTION SORT):")
        print(f" - Mảng đã sắp xếp: {arr}")
        print(f" - Tổng số lần so sánh: {comps}")
        print(f" - Tổng số lần hoán đổi/dịch chuyển: {swps}")
    else:
        print("Lựa chọn không hợp lệ!")

if __name__ == "__main__":
    main()
