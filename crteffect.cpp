#include "crteffect.h"

#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QRadialGradient>
#include <QRandomGenerator>

#include "crtbutton.h"



CRTEffect::CRTEffect(QWidget *parent)
{

    // 初始化动画定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        update();
        //child update
        foreach (CRTButton* childWidget, findChildren<CRTButton*>()) {
            childWidget->update();
            //qDebug() << "Updated child widget:" << childWidget->objectName();
        }
    });
    timer->start(100); // 控制扫描线动画速度

    //setCenter(this->size().);
    qDebug() << "crt" << center;
}


void CRTEffect::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, QImage(":/res/bkg_framework"));

    // 原始内容绘制到缓冲区
    QImage buffer(size(), QImage::Format_ARGB32);
    //buffer.fill(Qt::red);

    drawContent(buffer); // 这里绘制你的实际内容
    setCenter(buffer.rect().center());
    //qDebug() << "buffer" << buffer.size();

    // 应用颜色通道偏移
    QImage processed = applyChromaticAberration(buffer);

    // 应用曲面变形
    processed = applyCurvature(processed);

    // 绘制处理后的图像
    painter.drawImage(0, 0, processed);

    //painter.fillRect(80,80,400,500,QColor(0,0,0,200));
    // 叠加扫描线效果
    drawScanlines(painter);

    // 添加暗角效果
    drawVignette(painter);



    // 添加随机噪点
    //drawNoise(painter);


}


void CRTEffect::drawContent(QImage &buffer)
{
    // 在此处绘制你的实际内容，示例中绘制一个渐变
    QPainter p(&buffer);
    QImage title(":/res/title.png");

    p.fillRect(buffer.rect(), QColor(78,106,91));
    //p.fillRect(buffer.rect(), QColor(90,108,108));

    p.drawImage(QPoint(0,-70),title);


}

QImage CRTEffect::applyChromaticAberration(const QImage &src)
{
    QImage result(src.size(), src.format());
    result.fill(Qt::transparent);

    QPainter p(&result);


    // 提取红、绿、蓝三个通道
    QImage redChannel = CRTEffect::extractChannel(src, true, false, false);
    QImage greenChannel = CRTEffect::extractChannel(src, false, true, false);
    QImage blueChannel = CRTEffect::extractChannel(src, false, false, true);

    // 设置叠加模式（颜色值相加）
    p.setCompositionMode(QPainter::CompositionMode_Plus);

    // 绘制红色通道（左移2像素）
    p.drawImage(QPoint(-2, 0), redChannel);

    // 绘制绿色通道（原位置）
    p.drawImage(QPoint(0, 0), greenChannel);

    // 绘制蓝色通道（右移2像素）
    p.drawImage(QPoint(2, 0), blueChannel);

    //qDebug() << "btn_ChromaticAberration_Success";
    return result;
}

QImage CRTEffect::applyCurvature(const QImage &src)
{
    QImage distorted(src.size(), src.format());
    distorted.fill(Qt::transparent);



    for (int y = 0; y < src.height(); ++y) {
        for (int x = 0; x < src.width(); ++x) {
            // 计算归一化坐标
            float nx = (x - center.x()) / static_cast<float>(center.x());
            float ny = (y - center.y()) / static_cast<float>(center.y());

            // 应用弯曲变形
            float dx = nx * (1.0f + bend * (nx*nx + ny*ny));
            float dy = ny * (1.0f + bend * (nx*nx + ny*ny));

            // 转换回图像坐标
            int sx = static_cast<int>(center.x() * (dx + 1));
            int sy = static_cast<int>(center.y() * (dy + 1));

            if (QRect(QPoint(0,0), src.size()).contains(QPoint(sx, sy))) {
                distorted.setPixelColor(x, y, src.pixelColor(sx, sy));
            }
        }
    }
    return distorted;
}

void CRTEffect::drawScanlines(QPainter &painter)
{
    static int currentLine = 50;
    painter.save();
    QColor scanColor(0, 0, 80, 255); // 半透明黑色
    QRegion region(30,30,540,600);
    painter.setClipRegion(region);  // 使用图标蒙版作为绘制区域
    painter.setPen(Qt::NoPen);
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    // // 水平扫描线
    // for (int y = 0; y < height(); y += 2) {
    //     painter.fillRect(0, y, width(), 1, scanColor);
    // }
    if(currentLine > 650) currentLine = 50;
    for (int y = currentLine; y < height(); y += 2) {
        painter.fillRect(0, y, width(), 1, scanColor);
    }
    for (int y = currentLine - 2; y >= 50 ; y -= 2) {
        painter.fillRect(0, y, width(), 1, scanColor);
    }


    // 垂直间隔线（可选）
    painter.setBrush(QColor(Qt::green));
    painter.setOpacity(0.8);
    for (int x = 0; x < width(); x += 50) {
        painter.drawLine(x, 0, x, height());
    }
    painter.setClipRegion(QRegion());
    painter.restore();
}

void CRTEffect::drawVignette(QPainter &painter)
{
    QRadialGradient gradient(width()/2, height()/2, qMax(width(), height())/1.5);
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(1, QColor(0, 0, 0, 200));
    painter.fillRect(rect(), gradient);
}

void CRTEffect::drawNoise(QPainter &painter)
{
    painter.save();
    QRandomGenerator *rand = QRandomGenerator::global();
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < 10; ++i) { // 噪声点数量
        int x = rand->bounded(width());
        int y = rand->bounded(height());
        int size = rand->bounded(2);
        int alpha = rand->bounded(50) + 30;
        painter.setBrush(QColor(255, 255, 255, alpha));
        painter.drawEllipse(QPoint(x, y), size, size);
    }
    painter.restore();
}

/**
 * @brief 提取图像的单色通道（保留指定通道，其他通道置零）
 * @param src 原图
 * @param keepRed 是否保留红色通道
 * @param keepGreen 是否保留绿色通道
 * @param keepBlue 是否保留蓝色通道
 * @return 处理后的单色通道图像
 */
QImage CRTEffect::extractChannel(const QImage &src, bool keepRed, bool keepGreen, bool keepBlue) {
    QImage channel = src.convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < channel.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(channel.scanLine(y));
        for (int x = 0; x < channel.width(); ++x) {
            QRgb pixel = line[x];
            int r = keepRed ? qRed(pixel) : 0;
            int g = keepGreen ? qGreen(pixel) : 0;
            int b = keepBlue ? qBlue(pixel) : 0;
            int a = qAlpha(pixel);
            line[x] = qRgba(r, g, b, a); // 仅保留目标通道
        }
    }
    return channel;
}
