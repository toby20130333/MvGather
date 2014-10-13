#include "downloadcontrol.h"
#include "qmath.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFileDialog>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
DownLoadControl::DownLoadControl(QObject *parent) :
    QObject(parent)
{
    manager =  new QNetworkAccessManager;
    totalBytesCount = 0;
    downLoadBytesCount = 0;
    updateSpeedTimer = new QTimer;
    connect(updateSpeedTimer,SIGNAL(timeout()),SLOT(updateSpeed()));
    updateSpeedTimer->start(1000);
    updateRealUrlTimer = new QTimer;//每半小时更新真实的地址一次
    connect(updateRealUrlTimer,SIGNAL(timeout()),SLOT(getRealUrl()));
    updateRealUrlTimer->start(1000*60*30);

}

bool DownLoadControl::setDownLoadSettings(QString url, QString fileSavePath, int threadCount)
{
    //    url = "http://dlsw.baidu.com/sw-search-sp/soft/70/17456/BaiduAn_Setup_3.0.0.3971_50043.1408407200.exe";
    //    fileSavePath = "/home/xusongjie/BaiduAn_Setup_3.0.0.3971_50043.1408407200.exe";
    //    threadCount=5;

    this->url=url;
    getRealUrl();

    this->fileSavePath = fileSavePath;
    QFileInfo finfo(fileSavePath);
    QDir dir;
    dir.mkpath(finfo.dir().path());
    file = new QFile(fileSavePath);
    if(! file->open(QFile::WriteOnly))
    {
        //        QMessageBox msg;
        //        msg.setText("文件打开失败。");
        //        msg.exec();
        myTipWin.show();
        myTipWin.setText("文件打开失败。");
        myTipWin.timeToHide(3);
        return false;
    }
    this->threadCount = threadCount;
    this->totalBytesCount= getFileSize();

    if(totalBytesCount == -1|| totalBytesCount ==0)
    {
        emit retrySignal();
        //        QMessageBox msg;
        //        msg.setText("获取文件大小失败。");
        //        msg.exec();
        myTipWin.show();
        myTipWin.setText("获取文件大小失败。");
        myTipWin.timeToHide(3);
        return false;
    }

    if(! file->resize(totalBytesCount))
    {
        //qDebug()<<"file->resize error";
        return false;
    }
    file->close();
    for(int i=0;i<(totalBytesCount/bufferSize+1);i++)
    {
        this->sectionList.append("-1");
    }
    downLoadFourPointDataForSubtittle();//
    return true;
}

qint64 DownLoadControl::getFileSize()
{
    qint64 size = -1;
    int tryTimes=3;
    //尝试tryTimes次
    while(tryTimes --)
    {
        QEventLoop loop;

        //发出请求，获取目标地址的头部信息
        QNetworkReply *reply = manager->head(QNetworkRequest(this->realUrl));
        //if(!reply)continue;

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->error() != QNetworkReply::NoError)
        {
            //qDebug()<<"error:"<<reply->errorString();
            continue;
        }
        QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
        reply->deleteLater();
        size = var.toLongLong();
        break;
    }
    return size;
}

void DownLoadControl::startDownLoad()//创建下载线程并将要下载的文件进行字节分块
{
    if(this->settingFilePath != "")//download from setting file
    {
        getRealUrl();
    }
    for(int i=0;i<threadCount;i++)
    {
        DownLoadThread *thread = new DownLoadThread(0,fileSavePath);
        connect(thread,SIGNAL(finishTask(int,qint64,qint64)),this,SLOT(threadFinishTask(int,qint64,qint64)));
        connect(thread,SIGNAL(errorSection(int)),this,SLOT(handleErrorSection(int)));
        connect(thread,SIGNAL(updateUrl()),this,SLOT(getRealUrl()));
        threadList.append(thread);
    }

    allotTask();
}

void DownLoadControl::allotTask()
{
    //qDebug()<<"分配任务";
    foreach (DownLoadThread *thread, threadList) {
        if(thread->isIdle)
        {
            qint64 startPoint=0;
            qint64 endPoint=0;
            int index=-1;
            if((index = sectionList.indexOf("-1")) != -1)
            {
                if(index == (sectionList.count()-1))
                {
                    startPoint = qint64(index)*bufferSize;
                    endPoint = totalBytesCount;
                }else
                {
                    startPoint = qint64(index)*bufferSize;
                    endPoint = qint64(index+1)*bufferSize-1;
                }
                sectionList.replace(index,"0");/*
                if(startPoint<0)
                {
                    startPoint = -startPoint;
                }
                if(endPoint<0)
                {
                    endPoint = -endPoint;
                }*/
                thread->receiveTask(index,this->realUrl,startPoint,endPoint);
                //qDebug()<<"当前任务总进度:"<<downLoadBytesCount/1024<<"/"<<totalBytesCount/1024<<"KB";

            }
        }
    }

}

