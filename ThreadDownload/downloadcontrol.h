#ifndef DOWNLOADCONTROL_H
#define DOWNLOADCONTROL_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QSettings>
#include "downloadthread.h"
#include "mytip.h"

const qint64 bufferSize = 10*1024*1024;//为每次每个线程分配的字节

class DownLoadControl : public QObject
{
    Q_OBJECT
public:
    explicit DownLoadControl(QObject *parent = 0);

signals:
    void speedChange(QString speed);
    void progressChange(int percentV);
    void downLoadFinished();
    void retrySignal();
public slots:
    bool setDownLoadSettings(QString url,QString fileSavePath,int threadCount);
    qint64 getFileSize();
    void startDownLoad();
    void allotTask();//给空闲的线程分配任务
    void threadFinishTask(int index, qint64 endPoint, qint64 startPoint);
    void writeSettings();//保存下载配置，断点续传
    void updateSpeed();
    void updateProgressBar();
    void getRealUrl();

    void pauseDownLoad(bool b);
    void handleErrorSection(int index);//某区块下载失败，标记为未下载，并重新下载

    void startDownLoadFromSettingFile(QString settingFilePath);

    void downLoadFourPointDataForSubtittle();//先下载射手字幕需要的四个点的数据
private:
    QString url;
    QString realUrl;
    QString fileSavePath;
    QString settingFilePath;
    QFile *file;
    int threadCount;
    qint64 totalBytesCount;
    qint64 downLoadBytesCount;
    QList<DownLoadThread *> threadList;
    QStringList sectionList;//值-1表示该字节区块未分配下载，0表示正在下载，1表示下载完成
    QTimer *updateSpeedTimer;
    QTimer *updateRealUrlTimer;
    QNetworkAccessManager *manager;
    MyTip myTipWin;
};

#endif // DOWNLOADCONTROL_H
