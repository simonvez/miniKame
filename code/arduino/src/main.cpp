#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>
#include "minikame.h"

// Wifi Access Point configuration
const char* ssid = "vzkame";
const char* password = "kickass.sim";

MiniKame robot;
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket on port 81
ESP8266WebServer server(80);                        // HTTP server on port 80
bool running = 0;
String input;

// Forward declarations
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void parseData(String data);

void setup() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    
    Serial.begin(115200);
    
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    
    // HTTP server routes
    server.on("/", HTTP_GET, []() {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            server.send(404, "text/plain", "File not found");
            return;
        }
        server.sendHeader("Content-Type", "text/html; charset=UTF-8");
        server.streamFile(file, "text/html");
        file.close();
    });

    // Handle all other URLs as potential commands
    server.onNotFound([]() {
        String uri = server.uri();
        // Check if it matches our command pattern (number followed by +)
        if (uri.length() > 2 && uri[0] == '/' && uri[uri.length()-1] == '+') {
            String command = uri.substring(1, uri.length()-1);  // Remove / and +
            if (command.toInt() > 0) {  // Valid number
                input = command;
                parseData(input);
                server.send(200, "text/plain", "Command received");
                return;
            }
        }
        server.send(404, "text/plain", "Not found");
    });
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    server.begin();
    
    robot.init();
}

void loop() {
    webSocket.loop();
    server.handleClient();  // Handle HTTP requests
    
    // Handle robot movement
    if (running) {
        parseData(input);
    } else if (input != "") {
        robot.home();
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
            }
            break;
            
        case WStype_TEXT:
            input = String((char*)payload);
            parseData(input);
            break;
    }
}

void parseData(String data){

    switch (data.toInt()){

        case 1: // Up - Forward
            robot.walk(1,350);
            running = 1;
            break;

        case 2: // Down - Moonwalk
            robot.moonwalkL(1,350);  // Using moonwalk instead of backward walk
            running = 1;
            break;

        case 3: // Left
            robot.turnL(1,350);
            running = 1;
            break;

        case 4: // Right
            robot.turnR(1,350);
            running = 1;
            break;

        case 5: // STOP
            running = 0;

            break;

        case 6: // heart
            robot.pushUp(2,2000);
            break;

        case 7: // fire
            robot.upDown(4,250);
            break;

        case 8: // skull
            robot.jump();
            break;

        case 9: // cross
            robot.hello();
            break;

        case 10: // punch
            robot.frontBack(4,200);
            break;

        case 11: // mask
            robot.dance(2,1000);
            break;

        case 12: // moonwalk
            robot.moonwalkL(1,4000);
            running = 1;
            break;

        case 13: // run
            robot.run(1,350);  // Using faster period than walk
            running = 1;
            break;

        case 14: // spin dance
            robot.spinDance(2,1000);  // Do 2 spins
            break;

        default:
            robot.home();
            break;
    }
}
