#include "downloadthread.h"
#include <QFile>
#include <QDebug>
DownLoadThread::DownLoadThread(QObject *parent, QString fileSavePath) :
    QObject(parent)
{
    isIdle =true;
    readyBytes = 0;
    this->fileSavePath = fileSavePath;
    BytesCountFromLastTime=0;
}

void DownLoadThread::start()
{
    QNetworkRequest request;
    request.setUrl(this->url);
    QString range = QString( "bytes=%0-%1" ).arg(startPoint).arg(endPoint );
    request.setRawHeader("Range", range.toLatin1());
    //qDebug()<<"线程正处理的区块"<<index<<startPoint/1024<<"-"<<endPoint/1024<<"KB";
    QNetworkAccessManager *manager =  new QNetworkAccessManager;
    reply = manager->get(request);
    connect(reply,SIGNAL(finished()),SLOT(finishedSlot()));
    connect(reply,SIGNAL(readyRead()),SLOT(readyReadSlot()));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(errorSlot(QNetworkReply::NetworkError)));
    //connect(reply,SIGNAL(finished()),this,SLOT(quit()));

}

void DownLoadThread::pause()
{
    if(reply != Q_NULLPTR)
        delete reply;
    isIdle =true;
    readyBytes = 0;
}

void DownLoadThread::receiveTask(int index,QString url, qint64 startPoint, qint64 endPoint)
{
    isIdle =false;
    readyBytes = 0;
    this->index = index;
    this->url = url;
    buffer.open(QIODevice::ReadWrite);
    buffer.reset();
    this->startPoint = startPoint;
    this->endPoint = endPoint;
    //qDebug()<<index<<"线程正处理的区块"<<startPoint/1024<<"-"<<endPoint/1024<<"KB";
    this->start();
}

void DownLoadThread::finishedSlot()
{
    if(buffer.size()<(endPoint - startPoint))//有错误，重新下载
    {
        readyBytes=0;
        isIdle =true;
        emit updateUrl();
        errorIndex();
    }else
    {
        QFile file(fileSavePath);
        if(! file.open(QFile::ReadWrite))//不能用Write,它会清空已下载部分
        {
            errorIndex();
            //qDebug()<<"file open error";
            return;
        }
        if(!file.seek(startPoint))
        {
            errorIndex();
            //qDebug()<<"error seek";
            return;
        }
        if(file.write(buffer.data(),buffer.size())==-1)
        {
            errorIndex();
            //qDebug()<<"file.write";
            return;
        }
        file.close();
        readyBytes=0;
        isIdle =true;
        emit finishTask(index,endPoint,startPoint);
        //qDebug()<<"finishTask(index)"<<index;
    }
    buffer.close();
}

void DownLoadThread::readyReadSlot()
{
    if(!buffer.isOpen())
    {
        if(!buffer.open(QIODevice::WriteOnly))
        {
            return;
        }
    }
    if(!buffer.seek(readyBytes))
    {
        return;
    }
    QByteArray byteArray(reply->readAll());
    buffer.write(byteArray, byteArray.size());
    readyBytes+=byteArray.size();
    BytesCountFromLastTime+=byteArray.size();

}

void DownLoadThread::errorSlot(QNetworkReply::NetworkError)
{
    errorIndex();
    //qDebug()<<reply->errorString();
}

void DownLoadThread::errorIndex()
{
    emit errorSection(this->index);
}

qint64 DownLoadThread::getBytesCountFromLastTime()
{
    qint64 temp= BytesCountFromLastTime;
    BytesCountFromLastTime = 0;
    return temp;
}
