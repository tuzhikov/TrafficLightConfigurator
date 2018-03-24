#ifndef COMPORT_H
#define COMPORT_H

#include <QtSerialPort/QSerialPort>
#include "link_interface.h"

QT_USE_NAMESPACE

class QByteArray;
class QMessageBox;

class ComPort : public LinkInreface
{
    public:
        ComPort();
        ~ComPort();
        RET_ANSWER sendDate( QByteArray &date );
        RET_ANSWER readDate(QByteArray &date);
        RET_CONNECT connectionInformation();
        bool connections(const QStringList &);
        bool close();
        bool open();
        int retTimeDelay() {return timeDelay;}
    private:
        QSerialPort *serial;
        int timeDelay;
        enum TYPE_PARAMETR_COMPORT{
            COM_NAME,
            COM_BAUDRATE,
            COM_DATABITS,
            COM_STOPBITS,
            COM_PARITY,
            COM_FLOWCONTROL,
            END_COMPORT
        };
        struct Settings
        {
            QString name;
            qint32 baudRate;
            QSerialPort::DataBits dataBits;
            QSerialPort::Parity parity;
            QSerialPort::StopBits stopBits;
            QSerialPort::FlowControl flowControl;
        };
        Settings setting;
};

Q_DECLARE_METATYPE( ComPort )

#endif // COMPORT_H
