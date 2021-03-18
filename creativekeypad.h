#ifndef CREATIVEKEYPAD_H
#define CREATIVEKEYPAD_H

#include <QObject>
#include <QtBluetooth>
#define DEVICE_NAME "Adafruit Bluefruit LE"

class CreativeKeypad : public QObject
{
    Q_OBJECT
public:
    explicit CreativeKeypad(QObject *parent = nullptr);
private:
    void addDevice(const QBluetoothDeviceInfo &device);
    void connectDevice();
    void deviceConnected();
    void serviceDiscovery(const QBluetoothUuid &gatt);
    void getData(const QLowEnergyCharacteristic &c, const QByteArray &val);
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void serviceScanDone();
    void readCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    char str2byte(char c);

    bool serviceFound = false;
    /**
     * @brief UUID of the service to listen
     */
    QUuid serviceUuid = QUuid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    /**
     * @brief UUID of the characteristic of the service to receive data from
     */
    QUuid receiveUuid = QUuid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
    QBluetoothDeviceInfo * creativeKeypadInfo = NULL;
    QLowEnergyController * connection = NULL;
    /**
     * @brief
     * Service for the keypad device, discovers the characteristics
     *
     */
    QLowEnergyService * keypadService = NULL;
    /**
     * @brief
     * Agent that discovers actualBLE devices
     */
    QBluetoothDeviceDiscoveryAgent * agent = NULL;

    /**
     * @brief
     * Receive buffer from the device
     * holds the character data
     */
    QByteArray rxBuffer;
    bool flushBuffer = false;
    int arrTail = 0;
signals:
    /**
     * @brief Emitted when a stop character (0xFF) is received from the device. Send the contents of the
     * buffer for parsing.
     * @param msg Receiver buffer
     */

    void messageReveiced(const QByteArray msg);
};

#endif // CREATIVEKEYPAD_H
