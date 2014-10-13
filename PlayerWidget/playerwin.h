#ifndef PLAYERWIN_H
#define PLAYERWIN_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QCursor>
#include <QtAV/GLWidgetRenderer.h>
#include <QtAV/VideoRenderer.h>
#include <QtAV/AVPlayer.h>
#include <QtAV/AVError.h>
#include <QtAV/VideoRenderer.h>
#include <QGridLayout>
#include <QLabel>
#include "mytip.h"
#include "SubtitleWidget/subtitlewidget.h"
using namespace QtAV;
class PlayerWin : public QWidget//输出·视频图像与控制区的播放部分
{
    Q_OBJECT
public:
    explicit PlayerWin(QWidget *parent = 0);
    AVPlayer *player;
    int state;//0表示停止，1表示播放，2表示暂停
    QString currentPlayer;
    bool isFullScreen;
    GLWidgetRenderer *renderer;
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    //void keyReleaseEvent(QKeyEvent *e);
signals:
    void hideToFullScreen(bool FullScreen);
    void getIntoWideModel(bool ok);
    void percentChange(qreal percent);
    void playFinished();
    void loadingMedia();
    void callForPlayer2Ready();
    void progressBarDrawTxt(QString txt);
    void sendloadSlice(int slice);//加载指定视频片,-1指上一片,0重载当前,1下一片
public slots:
    //void finished(int i);
    void setState(int i);//0表示停止，1表示播放，2表示暂停
    void setProgressbarPosition();
    void seekPos(qreal pos);
    void setPauseState();
    void setStopState();
    void openFile();
    void setWideModel();
    void loadSlice(int slice);
    void loadPreSlice();
    void loadCurSlice();
    void loadNextSlice();
    void handleMediaStatusChanged(QtAV::MediaStatus status);
    void handleError(QtAV::AVError error);
    void newPlay(QString file);
    void setTotalSlices(int i);
    void showSubtitleWidget(bool b);
private:

    bool isWideModel;
    QTimer *updateProgressBarT;
    int EndOfMediaMark;//标记,播放到98%，解析下一片视频并切换播放器.1表示已经发出信号，0表示没有。
    QGridLayout *mainGLayout;

    int totalSlices;//总分片数量
    QLabel *playerMaskLbl;
    MyTip myTipWin;
    SubtitleWidget *subtitleWidget;


};

#endif // PLAYERWIN_H
