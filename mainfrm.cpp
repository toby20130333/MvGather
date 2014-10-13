#include "mainfrm.h"
#include <QDir>
#include <QSqlQuery>
#include <QDebug>
#include <QMenu>
#include <QCursor>
#include <QSettings>
#include <QContextMenuEvent>
#include <QTimer>//
#include <QDateTime>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QClipboard>
#include <QApplication>
#include <QToolButton>
#include "prefersetting.h"
#include <QFileDialog>
#include <QCursor>
MainFrm::MainFrm(QWidget *parent)
    : QWidget(parent)
{
    setWindowState(Qt::WindowMaximized);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setAcceptDrops(true);
    setWindowIcon(QIcon("://images/icon.png"));
    setWindowTitle("影视集结号2.5.0");//
    this->version = "2.5.0";

    isDownLoadManagerFirstLoad =true;

    QDir dir;
    QDir dir2(dir.homePath()+"/视频");
    QDir dir3(dir.homePath()+"/Videos");
    QString dbPath;
    if(dir2.exists())
    {
        dbPath = dir.homePath()+"/视频/MvGather/Database";
    }else if(dir3.exists())
    {
        dbPath = dir.homePath()+"/Videos/MvGather/Database";
    }else
    {
        dbPath = dir.homePath()+"/MvGather";
    }
    dir.mkpath(dbPath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//添加数据库驱动，这里用sqlite
    db.setDatabaseName(dbPath+"/MvGather.db");
    //    db.setDatabaseName("MvGather.db");
    if(db.open())
    {
        //tvId:该视频唯一编号,tvName:视频中文名.tvno_hrefs:集数与相应地址...historyNo:上次观看到的集数;quality:清晰度;tvUrl:yunfan视频列表地址;source:视频来源标识
        QSqlQuery query_creat_tb("CREATE TABLE IF NOT EXISTS playlistTB(tvId VARCHAR( 30 ) NOT NULL,tvName VARCHAR( 30 ),tvno_hrefs VARCHAR( 30 ),historyNo VARCHAR( 30 ),quality VARCHAR( 30 ),tvUrl VARCHAR( 30 ),source VARCHAR( 30 ))");
        query_creat_tb.exec();
    }

    QSettings settings("MvGather", "xusongjie");
    QString preferQualitysSetting = settings.value("app/preferQualitys", "").toString();
    if(preferQualitysSetting =="")
    {
        preferQualitysSetting="高清#720P#1080P#标清#低清#手机标清#超清#MP4格式M3U8#高清FLV#高清MP4#高清M3U8#超清MP4#超清M3U8#超清FLV#FLV格式M3U8#FLV标清#M3U8";
        settings.setValue("app/preferQualitys",preferQualitysSetting);
    }

    tabBtnPlayer = new QToolButton(this);
    tabBtnBrowser = new QToolButton(this);
    tabBtnRecommend = new QToolButton(this);
    tabBtnDownLoadManager = new QToolButton(this);
    tabBtnGroup = new QButtonGroup(this);
    browseWidget = new BrowseWidget(this);
    playerWidget = new PlayerWidget(this);
    recommendWidget = new RecommendWidget(this);
    downloadManageWidget = new QWidget(this);
    downloadManageScrollArea = new QScrollArea(this);
    downloadManageScrollAreaWidget = new QWidget(this);
    downloadManageScrollAreaWidget->setMinimumSize(400,200);

    mainLayout = new QGridLayout(this);
    mainStackLayout = new QStackedLayout;
    downloadManageMainLayout = new QVBoxLayout;
    downloadManageScrollAreaWidgetMainLayout = new QVBoxLayout;
    downloadManageScrollAreaWidgetMainLayout->setAlignment(Qt::AlignTop);
    downloadManageWidget->setLayout(downloadManageMainLayout);
    downloadManageMainLayout->addWidget(downloadManageScrollArea);

    downloadManageScrollArea->setWidget(downloadManageScrollAreaWidget);
    //downloadManageScrollArea->setWidgetResizable(true);
    downloadManageScrollAreaWidget->setLayout(downloadManageScrollAreaWidgetMainLayout);

    tabBtnPlayer->setCheckable(true);
    tabBtnPlayer->setChecked(true);
    tabBtnPlayer->setText("播放器");
    tabBtnPlayer->setIcon(QIcon("://images/player.png"));
    tabBtnPlayer->setIconSize(QSize(50,50));

    tabBtnBrowser->setCheckable(true);
    //tabBtnBrowser->setChecked(true);
    tabBtnBrowser->setText("视频库");
    tabBtnBrowser->setIcon(QIcon("://images/list.png"));
    tabBtnBrowser->setIconSize(QSize(50,50));

    tabBtnRecommend->setCheckable(true);
    tabBtnRecommend->setText("搜索");
    tabBtnRecommend->setIcon(QIcon("://images/share.png"));
    tabBtnRecommend->setIconSize(QSize(50,50));


    tabBtnDownLoadManager->setCheckable(true);
    tabBtnDownLoadManager->setText("下载管理");
    tabBtnDownLoadManager->setIcon(QIcon("://images/downloadManage.png"));
    tabBtnDownLoadManager->setIconSize(QSize(50,50));


    //    tabBtnCollection = new QToolButton;
    //    tabBtnCollection->setCheckable(true);
    //    tabBtnCollection->setText("收藏夹");
    //    tabBtnCollection->setIcon(QIcon("://images/collection.png"));
    //    tabBtnCollection->setIconSize(QSize(50,50));


    tabBtnGroup->setExclusive(true);
    tabBtnGroup->addButton(tabBtnPlayer,0);
    tabBtnGroup->addButton(tabBtnBrowser,1);
    tabBtnGroup->addButton(tabBtnRecommend,2);
    tabBtnGroup->addButton(tabBtnDownLoadManager,3);
    //    tabBtnGroup->addButton(tabBtnCollection,3);


    connect(browseWidget,SIGNAL(play(QString)),this,SLOT(addMvToPlaylist(QString)));

    connect(playerWidget,SIGNAL(hideToFullScreen(bool)),this,SLOT(getIntofullScreenMode(bool)));
    connect(playerWidget,SIGNAL(getIntoWideModel(bool)),this,SLOT(getIntoWideModel(bool)));

    //connect(recommendWidget,SIGNAL(switchToPlayerAndPlay(QString)),this,SLOT(switchToPlayerAndPlay(QString)));
    connect(recommendWidget,SIGNAL(addDownloadTaskSignal(QString)),this,SLOT(addDownloadTask(QString)));


    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
    mainLayout->addWidget(tabBtnPlayer,0,0,1,1);
    mainLayout->addWidget(tabBtnBrowser,1,0,1,1);
    mainLayout->addWidget(tabBtnRecommend,2,0,1,1);
    mainLayout->addWidget(tabBtnDownLoadManager,3,0,1,1);
    //    mainLayout->addWidget(tabBtnCollection,3,0,1,1);
    mainLayout->addLayout(mainStackLayout,0,1,5,1);
    connect(tabBtnGroup,SIGNAL(buttonClicked(int)),mainStackLayout,SLOT(setCurrentIndex(int)));
    connect(tabBtnGroup,SIGNAL(buttonClicked(int)),this,SLOT(firstLoadList(int)));
    //setLayout(mainLayout);

    mainStackLayout->setMargin(5);
    mainStackLayout->setSpacing(5);
    mainStackLayout->addWidget(playerWidget);
    mainStackLayout->addWidget(browseWidget);
    mainStackLayout->addWidget(recommendWidget);
    mainStackLayout->addWidget(downloadManageWidget);
    mainStackLayout->setCurrentIndex(0);

    playerWidget->installEventFilter(this);
    browseWidget->installEventFilter(this);
    recommendWidget->installEventFilter(this);
    tabBtnPlayer->installEventFilter(this);
    tabBtnBrowser->installEventFilter(this);
    tabBtnRecommend->installEventFilter(this);

    timeToCheckV();

    hideMouseTimer = new QTimer;
    connect(hideMouseTimer,SIGNAL(timeout()),this,SLOT(hideMouse()));
    hideMouseTimer->start(500);

    loadDownloadSettings();

}

MainFrm::~MainFrm()
{

}

void MainFrm::closeEvent(QCloseEvent *)
{
    playerWidget->writeQuitPro();
    exit(0);
}

void MainFrm::contextMenuEvent(QContextMenuEvent *event)
{
    if((event->globalX()-this->x()) < 70)
    {
        QMenu *menu= new QMenu;
        QAction *downLoadManagerAct = new QAction(tr("断点续传"), this);

        QAction *themeAct = new QAction(tr("主题"), this);
        QAction *themeSystemAct = new QAction(tr("使用系统主题"), this);
        QAction *themeDarkAct = new QAction(tr("使用暗色主题"), this);
        QAction *themeLiteBlueAct = new QAction(tr("使用淡蓝色主题"), this);
        QMenu *themeMenu = new QMenu;
        themeMenu->addAction(themeSystemAct);
        themeMenu->addAction(themeDarkAct);
        //themeMenu->addAction(themeLiteBlueAct);
        themeAct->setMenu(themeMenu);
        QAction *preferSetAct = new QAction(tr("偏好设置"), this);
        QAction *checkVAct = new QAction(tr("检查更新"), this);//latest_version=2.1.0

        connect(downLoadManagerAct, SIGNAL(triggered()), this, SLOT(downLoadManager()));
        connect(themeSystemAct, SIGNAL(triggered()), this, SLOT(setThemeSystem()));
        connect(themeDarkAct, SIGNAL(triggered()), this, SLOT(setThemeDark()));
        connect(themeLiteBlueAct, SIGNAL(triggered()), this, SLOT(setThemeLiteBlue()));
        connect(preferSetAct, SIGNAL(triggered()), this, SLOT(setPreferSettings()));
        connect(checkVAct, SIGNAL(triggered()), this, SLOT(checkVersion()));

        menu->addAction(downLoadManagerAct);
        menu->addAction(themeAct);
        menu->addAction(preferSetAct);
        //menu->addAction(themeLiteBlueAct);
        menu->addAction(checkVAct);
        menu->exec(QCursor::pos());
    }

}
bool MainFrm::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Space) {
            if(playerWidget->playerWin->state==1)
            {
                playerWidget->playerWin->setState(2);
            }else if(playerWidget->playerWin->state==2)
            {
                playerWidget->playerWin->setState(1);
            }
            return true;
        }else if(keyEvent->key() == Qt::Key_Left)
        {
            playerWidget->playerWin->player->seekBackward();
            return true;
        }else if(keyEvent->key() == Qt::Key_Right)
        {
            playerWidget->playerWin->player->seekForward();
            return true;
        }
    }
    return QWidget::eventFilter(target, event);
}

