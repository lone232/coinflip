#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include "playscene.h"
//#include "chooselevelscene.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainScene;
}
QT_END_NAMESPACE

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    MainScene(QWidget *parent = nullptr);
    ~MainScene();

private:
    Ui::MainScene *ui;
    PlayScene *play = nullptr;
    //ChooseLevelScene *lvl = nullptr;
};
#endif // MAINSCENE_H
