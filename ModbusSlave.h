#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H
#include <QObject>
#include <QModbusTcpServer>
#include <QModbusDataUnit>
#include <QDebug>
#include <vector>

class ModbusSlave : public QObject
{
    Q_OBJECT
private:
    QModbusTcpServer *modbusServer;
    bool isUpdatingData = false;
public:
    explicit ModbusSlave(QObject *parent = nullptr);

signals:

public slots:
    void onDataWritten(QModbusDataUnit::RegisterType registerType, quint16 firstAddress, quint16 valueCount);
};

#endif // MODBUSSLAVE_H
