// MCF8316A GUI
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

    <div class="container collapsed" id="ms1Container">
        <!-- Tıklanabilir Başlık -->
        <div class="header" onclick="toggleContainer('ms1Container', 'ms1Content', 'ms1Arrow')">
            MOTOR_STARTUP1
            <span class="arrow" id="ms1Arrow">▼</span>
        </div>

        <!-- İçerik -->
        <div class="content" id="ms1Content" style="display: none;">
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

                // Açılır kutulara da aynı verileri yazalım
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


    </script>

</body>
</html>



)rawliteral";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#define ALGO_CTRL1_REG 0x000000EA
#define INT_ALGO_1_REG 0x000000A0
#define INT_ALGO_2_REG 0x000000A2


unsigned long register_value = 0;

#define READ_BITS(value, high, low) ((value >> low) & ((1 << (high - low + 1)) - 1))
#define WRITE_BITS(target, value, high, low) (target |= ((value & ((1 << (high - low + 1)) - 1)) << low))

struct ISDCONFIG
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

struct REVDRIVECONFIG
{
    byte REV_DRV_OPEN_LOOP_ACCEL_A1;
    byte REV_DRV_OPEN_LOOP_ACCEL_A2;
    byte ACTIVE_BRAKE_CURRENT_LIMIT;
    byte ACTIVE_BRAKE_KP;
    byte ACTIVE_BRAKE_KI;
};

struct MOTORSTARTUP1
{
    byte MTR_STARTUP;
    byte ALIGN_SLOW_RAMP_RATE;
    byte ALIGN_TIME;
    byte ALIGN_OR_SLOW_CURRENT_ILIMIT;
    byte IPD_CLK_FREQ;
    byte IPD_CURR_THR;
    byte IPD_RLS_MODE;
    byte IPD_ADV_ANGLE;
    byte IPD_REPEAT;
    byte OL_ILIMIT_CONFIG;
    byte IQ_RAMP_EN;
    byte ACTIVE_BRAKE_EN;
    byte REV_DRV_CONFIG;
};

ISDCONFIG ISD_CONFIG;
REVDRIVECONFIG REV_DRIVE_CONFIG;
MOTORSTARTUP1 MOTOR_STARTUP1;

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
    read32(ISD_CONFIG_REG);

    ISD_CONFIG.ISD_EN = READ_BITS(register_value, 30, 30);
    ISD_CONFIG.BRAKE_EN = READ_BITS(register_value, 29, 29);
    ISD_CONFIG.HIZ_EN = READ_BITS(register_value, 28, 28);
    ISD_CONFIG.RVS_DR_EN = READ_BITS(register_value, 27, 27);
    ISD_CONFIG.RESYNC_EN = READ_BITS(register_value, 26, 26);
    ISD_CONFIG.FW_DRV_RESYN_THR = READ_BITS(register_value, 25, 22);
    ISD_CONFIG.BRK_MODE = READ_BITS(register_value, 21, 21);
    ISD_CONFIG.BRK_TIME = READ_BITS(register_value, 16, 13);
    ISD_CONFIG.HIZ_TIME = READ_BITS(register_value, 12, 9);
    ISD_CONFIG.STAT_DETECT_THR = READ_BITS(register_value, 8, 6);
    ISD_CONFIG.REV_DRV_HANDOFF_THR = READ_BITS(register_value, 5, 2);
    ISD_CONFIG.REV_DRV_OPEN_LOOP_CURRENT = READ_BITS(register_value, 1, 0);

    String json = "{ \"ISD_EN\": \"" + toUpperCaseHex(ISD_CONFIG.ISD_EN) +
                  "\", \"BRAKE_EN\": \"" + toUpperCaseHex(ISD_CONFIG.BRAKE_EN) +
                  "\", \"HIZ_EN\": \"" + toUpperCaseHex(ISD_CONFIG.HIZ_EN) +
                  "\", \"RVS_DR_EN\": \"" + toUpperCaseHex(ISD_CONFIG.RVS_DR_EN) +
                  "\", \"RESYNC_EN\": \"" + toUpperCaseHex(ISD_CONFIG.RESYNC_EN) +
                  "\", \"FW_DRV_RESYN_THR\": \"" + toUpperCaseHex(ISD_CONFIG.FW_DRV_RESYN_THR) +
                  "\", \"BRK_MODE\": \"" + toUpperCaseHex(ISD_CONFIG.BRK_MODE) +
                  "\", \"BRK_TIME\": \"" + toUpperCaseHex(ISD_CONFIG.BRK_TIME) +
                  "\", \"HIZ_TIME\": \"" + toUpperCaseHex(ISD_CONFIG.HIZ_TIME) +
                  "\", \"STAT_DETECT_THR\": \"" + toUpperCaseHex(ISD_CONFIG.STAT_DETECT_THR) +
                  "\", \"REV_DRV_HANDOFF_THR\": \"" + toUpperCaseHex(ISD_CONFIG.REV_DRV_HANDOFF_THR) +
                  "\", \"REV_DRV_OPEN_LOOP_CURRENT\": \"" + toUpperCaseHex(ISD_CONFIG.REV_DRV_OPEN_LOOP_CURRENT) + "\" }";

    server.send(200, "application/json", json);
}

