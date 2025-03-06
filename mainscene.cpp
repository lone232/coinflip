#include "mainscene.h"
#include "./ui_mainscene.h"
#include <QIcon>
#include <QPushButton>
#include <QStackedWidget>
#include <QFontDatabase>
#include <QtMultimedia/QSoundEffect>
#include <QtMultimedia/QMediaPlayer>
#include <QAudioOutput>

#include "crteffect.h"
#include "crtbutton.h"
#include "chooselevelscene.h"
#include "winscene.h"

MainScene::MainScene(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainScene)
{
    ui->setupUi(this);

    this->setFixedSize(600,700);

    this->setWindowTitle("翻金币");

    this->setWindowIcon(QIcon(":/res/coinIcon.png"));

    QStackedWidget *stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(stackedWidget); // 设置堆栈容器为中心控件

    //sounds
    QSoundEffect *sndbutton = new QSoundEffect(this);
    QSoundEffect *sndnoise = new QSoundEffect(this);
    QMediaPlayer *sndstart = new QMediaPlayer(this);
    QSoundEffect *sndwin = new QSoundEffect(this);


    QAudioOutput *audioOutput = new QAudioOutput;
    audioOutput->setVolume(0.8);

    // QAudioOutput *btnOutput = new QAudioOutput;
    // btnOutput->setVolume(0.8);

    sndbutton->setSource(QUrl("qrc:/res/crt_button.wav"));
    sndnoise->setSource(QUrl("qrc:/res/crt_noise.wav"));
    //sndstart->setSource(QUrl("qrc:/res/crt_turnon.mp3"));
    sndwin->setSource(QUrl("qrc:/res/snd_win.wav"));

    sndbutton->setVolume(0.8);
    sndnoise->setVolume(0.3);
    sndnoise->setLoopCount(QSoundEffect::Infinite);
    sndstart->setAudioOutput(audioOutput);
    sndwin->setVolume(0.8);

    sndnoise->play();
    //scenes
    CRTEffect *crt = new CRTEffect(this);
    ChooseLevelScene *lvl = new ChooseLevelScene(this);
    WinScene *win = new WinScene(this);

    //add into stack
    stackedWidget->addWidget(crt);
    stackedWidget->addWidget(lvl);
    stackedWidget->addWidget(win);

    //buttons
    CRTButton *btnstart = new CRTButton(QPoint(crt->getCenter().x() - 100, 250), QSize(200,100), ":/res/btn_start_normal");
    btnstart->setParent(crt);
    connect(btnstart, &CRTButton::pressed, this, [=](){
        if (sndstart->mediaStatus() == QMediaPlayer::NoMedia) {
            sndstart->setSource(QUrl("qrc:/res/crt_turnon.mp3")); // 首次点击时设置媒体源
        }

        sndstart->play();
        qDebug() << "start";
        //lvl = new ChooseLevelScene(this);
        stackedWidget->setCurrentWidget(lvl);
        this->setWindowTitle("选择关卡");

    });

    connect(lvl, &ChooseLevelScene::back, this, [=](){
        sndbutton->play();
        stackedWidget->setCurrentWidget(crt);
        this->setWindowTitle("翻金币");
    });

    connect(lvl, &ChooseLevelScene::level, this, [=](int level){
        sndbutton->play();
        play = new PlayScene(level, this);
        stackedWidget->addWidget(play);
        stackedWidget->setCurrentWidget(play);
        QString title = QString("第 %1 关").arg(level);

        this->setWindowTitle(title);


        connect(play, &PlayScene::back, this, [=](){
            //lvl->updateLevel(play->currentLevelIndex-1);
            sndbutton->play();
            stackedWidget->setCurrentWidget(lvl);
            this->setWindowTitle("选择关卡");
            delete play;
            play = nullptr;
        });
        connect(play, &PlayScene::win, this, [=](int level){

            //sndwin->play();

            lvl->updateLevel(level - 1);
            //qDebug() << "win";
            stackedWidget->setCurrentWidget(win);
            QString title = QString("第 %1 关  你赢了").arg(level);
            win->currentLevelIndex = level;
            this->setWindowTitle(title);
        });
        connect(play, &PlayScene::win, win, &WinScene::play);
    });

    connect(win, &WinScene::back, this, [=](){
        sndbutton->play();

        stackedWidget->setCurrentWidget(lvl);
        this->setWindowTitle("选择关卡");
        delete play;
        play = nullptr;
    });
    connect(win, &WinScene::next, this, [=](){
        sndbutton->play();
        if(win->currentLevelIndex == 20) return;
        int level = win->currentLevelIndex+1;
        play = new PlayScene(level, this);
        stackedWidget->addWidget(play);
        stackedWidget->setCurrentWidget(play);
        QString title = QString("第 %1 关").arg(level);

        this->setWindowTitle(title);


        connect(play, &PlayScene::back, this, [=](){
            sndbutton->play();
            stackedWidget->setCurrentWidget(lvl);
            this->setWindowTitle("选择关卡");
            delete play;
            play = nullptr;
        });

        connect(play, &PlayScene::win, this, [=](int level){
            //sndwin->play();
            lvl->updateLevel(level - 1);
            //qDebug() << "win";
            stackedWidget->setCurrentWidget(win);
            QString title = QString("第 %1 关  你赢了").arg(level);
            win->currentLevelIndex = level;
            this->setWindowTitle(title);
        });
        connect(play, &PlayScene::win, win, &WinScene::play);
    });

    CRTButton *btnquit = new CRTButton(QPoint(crt->getCenter().x() - 100, 450), QSize(200,100), ":/res/btn_quit_normal");
    btnquit->setParent(crt);
    connect(btnquit, &CRTButton::pressed, this->ui->actionquit, [=](){
        sndbutton->play();
        ui->actionquit->trigger();
    });



    //actionquit
    connect(ui->actionquit, &QAction::triggered, this, [=](){
        this->close();
    });



}

MainScene::~MainScene()
{
    delete ui;
}
