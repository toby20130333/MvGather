#include "playerwin.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QFileDialog>
#include "qmath.h"
PlayerWin::PlayerWin(QWidget *parent) :
    QWidget(parent)
{
    setMinimumWidth(200);
    //setCursor(QCursor(Qt::UpArrowCursor));
    isFullScreen = false;
    state = 0;
    isWideModel = false;

    mainGLayout = new QGridLayout(this);
    mainGLayout->setMargin(0);
    mainGLayout->setSpacing(0);

    player= new  AVPlayer(this);
    renderer =  new GLWidgetRenderer(this);
    player->setRenderer(renderer);
    player->setAudioOutput(QtAV::AudioOutputFactory::create(QtAV::AudioOutputId_OpenAL));
    QVariantHash opt;
    opt["probesize"] = 14096;
    player->setOptionsForFormat(opt);

    mainGLayout->addWidget(renderer,0,0,1,1);

    //QImage img("://images/icon.png");

    //renderer->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
//    QImage img(5,5,QImage::Format_ARGB32);
//    img.fill(Qt::black);
//    renderer->receive(VideoFrame(img));
    renderer->setVisible(false);

    playerMaskLbl = new QLabel;
    playerMaskLbl->setAlignment(Qt::AlignCenter);
    playerMaskLbl->setStyleSheet("background-color:rgba(0,0,0,255)");
    playerMaskLbl->setPixmap(QPixmap("://images/icon.png"));
    mainGLayout->addWidget(playerMaskLbl,1,0,1,1);


    connect(player, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(handleMediaStatusChanged(QtAV::MediaStatus)));
    connect(player, SIGNAL(error(QtAV::AVError)), this, SLOT(handleError(QtAV::AVError)));
    updateProgressBarT = new QTimer;
    connect(updateProgressBarT, SIGNAL(timeout()), this, SLOT(setProgressbarPosition()));
    updateProgressBarT->start(500);

    subtitleWidget = new SubtitleWidget(this);
    EndOfMediaMark=0;

}

void PlayerWin::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(isFullScreen)
    {
        emit hideToFullScreen(true);
        isFullScreen=false;
    }else
    {
        emit hideToFullScreen(false);
        isFullScreen=true;
    }
    //QTimer::singleShot(200,this,SLOT(repaint()));
}

void PlayerWin::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        QMenu *menu= new QMenu;
        QAction *pauseAct = new QAction(tr(this->state==1?"暂停&P":"播放&P"), this);
        QAction *stopAct = new QAction(tr("停止&S"), this);
        QAction *wideAct = new QAction(tr(this->isWideModel==true?"退出精简模式":"精简模式"), this);
        QAction *openAct = new QAction(tr("打开本地文件&O"), this);

        QAction *loadAct = new QAction(tr("视频片切换"), this);
        QMenu *loadMenu= new QMenu;
        QAction *loadPreAct = new QAction(tr("上一片"), this);
        QAction *loadCurAct = new QAction(tr("重载当前"), this);
        QAction *loadNextAct = new QAction(tr("下一片"), this);
        loadMenu->addAction(loadPreAct);
        //loadMenu->addAction(loadCurAct);
        loadMenu->addAction(loadNextAct);
        loadAct->setMenu(loadMenu);

        QAction *subAct = new QAction(tr("外挂字幕"), this);
        subAct->setCheckable(true);
        subAct->setChecked((subtitleWidget->isVisible())?true:false);

        connect(pauseAct, SIGNAL(triggered()), this, SLOT(setPauseState()));
        connect(stopAct, SIGNAL(triggered()), this, SLOT(setStopState()));
        connect(wideAct, SIGNAL(triggered()), this, SLOT(setWideModel()));
        connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
        connect(loadPreAct, SIGNAL(triggered()), this, SLOT(loadPreSlice()));
        connect(loadCurAct, SIGNAL(triggered()), this, SLOT(loadCurSlice()));
        connect(loadNextAct, SIGNAL(triggered()), this, SLOT(loadNextSlice()));
        connect(subAct, SIGNAL(triggered(bool)), this, SLOT(showSubtitleWidget(bool)));

        menu->addAction(pauseAct);
        menu->addAction(stopAct);
        menu->addAction(wideAct);
        menu->addAction(openAct);
        menu->addSeparator();
        menu->addAction(loadAct);
        menu->addAction(subAct);
        menu->exec(QCursor::pos());
    }

}

void PlayerWin::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QPen pen;
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawRect(this->rect());
}

void PlayerWin::resizeEvent(QResizeEvent *event)
{
    //qDebug()<<"resize"<<event->size()<<this->mapToGlobal(this->pos());
}

