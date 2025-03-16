#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pgmspace.h>

const char *ssid = "byte_sender_2.4GHz";
const char *password = "qwer123456";

ESP8266WebServer server(80);

const char HTML_PAGE[] PROGMEM = R"rawliteral(


<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Config Ayarları</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            overflow-x: auto;
        }
        .container {
            max-width: 1400px;
            margin: 40px auto;
            padding: 20px;
            background: white;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            border-radius: 10px;
            text-align: left;
            overflow-x: auto;
            transition: height 0.3s ease;
        }
        .collapsed {
            height: 50px;
            overflow: hidden;
        }
        .header {
            font-size: 18px;
            font-weight: bold;
            color: white;
            background-color: #28a745;
            padding: 10px;
            border-radius: 8px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            cursor: pointer; /* Sadece başlık tıklanabilir */
        }
        .arrow {
            transition: transform 0.3s ease;
        }
        .arrow.rotate {
            transform: rotate(180deg); /* Ok yön değiştirir */
        }
        .table-container {
            display: flex;
            flex-direction: column;
            align-items: flex-start;
            overflow-x: auto;
        }
        table {
            width: 100%;
            min-width: 800px;
            border-collapse: collapse;
            margin-top: 10px;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: center;
            font-size: 12px;
            white-space: nowrap;
        }
        th {
            background-color: #f8f8f8;
            font-weight: bold;
        }
        td input, td select {
            width: 32px;
            height: 22px;
            font-size: 12px;
            text-align: center;
            border: 1px solid #ccc;
            border-radius: 6px;
            background: #f9f9f9;
        }
        button {
            padding: 8px 15px;
            font-size: 14px;
            color: white;
            background: #28a745;
            border: none;
            border-radius: 6px;
            cursor: pointer;
        }
        button:hover {
            background: #218838;
        }
    </style>
