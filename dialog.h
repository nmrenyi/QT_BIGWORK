#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <vector>
#include <string>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    Ui::Dialog *ui;
//    bool eventFilter(QObject*, QEvent*);
private slots:
    void on_buttonBox_accepted();

    void on_checkBox_stateChanged(int arg1);

//private:
//    Ui::Dialog *ui;
signals:
    void rowChanged(int);
    void lineChanged(int);
    void inputxChanged(std::vector<int>);
    void inputyChanged(std::vector<int>);
    void outputxChanged(int);
    void outputyChanged(int);
    void washChanged(int);
};

#endif // DIALOG_H
