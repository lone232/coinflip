#include "playscene.h"
#include "crtbutton.h"
#include "crtcoin.h"
#include "dataconfig.h"
#include <QPainter>
#include <QLibraryInfo>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDir>
#include <QSoundEffect>


PlayScene::PlayScene(int levelIndex, QWidget *parent)
{
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/LevelData.json";
    currentLevelIndex = levelIndex;
    currentStep = 0;
    loadRecord(filePath);
    dataConfig gameData;

    QSoundEffect *sndflip = new QSoundEffect(this);
    sndflip->setSource(QUrl("qrc:/res/snd_coinflip.wav"));
    sndflip->setVolume(0.8);

    sndwin = new QSoundEffect(this);
    sndwin->setSource(QUrl("qrc:/res/snd_win.wav"));
    sndwin->setVolume(0.8);


    CRTButton *btnback = new CRTButton(QPoint(600 - 200, this->getCenter().y() * 2 - 100), QSize(200,100), ":/res/btn_back_normal", ":/res/btn_back_pressed");
    btnback->setParent(this);
    connect(btnback, &CRTButton::released, this, [=](){
        //this->parentWidget()->hide();
        //qDebug() << "back";
        QTimer::singleShot(200, this, [=](){
            emit this->back();
        });

    });

    // for(int i = 0; i < 4; i++){
    //     for(int j = 0; j < 4; j++){
    //         this->gameArray[i][j] = gameData.mData[currentLevelIndex][i][j];
    //     }
    // }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gameArray[i][j] = gameData.mData[currentLevelIndex][i][j];
            int coinIndex = this->gameArray[i][j]? 0: 6; // gold : 1, silver : 0;
            CRTCoin *coin = new CRTCoin(QPoint(133 + i * 90, 188 + j * 90), QSize(70,70), coinIndex, 7, ":/res/anm_coin_flip.png");
            coin->i = i; coin->j = j;
            coin->updateCoinStatus();
            coin->setParent(this);
            coinArray[i][j] = coin;
            connect(coin, &CRTCoin::pressed, this, [=]{
                if(!coin->doNotFlip){
                    sndflip->play();
                    flipCoin(coin);
                    qDebug() << coin->i << coin->j << (coin->coinStatus == CRTCoin::CoinStatus::Silver? "silver coin": coin->coinStatus == CRTCoin::CoinStatus::Gold? "gold coin" : "fliping");


                    forbidFlip();
                }

            });
            connect(coin, &CRTCoin::flipDone, this, [=]{
                if(isFliping()){
                    return;
                }
                currentStep++;
                isWin();

                unforbidFlip();


                QString array;
                for(int i = 0; i < 4; i++){
                    for(int j = 0; j < 4; j++){
                        array.append(QString("[%1]").arg(this->gameArray[j][i]));
                    }
                    array.append("\n");
                }
                qDebug() << array;



            });


        }

    }
    // for(int i = 0; i < 4; i++){
    //     for(int j = 0; j < 4; j++){

    //     }
    // }

}
void PlayScene::drawContent(QImage &buffer)
{
    // 在此处绘制你的实际内容，示例中绘制一个渐变
    QPainter p(&buffer);
    QImage title(":/res/title.png");

    p.fillRect(buffer.rect(), QColor(78,106,91));
    //p.fillRect(buffer.rect(), QColor(90,108,108));

    for(int i = 0; i < 16 ; i++){
        p.fillRect(125+(i%4)*90,180+(i/4)*90,80,80,QColor(39,50,70,100));
    }

    QFont font("Microsoft YaHei", 15);
    font.setHintingPreference(QFont::PreferVerticalHinting); // 启用竖线精确对齐
    // 关闭字体抗锯齿
    //font.setStyleStrategy(QFont::NoAntialias);
    p.setFont(font);

    p.drawImage(QPoint(0,-70),title);
    p.setPen(QColor(0, 255, 0));


    // 获取 Qt 安装根目录
    //QString qtPath = QCoreApplication::applicationDirPath();
    QString homePath = QDir::homePath();
    QString qtPath = QDir::toNativeSeparators(homePath);

    //qDebug() << "Qt路径：" << qtPath;

    QString level = QString("%1> LEVEL: %2").arg(qtPath).arg(currentLevelIndex);
    QString step = QString("%1> STEP: %2").arg(qtPath).arg(this->currentStep);
    QString min = QString("MINSTEP: %2").arg(this->record.minStep);
    p.drawText(QPoint(10,600),level);
    p.drawText(QPoint(10,630),step);
    if(this->record.minStep != 0) p.drawText(QPoint(10,660),min);
    //QRect textRect = QRect(10,600,400,22);
    // adjustFontToFit(p, QRect(10,600,400,30), level);
    // adjustFontToFit(p, QRect(10,630,400,30), step);
    // if(this->record.minStep != 0) adjustFontToFit(p, QRect(10,660,400,30), min);


}
void PlayScene::forbidFlip(){
    for(int i = 0; i < 16; i++){
        coinArray[i/4][i%4]->doNotFlip = true;
    }
    qDebug() << "forbid";
}
void PlayScene::unforbidFlip(){
    for(int i = 0; i < 16; i++){
        coinArray[i/4][i%4]->doNotFlip = false;
    }
    qDebug() << "forbid";
}
void PlayScene::isWin(){
    for(int i = 0; i < 16; i++){
        if( !this->gameArray[i/4][i%4] ) return;
    }
    this->sndwin->play();
    QTimer::singleShot(1000, this, [=](){
        this->record.isWin = true;
        if(this->record.minStep > 0)this->record.minStep = std::min(this->record.minStep, this->currentStep);
        else this->record.minStep = this->currentStep;
        QString exeDir = QCoreApplication::applicationDirPath();
        QString filePath = exeDir + "/LevelData.json";
        updateGameRecord(filePath,this->record,this->currentLevelIndex-1);
        emit win(this->currentLevelIndex);
        qDebug() << "win";
    });

}
void PlayScene::flipCoin(CRTCoin * coin){

    if(coin->i > 0) {this->gameArray[coin->i-1][coin->j] = !this->gameArray[coin->i-1][coin->j];}
    if(coin->i < 3) {this->gameArray[coin->i+1][coin->j] = !this->gameArray[coin->i+1][coin->j];}
    if(coin->j > 0) {this->gameArray[coin->i][coin->j-1] = !this->gameArray[coin->i][coin->j-1];}
    if(coin->j < 3) {this->gameArray[coin->i][coin->j+1] = !this->gameArray[coin->i][coin->j+1];}
    this->gameArray[coin->i][coin->j] = !this->gameArray[coin->i][coin->j];
    coin->shouldFlip = true;
    QTimer::singleShot(100, this, [=]{
        if(coin->i > 0) {coinArray[coin->i-1][coin->j]->shouldFlip = true; /*this->gameArray[i-1][j] = !this->gameArray[i-1][j];*/}
        if(coin->i < 3) {coinArray[coin->i+1][coin->j]->shouldFlip = true; /*this->gameArray[i+1][j] = !this->gameArray[i+1][j];*/}
        if(coin->j > 0) {coinArray[coin->i][coin->j-1]->shouldFlip = true; /*this->gameArray[i][j-1] = !this->gameArray[i][j-1];*/}
        if(coin->j < 3) {coinArray[coin->i][coin->j+1]->shouldFlip = true; /*this->gameArray[i][j+1] = !this->gameArray[i][j+1];*/}
    });
}
bool PlayScene::isFliping(){
    for(int i = 0; i < 16; i++){
        if(coinArray[i%4][i/4]->coinStatus == CRTCoin::CoinStatus::Fliping) return true;
    }
    return false;
}


