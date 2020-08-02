#include <QCoreApplication>
#include <iostream>
#include <TarnaBot>
#include <QNetworkProxy>
#include "note_bot.h"

using namespace Telegram;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Note_bot bot("1158035122:AAExA2xozNsSHQ26vAqQEem4Z-F_0Y5_UWk", 1000);

    return a.exec();

}
