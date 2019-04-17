#ifndef NETQOBJECT_H
#define NETQOBJECT_H

#include <QmlNet.h>
#include <QmlNet/qml/NetVariant.h>
#include <QSharedPointer>

class NetVariant;
class NetVariantList;

class NetQObject
{
public:
    NetQObject(QObject* qObject, bool ownsObject = false);
    ~NetQObject();
    QObject* getQObject();
    QSharedPointer<NetVariant> getProperty(QString propertyName);
    void setProperty(QString propertyName, QSharedPointer<NetVariant> value);
private:
    QObject* _qObject;
    bool _ownsObject;
};

struct NetQObjectContainer {
    QSharedPointer<NetQObject> qObject;
};

#endif // NETQOBJECT_H