void DownLoadControl::threadFinishTask(int index, qint64 endPoint, qint64 startPoint)
{
    downLoadBytesCount +=(endPoint - startPoint+1);
    if(downLoadBytesCount>=totalBytesCount)
    {
        foreach (DownLoadThread *thread, threadList) {
            thread->deleteLater();
            threadList.clear();
        }
        //qDebug()<<"任务完成！";
    }
    sectionList.replace(index,"1");
    writeSettings();
    allotTask();
    updateProgressBar();

}

void DownLoadControl::writeSettings()
{
    QString iniFile = file->fileName() + ".setting";
    QSettings settings(iniFile,QSettings::IniFormat);

    settings.setValue("url",url);
    settings.setValue("fileSavePath",fileSavePath);
    settings.setValue("threadCount",threadCount);
    settings.setValue("totalBytesCount",totalBytesCount);
    settings.setValue("downLoadBytesCount",downLoadBytesCount);
    settings.setValue("sectionList",sectionList);
    settings.sync();
}

void DownLoadControl::updateSpeed()
{
    qint64 speed = 0;
    foreach (DownLoadThread *thread, threadList) {
        speed +=thread->getBytesCountFromLastTime();
    }
    emit speedChange(QString::number(speed/1024)+"Kb/s");
    //qDebug()<<"下载速度："<<speed/1024<<"Kb/s";
}

void DownLoadControl::updateProgressBar()
{
    //qDebug()<<"updateProgressBar"<<downLoadBytesCount<<totalBytesCount;
    int percentV =qFloor(100* (qreal(downLoadBytesCount)/qreal(totalBytesCount)));
    if(percentV>=100)
    {
        percentV=100;
        emit downLoadFinished();
    }
    emit progressChange(percentV);
}

void DownLoadControl::getRealUrl()
{
    //    realUrl= url;//如果真实下载地址与给的地址相同，则直接返回
    //    return;

    if(url.contains("plyz"))
    {
        int tryTimes=5;
        //尝试tryTimes次
        QString realUrl_temp = this->url;
        while(tryTimes --)
        {
            QEventLoop loop;
            QNetworkReply *reply = manager->head(QNetworkRequest(realUrl_temp));
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
            QVariant varLocation = reply->header(QNetworkRequest::LocationHeader);
            if(!varLocation.isNull())
            {
                realUrl_temp = varLocation.toString();
                //qDebug()<<realUrl_temp;
                continue;
            }
            delete reply;
            break;
        }
        this->realUrl = realUrl_temp;
        if(realUrl == "")
        {
            myTipWin.show();
            myTipWin.setText("获取下载链接失败。");
            myTipWin.timeToHide(3);
        }

    }else
    {
        QNetworkRequest request0;
        request0.setUrl(QString("http://www.mvgather.com/mvgather/api/getRealUrl.php?url=%0").arg(url));//
        QNetworkReply *reply0 = manager->get(request0);
        QEventLoop loop0;
        QObject::connect(reply0, SIGNAL(finished()), &loop0, SLOT(quit()));
        loop0.exec();
        QByteArray data0 = reply0->readAll();
        this->realUrl = data0;
    }
}

void DownLoadControl::pauseDownLoad(bool b)
{
    if(b)
    {
        foreach (DownLoadThread *thread, threadList) {
            thread->pause();
        }
        int index;
        while( (index = sectionList.indexOf("0")) != -1)
        {
            sectionList.replace(index,"-1");
        }
    }else
    {
        allotTask();
    }

}

void DownLoadControl::handleErrorSection(int index)
{
    sectionList.replace(index,"-1");
}

