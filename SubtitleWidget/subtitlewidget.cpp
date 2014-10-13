#include "subtitlewidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QTextStream>
SubtitleWidget::SubtitleWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    isBackgroundPaint = false;

    subIndexMark = 0;
    this->MPos = QPoint(0,0);
    QSettings settings("MvGather", "xusongjie");
    fontSize = settings.value("app/subtitleFontSize",26).toInt();
    if(fontSize<5)
    {
        fontSize=26;
    }
    QDesktopWidget* desktop = QApplication::desktop();
    resize(fontSize*40,fontSize*3);
    move((desktop->width()-this->width())/2,desktop->height()-this->height()-45);
    subDataList.append("0#999999999#欢迎使用字幕.");
    manager = new QNetworkAccessManager;
    myTipWin = new MyTip;

    selectSubGroup = new QActionGroup(this);
    selectFontSizeGroup = new QActionGroup(this);
    selectSubGroup->setExclusive(true);
    selectFontSizeGroup->setExclusive(true);



}

void SubtitleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(isBackgroundPaint)
    {
        QImage image("://images/tipBackgound.png");
        painter.drawImage(this->rect(),image);
    }
    painter.setPen(Qt::white);
    QFont sansFont("Helvetica [Cronyx]",fontSize);
    painter.setFont(sansFont);
    painter.drawText(this->rect(), Qt::AlignCenter, subTextToPaint);
}

void SubtitleWidget::enterEvent(QEvent *event)
{
    isBackgroundPaint  = true;
    repaint();
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    isBackgroundPaint = false;
    repaint();
}

void SubtitleWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        QMenu *menu= new QMenu;
        QMenu *selectSubMenu= new QMenu("字幕选择");
        foreach (QAction *a, selectSubGroup->actions()) {
            delete a;
        }

        foreach (QString url, subDownloadUrlList) {
            int index = subDownloadUrlList.indexOf(url);
            QAction *selectSubAct = new QAction(QString("字幕%0").arg(index), this);
            selectSubAct->setData(url);
            selectSubAct->setCheckable(true);
            if(index==subIndexMark)
            {
                selectSubAct->setChecked(true);
            }
            selectSubGroup->addAction(selectSubAct);
            selectSubMenu->addAction(selectSubAct);
        }
        connect(selectSubGroup, SIGNAL(triggered(QAction*)), this, SLOT(getSubFromMenu(QAction*)));

        QMenu *selectFontSizeMenu= new QMenu("字体大小");
        foreach (QAction *a, selectFontSizeGroup->actions()) {
            delete a;
        }
        for(int i=((fontSize-10)>0?(fontSize-10):0);i<(fontSize+10);i++)
        {
            QAction *selectFontSizeAct = new QAction(QString("%0").arg(i), this);
            selectFontSizeAct->setCheckable(true);
            selectFontSizeAct->setData(i);
            if(i==fontSize)
            {
                selectFontSizeAct->setChecked(true);
            }
            selectFontSizeGroup->addAction(selectFontSizeAct);
            selectFontSizeMenu->addAction(selectFontSizeAct);
        }
        connect(selectFontSizeGroup, SIGNAL(triggered(QAction*)), this, SLOT(selectFontSize(QAction*)));



        menu->addMenu(selectSubMenu);
        menu->addMenu(selectFontSizeMenu);
        menu->exec(QCursor::pos());
    }else if(event->button()==Qt::LeftButton)
    {
        this->MPos = event->globalPos()- this->pos();
    }
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->MPos);
}

void SubtitleWidget::makeSubReady(QString filePath)
{
    //qDebug()<<"makeSubReady"<<filePath;
    if(this->filePath == filePath || filePath=="")
    {
        return;
    }
    this->filePath = filePath;
    myTipWin->show();
    myTipWin->setBusyIndicatorShow(true);
    myTipWin->setText("获取字幕列表");
    getLocalFileSub(filePath);
    QString url = subDownloadUrlList.value(0,"");
    if(url=="")
    {
        myTipWin->setText("找不到字幕");
    }else
    {
        myTipWin->setText("正在获取字幕");
        getSubFileFromShooter(url);
        myTipWin->setText("获取完成");
    }
    myTipWin->timeToHide(2);

}

