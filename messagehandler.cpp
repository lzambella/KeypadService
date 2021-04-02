#include "messagehandler.h"
MessageHandler::MessageHandler(QObject *parent) : QObject(parent)
{
    //enabled_keys->resize(NUM_KEYSTROKES);
    enabled_keys.resize(6);
    // File streams
    QFile file("../config.dat");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open configuration file!";
    } else {
        QDataStream str(&file);
        str.startTransaction();
        str >> config;
        str.commitTransaction();
        file.close();
    }
}

void MessageHandler::handleMessage(const QByteArray &msg) {
    qDebug("Message Received!");
    qDebug() << "Message: " << msg;

    if (msg.size() != NUM_KEYSTROKES +2) {
        qDebug("Payload is not the correct size! Ignoring...");
        return;
    } else {
        MessageHandler::hid_msg hid = msg2hid(msg);
        MessageHandler::sendVirtualKeyEvent(hid);
    }

}

MessageHandler::hid_msg MessageHandler::msg2hid(QByteArray msg) {
    struct MessageHandler::hid_msg hid;

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
        // If zero, set the enabled_keys[i] to zero as well
        if (hid.k[i] == 0x00) {
            enabled_keys[i] = 0x00;
         } else if (hid.k[i] == enabled_keys.at(i))
            continue;

        INPUT inputs[2];
        ZeroMemory(inputs, sizeof(inputs));
        // Go through the keystroke and create a windows-compatible virtual keycode event
        // Disable 'N-key rollover' for now
        inputs[0].type = INPUT_KEYBOARD;
        DWORD VK = hid2vk(hid.k[i]);
        inputs[0].ki.wVk = VK;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(NUM_KEYSTROKES, inputs, sizeof(INPUT));
        char c = hid.k[i];
        enabled_keys[i] = c;
    }
}

// This should eventually be customizable with a struct containing
// any modifiers and keys
DWORD MessageHandler::hid2vk(char key) {
    //qDebug("Received key: %x", key);
    switch (key) {
    case 0x27:
        return this->config.num_pad[0].virtual_key;
    case 0x1E:
        return this->config.num_pad[1].virtual_key;
    case 0x1F:
        return this->config.num_pad[2].virtual_key;
    case 0x20:
        return this->config.num_pad[3].virtual_key;
    case 0x21:
        return this->config.num_pad[4].virtual_key;
    case 0x22:
        return this->config.num_pad[5].virtual_key;
    case 0x23:
        return this->config.num_pad[6].virtual_key;
    case 0x24:
        return this->config.num_pad[7].virtual_key;
    case 0x25:
        return this->config.num_pad[8].virtual_key;
    case 0x26:
        return this->config.num_pad[9].virtual_key;
    case 0x04:				// Encoder A Left
        return this->config.encoder_a[1].virtual_key;
    case 0x05:				// Encoder A Right
        return this->config.encoder_a[0].virtual_key;
    case 0x06:
        return this->config.encoder_b[1].virtual_key;
    case 0x07:
        return this->config.encoder_b[0].virtual_key;
    case 0x08:
        return this->config.encoder_c[1].virtual_key;
    case 0x09:
        return this->config.encoder_c[0].virtual_key;
    case 0x0A:
        return this->config.encoder_d[1].virtual_key;
    case 0x0B:
        return this->config.encoder_d[0].virtual_key;
    case 0x0C:
        return this->config.encoder_e[1].virtual_key;
    case 0x0D:
        return this->config.encoder_e[0].virtual_key;
    case 0x0E:
        return this->config.encoder_f[1].virtual_key;
    case 0x0F:
        return this->config.encoder_f[0].virtual_key;
    }
    return 0x00;
}
