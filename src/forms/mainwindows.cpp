#include <QMessageBox>
#include "mainwindows.h"
#include "ui_mainwindow.h"
#include "setting_port.h"

MainWindow::MainWindow(QWidget *parent, controller *pdata) :
            QMainWindow(parent)
            ,ui(new Ui::MainWindow)
            ,setting_port( new SettingsDialog(parent) )
            ,ptr_controller(pdata)
            ,date_time_make(__TIMESTAMP__)
            ,number_version("1.100")

{
    ui->setupUi(this);
    //create
    createConnectionGui();
    // read registry data
    readSetting();
    // macros
    qDebug()<<date_time_make<<number_version;
}
/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}
/**
 * @brief MainWindow::createConnectionGui()
 */
void MainWindow::createConnectionGui()
{
    /*set slot GUI*/
    connect(ui->actionAbout,&QAction::triggered,this,MainWindow::onActionabout); // about
    // win setting uart
    connect(ui->actionConnection, &QAction::triggered, setting_port, &SettingsDialog::openWindowsSetting );
    //set parametrs com port
    connect(setting_port,&SettingsDialog::signalListParametr,ptr_controller,&controller::setParametrs);
    // connect run command
    QList<QPushButton*> list_button_command;
    list_button_command<<ui->pb_send<<ui->pb_read;
    QSignalMapper *sm_command = new QSignalMapper(this);
    connect(sm_command,SIGNAL(mapped(int)),this,SLOT(onCommand(int)));
    for (int i = 0; i <list_button_command.size(); i++)
    {
        QPushButton *const button(list_button_command.at(i));
        sm_command->setMapping(button, i);
        connect(button, SIGNAL(clicked(bool)), sm_command,SLOT(map()));
    }
    // connect controller
    connect(ptr_controller,&controller::signalMessageOk,this,MainWindow::onMessageOk);
    connect(ptr_controller,&controller::signalMessageError,this,MainWindow::onMessageError);
}
/**
 * @brief MainWindow::upGuiToData
 */
void MainWindow::upGuiToData()
{
    const quint8 direct = ui->cbDirect->currentIndex()+1;
    const quint8 mode_slave = (ui->rb_mode_slave->isChecked())? true:false;
    ptr_controller->setDataProtocol( mode_slave,direct );
}
/**
 * @brief MainWindow::upDataToGui
 */
void MainWindow::upDataToGui()
{
    quint8 direct = 0;
    quint8 mode_slave = 0;
    ptr_controller->getDataProtocol(mode_slave,direct);
    bool checked = (mode_slave)?true:false;
    ui->rb_mode_slave->setChecked(checked);
    ui->cbDirect->setCurrentIndex(direct-1);
}
/**
 * @brief MainWindow::writeSetting
 */
void MainWindow::writeSetting()
{
    //Settings::set(Settings::GEOMETRY,Settings::GENERAL) = saveGeometry();
    //Settings::set(Settings::GEOMETRY) = saveGeometry();
    Settings::set(Settings::COMNAME,Settings::COMPORT) = setting_port->getComName();
    Settings::set(Settings::BAUDRATE,Settings::COMPORT) = setting_port->getBaudRate();
    Settings::set(Settings::DATABITS,Settings::COMPORT) = setting_port->getDataBits();
    Settings::set(Settings::STOPBITS,Settings::COMPORT) = setting_port->getStopBits();
    Settings::set(Settings::PARITY,Settings::COMPORT) = setting_port->getParity();
    Settings::set(Settings::FLOWCONTROL,Settings::COMPORT) = setting_port->getFlowControl();
}
/**
 * @brief MainWindow::readSetting
 */
void MainWindow::readSetting()
{
    // set default
    QFile file(":/config/default.ini");
    file.open(QIODevice::ReadOnly);
    Settings::setDefaults(file.readAll());
    // read
    //const QByteArray gemData = Settings::get(Settings::GEOMETRY).toByteArray();
    //restoreGeometry(gemData);
    // COM PORT
    const QString comname = Settings::get(Settings::COMNAME,Settings::COMPORT).toString();
    if (!comname.isEmpty())
        setting_port->setComName(comname);
    const QString baudRate = Settings::get(Settings::BAUDRATE,Settings::COMPORT).toString();
    if (!baudRate.isEmpty())
        setting_port->setBaudRate(baudRate);
    const QString databits = Settings::get(Settings::DATABITS,Settings::COMPORT).toString();
    if (!databits.isEmpty())
        setting_port->setDataBits(databits);
    const QString stopbits = Settings::get(Settings::STOPBITS,Settings::COMPORT).toString();
    if (!stopbits.isEmpty())
        setting_port->setStopBits(stopbits);
    const QString parity = Settings::get(Settings::PARITY,Settings::COMPORT).toString();
    if (!parity.isEmpty())
        setting_port->setParity(parity);
    const QString flowcnt = Settings::get(Settings::FLOWCONTROL,Settings::COMPORT).toString();
    if (!flowcnt.isEmpty())
        setting_port->setFlowControl(flowcnt);
    setting_port->updateParametrs();
}
/**
 * @brief MainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *)
{
    writeSetting();
    delete setting_port;
}
/**
 * @brief MainWindow::onActionabout
 */
void MainWindow::onActionabout()
{
    QMessageBox::about(this,tr("About the program a Configure RS485."),
                                tr(
                               "<h3>The program configure TrafficLight. version: %1 </h3>"
                               "<p>All rights to this software belong to Co., <a href= http://rtk-orel.ru >Ltd. RTK</a> </p>"
                               "<p> Build time: %2 </p>"
                                ).arg(number_version).arg(date_time_make));
}
/**
 * @brief MainWindow::onCommand
 * @param number_comm
 */
void MainWindow::onCommand(const int number_comm)
{
    upGuiToData();
    // run command
    ptr_controller->commandRun(number_comm);
    qDebug("MainWindow::onCommand: Number command %d",number_comm);
}

void MainWindow::onMessageOk(const QString &msg)
{
    QMessageBox::information(this,tr("Successfully"),msg);
    upDataToGui(); // is OK
}

void MainWindow::onMessageError(const QString &msg)
{
    QMessageBox::critical(this,tr("Error"),msg);
}
