#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_dialog.h"
#include "dialog.h"
#include <QDebug>
#include <QPen>
#include <QPainter>
#include <QBrush>
#include <vector>
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <QTime>
#include <QString>
#include <QSound>
#include <QFile>
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <square.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d(new Dialog),
    Move(new QSound(":/Input.wav")),
    Split1(new QSound(":/Split1.wav")),
    Split2(new QSound(":/Split2.wav")),
    Merge(new QSound(":/Merge.wav"))

{
    ui->setupUi(this);
    connect(this, SIGNAL(Error()), qApp, SLOT(quit()));
    connect(this, SIGNAL(nowStepChanged(int)), ui->lcd, SLOT(display(int)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(testSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete d;
}

void MainWindow::paintEvent(QPaintEvent * e) {
    if (errorState)
        return;
//    qDebug()<<"in paintEvent";
    if (!initialized)
        return;

    QPainter painter(this);
    QBrush brushInput(Qt::yellow, Qt::SolidPattern);
    int row = this->row;
    int line = this->line;

    for (int i = 0; i <= line; i++) {
        int x1 = startx + i * interval;
        int y1 = starty;
        int x2 = startx + i * interval;
        int y2 = starty + row * interval;
//        qDebug() << "are you here drawing?";
        painter.drawLine(x1, y1, x2, y2);
    }
    for (int i = 0; i <= row; i++) {
        int x1 = startx;
        int y1 = starty + i * interval;
        int x2 = startx + line * interval;
        int y2 = starty + i * interval;
        painter.drawLine(x1, y1, x2, y2);
    }

    painter.setBrush(brushInput);
    for (unsigned int i = 0; i < inputx.size(); i++) {
        int x = inputx[i] - 1;
        int y = inputy[i] - 1;
        painter.drawRect(y * interval + startx, x * interval + starty, interval, interval);
    }

    QBrush brushOutput(Qt::blue, Qt::SolidPattern);
    painter.setBrush(brushOutput);
//    qDebug() << "drawing output x, y = " << outputx << " " << outputy;
    painter.drawRect((outputy - 1) * interval + startx, (outputx - 1) * interval + starty, interval, interval);

//    qDebug()<<"playinit = " << playInitialized;
    if (playInitialized && readyToPlay) {
//        qDebug()<<"in painting main";
        for (int i = 0; i <= row; i++) {
            for (int j = 0; j <= line; j++) {
//                qDebug()<<"do you hold droplet?" << status[nowStep][i][j].holdDroplet;
                if (status[nowStep][i][j].holdDroplet) {
//                    qDebug()<<"finally I find one!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! start to draw";
                    qDebug()<<"isCircle = " << status[nowStep][i][j].droplet->isCircle;
                    qDebug()<<"isEllipse = "<< status[nowStep][i][j].droplet->isEllipse;
                    QColor qc = status[nowStep][i][j].droplet->colour;
                    QBrush nowBrush(qc, Qt::SolidPattern);
                    painter.setBrush(nowBrush);
                    if (status[nowStep][i][j].droplet->isCircle) {
                        qDebug() << "i = " << i << " j = " << j;
                        painter.drawEllipse((j - 1) * interval + startx, (i - 1) * interval + starty, interval, interval);
                    } else if (status[nowStep][i][j].droplet->isEllipse) {
                        int upX = 0, upY = 0;
                        if (status[nowStep][i][j].droplet->EllipseHorizonal) {
                            upX = (j - 1) * interval + startx;
                            upY = (i - 2) * interval + starty;
                            painter.drawEllipse(upX, upY, interval, 3 * interval);
                        } else {
                            upX = (j - 2) * interval + startx;
                            upY = (i - 1) * interval + starty;
                            painter.drawEllipse(upX, upY, 3 * interval, interval);
                        }

                    } else {
                    }
                }

                if (status[nowStep][i][j].polluteTime) {
                    QColor qc = status[nowStep][i][j].polluteColour;
                    qc.setAlpha(70);
                    QBrush nowBrush(qc, Qt::SolidPattern);
                    painter.setBrush(nowBrush);
                    painter.drawRect((j - 1) * interval + startx, (i - 1) * interval + starty, interval, interval);
                }
            }
        }
    }

    if (showPolluteTime) {
        for (int i = 1; i <= row; i++) {
            for (int j = 1; j <= line; j++) {
                int x = startx + (j - 1) * interval + 0.5 * interval;
                int y = starty + (i - 1) * interval + 0.5 * interval;
                std::string sTemp = std::to_string(status[maxStep][i][j].polluteTime);
                QString s = s.fromStdString(sTemp);
                painter.drawText(x, y, s);
            }
        }
    }
//    if (needWash) {
//        int x =  startx + interval * washY;
//        int y = starty + interval * washX;
//        QBrush nowBrush(Qt::cyan, Qt::SolidPattern);
//        painter.setBrush(nowBrush);
//        painter.drawRect(x, y, interval, interval);
//    }
    if (needWash) {
        for (int i = 1; i <= row; i++) {
            for (int j = 1; j <= line; j++) {
                if (blocked[i][j]) {
                    qDebug() << "i = " << i << " j = " << j;
                    int x = startx + (j - 1) * interval;
                    int y = starty + (i - 1) * interval;
                    QColor qc = Qt::gray;
                    qc.setAlpha(60);
                    QBrush nowBrush(qc, Qt::SolidPattern);
                    painter.setBrush(nowBrush);
                    painter.drawRect(x, y, interval, interval);
                }
            }
        }
    }

    if (needWash && startWash) {
        for (int i = 1; i <= row; i++) {
            for (int j = 1; j <= line; j++) {
                if (cleanArea[nowStep][i][j]) {
                    int x = startx + (j - 1) * interval;
                    int y = starty + (i - 1) * interval;
                    QColor qc = Qt::green;
                    qc.setAlpha(70);
                    QBrush nowBrush(qc, Qt::SolidPattern);
                    painter.setBrush(nowBrush);
                    painter.drawRect(x, y, interval, interval);
                }
            }
        }
    }
}

void MainWindow::setInputx(std::vector<int>now) {
    inputx = now;
}

void MainWindow::setInputy(std::vector<int>now) {
    inputy = now;
}

void MainWindow::setOutputx(int now) {
    outputx = now;
}
void MainWindow::setOutputy(int now) {
    outputy = now;
}
void MainWindow::setRow(int now) {
    row = now;
    qDebug()<<"row set to " << row;
}

void MainWindow::setLine(int now) {
    line = now;
    qDebug() << "line set to " << line;
    initialized = true;
    this->update();
}

void MainWindow::on_actionNewSession_triggered()
{
    connect(d, SIGNAL(rowChanged(int)), this, SLOT(setRow(int)));
    connect(d, SIGNAL(lineChanged(int)), this, SLOT(setLine(int)));
    connect(d, SIGNAL(inputxChanged(std::vector<int>)), this, SLOT(setInputx(std::vector<int>)));
    connect(d, SIGNAL(inputyChanged(std::vector<int>)), this, SLOT(setInputy(std::vector<int>)));
    connect(d, SIGNAL(outputxChanged(int)), this, SLOT(setOutputx(int)));
    connect(d, SIGNAL(outputyChanged(int)), this, SLOT(setOutputy(int)));
    connect(d->ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(setWash(int)));
    d->show();
}

void MainWindow::pushBackCommand(Command c) {
    commandSequence.push_back(c);
}

void MainWindow::on_actionReadFile_triggered() {
    QString fileName = QFileDialog::getOpenFileName(NULL,"标题",".","*.txt");
//    qDebug()<<fileName;
//    QFile file("D:\\Tsinghua\\2019Summer\\QT\\Programs\\Big2\\testcase1.txt");
//    QFile file("D:\\Tsinghua\\2019Summer\\QT\\Week1\\Input\\testcaseerror.txt");
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        while(!in.atEnd()) {
            QString s = in.readLine();
            qDebug()<<s;
            std::string line = s.toStdString();
            std::stringstream ss;
            std::string commandName;
            std::string info;
            ss << line;
            ss >> commandName;
            ss >> info;
            info[info.size() - 1] = ',';
            std::istringstream is(info);
            std::string token;
            Command command ;
            command.setName(commandName);
            bool timeSet = false;
            while(std::getline(is, token, ',')) {
                QString s1 = s1.fromStdString(token);
                int here = s1.toInt();
                if (!timeSet) {
                    timeSet = true;
                    command.setTime(here);

                } else {
                    command.commandPushBack(here);
                }
            }
            pushBackCommand(command);
        }

        InitPlay();
    } else {
        qDebug()<<"file open failed";
    }
}


bool checkInput(std::vector<int>para, std::vector<int>inputx, std::vector<int>inputy) {
//    qDebug()<< "para[0] = " << para[0] << "para[1] = " << para[1];
    int x = para[0];
    int y = para[1];
    for (unsigned int i = 0; i < inputx.size(); i++) {
        int inputX = inputx[i];
        int inputY = inputy[i];
        bool inputNear = ((x + 1 == inputX && y == inputY) || (x - 1 == inputX && y == inputY)
                       || (x == inputX && y + 1 == inputY) || (x == inputX && y - 1 == inputY));
        if (inputNear)
            return true;
    }
    return false;
}

QColor getRandomColour() {
    return QColor::fromHsl(rand()%360,rand()%256,rand()%200);
}
void MainWindow::setEnable(int nowTime, int x , int y) {
    for (int i = nowTime; i <= maxStep; i++) {
        unableToWash[i][x][y] = false;
        unableToWash[i][x - 1][y] = false;
        unableToWash[i][x + 1][y] = false;
        unableToWash[i][x][y - 1] = false;
        unableToWash[i][x][y + 1] = false;
        unableToWash[i][x + 1][y + 1] = false;
        unableToWash[i][x + 1][y - 1] = false;
        unableToWash[i][x - 1][y + 1] = false;
        unableToWash[i][x - 1][y - 1] = false;
    }
}
void MainWindow::setUnable(int nowTime, int x, int y) {
    for (int i = nowTime; i <= maxStep; i++) {
        unableToWash[i][x][y] = true;
        unableToWash[i][x - 1][y] = true;
        unableToWash[i][x + 1][y] = true;
        unableToWash[i][x][y - 1] = true;
        unableToWash[i][x][y + 1] = true;
        unableToWash[i][x + 1][y + 1] = true;
        unableToWash[i][x + 1][y - 1] = true;
        unableToWash[i][x - 1][y + 1] = true;
        unableToWash[i][x - 1][y - 1] = true;
    }
}
void MainWindow::setUnableOnce(int i, int x, int y) {
//    for (int i = nowTime; i <= maxStep; i++) {
        unableToWash[i][x][y] = true;
        unableToWash[i][x - 1][y] = true;
        unableToWash[i][x + 1][y] = true;
        unableToWash[i][x][y - 1] = true;
        unableToWash[i][x][y + 1] = true;
        unableToWash[i][x + 1][y + 1] = true;
        unableToWash[i][x + 1][y - 1] = true;
        unableToWash[i][x - 1][y + 1] = true;
        unableToWash[i][x - 1][y - 1] = true;
//    }
}
void MainWindow::setEnableOnce(int i, int x , int y) {
//    for (int i = nowTime; i <= maxStep; i++) {
        unableToWash[i][x][y] = false;
        unableToWash[i][x - 1][y] = false;
        unableToWash[i][x + 1][y] = false;
        unableToWash[i][x][y - 1] = false;
        unableToWash[i][x][y + 1] = false;
        unableToWash[i][x + 1][y + 1] = false;
        unableToWash[i][x + 1][y - 1] = false;
        unableToWash[i][x - 1][y + 1] = false;
        unableToWash[i][x - 1][y - 1] = false;
//    }
}
void MainWindow::InitPlay() {
{
            playInitialized = true;
            nowStep = -1;
            for (auto x : commandSequence) {
                int nowTime = x.getTime();
                if (nowTime > maxStep)
                    maxStep = nowTime;
            }


            std::sort(commandSequence.begin(), commandSequence.end());
            for (int nowTime = 0; nowTime <= maxStep; nowTime++) {
                for (auto x : commandSequence) {
                    if (x.getTime() == nowTime) {
                        std::string name = x.getName();
                        std::vector<int> para = x.getPara();  // 输入序列的命令参数
    //                    QString tmp = tmp.fromStdString(name);
    //                    qDebug() << "command name = " << tmp;

                        if (name == "Input") {
//                            qDebug()<<"before check input";

                            if (!checkInput(para, inputx, inputy)) {
    //                            QMessageBox::about(nullptr, "About", "No input nearby <font color='red'>application</font>");
                                QMessageBox::warning(nullptr, tr("MainWindow"), tr("输入附近没有输入口"), QMessageBox::Yes);
                                qDebug()<< "no proper here input here check input";
                            }
//                            else {
//                                qDebug() << "BEFORE color";
                                QColor qc=getRandomColour();
//                                qDebug()<<"color complete";
    //                            std::vector<int> para = x->getPara();
                                Droplet* drop = new Droplet;
                                drop->colour = qc;
                                drop->isCircle = true;
                                sound[nowTime].push_back("Input");
                                for (int i = nowTime; i <= maxStep; i++) {
                                    status[i][para[0]][para[1]].holdDroplet = true;
                                    status[i][para[0]][para[1]].droplet = drop;
                                    status[i][para[0]][para[1]].polluteTime++;
                                    status[i][para[0]][para[1]].polluteColour = drop->colour;
                                }
                                setUnable(nowTime, para[0], para[1]);

//                            }

                        } else if (name == "Move") {
//                            qDebug()<<"in Move";
                            Droplet* drop = new Droplet;
                            drop->colour = status[nowTime][para[0]][para[1]].droplet->colour;
//                            qDebug()<< "in para0, 1, 2, 3 = " << para[0]<< " "<<para[1] << " " << para[2] <<" " <<para[3] <<" tell me the color is = " << drop->colour;
                            drop->isCircle = true;
                            status[nowTime][para[2]][para[3]].holdDroplet = true;
                            status[nowTime][para[2]][para[3]].droplet = drop;
                            sound[nowTime].push_back("Move");
                            for (int i = nowTime; i <= maxStep; i++) {
                                status[i][para[2]][para[3]].holdDroplet = true;
                                status[i][para[2]][para[3]].droplet = drop;
                                status[i][para[2]][para[3]].polluteTime++;
                                status[i][para[2]][para[3]].polluteColour = drop->colour;
                            }
                            for (int i = nowTime; i <= maxStep; i++) {
                                status[i][para[0]][para[1]].holdDroplet = false;
                            }
                            setEnable(nowTime, para[0], para[1]);
                            setUnable(nowTime, para[2], para[3]);
                        } else if (name == "Split") {
                            qDebug()<< "in split";
                            Droplet* drop1 = new Droplet;
                            Droplet* drop2 = new Droplet;
                            Droplet* drop3 = new Droplet;
                            QColor qc1=getRandomColour();
                            QColor qc2=getRandomColour();
                            QColor qc3=getRandomColour();

                            drop1->colour = qc1;
                            drop1->isEllipse = true;
                            if (para[0] != para[2]) {
                                drop1->EllipseHorizonal = true;
                            }
                            setUnable(nowTime, para[0], para[1]);

                            status[nowTime][para[0]][para[1]].holdDroplet = true;
                            status[nowTime][para[0]][para[1]].droplet = drop1;
                            sound[nowTime].push_back("Split1");
                            sound[nowTime + 1].push_back("Split2");
                            for (int i = nowTime + 1; i <= maxStep; i++) {
                                status[i][para[0]][para[1]].holdDroplet = false;
                            }

                            drop2->colour = qc2;
                            drop2->isCircle = true;
                            for (int i = nowTime + 1; i <= maxStep; i++)
                            {
                                status[i][para[2]][para[3]].holdDroplet = true;
                                status[i][para[2]][para[3]].droplet = drop2;
                            }

                            drop3->colour = qc3;
                            drop3->isCircle = true;
                            for (int i = nowTime + 1; i <= maxStep; i++)
                            {
                                status[i][para[4]][para[5]].holdDroplet = true;
                                status[i][para[4]][para[5]].droplet = drop3;
                                status[i][para[4]][para[5]].polluteTime++;
                                status[i][para[4]][para[5]].polluteColour = drop3->colour;
                            }
                            setEnable(nowTime + 1, para[0],para[1]);
                            setUnable(nowTime + 1, para[2], para[3]);
                            setUnable(nowTime + 1, para[4], para[5]);
                        } else if (name == "Mix") {
                            // Mix这里到最后需不需要一直摆放着？？我应该是没写吧
                            qDebug()<<"in Mix";
                            QColor qc = status[nowTime - 1][para[0]][para[1]].droplet->colour;
                            for (int i = nowTime; i <= maxStep; i++) {
                                status[i][para[0]][para[1]].holdDroplet = false;
                            }
                            int k = 0;
                            setEnable(nowTime, para[0], para[1]);
                            for (unsigned int i = 2; i < para.size(); i += 2) {
                                Droplet* drop = new Droplet;
                                drop->colour = qc;
                                drop->isCircle = true;
                                status[nowTime + k][para[i]][para[i + 1]].holdDroplet = true;
                                status[nowTime + k][para[i]][para[i + 1]].droplet = drop;
                                sound[nowTime + k].push_back("Move");
                                setEnableOnce(nowTime + k, para[i - 2], para[i - 1]);
                                setUnableOnce(nowTime + k, para[i], para[i + 1]);
                                // 这里是不是不对
                                for (int j = nowTime + k; j <= maxStep; j++) {
                                    status[j][para[i]][para[i + 1]].polluteTime++;
                                    status[j][para[i]][para[i + 1]].polluteColour = qc;
                                }
                                k++;
                            }
                            setUnable(nowTime + k, para[para.size()- 2], para[para.size() - 1]);

                            for (int i = nowTime + k; i <= maxStep; i++) {
                                Droplet* drop = new Droplet;
                                drop->colour = qc;
                                drop->isCircle = true;
                                status[i][para[para.size() - 2]][para[para.size() - 1]].holdDroplet = true;
                                status[i][para[para.size() - 2]][para[para.size() - 1]].droplet = drop;
    //                            sound[i].push_back("Move");
                            }
                        } else if (name == "Merge") {
                            qDebug()<<"in Merge";
                            QColor qc1=getRandomColour();
                            QColor qc2=getRandomColour();
                            Droplet* drop1 = new Droplet;
                            Droplet* drop2 = new Droplet;
                            drop1->colour = qc1;
                            drop1->isEllipse = true;

                            if (para[1] == para[3]) {
                                drop1->EllipseHorizonal = true;
                            }
                            int middleX = (para[0] + para[2]) / 2;
                            int middleY = (para[1] + para[3]) / 2;
                            for (int i = nowTime; i <= maxStep; i++) {
                                status[i][para[0]][para[1]].holdDroplet = false;
                                status[i][para[2]][para[3]].holdDroplet = false;
                            }
                            setEnable(nowTime, para[0], para[1]);
                            setEnable(nowTime, para[2], para[3]);
                            setUnable(nowTime, middleX, middleY);
                            status[nowTime][middleX][middleY].holdDroplet = true;
                            status[nowTime][middleX][middleY].droplet = drop1;

                            drop2->colour = qc2;
                            drop2->isCircle = true;
                            sound[nowTime + 1].push_back("Merge");

                            for (int i = nowTime + 1; i <= maxStep; i++)
                            {
                                status[i][middleX][middleY].holdDroplet = true;
                                status[i][middleX][middleY].droplet = drop2;
                                status[i][middleX][middleY].polluteTime++;
                                status[i][middleX][middleY].polluteColour = drop2->colour;
                            }
                        } else if (name == "Output") {
                            int x = para[0], y = para[1];
                            bool outputNear = ((x + 1 == outputx && y == outputy) || (x - 1 == outputx && y == outputy)
                                               || (x == outputx && y == outputy + 1) || (x == outputx && y == outputy - 1));
    //                        qDebug()<<"after construct bool = " << outputNear;

                            if (!outputNear) {
                                 QMessageBox::warning(nullptr, tr("MainWindow"), tr("附近没有输出口"), QMessageBox::Yes);
                            }
                            for (int i = nowTime; i <= maxStep; i++) {
                                status[i][para[0]][para[1]].holdDroplet = false;
                            }
                            setEnable(nowTime, para[0], para[1]);
//                            status[nowTime][x][y].polluteTime++;
                            status[nowTime][x][y].polluteColour = status[nowTime][para[0]][para[1]].droplet->colour;
                        }
                    }
                }
            }
//            qDebug() << "initialize complete";
        }
    limitCheck();

    if (needWash)
    {
        bool needMinus = false;
        calcRoute();
        for (int t = 0; t <= maxStep; t++)
            for (int i = 1; i <= row; i++) {
                for (int j = 1; j <= line; j++) {
                    if (cleanArea[t][i][j]) {
                        for (int k = t; k < maxStep; k++) {
                            if (status[k][i][j].polluteTime != status[k + 1][i][j].polluteTime) {
//                                needMinus = true;
                                break;
                            }
//                            if (needMinus)
//                                status[k + 1][i][j].polluteTime--;
//                            else
                                status[k][i][j].polluteTime = 0;
                        }

                    }
                }
            }
    }
}
int ABS(int i) {
    if (i < 0)
        return -i;
    return i;
}
bool MainWindow::limitCheck() {
    for (int t = 0; t <= maxStep; t++) {
        for (int i = 0; i <= row; i++) {
            for (int j = 0;j <= line; j++) {
                if (status[t][i][j].holdDroplet) {
                    for (int k = i; k <= row; k++) {
                        for (int l = j + 1; j <= line; j++) {
                            if (status[t][k][l].holdDroplet) {
                                if (ABS(k - i) > 1 || ABS(l - j) > 1) {
                                } else {
                                    errorLimit[t] = true;
//                                     QMessageBox::warning(nullptr, tr("MainWindow"), tr("static"), QMessageBox::Yes);
                                    qDebug() << "static error limit here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (int t = 0; t < maxStep; t++) {
        for (int i = 0; i <= row; i++) {
            for (int j = 0; j <= line; j++) {
                if (status[t][i][j].holdDroplet) {
                    for (int k = 0; k <= row; k++) {
                        for (int l = 0; l <= line; l++) {
                            if (status[t + 1][k][l].holdDroplet) {
                                if (k != i || j != l) {

                                    if (ABS(k - i) > 1 || ABS(l - j) > 1 || status[t][i][j].droplet->colour == status[t + 1][k][l].droplet->colour || status[t][i][j].droplet->isEllipse || status[t][i][j].droplet->isEllipse) {
                                    } else {
                                        bool neverMind = false;
                                        for (auto x : sound[t + 2])
                                            if (x == "Merge")
                                            {neverMind = true;
                                            break;}
//                                        if (!neverMind) {
//                                            for (auto x : sound[t + ])
//                                        }
                                        if (neverMind)
                                            continue;
                                        errorLimit[t] = true;
//                                        QMessageBox::warning(nullptr, tr("MainWindow"), tr("dynamic"), QMessageBox::Yes);
                                        qDebug() << "dynamic error limit here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
                                        qDebug() << t << " " << i << " " << j << " " << k << " " << l;
                                        qDebug() << status[t][i][j].droplet->isEllipse << " " <<  status[t][i][j].droplet->isEllipse;
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}



void MainWindow::on_actionNextStep_triggered() {
    // 时刻与时间的定义！！！！！哪里开始

//    if (!playInitialized)
//    {
//        InitPlay();
//    }

    if (nowStep == maxStep) {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("已经到最后一步啦"), QMessageBox::Yes);
        showPolluteTime = true;
        this->update();
        return;
    }
    startWash = true;
    readyToPlay = true;
    nowStep ++;
    if (errorLimit[nowStep]) {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("这里不满足约束条件"), QMessageBox::Yes);

    }
    if (needWash)
    {
        if (unableToWash[nowStep][wasteX][wasteY] || blocked[wasteX][wasteY]) {
//            QMessageBox::warning(nullptr, tr("MainWindow"), tr("清洗液滴无法到waste口"), QMessageBox::Yes);
            qDebug() << "清洗液滴到不了waste口";
        }

//        calcRoute(nowStep);
    }
    emit(nowStepChanged(nowStep));
    this->update();
    this->play();
}

void MainWindow::calcRoute() {
    for (int i = 0; i <= maxStep; i++ ) {
        dfs(i, row, 1);
    }
}
void MainWindow::dfs(int nowTime, int x, int y) {
//    qDebug() << "nowTime = " << nowTime << " x = " << x << " y = " << y;
    cleanArea[nowTime][x][y] = true;
    if (x > 1) {
        if (!(blocked[x - 1][y] || unableToWash[nowTime][x - 1][y])) {
            if (!cleanArea[nowTime][x - 1][y])
                dfs(nowTime, x - 1, y);
        }
    }
    if (y > 1) {
        if (!(blocked[x][y - 1] || unableToWash[nowTime][x][y - 1])) {
            if (!cleanArea[nowTime][x][y - 1])
                dfs(nowTime, x, y - 1);
        }
    }
    if (y < line) {
        if (!(blocked[x][y + 1] || unableToWash[nowTime][x][y + 1])) {
            if (!cleanArea[nowTime][x][y + 1])
                dfs(nowTime, x, y + 1);
        }
    }
    if (x < row) {
        if (!(blocked[x + 1][y] || unableToWash[nowTime][x + 1][y])) {
            if (!cleanArea[nowTime][x + 1][y])
                dfs(nowTime, x + 1, y);
        }
    }
}

void MainWindow::play() {
    for (auto x : sound[nowStep]) {
        if (x == "Move") {
            qDebug() << "in move";
            Move->play();
        } else if (x == "Split1") {
            qDebug() << "in split1";
            Split1->play();
        } else if (x == "Split2") {
            qDebug()<<"in split1";
            Split2->play();
        } else if (x == "Merge") {
            qDebug()<<"in merge";
            Merge->play();
        } else {
            qDebug() << "invalid music type";
            return;
        }
    }
}

void MainWindow::on_actionPreviosStep_triggered()
{
    if (nowStep > 0)
    {
        nowStep--;
        emit(nowStepChanged(nowStep));
        this->update();
        this->play();
    }
    else {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("不能往前啦"), QMessageBox::Yes);
    }
}

void MainWindow::testSlot() {
    nowStep ++;
    startWash = true;
    readyToPlay = true;
    if (errorLimit[nowStep]) {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("这里不满足约束条件"), QMessageBox::Yes);
        timer->stop();
    }
    this->update();
    this->play();
    emit(nowStepChanged(nowStep));
    if (nowStep == maxStep)
    {
        timer->stop();
        showPolluteTime = true;
        this->update();
    }
}

void MainWindow::on_actionPlayAll_triggered() {
    readyToPlay = true;
//    if (!playInitialized)
//        InitPlay();
    timer->start(500);
}

void MainWindow::on_actionReset_triggered() {
    nowStep = 0;
    showPolluteTime = 0;
    readyToPlay = 0;
    startWash = 0;
    emit(nowStepChanged(nowStep));
    this->update();
    this->play();
}

void MainWindow::setWash(int s) {
    qDebug() << "needWash = " << s;
    needWash = s;
}

void MainWindow::mousePressEvent(QMouseEvent * event) {
    if (!needWash)
        return;
    if (event->button() == Qt::RightButton) {
        int y = (event->y() - starty + interval) / interval;
        int x = (event->x() - startx + interval) / interval;
//        if (x >= 0 && y >= 0 && x <= line && y <= row) {
            blocked[y][x] = true;
            for (int i = 0; i <= 100; i++) {
                unableToWash[i][y][x] = true;
            }
            this->update();

//        }
    }
}