</head>
<body>

    <div class="container collapsed" id="isdContainer">
        <div class="header" onclick="toggleContainer('isdContainer', 'isdContent', 'isdArrow')">
            ISD_CONFIG
            <span class="arrow" id="isdArrow">▼</span>
        </div>
        <div class="content" id="isdContent" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="ISD enable">ISD_EN</th>
                        <th title="Brake enable">BRAKE_EN</th>
                        <th title="Hi-Z enable">HIZ_EN</th>
                        <th title="Reverse drive enable">RVS_DR_EN</th>
                        <th title="Resynchronization enable">RESYNC_EN</th>
                        <th title="Minimum speed threshold to resynchronize to close loop (% of MAX_SPEED)">FW_DRV_RESYN_THR</th>
                        <th title="Brake mode">BRK_MODE</th>
                        <th title="Brake time">BRK_TIME</th>
                        <th title="Hi-Z time">HIZ_TIME</th>
                        <th title="BEMF threshold to detect if motor is stationary">STAT_DETECT_THR</th>
                        <th title="Speed threshold used to transition to open loop during reverse deceleration (% of MAX_SPEED)">REV_DRV_HANDOFF_THR</th>
                        <th title="Open loop current limit during speed reversal">REV_DRV_OPEN_LOOP_CURRENT</th>
                        <th>Read/Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="ISD_EN" readonly></td>
                        <td><input type="text" id="BRAKE_EN" readonly></td>
                        <td><input type="text" id="HIZ_EN" readonly></td>
                        <td><input type="text" id="RVS_DR_EN" readonly></td>
                        <td><input type="text" id="RESYNC_EN" readonly></td>
                        <td><input type="text" id="FW_DRV_RESYN_THR" readonly></td>
                        <td><input type="text" id="BRK_MODE" readonly></td>
                        <td><input type="text" id="BRK_TIME" readonly></td>
                        <td><input type="text" id="HIZ_TIME" readonly></td>
                        <td><input type="text" id="STAT_DETECT_THR" readonly></td>
                        <td><input type="text" id="REV_DRV_HANDOFF_THR" readonly></td>
                        <td><input type="text" id="REV_DRV_OPEN_LOOP_CURRENT" readonly></td>
                        <td><button onclick="ReadISDConfig()">Read</button></td>
                    </tr>
                    <tr>
                        <td><select id="ISD_EN_SELECT"><option value="0">0</option><option value="1">1</option></select></td>
                        <td><select id="BRAKE_EN_SELECT"><option value="0">0</option><option value="1">1</option></select></td>
                        <td><select id="HIZ_EN_SELECT"><option value="0">0</option><option value="1">1</option></select></td>
                        <td><select id="RVS_DR_EN_SELECT"><option value="0">0</option><option value="1">1</option></select></td>
                        <td><select id="RESYNC_EN_SELECT"><option value="0">0</option><option value="1">1</option></select></td>
                        <td>
                            <select id="FW_DRV_RESYN_THR_SELECT">
                                <option value="0">0 = 5%</option>
                                <option value="1">1 = 10%</option>
                                <option value="2">2 = 15%</option>
                                <option value="3">3 = 20%</option>
                                <option value="4">4 = 25%</option>
                                <option value="5">5 = 30%</option>
                                <option value="6">6 = 35%</option>
                                <option value="7">7 = 40%</option>
                                <option value="8">8 = 45%</option>
                                <option value="9">9 = 50%</option>
                                <option value="A">A = 55%</option>
                                <option value="B">B = 60%</option>
                                <option value="C">C = 70%</option>
                                <option value="D">D = 80%</option>
                                <option value="E">E = 90%</option>
                                <option value="F">F = 100%</option>
                            </select>
                        </td>
                        <td>
                            <select id="BRK_MODE_SELECT">
                                <option value="0">0 = All three high side FETs ON</option>
                                <option value="1">1 = All three low side FETs ON</option>
                            </select>
                        </td>
                        <td>
                            <select id="BRK_TIME_SELECT">
                                <option value="0">0 = 10 ms</option>
                                <option value="1">1 = 50 ms</option>
                                <option value="2">2 = 100 ms</option>
                                <option value="3">3 = 200 ms</option>
                                <option value="4">4 = 300 ms</option>
                                <option value="5">5 = 400 ms</option>
                                <option value="6">6 = 500 ms</option>
                                <option value="7">7 = 750 ms</option>
                                <option value="8">8 = 1 s</option>
                                <option value="9">9 = 2 s</option>
                                <option value="A">A = 3 s</option>
                                <option value="B">B = 4 s</option>
                                <option value="C">C = 5 s</option>
                                <option value="D">D = 7.5 s</option>
                                <option value="E">E = 10 s</option>
                                <option value="F">F = 15 s</option>
                            </select>
                        </td>
                        <td>
                            <select id="HIZ_TIME_SELECT">
                                <option value="0">0 = 10 ms</option>
                                <option value="1">1 = 50 ms</option>
                                <option value="2">2 = 100 ms</option>
                                <option value="3">3 = 200 ms</option>
                                <option value="4">4 = 300 ms</option>
                                <option value="5">5 = 400 ms</option>
                                <option value="6">6 = 500 ms</option>
                                <option value="7">7 = 750 ms</option>
                                <option value="8">8 = 1 s</option>
                                <option value="9">9 = 2 s</option>
                                <option value="A">A = 3 s</option>
                                <option value="B">B = 4 s</option>
                                <option value="C">C = 5 s</option>
                                <option value="D">D = 7.5 s</option>
                                <option value="E">E = 10 s</option>
                                <option value="F">F = 15 s</option>
                            </select>
                        </td>
                        <td>
                            <select id="STAT_DETECT_THR_SELECT">
                                <option value="0">0 = 50 mV</option>
                                <option value="1">1 = 75 mV</option>
                                <option value="2">2 = 100 mV</option>
                                <option value="3">3 = 250 mV</option>
                                <option value="4">4 = 500 mV</option>
                                <option value="5">5 = 750 mV</option>
                                <option value="6">6 = 1000 mV</option>
                                <option value="7">7 = 1500 mV</option>
                            </select>
                        </td>
                        <td>
                            <select id="REV_DRV_HANDOFF_THR_SELECT">
                                <option value="0">0 = 2.5%</option>
                                <option value="1">1 = 5%</option>
                                <option value="2">2 = 7.5%</option>
                                <option value="3">3 = 10%</option>
                                <option value="4">4 = 12.5%</option>
                                <option value="5">5 = 15%</option>
                                <option value="6">6 = 20%</option>
                                <option value="7">7 = 25%</option>
                                <option value="8">8 = 30%</option>
                                <option value="9">9 = 40%</option>
                                <option value="A">A = 50%</option>
                                <option value="B">B = 60%</option>
                                <option value="C">C = 70%</option>
                                <option value="D">D = 80%</option>
                                <option value="E">E = 90%</option>
                                <option value="F">F = 100%</option>
                            </select>
                        </td>
                        <td>
                            <select id="REV_DRV_OPEN_LOOP_CURRENT_SELECT">
                                <option value="0">0 = 1.5 A</option>
                                <option value="1">1 = 2.5 A</option>
                                <option value="2">2 = 3.5 A</option>
                                <option value="3">3 = 5.0 A</option>
                            </select>
                        </td>
                        <td><button onclick="WriteISDConfig()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

    <div class="container collapsed" id="revContainer">
        <!-- Tıklanabilir Başlık -->
        <div class="header" onclick="toggleContainer('revContainer', 'revContent', 'revArrow')">
            REV_DRIVE_CONFIG
            <span class="arrow" id="revArrow">▼</span>
        </div>

        <!-- İçerik -->
        <div class="content" id="revContent" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Open loop acceleration coefficient A1 during reverse drive">REV_DRV_OPEN_LOOP_ACCEL_A1</th>
                        <th title="Open loop acceleration coefficient A2 during reverse drive">REV_DRV_OPEN_LOOP_ACCEL_A2</th>
                        <th title="Bus current limit during active braking">ACTIVE_BRAKE_CURRENT_LIMIT</th>
                        <th title="10-bit value for active braking loop Kp. Kp = ACTIVE_BRAKE_KP / 2^7">ACTIVE_BRAKE_KP</th>
                        <th title="10-bit value for active braking loop Ki. Ki = ACTIVE_BRAKE_KI / 2^9">ACTIVE_BRAKE_KI</th>
                        <th>Read/Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="REV_DRV_OPEN_LOOP_ACCEL_A1" readonly></td>
                        <td><input type="text" id="REV_DRV_OPEN_LOOP_ACCEL_A2" readonly></td>
                        <td><input type="text" id="ACTIVE_BRAKE_CURRENT_LIMIT" readonly></td>
                        <td><input type="text" id="ACTIVE_BRAKE_KP" readonly></td>
                        <td><input type="text" id="ACTIVE_BRAKE_KI" readonly></td>
                        <td><button onclick="ReadRevDriveConfig()">Read</button></td>
                    </tr>
                    <tr>
                        <td>
                            <select id="REV_DRV_OPEN_LOOP_ACCEL_A1_SELECT">
                                <option value="0">0 = 0.01 Hz/s</option>
                                <option value="1">1 = 0.05 Hz/s</option>
                                <option value="2">2 = 1 Hz/s</option>
                                <option value="3">3 = 2.5 Hz/s</option>
                                <option value="4">4 = 5 Hz/s</option>
                                <option value="5">5 = 10 Hz/s</option>
                                <option value="6">6 = 25 Hz/s</option>
                                <option value="7">7 = 50 Hz/s</option>
                                <option value="8">8 = 75 Hz/s</option>
                                <option value="9">9 = 100 Hz/s</option>
                                <option value="A">A = 250 Hz/s</option>
                                <option value="B">B = 500 Hz/s</option>
                                <option value="C">C = 750 Hz/s</option>
                                <option value="D">D = 1000 Hz/s</option>
                                <option value="E">E = 5000 Hz/s</option>
                                <option value="F">F = 10000 Hz/s</option>
                            </select>
                        </td>

                        <td>
                            <select id="REV_DRV_OPEN_LOOP_ACCEL_A2_SELECT">
                                <option value="0">0 = 0.0 Hz/s²</option>
                                <option value="1">1 = 0.05 Hz/s²</option>
                                <option value="2">2 = 1 Hz/s²</option>
                                <option value="3">3 = 2.5 Hz/s²</option>
                                <option value="4">4 = 5 Hz/s²</option>
                                <option value="5">5 = 10 Hz/s²</option>
                                <option value="6">6 = 25 Hz/s²</option>
                                <option value="7">7 = 50 Hz/s²</option>
                                <option value="8">8 = 75 Hz/s²</option>
                                <option value="9">9 = 100 Hz/s²</option>
                                <option value="A">A = 250 Hz/s²</option>
                                <option value="B">B = 500 Hz/s²</option>
                                <option value="C">C = 750 Hz/s²</option>
                                <option value="D">D = 1000 Hz/s²</option>
                                <option value="E">E = 5000 Hz/s²</option>
                                <option value="F">F = 10000 Hz/s²</option>
                            </select>
                        </td>

                        <td>
                            <select id="ACTIVE_BRAKE_CURRENT_LIMIT_SELECT">
                                <option value="0">0 = 0.5 A</option>
                                <option value="1">1 = 1 A</option>
                                <option value="2">2 = 2 A</option>
                                <option value="3">3 = 3 A</option>
                                <option value="4">4 = 4 A</option>
                                <option value="5">5 = 5 A</option>
                                <option value="6">6 = 6 A</option>
                                <option value="7">7 = 7 A</option>
                            </select>
                        </td>

                        <td>
                            <input type="text" id="ACTIVE_BRAKE_KP_INPUT" value="0">
                        </td>

                        <td>
                            <input type="text" id="ACTIVE_BRAKE_KI_INPUT" value="0">
                        </td>

                        <td><button onclick="WriteRevDriveConfig()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

    <script>
        
        function toggleContainer(containerId, contentId, arrowId) {
            let container = document.getElementById(containerId);
            let content = document.getElementById(contentId);
            let arrow = document.getElementById(arrowId);
            
            if (content.style.display === "none" || content.style.display === "") {
                content.style.display = "block";
                container.classList.remove("collapsed");
                arrow.classList.add("rotate");
            } else {
                content.style.display = "none";
                container.classList.add("collapsed");
                arrow.classList.remove("rotate");
            }
        }
        
        function ReadISDConfig() {
        fetch('/ReadISDConfig', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('ISD_EN').value = data.ISD_EN;
            document.getElementById('BRAKE_EN').value = data.BRAKE_EN;
            document.getElementById('HIZ_EN').value = data.HIZ_EN;
            document.getElementById('RVS_DR_EN').value = data.RVS_DR_EN;
            document.getElementById('RESYNC_EN').value = data.RESYNC_EN;
            document.getElementById('FW_DRV_RESYN_THR').value = data.FW_DRV_RESYN_THR;
            document.getElementById('BRK_MODE').value = data.BRK_MODE;
            document.getElementById('BRK_TIME').value = data.BRK_TIME;
            document.getElementById('HIZ_TIME').value = data.HIZ_TIME;
            document.getElementById('STAT_DETECT_THR').value = data.STAT_DETECT_THR;
            document.getElementById('REV_DRV_HANDOFF_THR').value = data.REV_DRV_HANDOFF_THR;
            document.getElementById('REV_DRV_OPEN_LOOP_CURRENT').value = data.REV_DRV_OPEN_LOOP_CURRENT;

            // Açılır kutulara da aynı verileri yazalım
            document.getElementById('ISD_EN_SELECT').value = data.ISD_EN;
            document.getElementById('BRAKE_EN_SELECT').value = data.BRAKE_EN;
            document.getElementById('HIZ_EN_SELECT').value = data.HIZ_EN;
            document.getElementById('RVS_DR_EN_SELECT').value = data.RVS_DR_EN;
            document.getElementById('RESYNC_EN_SELECT').value = data.RESYNC_EN;
            document.getElementById('FW_DRV_RESYN_THR_SELECT').value = data.FW_DRV_RESYN_THR;
            document.getElementById('BRK_MODE_SELECT').value = data.BRK_MODE;
            document.getElementById('BRK_TIME_SELECT').value = data.BRK_TIME;
            document.getElementById('HIZ_TIME_SELECT').value = data.HIZ_TIME;
            document.getElementById('STAT_DETECT_THR_SELECT').value = data.STAT_DETECT_THR;
            document.getElementById('REV_DRV_HANDOFF_THR_SELECT').value = data.REV_DRV_HANDOFF_THR;
            document.getElementById('REV_DRV_OPEN_LOOP_CURRENT_SELECT').value = data.REV_DRV_OPEN_LOOP_CURRENT;
            })
            .catch(error => console.error('Hata:', error));
          }

          function WriteISDConfig() {
            let data = {
                ISD_EN: document.getElementById('ISD_EN_SELECT').value,
                BRAKE_EN: document.getElementById('BRAKE_EN_SELECT').value,
                HIZ_EN: document.getElementById('HIZ_EN_SELECT').value,
                RVS_DR_EN: document.getElementById('RVS_DR_EN_SELECT').value,
                RESYNC_EN: document.getElementById('RESYNC_EN_SELECT').value,
                FW_DRV_RESYN_THR: document.getElementById('FW_DRV_RESYN_THR_SELECT').value,
                BRK_MODE: document.getElementById('BRK_MODE_SELECT').value,
                BRK_TIME: document.getElementById('BRK_TIME_SELECT').value,
                HIZ_TIME: document.getElementById('HIZ_TIME_SELECT').value,
                STAT_DETECT_THR: document.getElementById('STAT_DETECT_THR_SELECT').value,
                REV_DRV_HANDOFF_THR: document.getElementById('REV_DRV_HANDOFF_THR_SELECT').value,
                REV_DRV_OPEN_LOOP_CURRENT: document.getElementById('REV_DRV_OPEN_LOOP_CURRENT_SELECT').value
            };

            fetch('/WriteISDConfig', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(response => response.text())
            .then(response => console.log("Yazma başarılı:", response))
            .catch(error => console.error('Yazma hatası:', error));
        }

        function ReadRevDriveConfig() {
        fetch('/ReadRevDriveConfig', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A1').value = data.REV_DRV_OPEN_LOOP_ACCEL_A1;
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A2').value = data.REV_DRV_OPEN_LOOP_ACCEL_A2;
            document.getElementById('ACTIVE_BRAKE_CURRENT_LIMIT').value = data.ACTIVE_BRAKE_CURRENT_LIMIT;
            document.getElementById('ACTIVE_BRAKE_KP').value = data.ACTIVE_BRAKE_KP;
            document.getElementById('ACTIVE_BRAKE_KI').value = data.ACTIVE_BRAKE_KI;


            // Açılır kutulara da aynı verileri yazalım
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A1_SELECT').value = data.REV_DRV_OPEN_LOOP_ACCEL_A1;
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A2_SELECT').value = data.REV_DRV_OPEN_LOOP_ACCEL_A2;
            document.getElementById('ACTIVE_BRAKE_CURRENT_LIMIT_SELECT').value = data.ACTIVE_BRAKE_CURRENT_LIMIT;
            document.getElementById('ACTIVE_BRAKE_KP_INPUT').value = data.ACTIVE_BRAKE_KP;
            document.getElementById('ACTIVE_BRAKE_KI_INPUT').value = data.ACTIVE_BRAKE_KI;

            })
        .catch(error => console.error('Hata:', error));
          }

          function WriteRevDriveConfig() {
            let data = {
                REV_DRV_OPEN_LOOP_ACCEL_A1: document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A1_SELECT').value,
                REV_DRV_OPEN_LOOP_ACCEL_A2: document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A2_SELECT').value,
                ACTIVE_BRAKE_CURRENT_LIMIT: document.getElementById('ACTIVE_BRAKE_CURRENT_LIMIT_SELECT').value,
                ACTIVE_BRAKE_KP: document.getElementById('ACTIVE_BRAKE_KP_INPUT').value.trim(),
                ACTIVE_BRAKE_KI: document.getElementById('ACTIVE_BRAKE_KI_INPUT').value.trim()

              };

            fetch('/WriteRevDriveConfig', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
              })
            .then(response => response.text())
            .then(response => console.log("Yazma başarılı:", response))
            .catch(error => console.error('Yazma hatası:', error));
          }

    </script>

