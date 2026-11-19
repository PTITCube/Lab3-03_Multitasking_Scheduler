/**
 * LAB 3 - VÍ DỤ 03: ĐA NHIỆM (MULTITASKING) & BẢO VỆ TÀI NGUYÊN BẰNG MUTEX
 */
#include <Arduino.h>
#include <PTITCube.h>

PTIT_CDH cdh;
PTIT_Scheduler scheduler;

#define LED_HEARTBEAT 2

void heartbeatTask(void* pvParameters) {
    pinMode(LED_HEARTBEAT, OUTPUT);
    bool state = false;
    for (;;) {
        state = !state;
        digitalWrite(LED_HEARTBEAT, state ? HIGH : LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void telemetryGeneratorTask(void* pvParameters) {
    uint16_t packetId = 0;
    for (;;) {
        float mockVoltage = 3.7f + (float)random(-20, 20) / 100.0f;
        float mockTemp    = 24.5f + (float)random(-10, 30) / 10.0f;

        if (scheduler.lock(PTIT_BUS_SERIAL, 100)) {
            cdh.printf("TELEM: #%04u | V: %.2fV | T: %.1fC | Uptime: %lus\n",
                       packetId++, mockVoltage, mockTemp, millis() / 1000);
            scheduler.unlock(PTIT_BUS_SERIAL);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void systemDiagnosticTask(void* pvParameters) {
    for (;;) {
        if (scheduler.lock(PTIT_BUS_SERIAL, 100)) {
            cdh.printf("DIAG: Heap: %u bytes | CPU: %u MHz\n",
                       ESP.getFreeHeap(), ESP.getCpuFreqMHz());
            scheduler.unlock(PTIT_BUS_SERIAL);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void setup() {
    cdh.begin(115200);

    scheduler.addTask("Heartbeat",    heartbeatTask,          500,  1, 1);
    scheduler.addTask("TelemGen",     telemetryGeneratorTask, 1000, 1, 1);
    scheduler.addTask("SysDiag",      systemDiagnosticTask,   5000, 0, 0);

    scheduler.start();
}

void loop() {
}
