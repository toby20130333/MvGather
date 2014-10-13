#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QBuffer>
class DownLoadThread : public QObject
{
    Q_OBJECT
public:
    explicit DownLoadThread(QObject *parent = 0,QString fileSavePath="");
    bool isIdle;//线程是否空闲中
protected:

signals:
    void finishTask(int index,qint64 endPoint,qint64 startPoint);//下载任务完成后发送信号，请求下一个任务
    void errorSection(int index);
    void updateUrl();
public slots:
    void receiveTask(int index, QString url, qint64 startPoint, qint64 endPoint);//获取一段新的下载任务
    void start();
    void  pause();
    void finishedSlot();
    void readyReadSlot();
    void errorSlot(QNetworkReply::NetworkError);
    void errorIndex();
    qint64 getBytesCountFromLastTime();//上一秒到现在所接收到的字节数，用于统计速度
private:

    QNetworkReply *reply;
    QString url;
    QString fileSavePath;
    qint64 startPoint;
    qint64 endPoint;
    qint64 readyBytes;
    QBuffer buffer;
    int index;
    qint64 BytesCountFromLastTime;
};

#endif // DOWNLOADTHREAD_H
