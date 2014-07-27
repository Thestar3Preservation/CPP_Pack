#include "widget.hpp"
#include "ui_widget.h"
#include <QDesktopWidget>
#include <QProcess>
#include <QKeyEvent>
#include "singleprocess.hpp"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //이곳에 추가적인 초기화 명령을 입력합니다.

    //이 창의 크기를 가져옴.
    QSize widgetSize = this->size();

    //위치를 중앙으로 고정.
    {
        QDesktopWidget *d = QApplication::desktop();
        int w = widgetSize.width();
        int h = widgetSize.height();
        int x = ( d->width() - w ) / 2;
        int y = ( d->height() - h ) / 2;
        this->setGeometry(x, y, w, h);
    }

    //최소화, 종료 버튼을 삭제
    this->setWindowFlags(Qt::WindowTitleHint);

    //크기를 변경하지 못하도록 고정
    this->setMinimumSize(widgetSize);
    this->setMaximumSize(widgetSize);
}

void Widget::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
}

Widget::~Widget(){
    delete ui;
}

void Widget::shutdown(){
    GV_share.detach();
    QProcess::execute("shutdown -h now");
}

void Widget::reboot(){
    GV_share.detach();
    QProcess::execute("reboot");
}

void Widget::logout(){
    GV_share.detach();
    QProcess::execute("pkill -u user");
}

void Widget::switchUsers(){
    QProcess::execute("mdmflexiserver -l");
    QApplication::quit();
}
