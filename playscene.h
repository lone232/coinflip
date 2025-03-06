#ifndef PLAYSCENE_H
#define PLAYSCENE_H

#include "crteffect.h"
#include "crtcoin.h"

#include <QSoundEffect>
class PlayScene : public CRTEffect
{
    Q_OBJECT
public:
    struct GameRecord {
        bool isWin;
        int minStep;

        // 打印函数（可选）
        void print() const {
            qDebug() << "isWin:" << isWin << ", minStep:" << minStep;
        }
    };
    int currentLevelIndex;
    GameRecord record;
    int currentStep;
    PlayScene(int levelIndex, QWidget *parent = nullptr);
    void flipCoin(CRTCoin * coin);
    CRTCoin* coinArray[4][4];
    int gameArray[4][4];
    bool isFliping();
    void saveGameRecords(const QString& filePath, const QList<GameRecord>& records);
    void updateGameRecord(const QString& filePath, const GameRecord& newRecord, int index);
    void loadRecord(const QString& filePath);
    void extracted(QList<GameRecord> &gameRecords, QJsonArray &jsonArray);
    void adjustFontToFit(QPainter& painter, const QRect& rect, QString text);

    QList<GameRecord> loadGameRecords(const QString &filePath);

private:


    void drawContent(QImage &buffer) override;

    void forbidFlip();
    void unforbidFlip();
    void isWin();

    QSoundEffect *sndwin;



signals:
    void back();
    void win(int level);
};

#endif // PLAYSCENE_H
