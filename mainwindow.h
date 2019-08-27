#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QTimer>
#include <string>
#include "dialog.h"
#include "command.h"
#include "square.h"
#include <QSound>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const int startx = 250;
    const int starty = 250;
    const int interval = 100;
    const int wasteX = 2;
    const int wasteY = 1;
    int washX;
    int washY;
    explicit MainWindow(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent*);
    int needWash = 0;
    Square status[101][15][15];
    bool unableToWash[101][15][15] = {{{0}}};
    bool cleanArea[101][15][15] = {{{0}}};
    std::vector<std::string> sound[101];
    bool errorLimit[101] = {0};
    int nowStep = -1;
    int maxStep = -1;
    bool errorState = false;
    bool showPolluteTime = false;
    QSound* Move;
    QSound* Split1;
    QSound* Split2;
    QSound* Merge;
    QTimer* timer;
    bool limitCheck();
    void InitPlay();
    ~MainWindow();
    void play();
protected:
    void paintEvent(QPaintEvent*);
private slots:
    void setRow(int);
    void setLine(int);
    void setInputx(std::vector<int>);
    void setInputy(std::vector<int>);
    void setOutputx(int);
    void setOutputy(int);
    void on_actionNewSession_triggered();
    void on_actionReadFile_triggered();
    void pushBackCommand(Command);
    void calcRoute();
//    void ErrorReceived();

    void on_actionNextStep_triggered();
    void testSlot();

    void on_actionPreviosStep_triggered();

    void on_actionPlayAll_triggered();

    void on_actionReset_triggered();
    void setWash(int);

private:
    void dfs(int, int, int);
    void setUnable(int, int, int);
    void setEnable(int, int, int);
    void setUnableOnce(int i, int x, int y);
    void setEnableOnce(int i, int x, int y);
    Ui::MainWindow *ui;
    Dialog* d;
    bool initialized = false;
    bool playInitialized = false;
    bool readyToPlay = false;
    int totalStep;
    int row;  // 行数
    int line;  // 列数
    int outputx;
    int outputy;
    bool startWash = false;
    std::vector<int> inputx;
    std::vector<int> inputy;
    std::vector<Command>commandSequence;
    bool blocked[15][15] = {{0}};
signals:
    void Error();
    void nowStepChanged(int);
};

#endif // MAINWINDOW_H
