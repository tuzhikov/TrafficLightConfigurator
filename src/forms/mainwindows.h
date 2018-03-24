#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "src/forms/setting_port.h"
#include "src/controller/controller.h"
#include "src/settings/settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0, controller *pdata=0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    SettingsDialog *setting_port;
    controller *const ptr_controller;
    const QString date_time_make;
    const QString number_version;

    void createConnectionGui();
    void upGuiToData();
    void upDataToGui();
    void writeSetting();
    void readSetting();

protected:
    void closeEvent(QCloseEvent *event);
signals:

private slots:
    void onActionabout();
    void onCommand( const int );
    void onMessageOk( const QString& );
    void onMessageError( const QString& );
};

#endif // MAINWINDOW_H
