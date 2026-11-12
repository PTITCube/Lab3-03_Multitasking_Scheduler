# Lab3-03_Multitasking_Scheduler

**Đa nhiệm FreeRTOS & Cơ chế Khóa Bảo vệ Tài nguyên (Mutex)**

Bài thực hành thuộc Phân hệ C&DH (Lab 3) trong hệ thống PTITCube.

## 1. Mục tiêu
- Phân chia hệ thống C&DH thành các tác vụ (Task) chạy độc lập, đa chu kỳ trên 2 nhân vi xử lý (Core 0 và Core 1).
- Áp dụng cơ chế khóa **Mutex** (`PTIT_BUS_SERIAL`) để ngăn ngừa xung đột dữ liệu (Race Condition) khi nhiều tác vụ cùng truy cập cổng Serial Monitor.
- Sử dụng các ngoại vi từ Lab 1 (LED GPIO 2, Serial Monitor) kết hợp cấu trúc gói tin viễn trắc giả lập (Mock Telemetry), hoàn toàn độc lập với phần cứng của các bài lab sau.

## 2. Phần cứng sử dụng
- **ESP32 NodeMCU OBC**
- **Heartbeat LED:** Chân GPIO 2 tích hợp trên bo mạch.
- **Giao tiếp:** Cổng USB UART / Serial Monitor (Baudrate: `115200`).

## 3. Các tác vụ trong hệ thống
1. `HeartbeatLED` (Chu kỳ 200ms, Core 1, Priority 2): Nhấp nháy LED trạng thái OBC.
2. `TelemGen` (Chu kỳ 1000ms, Core 1, Priority 2): Tạo gói tin viễn trắc giả lập (Điện áp pin, nhiệt độ mạch, thời gian hoạt động) và in ra Serial có khóa Mutex.
3. `SysDiag` (Chu kỳ 3000ms, Core 0, Priority 1): Thực hiện tác vụ tự chẩn đoán bộ nhớ Heap và CPU trên Core 0, in báo cáo nhiều dòng có bảo vệ Mutex.
