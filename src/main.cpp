// MCF8316A GUI
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
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
            max-width: 1800px;
            margin: 20px auto;
            padding: 20px;
            background: white;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            border-radius: 10px;
            text-align: left;
            overflow-x: auto;
            transition: height 0.3s ease;
        }
        .collapsed {
            height: 40px;
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
            width: 50px;
            height: 30px;
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

    <div class="container collapsed" id="isdConfigContainer">
        <div class="header" onclick="toggleContainer('isdConfigContainer', 'isdConfigContent', 'isdConfigArrow')">
            ISD_CONFIG
            <span class="arrow" id="isdConfigArrow">▼</span>
        </div>
        <div class="content" id="isdConfigContent" style="display: none;">
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
                        <th>Read / Write</th>
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
                        <td><select id="ISD_EN_SELECT"><option value="0">0 = Disable</option><option value="1">1 = Enable</option></select></td>
                        <td><select id="BRAKE_EN_SELECT"><option value="0">0 = Disable</option><option value="1">1 = Enable</option></select></td>
                        <td><select id="HIZ_EN_SELECT"><option value="0">0 = Disable</option><option value="1">1 = Enable</option></select></td>
                        <td><select id="RVS_DR_EN_SELECT"><option value="0">0 = Disable</option><option value="1">1 = Enable</option></select></td>
                        <td><select id="RESYNC_EN_SELECT"><option value="0">0 = Disable</option><option value="1">1 = Enable</option></select></td>
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

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

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
                        <th>Read / Write</th>
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

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="motorStartup1Container">
        <!-- Tıklanabilir Başlık -->
        <div class="header" onclick="toggleContainer('motorStartup1Container', 'motorStartup1Content', 'motorStartup1Arrow')">
            MOTOR_STARTUP1
            <span class="arrow" id="motorStartup1Arrow">▼</span>
        </div>

        <!-- İçerik -->
        <div class="content" id="motorStartup1Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Motor start-up method">MTR_STARTUP</th>
                        <th title="Align, slow first cycle and open loop current ramp rate">ALIGN_SLOW_RAMP_RATE</th>
                        <th title="Align time">ALIGN_TIME</th>
                        <th title="Align or slow first cycle current limit">ALIGN_OR_SLOW_CURRENT_ILIMIT</th>
                        <th title="IPD clock frequency">IPD_CLK_FREQ</th>
                        <th title="IPD current threshold">IPD_CURR_THR</th>
                        <th title="IPD release mode">IPD_RLS_MODE</th>
                        <th title="IPD advance angle">IPD_ADV_ANGLE</th>
                        <th title="Number of times IPD is executed">IPD_REPEAT</th>
                        <th title="Open loop current limit configuration">OL_ILIMIT_CONFIG</th>
                        <th title="Iq ramp down before transition to close loop">IQ_RAMP_EN</th>
                        <th title="Active braking enable">ACTIVE_BRAKE_EN</th>
                        <th title="Chooses between forward and reverse drive setting for reverse drive">REV_DRV_CONFIG</th>
                        <th>Read / Write</th>
                    </tr>

                    <tr>
                        <td><input type="text" id="MTR_STARTUP" readonly></td>
                        <td><input type="text" id="ALIGN_SLOW_RAMP_RATE" readonly></td>
                        <td><input type="text" id="ALIGN_TIME" readonly></td>
                        <td><input type="text" id="ALIGN_OR_SLOW_CURRENT_ILIMIT" readonly></td>
                        <td><input type="text" id="IPD_CLK_FREQ" readonly></td>
                        <td><input type="text" id="IPD_CURR_THR" readonly></td>
                        <td><input type="text" id="IPD_RLS_MODE" readonly></td>
                        <td><input type="text" id="IPD_ADV_ANGLE" readonly></td>
                        <td><input type="text" id="IPD_REPEAT" readonly></td>
                        <td><input type="text" id="OL_ILIMIT_CONFIG" readonly></td>
                        <td><input type="text" id="IQ_RAMP_EN" readonly></td>
                        <td><input type="text" id="ACTIVE_BRAKE_EN" readonly></td>
                        <td><input type="text" id="REV_DRV_CONFIG" readonly></td>
                        <td><button onclick="ReadMotorStartup1()">Read</button></td>
                    </tr>
                    <tr>
                        <td>
                            <select id="MTR_STARTUP_SELECT">
                                <option value="0">0 = Align</option>
                                <option value="1">1 = Double Align</option>
                                <option value="2">2 = IPD</option>
                                <option value="3">3 = Slow first cycle</option>
                            </select>
                        </td>
                        <td>
                            <select id="ALIGN_SLOW_RAMP_RATE_SELECT">
                                <option value="0">0 = 0.1 A/s</option>
                                <option value="1">1 = 1 A/s</option>
                                <option value="2">2 = 5 A/s</option>
                                <option value="3">3 = 10 A/s</option>
                                <option value="4">4 = 15 A/s</option>
                                <option value="5">5 = 25 A/s</option>
                                <option value="6">6 = 50 A/s</option>
                                <option value="7">7 = 100 A/s</option>
                                <option value="8">8 = 150 A/s</option>
                                <option value="9">9 = 200 A/s</option>
                                <option value="A">A = 250 A/s</option>
                                <option value="B">B = 500 A/s</option>
                                <option value="C">C = 1000 A/s</option>
                                <option value="D">D = 2000 A/s</option>
                                <option value="E">E = 5000 A/s</option>
                                <option value="F">F = No Limit</option>
                            </select>
                        </td>
                        <td>
                            <select id="ALIGN_TIME_SELECT">
                                <option value="0">0 = 10 ms</option>
                                <option value="1">1 = 50 ms</option>
                                <option value="2">2 = 100 ms</option>
                                <option value="3">3 = 200 ms</option>
                                <option value="4">4 = 300 ms</option>
                                <option value="5">5 = 400 ms</option>
                                <option value="6">6 = 500 ms</option>
                                <option value="7">7 = 750 ms</option>
                                <option value="8">8 = 1 s</option>
                                <option value="9">9 = 1.5 s</option>
                                <option value="A">A = 2 s</option>
                                <option value="B">B = 3 s</option>
                                <option value="C">C = 4 s</option>
                                <option value="D">D = 5 s</option>
                                <option value="E">E = 7.5 s</option>
                                <option value="F">F = 10 s</option>
                            </select>
                        </td>
                        <td>
                            <select id="ALIGN_OR_SLOW_CURRENT_ILIMIT_SELECT">
                                <option value="0">0 = 0.125 A</option>
                                <option value="1">1 = 0.25 A</option>
                                <option value="2">2 = 0.5 A</option>
                                <option value="3">3 = 1.0 A</option>
                                <option value="4">4 = 1.5 A</option>
                                <option value="5">5 = 2.0 A</option>
                                <option value="6">6 = 2.5 A</option>
                                <option value="7">7 = 3.0 A</option>
                                <option value="8">8 = 3.5 A</option>
                                <option value="9">9 = 4.0 A</option>
                                <option value="A">A = 4.5 A</option>
                                <option value="B">B = 5.0 A</option>
                                <option value="C">C = 5.5 A</option>
                                <option value="D">D = 6.0 A</option>
                                <option value="E">E = 7.0 A</option>
                                <option value="F">F = 8.0 A</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IPD_CLK_FREQ_SELECT">
                                <option value="0">0 = 50 Hz</option>
                                <option value="1">1 = 100 Hz</option>
                                <option value="2">2 = 250 Hz</option>
                                <option value="3">3 = 500 Hz</option>
                                <option value="4">4 = 1000 Hz</option>
                                <option value="5">5 = 2000 Hz</option>
                                <option value="6">6 = 5000 Hz</option>
                                <option value="7">7 = 10000 Hz</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IPD_CURR_THR_SELECT">
                                <option value="0">0 = 0.25 A</option>
                                <option value="1">1 = 0.5 A</option>
                                <option value="2">2 = 0.75 A</option>
                                <option value="3">3 = 1.0 A</option>
                                <option value="4">4 = 1.25 A</option>
                                <option value="5">5 = 1.5 A</option>
                                <option value="6">6 = 2.0 A</option>
                                <option value="7">7 = 2.5 A</option>
                                <option value="8">8 = 3.0 A</option>
                                <option value="9">9 = 3.667 A</option>
                                <option value="A">A = 4.0 A</option>
                                <option value="B">B = 4.667 A</option>
                                <option value="C">C = 5.0 A</option>
                                <option value="D">D = 5.333 A</option>
                                <option value="E">E = 6.0 A</option>
                                <option value="F">F = 6.667 A</option>
                                <option value="10">10 = 7.333 A</option>
                                <option value="11">11 = 8.0 A</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IPD_RLS_MODE_SELECT">
                                <option value="0">0 = Brake</option>
                                <option value="1">1 = Tristate</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IPD_ADV_ANGLE_SELECT">
                                <option value="0">0 = 0°</option>
                                <option value="1">1 = 30°</option>
                                <option value="2">2 = 60°</option>
                                <option value="3">3 = 90°</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IPD_REPEAT_SELECT">
                                <option value="0">0 = 1 time</option>
                                <option value="1">1 = average of 2 times</option>
                                <option value="2">2 = average of 3 times</option>
                                <option value="3">3 = average of 4 times</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="OL_ILIMIT_CONFIG_SELECT">
                                <option value="0">0 = Open loop current limit defined by OL_ILIMIT</option>
                                <option value="1">1 = Open loop current limit defined by ILIMIT</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="IQ_RAMP_EN_SELECT">
                                <option value="0">0 = Disable Iq ramp down</option>
                                <option value="1">1 = Enable Iq ramp down</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="ACTIVE_BRAKE_EN_SELECT">
                                <option value="0">0 = Disable Active Brake</option>
                                <option value="1">1 = Enable Active Brake</option>
                            </select>
                        </td>
                        
                        <td>
                            <select id="REV_DRV_CONFIG_SELECT">
                                <option value="0">0 = Open loop current, A1, A2 based on forward drive</option>
                                <option value="1">1 = Open loop current, A1, A2 based on reverse drive</option>
                            </select>
                        </td>
                        
                        <td><button onclick="WriteMotorStartup1()">Write</button></td>

                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="motorStartup2Container">
        <!-- Tıklanabilir Başlık -->
        <div class="header" onclick="toggleContainer('motorStartup2Container', 'motorStartup2Content', 'motorStartup2Arrow')">
            MOTOR_STARTUP2
            <span class="arrow" id="motorStartup2Arrow">▼</span>
        </div>

        <!-- İçerik -->
        <div class="content" id="motorStartup2Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Open loop current limit">OL_ILIMIT</th>
                        <th title="Open loop acceleration coefficient A1">OL_ACC_A1</th>
                        <th title="Open loop acceleration coefficient A2">OL_ACC_A2</th>
                        <th title="Auto handoff enable">AUTO_HANDOFF_EN</th>
                        <th title="Open to close loop handoff threshold">OPN_CL_HANDOFF_THR</th>
                        <th title="Align angle">ALIGN_ANGLE</th>
                        <th title="Frequency of first cycle in closed loop startup">SLOW_FIRST_CYC_FREQ</th>
                        <th title="First cycle frequency in open loop for align, double align and IPD startup options">FIRST_CYCLE_FREQ_SEL</th>
                        <th title="Ramp rate for reducing difference between estimated theta and open loop theta">THETA_ERROR_RAMP_RATE</th>
                        <th>Read / Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="OL_ILIMIT" readonly></td>
                        <td><input type="text" id="OL_ACC_A1" readonly></td>
                        <td><input type="text" id="OL_ACC_A2" readonly></td>
                        <td><input type="text" id="AUTO_HANDOFF_EN" readonly></td>
                        <td><input type="text" id="OPN_CL_HANDOFF_THR" readonly></td>
                        <td><input type="text" id="ALIGN_ANGLE" readonly></td>
                        <td><input type="text" id="SLOW_FIRST_CYC_FREQ" readonly></td>
                        <td><input type="text" id="FIRST_CYCLE_FREQ_SEL" readonly></td>
                        <td><input type="text" id="THETA_ERROR_RAMP_RATE" readonly></td>
                        <td><button onclick="ReadMotorStartup2()">Read</button></td>
                    </tr>
                    <tr>
                        <td>
                            <select id="OL_ILIMIT_SELECT">
                                <option value="0">0 = 0.125 A</option>
                                <option value="1">1 = 0.25 A</option>
                                <option value="2">2 = 0.5 A</option>
                                <option value="3">3 = 1.0 A</option>
                                <option value="4">4 = 1.5 A</option>
                                <option value="5">5 = 2.0 A</option>
                                <option value="6">6 = 2.5 A</option>
                                <option value="7">7 = 3.0 A</option>
                                <option value="8">8 = 3.5 A</option>
                                <option value="9">9 = 4.0 A</option>
                                <option value="A">A = 4.5 A</option> 
                                <option value="B">B = 5.0 A</option>
                                <option value="C">C = 5.5 A</option>
                                <option value="D">D = 6.0 A</option>
                                <option value="E">E = 7.0 A</option>
                                <option value="F">F = 8.0 A</option>
                            </select>
                        </td>

                        <td>
                            <select id="OL_ACC_A1_SELECT">
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
                            <select id="OL_ACC_A2_SELECT">
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
                            <select id="AUTO_HANDOFF_EN_SELECT">
                                <option value="0">0 = Disable Auto Handoff (and use OPN_CL_HANDOFF_THR)</option>
                                <option value="1">1 = Enable Auto Handoff</option>
                            </select>
                        </td>

                        <td>
                            <select id="OPN_CL_HANDOFF_THR_SELECT">
                                <option value="0">0 = 1%</option>
                                <option value="1">1 = 2%</option>
                                <option value="2">2 = 3%</option>
                                <option value="3">3 = 4%</option>
                                <option value="4">4 = 5%</option>
                                <option value="5">5 = 6%</option>
                                <option value="6">6 = 7%</option>
                                <option value="7">7 = 8%</option>
                                <option value="8">8 = 9%</option>
                                <option value="9">9 = 10%</option>
                                <option value="A">A = 11%</option>
                                <option value="B">B = 12%</option>
                                <option value="C">C = 13%</option>
                                <option value="D">D = 14%</option>
                                <option value="E">E = 15%</option>
                                <option value="F">F = 16%</option>
                                <option value="10">10 = 17%</option>
                                <option value="11">11 = 18%</option>
                                <option value="12">12 = 19%</option>
                                <option value="13">13 = 20%</option>
                                <option value="14">14 = 22.5%</option>
                                <option value="15">15 = 25%</option>
                                <option value="16">16 = 27.5%</option>
                                <option value="17">17 = 30%</option>
                                <option value="18">18 = 32.5%</option>
                                <option value="19">19 = 35%</option>
                                <option value="1A">1A = 37.5%</option>
                                <option value="1B">1B = 40%</option>
                                <option value="1C">1C = 42.5%</option>
                                <option value="1D">1D = 45%</option>
                                <option value="1E">1E = 47.5%</option>
                                <option value="1F">1F = 50%</option>
                            </select>
                        </td>

                        <td>
                            <select id="ALIGN_ANGLE_SELECT">
                                <option value="0">0 = 0°</option>
                                <option value="1">1 = 10°</option>
                                <option value="2">2 = 20°</option>
                                <option value="3">3 = 30°</option>
                                <option value="4">4 = 45°</option>
                                <option value="5">5 = 60°</option>
                                <option value="6">6 = 70°</option>
                                <option value="7">7 = 80°</option>
                                <option value="8">8 = 90°</option>
                                <option value="9">9 = 110°</option>
                                <option value="A">A = 120°</option>
                                <option value="B">B = 135°</option>
                                <option value="C">C = 150°</option>
                                <option value="D">D = 160°</option>
                                <option value="E">E = 170°</option>
                                <option value="F">F = 180°</option>
                                <option value="10">10 = 190°</option>
                                <option value="11">11 = 210°</option>
                                <option value="12">12 = 225°</option>
                                <option value="13">13 = 240°</option>
                                <option value="14">14 = 250°</option>
                                <option value="15">15 = 260°</option>
                                <option value="16">16 = 270°</option>
                                <option value="17">17 = 280°</option>
                                <option value="18">18 = 290°</option>
                                <option value="19">19 = 315°</option>
                                <option value="1A">1A = 330°</option>
                                <option value="1B">1B = 340°</option>
                                <option value="1C">1C = 350°</option>
                            </select>
                        </td>

                        <td>
                            <select id="SLOW_FIRST_CYC_FREQ_SELECT">
                                <option value="0">0 = 1%</option>
                                <option value="1">1 = 2%</option>
                                <option value="2">2 = 3%</option>
                                <option value="3">3 = 5%</option>
                                <option value="4">4 = 7.5%</option>
                                <option value="5">5 = 10%</option>
                                <option value="6">6 = 12.5%</option>
                                <option value="7">7 = 15%</option>
                                <option value="8">8 = 17.5%</option>
                                <option value="9">9 = 20%</option>
                                <option value="A">A = 25%</option>
                                <option value="B">B = 30%</option>
                                <option value="C">C = 35%</option>
                                <option value="D">D = 40%</option>
                                <option value="E">E = 45%</option>
                                <option value="F">F = 50%</option>
                            </select>
                        </td>

                        <td>
                            <select id="FIRST_CYCLE_FREQ_SEL_SELECT">
                                <option value="0">0 = Defined by SLOW_FIRST_CYC_FREQ</option>
                                <option value="1">1 = 0 Hz</option>
                            </select>
                        </td>

                        <td>
                            <select id="THETA_ERROR_RAMP_RATE_SELECT">
                                <option value="0">0 = 0.01 deg/ms</option>
                                <option value="1">1 = 0.05 deg/ms</option>
                                <option value="2">2 = 0.1 deg/ms</option>
                                <option value="3">3 = 0.15 deg/ms</option>
                                <option value="4">4 = 0.2 deg/ms</option>
                                <option value="5">5 = 0.5 deg/ms</option>
                                <option value="6">6 = 1 deg/ms</option>
                                <option value="7">7 = 2 deg/ms</option>
                            </select>
                        </td>
                        
                        <td><button onclick="WriteMotorStartup2()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="closedLoop1Container">
        <!-- Tıklanabilir Başlık -->
        <div class="header" onclick="toggleContainer('closedLoop1Container', 'closedLoop1Content', 'closedLoop1Arrow')">
            CLOSED_LOOP1
            <span class="arrow" id="closedLoop1Arrow">▼</span>
        </div>

        <div class="content" id="closedLoop1Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Overmodulation enable">OVERMODULATION_ENABLE</th>
                        <th title="Closed loop acceleration">CL_ACC</th>
                        <th title="Closed loop deceleration configuration">CL_DEC_CONFIG</th>
                        <th title="Closed loop deceleration. This register is used only if AVS is disabled and CL_DEC_CONFIG is set to '0'">CL_DEC</th>
                        <th title="Output PWM switching frequency">PWM_FREQ_OUT</th>
                        <th title="PWM modulation">PWM_MODE</th>
                        <th title="FG select">FG_SEL</th>
                        <th title="FG division factor">FG_DIV</th>
                        <th title="FG output configuration">FG_CONFIG</th>
                        <th title="FG output BEMF threshold">FG_BEMF_THR</th>
                        <th title="AVS enable">AVS_EN</th>
                        <th title="Deadtime compensation enable">DEADTIME_COMP_EN</th>
                        <th title="Speed loop disable">SPEED_LOOP_DIS</th>
                        <th title="Stop mode applied when stop mode is recirculation brake and motor running in align or open loop">LOW_SPEED_RECIRC_BRAKE_EN</th>
                        <th>Read / Write</th>
                    </tr>

                    <tr>
                        <td><input type="text" id="OVERMODULATION_ENABLE" readonly></td>
                        <td><input type="text" id="CL_ACC" readonly></td>
                        <td><input type="text" id="CL_DEC_CONFIG" readonly></td>
                        <td><input type="text" id="CL_DEC" readonly></td>
                        <td><input type="text" id="PWM_FREQ_OUT" readonly></td>
                        <td><input type="text" id="PWM_MODE" readonly></td>
                        <td><input type="text" id="FG_SEL" readonly></td>
                        <td><input type="text" id="FG_DIV" readonly></td>
                        <td><input type="text" id="FG_CONFIG" readonly></td>
                        <td><input type="text" id="FG_BEMF_THR" readonly></td>
                        <td><input type="text" id="AVS_EN" readonly></td>
                        <td><input type="text" id="DEADTIME_COMP_EN" readonly></td>
                        <td><input type="text" id="SPEED_LOOP_DIS" readonly></td>
                        <td><input type="text" id="LOW_SPEED_RECIRC_BRAKE_EN" readonly></td>
                        <td><button onclick="ReadClosedLoop1()">Read</button></td>
                    </tr>

                    <tr>

                    <td>
                        <select id="OVERMODULATION_ENABLE_SELECT">
                            <option value="0">0 = Disable Overmodulation</option>
                            <option value="1">1 = Enable Overmodulation</option>
                        </select>
                    </td>

                    <td>
                        <select id="CL_ACC_SELECT">
                            <option value="0">0 = 0.5 Hz/s</option>
                            <option value="1">1 = 1 Hz/s</option>
                            <option value="2">2 = 2.5 Hz/s</option>
                            <option value="3">3 = 5 Hz/s</option>
                            <option value="4">4 = 7.5 Hz/s</option>
                            <option value="5">5 = 10 Hz/s</option>
                            <option value="6">6 = 20 Hz/s</option>
                            <option value="7">7 = 40 Hz/s</option>
                            <option value="8">8 = 60 Hz/s</option>
                            <option value="9">9 = 80 Hz/s</option>
                            <option value="A">A = 100 Hz/s</option>
                            <option value="B">B = 200 Hz/s</option>
                            <option value="C">C = 300 Hz/s</option>
                            <option value="D">D = 400 Hz/s</option>
                            <option value="E">E = 500 Hz/s</option>
                            <option value="F">F = 600 Hz/s</option>
                            <option value="10">10 = 700 Hz/s</option>
                            <option value="11">11 = 800 Hz/s</option>
                            <option value="12">12 = 900 Hz/s</option>
                            <option value="13">13 = 1000 Hz/s</option>
                            <option value="14">14 = 2000 Hz/s</option>
                            <option value="15">15 = 4000 Hz/s</option>
                            <option value="16">16 = 6000 Hz/s</option>
                            <option value="17">17 = 8000 Hz/s</option>
                            <option value="18">18 = 10000 Hz/s</option>
                            <option value="19">19 = 20000 Hz/s</option>
                            <option value="1A">1A = 30000 Hz/s</option>
                            <option value="1B">1B = 40000 Hz/s</option>
                            <option value="1C">1C = 50000 Hz/s</option>
                            <option value="1D">1D = 60000 Hz/s</option>
                            <option value="1E">1E = 70000 Hz/s</option>
                            <option value="1F">1F = No limit</option>
                        </select>
                    </td>

                    <td>
                        <select id="CL_DEC_CONFIG_SELECT">
                            <option value="0">0 = Defined by CL_DEC</option>
                            <option value="1">1 = Defined by CL_ACC</option>
                        </select>
                    </td>

                    <td>
                        <select id="CL_DEC_SELECT">
                            <option value="0">0 = 0.5 Hz/s</option>
                            <option value="1">1 = 1 Hz/s</option>
                            <option value="2">2 = 2.5 Hz/s</option>
                            <option value="3">3 = 5 Hz/s</option>
                            <option value="4">4 = 7.5 Hz/s</option>
                            <option value="5">5 = 10 Hz/s</option>
                            <option value="6">6 = 20 Hz/s</option>
                            <option value="7">7 = 40 Hz/s</option>
                            <option value="8">8 = 60 Hz/s</option>
                            <option value="9">9 = 80 Hz/s</option>
                            <option value="A">A = 100 Hz/s</option>
                            <option value="B">B = 200 Hz/s</option>
                            <option value="C">C = 300 Hz/s</option>
                            <option value="D">D = 400 Hz/s</option>
                            <option value="E">E = 500 Hz/s</option>
                            <option value="F">F = 600 Hz/s</option>
                            <option value="10">10 = 700 Hz/s</option>
                            <option value="11">11 = 800 Hz/s</option>
                            <option value="12">12 = 900 Hz/s</option>
                            <option value="13">13 = 1000 Hz/s</option>
                            <option value="14">14 = 2000 Hz/s</option>
                            <option value="15">15 = 4000 Hz/s</option>
                            <option value="16">16 = 6000 Hz/s</option>
                            <option value="17">17 = 8000 Hz/s</option>
                            <option value="18">18 = 10000 Hz/s</option>
                            <option value="19">19 = 20000 Hz/s</option>
                            <option value="1A">1A = 30000 Hz/s</option>
                            <option value="1B">1B = 40000 Hz/s</option>
                            <option value="1C">1C = 50000 Hz/s</option>
                            <option value="1D">1D = 60000 Hz/s</option>
                            <option value="1E">1E = 70000 Hz/s</option>
                            <option value="1F">1F = No limit</option>
                        </select>
                    </td>

                    <td>
                        <select id="PWM_FREQ_OUT_SELECT">
                            <option value="0">0 = 10 kHz</option>
                            <option value="1">1 = 15 kHz</option>
                            <option value="2">2 = 20 kHz</option>
                            <option value="3">3 = 25 kHz</option>
                            <option value="4">4 = 30 kHz</option>
                            <option value="5">5 = 35 kHz</option>
                            <option value="6">6 = 40 kHz</option>
                            <option value="7">7 = 45 kHz</option>
                            <option value="8">8 = 50 kHz</option>
                            <option value="9">9 = 55 kHz</option>
                            <option value="A">A = 60 kHz</option>
                            <option value="B">B = 65 kHz</option>
                            <option value="C">C = 70 kHz</option>
                            <option value="D">D = 75 kHz</option>
                        </select>
                    </td>

                    <td>
                        <select id="PWM_MODE_SELECT">
                            <option value="0">0 = Continuous Space Vector Modulation</option>
                            <option value="1">1 = Discontinuous Space Vector Modulation</option>
                        </select>
                    </td>
                    
                    <td>
                        <select id="FG_SEL_SELECT">
                            <option value="0">0 = Output FG in open loop and closed loop</option>
                            <option value="1">1 = Output FG in only closed loop</option>
                            <option value="2">2 = Output FG in open loop for the first try</option>
                            <option value="3">3 = N/A</option>
                        </select>
                    </td>

                    <td>
                        <select id="FG_DIV_SELECT">
                            <option value="0">0 = Divide by 1 (2-pole motor mechanical speed)</option>
                            <option value="1">1 = Divide by 1 (2-pole motor mechanical speed)</option>
                            <option value="2">2 = Divide by 2 (4-pole motor mechanical speed)</option>
                            <option value="3">3 = Divide by 3 (6-pole motor mechanical speed)</option>
                            <option value="4">4 = Divide by 4 (8-pole motor mechanical speed)</option>
                            <option value="F">F = Divide by 15 (30-pole motor mechanical speed)</option>
                        </select>
                    </td>

                    <td>
                        <select id="FG_CONFIG_SELECT">
                            <option value="0">0 = FG active as long as motor is driven</option>
                            <option value="1">1 = FG active till BEMF drops below BEMF threshold defined by FG_BEMF_THR</option>
                        </select>
                    </td>

                    <td>
                        <select id="FG_BEMF_THR_SELECT">
                            <option value="0">0 = ±1 mV</option>
                            <option value="1">1 = ±2 mV</option>
                            <option value="2">2 = ±5 mV</option>
                            <option value="3">3 = ±10 mV</option>
                            <option value="4">4 = ±20 mV</option>
                            <option value="5">5 = ±30 mV</option>
                        </select>
                    </td>

                    <td>
                        <select id="AVS_EN_SELECT">
                            <option value="0">0 = AVS Disabled</option>
                            <option value="1">1 = AVS Enabled</option>
                        </select>
                    </td>

                    <td>
                        <select id="DEADTIME_COMP_EN_SELECT">
                            <option value="0">0 = Deadtime compensation disabled</option>
                            <option value="1">1 = Deadtime compensation enabled</option>
                        </select>
                    </td>

                    <td>
                        <select id="SPEED_LOOP_DIS_SELECT">
                            <option value="0">0 = Speed loop enabled</option>
                            <option value="1">1 = Speed loop disabled</option>
                        </select>
                    </td>

                    <td>
                        <select id="LOW_SPEED_RECIRC_BRAKE_EN_SELECT">
                            <option value="0">0 = Hi-Z</option>
                            <option value="1">1 = Low Side Brake</option>
                        </select>
                    </td>

                    <td><button onclick="WriteClosedLoop1()">Write</button></td>

                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="closedLoop2Container">
        <div class="header" onclick="toggleContainer('closedLoop2Container', 'closedLoop2Content', 'closedLoop2Arrow')">
            CLOSED_LOOP2
            <span class="arrow" id="closedLoop2Arrow">▼</span>
        </div>

        <div class="content" id="closedLoop2Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Motor stop method">MTR_STOP</th>
                        <th title="Brake time during motor stop">MTR_STOP_BRK_TIME</th>
                        <th title="Speed threshold for active spin down (% of MAX_SPEED)">ACT_SPIN_THR</th>
                        <th title="Speed threshold for BRAKE pin and motor stop options (low-side braking or high-side braking or align braking) (% of MAX_SPEED)">BRAKE_SPEED_THRESHOLD</th>
                        <th title="8-bit values for motor phase resistance">MOTOR_RES</th>
                        <th title="8-bit values for motor phase inductance">MOTOR_IND</th>
                        <th>Read / Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="MTR_STOP" readonly></td>
                        <td><input type="text" id="MTR_STOP_BRK_TIME" readonly></td>
                        <td><input type="text" id="ACT_SPIN_THR" readonly></td>
                        <td><input type="text" id="BRAKE_SPEED_THRESHOLD" readonly></td>
                        <td><input type="text" id="MOTOR_RES" readonly></td>
                        <td><input type="text" id="MOTOR_IND" readonly></td>
                        <td><button onclick="ReadClosedLoop2()">Read</button></td>
                    </tr>
                    <tr>
                        <td>
                            <select id="MTR_STOP_SELECT">
                                <option value="0">0 = Hi-Z</option>
                                <option value="1">1 = Recirculation Mode</option>
                                <option value="2">2 = Low Side Braking</option>
                                <option value="3">3 = High Side Braking</option>
                                <option value="4">4 = Active Spin Down</option>
                                <option value="5">5 = Align Braking</option>
                            </select>
                        </td>
                        <td>
                            <select id="MTR_STOP_BRK_TIME_SELECT">
                                <option value="0">0 = 0.1 ms</option>
                                <option value="1">1 = 0.1 ms</option>
                                <option value="2">2 = 0.25 ms</option>
                                <option value="3">3 = 0.5 ms</option>
                                <option value="4">4 = 1 ms</option>
                                <option value="5">5 = 5 ms</option>
                                <option value="6">6 = 10 ms</option>
                                <option value="7">7 = 50 ms</option>
                                <option value="8">8 = 100 ms</option>
                                <option value="9">9 = 250 ms</option>
                                <option value="A">A = 500 ms</option>
                                <option value="B">B = 1000 ms</option>
                                <option value="C">C = 2500 ms</option>
                                <option value="D">D = 5000 ms</option>
                                <option value="E">E = 10000 ms</option>
                                <option value="F">F = 15000 ms</option>
                            </select>
                        </td>
                        <td>
                            <select id="ACT_SPIN_THR_SELECT">
                                <option value="0">0 = 100%</option>
                                <option value="1">1 = 90%</option>
                                <option value="2">2 = 80%</option>
                                <option value="3">3 = 70%</option>
                                <option value="4">4 = 60%</option>
                                <option value="5">5 = 50%</option>
                                <option value="6">6 = 45%</option>
                                <option value="7">7 = 40%</option>
                                <option value="8">8 = 35%</option>
                                <option value="9">9 = 30%</option>
                                <option value="A">A = 25%</option>
                                <option value="B">B = 20%</option>
                                <option value="C">C = 15%</option>
                                <option value="D">D = 10%</option>
                                <option value="E">E = 5%</option>
                                <option value="F">F = 2.5%</option>
                            </select>
                        </td>
                        <td>
                            <select id="BRAKE_SPEED_THRESHOLD_SELECT">
                                <option value="0">0 = 100%</option>
                                <option value="1">1 = 90%</option>
                                <option value="2">2 = 80%</option>
                                <option value="3">3 = 70%</option>
                                <option value="4">4 = 60%</option>
                                <option value="5">5 = 50%</option>
                                <option value="6">6 = 45%</option>
                                <option value="7">7 = 40%</option>
                                <option value="8">8 = 35%</option>
                                <option value="9">9 = 30%</option>
                                <option value="A">A = 25%</option>
                                <option value="B">B = 20%</option>
                                <option value="C">C = 15%</option>
                                <option value="D">D = 10%</option>
                                <option value="E">E = 5%</option>
                                <option value="F">F = 2.5%</option>
                            </select>
                        </td>
                        <td><input type="text" id="MOTOR_RES_INPUT" value="0"></td>
                        <td><input type="text" id="MOTOR_IND_INPUT" value="0"></td>
                        <td><button onclick="WriteClosedLoop2()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>


<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="closedLoop3Container">
        <div class="header" onclick="toggleContainer('closedLoop3Container', 'closedLoop3Content', 'closedLoop3Arrow')">
            CLOSED_LOOP3
            <span class="arrow" id="closedLoop3Arrow">▼</span>
        </div>

        <div class="content" id="closedLoop3Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="8-bit values for motor BEMF constant">MOTOR_BEMF_CONST</th>
                        <th title="10-bit value for current Iq and Id loop Kp. Kp = 8LSB of CURR_LOOP_KP / 10^2MSB of CURR_LOOP_KP. Set to 0 for auto calculation of current loop Kp.">CURR_LOOP_KP</th>
                        <th title="10-bit value for current Iq and Id loop Ki. Ki = 1000 * 8LSB of CURR_LOOP_KI / 10^2MSB of CURR_LOOP_KI. Set to 0 for auto calculation of current loop Ki.">CURR_LOOP_KI</th>
                        <th title="3 MSB bits for speed loop Kp. Kp = 0.01 * 8LSB of SPD_LOOP_KP / 10^2MSB of SPD_LOOP_KP">SPD_LOOP_KP</th>
                        <th>Read / Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="MOTOR_BEMF_CONST" value="0"></td>
                        <td><input type="text" id="CURR_LOOP_KP" value="0"></td>
                        <td><input type="text" id="CURR_LOOP_KI" value="0"></td>
                        <td><input type="text" id="SPD_LOOP_KP_CL3" value="0"></td>
                        <td><button onclick="ReadClosedLoop3()">Read</button></td>
                    </tr>
                    <tr>
                        <td><input type="text" id="MOTOR_BEMF_CONST_INPUT" value="0"></td>
                        <td><input type="text" id="CURR_LOOP_KP_INPUT" value="0"></td>
                        <td><input type="text" id="CURR_LOOP_KI_INPUT" value="0"></td>
                        <td><input type="text" id="SPD_LOOP_KP_CL3_INPUT" value="0"></td>
                        <td><button onclick="WriteClosedLoop3()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="closedLoop4Container">
        <div class="header" onclick="toggleContainer('closedLoop4Container', 'closedLoop4Content', 'closedLoop4Arrow')">
            CLOSED_LOOP4
            <span class="arrow" id="closedLoop4Arrow">▼</span>
        </div>

        <div class="content" id="closedLoop4Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="7 LSB bits for speed loop Kp. Kp = 0.01 * 8LSB of SPD_LOOP_KP / 10^2MSB of SPD_LOOP_KP. Set to 0 for auto calculation of speed loop Kp.">SPD_LOOP_KP</th>
                        <th title="10-bit value for speed loop Ki. Ki = 0.1 * 8LSB of SPD_LOOP_KI / 10^2MSB of SPD_LOOP_KI. Set to 0 for auto calculation of speed loop Ki">SPD_LOOP_KI</th>
                        <th title="14-bit value for setting maximum value of speed in electrical Hz Maximum motor electrical speed (Hz): {MOTOR_SPEED/6} For example: if MOTOR_SPEED is 0x2710, then maximum motor speed (Hz) = 10000(0x2710)/6 = 1666 Hz">MAX_SPEED</th>
                        <th>Read / Write</th>
                    </tr>
                    <tr>
                        <td><input type="text" id="SPD_LOOP_KP_CL4" value="0"></td>
                        <td><input type="text" id="SPD_LOOP_KI" value="0"></td>
                        <td><input type="text" id="MAX_SPEED" value="0"></td>
                        <td><button onclick="ReadClosedLoop4()">Read</button></td>
                    </tr>
                    <tr>
                        <td><input type="text" id="SPD_LOOP_KP_CL4_INPUT" value="0"></td>
                        <td><input type="text" id="SPD_LOOP_KI_INPUT" value="0"></td>
                        <td><input type="text" id="MAX_SPEED_INPUT" value="0"></td>
                        <td><button onclick="WriteClosedLoop4()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>


<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="pinConfigContainer">
        <div class="header" onclick="toggleContainer('pinConfigContainer', 'pinConfigContent', 'pinConfigArrow')">
            PIN_CONFIG
            <span class="arrow" id="pinConfigArrow">▼</span>
        </div>

        <div class="content" id="pinConfigContent" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Brake pin mode">BRAKE_PIN_MODE</th>
                        <th title="Align brake angle select">ALIGN_BRAKE_ANGLE_SEL</th>
                        <th title="Brake pin override">BRAKE_INPUT</th>
                        <th title="Configure speed control mode from speed pin">SPEED_MODE</th>
                        <th>Read/Write</th>
                    </tr>

                    <tr>
                        <td><input type="text" id="BRAKE_PIN_MODE" readonly></td>
                        <td><input type="text" id="ALIGN_BRAKE_ANGLE_SEL" readonly></td>
                        <td><input type="text" id="BRAKE_INPUT" readonly></td>
                        <td><input type="text" id="SPEED_MODE" readonly></td>
                        <td><button onclick="ReadPinConfig()">Read</button></td>
                    </tr>

                    <tr>
                        <td>
                            <select id="BRAKE_PIN_MODE_SELECT">
                                <option value="0">0 = Low side Brake</option>
                                <option value="1">1 = Align Brake</option>
                            </select>
                        </td>
                        <td>
                            <select id="ALIGN_BRAKE_ANGLE_SEL_SELECT">
                                <option value="0">0 = Use last commutation angle before entering align braking</option>
                                <option value="1">1 = Use ALIGN_ANGLE configuration for align braking</option>
                            </select>
                        </td>
                        <td>
                            <select id="BRAKE_INPUT_SELECT">
                                <option value="0">0 = Hardware Pin BRAKE</option>
                                <option value="1">1 = Override pin and brake/align according to BRAKE_PIN_MODE</option>
                                <option value="2">2 = Override pin and do not brake/align</option>
                                <option value="3">3 = Hardware Pin BRAKE</option>
                            </select>
                        </td>
                        <td>
                            <select id="SPEED_MODE_SELECT">
                                <option value="0">0 = Analog Mode</option>
                                <option value="1">1 = Controlled by Duty Cycle of SPEED Input Pin</option>
                                <option value="2">2 = Register Override mode</option>
                                <option value="3">3 = Controlled by Frequency of SPEED Input Pin</option>
                            </select>
                        </td>
                        <td><button onclick="WritePinConfig()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="algoCtrl1Container">
        <div class="header" onclick="toggleContainer('algoCtrl1Container', 'algoCtrl1Content', 'algoCtrl1Arrow')">
            ALGO_CTRL1
            <span class="arrow" id="algoCtrl1Arrow">▼</span>
        </div>

        <div class="content" id="algoCtrl1Content" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Use to control the SPD_CTRL bits. If OVERRIDE = 1b, speed command can be written by the user through serial interface.">OVERRIDE</th>
                        <th title="Digital speed control If OVERRIDE = 1b, then SPEED_CMD is control using DIGITAL_SPEED_CTRL">DIGITAL_SPEED_CTRL</th>
                        <th title="Use to disable closed loop">CLOSED_LOOP_DIS</th>
                        <th title="Force align state enable">FORCE_ALIGN_EN</th>
                        <th title="Force slow first cycle enable">FORCE_SLOW_FIRST_CYCLE_EN</th>
                        <th title="Force IPD enable">FORCE_IPD_EN</th>
                        <th title="Force ISD enable">FORCE_ISD_EN</th>
                        <th title="Force align angle state source select">FORCE_ALIGN_ANGLE_SRC_SEL</th>
                        <th title="Sets Iq_ref when speed loop is disabled">FORCE_IQ_REF_SPEED_LOOP_DIS</th>
                        <th>Read/Write</th>
                    </tr>

                    <tr>
                        <td><input type="text" id="OVERRIDE" readonly></td>
                        <td><input type="text" id="DIGITAL_SPEED_CTRL" readonly></td>
                        <td><input type="text" id="CLOSED_LOOP_DIS" readonly></td>
                        <td><input type="text" id="FORCE_ALIGN_EN" readonly></td>
                        <td><input type="text" id="FORCE_SLOW_FIRST_CYCLE_EN" readonly></td>
                        <td><input type="text" id="FORCE_IPD_EN" readonly></td>
                        <td><input type="text" id="FORCE_ISD_EN" readonly></td>
                        <td><input type="text" id="FORCE_ALIGN_ANGLE_SRC_SEL" readonly></td>
                        <td><input type="text" id="FORCE_IQ_REF_SPEED_LOOP_DIS" readonly></td>
                        <td><button onclick="ReadAlgoCtrl1()">Read</button></td>
                    </tr>

                    <tr>
                        <td>
                            <select id="OVERRIDE_SELECT">
                                <option value="0">0 = SPEED_CMD using Analog/PWM/Freq mode</option>
                                <option value="1">1 = SPEED_CMD using SPD_CTRL[14:0]</option>
                            </select>
                        </td>
                        <td><input type="text" id="DIGITAL_SPEED_CTRL_SELECT" value="0"></td>
                        <td>
                            <select id="CLOSED_LOOP_DIS_SELECT">
                                <option value="0">0 = Enable Closed Loop</option>
                                <option value="1">1 = Disable Closed Loop</option>
                            </select>
                        </td>
                        <td>
                            <select id="FORCE_ALIGN_EN_SELECT">
                                <option value="0">0 = Disable Force Align state</option>
                                <option value="1">1 = Enable Force Align state</option>
                            </select>
                        </td>
                        <td>
                            <select id="FORCE_SLOW_FIRST_CYCLE_EN_SELECT">
                                <option value="0">0 = Disable Force Slow First Cycle</option>
                                <option value="1">1 = Enable Force Slow First Cycle</option>
                            </select>
                        </td>
                        <td>
                            <select id="FORCE_IPD_EN_SELECT">
                                <option value="0">0 = Disable Force IPD state</option>
                                <option value="1">1 = Enable Force IPD state</option>
                            </select>
                        </td>
                        <td>
                            <select id="FORCE_ISD_EN_SELECT">
                                <option value="0">0 = Disable Force ISD state</option>
                                <option value="1">1 = Enable Force ISD state</option>
                            </select>
                        </td>
                        <td>
                            <select id="FORCE_ALIGN_ANGLE_SRC_SEL_SELECT">
                                <option value="0">0 = Force Align Angle defined by ALIGN_ANGLE</option>
                                <option value="1">1 = Force Align Angle defined by FORCED_ALIGN_ANGLE</option>
                            </select>
                        </td>
                        <td><input type="text" id="FORCE_IQ_REF_SPEED_LOOP_DIS_SELECT" value="0"></td>
                        <td><button onclick="WriteAlgoCtrl1()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>

    <div class="container collapsed" id="devCtrlContainer">
        <div class="header" onclick="toggleContainer('devCtrlContainer', 'devCtrlContent', 'devCtrlArrow')">
            DEV_CTRL
            <span class="arrow" id="devCtrlArrow">▼</span>
        </div>

        <div class="content" id="devCtrlContent" style="display: none;">
            <div class="table-container">
                <table>
                    <tr>
                        <th title="Write the configuration to EEPROM">EEPROM_WRT</th>
                        <th title="Read the default configuration from EEPROM">EEPROM_READ</th>
                        <th title="Clears all faults">CLR_FLT</th>
                        <th title="Clears fault retry count">CLR_FLT_RETRY_COUNT</th>
                        <th title="EEPROM write access key">EEPROM_WRITE_ACCESS_KEY</th>
                        <th title="9-bit value used during forced align state">FORCED_ALIGN_ANGLE</th>
                        <th title="RAM bit to tickle watchdog in I2C mode">WATCHDOG_TICKLE</th>
                        <th><button onclick="WriteEEPROM()">Write EEPROM</button></th>
                    </tr>

                    <tr>
                        <td><input type="text" id="EEPROM_WRT" readonly></td>
                        <td><input type="text" id="EEPROM_READ" readonly></td>
                        <td><input type="text" id="CLR_FLT" readonly></td>
                        <td><input type="text" id="CLR_FLT_RETRY_COUNT" readonly></td>
                        <td><input type="text" id="EEPROM_WRITE_ACCESS_KEY" readonly></td>
                        <td><input type="text" id="FORCED_ALIGN_ANGLE" readonly></td>
                        <td><input type="text" id="WATCHDOG_TICKLE" readonly></td>
                        <td><button onclick="ReadDevCtrl()">Read</button></td>
                    </tr>

                    <tr>
                        <td>
                            <select id="EEPROM_WRT_SELECT">
                                <option value="0">0 = Do not write to EEPROM</option>
                                <option value="1">1 = Write configuration to EEPROM</option>
                            </select>
                        </td>
                        <td>
                            <select id="EEPROM_READ_SELECT">
                                <option value="0">0 = Do not read EEPROM</option>
                                <option value="1">1 = Read default configuration from EEPROM</option>
                            </select>
                        </td>
                        <td>
                            <select id="CLR_FLT_SELECT">
                                <option value="0">0 = Do not clear faults</option>
                                <option value="1">1 = Clear all faults</option>
                            </select>
                        </td>
                        <td>
                            <select id="CLR_FLT_RETRY_COUNT_SELECT">
                                <option value="0">0 = Do not clear retry count</option>
                                <option value="1">1 = Clear fault retry count</option>
                            </select>
                        </td>
                        <td><input type="text" id="EEPROM_WRITE_ACCESS_KEY_SELECT" value="0"></td>
                        <td><input type="text" id="FORCED_ALIGN_ANGLE_SELECT" value="0"></td>
                        <td>
                            <select id="WATCHDOG_TICKLE_SELECT">
                                <option value="0">0 = Do not tickle watchdog</option>
                                <option value="1">1 = Tickle watchdog in I2C mode</option>
                            </select>
                        </td>
                        <td><button onclick="WriteDevCtrl()">Write</button></td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>
<!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////>


    <div class="container collapsed" id="controlContainer">
        <div class="header" onclick="toggleContainer('controlContainer', 'controlContent', 'controlArrow')">
            Read | Write | Other Functions
            <span class="arrow" id="controlArrow">▼</span>
        </div>

        <div class="content" id="controlContent" style="display: none;">
            <div class="table-container">
                <table>

                    <tr>
                        <th title="Register Address">Register Address</th>
                        <th title="Register Value">Register Value</th>
                        <th title="Read / Write">Read / Write</th>
                        <th title="Other Functions">Other Functions</th>

                    </tr>

                    <tr>
                        <td><input type="text" id="ReadRegAddress" value="0x"></td>
                        <td><input type="text" id="ReadRegValue" readonly></td>
                        <th><button onclick="ReadRegister()">Read Register</button></th>
                        <th><button onclick="ReadAlgorithmState()">Read Algorithm State</button></th>

                    </tr>

                    <tr>
                        <td><input type="text" id="WriteRegAddress" value="0x"></td>
                        <td><input type="text" id="WriteRegValue" value="0x"></td>
                        <th><button onclick="WriteRegister()">Write Register</button></th>
                        <th><button onclick="WriteEEPROM()">Write EEPROM</button></th>
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadRevDriveConfig() {
        fetch('/ReadRevDriveConfig', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A1').value = data.REV_DRV_OPEN_LOOP_ACCEL_A1;
            document.getElementById('REV_DRV_OPEN_LOOP_ACCEL_A2').value = data.REV_DRV_OPEN_LOOP_ACCEL_A2;
            document.getElementById('ACTIVE_BRAKE_CURRENT_LIMIT').value = data.ACTIVE_BRAKE_CURRENT_LIMIT;
            document.getElementById('ACTIVE_BRAKE_KP').value = data.ACTIVE_BRAKE_KP;
            document.getElementById('ACTIVE_BRAKE_KI').value = data.ACTIVE_BRAKE_KI;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadMotorStartup1() {
        fetch('/ReadMotorStartup1', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('MTR_STARTUP').value = data.MTR_STARTUP;
            document.getElementById('ALIGN_SLOW_RAMP_RATE').value = data.ALIGN_SLOW_RAMP_RATE;
            document.getElementById('ALIGN_TIME').value = data.ALIGN_TIME;
            document.getElementById('ALIGN_OR_SLOW_CURRENT_ILIMIT').value = data.ALIGN_OR_SLOW_CURRENT_ILIMIT;
            document.getElementById('IPD_CLK_FREQ').value = data.IPD_CLK_FREQ;
            document.getElementById('IPD_CURR_THR').value = data.IPD_CURR_THR;
            document.getElementById('IPD_RLS_MODE').value = data.IPD_RLS_MODE;
            document.getElementById('IPD_ADV_ANGLE').value = data.IPD_ADV_ANGLE;
            document.getElementById('IPD_REPEAT').value = data.IPD_REPEAT;
            document.getElementById('OL_ILIMIT_CONFIG').value = data.OL_ILIMIT_CONFIG;
            document.getElementById('IQ_RAMP_EN').value = data.IQ_RAMP_EN;
            document.getElementById('ACTIVE_BRAKE_EN').value = data.ACTIVE_BRAKE_EN;
            document.getElementById('REV_DRV_CONFIG').value = data.REV_DRV_CONFIG;

            document.getElementById('MTR_STARTUP_SELECT').value = data.MTR_STARTUP;
            document.getElementById('ALIGN_SLOW_RAMP_RATE_SELECT').value = data.ALIGN_SLOW_RAMP_RATE;
            document.getElementById('ALIGN_TIME_SELECT').value = data.ALIGN_TIME;
            document.getElementById('ALIGN_OR_SLOW_CURRENT_ILIMIT_SELECT').value = data.ALIGN_OR_SLOW_CURRENT_ILIMIT;
            document.getElementById('IPD_CLK_FREQ_SELECT').value = data.IPD_CLK_FREQ;
            document.getElementById('IPD_CURR_THR_SELECT').value = data.IPD_CURR_THR;
            document.getElementById('IPD_RLS_MODE_SELECT').value = data.IPD_RLS_MODE;
            document.getElementById('IPD_ADV_ANGLE_SELECT').value = data.IPD_ADV_ANGLE;
            document.getElementById('IPD_REPEAT_SELECT').value = data.IPD_REPEAT;
            document.getElementById('OL_ILIMIT_CONFIG_SELECT').value = data.OL_ILIMIT_CONFIG;
            document.getElementById('IQ_RAMP_EN_SELECT').value = data.IQ_RAMP_EN;
            document.getElementById('ACTIVE_BRAKE_EN_SELECT').value = data.ACTIVE_BRAKE_EN;
            document.getElementById('REV_DRV_CONFIG_SELECT').value = data.REV_DRV_CONFIG;
        })
        .catch(error => console.error('Hata:', error));
    }


    function WriteMotorStartup1() {
        let data = {
            MTR_STARTUP: document.getElementById('MTR_STARTUP_SELECT').value,
            ALIGN_SLOW_RAMP_RATE: document.getElementById('ALIGN_SLOW_RAMP_RATE_SELECT').value,
            ALIGN_TIME: document.getElementById('ALIGN_TIME_SELECT').value,
            ALIGN_OR_SLOW_CURRENT_ILIMIT: document.getElementById('ALIGN_OR_SLOW_CURRENT_ILIMIT_SELECT').value,
            IPD_CLK_FREQ: document.getElementById('IPD_CLK_FREQ_SELECT').value,
            IPD_CURR_THR: document.getElementById('IPD_CURR_THR_SELECT').value,
            IPD_RLS_MODE: document.getElementById('IPD_RLS_MODE_SELECT').value,
            IPD_ADV_ANGLE: document.getElementById('IPD_ADV_ANGLE_SELECT').value,
            IPD_REPEAT: document.getElementById('IPD_REPEAT_SELECT').value,
            OL_ILIMIT_CONFIG: document.getElementById('OL_ILIMIT_CONFIG_SELECT').value,
            IQ_RAMP_EN: document.getElementById('IQ_RAMP_EN_SELECT').value,
            ACTIVE_BRAKE_EN: document.getElementById('ACTIVE_BRAKE_EN_SELECT').value,
            REV_DRV_CONFIG: document.getElementById('REV_DRV_CONFIG_SELECT').value
        };

        fetch('/WriteMotorStartup1', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response))
        .catch(error => console.error('Yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadMotorStartup2() {
        fetch('/ReadMotorStartup2', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('OL_ILIMIT').value = data.OL_ILIMIT;
            document.getElementById('OL_ACC_A1').value = data.OL_ACC_A1;
            document.getElementById('OL_ACC_A2').value = data.OL_ACC_A2;
            document.getElementById('AUTO_HANDOFF_EN').value = data.AUTO_HANDOFF_EN;
            document.getElementById('OPN_CL_HANDOFF_THR').value = data.OPN_CL_HANDOFF_THR;
            document.getElementById('ALIGN_ANGLE').value = data.ALIGN_ANGLE;
            document.getElementById('SLOW_FIRST_CYC_FREQ').value = data.SLOW_FIRST_CYC_FREQ;
            document.getElementById('FIRST_CYCLE_FREQ_SEL').value = data.FIRST_CYCLE_FREQ_SEL;
            document.getElementById('THETA_ERROR_RAMP_RATE').value = data.THETA_ERROR_RAMP_RATE;

            document.getElementById('OL_ILIMIT_SELECT').value = data.OL_ILIMIT;
            document.getElementById('OL_ACC_A1_SELECT').value = data.OL_ACC_A1;
            document.getElementById('OL_ACC_A2_SELECT').value = data.OL_ACC_A2;
            document.getElementById('AUTO_HANDOFF_EN_SELECT').value = data.AUTO_HANDOFF_EN;
            document.getElementById('OPN_CL_HANDOFF_THR_SELECT').value = data.OPN_CL_HANDOFF_THR;
            document.getElementById('ALIGN_ANGLE_SELECT').value = data.ALIGN_ANGLE;
            document.getElementById('SLOW_FIRST_CYC_FREQ_SELECT').value = data.SLOW_FIRST_CYC_FREQ;
            document.getElementById('FIRST_CYCLE_FREQ_SEL_SELECT').value = data.FIRST_CYCLE_FREQ_SEL;
            document.getElementById('THETA_ERROR_RAMP_RATE_SELECT').value = data.THETA_ERROR_RAMP_RATE;
        })
        .catch(error => console.error('Hata:', error));
    }


    function WriteMotorStartup2() {
        let data = {
            OL_ILIMIT: document.getElementById('OL_ILIMIT_SELECT').value,
            OL_ACC_A1: document.getElementById('OL_ACC_A1_SELECT').value,
            OL_ACC_A2: document.getElementById('OL_ACC_A2_SELECT').value,
            AUTO_HANDOFF_EN: document.getElementById('AUTO_HANDOFF_EN_SELECT').value,
            OPN_CL_HANDOFF_THR: document.getElementById('OPN_CL_HANDOFF_THR_SELECT').value,
            ALIGN_ANGLE: document.getElementById('ALIGN_ANGLE_SELECT').value,
            SLOW_FIRST_CYC_FREQ: document.getElementById('SLOW_FIRST_CYC_FREQ_SELECT').value,
            FIRST_CYCLE_FREQ_SEL: document.getElementById('FIRST_CYCLE_FREQ_SEL_SELECT').value,
            THETA_ERROR_RAMP_RATE: document.getElementById('THETA_ERROR_RAMP_RATE_SELECT').value
        };

        fetch('/WriteMotorStartup2', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response))
        .catch(error => console.error('Yazma hatası:', error));
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    function ReadClosedLoop1() {
        fetch('/ReadClosedLoop1', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('OVERMODULATION_ENABLE').value = data.OVERMODULATION_ENABLE;
            document.getElementById('CL_ACC').value = data.CL_ACC;
            document.getElementById('CL_DEC_CONFIG').value = data.CL_DEC_CONFIG;
            document.getElementById('CL_DEC').value = data.CL_DEC;
            document.getElementById('PWM_FREQ_OUT').value = data.PWM_FREQ_OUT;
            document.getElementById('PWM_MODE').value = data.PWM_MODE;
            document.getElementById('FG_SEL').value = data.FG_SEL;
            document.getElementById('FG_DIV').value = data.FG_DIV;
            document.getElementById('FG_CONFIG').value = data.FG_CONFIG;
            document.getElementById('FG_BEMF_THR').value = data.FG_BEMF_THR;
            document.getElementById('AVS_EN').value = data.AVS_EN;
            document.getElementById('DEADTIME_COMP_EN').value = data.DEADTIME_COMP_EN;
            document.getElementById('SPEED_LOOP_DIS').value = data.SPEED_LOOP_DIS;
            document.getElementById('LOW_SPEED_RECIRC_BRAKE_EN').value = data.LOW_SPEED_RECIRC_BRAKE_EN;

            document.getElementById('OVERMODULATION_ENABLE_SELECT').value = data.OVERMODULATION_ENABLE;
            document.getElementById('CL_ACC_SELECT').value = data.CL_ACC;
            document.getElementById('CL_DEC_CONFIG_SELECT').value = data.CL_DEC_CONFIG;
            document.getElementById('CL_DEC_SELECT').value = data.CL_DEC;
            document.getElementById('PWM_FREQ_OUT_SELECT').value = data.PWM_FREQ_OUT;
            document.getElementById('PWM_MODE').value = data.PWM_MODE;
            document.getElementById('FG_SEL_SELECT').value = data.FG_SEL;
            document.getElementById('FG_DIV_SELECT').value = data.FG_DIV;
            document.getElementById('FG_CONFIG_SELECT').value = data.FG_CONFIG;
            document.getElementById('FG_BEMF_THR_SELECT').value = data.FG_BEMF_THR;
            document.getElementById('AVS_EN_SELECT').value = data.AVS_EN;
            document.getElementById('DEADTIME_COMP_EN_SELECT').value = data.DEADTIME_COMP_EN;
            document.getElementById('SPEED_LOOP_DIS_SELECT').value = data.SPEED_LOOP_DIS;
            document.getElementById('LOW_SPEED_RECIRC_BRAKE_EN_SELECT').value = data.LOW_SPEED_RECIRC_BRAKE_EN;
        });
    }



    function WriteClosedLoop1() {
        let data = {
            OVERMODULATION_ENABLE: document.getElementById('OVERMODULATION_ENABLE_SELECT').value,
            CL_ACC: document.getElementById('CL_ACC_SELECT').value,
            CL_DEC_CONFIG: document.getElementById('CL_DEC_CONFIG_SELECT').value,
            CL_DEC: document.getElementById('CL_DEC_SELECT').value,
            PWM_FREQ_OUT: document.getElementById('PWM_FREQ_OUT_SELECT').value,
            PWM_MODE: document.getElementById('PWM_MODE_SELECT').value,
            FG_SEL: document.getElementById('FG_SEL_SELECT').value,
            FG_DIV: document.getElementById('FG_DIV_SELECT').value,
            FG_CONFIG: document.getElementById('FG_CONFIG_SELECT').value,
            FG_BEMF_THR: document.getElementById('FG_BEMF_THR_SELECT').value,
            AVS_EN: document.getElementById('AVS_EN_SELECT').value,
            DEADTIME_COMP_EN: document.getElementById('DEADTIME_COMP_EN_SELECT').value,
            SPEED_LOOP_DIS: document.getElementById('SPEED_LOOP_DIS_SELECT').value,
            LOW_SPEED_RECIRC_BRAKE_EN: document.getElementById('LOW_SPEED_RECIRC_BRAKE_EN_SELECT').value
        };

        fetch('/WriteClosedLoop1', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response));
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadClosedLoop2() {
        fetch('/ReadClosedLoop2', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('MTR_STOP').value = data.MTR_STOP;
            document.getElementById('MTR_STOP_BRK_TIME').value = data.MTR_STOP_BRK_TIME;
            document.getElementById('ACT_SPIN_THR').value = data.ACT_SPIN_THR;
            document.getElementById('BRAKE_SPEED_THRESHOLD').value = data.BRAKE_SPEED_THRESHOLD;
            document.getElementById('MOTOR_RES').value = data.MOTOR_RES;
            document.getElementById('MOTOR_IND').value = data.MOTOR_IND;

            // Dropdownlar için
            document.getElementById('MTR_STOP_SELECT').value = data.MTR_STOP;
            document.getElementById('MTR_STOP_BRK_TIME_SELECT').value = data.MTR_STOP_BRK_TIME;
            document.getElementById('ACT_SPIN_THR_SELECT').value = data.ACT_SPIN_THR;
            document.getElementById('BRAKE_SPEED_THRESHOLD_SELECT').value = data.BRAKE_SPEED_THRESHOLD;

            // Number inputlar için
            document.getElementById('MOTOR_RES_INPUT').value = data.MOTOR_RES;
            document.getElementById('MOTOR_IND_INPUT').value = data.MOTOR_IND;
        });
    }

    function WriteClosedLoop2() {
        let data = {
            MTR_STOP: document.getElementById('MTR_STOP_SELECT').value,
            MTR_STOP_BRK_TIME: document.getElementById('MTR_STOP_BRK_TIME_SELECT').value,
            ACT_SPIN_THR: document.getElementById('ACT_SPIN_THR_SELECT').value,
            BRAKE_SPEED_THRESHOLD: document.getElementById('BRAKE_SPEED_THRESHOLD_SELECT').value,
            MOTOR_RES: document.getElementById('MOTOR_RES_INPUT').value,
            MOTOR_IND: document.getElementById('MOTOR_IND_INPUT').value
        };

        fetch('/WriteClosedLoop2', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response));
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadClosedLoop3() {
        fetch('/ReadClosedLoop3', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('MOTOR_BEMF_CONST').value = data.MOTOR_BEMF_CONST;
            document.getElementById('CURR_LOOP_KP').value = data.CURR_LOOP_KP;
            document.getElementById('CURR_LOOP_KI').value = data.CURR_LOOP_KI;
            document.getElementById('SPD_LOOP_KP_CL3').value = data.SPD_LOOP_KP_CL3;

            document.getElementById('MOTOR_BEMF_CONST_INPUT').value = data.MOTOR_BEMF_CONST;
            document.getElementById('CURR_LOOP_KP_INPUT').value = data.CURR_LOOP_KP;
            document.getElementById('CURR_LOOP_KI_INPUT').value = data.CURR_LOOP_KI;
            document.getElementById('SPD_LOOP_KP_CL3_INPUT').value = data.SPD_LOOP_KP_CL3;
        });
    }

    function WriteClosedLoop3() {
        let data = {
            MOTOR_BEMF_CONST: document.getElementById('MOTOR_BEMF_CONST_INPUT').value,
            CURR_LOOP_KP: document.getElementById('CURR_LOOP_KP_INPUT').value,
            CURR_LOOP_KI: document.getElementById('CURR_LOOP_KI_INPUT').value,
            SPD_LOOP_KP_CL3: document.getElementById('SPD_LOOP_KP_CL3_INPUT').value
        };

        fetch('/WriteClosedLoop3', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response));
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function ReadClosedLoop4() {
    fetch('/ReadClosedLoop4', { method: 'GET' })
    .then(response => response.json())
    .then(data => {
        document.getElementById('SPD_LOOP_KP_CL4').value = data.SPD_LOOP_KP_CL4;
        document.getElementById('SPD_LOOP_KI').value = data.SPD_LOOP_KI;
        document.getElementById('MAX_SPEED').value = data.MAX_SPEED;

        document.getElementById('SPD_LOOP_KP_CL4_INPUT').value = data.SPD_LOOP_KP_CL4;
        document.getElementById('SPD_LOOP_KI_INPUT').value = data.SPD_LOOP_KI;
        document.getElementById('MAX_SPEED_INPUT').value = data.MAX_SPEED;
    });
}