</body>
</html>



)rawliteral";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register Address Definitions
#define ISD_CONFIG 0x00000080
#define REV_DRIVE_CONFIG 0x00000082
#define MOTOR_STARTUP1 0x00000084
#define MOTOR_STARTUP2 0x00000086
#define CLOSED_LOOP1 0x00000088
#define CLOSED_LOOP2 0x0000008A
#define CLOSED_LOOP3 0x0000008C
#define CLOSED_LOOP4 0x0000008E
#define SPEED_PROFILES1 0x00000094
#define SPEED_PROFILES2 0x00000096
#define SPEED_PROFILES3 0x00000098
#define SPEED_PROFILES4 0x0000009A
#define SPEED_PROFILES5 0x0000009C
#define SPEED_PROFILES6 0x0000009E
#define FAULT_CONFIG1 0x00000090
#define FAULT_CONFIG2 0x00000092
#define PIN_CONFIG 0x000000A4
#define DEVICE_CONFIG1 0x000000A6
#define DEVICE_CONFIG2 0x000000A8
#define PERI_CONFIG1 0x000000AA
#define GD_CONFIG1 0x000000AC
#define GD_CONFIG2 0x000000AE
#define ALGO_CTRL1 0x000000EA
#define INT_ALGO_1 0x000000A0
#define INT_ALGO_2 0x000000A2

