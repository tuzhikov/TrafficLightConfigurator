#include <QDebug>
#include "controller.h"
#include "crc.h"

#define TEST_DEBUG 0
//#define DEBUG_ANSWER

void controller::commandWriteAll()
{
    QByteArray cmd;
    cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
    cmd.append((char)7);// lengh DataGui +crc +cmd
    cmd.append((char)0);
    cmd.append(TYPE_TL);    // type
    cmd.append(CMD_TR_WRITE);   // cmd
    cmd.append(CMD_DATA);       // type cmd
    cmd.append((char*)&DataGui,sizeof(DataGui)); //data
    quint16 crc16 = CRC::Bit16( cmd );
    cmd.append((char*)&crc16,sizeof(quint16));

#if (TEST_DEBUG == 1)
    const quint16 CRC = CRC::Bit16( cmd );
    qDebug()<<"Output CRC:"<<CRC;
#endif

    if (currentInterface->open())
    {
        timer_delay_read->setInterval(currentInterface->retTimeDelay());
        currentInterface->sendDate(cmd);
        timer_delay_read->start();
        qDebug()<<"command set data: " <<cmd;
    }
    else
    {
        qDebug()<<"Error opening port.";
    }
}

void controller::commandReadAll()
{
    QByteArray cmd;
    cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
    cmd.append((char)7);     // lengh low
    cmd.append((char)0);        // lengh hi
    cmd.append(TYPE_TL);        // type
    cmd.append(CMD_TR_READ);    // cmd
    cmd.append(CMD_DATA);       // type cmd
    cmd.append((char)0);        // null byte
    cmd.append((char)0);        // null byte
    quint16 crc16 = CRC::Bit16( cmd );
    cmd.append((char*)&crc16,sizeof(crc16));

#if (TEST_DEBUG == 1)
    const quint16 CRC = CRC::Bit16( cmd );
    qDebug()<<"Output CRC:"<<CRC;
#endif

    if (currentInterface->open())
    {
        timer_delay_read->setInterval(currentInterface->retTimeDelay());
        currentInterface->sendDate(cmd);
        timer_delay_read->start();
        qDebug()<<"command get data"<<cmd;
    }
    else
    {
        qDebug()<<"Error opening port.";
    }
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
        // [leng][leng][type][cmd][subcmd][answer][data byte1]....[data byte n][crc16 low byte][crc16 hi byte]
        quint16 lengh_prefix(sizeof(kHeaderProtocol));
        char *const data = (char *const)(cmd.data()+lengh_prefix);
        const TransportProtocolRead *pask = reinterpret_cast<const TransportProtocolRead *>(data);

        if (
             ( pask->answer != 0xFF )
             &&(pask->subcmd < END_TYPE_NUMBER_COMMAND)
             &&(pask->cmd < END_TYPE_NUMBER_COMMAND_TRANSPORT)
           )
        {
            const char *p_data = (const char *)&pask->data_start;
            const quint16 lengh_data = pask->lengh - 6;

            if ( pask->cmd == CMD_TR_WRITE )
            {

                result = (this->*data_write[pask->subcmd])(p_data,lengh_data);
            }
            else if ( pask->cmd == CMD_TR_READ )
            {
                result = (this->*data_read[pask->subcmd])(p_data,lengh_data);
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

#ifdef DEBUG_ANSWER
        QByteArray retTestMessage()
        {
            QByteArray cmd;
            cmd.append((const char*)kHeaderProtocol,sizeof(kHeaderProtocol));
            cmd.append((char)8);        // lengh  datalow
            cmd.append((char)0);        // lengh hi
            cmd.append(TYPE_TL);        // type
            cmd.append(CMD_TR_READ);    // cmd
            cmd.append(CMD_DATA);       // type cmd
            cmd.append((char)0);        // answer 0 - OK FF - error
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
    qDebug()<<"Read buff: "<<temp_buff;

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
}

