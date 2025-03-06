#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "qpainter.h"
#include <QMediaPlayer>
#include <QWidget>
#include <QVideoWidget>
#include <QVideoFrame>
#include <QCache>

class videowidget : public QWidget
{
    Q_OBJECT
public:
    explicit videowidget(QWidget *parent = nullptr);
    videowidget(QPoint pnt,int width, int height, QWidget *parent = nullptr);
    void loadVideo(const QUrl &path) {
        player->setSource(path);
        player->setLoops(QMediaPlayer::Infinite);
        player->play();
    }
    void unloadVideo() {
        //player->pause();
        player->stop();
    }
    static QImage extractChannel(const QImage &src, bool keepRed, bool keepGreen, bool keepBlue);
    float getBend(){
        return this->bend;
    };
    void setBend(float bend){
        this->bend = bend;
    };
    QPoint getScreenCenter(){
        return this->screenCenter;
    };
    void setScreenCenter(QPoint screenCenter){
        this->screenCenter = screenCenter;
    }
    QPoint addPoint(QPoint p1, QPoint p2){
        QPoint r(p1.x() + p2.x(), p1.y() + p2.y());
        return r;

    }

public slots:

private slots:
    void handleFrame(const QVideoFrame &frame);

protected:
    QCache<QPoint, QPoint> coordCache;
    QPoint btnPos;

    void paintEvent(QPaintEvent *) override ;
    QImage applyChromaticAberration(const QImage &src);
    QImage applyCurvature(const QImage &src);
    QRect transPos(QImage src);
    void drawScanlines(QPainter &painter);
    void drawVignette(QPainter &painter);
    void drawNoise(QPainter &painter);

    //void paintEvent(QPaintEvent* event) override;
private:
    QPoint inverseMapping(QPoint);
    QPoint cachedInverseMapping(QPoint pt);

    QMediaPlayer *player;
    QImage currentFrame;
    QVideoSink *sink;

    QPoint screenCenter = QPoint(299,333) ;

    int width = 200, height = 100;

    virtual void drawContent(QImage &buffer);




    float bend = 0.25f; // 弯曲强度
    QPoint center = QPoint(299,333);


signals:
};

#endif // VIDEOWIDGET_H
