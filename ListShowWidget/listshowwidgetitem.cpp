#include "listshowwidgetitem.h"
#include <QTimer>
ListShowWidgetItem::ListShowWidgetItem(QWidget *parent, QString tvUrl, QString imgUrl, QString title, QString descripe,int timeToDelay)
{
    this->tvUrl = tvUrl;
    this->imgUrl = imgUrl;
    this->title = title;
    this->descripe = descripe;

    mainGLayout = new QGridLayout(this);
    ImgLbl = new ImageLabel(this,imgUrl,timeToDelay);
    connect(ImgLbl,SIGNAL(clicked()),this,SLOT(SendClicked()));
    titleLbl = new QLabel(title,this);
    descripeLbl = new QLabel("<font color=grey>"+descripe+"</font>",this);
    descripeLbl->setToolTip(descripe);

    mainGLayout->addWidget(ImgLbl,0,0,6,1);
    mainGLayout->addWidget(titleLbl,6,0,1,1);
    mainGLayout->addWidget(descripeLbl,7,0,1,1);

    setMaximumSize(210,350);
    setMinimumSize(190,340);
}

void ListShowWidgetItem::SendClicked()
{

    //titleLbl->setStyleSheet("background-color:#7B7B7B;radius:3px");
    ImgLbl->setStyleSheet("border:5px solid #7B7B7B");
    QTimer::singleShot(300,this,SLOT(backgroundColorChange()));
    emit clicked(this->tvUrl,this->title);

}

void ListShowWidgetItem::loadImg()
{
    ImgLbl->getImage();
}

void ListShowWidgetItem::backgroundColorChange()
{
    //titleLbl->setStyleSheet("");
    ImgLbl->setStyleSheet("");
}
