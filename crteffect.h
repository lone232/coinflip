#ifndef CRTEFFECT_H
#define CRTEFFECT_H

#include <QWidget>
#include <QImage>
#include <QHash>

class CRTEffect : public QWidget
{
    Q_OBJECT
public:
    CRTEffect(QWidget *parent = nullptr);
    float getBend(){
        return this->bend;
    };
    void setBend(float bend){
        this->bend = bend;
    };
    QPoint getCenter(){
        return this->center;
    };
    void setCenter(QPoint Center){
        this->center = Center;
    }
    static QImage extractChannel(const QImage &src, bool keepRed, bool keepGreen, bool keepBlue);

protected:
    void paintEvent(QPaintEvent *) override;
    // ... 其他成员声明
private:
    virtual void drawContent(QImage &buffer);
    QImage applyChromaticAberration(const QImage &src);
    QImage applyCurvature(const QImage &src);
    void drawScanlines(QPainter &painter);
    void drawVignette(QPainter &painter);
    void drawNoise(QPainter &painter);



    float bend = 0.25f; // 弯曲强度
    QPoint center = QPoint(299,333);




};

#endif // CRTEFFECT_H
