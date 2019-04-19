#ifndef NETQOBJECT_H
#define NETQOBJECT_H

#include <QmlNet.h>
#include <QmlNet/qml/NetVariant.h>
#include <QSharedPointer>

class NetVariant;
class NetVariantList;

class NetQObjectSignalConnectionBase : public QObject
{
    Q_OBJECT
public:
    NetQObjectSignalConnectionBase();
    ~NetQObjectSignalConnectionBase();
    QMetaMethod getSignalHandler();
public slots:
    void signalRaised();
};

class NetQObjectSignalConnection : public NetQObjectSignalConnectionBase
{
public:
    NetQObjectSignalConnection(QSharedPointer<NetReference> delegate, QObject* qObject);
    ~NetQObjectSignalConnection() override;
    int qt_metacall(QMetaObject::Call c, int id, void** a) override;
private:
    QSharedPointer<NetReference> _delegate;
    QObject* _qObject;
};

class NetQObject
{
public:
    NetQObject(QObject* qObject, bool ownsObject = false);
    ~NetQObject();
    QObject* getQObject();
    QSharedPointer<NetVariant> getProperty(QString propertyName);
    void setProperty(QString propertyName, QSharedPointer<NetVariant> value);
    QSharedPointer<NetVariant> invokeMethod(QString methodName, QSharedPointer<NetVariantList> parameters);
    QSharedPointer<NetQObjectSignalConnection> attachSignal(QString signalName, QSharedPointer<NetReference> delegate);
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
