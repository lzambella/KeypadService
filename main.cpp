/**
  * Luke Zambella
  * Test code for connecting to our device
  */

#include <QCoreApplication>
#include <QtBluetooth>
#include <QObject>
#include "creativekeypad.h"
#include "messagehandler.h"

void addDevice (const QBluetoothDeviceInfo &device);
void connectDevice();
void connected();
void serviceDiscovery(const QBluetoothUuid &gatt);
void getData(const QLowEnergyCharacteristic &c, const QByteArray &val);
void serviceStateChanged(QLowEnergyService::ServiceState s);
void serviceScanDone();

// UART service
QUuid serviceUuid = QUuid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
// This is RXD on the device, this app receives
QUuid receiveUuid = QUuid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");


QBluetoothDeviceInfo * creativeKeypad = NULL;
QLowEnergyController * connection = NULL;
QLowEnergyService * keypadService = NULL;

// Whether the custom service has been found
bool serviceFound = false;

// UUID for custom GATT is
// Base: 3a1f8bb9-7c68-4beb-b15b-4055d81bf815
// Payload: 3a1f0001-7c68-4beb-b15b-4055d81bf815 or 0x0001
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CreativeKeypad * keypad = new CreativeKeypad();
    MessageHandler * handler = new MessageHandler();
    QObject::connect(keypad, &CreativeKeypad::messageReveiced, handler, &MessageHandler::handleMessage);

    return a.exec();
}
