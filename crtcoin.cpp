#include "crtcoin.h"
#include <QPainter>
#include <QSoundEffect>

CRTCoin::CRTCoin(QWidget* parent){

}

CRTCoin::CRTCoin(QPoint bpos, QSize size, int currentFrame, int frameCount, QString spriteSheet, CRTEffect *parent)
    :CRTButton(parent),currentFrame(currentFrame), frameCount(frameCount), spriteSheetPath(spriteSheet)
{
    btnPos = bpos;
    width = size.width();
    height = size.height();



    if (parent) {
        setScreenCenter(parent->getCenter());
    }

    bool ret = SheetPng.load(spriteSheetPath);
    if(!ret){
        qDebug() << spriteSheet << "SpriteSheet loaded failed";
    }
    frameWidth = SheetPng.width() / frameCount;
    frameHeight = SheetPng.height();
    //qDebug() << frameWidth << frameHeight;
    QImage buffer = SheetPng.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight);
    buffer = buffer.scaled(width,height);


    QPixmap pixMap;



    QImage processed = applyChromaticAberration(buffer);
    processed = applyCurvature(processed);

    pixMap = pixMap.fromImage(processed);

    QRect area = transPos(buffer);
    this->setFixedSize(area.width(), area.height());
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(pixMap);
    this->setIconSize(QSize(area.width(),area.height()));
    this->move(area.x(),area.y());






    // qDebug() << this->normalImagePath;
    // qDebug() << area.x() << area.y();
    // qDebug() << this->height << this->width;
    // qDebug() << area.height() << area.width();
}
void CRTCoin::paintEvent(QPaintEvent *)
{
    // coinStatus = currentFrame == 0 ? CoinStatus::Gold : currentFrame == frameCount -1 ? CoinStatus::Silver : CoinStatus::Fliping;
    if(currentFrame == 0) toSliver = true;
    if(currentFrame == 6) toSliver = false;
    if(shouldFlip){


        switch(coinStatus){
            case CoinStatus::Gold:

                //currentFrame += toSliver? 1 : 0;
                currentFrame += 1;
                updateCoinStatus();
                break;
            case CoinStatus::Silver:

                //currentFrame -= toSliver? 0 : 1;
                currentFrame -= 1;
                updateCoinStatus();
                break;
            case CoinStatus::Fliping:
                currentFrame += toSliver? 1 : -1;
                updateCoinStatus();
                if(currentFrame == 0 || currentFrame == frameCount -1) {
                    shouldFlip = false;
                    emit flipDone();
                }
                break;
            default:
                break;
        }

    }


    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    //QPixmap pixMap;

    QImage buffer = SheetPng.copy(1 + currentFrame * frameWidth, 1, frameWidth, frameHeight);
    buffer = buffer.scaled(width,height);

    QImage processed = applyChromaticAberration(buffer);
    processed = applyCurvature(processed);

    //QImage processed = applyCurvature(processed);

    //QPainter p(this);
    p.drawImage(0, 0, processed);


    // 叠加扫描线效果
    drawScanlines(p);

    // 添加暗角效果
    drawVignette(p);

    // 添加随机噪点
    //drawNoise(p);

}
void CRTCoin::updateCoinStatus(){
    this->coinStatus = this->currentFrame == 0 ? CoinStatus::Gold : this->currentFrame == frameCount -1 ? CoinStatus::Silver : CoinStatus::Fliping;
}
