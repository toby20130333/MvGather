#-------------------------------------------------
#
# Project created by QtCreator 2014-07-27T15:37:01
#
#-------------------------------------------------

QT       += core gui network sql av

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MvGather
TEMPLATE = app


SOURCES += main.cpp\
        mainfrm.cpp \
    BrowseWidget/browsewidget.cpp \
    ListShowWidget/listshowwidget.cpp \
    ListShowWidget/listshowwidgetitem.cpp \
    ListShowWidget/imagelabel.cpp \
    ListShowWidget/loadimgthread.cpp \
    PlayerWidget/playerwidget.cpp \
    selectsourcewidget.cpp \
    PlayerWidget/playerwin.cpp \
    PlayerWidget/playerlistarea.cpp \
    imgbtn.cpp \
    PlayerWidget/playerlistwidgetitem.cpp \
    PlayerWidget/progressbar.cpp \
    RecommendWidget/recommendwidget.cpp \
    prefersetting.cpp \
    ThreadDownload/downloadcontrol.cpp \
    ThreadDownload/downloadthread.cpp \
    ThreadDownload/widget.cpp \
    BrowseWidget/adshow.cpp \
    PlayerWidget/tvlistwidget.cpp \
    PlayerWidget/analyzer.cpp \
    mytip.cpp \
    SubtitleWidget/subtitlewidget.cpp

HEADERS  += mainfrm.h \
    BrowseWidget/browsewidget.h \
    ListShowWidget/listshowwidget.h \
    ListShowWidget/listshowwidgetitem.h \
    ListShowWidget/imagelabel.h \
    ListShowWidget/loadimgthread.h \
    PlayerWidget/playerwidget.h \
    selectsourcewidget.h \
    PlayerWidget/playerwin.h \
    PlayerWidget/playerlistarea.h \
    imgbtn.h \
    PlayerWidget/playerlistwidgetitem.h \
    PlayerWidget/progressbar.h \
    RecommendWidget/recommendwidget.h \
    prefersetting.h \
    ThreadDownload/downloadcontrol.h \
    ThreadDownload/downloadthread.h \
    ThreadDownload/widget.h \
    BrowseWidget/adshow.h \
    PlayerWidget/tvlistwidget.h \
    PlayerWidget/analyzer.h \
    mytip.h \
    SubtitleWidget/subtitlewidget.h

RESOURCES += \
    rs.qrc

FORMS += \
    ThreadDownload/widget.ui

OTHER_FILES += \
    ThreadDownload/download.ico
