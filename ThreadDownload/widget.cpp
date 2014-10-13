#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
Widget::Widget(QWidget *parent, QString url, QString fileSavePath) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->deleteBtn->setPixmap(QPixmap("://images/delete.png"));
    //setWindowIcon(QIcon("://images/download.ico"));
    //setWindowTitle("MvGather多线程下载管理器1.0");
    this->url = url;
    this->fileSavePath= fileSavePath;
    QFileInfo finfo(fileSavePath);
    ui->fileNameLbl->setText(fileSavePath.split("/").last());
    ui->fileNameLbl->setToolTip(finfo.absoluteFilePath());
    ui->pauseBtn->setEnabled(false);
    ui->playBtn->setEnabled(false);

    ui->startDownBtn->setEnabled(false);
    ui->threadCountSpinBox->setEnabled(false);
    ui->pauseBtn->setEnabled(true);

    QTimer::singleShot(500,this,SLOT(nomalDownload()));
    saveSettingToSettings();
    connect(ui->deleteBtn,SIGNAL(clicked()),SLOT(deleteThisTask()));

}

Widget::Widget(QString settingFilePath):
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->deleteBtn->setPixmap(QPixmap("://images/delete.png"));
    //setWindowIcon(QIcon("://images/download.ico"));
    //setWindowTitle("MvGather多线程下载管理器1.0");//
    this->settingFilePath = settingFilePath;
    this->fileSavePath= settingFilePath.replace(".setting","");

    QFileInfo finfo(fileSavePath);
    ui->fileNameLbl->setText(fileSavePath.split("/").last());
    ui->fileNameLbl->setToolTip(finfo.absoluteFilePath());
    ui->pauseBtn->setEnabled(false);
    ui->startDownBtn->setEnabled(true);
    ui->threadCountSpinBox->setEnabled(false);
    ui->playBtn->setEnabled(false);
    connect(ui->deleteBtn,SIGNAL(clicked()),SLOT(deleteThisTask()));
    QTimer::singleShot(500,this,SLOT(settingFileDownload()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startDownBtn_clicked()
{
    ui->startDownBtn->setEnabled(false);
    ui->pauseBtn->setEnabled(true);
    control->startDownLoad();

}

void Widget::on_pauseBtn_clicked()
{
    if(ui->pauseBtn->text()=="暂停")
    {
        ui->pauseBtn->setText("继续");
        control->pauseDownLoad(true);
    }else
    {
        ui->pauseBtn->setText("暂停");
        control->pauseDownLoad(false);
    }
}

void Widget::on_progressBar_valueChanged(int value)
{
    if(value>2 && !ui->playBtn->isEnabled())
    {
        ui->playBtn->setEnabled(true);
    }
}

void Widget::on_playBtn_clicked()
{
    emit play(fileSavePath);
    //qDebug()<<fileSavePath;
}

void Widget::downLoadFinished()
{
    ui->pauseBtn->setEnabled(false);
}

void Widget::nomalDownload()
{
    myTipWin.show();
    myTipWin.setText("任务添加成功");
    myTipWin.timeToHide(3);

    control =  new DownLoadControl;
    connect(control,SIGNAL(progressChange(int)),ui->progressBar,SLOT(setValue(int)));
    connect(control,SIGNAL(speedChange(QString)),ui->speedLbl,SLOT(setText(QString)));
    connect(control,SIGNAL(downLoadFinished()),this,SLOT(downLoadFinished()));
    //connect(control,SIGNAL(retrySignal()),this,SLOT(retryDownLoad()));
    bool t = control->setDownLoadSettings(url,fileSavePath,ui->threadCountSpinBox->value());
    if(!t)
    {
        return;
    }
    control->startDownLoad();

}

void Widget::settingFileDownload()
{
    control =  new DownLoadControl;
    connect(control,SIGNAL(progressChange(int)),ui->progressBar,SLOT(setValue(int)));
    connect(control,SIGNAL(speedChange(QString)),ui->speedLbl,SLOT(setText(QString)));
    connect(control,SIGNAL(downLoadFinished()),this,SLOT(downLoadFinished()));
    //connect(control,SIGNAL(retrySignal()),this,SLOT(retryDownLoad()));
    control->startDownLoadFromSettingFile(settingFilePath);
    control->updateProgressBar();
}

void Widget::saveSettingToSettings()
{
    QSettings settings("MvGather", "xusongjie");
    QString downloadSettings = settings.value("app/downloadSettings", "").toString();
    downloadSettings.append(QString("%1#").arg(fileSavePath+".setting"));
    //qDebug()<<downloadSettings;
    settings.setValue("app/downloadSettings",downloadSettings);
    settings.sync();
}

void Widget::retryDownLoad()
{
    ui->startDownBtn->setEnabled(true);
    ui->pauseBtn->setEnabled(false);
    ui->pauseBtn->setText("暂停");

}

void Widget::deleteThisTask()
{
    control->pauseDownLoad(true);
    this->hide();

    //删除相关文件
    QFile file;
    QFileInfo finfo(fileSavePath);
    QString dirName = finfo.absolutePath();

    file.setFileName(fileSavePath);
    file.remove();
    file.setFileName(fileSavePath+".setting");
    file.remove();

    QDir dir;
    dir.rmdir(dirName);

}
