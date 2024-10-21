#include "ModbusSlave.h"

ModbusSlave::ModbusSlave(QObject *parent) : QObject(parent)
{
    modbusServer = new QModbusTcpServer(this);


                modbusServer->setConnectionParameter(QModbusDevice::NetworkPortParameter, 502);
                modbusServer->setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
                modbusServer->setServerAddress(1);
                QModbusDataUnitMap reg;
                       reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 10 });
                       reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
                       reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 16 });
                       reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 16 });

                       modbusServer->setMap(reg);
                if (!modbusServer->connectDevice()) {
                    qFatal("Unable to start Modbus server: %s", qPrintable(modbusServer->errorString()));
                }
                connect(modbusServer, &QModbusTcpServer::dataWritten, this, &ModbusSlave::onDataWritten);

}
void ModbusSlave::onDataWritten(QModbusDataUnit::RegisterType registerType, quint16 firstAddress, quint16 valueCount) {
    if (isUpdatingData) {
            return;
        }
    QVector<quint16> values;
    quint64 value64[4];
    QVector<double> valuesBack;
    QVector<quint16> inputValues;
    for (int i = 0; i < valueCount; ++i) {
            quint16 value;

            switch (registerType) {
            case QModbusDataUnit::HoldingRegisters:
                modbusServer->data(QModbusDataUnit::HoldingRegisters, quint16(firstAddress + i), &value);

            values.push_back(value);
                break;
            default:
                break;
            }
        }

        for (int i = 0; i < 4; ++i) {
            value64[i] = (static_cast<quint64>(values[i * 4]) |
                          (static_cast<quint64>(values[i * 4 + 1]) << 16) |
                          (static_cast<quint64>(values[i * 4 + 2]) << 32) |
                          (static_cast<quint64>(values[i * 4 + 3]) << 48));
        }

        for (int i = 0; i < 4; ++i) {
            double tempValue;
            memcpy(&tempValue, &value64[i], sizeof(double));
            tempValue *=2;
            valuesBack.push_back(tempValue);
        }
        for (int i = 0; i < 4; ++i) {
                quint64 value64_i;
                memcpy(&value64_i, &valuesBack[i], sizeof(double));
                inputValues.push_back(static_cast<quint16>(value64_i & 0xFFFF));
                inputValues.push_back(static_cast<quint16>((value64_i >> 16) & 0xFFFF));
                inputValues.push_back(static_cast<quint16>((value64_i >> 32) & 0xFFFF));
                inputValues.push_back(static_cast<quint16>((value64_i >> 48) & 0xFFFF));
            }
         isUpdatingData = true;
        for (int i = 0; i < inputValues.size(); ++i) {
            if (i < 16) {
                        if (!modbusServer->setData(QModbusDataUnit::InputRegisters, quint16(i), inputValues[i])) {
                            qDebug() << "Could not set input register:" << modbusServer->errorString();
                        }
                    } else {
                        qDebug() << "Warning: inputValues size exceeds the number of available input registers.";
                    }
           }
    qDebug() << "Converted values:" << valuesBack[0] << " "
             << valuesBack[1] << " " << valuesBack[2] << " " << valuesBack [3];
    qDebug() << "\n\n";

     isUpdatingData = false;
}