void DownLoadControl::startDownLoadFromSettingFile(QString settingFilePath)
{
    this->settingFilePath = settingFilePath;
    QSettings settings(settingFilePath,QSettings::IniFormat);

    this->url =  settings.value("url","http://www.baidu.com").toString();
    this->fileSavePath = settings.value("fileSavePath","file_temp").toString();
    this->threadCount = settings.value("threadCount",5).toInt();
    this->totalBytesCount =  settings.value("totalBytesCount",1024).toLongLong();
    this->downLoadBytesCount =  settings.value("downLoadBytesCount","0").toLongLong();
    //qDebug()<<settingFilePath;
    if((totalBytesCount - downLoadBytesCount) <5 )
    {
        emit downLoadFinished();
        return;
    }
    QList<QVariant> list_temp =  settings.value("sectionList","").toList();
    foreach (QVariant a, list_temp) {
        this->sectionList.append(a.toString());
    }
    int index;
    while( (index = sectionList.indexOf("0")) != -1)
    {
        sectionList.replace(index,"-1");
    }

    file = new QFile(fileSavePath,this);
    if(! file->open(QFile::ReadWrite))//不能用Write,它会清空已下载部分
    {
        //        QMessageBox msg;
        //        msg.setText("文件打开失败。");
        //        msg.exec();
        myTipWin.show();
        myTipWin.setText("文件打开失败。");
        myTipWin.timeToHide(3);
        return;
    }
    file->resize(totalBytesCount);
    //qDebug()<<this->realUrl;
    //    for(int i=0;i<threadCount;i++)
    //    {
    //        DownLoadThread *thread = new DownLoadThread(0,this->fileSavePath);
    //        connect(thread,SIGNAL(finishTask(int,qint64,qint64)),this,SLOT(threadFinishTask(int,qint64,qint64)));
    //        connect(thread,SIGNAL(errorSection(int)),this,SLOT(handleErrorSection(int)));
    //        connect(thread,SIGNAL(updateUrl()),this,SLOT(getRealUrl()));
    //        threadList.append(thread);
    //    }
    //    allotTask();
    file->close();
}

void DownLoadControl::downLoadFourPointDataForSubtittle()
{
    qDebug()<<"downLoadFourPointDataForSubtittle start";
    QFile file(fileSavePath);
    file.open(QFile::ReadWrite);

    qint64 offset[4];
    offset[3] = totalBytesCount - 8192;
    offset[2] = totalBytesCount / 3;
    offset[1] = totalBytesCount / 3 * 2;
    offset[0] = 4096;

    //0
    QNetworkRequest request0;
    request0.setUrl(this->realUrl);
    QString range = QString( "bytes=%0-%1" ).arg(QString::number(offset[0])).arg(QString::number(offset[0]+4096));
    request0.setRawHeader("Range", range.toLatin1());
    QNetworkReply *reply0 = manager->get(request0);
    QEventLoop loop0;
    QObject::connect(reply0, SIGNAL(finished()), &loop0, SLOT(quit()));
    loop0.exec();
    QByteArray data0 = reply0->readAll();
    //qDebug()<<data0;
    //1
    QNetworkRequest request1;
    request1.setUrl(this->realUrl);
    range = QString( "bytes=%0-%1" ).arg(QString::number(offset[1])).arg(QString::number(offset[1]+4096));
    request1.setRawHeader("Range", range.toLatin1());
    QNetworkReply *reply1 = manager->get(request1);
    QEventLoop loop1;
    QObject::connect(reply1, SIGNAL(finished()), &loop1, SLOT(quit()));
    loop1.exec();
    QByteArray data1 = reply1->readAll();
    //qDebug()<<data1;
    //2
    QNetworkRequest request2;
    request2.setUrl(this->realUrl);
    range = QString( "bytes=%0-%1" ).arg(QString::number(offset[2])).arg(QString::number(offset[2]+4096));
    request2.setRawHeader("Range", range.toLatin1());
    QNetworkReply *reply2 = manager->get(request2);
    QEventLoop loop2;
    QObject::connect(reply2, SIGNAL(finished()), &loop2, SLOT(quit()));
    loop2.exec();
    QByteArray data2 = reply2->readAll();
    //qDebug()<<data2;
    //3
    QNetworkRequest request3;
    request3.setUrl(this->realUrl);
    range = QString( "bytes=%0-%1" ).arg(QString::number(offset[3])).arg(QString::number(offset[3]+4096));
    request3.setRawHeader("Range", range.toLatin1());
    QNetworkReply *reply3 = manager->get(request3);
    QEventLoop loop3;
    QObject::connect(reply3, SIGNAL(finished()), &loop3, SLOT(quit()));
    loop3.exec();
    QByteArray data3 = reply3->readAll();
    //qDebug()<<data3;

    file.seek(offset[0]);
    file.write(data0);
    file.seek(offset[1]);
    file.write(data1);
    file.seek(offset[2]);
    file.write(data2);
    file.seek(offset[3]);
    file.write(data3);
    qDebug()<<"downLoadFourPointDataForSubtittle end";
}