void MainFrm::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void MainFrm::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty()) {
        return;
    }
    play(fileName);
}

void MainFrm::addMvToPlaylist(QString tvUrl)
{
    selectSourceWidget = new SelectSourceWidget(tvUrl);
    //connect(selectSourceWidget,SIGNAL(updatePlayList()),playerWidget,SLOT(updatePlayList()));
    connect(selectSourceWidget,SIGNAL(updatePlayList()),this,SLOT(setStackLayoutIndex()));
}

void MainFrm::getIntofullScreenMode(bool b)
{

    tabBtnPlayer->setVisible(b);
    tabBtnBrowser->setVisible(b);
    //    tabBtnCollection->setVisible(b);
    tabBtnRecommend->setVisible(b);
    tabBtnDownLoadManager->setVisible(b);
    if(b==false)//设置了隐藏
    {
        this->showFullScreen();
        mainLayout->setMargin(0);
    }else
    {
        this->showMaximized();
        mainLayout->setMargin(5);
    }
    repaint();

}

void MainFrm::getIntoWideModel(bool b)
{
    tabBtnPlayer->setVisible(b);
    tabBtnBrowser->setVisible(b);
    tabBtnRecommend->setVisible(b);
    tabBtnDownLoadManager->setVisible(b);
    if(b==false)//设置了隐藏
    {
        mainLayout->setMargin(0);
    }else
    {
        mainLayout->setMargin(5);
    }
    repaint();

}