// 更新特定游戏记录
void PlayScene::updateGameRecord(const QString& filePath, const GameRecord& newRecord, int index) {
    // 加载现有的 GameRecord 列表
    QList<GameRecord> records = loadGameRecords(filePath);

    // 检查索引是否有效
    if (index >= 0 && index < records.size()) {
        // 更新指定索引的记录
        GameRecord& record = records[index];
        record.isWin = newRecord.isWin;
        record.minStep = newRecord.minStep;

        // 保存更新后的列表
        saveGameRecords(filePath, records);
    } else {
        qWarning() << "索引无效:" << index;
    }
}



void PlayScene::saveGameRecords(const QString& filePath, const QList<GameRecord>& records) {
    QJsonArray jsonArray;
    for (const GameRecord& record : records) {
        QJsonObject obj;
        obj["isWin"] = record.isWin;
        obj["minStep"] = record.minStep;
        jsonArray.append(obj);
    }

    // 创建顶层对象并添加 levels 数组
    QJsonObject jsonObject;
    jsonObject["levels"] = jsonArray;

    QJsonDocument jsonDoc(jsonObject); // 使用 QJsonObject 而非 QJsonArray
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filePath;
        return;
    }
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}
void PlayScene::extracted(QList<GameRecord> &gameRecords,
                          QJsonArray &jsonArray) {
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();

            // 提取 isWin 和 minStep
            if (obj.contains("isWin") && obj.contains("minStep")) {
                GameRecord record;
                record.isWin = obj["isWin"].toBool();
                record.minStep = obj["minStep"].toInt();
                gameRecords.append(record);
            }
        }
    }
}
QList<PlayScene::GameRecord>
PlayScene::loadGameRecords(const QString &filePath) {

    QList<GameRecord> gameRecords;

    // 打开 JSON 文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filePath;
        return gameRecords;
    }

    // 读取文件内容并解析为 JSON 文档
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qWarning() << "解析 JSON 文件失败";
        return gameRecords;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains("levels") || !jsonObject["levels"].isArray()) {
        qWarning() << "JSON 文件中没有找到 levels 数组";
        return gameRecords;
    }

    QJsonArray jsonArray = jsonObject["levels"].toArray();
    extracted(gameRecords, jsonArray);

    return gameRecords;
}

void PlayScene::loadRecord(const QString& filePath) {
    // 加载现有的 GameRecord 列表
    QList<GameRecord> records = loadGameRecords(filePath);
    int index = this->currentLevelIndex-1;

    // 检查索引是否有效
    if (index >= 0 && index < records.size()) {
        // 更新指定索引的记录
        this->record = records[index];


    } else {
        qWarning() << "索引无效:" << index;
    }
}

void PlayScene::adjustFontToFit(QPainter& painter, const QRect& rect, QString text) {
    QFont font = painter.font();
    QFontMetrics metrics(font);
    QRect textBound = metrics.boundingRect(rect, Qt::TextSingleLine, text);

    // 缩小字体直到适应矩形
    while (textBound.height() > rect.height() || textBound.width() > rect.width()) {
        if (font.pointSize() <= 1) break;  // 防止字体过小
        font.setPointSize(font.pointSize() - 1);
        painter.setFont(font);
        metrics = QFontMetrics(font);
        textBound = metrics.boundingRect(rect, Qt::TextSingleLine, text);
    }

    // 绘制最终文本
    painter.drawText(rect, Qt::TextSingleLine, text);
}