void SubtitleWidget::showSubText(qint64 timePos)
{
    this->subTextToPaint = getSubTextFromSubDataList(subDataList,timePos);
    repaint();
}

void SubtitleWidget::reset()
{
    subDataList.clear();
    subDataList.append("0#9999999999#欢迎使用字幕.");
    filePath="";
    subDownloadUrlList.clear();
    myTipWin->setText("");
    subIndexMark=0;
}

void SubtitleWidget::getLocalFileSub(QString filePath)
{

    QString fileHash = computerLocalFileHash(filePath);
//qDebug()<<"getLocalFileSub"<<fileHash;
    QNetworkRequest request(QUrl("http://www.shooter.cn/api/subapi.php"));

    QByteArray postData;
    postData.append(QString("filehash=%0&pathinfo=%1&format=json").arg(QString(fileHash.toUtf8().toPercentEncoding())).arg(QString(filePath.toUtf8().toPercentEncoding())));

    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QNetworkReply *reply = manager->post(request,postData);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray jsonSource = reply->readAll();//

    analyzeSubDownloadJson(jsonSource);

}


void SubtitleWidget::analyzeSubDownloadJson(QByteArray jsonSource)
{
    //qDebug()<<"analyzeSubDownloadJson";
    QJsonParseError *error=new QJsonParseError;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonSource,error);
    if(error->error==QJsonParseError::NoError)
    {
        QJsonArray rootArray = jsonDoc.array();
        foreach (QJsonValue subFileValue, rootArray) {
            QJsonObject subFileObj = subFileValue.toObject();
            QJsonArray FilesArray = subFileObj["Files"].toArray();
            foreach (QJsonValue fileValue, FilesArray) {
                QJsonObject fileObj = fileValue.toObject();
                if(fileObj["Ext"] == "srt")
                {
                    subDownloadUrlList.append(fileObj["Link"].toString());
                    //qDebug()<<fileObj["Link"];
                }
            }
        }
    }else
    {
        subDataList.clear();
        subDataList.append("00:00:00,000#11:11:11,111#无法找到匹配的字幕!");
    }
}

QStringList SubtitleWidget::transformSubFileToSubDataList(QString subFilePath)
{
    //qDebug()<<"transformSubFileToSubDataList";
    //http://blog.csdn.net/piaozhiye/article/details/6661964
    QStringList subDataList;//startTime#endTime#text
    QString timeStr;
    QString text;

    QFile file(subFilePath);
    file.open(QFile::ReadOnly);
    QRegExp numberLineRx("^[0-9]{1,10}");
    QRegExp timeLineRx("\\d\\d:\\d\\d:\\d\\d,\\d\\d\\d --> \\d\\d:\\d\\d:\\d\\d,\\d\\d\\d");//时间描述行
    QRegExp timeRx("\\d\\d:\\d\\d:\\d\\d,\\d\\d\\d");
    timeLineRx.setMinimal(true);
    timeRx.setMinimal(true);

    //QTextCodec *codec = QTextCodec::codecForName("GB2312");

    while (!file.atEnd()) {
        QString line = file.readLine();
        //QString line = codec->toUnicode(file.readLine());
        //qDebug()<<line;
        //将时间描述行转化成两个ms时间存入,将下一行的字幕存入String
        if(line.contains(timeLineRx))
        {
            line.replace(QRegExp("\\s+"),"");
            QStringList l=  line.split("-->");
            timeStr = QString::number(timeStrToMs(l.value(0,"")))+"#"+QString::number(timeStrToMs(l.value(1,"")));

            //QStringList timeList = line.split("-->");
            //qDebug()<<startTime<<endTime;

        }else if(line.contains(numberLineRx))
        {
            numberLineRx.indexIn(line);
            //qDebug()<<"numberLineRx"<<numberLineRx.cap(0);
        }else if(line.contains(QRegExp("^\\s+")))//空行，
        {
            subDataList.append(timeStr+"#"+text);
            text = "";
        }else
        {
            QRegExp rx("<.*>");
            rx.setMinimal(true);
            line.replace(rx,"");
            text.append(line);
        }
    }

    return subDataList;
}