void WriteISDConfig()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");

        ISD_CONFIG.ISD_EN = getJsonHexValue(body, "ISD_EN");
        ISD_CONFIG.BRAKE_EN = getJsonHexValue(body, "BRAKE_EN");
        ISD_CONFIG.HIZ_EN = getJsonHexValue(body, "HIZ_EN");
        ISD_CONFIG.RVS_DR_EN = getJsonHexValue(body, "RVS_DR_EN");
        ISD_CONFIG.RESYNC_EN = getJsonHexValue(body, "RESYNC_EN");
        ISD_CONFIG.FW_DRV_RESYN_THR = getJsonHexValue(body, "FW_DRV_RESYN_THR");
        ISD_CONFIG.BRK_MODE = getJsonHexValue(body, "BRK_MODE");
        ISD_CONFIG.BRK_TIME = getJsonHexValue(body, "BRK_TIME");
        ISD_CONFIG.HIZ_TIME = getJsonHexValue(body, "HIZ_TIME");
        ISD_CONFIG.STAT_DETECT_THR = getJsonHexValue(body, "STAT_DETECT_THR");
        ISD_CONFIG.REV_DRV_HANDOFF_THR = getJsonHexValue(body, "REV_DRV_HANDOFF_THR");
        ISD_CONFIG.REV_DRV_OPEN_LOOP_CURRENT = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_CURRENT");

        register_value = 0;
        WRITE_BITS(register_value, 0, 31, 31);
        WRITE_BITS(register_value, ISD_CONFIG.ISD_EN, 30, 30);
        WRITE_BITS(register_value, ISD_CONFIG.BRAKE_EN, 29, 29);
        WRITE_BITS(register_value, ISD_CONFIG.HIZ_EN, 28, 28);
        WRITE_BITS(register_value, ISD_CONFIG.RVS_DR_EN, 27, 27);
        WRITE_BITS(register_value, ISD_CONFIG.RESYNC_EN, 26, 26);
        WRITE_BITS(register_value, ISD_CONFIG.FW_DRV_RESYN_THR, 25, 22);
        WRITE_BITS(register_value, ISD_CONFIG.BRK_MODE, 21, 21);
        WRITE_BITS(register_value, 9, 20, 17);
        WRITE_BITS(register_value, ISD_CONFIG.BRK_TIME, 16, 13);
        WRITE_BITS(register_value, ISD_CONFIG.HIZ_TIME, 12, 9);
        WRITE_BITS(register_value, ISD_CONFIG.STAT_DETECT_THR, 8, 6);
        WRITE_BITS(register_value, ISD_CONFIG.REV_DRV_HANDOFF_THR, 5, 2);
        WRITE_BITS(register_value, ISD_CONFIG.REV_DRV_OPEN_LOOP_CURRENT, 1, 0);

        write32(ISD_CONFIG_REG, register_value);

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
    read32(REV_DRIVE_CONFIG_REG);

    REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A1 = READ_BITS(register_value, 30, 27);
    REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A2 = READ_BITS(register_value, 26, 23);
    REV_DRIVE_CONFIG.ACTIVE_BRAKE_CURRENT_LIMIT = READ_BITS(register_value, 22, 20);
    REV_DRIVE_CONFIG.ACTIVE_BRAKE_KP = READ_BITS(register_value, 19, 10);
    REV_DRIVE_CONFIG.ACTIVE_BRAKE_KI = READ_BITS(register_value, 9, 0);

    String json = "{ \"REV_DRV_OPEN_LOOP_ACCEL_A1\": \"" + toUpperCaseHex(REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A1) +
                  "\", \"REV_DRV_OPEN_LOOP_ACCEL_A2\": \"" + toUpperCaseHex(REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A2) +
                  "\", \"ACTIVE_BRAKE_CURRENT_LIMIT\": \"" + toUpperCaseHex(REV_DRIVE_CONFIG.ACTIVE_BRAKE_CURRENT_LIMIT) +
                  "\", \"ACTIVE_BRAKE_KP\": \"" + toUpperCaseHex(REV_DRIVE_CONFIG.ACTIVE_BRAKE_KP) +
                  "\", \"ACTIVE_BRAKE_KI\": \"" + toUpperCaseHex(REV_DRIVE_CONFIG.ACTIVE_BRAKE_KI) + "\" }";

    server.send(200, "application/json", json);
}