unsigned long register_value = 0;

#define READ_BITS(value, high, low) ((value >> low) & ((1 << (high - low + 1)) - 1))
#define WRITE_BITS(target, value, high, low) (target |= ((value & ((1 << (high - low + 1)) - 1)) << low))

struct ISDConfig
{
  byte ISD_EN;
  byte BRAKE_EN;
  byte HIZ_EN;
  byte RVS_DR_EN;
  byte RESYNC_EN;
  byte FW_DRV_RESYN_THR;
  byte BRK_MODE;
  byte BRK_TIME;
  byte HIZ_TIME;
  byte STAT_DETECT_THR;
  byte REV_DRV_HANDOFF_THR;
  byte REV_DRV_OPEN_LOOP_CURRENT;
};

struct REVDRIVEConfig
{
  byte REV_DRV_OPEN_LOOP_ACCEL_A1;
  byte REV_DRV_OPEN_LOOP_ACCEL_A2;
  byte ACTIVE_BRAKE_CURRENT_LIMIT;
  byte ACTIVE_BRAKE_KP;
  byte ACTIVE_BRAKE_KI;
};

ISDConfig ISD_Config;
REVDRIVEConfig REV_DRIVE_Config;

byte getJsonHexValue(String json, String key)
{
  int startIndex = json.indexOf("\"" + key + "\":");
  if (startIndex == -1)
    return 0; // Eğer bulunamazsa, 0 döndür

  startIndex += key.length() + 3; // "KEY": sonrası başlama noktası
  int endIndex = json.indexOf(",", startIndex);
  if (endIndex == -1)
    endIndex = json.indexOf("}", startIndex); // Son eleman için "}" ile bitiyor

  String hexValue = json.substring(startIndex, endIndex);
  hexValue.trim();            // Boşlukları temizle
  hexValue.replace("\"", ""); // Eğer çift tırnak varsa temizle

  return (byte)strtol(hexValue.c_str(), NULL, 16); // HEX formatında ayrıştır
}

