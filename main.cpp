#include <QApplication>
#include "TerminalDisplay.h"
#include "Vt102Emulation.h"
#include "ScreenWindow.h"
#include <QProcess>
#include <QDebug>
#include <QSsh>
#include "Shell.h"
#include <QLoggingCategory>
#include "ColorScheme.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLoggingCategory::setFilterRules("qtc.ssh.debug=false");

    QSsh::SshConnectionParameters parameters;
    parameters.authenticationType=QSsh::SshConnectionParameters::AuthenticationType::AuthenticationTypePassword;
    parameters.password="123";
    parameters.host="192.168.1.102";
    parameters.userName="czr";
    parameters.port=22;
    parameters.timeout=10;

    Shell shell(parameters);
    shell.run();

    TerminalEmulation* emulation = new Vt102Emulation();
    TerminalDisplay* display = new TerminalDisplay();

    emulation->setKeyBindings("");
    //emulation->setImageSize(30,80);
    emulation->setHistory(HistoryTypeBuffer(1000));
    emulation->setCodec(QTextCodec::codecForName("UTF-8"));

    QObject::connect(&shell,&Shell::remoteStdout,[=](QByteArray data){
        emulation->receiveData(data.data(),data.length());
    });
    QObject::connect(emulation,&TerminalEmulation::sendData,[&](const char*data,int len){
        QByteArray buf(data,len);
        shell.writeRemote(buf);
    });

    //addView
    QObject::connect( display , &TerminalDisplay::keyPressedSignal ,[=](QKeyEvent* event){
        emulation->sendKeyEvent(event);
    });
    QObject::connect( display , &TerminalDisplay::sendStringToEmu,[=](const char* str){
        emulation->sendString(str);
    });
    QTimer *resizeTimer=new QTimer(display);
    resizeTimer->setSingleShot(true);
    resizeTimer->setInterval(100);
    QObject::connect(resizeTimer,&QTimer::timeout,[&](){
        TerminalDisplay* view=display;
        int minLines = -1;
        int minColumns = -1;
        minLines = view->lines();
        minColumns = view->columns();
        // backend emulation must have a _terminal of at least 1 column x 1 line in size
        if ( minLines > 0 && minColumns > 0 ) {
            emulation->setImageSize( minLines , minColumns );
        }
    });
    QObject::connect(display,&TerminalDisplay::changedContentSizeSignal,[&](){
        resizeTimer->start();
    });
    display->setScreenWindow(emulation->createWindow());
    display->setTerminalSizeHint(true);
    display->setTerminalSizeStartup(true);
    display->setRandomSeed(0);
    QFont font("monospace",10);
    font.setStyleHint(QFont::Monospace);
    display->setVTFont(font);
    display->show();
    display->setScrollBarPosition(TerminalDisplay::ScrollBarRight);
    display->setUsesMouse(true);


    return a.exec();
}
