#include "creativekeypad.h"

CreativeKeypad::CreativeKeypad(QObject *parent) : QObject(parent)
{
    agent = new QBluetoothDeviceDiscoveryAgent();
    agent->setLowEnergyDiscoveryTimeout(5000);
    connect(agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &CreativeKeypad::addDevice);
    connect(agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &CreativeKeypad::connectDevice);
    agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}


void CreativeKeypad::addDevice(const QBluetoothDeviceInfo &device) {
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qDebug("Found an LE device\n");
        if (strcmp(DEVICE_NAME, device.name().toUtf8()) == 0) {
            qDebug("Found target Device\n");
            creativeKeypadInfo = new QBluetoothDeviceInfo(device);
            // Free memory
            //delete(agent);
        }
    }
}

void CreativeKeypad::connectDevice() {
    //delete(agent);
    qDebug("Connecting to device");
    if (creativeKeypadInfo == NULL) {
        qDebug("Target device was not found!\n");
        // restart the disovery process
        agent->start();
    }
    delete(agent);
    qDebug("Connecting to device\n");
    connection = new QLowEnergyController(*creativeKeypadInfo);
    // Set up connections
    connect(connection, &QLowEnergyController::connected, this, &CreativeKeypad::deviceConnected);
    connect(connection, &QLowEnergyController::serviceDiscovered, this, &CreativeKeypad::serviceDiscovery);
    //connect(connection, &QLowEnergyController::characteristicChanged, this, &CreativeKeypad::readCharacteristic);
    connect(connection, &QLowEnergyController::discoveryFinished, this, &CreativeKeypad::serviceScanDone, Qt::QueuedConnection);
    connection->connectToDevice();
}

void CreativeKeypad::deviceConnected() {
    qDebug("Connected to the device.");
    CreativeKeypad::connection->discoverServices();
}

void CreativeKeypad::serviceDiscovery(const QBluetoothUuid &gatt) {
    if (gatt == QBluetoothUuid(CreativeKeypad::serviceUuid))
        CreativeKeypad::serviceFound = true;
}

void CreativeKeypad::serviceScanDone() {
    if (!serviceFound)
        return;

    qDebug("Found the service\n");
    CreativeKeypad::keypadService = connection->createServiceObject(QBluetoothUuid(serviceUuid));
    //connect(keypadService, &QLowEnergyService::characteristicChanged, this, &CreativeKeypad::getData);
    connect(keypadService, &QLowEnergyService::stateChanged, this, &CreativeKeypad::serviceStateChanged);
    connect(keypadService, &QLowEnergyService::characteristicWritten, this, &CreativeKeypad::getData);
    connect(keypadService, &QLowEnergyService::characteristicChanged, this, &CreativeKeypad::readCharacteristic);
    keypadService->discoverDetails();
}

void CreativeKeypad::getData(const QLowEnergyCharacteristic &c, const QByteArray &val) {
    qDebug("State Changed!\n");
}

void CreativeKeypad::serviceStateChanged(QLowEnergyService::ServiceState s) {
    qDebug("ServiceStateChanged");
    if (s == QLowEnergyService::ServiceDiscovered) {
        qDebug("A new service was discovered!");
        foreach (QLowEnergyCharacteristic c, keypadService->characteristics()) {
            QLowEnergyDescriptor d = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (c.properties() & QLowEnergyCharacteristic::Notify) {
                qDebug("Notify");
                keypadService->writeDescriptor(d, QByteArray::fromHex("0100"));
            }
            if (c.properties() & QLowEnergyCharacteristic::Indicate) {
                qDebug("indicate");
                keypadService->writeDescriptor(d, QByteArray::fromHex("0200"));
            }
        }
    }
}
void CreativeKeypad::readCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
    // Add each character to the buffer
    // If the character is a stop character (0xff), then emit a copy of that message only to the signal
    // Then remove everything before that stop character
    // The payload is bytes but as a string, parse those to pure bytes so "00" becomes 0x00
    int i;
    for (i = 0; i < value.size(); i++) {
        // Check if the index overflows
        if (rxBuffer.size() > 100) {
            rxBuffer.clear();
            return;
        }
        if (value[i] == (char) 0xff) {
            // emit signal and clear the buffer
            emit CreativeKeypad::messageReveiced(rxBuffer);
            rxBuffer.clear();
        } else {
            // append the received bytes to a buffer
            rxBuffer.append(value[i]);
        }
    }

}