QString SubtitleWidget::getSubTextFromSubDataList(QStringList subDataList, qint64 timePos)
{
    QString subText;
    foreach (QString str, subDataList) {
        QStringList list = str.split("#");
        qint64 startTimeTemp = list.value(0,"").toInt();
        qint64 endTimeTemp = list.value(1,"").toInt();
        if(timePos>=startTimeTemp && timePos<= endTimeTemp)
        {
            subText = list.value(2,"");
            break;
        }

    }
    return subText;
}

void SubtitleWidget::getSubFileFromShooter(QString Url)
{
    //qDebug()<<"getSubFileFromShooter"<<Url;
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(Url.replace("https","http"))));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray srtFileData = reply->readAll();//

    QTextCodec *codecDefault = QTextCodec::codecForName("GB2312");
    QTextCodec *codec = QTextCodec::codecForUtfText(srtFileData,codecDefault);
    QString codecText= codec->toUnicode(srtFileData);

    QDir dir;
    QDir dir2(dir.homePath()+"/视频");
    QDir dir3(dir.homePath()+"/Videos");
    QString srtPath;
    if(dir2.exists())
    {
        srtPath = dir.homePath()+"/视频/MvGather/shooter.srt";
    }else if(dir3.exists())
    {
        srtPath = dir.homePath()+"/Videos/MvGather/shooter.srt";
    }else
    {
        srtPath = dir.homePath()+"/MvGather/shooter.srt";
    }
    QFile file(srtPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    QTextStream out(&file);
    QTextCodec *codecUtf8 = QTextCodec::codecForName("UTF-8");
    out.setCodec(codecUtf8);
    QByteArray a;
    a.append(codecText);
    out<<a;
    //qDebug()<<srtFileData;
    subDataList.clear();
    subDataList = transformSubFileToSubDataList(srtPath);
}

QString SubtitleWidget::computerLocalFileHash(QString filePath)
{
    //qDebug()<<"computerLocalFileHash";
    QStringList fileHashList;
    QFile file(filePath);
    file.open(QFile::ReadOnly);

    QFileInfo fileInfo(file);
    qint64 ftotallen = fileInfo.size();

    qint64 offset[4];
    offset[3] = ftotallen - 8192;
    offset[2] = ftotallen / 3;
    offset[1] = ftotallen / 3 * 2;
    offset[0] = 4096;

    file.seek(offset[0]);
    QByteArray data0 = file.read(4096);
    QByteArray md5_0 = QCryptographicHash::hash(data0,QCryptographicHash::Md5).toHex();
    fileHashList.append(md5_0);

    file.seek(offset[1]);
    QByteArray data1 = file.read(4096);
    QByteArray md5_1 = QCryptographicHash::hash(data1,QCryptographicHash::Md5).toHex();
    fileHashList.append(md5_1);

    file.seek(offset[2]);
    QByteArray data2 = file.read(4096);
    QByteArray md5_2 = QCryptographicHash::hash(data2,QCryptographicHash::Md5).toHex();
    fileHashList.append(md5_2);

    file.seek(offset[3]);
    QByteArray data3 = file.read(4096);
    QByteArray md5_3 = QCryptographicHash::hash(data3,QCryptographicHash::Md5).toHex();
    fileHashList.append(md5_3);

    return fileHashList.join(";");
}

void SubtitleWidget::getSubFromMenu(QAction *act)
{
    subIndexMark=selectSubGroup->actions().indexOf(act);
    QString url = act->data().toString();
    getSubFileFromShooter(url);

}

void SubtitleWidget::selectFontSize(QAction *act)
{
    this->fontSize = act->data().toInt();
    resize(fontSize*40,fontSize*3);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width()-this->width())/2,desktop->height()-this->height()-45);

    QSettings settings("MvGather", "xusongjie");
    settings.setValue("app/subtitleFontSize", this->fontSize);
    settings.sync();
}

qint64 SubtitleWidget::timeStrToMs(QString timeStr)
{
    QTime time = QTime::fromString(timeStr,"hh:mm:ss,zzz");
    int ms = time.msecsSinceStartOfDay();
    return ms;
}

