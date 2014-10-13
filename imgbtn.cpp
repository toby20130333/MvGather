#include "imgbtn.h"

ImgBtn::ImgBtn(QWidget *parent, QString picPath) :
    QLabel(parent)
{
    setScaledContents(true);
    setPixmap(QPixmap(picPath));
}

void ImgBtn::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();
}