byte getJsonIntValue(String json, String key)
{
  int startIndex = json.indexOf("\"" + key + "\":");
  if (startIndex == -1)
    return 0; // Eğer bulunamazsa, 0 döndür

  startIndex += key.length() + 3; // "KEY": sonrası başlama noktası
  int endIndex = json.indexOf(",", startIndex);
  if (endIndex == -1)
    endIndex = json.indexOf("}", startIndex); // Son eleman için "}" ile bitiyor

  String intValue = json.substring(startIndex, endIndex);
  intValue.trim();            // Boşlukları temizle
  intValue.replace("\"", ""); // Eğer çift tırnak varsa temizle

  return (byte)intValue.toInt(); // INT formatında ayrıştır ve byte olarak döndür
}

String toUpperCaseHex(byte value)
{
  String hexString = String(value, HEX); // HEX string'e çevir
  hexString.toUpperCase();               // Büyük harfe çevir
  return hexString;
}

void read32(int reg_addr)
{
  byte reg_addr_H = (reg_addr & 0x0F00) >> 8;
  byte reg_addr_L = (reg_addr & 0xFF);

  const byte control_word[] = {0x90, reg_addr_H, reg_addr_L};

  Wire.beginTransmission(0x01);
  Wire.write(control_word, 3);
  Wire.endTransmission();

  Wire.requestFrom(0x01, 4);

  for (int i = 0; i < 4; i++)
  {
    unsigned long c = Wire.read();
    register_value |= (c << (i * 8));
  }

  Serial.print("Register Okundu: 0x");
  Serial.println(reg_addr, HEX);
  Serial.print("Değer: 0x");
  Serial.println(register_value, HEX);
}

