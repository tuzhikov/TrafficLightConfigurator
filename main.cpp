#include <QApplication>
#include <QTranslator>
#include <QtWidgets>
#include "src/forms/mainwindows.h"
#include "src/controller/controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Q_INIT_RESOURCE(Resources);
    QTranslator trn;
    trn.load(QString(":/TS/language_ru"));
    a.installTranslator(&trn);
    //setting parametr
    QApplication::setOrganizationName("Software Ltd.");
    QApplication::setApplicationName("Traffic Light settings RS485");
    controller cntr;
    //create main windows
    MainWindow w(0,&cntr);
    w.setWindowIcon(QPixmap(":/ico/traffic.ico"));
    w.setWindowTitle(QObject::tr("Traffic light"));
    w.move((QApplication::desktop()->width()-w.width())/2,
           (QApplication::desktop()->height()-w.height())/2);
    w.show();

    return a.exec();
}
