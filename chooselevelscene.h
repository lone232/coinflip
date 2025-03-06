#ifndef CHOOSELEVELSCENE_H
#define CHOOSELEVELSCENE_H

#include "crteffect.h"
#include "crtbutton.h"
class ChooseLevelScene : public CRTEffect
{
    Q_OBJECT
private:
    CRTButton * menuArray[20];
public:
    struct GameRecord {
        bool isWin;
        int minStep;

        // 打印函数（可选）
        void print() const {
            qDebug() << "isWin:" << isWin << ", minStep:" << minStep;
        }
    };
    QList<GameRecord> records;
    ChooseLevelScene(QWidget *parent = nullptr);
    //void loadJsonAndCreateButtons(const QString& filePath);
    void extracted(QList<GameRecord> &gameRecords, QJsonArray &jsonArray);
    QList<GameRecord> loadGameRecords(const QString &filePath);
    CRTButton* LevelFromStruct(int i, bool isWin, int minStep);
    void createDefaultLevelData(const QString& filePath);
    void updateLevel(int i);

signals:
    void back();
    void level(int);
};

#endif // CHOOSELEVELSCENE_H
