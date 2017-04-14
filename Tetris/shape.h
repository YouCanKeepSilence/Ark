#ifndef SHAPE_H
#define SHAPE_H

#include <QtWidgets>

class Shape
{
protected:
    int height;
    int width;
    QVector<QPoint> parts;
    bool rotation;
public:
    virtual ~Shape()=0;
    int getHeight() const
    {
        return height;
    }
    int getWidth() const
    {
        return width;
    }
    QVector<QPoint> getParts() const
    {
        return parts;
    }
    bool canRotate() const
    {
        return rotation;
    }
};

class Square : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 1 1 0 0
        0 1 1 0 0
        0 0 0 0 0
    */
public:
    Square()
    {
        height=2;
        width=2;
        rotation=false;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(-1,0));
        parts.push_back(QPoint(0,0));
    }
};

class LittleSquare : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 0 1 0 0
        0 0 0 0 0
        0 0 0 0 0
    */
public:
    LittleSquare()
    {
        height=1;
        width=1;
        rotation=false;
        parts.push_back(QPoint(0,1));
    }
};

/*class BigSquare : public Shape
{
//
//        0 0 0 0 0
//        0 1 1 1 0
//        0 1 1 1 0
//        0 1 1 1 0
//        0 0 0 0 0
//
public:
    BigSquare()
    {
       height=3;
       width=3;
       rotation=false;
       parts.push_back(QPoint(0,1));
       parts.push_back(QPoint(0,2));
       parts.push_back(QPoint(-1,2));
       parts.push_back(QPoint(-1,1));
       parts.push_back(QPoint(-1,0));
       parts.push_back(QPoint(0,0));
       parts.push_back(QPoint(1,0));
       parts.push_back(QPoint(1,1));
       parts.push_back(QPoint(1,2));
    }
};*/

class Stick : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        1 1 1 1 0
        0 0 0 0 0
        0 0 0 0 0
    */
public:
    Stick()
    {
        height=1;
        width=4;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(1,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(-2,1));
    }
};

class TShape : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 1 1 1 0
        0 0 1 0 0
        0 0 0 0 0
    */
public:
    TShape()
    {
        height=2;
        width=3;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(0,0));
        parts.push_back(QPoint(1,0));
    }
};

class ZShape : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 1 1 0 0
        0 0 1 1 0
        0 0 0 0 0
    */
public:
    ZShape()
    {
        height=2;
        width=3;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(0,0));
        parts.push_back(QPoint(1,0));
    }
};

class SShape : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 0 1 1 0
        0 1 1 0 0
        0 0 0 0 0
    */
public:
    SShape()
    {
        height=2;
        width=3;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(1,1));
        parts.push_back(QPoint(0,0));
        parts.push_back(QPoint(-1,0));
    }
};

class JShape : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 1 1 1 0
        0 0 0 1 0
        0 0 0 0 0
    */
public:
    JShape()
    {
        height=2;
        width=3;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(1,1));
        parts.push_back(QPoint(1,0));
    }
};

class LShape : public Shape
{
    /*
        0 0 0 0 0
        0 0 0 0 0
        0 1 1 1 0
        0 1 0 0 0
        0 0 0 0 0
    */
    LShape()
    {
        height=2;
        width=3;
        rotation=true;
        parts.push_back(QPoint(0,1));
        parts.push_back(QPoint(-1,1));
        parts.push_back(QPoint(1,1));
        parts.push_back(QPoint(-1,0));
    }
};

#endif // SHAPE_H
