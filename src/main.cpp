/* !!!!! change these & don't commit !!!!! */
const char* wifi1_ssid = "ssid";
const char* wifi1_pass = "pass";

#include <Arduino.h>

#define GDBSTUB_BREAK_ON_INIT 1
#include <GDBStub.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 10000;

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// ATTENTION
// when enabled throw the error in gdb
#ifndef ARDUINOJSON_USE_DOUBLE
	#define ARDUINOJSON_USE_DOUBLE 1
#endif
#include <ArduinoJson.h>

void basicQuery() {
	HTTPClient https;
	WiFiClientSecure client;

	client.setInsecure();
	client.setTimeout(3000);
	https.useHTTP10(true);

	https.begin(client, "www.reddit.com", 443, "/r/arduino/.json", true);
	int http_code = https.GET();
	if(http_code > 0) {
		Serial.printf("return code: %d\n", http_code);
		StaticJsonDocument<4096> jdoc;
		DeserializationError err = deserializeJson(jdoc, https.getStream());
		if(err) {
			Serial.println("deserializeJson failed.");
		}
		else {
			Serial.println("done");
			// serializeJson(jdoc, Serial);
		}
	}
	else {
		Serial.println("http code != 200");
	}
	https.end();
}

void setup() {
	Serial.begin(115200);
	gdbstub_init();
	Serial.printf("Starting...\n");

	wifiMulti.addAP(wifi1_ssid, wifi1_pass);
}

void loop() {
	static uint32_t cnt = 0;
	Serial.printf("%d\n", cnt++);

	if(wifiMulti.run(connectTimeoutMs) != WL_CONNECTED)	{
		Serial.println("Not connected");
		delay(500);
		return;
	}
	Serial.printf("Connected to %s with ip addr %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	basicQuery();

	delay(2000);
}