void MainFrm::switchToPlayerAndPlay(QString fileSavePath)
{
    setStackLayoutIndex();
    playerWidget->playerWin->player->stop();
    playerWidget->playerWin->player->play(fileSavePath);
    playerWidget->playerWin->state = 1;
}

void MainFrm::setStackLayoutIndex()
{
    mainStackLayout->setCurrentIndex(0);
    tabBtnPlayer->setChecked(true);
    playerWidget->updatePlayList();
}

void MainFrm::setThemeSystem()
{
    QSettings settings("MvGather", "xusongjie");
    settings.setValue("app/theme", 0);
    settings.sync();
    qApp->setStyleSheet("");
}

void MainFrm::setThemeDark()
{
    QSettings settings("MvGather", "xusongjie");
    settings.setValue("app/theme", 1);
    settings.sync();
    QFile file("://darkGrey.qss");
    if(file.open(QFile::ReadOnly))
    {
        qApp->setStyleSheet(file.readAll());
    }
}

void MainFrm::setThemeLiteBlue()
{
    QSettings settings("MvGather", "xusongjie");
    settings.setValue("app/theme", 2);
    settings.sync();
    QFile file("://liteBlueTheme.qss");
    if(file.open(QFile::ReadOnly))
    {
        qApp->setStyleSheet(file.readAll());
    }
}

