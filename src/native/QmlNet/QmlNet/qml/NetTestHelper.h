#ifndef NETTESTHELPER_H
#define NETTESTHELPER_H

#include <QmlNet.h>
#include <QmlNet/qml/QQmlApplicationEngine.h>

class TestQObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int readOnly READ getReadOnly)
    Q_PROPERTY(int writeOnly WRITE setWriteOnly)
    Q_PROPERTY(int readAndWrite READ getReadAndWrite WRITE setReadAndWrite)
    Q_PROPERTY(int propWithSignal READ getPropWithSignal WRITE setPropWithSignal NOTIFY propWithSignalChanged)
public:
    TestQObject();
    ~TestQObject();
    int getReadOnly();
    void setWriteOnly(int value);
    int getReadAndWrite();
    void setReadAndWrite(int value);
    int getPropWithSignal();
    void setPropWithSignal(int value);
signals:
    void propWithSignalChanged();
    void testSignal();
    void testSignalWithArg(int arg);
public slots:
    void testSlot();
    void testSlotWithArg(int arg);
private:
    int _writeOnly;
    int _readAndWrite;
    int _propWithSignal;
};

#endif // NETTESTHELPER_H
