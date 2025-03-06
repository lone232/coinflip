#include "chooselevelscene.h"
#include "crtbutton.h"

#include <QTimer>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>

ChooseLevelScene::ChooseLevelScene(QWidget *parent) {
    //CRTEffect::CRTEffect(parent);

    // CRTButton *btnquit = new CRTButton(QPoint(this->getCenter().x() - 100, 450), QSize(200,100), ":/res/btn_quit_normal");
    // btnquit->setParent(this);
    // connect(btnquit, &CRTButton::pressed, this, [=](){
    //     this->parentWidget()->close();
    // });
    createDefaultLevelData("LevelData.json");

    CRTButton *btnback = new CRTButton(QPoint(600 - 200, this->getCenter().y() * 2 - 100), QSize(200,100), ":/res/btn_back_normal", ":/res/btn_back_pressed");
    btnback->setParent(this);
    connect(btnback, &CRTButton::released, this, [=](){
        //this->parentWidget()->hide();
        qDebug() << "back";
        QTimer::singleShot(200, this, [=](){
            emit this->back();
        });

    });


    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/LevelData.json";
    records = loadGameRecords(filePath);

    for(int i = 0; i < 20; i++){
        CRTButton* btn;
        btn = LevelFromStruct(i, records[i].isWin, records[i].minStep);
        this->menuArray[i] = btn;
    }

}


void ChooseLevelScene::extracted(QList<GameRecord> &gameRecords,
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
QList<ChooseLevelScene::GameRecord>
ChooseLevelScene::loadGameRecords(const QString &filePath) {

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
CRTButton* ChooseLevelScene::LevelFromStruct(int i, bool isWin, int minStep){
    QString icon(":/res/");
    icon.append(isWin? "btn_menu_win" : "btn_menu_normal" );
    CRTButton *btnmenu = new CRTButton(QPoint(60 + (i % 5) * 100, 200 + (i / 5) * 100), QSize(80,80), icon);
    btnmenu->setParent(this);
    QLabel *label = new QLabel(btnmenu);
    QFont font;
    font.setFamily("Ac437 IBM EGA 8x8-2x");
    font.setPixelSize(15);



    label->setFixedSize(btnmenu->size());
    label->setFont(font);
    label->setStyleSheet("QLabel{color:white}");
    label->setText(QString::number(i+1));
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    connect(btnmenu, &CRTButton::pressed, this, [=](){
        qDebug() << "level" << i + 1;
        //btnmenu->update();
        emit level(i+1);
    });

    return btnmenu;
}
void ChooseLevelScene::createDefaultLevelData(const QString& filePath) {
    // 如果文件存在，直接返回
    if (QFile::exists(filePath)) {
        return;
    }

    // 创建默认的 GameRecord 数据
    QJsonArray levelsArray;
    for (int i = 0; i < 20; i++) {
        QJsonObject levelObj;
        levelObj["isWin"] = false;
        levelObj["minStep"] = 0;
        levelsArray.append(levelObj);
    }

    // 创建 QJsonObject 并嵌套 levels 数组
    QJsonObject jsonObject;
    jsonObject["levels"] = levelsArray;

    // 创建 QJsonDocument
    QJsonDocument jsonDoc(jsonObject);

    // 创建 Qfile 对象
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(jsonDoc.toJson(QJsonDocument::Indented)); // 使用缩进格式
        file.close();
        qDebug() << "默认的 LevelData.json 文件已创建";
    } else {
        qWarning() << "无法创建默认文件:" << filePath;
    }
}
void ChooseLevelScene::updateLevel(int i){
    // this->menuArray[i]->close();
    QString exeDir = QCoreApplication::applicationDirPath();
    QString filePath = exeDir + "/LevelData.json";
    records = loadGameRecords(filePath);
    CRTButton* btn;
    btn = LevelFromStruct(i, records[i].isWin, records[i].minStep);
    this->menuArray[i] = btn;
    this->menuArray[i]->update();
    qDebug() << i+1 << "update";
}
