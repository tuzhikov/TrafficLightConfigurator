#include <QDebug>
#include "controller.h"
#include "crc.h"

void controller::commandWriteAll()
{
    const size_t lengh(sizeof(DataGui));
    QByteArray cmd;
    cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
    cmd.append((uint8_t)3+(uint8_t)lengh);// lengh DataGui +crc +cmd
    cmd.append((char)0);
    cmd.append(TYPE_TL);    // type
    cmd.append(CMD_TR_WRITE);   // cmd
    cmd.append(CMD_DATA);       // type cmd
    cmd.append((const char*)&DataGui,lengh); //data
    quint16 crc16 = CRC::Bit16( cmd );
    cmd.append((char*)&crc16,sizeof(crc16));

    if (currentInterface->open())
    {
        timer_delay_read->setInterval(currentInterface->retTimeDelay());
        currentInterface->sendDate(cmd);
        timer_delay_read->start();
    }
    qDebug()<<"command set data: " <<cmd;
}

void controller::commandReadAll()
{
    QByteArray cmd;
    cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
    cmd.append((uint8_t)3);     // lengh low
    cmd.append((char)0);        // lengh hi
    cmd.append(TYPE_TL);        // type
    cmd.append(CMD_TR_READ);    // cmd
    cmd.append(CMD_DATA);       // type cmd
    quint16 crc16 = CRC::Bit16( cmd );
    cmd.append((char*)&crc16,sizeof(crc16));
    if (currentInterface->open())
    {
        timer_delay_read->setInterval(currentInterface->retTimeDelay());
        currentInterface->sendDate(cmd);
        timer_delay_read->start();
    }
    qDebug()<<"command get data"<<cmd;
}
/**
 * @brief controller::parserReceivedPacket
 * @return
 */
RET_ANSWER controller::parserReceivedPacket(const QByteArray &cmd)
{
    RET_ANSWER ret = NO_ANSWER;
    QByteArray readBuffer(cmd);
    const QByteArray HEAD((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));

    const int index = readBuffer.indexOf(HEAD);
    if ( index !=-1 )
    {
        return checksumReceived(readBuffer.remove(0,index));
    }

    return ret;
}
/**
 * @brief controller::checksumReceived
 * @param cmd
 * @return
 */
RET_ANSWER controller::checksumReceived(const QByteArray &cmd)
{
    RET_ANSWER result = ERROR_ANSWER;

    if ( !CRC::Bit16(cmd) ) // CRC ok
    {
        // [leng][leng][type][cmd][subcmd][answer][data]
        quint16 lengh_prefix(sizeof(kHeaderProtocol));
        const TransportProtocolRead *pask((TransportProtocolRead*)(cmd.data()+lengh_prefix));
        if ( ( pask->answer != 0xFF )
             &&(pask->subcmd < END_TYPE_NUMBER_COMMAND)
             &&(pask->cmd < END_TYPE_NUMBER_COMMAND_TRANSPORT)
            )
        {
            const char *p_data = (const char *)&pask->data_start;
            if ( pask->cmd == CMD_TR_WRITE )
            {

                result= (this->*data_write[pask->subcmd])(p_data,pask->lengh[0]);
            }
            else if ( pask->cmd == CMD_TR_READ )
            {
                result= (this->*data_read[pask->subcmd])(p_data,pask->lengh[0]);
            }
        }
    }

    return result;
}
/**
 * @brief controller::controller
 * @param parent
 */
controller::controller(QObject *parent) :
                QObject(parent)
                ,timer_delay_read(new QTimer)
                ,currentInterface(new ComPort())
{

    connect(timer_delay_read,SIGNAL(timeout()),this,SLOT(onTimerHandle()),Qt::DirectConnection);
}

controller::~controller()
{
    currentInterface->close();
    delete timer_delay_read;
    delete currentInterface;
}

/**
 * @brief controller::commandRun
 * @param cmd
 */
void controller::commandRun(const int cmd)
{
    if ( cmd<CommandType::END_TYPE_COMMAND )
    {
        (this->*command_send[cmd])();
    }
}

void controller::setParametrs(const QStringList &list)
{
    currentInterface->connections(list);
    qDebug()<<list;
}

//#define DEBUG_ANSWER

#ifdef DEBUG_ANSWER
        QByteArray retTestMessage()
        {
            QByteArray cmd;
            cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
            cmd.append((uint8_t)2);     // lengh  datalow
            cmd.append((char)0);        // lengh hi
            cmd.append(TYPE_TL);        // type
            cmd.append(CMD_TR_READ);    // cmd
            cmd.append(CMD_DATA);       // type cmd
            cmd.append((uint8_t)0x01);     // answer 0 - OK FF - error
            cmd.append((uint8_t)1);     //
            cmd.append((uint8_t)7);     //
            quint16 crc16 = CRC::Bit16( cmd );
            cmd.append((char*)&crc16,sizeof(crc16));
            cmd.prepend((uint8_t)7);
            cmd.prepend((uint8_t)7);
            cmd.prepend((uint8_t)7);
            cmd.prepend((uint8_t)7);
            cmd.prepend((uint8_t)7);
            return cmd;
        }
#endif
/**
 * @brief controller::onTimerHandle
 */
void controller::onTimerHandle()
{
    QByteArray temp_buff;
    currentInterface->readDate(temp_buff);
    currentInterface->close();
    timer_delay_read->stop();
#ifdef DEBUG_ANSWER
    temp_buff.append(retTestMessage());
#endif
    const RET_ANSWER ret = parserReceivedPacket(temp_buff);
    if (ret == SUCCESSFULLY)
    {
        emit signalMessageOk(tr("<CENTER><b>Successfully command </CENTER></b>"));
    }
    else if ( ret == NO_ANSWER)
    {
        emit signalMessageError(tr("<CENTER><b> No answer </CENTER></b>"));
    }
    else if (ret == ERROR_ANSWER)
    {
        emit signalMessageError(tr("<CENTER><b> Data error </CENTER></b>"));
    }

    qDebug()<<"Read buff: "<<temp_buff;
}

