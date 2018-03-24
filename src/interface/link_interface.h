#ifndef LINKINREFACE_H
#define LINKINREFACE_H

#include <stdint.h>
#include <QByteArray>
#include <QVector>
#include <QVariant>

enum RET_ANSWER{
    SUCCESSFULLY    = 0x00000001,
    NO_ANSWER       = 0x00000002,
    ERROR_ANSWER    = 0x00000004,
    DELAY_ANSWER    = 0x00000008,
    END_ANS         = 0x00000009
};
enum RET_CONNECT{
    CONNECT_OK      = 0x00000001,
    CONNECT_NO      = 0x00000002,
    CONNECT_DELAY   = 0x00000004,
    END_CMD         = 0x00000005
};

typedef QVector <QVariant> LIST_PARAMETR;
/**
* Interface link
*/
class LinkInreface
{
    public:
        virtual ~LinkInreface(){}
        virtual RET_ANSWER sendDate( QByteArray &date ) = 0;
        virtual RET_ANSWER readDate(QByteArray &date) = 0;
        virtual RET_CONNECT connectionInformation() = 0;
        virtual bool connections(const QStringList &) = 0;
        virtual bool close() = 0;
        virtual bool open() = 0;
        virtual int retTimeDelay() = 0;
};

#endif // LINKINREFACE_H
