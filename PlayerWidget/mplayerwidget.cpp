#include "mplayerwidget.h"
#include <QStringList>
MplayerWidget::MplayerWidget(QWidget *parent) :
    QWidget(parent)
{
    playPro = new QProcess;
}

void MplayerWidget::play(QString fileList)
{
    QStringList fileLists = fileList.split("#");
    if(fileLists.count()<1) return;
    QStringList args;
    args << tr("-slave");
    args << "-quiet";
    args << "-zoom";
    args << tr("-wid") << QString::number(this->winId());  //这个是将mplayer的输出（就是视频定位到你自己的窗口中）
    args << fileLists.first();
    playPro->start(tr("mplayer"), args);   //开始播放视频
    fileLists.removeFirst();
    foreach (QString file, fileLists) {
        playPro->write(QString("loadfile %0 1\n").arg(file));
    }
}
