#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include "downloadcontrol.h"
#include "mytip.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0,QString url="",QString fileSavePath="./");
    Widget(QString settingFilePath="./");
    ~Widget();
signals:
    void play(QString filePath);
private slots:
    void on_startDownBtn_clicked();

    void on_pauseBtn_clicked();

    void on_progressBar_valueChanged(int value);

    void on_playBtn_clicked();

    void downLoadFinished();

    void nomalDownload();
    void settingFileDownload();
    void saveSettingToSettings();
    void retryDownLoad();
    void deleteThisTask();

private:
    Ui::Widget *ui;
    DownLoadControl *control;
    QString url;
    QString fileSavePath;
    QString settingFilePath;
    MyTip myTipWin;
};

#endif // WIDGET_H
