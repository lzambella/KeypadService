#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QDebug>
#include <windows.h>
#include <WinUser.h>
#define NUM_KEYSTROKES 6
class MessageHandler : public QObject
{
    Q_OBJECT
public:
    // Organized memory
    struct hid_msg {
        char modifier;
        char k[NUM_KEYSTROKES];
    };

    explicit MessageHandler(QObject *parent = nullptr);
    /**
     * @brief Event fired when a message has been received from CreativeKeypad::messageReceived
     * Responsible for parsing raw bytes into
     * @param msg
     */
    void handleMessage(const QByteArray &msg);
    /**
     * @brief Parse raw bytes in the array to a struct
     * @param msg
     * @return
     */
    hid_msg msg2hid(QByteArray msg);

    void sendVirtualKeyEvent(hid_msg hid);
    DWORD hid2vk(char key);

signals:

};

#endif // MESSAGEHANDLER_H
