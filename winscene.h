#ifndef WINSCENE_H
#define WINSCENE_H

#include <QObject>
#include "crteffect.h"
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QImage>
#include <QVideoSink>
#include <QAbstractVideoBuffer>
#include <QtMultimediaWidgets/QVideoWidget>
#include "videowidget.h"





class WinScene : public CRTEffect
{
    Q_OBJECT
public:
    WinScene(QWidget *parent = nullptr);

    int currentLevelIndex = NULL;

protected:
    void drawContent(QImage &buffer) override;
private:
    QMediaPlayer *player;
    QVideoSink *sink;
    QImage currentFrame;
    videowidget *video;
   // QVideoWidget *videoWidget;

private slots:


signals:
    void back();
    void next();
    void play();
};

#endif // WINSCENE_H
