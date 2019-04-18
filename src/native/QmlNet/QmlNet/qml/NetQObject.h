#ifndef NETQOBJECT_H
#define NETQOBJECT_H

#include <QmlNet.h>
#include <QmlNet/qml/NetVariant.h>
#include <QSharedPointer>

class NetVariant;
class NetVariantList;
class NetDelegate;

class NetQObjectSignalConnection : public QObject
{
    Q_OBJECT
public:
    NetQObjectSignalConnection(QSharedPointer<NetDelegate> netDelegate);
    ~NetQObjectSignalConnection();
public slots:
    void signalRaised();
private:
    QSharedPointer<NetDelegate> _netDelegate;
};

class NetQObject
{
public:
    NetQObject(QObject* qObject, bool ownsObject = false);
    ~NetQObject();
    QObject* getQObject();
    QSharedPointer<NetVariant> getProperty(QString propertyName);
    void setProperty(QString propertyName, QSharedPointer<NetVariant> value);
    QSharedPointer<NetVariant> invokeMethod(QString methodName);
    QSharedPointer<NetQObjectSignalConnection> attachSignal(QString signalName, QSharedPointer<NetDelegate> callback);
private:
    QObject* _qObject;
    bool _ownsObject;
};

struct NetQObjectContainer {
    QSharedPointer<NetQObject> qObject;
};

struct NetQObjectSignalConnectionContainer {
    QSharedPointer<NetQObjectSignalConnection> signalConnection;
};

#endif // NETQOBJECT_H
