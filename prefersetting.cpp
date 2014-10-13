#include "prefersetting.h"
#include <QSettings>
#include <QListWidgetItem>
#include <QInputDialog>
PreferSetting::PreferSetting(QWidget *parent)
{
    mainLayout =  new QGridLayout(this);
    qualityListWidget =  new QListWidget;
    qualityListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qualityUpBtn =  new QPushButton("↑");
    qualityDownBtn =  new QPushButton("↓");
    qualityAddBtn =  new QPushButton("添加");
    qualityDeleteBtn =  new QPushButton("删除");
    qualityResetBtn =new QPushButton("重置");
    qualitySaveBtn =  new QPushButton("保存");
    mainLayout->addWidget(qualityListWidget,0,0,5,2);
    mainLayout->addWidget(qualityUpBtn,5,0,1,1);
    mainLayout->addWidget(qualityDownBtn,5,1,1,1);
    mainLayout->addWidget(qualityAddBtn,6,0,1,1);
    mainLayout->addWidget(qualityDeleteBtn,6,1,1,1);
    mainLayout->addWidget(qualityResetBtn,7,0,1,1);
    mainLayout->addWidget(qualitySaveBtn,7,1,1,1);


    connect(qualityUpBtn,SIGNAL(clicked()),this,SLOT(UpQuality()));
    connect(qualityDownBtn,SIGNAL(clicked()),this,SLOT(DownQuality()));
    connect(qualityDeleteBtn,SIGNAL(clicked()),this,SLOT(DeleteQuality()));
    connect(qualityAddBtn,SIGNAL(clicked()),this,SLOT(AddQuality()));
    connect(qualityResetBtn,SIGNAL(clicked()),this,SLOT(ResetQuality()));
    connect(qualitySaveBtn,SIGNAL(clicked()),this,SLOT(saveQuality()));

    QSettings settings("MvGather", "xusongjie");
    preferQualitysSetting = settings.value("app/preferQualitys", "").toString();
    if(preferQualitysSetting =="")
    {
        preferQualitysSetting="高清#720P#1080P#标清#低清#手机标清#超清#MP4格式M3U8#高清FLV#高清MP4#高清M3U8#超清MP4#超清M3U8#超清FLV#FLV格式M3U8#FLV标清#M3U8";
    }
    qualityListWidget->addItems(preferQualitysSetting.split("#"));
    qualityListWidget->setCurrentRow(0);
}

void PreferSetting::saveQuality()
{
    QStringList list_temp;
    int totalRows = qualityListWidget->count();
    for(int i=0;i<totalRows;i++)
    {
        list_temp.append(qualityListWidget->item(i)->text());
    }
    QSettings settings("MvGather", "xusongjie");
    settings.setValue("app/preferQualitys", list_temp.join("#"));
    this->close();
}

void PreferSetting::UpQuality()
{
    int currentItemIndex = qualityListWidget->currentRow();
    if(currentItemIndex <= 0)
    {
        return;
    }
    QListWidgetItem *currentItem= qualityListWidget->takeItem(qualityListWidget->currentRow());
    qualityListWidget->insertItem(currentItemIndex-1,currentItem);
    qualityListWidget->setCurrentItem(currentItem);
}

void PreferSetting::DownQuality()
{
    int currentItemIndex = qualityListWidget->currentRow();
    if(currentItemIndex >= qualityListWidget->count()-1||currentItemIndex<0 )
    {
        return;
    }
    QListWidgetItem *currentItem= qualityListWidget->takeItem(qualityListWidget->currentRow());
    qualityListWidget->insertItem(currentItemIndex+1,currentItem);
    qualityListWidget->setCurrentItem(currentItem);
}

void PreferSetting::DeleteQuality()
{
    qualityListWidget->takeItem(qualityListWidget->currentRow());
}

void PreferSetting::AddQuality()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("请输入"),
                                         tr("视频清晰度名称:"), QLineEdit::Normal,
                                         "高清", &ok);
    if (ok && !text.isEmpty())
        qualityListWidget->addItem(text);
}

void PreferSetting::ResetQuality()
{
    qualityListWidget->clear();
    preferQualitysSetting="高清#720P#1080P#标清#低清#手机标清#超清#MP4格式M3U8#高清FLV#高清MP4#高清M3U8#超清MP4#超清M3U8#超清FLV#FLV格式M3U8#FLV标清#M3U8";
    qualityListWidget->addItems(preferQualitysSetting.split("#"));
    qualityListWidget->setCurrentRow(0);
}
