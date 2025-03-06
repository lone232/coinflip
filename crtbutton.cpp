#include "crtbutton.h"
#include <QPainter>
#include "crteffect.h"

#include <QRandomGenerator>
#include "crteffect.h"

CRTButton::CRTButton(QWidget *parent)
{

}

CRTButton::CRTButton(QPoint bpos, QSize size, QString normalImage, QString pressImage,  CRTEffect *parent)
{
    normalImagePath = normalImage;
    pressImagePath = pressImage;

    btnPos = bpos;

    width = size.width();
    height = size.height();

    if (parent) {
        setScreenCenter(parent->getCenter());
    }

    QPixmap pixMap;

    QImage buffer(normalImagePath);
    buffer = buffer.scaled(width,height);

    QImage processed = applyChromaticAberration(buffer);
    processed = applyCurvature(processed);

    //buffer = applyCurvature(buffer);


    //QPainter p(this);
    pixMap = pixMap.fromImage(processed);

    QRect area = transPos(buffer);
    this->setFixedSize(area.width(), area.height());
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(pixMap);
    this->setIconSize(QSize(area.width(),area.height()));
    this->move(area.x(),area.y());
    //this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    qDebug() << this->normalImagePath;
    qDebug() << area.x() << area.y();
    qDebug() << this->height << this->width;
    qDebug() << area.height() << area.width();

}

void CRTButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    //QPixmap pixMap;

    QImage buffer(isPressed? pressImagePath : normalImagePath);
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

QImage CRTButton::applyChromaticAberration(const QImage &src)
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

QImage CRTButton::applyCurvature(const QImage &src)
{
    QRect area = transPos(src);

    QImage distorted(area.size(), src.format());
    distorted.fill(Qt::transparent);
    QPainter p(&distorted);
    p.fillRect(QRect(QPoint(distorted.width()/2 - 10, distorted.height()/2 - 10), QSize(20,20)),distorted.pixelColor(distorted.width()/2,distorted.height()/2));
    //qDebug() << distorted.rect();

    const QPoint center = screenCenter; // 父部件的中心

    for (int y = 0; y < src.height(); ++y) {
        for (int x = 0; x < src.width(); ++x) {
            // 按钮像素在父部件中的全局坐标
            int globalX = btnPos.x() + x;
            int globalY = btnPos.y() + y;



            QPoint screenPos = cachedInverseMapping(QPoint(globalX, globalY));
            //qDebug() << screenPos;

            // 转换为按钮图像的局部坐标
            int sx = screenPos.x() - area.x();
            int sy = screenPos.y() - area.y();

            if(distorted.rect().contains(sx, sy)){
                distorted.setPixelColor(sx, sy, src.pixelColor(x, y));
            }


        }
    }
    //qDebug() << "curvature";
    return distorted;
}

QRect CRTButton::transPos(QImage src){


    QPoint tl = inverseMapping(addPoint(src.rect().topLeft() , btnPos));
    QPoint tr = inverseMapping(addPoint(src.rect().topRight() , btnPos));
    QPoint bl = inverseMapping(addPoint(src.rect().bottomLeft() , btnPos));
    QPoint br = inverseMapping(addPoint(src.rect().bottomRight() , btnPos));

    int sx = std::min(tl.x(),bl.x());
    int sy = std::min(tl.y(),tr.y());
    int sw = std::max(tr.x(),br.x());
    int sh = std::max(bl.y(),br.y());

    QRect rest(QPoint(sx, sy), QPoint(sw, sh));


    //qDebug() << rest;
    // int l = INT_MAX, r = 0, t = INT_MAX, b = 0;
    // for (int y = 0; y < src.height(); ++y) {
    //     for (int x = 0; x < src.width(); ++x) {
    //         // 按钮像素在父部件中的全局坐标
    //         int globalX = btnPos.x() + x;
    //         int globalY = btnPos.y() + y;



    //         QPoint screenPos = cachedInverseMapping(QPoint(globalX, globalY));
    //         //qDebug() << screenPos;

    //         // 转换为按钮图像的局部坐标
    //         // int sx = screenPos.x() - area.x();
    //         // int sy = screenPos.y() - area.y();
    //         l = std::min(l, screenPos.x());
    //         r = std::max(r, screenPos.x());
    //         t = std::min(t, screenPos.y());
    //         b = std::max(b, screenPos.y());


    //     }
    // }

    // QRect rest(QPoint(l, t), QPoint(r, b));
    return rest;
}

