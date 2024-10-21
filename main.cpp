#include <QCoreApplication>
#include "ModbusSlave.h"
#include <QVector>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ModbusSlave slave;
    return a.exec();
}
