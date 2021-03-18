#include "messagehandler.h"
MessageHandler::MessageHandler(QObject *parent) : QObject(parent)
{

}

void MessageHandler::handleMessage(const QByteArray &msg) {
    qDebug("Message Received!");
    MessageHandler::hid_msg hid = msg2hid(msg);
    MessageHandler::sendVirtualKeyEvent(hid);

}

MessageHandler::hid_msg MessageHandler::msg2hid(QByteArray msg) {
    struct MessageHandler::hid_msg hid;
    qDebug() << "Message: " << msg;
    // Ignore the first two bytes of the report
    for (int i = 2; i < NUM_KEYSTROKES + 2; i++) {
        hid.k[i - 2] = msg.at(i);
    }
    return hid;
}

void MessageHandler::sendVirtualKeyEvent(MessageHandler::hid_msg hid) {
    // Check each key event when there is a change
    // This method allows for n-key rollover
    for (int i = 0; i < NUM_KEYSTROKES; i++) {
        // If zero do nothing
        if (hid.k[i] == 0x00)
            continue;

        INPUT inputs[2];
        ZeroMemory(inputs, sizeof(inputs));
        // Go through the keystroke and create a windows-compatible virtual keycode event
        // Disable 'N-key rollover' for now
        inputs[0].type = INPUT_KEYBOARD;
        DWORD VK = hid2vk(hid.k[i]);
        inputs[0].ki.wVk = VK;
        qDebug("Returned VK %x", (char) VK);
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(NUM_KEYSTROKES, inputs, sizeof(INPUT));
        qDebug() << "Sending HID inputs";
    }
}

// This should eventually be customizable
DWORD MessageHandler::hid2vk(char key) {
    qDebug("Received key: %x", key);
    switch (key) {
    case 0x27:
        return VK_NUMPAD0;
    case 0x1E:
        return VK_NUMPAD1;
    case 0x1F:
        return VK_NUMPAD2;
    case 0x20:
        return VK_NUMPAD3;
    case 0x21:
        return VK_NUMPAD4;
    case 0x22:
        return VK_NUMPAD5;
    case 0x23:
        return VK_NUMPAD6;
    case 0x24:
        return VK_NUMPAD7;
    case 0x25:
        return VK_NUMPAD8;
    case 0x26:
        return VK_NUMPAD8;
    }
    return 0x00;
}
