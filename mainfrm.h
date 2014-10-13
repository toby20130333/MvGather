#ifndef MAINFRM_H
#define MAINFRM_H

#include <QWidget>
#include "PlayerWidget/playerwidget.h"
#include "BrowseWidget/browsewidget.h"
#include <QStackedLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QButtonGroup>
#include "selectsourcewidget.h"
#include <QSqlDatabase>
#include <QKeyEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkReply>
#include <QEventLoop>
#include "RecommendWidget/recommendwidget.h"
#include "ThreadDownload/widget.h"
#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include <QDropEvent>
#include <QMimeData>
#include "mytip.h"

class MainFrm : public QWidget
{
    Q_OBJECT

public:
    MainFrm(QWidget *parent = 0);
    ~MainFrm();
protected:
    void closeEvent(QCloseEvent *);
    void contextMenuEvent(QContextMenuEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
public slots:
    void addMvToPlaylist(QString tvUrl);
    void getIntofullScreenMode(bool b);
    void getIntoWideModel(bool b);
    void setStackLayoutIndex();
    void switchToPlayerAndPlay(QString fileSavePath);
    void setThemeSystem();
    void setThemeDark();
    void setThemeLiteBlue();
    void setPreferSettings();
    void checkVersion();
    void timeToCheckV();
    void downLoadManager();
    void play(QString fileSavePath);
    void firstLoadList(int i);
    void addDownloadTask(QString url);
    void loadDownloadSettings();
    void hideMouse();
private:
    PlayerWidget *playerWidget;
    BrowseWidget *browseWidget;
    RecommendWidget *recommendWidget;
    QWidget *downloadManageWidget;
    QVBoxLayout *downloadManageMainLayout;
    QGridLayout *mainLayout;
    QStackedLayout *mainStackLayout;
    QToolButton *tabBtnPlayer;
    QToolButton *tabBtnBrowser;
    QToolButton *tabBtnRecommend;
    QToolButton *tabBtnDownLoadManager;
    //QToolButton *tabBtnCollection;
    QButtonGroup *tabBtnGroup;
    SelectSourceWidget *selectSourceWidget;
    Widget *downLoadWidget;
    QString version;
    QScrollArea *downloadManageScrollArea;
    QWidget *downloadManageScrollAreaWidget;
    QVBoxLayout *downloadManageScrollAreaWidgetMainLayout;
    bool isDownLoadManagerFirstLoad;
    QString downloadingSettings;
    QTimer *hideMouseTimer;
    MyTip myTipWin;

};

#endif // MAINFRM_H