QPoint CRTButton::inverseMapping(QPoint point) {
    // int sx = point.x();
    // int sy = point.y();
    // QPoint center = this->screenCenter;
    // // 从 sx, sy 转换为归一化 dx, dy
    // float dx = (sx / static_cast<float>(center.x())) - 1.0f;
    // float dy = (sy / static_cast<float>(center.y())) - 1.0f;

    const QPoint center = screenCenter;
    float dx = (point.x() - center.x()) / (float)center.x(); // 转为归一化偏移
    float dy = (point.y() - center.y()) / (float)center.y();

    // // 映射函数加入平滑过渡
    // float r = dx*dx + dy*dy;
    // float factor = 1.0f / (1.0f + bend * r);
    // 添加最小偏移量防止除以零
    const float min_offset = 0.00001f;
    dx = qAbs(dx) < min_offset ? (dx > 0 ? min_offset : -min_offset) : dx;
    dy = qAbs(dy) < min_offset ? (dy > 0 ? min_offset : -min_offset) : dy;


    // 初始猜测 nx 和 ny
    float nx = dx;
    float ny = dy;

    const int max_iterations = 10;
    const float epsilon = 1e-5;

    for (int i = 0; i < max_iterations; ++i) {
        float r = nx * nx + ny * ny;
        float factor = 1.0f + bend * r;

        // 更新 nx 和 ny
        float nx_new = dx / (factor);
        float ny_new = dy / (factor);

        // 检查收敛性
        if (abs(nx_new - nx) < epsilon && abs(ny_new - ny) < epsilon) {
            break;
        }

        nx = nx_new;
        ny = ny_new;
    }

    // 转换为图像坐标
    int x = static_cast<int>(center.x() * (nx + 1));
    int y = static_cast<int>(center.y() * (ny + 1));

    // 确保在图像范围内
    x = qBound(0, x, center.x() * 2);
    y = qBound(0, y, center.y() * 2);

    return QPoint(x, y);
}

void CRTButton::drawScanlines(QPainter &painter)
{
    // 获取实际显示区域
    // QImage tmpBuffer(normalImagePath);
    // QRect actualArea = transPos(tmpBuffer.scaled(width, height));
    // painter.fillRect(actualArea, QBrush(QColor(255,0,0)));

    static int currentLine = 0;
    painter.save();
    QColor scanColor(0, 90, 80, 255); // 半透明黑色
    painter.setPen(Qt::NoPen);
    QBitmap mask = icon().pixmap(iconSize()).mask();
    QRegion region(mask);
    painter.setClipRegion(region);  // 使用图标蒙版作为绘制区域

    painter.setCompositionMode(QPainter::CompositionMode_Multiply);


    if(currentLine > this->height) currentLine = 0;
    for (int y = currentLine; y < height; y += 2) {
        painter.fillRect(0, y, width, 1, scanColor);
    }
    for (int y = currentLine - 2; y >= 0 ; y -= 2) {
        painter.fillRect(0, y, width, 1, scanColor);
    }


    // // 水平扫描线
    // for (int y = 0; y < height; y += 2) {

    //     painter.fillRect(0, y, width, 1, scanColor);
    // }

    // 垂直间隔线（可选）
    // painter.setOpacity(0.3);
    // for (int x = 0; x < width; x += 50) {
    //     painter.drawLine(x, 0, x, height);
    // }
    painter.setClipRegion(QRegion());  // 恢复默认绘制区域
    painter.restore();
}

void CRTButton::drawVignette(QPainter &painter)
{
    painter.save();
    QRadialGradient gradient(width/2, height/2, qMax(width, height)/1);
    gradient.setColorAt(0, isHovered? QColor(60, 200, 170, 100) : QColor(0, 10, 70, 100));
    gradient.setColorAt(1, QColor(0, 0, 0, 200));

    // 绘制暗角渐变，并限制在按钮图标区域
    QBitmap mask = icon().pixmap(iconSize()).mask();
    QRegion region(mask);
    painter.setClipRegion(region);  // 使用图标蒙版作为绘制区域

    //painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.fillRect(rect(), gradient);

    painter.setClipRegion(QRegion());  // 恢复默认绘制区域
    painter.restore();
}

void CRTButton::drawNoise(QPainter &painter)
{
    painter.save();
    QRandomGenerator *rand = QRandomGenerator::global();
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < 3; ++i) { // 噪声点数量
        int x = rand->bounded(width);
        int y = rand->bounded(height);
        int size = rand->bounded(2);
        int alpha = rand->bounded(50) + 30;
        painter.setBrush(QColor(255, 255, 255, alpha));
        painter.drawEllipse(QPoint(x, y), size, size);
    }
    painter.restore();
}
QPoint CRTButton::cachedInverseMapping(QPoint pt) {
    if (coordCache.contains(pt)) {
        return *coordCache.object(pt);
    } else {
        QPoint result = inverseMapping(pt);
        coordCache.insert(pt, new QPoint(result));
        return result;
    }
}
