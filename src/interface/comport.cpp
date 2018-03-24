#include "comport.h"

/**
 * @brief ComPort::ComPort
 */
ComPort::ComPort()
{
    serial = new QSerialPort;
    // default parametr
    setting.name = "COM1";
    setting.baudRate = QSerialPort::Baud115200;
    setting.dataBits = QSerialPort::Data8;
    setting.parity = QSerialPort::NoParity;
    setting.stopBits = QSerialPort::OneStop;
    setting.flowControl = QSerialPort::NoFlowControl;
    timeDelay = 200;// ms
}
/**
 * @brief ComPort::~ComPort
 */
ComPort::~ComPort()
{
    delete serial;
}
/**
 * @brief ComPort::sendDate
 * @param date
 * @return
 */
RET_ANSWER ComPort::sendDate(QByteArray &date)
{
    RET_ANSWER ret(RET_ANSWER::SUCCESSFULLY);
    serial->write(date);
    return ret;
}
/**
 * @brief ComPort::readDate
 * @param date
 * @return
 */
RET_ANSWER ComPort::readDate(QByteArray &date)
{
    RET_ANSWER ret(RET_ANSWER::ERROR_ANSWER);
    const int count = serial->readChannelCount();
    if ( count )
    {
        date.append(serial->readAll());
        ret = RET_ANSWER::SUCCESSFULLY;
    }
    return ret;
}
/**
 * @brief ComPort::connectionInformation
 * @return
 */
RET_CONNECT ComPort::connectionInformation()
{
    RET_CONNECT ret(RET_CONNECT::CONNECT_NO);

    return ret;
}
/**
 * @brief ComPort::connections
 * @param list
 * @return
 */
bool ComPort::connections(const QStringList &list)
{
    bool returnStatus = false;
    if ( list.count() >= static_cast<int>(END_COMPORT) )
    {
        setting.name = list.at(COM_NAME);
        setting.baudRate = static_cast<QSerialPort::BaudRate>(list.at(COM_BAUDRATE).toInt());
        setting.dataBits = static_cast<QSerialPort::DataBits>(list.at(COM_DATABITS).toInt());
        setting.stopBits = static_cast<QSerialPort::StopBits>(list.at(COM_STOPBITS).toInt());
        setting.parity = static_cast<QSerialPort::Parity>(list.at(COM_PARITY).toInt());
        setting.flowControl = static_cast<QSerialPort::FlowControl>(list.at(COM_FLOWCONTROL).toInt());
        returnStatus = true;
    }
    return returnStatus;
}
/**
 * @brief ComPort::closePort
 * @return
 */
bool ComPort::close()
{
    if (serial->isOpen())
        serial->close();
    return true;
}
/**
 * @brief ComPort::open
 * @return
 */
bool ComPort::open()
{
    bool returnStatus = false;
    if ( !setting.name.isEmpty() )
    {
        serial->setPortName(setting.name);
        serial->setBaudRate(setting.baudRate);
        serial->setDataBits(setting.dataBits);
        serial->setParity(setting.parity);
        serial->setStopBits(setting.stopBits);
        serial->setFlowControl(setting.flowControl);

        if (serial->open(QIODevice::ReadWrite))
        {
            returnStatus = true;
        }
    }
    return returnStatus;
}
