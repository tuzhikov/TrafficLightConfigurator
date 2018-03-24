#ifndef CRC_H
#define CRC_H

#include <QByteArray>

/*Checksum calculation*/
class CRC
{
public:
    //CRC8
    static quint8 Bit8(QByteArray &data){
      quint8 CRC8=0;
      foreach(char byte,data)
        CRC8 += byte;
      return CRC8;
      }
    //CRC-16
    static quint16 Bit16(const QByteArray &data){
        quint16 CRC16 = 0xFFFF;
        quint16 leng = data.size();
        const quint8 *pMess = (quint8*)(data.data());

        while(leng--)
            {
            CRC16^=*pMess;
            for(int i=0;i<8;i++)
                {
                if(CRC16&0x0001){ // low bit
                    CRC16>>=1;
                    CRC16^=0xA001;
                    }
                else{
                    CRC16>>=1;
                    }
                }
            pMess++;
            }
        return CRC16;
        }
};

#endif // CRC_H
