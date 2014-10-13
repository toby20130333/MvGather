#ifndef SELECTSOURCEWIDGET_H
#define SELECTSOURCEWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QGridLayout>
#include <QToolButton>
#include <QButtonGroup>
#include "mytip.h"
class SelectSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SelectSourceWidget(QString tvUrl="");

signals:
    void selectChange(QString tvName,QStringList serials);//来源名称及剧集列表,serials:剧集数#剧集地址
    void updatePlayList();
public slots:
    void processData(QByteArray data);
    void addTvToPlaylistTB(int i);//将视频名、地址等信息写入库，并让list更新
private:
    QString tvUrl;
    QNetworkAccessManager *manager;
    QGridLayout *mainLayout;
    QToolButton *btn;
    QButtonGroup *btnGroup;
    QString tvName;
    QStringList serials;
    QStringList tvNoHrefList;
    MyTip myTipWin;
};

#endif // SELECTSOURCEWIDGET_H
