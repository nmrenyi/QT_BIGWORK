#ifndef SQUARE_H
#define SQUARE_H
#include <QColor>
class Droplet {
 public:
    QColor colour;
    bool isCircle = false;
    bool isEllipse = false;
    bool EllipseHorizonal = false;
};

class Square {
 public:
    int polluteTime = 0;
    QColor polluteColour;
    bool holdDroplet = false;
    Droplet* droplet;
    Square();
};

#endif // SQUARE_H