void write32(int reg_addr, unsigned long writedata)
{
  byte reg_addr_H = (reg_addr & 0x0F00) >> 8;
  byte reg_addr_L = (reg_addr & 0xFF);

  const byte control_word[] = {0x10, reg_addr_H, reg_addr_L};
  byte data[] = {0x00, 0x00, 0x00, 0x00};

  for (int i = 0; i < 4; i++)
  {
    data[i] = ((writedata & (((unsigned long)0xFF) << (i * 8))) >> (i * 8));
  }

  Wire.beginTransmission(0x01);
  Wire.write(control_word, 3);
  Wire.write(data, 4);
  Wire.endTransmission();

  Serial.print("Yazıldı: 0x");
  Serial.println(reg_addr, HEX);
  Serial.print("Değer: 0x");
  Serial.println(writedata, HEX);
}

void handleRoot()
{
  server.send(200, "text/html", HTML_PAGE);
}

void ReadISDConfig()
{
  register_value = 0;
  read32(ISD_CONFIG);

  ISD_Config.ISD_EN = READ_BITS(register_value, 30, 30);
  ISD_Config.BRAKE_EN = READ_BITS(register_value, 29, 29);
  ISD_Config.HIZ_EN = READ_BITS(register_value, 28, 28);
  ISD_Config.RVS_DR_EN = READ_BITS(register_value, 27, 27);
  ISD_Config.RESYNC_EN = READ_BITS(register_value, 26, 26);
  ISD_Config.FW_DRV_RESYN_THR = READ_BITS(register_value, 25, 22);
  ISD_Config.BRK_MODE = READ_BITS(register_value, 21, 21);
  ISD_Config.BRK_TIME = READ_BITS(register_value, 16, 13);
  ISD_Config.HIZ_TIME = READ_BITS(register_value, 12, 9);
  ISD_Config.STAT_DETECT_THR = READ_BITS(register_value, 8, 6);
  ISD_Config.REV_DRV_HANDOFF_THR = READ_BITS(register_value, 5, 2);
  ISD_Config.REV_DRV_OPEN_LOOP_CURRENT = READ_BITS(register_value, 1, 0);

  String json = "{ \"ISD_EN\": \"" + toUpperCaseHex(ISD_Config.ISD_EN) +
                "\", \"BRAKE_EN\": \"" + toUpperCaseHex(ISD_Config.BRAKE_EN) +
                "\", \"HIZ_EN\": \"" + toUpperCaseHex(ISD_Config.HIZ_EN) +
                "\", \"RVS_DR_EN\": \"" + toUpperCaseHex(ISD_Config.RVS_DR_EN) +
                "\", \"RESYNC_EN\": \"" + toUpperCaseHex(ISD_Config.RESYNC_EN) +
                "\", \"FW_DRV_RESYN_THR\": \"" + toUpperCaseHex(ISD_Config.FW_DRV_RESYN_THR) +
                "\", \"BRK_MODE\": \"" + toUpperCaseHex(ISD_Config.BRK_MODE) +
                "\", \"BRK_TIME\": \"" + toUpperCaseHex(ISD_Config.BRK_TIME) +
                "\", \"HIZ_TIME\": \"" + toUpperCaseHex(ISD_Config.HIZ_TIME) +
                "\", \"STAT_DETECT_THR\": \"" + toUpperCaseHex(ISD_Config.STAT_DETECT_THR) +
                "\", \"REV_DRV_HANDOFF_THR\": \"" + toUpperCaseHex(ISD_Config.REV_DRV_HANDOFF_THR) +
                "\", \"REV_DRV_OPEN_LOOP_CURRENT\": \"" + toUpperCaseHex(ISD_Config.REV_DRV_OPEN_LOOP_CURRENT) + "\" }";

  server.send(200, "application/json", json);
}

