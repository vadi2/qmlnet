#include <QmlNet/qml/NetQObject.h>

NetQObject::NetQObject(QObject* qObject, bool ownsObject) :
    _qObject(qObject),
    _ownsObject(ownsObject)
{
}

NetQObject::~NetQObject()
{
    if(_ownsObject) {
        _qObject->deleteLater();
    }
}

QObject* NetQObject::getQObject()
{
    return _qObject;
}

QSharedPointer<NetVariant> NetQObject::getProperty(QString propertyName)
{
    QSharedPointer<NetVariant> netVariant;
    QVariant result = _qObject->property(propertyName.toLocal8Bit().data());
    if(!result.isNull()) {
        netVariant = QSharedPointer<NetVariant>(new NetVariant());
        NetVariant::fromQVariant(&result, netVariant);
    }
    return netVariant;
}

void NetQObject::setProperty(QString propertyName, QSharedPointer<NetVariant> value)
{
    if(value == nullptr) {
        _qObject->setProperty(propertyName.toLocal8Bit().data(), QVariant());
    } else {
        _qObject->setProperty(propertyName.toLocal8Bit().data(), value->toQVariant());
    }
}

extern "C" {

Q_DECL_EXPORT void net_qobject_destroy(NetQObjectContainer* qObjectContainer)
{
    delete qObjectContainer;
}

Q_DECL_EXPORT NetVariantContainer* net_qobject_getProperty(NetQObjectContainer* qObjectContainer, LPWCSTR propertyName)
{
    auto result = qObjectContainer->qObject->getProperty(QString::fromUtf16(propertyName));
    if(result == nullptr) {
        return nullptr;
    }
    return new NetVariantContainer{ result };
}

Q_DECL_EXPORT void net_qobject_setProperty(NetQObjectContainer* qObjectContainer, LPWCSTR propertyName, NetVariantContainer* netVariantContainer)
{
    if(netVariantContainer == nullptr) {
        qObjectContainer->qObject->setProperty(QString::fromUtf16(propertyName), nullptr);
    } else {
        qObjectContainer->qObject->setProperty(QString::fromUtf16(propertyName), netVariantContainer->variant);
    }
}

}
