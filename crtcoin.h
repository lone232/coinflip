#ifndef CRTCOIN_H
#define CRTCOIN_H

#include <QObject>
#include "crtbutton.h"

class CRTCoin  : public CRTButton
{
    Q_OBJECT
public:
    explicit CRTCoin(QWidget *parent = nullptr);
    CRTCoin(QPoint bpos, QSize size, int currentFrame, int frameCount, QString spriteSheet, CRTEffect *parent = nullptr);
    int currentFrame;
    bool shouldFlip = false;
    bool doNotFlip = false;
    enum class CoinStatus: uint8_t{Gold, Silver, Fliping};
    CoinStatus coinStatus;
    int i, j;
    void updateCoinStatus();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override {


        if(this->coinStatus == CoinStatus::Fliping || doNotFlip){
            event->ignore();

        } else CRTButton::mousePressEvent(event);


    }
    void mouseReleaseEvent(QMouseEvent *event) override {

        if(this->coinStatus == CoinStatus::Fliping || doNotFlip){
            event->ignore();

        } else CRTButton::mouseReleaseEvent(event);

    }
private:

    QString spriteSheetPath;
    //QPoint btnPos;
    QImage SheetPng;
    int frameWidth;
    int frameHeight;
    int frameCount;
    bool toSliver;

signals:

    void flipDone();

};

#endif // CRTCOIN_H