void WriteISDConfig()
{
  if (server.hasArg("plain"))
  {
    String body = server.arg("plain");

    ISD_Config.ISD_EN = getJsonHexValue(body, "ISD_EN");
    ISD_Config.BRAKE_EN = getJsonHexValue(body, "BRAKE_EN");
    ISD_Config.HIZ_EN = getJsonHexValue(body, "HIZ_EN");
    ISD_Config.RVS_DR_EN = getJsonHexValue(body, "RVS_DR_EN");
    ISD_Config.RESYNC_EN = getJsonHexValue(body, "RESYNC_EN");
    ISD_Config.FW_DRV_RESYN_THR = getJsonHexValue(body, "FW_DRV_RESYN_THR");
    ISD_Config.BRK_MODE = getJsonHexValue(body, "BRK_MODE");
    ISD_Config.BRK_TIME = getJsonHexValue(body, "BRK_TIME");
    ISD_Config.HIZ_TIME = getJsonHexValue(body, "HIZ_TIME");
    ISD_Config.STAT_DETECT_THR = getJsonHexValue(body, "STAT_DETECT_THR");
    ISD_Config.REV_DRV_HANDOFF_THR = getJsonHexValue(body, "REV_DRV_HANDOFF_THR");
    ISD_Config.REV_DRV_OPEN_LOOP_CURRENT = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_CURRENT");

    register_value = 0;
    WRITE_BITS(register_value, 0, 31, 31);
    WRITE_BITS(register_value, ISD_Config.ISD_EN, 30, 30);
    WRITE_BITS(register_value, ISD_Config.BRAKE_EN, 29, 29);
    WRITE_BITS(register_value, ISD_Config.HIZ_EN, 28, 28);
    WRITE_BITS(register_value, ISD_Config.RVS_DR_EN, 27, 27);
    WRITE_BITS(register_value, ISD_Config.RESYNC_EN, 26, 26);
    WRITE_BITS(register_value, ISD_Config.FW_DRV_RESYN_THR, 25, 22);
    WRITE_BITS(register_value, ISD_Config.BRK_MODE, 21, 21);
    WRITE_BITS(register_value, 9, 20, 17);
    WRITE_BITS(register_value, ISD_Config.BRK_TIME, 16, 13);
    WRITE_BITS(register_value, ISD_Config.HIZ_TIME, 12, 9);
    WRITE_BITS(register_value, ISD_Config.STAT_DETECT_THR, 8, 6);
    WRITE_BITS(register_value, ISD_Config.REV_DRV_HANDOFF_THR, 5, 2);
    WRITE_BITS(register_value, ISD_Config.REV_DRV_OPEN_LOOP_CURRENT, 1, 0);

    write32(ISD_CONFIG, register_value);

    server.send(200, "application/json", "{\"status\":\"success\"}");
  }
  else
  {
    server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
  }
}