void WriteRevDriveConfig()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");

        REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A1 = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_ACCEL_A1");
        REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A2 = getJsonHexValue(body, "REV_DRV_OPEN_LOOP_ACCEL_A2");
        REV_DRIVE_CONFIG.ACTIVE_BRAKE_CURRENT_LIMIT = getJsonHexValue(body, "ACTIVE_BRAKE_CURRENT_LIMIT");
        REV_DRIVE_CONFIG.ACTIVE_BRAKE_KP = getJsonHexValue(body, "ACTIVE_BRAKE_KP");
        REV_DRIVE_CONFIG.ACTIVE_BRAKE_KI = getJsonHexValue(body, "ACTIVE_BRAKE_KI");

        register_value = 0;
        WRITE_BITS(register_value, 0, 31, 31);
        WRITE_BITS(register_value, REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A1, 30, 27);
        WRITE_BITS(register_value, REV_DRIVE_CONFIG.REV_DRV_OPEN_LOOP_ACCEL_A2, 26, 23);
        WRITE_BITS(register_value, REV_DRIVE_CONFIG.ACTIVE_BRAKE_CURRENT_LIMIT, 22, 20);
        WRITE_BITS(register_value, REV_DRIVE_CONFIG.ACTIVE_BRAKE_KP, 19, 10);
        WRITE_BITS(register_value, REV_DRIVE_CONFIG.ACTIVE_BRAKE_KI, 9, 0);

        write32(REV_DRIVE_CONFIG_REG, register_value);

        server.send(200, "application/json", "{\"status\":\"success\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
    }
}

void ReadMotorStartup1()
{
    register_value = 0;
    read32(MOTOR_STARTUP1_REG);

    MOTOR_STARTUP1.MTR_STARTUP = READ_BITS(register_value, 30, 29);
    MOTOR_STARTUP1.ALIGN_SLOW_RAMP_RATE = READ_BITS(register_value, 28, 25);
    MOTOR_STARTUP1.ALIGN_TIME = READ_BITS(register_value, 24, 21);
    MOTOR_STARTUP1.ALIGN_OR_SLOW_CURRENT_ILIMIT = READ_BITS(register_value, 20, 17);
    MOTOR_STARTUP1.IPD_CLK_FREQ = READ_BITS(register_value, 16, 14);
    MOTOR_STARTUP1.IPD_CURR_THR = READ_BITS(register_value, 13, 9);
    MOTOR_STARTUP1.IPD_RLS_MODE = READ_BITS(register_value, 8, 8);
    MOTOR_STARTUP1.IPD_ADV_ANGLE = READ_BITS(register_value, 7, 6);
    MOTOR_STARTUP1.IPD_REPEAT = READ_BITS(register_value, 5, 4);
    MOTOR_STARTUP1.OL_ILIMIT_CONFIG = READ_BITS(register_value, 3, 3);
    MOTOR_STARTUP1.IQ_RAMP_EN = READ_BITS(register_value, 2, 2);
    MOTOR_STARTUP1.ACTIVE_BRAKE_EN = READ_BITS(register_value, 1, 1);
    MOTOR_STARTUP1.REV_DRV_CONFIG = READ_BITS(register_value, 0, 0);

    String json = "{ \"MTR_STARTUP\": \"" + toUpperCaseHex(MOTOR_STARTUP1.MTR_STARTUP) +
                  "\", \"ALIGN_SLOW_RAMP_RATE\": \"" + toUpperCaseHex(MOTOR_STARTUP1.ALIGN_SLOW_RAMP_RATE) +
                  "\", \"ALIGN_TIME\": \"" + toUpperCaseHex(MOTOR_STARTUP1.ALIGN_TIME) +
                  "\", \"ALIGN_OR_SLOW_CURRENT_ILIMIT\": \"" + toUpperCaseHex(MOTOR_STARTUP1.ALIGN_OR_SLOW_CURRENT_ILIMIT) +
                  "\", \"IPD_CLK_FREQ\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IPD_CLK_FREQ) +
                  "\", \"IPD_CURR_THR\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IPD_CURR_THR) +
                  "\", \"IPD_RLS_MODE\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IPD_RLS_MODE) +
                  "\", \"IPD_ADV_ANGLE\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IPD_ADV_ANGLE) +
                  "\", \"IPD_REPEAT\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IPD_REPEAT) +
                  "\", \"OL_ILIMIT_CONFIG\": \"" + toUpperCaseHex(MOTOR_STARTUP1.OL_ILIMIT_CONFIG) +
                  "\", \"IQ_RAMP_EN\": \"" + toUpperCaseHex(MOTOR_STARTUP1.IQ_RAMP_EN) +
                  "\", \"ACTIVE_BRAKE_EN\": \"" + toUpperCaseHex(MOTOR_STARTUP1.ACTIVE_BRAKE_EN) +
                  "\", \"REV_DRV_CONFIG\": \"" + toUpperCaseHex(MOTOR_STARTUP1.REV_DRV_CONFIG) + "\" }";

    server.send(200, "application/json", json);
}

