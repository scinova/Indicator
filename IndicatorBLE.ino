#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>
#include "Indicator.h"

#define FRONT_TOTAL_PIXELS 72
#define FRONT_SIDE_PIXELS 15
#define REAR_TOTAL_PIXELS 72
#define REAR_SIDE_PIXELS 15

#define FRONT_NEOPIXEL_PIN 12
#define REAR_NEOPIXEL_PIN 13
//#define HIGHBEAM_PIN -1
//#define LOWBEAM_PIN -1
//#define VISIBILITY_PIN -1
//#define BRAKE_PIN -1
//#define REVERSE_PIN -1
//#define LEFT_PIN -1
//#define RIGHT_PIN -1
//#define HAZARD_PIN -1
#define LED_BUILTIN 2

#define COMMAND_UUID "f9a4c5f3-e63e-426a-abbc-cf9fcce71a5a"
#define SERVICE_UUID "f725ca05-413c-4352-9e90-bdd8b917c3d0"
#define STATUS_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

uint32_t timestamp;

BLEServer *server;
BLEService *service;
BLECharacteristic *commandChar;
BLECharacteristic *statusChar;

Indicator indicator;
Adafruit_NeoPixel front_pixels(FRONT_TOTAL_PIXELS, FRONT_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rear_pixels(REAR_TOTAL_PIXELS, REAR_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void notify() {
	uint8_t value = (indicator.status.visibility << 0) | (indicator.status.lowbeam << 1) | (indicator.status.highbeam << 2) |
			(indicator.status.left << 3) | (indicator.status.right << 4) | (indicator.status.hazard << 5);
	statusChar->setValue(&value, 1);
	statusChar->notify();
}

class ServerCallbacks : public BLEServerCallbacks {
	void onConnect(BLEServer * server) {
		notify();
	}
	void onDisconnect(BLEServer * server) {
	}
};

class CommandCallbacks : public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic * ch) {
		uint8_t * data = ch->getData();
		switch (data[0]) {
			case 1:
				indicator.visibility();
				break;
			case 2:
				indicator.lowbeam();
				break;
			case 3:
				indicator.highbeam();
				break;
			case 4:
				indicator.left();
				break;
			case 5:
				indicator.right();
				break;
			case 6:
				indicator.hazard();
				break;
			default:
				break;
		}
		notify();
	}
};

class StatusCallbacks : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic * ch) {
		uint8_t value = (indicator.status.visibility << 0) | (indicator.status.highbeam << 1) | (indicator.status.lowbeam << 2) |
				(indicator.status.left << 3) | (indicator.status.right << 4) | (indicator.status.hazard << 5);
		ch->setValue(&value, 1);
	}
};

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	indicator.begin(FRONT_TOTAL_PIXELS, FRONT_SIDE_PIXELS, REAR_TOTAL_PIXELS, REAR_SIDE_PIXELS);
	front_pixels.begin();
	rear_pixels.begin();
	front_pixels.setBrightness(50);
	rear_pixels.setBrightness(50);
	//return;
	BLEDevice::init("Indicator");
	server = BLEDevice::createServer();
	server->setCallbacks(new ServerCallbacks());
	service = server->createService(SERVICE_UUID);
	commandChar = service->createCharacteristic(COMMAND_UUID, BLECharacteristic::PROPERTY_WRITE);
	commandChar->addDescriptor(new BLE2902());
	commandChar->setCallbacks(new CommandCallbacks());
	statusChar = service->createCharacteristic(STATUS_UUID, BLECharacteristic::PROPERTY_READ |
			BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
	statusChar->addDescriptor(new BLE2902());
	statusChar->setCallbacks(new StatusCallbacks());
	service->start();
	BLEAdvertising *advertising = BLEDevice::getAdvertising();
	advertising->addServiceUUID(SERVICE_UUID);
	advertising->setScanResponse(true);
	BLEDevice::startAdvertising();
}

void loop() {
	indicator.loop();
	for (int i = 0; i < FRONT_TOTAL_PIXELS; i++)
		front_pixels.setPixelColor(i, indicator.getFrontPixel(i));
	for (int i = 0; i < REAR_TOTAL_PIXELS; i++)
		rear_pixels.setPixelColor(i, indicator.getRearPixel(i));
	front_pixels.show();
	rear_pixels.show();
	digitalWrite(LED_BUILTIN, millis() % 1000 < 200 ? HIGH : LOW);
	delay(10);
}
