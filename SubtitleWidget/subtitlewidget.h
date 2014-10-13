#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>
#include <QTextCodec>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QActionGroup>
#include <QSettings>
#include "mytip.h"
class SubtitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubtitleWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals:

public slots:
    void makeSubReady(QString filePath);//接口,下载文件对应字幕
    void showSubText(qint64 timePos);//接口,不断从这里得到相应时间字幕文本
    void reset();
//++++
    void getLocalFileSub(QString filePath);
    void analyzeSubDownloadJson(QByteArray jsonSource);
    QStringList transformSubFileToSubDataList(QString subFilePath);
    QString getSubTextFromSubDataList(QStringList subDataList, qint64 timePos);
    void getSubFileFromShooter(QString Url);
    QString computerLocalFileHash(QString filePath);
    void getSubFromMenu(QAction* act);
    void selectFontSize(QAction*act);

    qint64 timeStrToMs(QString timeStr);

private:
    bool isBackgroundPaint;
    QString filePath;
    QString subTextToPaint;
    QStringList subDataList;
    QStringList subDownloadUrlList;
    QNetworkAccessManager *manager;
    bool isFirstShow;
    MyTip *myTipWin;
    QPoint MPos;
    int subIndexMark;
    QActionGroup *selectSubGroup;
    QActionGroup *selectFontSizeGroup;

    int fontSize;
};

#endif // SUBTITLEWIDGET_H
