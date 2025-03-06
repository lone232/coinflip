#include "winscene.h"
#include <QPainter>
#include "crtbutton.h"



WinScene::WinScene(QWidget *parent)
{
    video = new videowidget(QPoint(30,10), 600 + 60, 280);
    video->setParent(this);
    connect(this, &WinScene::play, video, [=](){
        video->loadVideo(QUrl("qrc:/res/cowgirl.mp4"));
    });

    CRTButton *btnback = new CRTButton(QPoint(this->getCenter().x() - 100, this->getCenter().y() * 2 - 140), QSize(200,100), ":/res/btn_back_normal", ":/res/btn_back_pressed");
    btnback->setParent(this);
    connect(btnback, &CRTButton::released, this, [=](){
        //this->parentWidget()->hide();
        //qDebug() << "back";
        video->unloadVideo();
        QTimer::singleShot(200, this, [=](){
            emit this->back();
        });

    });
    CRTButton *btnnext = new CRTButton(QPoint(this->getCenter().x() - 100, 400), QSize(200,100), ":/res/btn_next");
    btnnext->setParent(this);
    connect(btnnext, &CRTButton::pressed, this, [=](){
        video->unloadVideo();
        QTimer::singleShot(200, this, [=](){
            emit this->next();
        });

    });







}
void WinScene::drawContent(QImage &buffer)
{
    QPainter p(&buffer);
    QImage winTitle(":/res/ttl_win.png");

    p.fillRect(buffer.rect(), QColor(78,106,91));


    p.drawImage(QPoint(0,200),winTitle);

}

