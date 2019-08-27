#include "dialog.h"
#include "ui_dialog.h"
#include <string>
#include <sstream>
#include <vector>
#include <QDebug>
#include <iostream>
#include <QMessageBox>
#include <cstdlib>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
//    ui->plainTextEdit->installEventFilter(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    int row = ui->plainTextEdit->toPlainText().toInt();
    int line = ui->plainTextEdit_2->toPlainText().toInt();
    int outputx = ui->plainTextEdit_15->toPlainText().toInt();
    int outputy = ui->plainTextEdit_16->toPlainText().toInt();
    std::string xString = ui->plainTextEdit_3->toPlainText().toStdString();
    std::string yString = ui->plainTextEdit_4->toPlainText().toStdString();
    std::stringstream ssX;
    ssX << xString;
    std::string tmp;
    std::vector<int> inputx;
    while (ssX >> tmp) {
        int x = std::stoi(tmp);
        inputx.push_back(x);
    }
    std::stringstream ssY;
    ssY << yString;
    std::vector<int> inputy;
    while (ssY >> tmp) {
        int y = std::stoi(tmp);
        inputy.push_back(y);
    }
    emit(inputxChanged(inputx));
    emit(inputyChanged(inputy));
    emit(outputxChanged(outputx));
    emit(outputyChanged(outputy));
    emit(rowChanged(row));
    emit(lineChanged(line));

    if (row <= 3 && line <= 3) {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("输入的行列均小于等于3"), QMessageBox::Yes);
    }
    qDebug() << "outputx = " << outputx << " row = " << row << " outputy =  "<< outputy << " line = " << line;
    if ((outputx <= row && outputx >= 0 && outputy == 0)
            || (outputx <= row && outputx >= 0 && outputy == line + 1)
            || (outputx == 0 && outputy <= line && outputy >= 0)
            || (outputx == row + 1 && outputy <= line && outputy >= 0)) {
        //        qDebug() << "valid output position";
    } else {
        QMessageBox::warning(nullptr, tr("MainWindow"), tr("输出口位置不在边界上"), QMessageBox::Yes);
    }
    bool valid = true;
    for (int i = 0; i < inputx.size(); i++) {
        qDebug() << "input x = " << inputx[i] << " inputy = " << inputy[i];
        if ((inputy[i] <= line && inputy[i] >= 0 && inputx[i] == 0)
                || (inputy[i] <= line && inputy[i] >= 0 && inputx[i] == row + 1)
                || (inputy[i] == 0 && inputx[i] <= row && inputx[i] >= 0)
                || (inputy[i] == line + 1 && inputx[i] <= row && inputx[i] >= 0)) {
            //                qDebug() << "valid output position";
        } else {
            qDebug() << "invalid here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1";
            valid = false;
            break;
        }
    }

    if (!valid) {
        int ret = QMessageBox::warning(nullptr, tr("MainWindow"), tr("输入口位置不在边界上"), QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
//            emit(Error());
            return;
        }
    }


//    this->parent->update();
}

void Dialog::on_checkBox_stateChanged(int arg1)
{
    emit(washChanged(arg1));
}

//void Dialog::eventFilter(QObject* obj, QEvent* e) {

//}
