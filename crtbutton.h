#ifndef CRTBUTTON_H
#define CRTBUTTON_H

#include <QWidget>
#include <QPushButton>
#include "crteffect.h"
#include <QMouseEvent>
#include <QApplication>
#include <QTimer>
#include <QCache>

class CRTButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CRTButton(QWidget *parent = nullptr);
    CRTButton(QPoint bpos, QSize size, QString normalImage, QString pressImage = "",  CRTEffect *parent = nullptr);



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
    // void setSize(int width, int height){
    //     this->width = width;
    //     this->height = height;
    // }



private:
    // QImage applyChromaticAberration(const QImage &src);
    // QImage applyCurvature(const QImage &src);
    // QRect transPos(QImage src);

    QPoint inverseMapping(QPoint);
    QPoint cachedInverseMapping(QPoint pt);

    bool isPointInsideImage(const QPoint &point) {
        // 获取按钮的图标
        QPixmap pixmap = icon().pixmap(size());
        if (pixmap.isNull()) return false;

        // 检查点击位置是否在图片的非透明区域内
        QColor color = pixmap.toImage().pixelColor(point);
        return color.alpha(); // 如果是透明区域，返回 false
    }

    // QCache<QPoint, QPoint> coordCache;
    // QPoint btnPos;
    // QString normalImagePath;
    // QString pressImagePath;

    // float bend = 0.25f; // 弯曲强度
    // QPoint screenCenter = QPoint(299,333) ;

    // int width = 200, height = 100;

    // bool isHovered = false;
    // bool isPressed = false;


protected:
    QCache<QPoint, QPoint> coordCache;
    QPoint btnPos;
    QString normalImagePath;
    QString pressImagePath;

    float bend = 0.25f; // 弯曲强度
    QPoint screenCenter = QPoint(299,333) ;

    int width = 200, height = 100;

    bool isHovered = false;
    bool isPressed = false;


    QImage applyChromaticAberration(const QImage &src);
    QImage applyCurvature(const QImage &src);
    QRect transPos(QImage src);
    void drawScanlines(QPainter &painter);
    void drawVignette(QPainter &painter);
    void drawNoise(QPainter &painter);



    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 检查点击位置是否在图片区域内
            if (isPointInsideImage(event->pos())) {
                // 调用默认的点击处理
                //emit pressed();
                qDebug() << "pressed";
                if(pressImagePath != "") isPressed = true;
                QPushButton::mousePressEvent(event);
            } else {
                // 忽略点击事件
                event->ignore();

            }
        }
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 检查点击位置是否在图片区域内
            if (isPointInsideImage(event->pos())) {
                // 调用默认的点击处理
                // emit pressed();
                // qDebug() << "pressed";
                QTimer::singleShot(200, this, [=](){
                    isPressed = false;
                });

                //emit released();
                QPushButton::mouseReleaseEvent(event);
            } else {
                // 忽略点击事件
                event->ignore();

            }
        }
    }
    void enterEvent(QEnterEvent* event) override {
        isHovered = true;
        update(); // 触发重绘
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        isHovered = false;
        update(); // 触发重绘
        QWidget::leaveEvent(event);
    }

    void paintEvent(QPaintEvent *) override;
signals:
    //void pressed();
};

#endif // CRTBUTTON_H