void MainFrm::setPreferSettings()
{
    PreferSetting *p= new PreferSetting;
    QDesktopWidget *desk=QApplication::desktop();
    int wd=desk->width();
    int ht=desk->height();
    p->show();
    p->move((wd-width())/2,(ht-height())/2);

}

void MainFrm::checkVersion()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl("http://mvgather.com/mvgather/checkVersion.php")));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray data = reply->readAll();
    if(data == "")
    {
        //        QMessageBox msgBox;
        //        msgBox.setText("检查更新失败，请检查网络状况，或到软件主页下载。");
        //        msgBox.exec();
        myTipWin.show();
        myTipWin.setText("检查更新失败，请检查网络状况，或到软件主页下载。");
        myTipWin.timeToHide(3);

    }else
    {
        QString v = QString(data.split('=').value(1,""));
        QString vLatest = v;
        QString vThis = version;
        vLatest.replace(".","");
        vThis.replace(".","");
        if(vLatest>vThis)
        {
            //有新版本
            QMessageBox::StandardButton rb  = QMessageBox::question(this, tr("询问"),
                                                                    tr("发现新版本.\n"
                                                                       "您想要下载更新吗？"),
                                                                    QMessageBox::Cancel | QMessageBox::Ok);
            if(rb == QMessageBox::Ok)
            {

                QNetworkReply *reply1 = manager->get(QNetworkRequest(QUrl("http://mvgather.com/mvgather/downloadAddress.php")));
                QEventLoop loop1;
                QObject::connect(reply1, SIGNAL(finished()), &loop1, SLOT(quit()));
                loop1.exec();
                QByteArray data = reply1->readAll();
                if(! QDesktopServices::openUrl(QUrl(QString(data), QUrl::TolerantMode)))
                {
                    //                    QMessageBox msgBox;
                    //                    msgBox.setText("调用浏览器失败，下载地址已复制到剪贴板！");
                    //                    msgBox.exec();
                    myTipWin.show();
                    myTipWin.setText("调用浏览器失败.");
                    myTipWin.timeToHide(3);
                    QClipboard *board = QApplication::clipboard();
                    board->setText(QString(data));
                }

            }
        }else
        {
            //            QMessageBox msgBox;
            //            msgBox.setText(QString("当前已是最新版本：%1").arg(v));
            //            msgBox.exec();
            myTipWin.show();
            myTipWin.setText(QString("当前已是最新版本：%1").arg(v));
            myTipWin.timeToHide(3);
        }

    }
    QSettings settings("MvGather", "xusongjie");
    qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
    settings.setValue("app/checkTime", nowTime);
}

void MainFrm::timeToCheckV()
{
    QSettings settings("MvGather", "xusongjie");
    qint64 LastCheckTime=settings.value("app/checkTime", 0).toLongLong();
    qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
    if(nowTime > (LastCheckTime +7*86400000))
    {
        QTimer::singleShot(10000,this,SLOT(checkVersion()));
    }
}

