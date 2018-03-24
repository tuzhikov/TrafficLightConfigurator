#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include "src/interface/link_interface.h"
#include "src/interface/comport.h"

enum TYPE_NUMBER_COMMAND_TRANSPORT{
    CMD_TR_WRITE = 0x01,
    CMD_TR_READ  = 0x02,
    END_TYPE_NUMBER_COMMAND_TRANSPORT
};

enum TYPE_NUMBER_COMMAND{
    CMD_DATA    = 0x00,
    END_TYPE_NUMBER_COMMAND
};

enum TYPE_DEVICE{
    TYPE_TL = 0x10,
    END_TYPE_DEVICE
};

struct TransportProtocolRead{
    quint8 lengh[2];
    quint8 type;
    quint8 cmd;
    quint8 subcmd;
    quint8 answer;
    quint8 data_start;
};
const quint8 kHeaderProtocol[] = { 0x55, 0xC3, 0x5A };

class controller : public QObject
{
    Q_OBJECT

public:
    enum CommandType{cmWriteAll,cmReadAll,END_TYPE_COMMAND};
    enum TYPE_INTERFACE{INTERFACE_COM,END_INTERFACE};
    explicit controller(QObject *parent = 0);
    ~controller();

private:
    struct data_gui {
        quint8 mode_slave;
        quint8 direct_number;
    }DataGui;
    QTimer *timer_delay_read;
    LinkInreface* currentInterface;
    // send command
    void commandWriteAll();
    void commandReadAll();
    typedef void (controller::*NumberCommandSend)();
    const NumberCommandSend command_send[END_TYPE_COMMAND] = {commandWriteAll,commandReadAll};
    RET_ANSWER parserReceivedPacket(const QByteArray &);
    RET_ANSWER checksumReceived(const QByteArray &);
    // read command
    // answer write
    RET_ANSWER commandDataWriteOk( const char *, const int ){return SUCCESSFULLY;}
    // answer read
    RET_ANSWER commandDataReadOk( const char *p, const int lengh)
    {
        const int cound_data = sizeof(data_gui);
        if (cound_data == lengh)
        {
            quint8 mode_slave = *p++;
            quint8 direct_number = *p;
            setDataProtocol( mode_slave, direct_number );
            return SUCCESSFULLY;
        }
        return ERROR_ANSWER;
    }
    typedef RET_ANSWER (controller::*NumberCommandRead)(const char *, const int);

    const NumberCommandRead data_write[END_TYPE_NUMBER_COMMAND] = {commandDataWriteOk};

    const NumberCommandRead data_read[END_TYPE_NUMBER_COMMAND] = {commandDataReadOk};

public slots:
    void commandRun(const int );
    void setParametrs(const QStringList &);
    void setDataProtocol(const quint8 mode_slave,const quint8 direct_number)
    {
        DataGui.mode_slave = mode_slave;
        DataGui.direct_number = direct_number;
    }
    void getDataProtocol(quint8 &mode_slave, quint8 &direct_number)
    {
        mode_slave = DataGui.mode_slave;
        direct_number = DataGui.direct_number;
    }

private slots:
    void onTimerHandle(void);

signals:
    void signalMessageOk(const QString&);
    void signalMessageError(const QString&);
};

#endif // CONTROLLER_H