void ReadRevDriveConfig()
{
  register_value = 0;
  read32(REV_DRIVE_CONFIG);

  REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A1 = READ_BITS(register_value, 30, 27);
  REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A2 = READ_BITS(register_value, 26, 23);
  REV_DRIVE_Config.ACTIVE_BRAKE_CURRENT_LIMIT = READ_BITS(register_value, 22, 20);
  REV_DRIVE_Config.ACTIVE_BRAKE_KP = READ_BITS(register_value, 19, 10);
  REV_DRIVE_Config.ACTIVE_BRAKE_KI = READ_BITS(register_value, 9, 0);

  String json = "{ \"REV_DRV_OPEN_LOOP_ACCEL_A1\": \"" + toUpperCaseHex(REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A1) +
                "\", \"REV_DRV_OPEN_LOOP_ACCEL_A2\": \"" + toUpperCaseHex(REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A2) +
                "\", \"ACTIVE_BRAKE_CURRENT_LIMIT\": \"" + toUpperCaseHex(REV_DRIVE_Config.ACTIVE_BRAKE_CURRENT_LIMIT) +
                "\", \"ACTIVE_BRAKE_KP\": \"" + toUpperCaseHex(REV_DRIVE_Config.ACTIVE_BRAKE_KP) +
                "\", \"ACTIVE_BRAKE_KI\": \"" + toUpperCaseHex(REV_DRIVE_Config.ACTIVE_BRAKE_KI) + "\" }";

  server.send(200, "application/json", json);
}

void WriteRevDriveConfig()
{
  if (server.hasArg("plain"))
  {
    String body = server.arg("plain");

    REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A1 = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_ACCEL_A1");
    REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A2 = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_ACCEL_A2");
    REV_DRIVE_Config.ACTIVE_BRAKE_CURRENT_LIMIT = getJsonHexValue(body, "ACTIVE_BRAKE_CURRENT_LIMIT");
    REV_DRIVE_Config.ACTIVE_BRAKE_KP = getJsonHexValue(body, "ACTIVE_BRAKE_KP");
    REV_DRIVE_Config.ACTIVE_BRAKE_KI = getJsonHexValue(body, "ACTIVE_BRAKE_KI");

    register_value = 0;
    WRITE_BITS(register_value, 0, 31, 31);
    WRITE_BITS(register_value, REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A1, 30, 27);
    WRITE_BITS(register_value, REV_DRIVE_Config.REV_DRV_OPEN_LOOP_ACCEL_A2, 26, 23);
    WRITE_BITS(register_value, REV_DRIVE_Config.ACTIVE_BRAKE_CURRENT_LIMIT, 22, 20);
    WRITE_BITS(register_value, REV_DRIVE_Config.ACTIVE_BRAKE_KP, 19, 10);
    WRITE_BITS(register_value, REV_DRIVE_Config.ACTIVE_BRAKE_KI, 9, 0);

    write32(REV_DRIVE_CONFIG, register_value);

    server.send(200, "application/json", "{\"status\":\"success\"}");
  }
  else
  {
    server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(4, 5);
  Wire.setClock(100000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Bağlandı!");
  Serial.print("ESP'nin IP Adresi: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/ReadISDConfig", HTTP_GET, ReadISDConfig);
  server.on("/WriteISDConfig", HTTP_POST, WriteISDConfig);
  server.on("/ReadRevDriveConfig", HTTP_GET, ReadRevDriveConfig);
  server.on("/WriteRevDriveConfig", HTTP_POST, WriteRevDriveConfig);
  server.begin();
}

void loop()
{
  server.handleClient();
}