void MainFrm::downLoadManager()
{
    QString settingFilePath = QFileDialog::getOpenFileName(0,"选择停止时保存的配置文件","~/","setting file (*.setting)");
    if(settingFilePath.isEmpty()) return;
    downLoadWidget = new Widget(settingFilePath);
    connect(downLoadWidget,SIGNAL(play(QString)),SLOT(switchToPlayerAndPlay(QString)));
    //downLoadWidget->show();
    downloadManageScrollAreaWidgetMainLayout->addWidget(downLoadWidget);
    downloadManageScrollAreaWidget->resize(810,150*downloadManageScrollAreaWidgetMainLayout->count());
}

void MainFrm::play(QString fileSavePath)
{
    switchToPlayerAndPlay(fileSavePath);
}

void MainFrm::firstLoadList(int i)
{

    if(browseWidget->isFirstLoad && i==1)
    {
        browseWidget->updateShowWidget();
        browseWidget->isFirstLoad = false;
    }else if(recommendWidget->isFirstLoad && i==2)
    {
        recommendWidget->loadData();
        recommendWidget->isFirstLoad = false;
    }/*else if(isDownLoadManagerFirstLoad && i==3)
    {
        isDownLoadManagerFirstLoad = false;
        loadDownloadSettings();
    }*/
}

void MainFrm::addDownloadTask(QString url)
{
    QStringList urlList = url.split("#");
    foreach (QString urlForDownload, urlList) {
        QDir dir;
        QDir dir2(dir.homePath()+"/视频");
        QDir dir3(dir.homePath()+"/Videos");
        QString downloadPath;
        if(dir2.exists())
        {
            downloadPath = dir.homePath()+"/视频/MvGather/download";
        }else if(dir3.exists())
        {
            downloadPath = dir.homePath()+"/Videos/MvGather/download";
        }else
        {
            downloadPath = dir.homePath()+"/MvGather/download";
        }
        QString fileName = urlForDownload.split("=").last().split("/").last();
        QString fileSavePath=QString("%0/%1").arg(downloadPath).arg(fileName);

        downLoadWidget = new Widget(0,url,fileSavePath);
        connect(downLoadWidget,SIGNAL(play(QString)),SLOT(play(QString)));
        downloadManageScrollAreaWidgetMainLayout->addWidget(downLoadWidget);
        downloadManageScrollAreaWidget->resize(810,150*downloadManageScrollAreaWidgetMainLayout->count());
        downloadingSettings.append(fileSavePath+".setting#");
    }

}

void MainFrm::loadDownloadSettings()
{
    QSettings settings("MvGather", "xusongjie");
    QString downloadSettings = settings.value("app/downloadSettings", "").toString();
    QStringList downloadSettingsList = downloadSettings.split("#");
    downloadSettingsList.removeAll("");
    qDebug()<<downloadSettingsList;
    QString downloadSettings_temp;
    foreach (QString settingPath, downloadSettingsList) {
        QFile settingFile(settingPath);
        if(settingFile.exists())
        {
            if(!downloadingSettings.contains(settingPath))
            {
                downLoadWidget = new Widget(settingPath);
                //qDebug()<<settingPath;
                connect(downLoadWidget,SIGNAL(play(QString)),SLOT(switchToPlayerAndPlay(QString)));
                //downLoadWidget->show();
                downloadManageScrollAreaWidgetMainLayout->addWidget(downLoadWidget);
                downloadManageScrollAreaWidget->resize(810,150*downloadManageScrollAreaWidgetMainLayout->count());
            }
            downloadSettings_temp.append(settingPath+"#");
        }else
        {
            //
            //删除相关文件
            QFile file;
            QFileInfo finfo(settingPath);
            QString dirName = finfo.absolutePath();

            file.setFileName(settingPath);
            file.remove();
            file.setFileName(settingPath.replace(".setting",""));
            file.remove();

            QDir dir;
            dir.rmdir(dirName);

        }
    }
    settings.setValue("app/downloadSettings",downloadSettings_temp);
}

void MainFrm::hideMouse()
{
    QPoint pos = QCursor::pos();
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskRect = desktopWidget->availableGeometry();
    int screenW = deskRect.width();
    if(playerWidget->playerWin->isFullScreen && pos.x()>screenW*0.95)
    {
        playerWidget->playerWin->renderer->setCursor(Qt::BlankCursor);
    }else
    {
        playerWidget->playerWin->renderer->setCursor(Qt::ArrowCursor);
    }

}