function WriteClosedLoop4() {
    let data = {
        SPD_LOOP_KP_CL4: document.getElementById('SPD_LOOP_KP_CL4_INPUT').value,
        SPD_LOOP_KI: document.getElementById('SPD_LOOP_KI_INPUT').value,
        MAX_SPEED: document.getElementById('MAX_SPEED_INPUT').value
    };

    fetch('/WriteClosedLoop4', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(data)
    });
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadPinConfig() {
        fetch('/ReadPinConfig', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('BRAKE_PIN_MODE').value = data.BRAKE_PIN_MODE;
            document.getElementById('ALIGN_BRAKE_ANGLE_SEL').value = data.ALIGN_BRAKE_ANGLE_SEL;
            document.getElementById('BRAKE_INPUT').value = data.BRAKE_INPUT;
            document.getElementById('SPEED_MODE').value = data.SPEED_MODE;

            document.getElementById('BRAKE_PIN_MODE_SELECT').value = data.BRAKE_PIN_MODE;
            document.getElementById('ALIGN_BRAKE_ANGLE_SEL_SELECT').value = data.ALIGN_BRAKE_ANGLE_SEL;
            document.getElementById('BRAKE_INPUT_SELECT').value = data.BRAKE_INPUT;
            document.getElementById('SPEED_MODE_SELECT').value = data.SPEED_MODE;
        })
        .catch(error => console.error('Hata:', error));
    }


    function WritePinConfig() {
        let data = {
            BRAKE_PIN_MODE: document.getElementById('BRAKE_PIN_MODE_SELECT').value,
            ALIGN_BRAKE_ANGLE_SEL: document.getElementById('ALIGN_BRAKE_ANGLE_SEL_SELECT').value,
            BRAKE_INPUT: document.getElementById('BRAKE_INPUT_SELECT').value,
            SPEED_MODE: document.getElementById('SPEED_MODE_SELECT').value
        };

        fetch('/WritePinConfig', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response))
        .catch(error => console.error('Yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadAlgoCtrl1() {
        fetch('/ReadAlgoCtrl1', { method: 'GET' })
        .then(response => response.json())
        .then(data => {
            document.getElementById('OVERRIDE').value = data.OVERRIDE;
            document.getElementById('DIGITAL_SPEED_CTRL').value = data.DIGITAL_SPEED_CTRL;
            document.getElementById('CLOSED_LOOP_DIS').value = data.CLOSED_LOOP_DIS;
            document.getElementById('FORCE_ALIGN_EN').value = data.FORCE_ALIGN_EN;
            document.getElementById('FORCE_SLOW_FIRST_CYCLE_EN').value = data.FORCE_SLOW_FIRST_CYCLE_EN;
            document.getElementById('FORCE_IPD_EN').value = data.FORCE_IPD_EN;
            document.getElementById('FORCE_ISD_EN').value = data.FORCE_ISD_EN;
            document.getElementById('FORCE_ALIGN_ANGLE_SRC_SEL').value = data.FORCE_ALIGN_ANGLE_SRC_SEL;
            document.getElementById('FORCE_IQ_REF_SPEED_LOOP_DIS').value = data.FORCE_IQ_REF_SPEED_LOOP_DIS;

            document.getElementById('OVERRIDE_SELECT').value = data.OVERRIDE;
            document.getElementById('DIGITAL_SPEED_CTRL_SELECT').value = data.DIGITAL_SPEED_CTRL;
            document.getElementById('CLOSED_LOOP_DIS_SELECT').value = data.CLOSED_LOOP_DIS;
            document.getElementById('FORCE_ALIGN_EN_SELECT').value = data.FORCE_ALIGN_EN;
            document.getElementById('FORCE_SLOW_FIRST_CYCLE_EN_SELECT').value = data.FORCE_SLOW_FIRST_CYCLE_EN;
            document.getElementById('FORCE_IPD_EN_SELECT').value = data.FORCE_IPD_EN;
            document.getElementById('FORCE_ISD_EN_SELECT').value = data.FORCE_ISD_EN;
            document.getElementById('FORCE_ALIGN_ANGLE_SRC_SEL_SELECT').value = data.FORCE_ALIGN_ANGLE_SRC_SEL;
            document.getElementById('FORCE_IQ_REF_SPEED_LOOP_DIS_SELECT').value = data.FORCE_IQ_REF_SPEED_LOOP_DIS;
        })
        .catch(error => console.error('Hata:', error));
    }


    function WriteAlgoCtrl1() {
        let data = {
            OVERRIDE: document.getElementById('OVERRIDE_SELECT').value,
            DIGITAL_SPEED_CTRL: document.getElementById('DIGITAL_SPEED_CTRL_SELECT').value,
            CLOSED_LOOP_DIS: document.getElementById('CLOSED_LOOP_DIS_SELECT').value,
            FORCE_ALIGN_EN: document.getElementById('FORCE_ALIGN_EN_SELECT').value,
            FORCE_SLOW_FIRST_CYCLE_EN: document.getElementById('FORCE_SLOW_FIRST_CYCLE_EN_SELECT').value,
            FORCE_IPD_EN: document.getElementById('FORCE_IPD_EN_SELECT').value,
            FORCE_ISD_EN: document.getElementById('FORCE_ISD_EN_SELECT').value,
            FORCE_ALIGN_ANGLE_SRC_SEL: document.getElementById('FORCE_ALIGN_ANGLE_SRC_SEL_SELECT').value,
            FORCE_IQ_REF_SPEED_LOOP_DIS: document.getElementById('FORCE_IQ_REF_SPEED_LOOP_DIS_SELECT').value
        };

        fetch('/WriteAlgoCtrl1', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
        .then(response => response.text())
        .then(response => console.log("Yazma başarılı:", response))
        .catch(error => console.error('Yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadDevCtrl() {
        fetch('/ReadDevCtrl', { method: 'GET' })
            .then(response => response.json())
            .then(data => {
                document.getElementById('EEPROM_WRT').value = data.EEPROM_WRT;
                document.getElementById('EEPROM_READ').value = data.EEPROM_READ;
                document.getElementById('CLR_FLT').value = data.CLR_FLT;
                document.getElementById('CLR_FLT_RETRY_COUNT').value = data.CLR_FLT_RETRY_COUNT;
                document.getElementById('EEPROM_WRITE_ACCESS_KEY').value = data.EEPROM_WRITE_ACCESS_KEY;
                document.getElementById('FORCED_ALIGN_ANGLE').value = data.FORCED_ALIGN_ANGLE;
                document.getElementById('WATCHDOG_TICKLE').value = data.WATCHDOG_TICKLE;

                document.getElementById('EEPROM_WRT_SELECT').value = data.EEPROM_WRT;
                document.getElementById('EEPROM_READ_SELECT').value = data.EEPROM_READ;
                document.getElementById('CLR_FLT_SELECT').value = data.CLR_FLT;
                document.getElementById('CLR_FLT_RETRY_COUNT_SELECT').value = data.CLR_FLT_RETRY_COUNT;
                document.getElementById('EEPROM_WRITE_ACCESS_KEY_SELECT').value = data.EEPROM_WRITE_ACCESS_KEY;
                document.getElementById('FORCED_ALIGN_ANGLE_SELECT').value = data.FORCED_ALIGN_ANGLE;
                document.getElementById('WATCHDOG_TICKLE_SELECT').value = data.WATCHDOG_TICKLE;
            })
            .catch(error => console.error('Hata:', error));
    }


    function WriteDevCtrl() {
        let data = {
            EEPROM_WRT: document.getElementById('EEPROM_WRT_SELECT').value,
            EEPROM_READ: document.getElementById('EEPROM_READ_SELECT').value,
            CLR_FLT: document.getElementById('CLR_FLT_SELECT').value,
            CLR_FLT_RETRY_COUNT: document.getElementById('CLR_FLT_RETRY_COUNT_SELECT').value,
            EEPROM_WRITE_ACCESS_KEY: document.getElementById('EEPROM_WRITE_ACCESS_KEY_SELECT').value,
            FORCED_ALIGN_ANGLE: document.getElementById('FORCED_ALIGN_ANGLE_SELECT').value,
            WATCHDOG_TICKLE: document.getElementById('WATCHDOG_TICKLE_SELECT').value,
        };

        fetch('/WriteDevCtrl', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        })
            .then(response => response.text())
            .then(response => console.log("Yazma başarılı:", response))
            .catch(error => console.error('Yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function WriteEEPROM() {
        fetch('/WriteEEPROM', { method: 'GET' })
            .then(response => response.json())
            .then(data => console.log("EEPROM yazma başarılı:", data))
            .catch(error => console.error('EEPROM yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadAlgorithmState() {
        fetch('/ReadAlgorithmState', { method: 'GET' })
            .then(response => response.json())
            .then(data => console.log("EEPROM yazma başarılı:", data))
            .catch(error => console.error('EEPROM yazma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function ReadRegister() {
        let address = document.getElementById('ReadRegAddress').value;

        fetch(`/ReadRegister?address=${address}`, { method: 'GET' })
            .then(response => response.json())
            .then(data => {
                document.getElementById('ReadRegValue').value = data.value;
            })
            .catch(error => console.error('Register okuma hatası:', error));
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function WriteRegister() {
        let data = {
            address: document.getElementById('WriteRegAddress').value,
            value: document.getElementById('WriteRegValue').value
        };

        fetch('/WriteRegister', {
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

#define MCF8316A_ADDRESS 0x01

// Register Address Definitions
#define ISD_CONFIG_REG 0x00000080
#define REV_DRIVE_CONFIG_REG 0x00000082
#define MOTOR_STARTUP1_REG 0x00000084
#define MOTOR_STARTUP2_REG 0x00000086
#define CLOSED_LOOP1_REG 0x00000088
#define CLOSED_LOOP2_REG 0x0000008A
#define CLOSED_LOOP3_REG 0x0000008C
#define CLOSED_LOOP4_REG 0x0000008E
#define SPEED_PROFILES1_REG 0x00000094
#define SPEED_PROFILES2_REG 0x00000096
#define SPEED_PROFILES3_REG 0x00000098
#define SPEED_PROFILES4_REG 0x0000009A
#define SPEED_PROFILES5_REG 0x0000009C
#define SPEED_PROFILES6_REG 0x0000009E
#define FAULT_CONFIG1_REG 0x00000090
#define FAULT_CONFIG2_REG 0x00000092
#define PIN_CONFIG_REG 0x000000A4
#define DEVICE_CONFIG1_REG 0x000000A6
#define DEVICE_CONFIG2_REG 0x000000A8
#define PERI_CONFIG1_REG 0x000000AA
#define GD_CONFIG1_REG 0x000000AC
#define GD_CONFIG2_REG 0x000000AE
#define ALGO_CTRL1_REG 0x000000EC
#define INT_ALGO_1_REG 0x000000A0
#define INT_ALGO_2_REG 0x000000A2
#define DEV_CTRL_REG 0x000000EA

unsigned long register_value = 0;

JsonDocument doc;

#define READ_BITS(value, high, low) ((value >> low) & ((1 << (high - low + 1)) - 1))
#define WRITE_BITS(target, value, high, low) (target |= ((value & ((1 << (high - low + 1)) - 1)) << low))

String toUpperCaseHex(uint16_t value)
{
    String hexString = String(value, HEX); // HEX string'e çevir
    hexString.toUpperCase();               // Büyük harfe çevir
    return hexString;
}

void handleRoot()
{
    server.send(200, "text/html", HTML_PAGE);
}

/////////////////////////////////////////////////////////////////////////////////

void read32(int reg_addr)
{
    byte reg_addr_H = (reg_addr & 0x0F00) >> 8;
    byte reg_addr_L = (reg_addr & 0xFF);

    const byte control_word[] = {0x90, reg_addr_H, reg_addr_L};

    Wire.beginTransmission(MCF8316A_ADDRESS);
    Wire.write(control_word, 3);
    Wire.endTransmission();

    Wire.requestFrom(MCF8316A_ADDRESS, 4);

    for (int i = 0; i < 4; i++)
    {
        unsigned long c = Wire.read();
        register_value |= (c << (i * 8));
    }

    Serial.print("Read Register: 0x");
    Serial.print(reg_addr, HEX);
    Serial.print(" | Value: 0x");
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

    Wire.beginTransmission(MCF8316A_ADDRESS);
    Wire.write(control_word, 3);
    Wire.write(data, 4);
    Wire.endTransmission();

    Serial.print("Write Register: 0x");
    Serial.print(reg_addr, HEX);
    Serial.print(" | Value: 0x");
    Serial.println(writedata, HEX);
}

/////////////////////////////////////////////////////////////////////////////////

void readRegister(uint32_t reg, std::initializer_list<std::pair<const char*, std::pair<int, int>>> bitFields)
{
    register_value = 0;
    read32(reg);
    doc.clear();

    for (auto &field : bitFields)
    {
        const char* fieldName = field.first;
        int high = field.second.first;
        int low = field.second.second;

        doc[fieldName] = toUpperCaseHex(READ_BITS(register_value, high, low));
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void writeRegister(uint32_t reg, std::initializer_list<std::pair<const char*, std::pair<int, int>>> bitFields)
{
    if (!server.hasArg("plain"))
    {
        server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
        return;
    }

    doc.clear();
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error)
    {
        server.send(400, "application/json", "{\"error\":\"JSON Parse Failed\"}");
        return;
    }

    register_value = 0;
    WRITE_BITS(register_value, 0, 31, 31);

    for (auto &field : bitFields)
    {
        const char* fieldName = field.first;
        int high = field.second.first;
        int low = field.second.second;

        uint32_t value = strtoul(doc[fieldName].as<String>().c_str(), NULL, 16);
        WRITE_BITS(register_value, value, high, low);
    }

    write32(reg, register_value);
    server.send(200, "application/json", "{\"status\":\"success\"}");
}

/////////////////////////////////////////////////////////////////////////////////

void ReadISDConfig()
{
    readRegister(ISD_CONFIG_REG,
    {
        {"ISD_EN", {30, 30}},
        {"BRAKE_EN", {29, 29}},
        {"HIZ_EN", {28, 28}},
        {"RVS_DR_EN", {27, 27}},
        {"RESYNC_EN", {26, 26}},
        {"FW_DRV_RESYN_THR", {25, 22}},
        {"BRK_MODE", {21, 21}},
        {"BRK_TIME", {16, 13}},
        {"HIZ_TIME", {12, 9}},
        {"STAT_DETECT_THR", {8, 6}},
        {"REV_DRV_HANDOFF_THR", {5, 2}},
        {"REV_DRV_OPEN_LOOP_CURRENT", {1, 0}}
    });
}

void WriteISDConfig()
{
    writeRegister(ISD_CONFIG_REG,
    {
        {"ISD_EN", {30, 30}},
        {"BRAKE_EN", {29, 29}},
        {"HIZ_EN", {28, 28}},
        {"RVS_DR_EN", {27, 27}},
        {"RESYNC_EN", {26, 26}},
        {"FW_DRV_RESYN_THR", {25, 22}},
        {"BRK_MODE", {21, 21}},
        {"BRK_TIME", {16, 13}},
        {"HIZ_TIME", {12, 9}},
        {"STAT_DETECT_THR", {8, 6}},
        {"REV_DRV_HANDOFF_THR", {5, 2}},
        {"REV_DRV_OPEN_LOOP_CURRENT", {1, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadRevDriveConfig()
{
    readRegister(REV_DRIVE_CONFIG_REG,
    {
        {"REV_DRV_OPEN_LOOP_ACCEL_A1", {30, 27}},
        {"REV_DRV_OPEN_LOOP_ACCEL_A2", {26, 23}},
        {"ACTIVE_BRAKE_CURRENT_LIMIT", {22, 20}},
        {"ACTIVE_BRAKE_KP", {19, 10}}, // 10-bit
        {"ACTIVE_BRAKE_KI", {9, 0}}    // 10-bit
    });
}

void WriteRevDriveConfig()
{
    writeRegister(REV_DRIVE_CONFIG_REG,
    {
        {"REV_DRV_OPEN_LOOP_ACCEL_A1", {30, 27}},
        {"REV_DRV_OPEN_LOOP_ACCEL_A2", {26, 23}},
        {"ACTIVE_BRAKE_CURRENT_LIMIT", {22, 20}},
        {"ACTIVE_BRAKE_KP", {19, 10}}, // 10-bit
        {"ACTIVE_BRAKE_KI", {9, 0}}    // 10-bit
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadMotorStartup1()
{
    readRegister(MOTOR_STARTUP1_REG,
    {
        {"MTR_STARTUP", {30, 29}},
        {"ALIGN_SLOW_RAMP_RATE", {28, 25}},
        {"ALIGN_TIME", {24, 21}},
        {"ALIGN_OR_SLOW_CURRENT_ILIMIT", {20, 17}},
        {"IPD_CLK_FREQ", {16, 14}},
        {"IPD_CURR_THR", {13, 9}},
        {"IPD_RLS_MODE", {8, 8}},
        {"IPD_ADV_ANGLE", {7, 6}},
        {"IPD_REPEAT", {5, 4}},
        {"OL_ILIMIT_CONFIG", {3, 3}},
        {"IQ_RAMP_EN", {2, 2}},
        {"ACTIVE_BRAKE_EN", {1, 1}},
        {"REV_DRV_CONFIG", {0, 0}}
    });
}

void WriteMotorStartup1()
{
    writeRegister(MOTOR_STARTUP1_REG,
    {
        {"MTR_STARTUP", {30, 29}},
        {"ALIGN_SLOW_RAMP_RATE", {28, 25}},
        {"ALIGN_TIME", {24, 21}},
        {"ALIGN_OR_SLOW_CURRENT_ILIMIT", {20, 17}},
        {"IPD_CLK_FREQ", {16, 14}},
        {"IPD_CURR_THR", {13, 9}},
        {"IPD_RLS_MODE", {8, 8}},
        {"IPD_ADV_ANGLE", {7, 6}},
        {"IPD_REPEAT", {5, 4}},
        {"OL_ILIMIT_CONFIG", {3, 3}},
        {"IQ_RAMP_EN", {2, 2}},
        {"ACTIVE_BRAKE_EN", {1, 1}},
        {"REV_DRV_CONFIG", {0, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadMotorStartup2()
{
    readRegister(MOTOR_STARTUP2_REG,
    {
        {"OL_ILIMIT", {30, 27}},              // Açık döngü akım limiti
        {"OL_ACC_A1", {26, 23}},              // Açık döngü ivme katsayısı A1
        {"OL_ACC_A2", {22, 19}},              // Açık döngü ivme katsayısı A2
        {"AUTO_HANDOFF_EN", {18, 18}},        // Otomatik devralma etkinleştirme
        {"OPN_CL_HANDOFF_THR", {17, 13}},     // Açık/kapalı döngü devralma eşiği
        {"ALIGN_ANGLE", {12, 8}},             // Hizalama açısı
        {"SLOW_FIRST_CYC_FREQ", {7, 4}},      // İlk döngü frekansı
        {"FIRST_CYCLE_FREQ_SEL", {3, 3}},     // İlk döngü frekans seçimi
        {"THETA_ERROR_RAMP_RATE", {2, 0}}     // Theta hata rampa oranı
    });
}

void WriteMotorStartup2()
{
    writeRegister(MOTOR_STARTUP2_REG,
    {
        {"OL_ILIMIT", {30, 27}},
        {"OL_ACC_A1", {26, 23}},
        {"OL_ACC_A2", {22, 19}},
        {"AUTO_HANDOFF_EN", {18, 18}},
        {"OPN_CL_HANDOFF_THR", {17, 13}},
        {"ALIGN_ANGLE", {12, 8}},
        {"SLOW_FIRST_CYC_FREQ", {7, 4}},
        {"FIRST_CYCLE_FREQ_SEL", {3, 3}},
        {"THETA_ERROR_RAMP_RATE", {2, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadClosedLoop1()
{
    readRegister(CLOSED_LOOP1_REG,
    {
        {"OVERMODULATION_ENABLE", {30, 30}},
        {"CL_ACC", {29, 25}},
        {"CL_DEC_CONFIG", {24, 24}},
        {"CL_DEC", {23, 19}},
        {"PWM_FREQ_OUT", {18, 15}},
        {"PWM_MODE", {14, 14}},
        {"FG_SEL", {13, 12}},
        {"FG_DIV", {11, 8}},
        {"FG_CONFIG", {7, 7}},
        {"FG_BEMF_THR", {6, 4}},
        {"AVS_EN", {3, 3}},
        {"DEADTIME_COMP_EN", {2, 2}},
        {"SPEED_LOOP_DIS", {1, 1}},
        {"LOW_SPEED_RECIRC_BRAKE_EN", {0, 0}}
    });
}

void WriteClosedLoop1()
{
    writeRegister(CLOSED_LOOP1_REG,
    {
        {"OVERMODULATION_ENABLE", {30, 30}},
        {"CL_ACC", {29, 25}},
        {"CL_DEC_CONFIG", {24, 24}},
        {"CL_DEC", {23, 19}},
        {"PWM_FREQ_OUT", {18, 15}},
        {"PWM_MODE", {14, 14}},
        {"FG_SEL", {13, 12}},
        {"FG_DIV", {11, 8}},
        {"FG_CONFIG", {7, 7}},
        {"FG_BEMF_THR", {6, 4}},
        {"AVS_EN", {3, 3}},
        {"DEADTIME_COMP_EN", {2, 2}},
        {"SPEED_LOOP_DIS", {1, 1}},
        {"LOW_SPEED_RECIRC_BRAKE_EN", {0, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadClosedLoop2()
{
    readRegister(CLOSED_LOOP2_REG,
    {
        {"MTR_STOP", {30, 28}},
        {"MTR_STOP_BRK_TIME", {27, 24}},
        {"ACT_SPIN_THR", {23, 20}},
        {"BRAKE_SPEED_THRESHOLD", {19, 16}},
        {"MOTOR_RES", {15, 8}},
        {"MOTOR_IND", {7, 0}}
    });
}

void WriteClosedLoop2()
{
    writeRegister(CLOSED_LOOP2_REG,
    {
        {"MTR_STOP", {30, 28}},
        {"MTR_STOP_BRK_TIME", {27, 24}},
        {"ACT_SPIN_THR", {23, 20}},
        {"BRAKE_SPEED_THRESHOLD", {19, 16}},
        {"MOTOR_RES", {15, 8}},
        {"MOTOR_IND", {7, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadClosedLoop3()
{
    readRegister(CLOSED_LOOP3_REG,
    {
        {"MOTOR_BEMF_CONST", {30, 23}},
        {"CURR_LOOP_KP", {22, 13}},
        {"CURR_LOOP_KI", {12, 3}},
        {"SPD_LOOP_KP_CL3", {2, 0}}
    });
}

void WriteClosedLoop3()
{
    writeRegister(CLOSED_LOOP3_REG,
    {
        {"MOTOR_BEMF_CONST", {30, 23}},
        {"CURR_LOOP_KP", {22, 13}},
        {"CURR_LOOP_KI", {12, 3}},
        {"SPD_LOOP_KP_CL3", {2, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadClosedLoop4()
{
    readRegister(CLOSED_LOOP4_REG,
    {
        {"SPD_LOOP_KP_CL4", {30, 24}},
        {"SPD_LOOP_KI", {23, 14}},
        {"MAX_SPEED", {13, 0}}
    });
}

void WriteClosedLoop4()
{
    writeRegister(CLOSED_LOOP4_REG,
    {
        {"SPD_LOOP_KP_CL4", {30, 24}},
        {"SPD_LOOP_KI", {23, 14}},
        {"MAX_SPEED", {13, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////





void ReadPinConfig()
{
    readRegister(PIN_CONFIG_REG,
    {
        {"BRAKE_PIN_MODE", {5, 5}},
        {"ALIGN_BRAKE_ANGLE_SEL", {4, 4}},
        {"BRAKE_INPUT", {3, 2}},
        {"SPEED_MODE", {1, 0}}
    });
}

void WritePinConfig()
{
    writeRegister(PIN_CONFIG_REG,
    {
        {"BRAKE_PIN_MODE", {5, 5}},
        {"ALIGN_BRAKE_ANGLE_SEL", {4, 4}},
        {"BRAKE_INPUT", {3, 2}},
        {"SPEED_MODE", {1, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadAlgoCtrl1()
{
    readRegister(ALGO_CTRL1_REG,
    {
        {"OVERRIDE", {31, 31}},
        {"DIGITAL_SPEED_CTRL", {30, 16}},
        {"CLOSED_LOOP_DIS", {15, 15}},
        {"FORCE_ALIGN_EN", {14, 14}},
        {"FORCE_SLOW_FIRST_CYCLE_EN", {13, 13}},
        {"FORCE_IPD_EN", {12, 12}},
        {"FORCE_ISD_EN", {11, 11}},
        {"FORCE_ALIGN_ANGLE_SRC_SEL", {10, 10}},
        {"FORCE_IQ_REF_SPEED_LOOP_DIS", {9, 0}}
    });
}

void WriteAlgoCtrl1()
{
    writeRegister(ALGO_CTRL1_REG,
    {
        {"OVERRIDE", {31, 31}},
        {"DIGITAL_SPEED_CTRL", {30, 16}},
        {"CLOSED_LOOP_DIS", {15, 15}},
        {"FORCE_ALIGN_EN", {14, 14}},
        {"FORCE_SLOW_FIRST_CYCLE_EN", {13, 13}},
        {"FORCE_IPD_EN", {12, 12}},
        {"FORCE_ISD_EN", {11, 11}},
        {"FORCE_ALIGN_ANGLE_SRC_SEL", {10, 10}},
        {"FORCE_IQ_REF_SPEED_LOOP_DIS", {9, 0}}
    });
}

/////////////////////////////////////////////////////////////////////////////////

void ReadDevCtrl()
{
    readRegister(DEV_CTRL_REG,
    {
        {"EEPROM_WRT", {31, 31}},
        {"EEPROM_READ", {30, 30}},
        {"CLR_FLT", {29, 29}},
        {"CLR_FLT_RETRY_COUNT", {28, 28}},
        {"EEPROM_WRITE_ACCESS_KEY", {27, 20}},
        {"FORCED_ALIGN_ANGLE", {19, 11}},
        {"WATCHDOG_TICKLE", {10, 10}}
    });
}

void WriteDevCtrl()
{
    writeRegister(DEV_CTRL_REG,
    {
        {"EEPROM_WRT", {31, 31}},
        {"EEPROM_READ", {30, 30}},
        {"CLR_FLT", {29, 29}},
        {"CLR_FLT_RETRY_COUNT", {28, 28}},
        {"EEPROM_WRITE_ACCESS_KEY", {27, 20}},
        {"FORCED_ALIGN_ANGLE", {19, 11}},
        {"WATCHDOG_TICKLE", {10, 10}}
    });
}



void WriteEEPROM()
{
    delay(500);
    write32(0x0000EA, 0x8A500000);
    //delay(1000);
    //write32(0x0000EA, 0x80000000);

    server.send(200, "application/json", "{\"status\":\"success\"}");
}

void ReadRegister()
{
    if (!server.hasArg("address"))
    {
        server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
        return;
    }

    doc.clear(); // 📌 JSON nesnesini temizle
    String addressStr = server.arg("address");
    uint32_t address = strtoul(addressStr.c_str(), NULL, 16);

    register_value = 0;
    read32(address);

    doc["address"] = addressStr;                       // JSON nesnesine adresi ekle
    doc["value"] = "0x" + String(register_value, HEX); // JSON nesnesine okunan değeri ekle

    String response;
    serializeJson(doc, response); // JSON'u string olarak hazırla
    server.send(200, "application/json", response);
}

void WriteRegister()
{
    if (!server.hasArg("plain"))
    {
        server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
        return;
    }

    doc.clear(); // 📌 JSON nesnesini temizle
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error)
    {
        server.send(400, "application/json", "{\"error\":\"JSON Parse Failed\"}");
        return;
    }

    uint32_t address = strtoul(doc["address"].as<String>().c_str(), NULL, 16);
    uint32_t value = strtoul(doc["value"].as<String>().c_str(), NULL, 16);

    write32(address, value);

    server.send(200, "application/json", "{\"status\":\"success\"}");
}

void ReadAlgorithmState()
{
    register_value = 0;
    read32(0x210);
    server.send(200, "application/json", "{\"status\":\"success\"}");
}

void default_settings()
{
    write32(0x00000080, 0x64738C20);
    write32(0x00000082, 0x28200000);
    write32(0x00000084, 0x0B6807D0);
    write32(0x00000086, 0x2306600C);
    write32(0x00000088, 0x0D3201B5);
    write32(0x0000008A, 0x1BAD0000);
    write32(0x0000008C, 0x00000000);
    write32(0x0000008E, 0x00000000);
    write32(0x00000094, 0x00000000);
    write32(0x00000096, 0x00000000);
    write32(0x00000098, 0x00000000);
    write32(0x0000009A, 0x00000000);
    write32(0x0000009C, 0x00000000);
    write32(0x0000009E, 0x00000000);
    write32(0x00000090, 0x3EC80106);
    write32(0x00000092, 0x70DC0888);
    write32(0x000000A4, 0x00000088);
    write32(0x000000A6, 0x00101462);
    write32(0x000000A8, 0x4000F00F);
    write32(0x000000AA, 0x41C01F00);
    write32(0x000000AC, 0x1C450100);
    write32(0x000000AE, 0x00200000);
    write32(0x000000A0, 0x2433407D);
    write32(0x000000A2, 0x000001A7);

    WriteEEPROM();
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

    // default_settings();

    server.on("/", handleRoot);
    server.on("/ReadISDConfig", HTTP_GET, ReadISDConfig);
    server.on("/WriteISDConfig", HTTP_POST, WriteISDConfig);

    server.on("/ReadRevDriveConfig", HTTP_GET, ReadRevDriveConfig);
    server.on("/WriteRevDriveConfig", HTTP_POST, WriteRevDriveConfig);

    server.on("/ReadMotorStartup1", HTTP_GET, ReadMotorStartup1);
    server.on("/WriteMotorStartup1", HTTP_POST, WriteMotorStartup1);

    server.on("/ReadMotorStartup2", HTTP_GET, ReadMotorStartup2);
    server.on("/WriteMotorStartup2", HTTP_POST, WriteMotorStartup2);

    server.on("/ReadClosedLoop1", HTTP_GET, ReadClosedLoop1);
    server.on("/WriteClosedLoop1", HTTP_POST, WriteClosedLoop1);

    server.on("/ReadClosedLoop2", HTTP_GET, ReadClosedLoop2);
    server.on("/WriteClosedLoop2", HTTP_POST, WriteClosedLoop2);

    server.on("/ReadClosedLoop3", HTTP_GET, ReadClosedLoop3);
    server.on("/WriteClosedLoop3", HTTP_POST, WriteClosedLoop3);

    server.on("/ReadClosedLoop4", HTTP_GET, ReadClosedLoop4);
    server.on("/WriteClosedLoop4", HTTP_POST, WriteClosedLoop4);

    server.on("/ReadPinConfig", HTTP_GET, ReadPinConfig);
    server.on("/WritePinConfig", HTTP_POST, WritePinConfig);

    server.on("/ReadAlgoCtrl1", HTTP_GET, ReadAlgoCtrl1);
    server.on("/WriteAlgoCtrl1", HTTP_POST, WriteAlgoCtrl1);

    server.on("/ReadDevCtrl", HTTP_GET, ReadDevCtrl);
    server.on("/WriteDevCtrl", HTTP_POST, WriteDevCtrl);

    server.on("/WriteEEPROM", HTTP_GET, WriteEEPROM);

    server.on("/ReadAlgorithmState", HTTP_GET, ReadAlgorithmState);

    server.on("/ReadRegister", HTTP_GET, ReadRegister);
    server.on("/WriteRegister", HTTP_POST, WriteRegister);

    server.begin();
}

void loop()
{
    server.handleClient();
}
