#include "mainfrm.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("MvGather", "xusongjie");
    int theme=settings.value("app/theme", 1).toInt();
    if(theme==0)
    {

    }else if(theme ==1)
    {
        QFile file("://darkGrey.qss");
        if(file.open(QFile::ReadOnly))
        {
            a.setStyleSheet(file.readAll());
        }
    }else if(theme == 2)
    {
        QFile file("://liteBlueTheme.qss");
        if(file.open(QFile::ReadOnly))
        {
            a.setStyleSheet(file.readAll());
        }
    }
    MainFrm w;
    w.show();

    QStringList arguments = a.arguments();
    arguments.removeFirst();
    QString filePath = arguments.join(" ");
    if (filePath != "")
    {
        w.play(filePath);
    }
    return a.exec();
}
