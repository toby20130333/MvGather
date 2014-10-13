#include "imagelabel.h"
#include <QPixmap>
#include <QDebug>
#include <QEventLoop>
#include <QUrl>
#include <QTimer>
ImageLabel::ImageLabel(QObject *parent, QString url, int timeToDelay)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setScaledContents(true);
    setMaximumSize(260,290);
    setMinimumSize(240,280);
    //setStyleSheet("border-width: 3px;border-color: rgba(255,170,0,150);");

    this->url=url;
    QPixmap p;
    p.load("://images/load.jpg");
    p = p.scaled(this->width(),this->height(),Qt::KeepAspectRatio);
    setPixmap(p);
    //QTimer::singleShot(timeToDelay,this,SLOT(getImage()));
    isImgLoaded =false;
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    emit clicked();
}
void ImageLabel::getImage()
{
    LoadImgThread *t1 = new LoadImgThread(0,this->url);
    connect(t1,SIGNAL(imageReady(QByteArray)),this,SLOT(readImage(QByteArray)));
    connect(t1,SIGNAL(finished()),t1,SLOT(deleteLater()));
    t1->start();
}

void ImageLabel::readImage(QByteArray data)
{
    QPixmap pix;
    if(pix.loadFromData(data))
    {
        pix = pix.scaled(this->width(),this->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        this->setPixmap(pix);

    }

}