void WriteMotorStartup1()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");

        MOTOR_STARTUP1.MTR_STARTUP = getJsonHexValue(body, "MTR_STARTUP");
        MOTOR_STARTUP1.ALIGN_SLOW_RAMP_RATE = getJsonHexValue(body, "ALIGN_SLOW_RAMP_RATE");
        MOTOR_STARTUP1.ALIGN_TIME = getJsonHexValue(body, "ALIGN_TIME");
        MOTOR_STARTUP1.ALIGN_OR_SLOW_CURRENT_ILIMIT = getJsonHexValue(body, "ALIGN_OR_SLOW_CURRENT_ILIMIT");
        MOTOR_STARTUP1.IPD_CLK_FREQ = getJsonHexValue(body, "IPD_CLK_FREQ");
        MOTOR_STARTUP1.IPD_CURR_THR = getJsonHexValue(body, "IPD_CURR_THR");
        MOTOR_STARTUP1.IPD_RLS_MODE = getJsonHexValue(body, "IPD_RLS_MODE");
        MOTOR_STARTUP1.IPD_ADV_ANGLE = getJsonHexValue(body, "IPD_ADV_ANGLE");
        MOTOR_STARTUP1.IPD_REPEAT = getJsonHexValue(body, "IPD_REPEAT");
        MOTOR_STARTUP1.OL_ILIMIT_CONFIG = getJsonHexValue(body, "OL_ILIMIT_CONFIG");
        MOTOR_STARTUP1.IQ_RAMP_EN = getJsonHexValue(body, "IQ_RAMP_EN");
        MOTOR_STARTUP1.ACTIVE_BRAKE_EN = getJsonHexValue(body, "ACTIVE_BRAKE_EN");
        MOTOR_STARTUP1.REV_DRV_CONFIG = getJsonHexValue(body, "REV_DRV_CONFIG");

        register_value = 0;
        WRITE_BITS(register_value, 0, 31, 31);
        WRITE_BITS(register_value, MOTOR_STARTUP1.MTR_STARTUP, 30, 29);
        WRITE_BITS(register_value, MOTOR_STARTUP1.ALIGN_SLOW_RAMP_RATE, 28, 25);
        WRITE_BITS(register_value, MOTOR_STARTUP1.ALIGN_TIME, 24, 21);
        WRITE_BITS(register_value, MOTOR_STARTUP1.ALIGN_OR_SLOW_CURRENT_ILIMIT, 20, 17);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IPD_CLK_FREQ, 16, 14);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IPD_CURR_THR, 13, 9);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IPD_RLS_MODE, 8, 8);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IPD_ADV_ANGLE, 7, 6);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IPD_REPEAT, 5, 4);
        WRITE_BITS(register_value, MOTOR_STARTUP1.OL_ILIMIT_CONFIG, 3, 3);
        WRITE_BITS(register_value, MOTOR_STARTUP1.IQ_RAMP_EN, 2, 2);
        WRITE_BITS(register_value, MOTOR_STARTUP1.ACTIVE_BRAKE_EN, 1, 1);
        WRITE_BITS(register_value, MOTOR_STARTUP1.REV_DRV_CONFIG, 0, 0);

        write32(MOTOR_STARTUP1_REG, register_value);

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
    server.on("/ReadMotorStartup1", HTTP_GET, ReadMotorStartup1);
    server.on("/WriteMotorStartup1", HTTP_POST, WriteMotorStartup1);
    server.begin();
}

void loop()
{
    server.handleClient();
}