//void PlayerWin::keyReleaseEvent(QKeyEvent *e)
//{
//    if(e->key()==Qt::Key_Space)
//    {
//        if(this->state==1)
//        {
//            setState(2);
//        }else if(this->state==2)
//        {
//            setState(1);
//        }

//    }
//}

//void PlayerWin::finished(int i)
//{
//    emit playFinished();
//}

void PlayerWin::setState(int i)
{
    if(i == 0)//停止
    {
        player->stop();
        emit progressBarDrawTxt(QString("停止"));
        if(!playerMaskLbl->isVisible())
        {
            renderer->setVisible(false);
            playerMaskLbl->setVisible(true);
        }
        subtitleWidget->hide();
    }else if(i==1)
    {
        player->togglePause();
        emit progressBarDrawTxt(QString("播放"));
    }else
    {
        player->togglePause();
        emit progressBarDrawTxt(QString("暂停"));
    }
    state = i;
}

void PlayerWin::setProgressbarPosition()
{
    if(player->isPlaying())
    {

        qreal posPercent = float(player->position()/float(player->duration()));
        //qDebug()<<player->position()<<player->duration()<<posPercent;
        posPercent = posPercent - qFloor(posPercent);
        emit percentChange(posPercent);
        if(posPercent>0.9 && EndOfMediaMark==0)
        {
            emit callForPlayer2Ready();
            EndOfMediaMark=1;
        }
        if (subtitleWidget->isVisible())
        {
            subtitleWidget->showSubText(player->position());
        }
    }

    repaint();

}

void PlayerWin::seekPos(qreal pos)
{
    if(!this->state==0)
    {
        player->seek(pos);
        //mplayerProc->write(QString("seek %1 1\n").arg(pos*100).toLatin1());
    }
}

void PlayerWin::setPauseState()
{
    if(this->state==1)
    {
        this->setState(2);
    }else if(this->state==2)
    {
        this->setState(1);
    }
}

void PlayerWin::setStopState()
{
    this->setState(0);
}

void PlayerWin::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home");
    if(fileName =="")
    {
    }else
    {
        player->stop();
        player->play(fileName);
        this->state=1;
    }
}

void PlayerWin::setWideModel()
{
    if(isWideModel)
    {
        emit getIntoWideModel(true);
        isWideModel=false;
    }else
    {
        emit getIntoWideModel(false);
        isWideModel=true;
    }
}

void PlayerWin::loadSlice(int slice)
{
    emit sendloadSlice(slice);
}

void PlayerWin::loadPreSlice()
{
    loadSlice(-1);
}

void PlayerWin::loadCurSlice()
{
    loadSlice(0);
}

void PlayerWin::loadNextSlice()
{
    loadSlice(1);
}

void PlayerWin::handleMediaStatusChanged(MediaStatus status)
{
    if(status == QtAV::EndOfMedia)
    {
        emit playFinished();
        EndOfMediaMark=0;
        subtitleWidget->hide();
    }else if(status == QtAV::LoadingMedia)
    {
        playerMaskLbl->setVisible(false);
        renderer->setVisible(true);
        emit loadingMedia();
    }/*else if(status == QtAV::LoadedMedia)
    {
        emit progressBarDrawTxt(QString("LoadedMedia"));
    }else if(status == QtAV::StalledMedia)
    {
        emit progressBarDrawTxt(QString("StalledMedia"));
    }*/else if(status == QtAV::BufferingMedia)
    {
        emit progressBarDrawTxt(QString("正在缓冲……"));
    }/*else if(status == QtAV::BufferedMedia)
    {
        emit progressBarDrawTxt(QString("BufferedMedia"));
    }*/
}

void PlayerWin::handleError(AVError error)
{
    //qDebug()<<error;
    myTipWin.show();
    myTipWin.setBusyIndicatorShow(false);
    myTipWin.setText("播放出错");
    myTipWin.timeToHide(3);
    if(!playerMaskLbl->isVisible())
    {
        playerMaskLbl->setVisible(true);
        renderer->setVisible(false);
    }
    player->stop();
}

void PlayerWin::newPlay(QString file)
{
    //player->stop();
    player->play(file);
    this->state=1;
}

void PlayerWin::setTotalSlices(int i)
{
    this->totalSlices=i;
}

void PlayerWin::showSubtitleWidget(bool b)
{
    if(b)
    {
        subtitleWidget->show();
        subtitleWidget->reset();
        subtitleWidget->makeSubReady(player->file());
    }else
    {
        subtitleWidget->hide();
    }

}

