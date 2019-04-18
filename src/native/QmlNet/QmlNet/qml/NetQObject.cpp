#include <QmlNet/qml/NetQObject.h>
#include <QmlNet/types/NetDelegate.h>
#include <QMetaMethod>
#include <QDebug>

NetQObjectSignalConnection::NetQObjectSignalConnection(QSharedPointer<NetDelegate> netDelegate)
    : _netDelegate(netDelegate)
{

}

NetQObjectSignalConnection::~NetQObjectSignalConnection()
{

}

void NetQObjectSignalConnection::signalRaised()
{
    // TODO:
}

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

QSharedPointer<NetVariant> NetQObject::invokeMethod(QString methodName)
{
    QMetaObject::invokeMethod(_qObject, methodName.toLocal8Bit().data(), Qt::DirectConnection);
    return nullptr;
}

QSharedPointer<NetQObjectSignalConnection> NetQObject::attachSignal(QString signalName, QSharedPointer<NetDelegate> callback)
{
    int signalMethodIndex = -1;
    for(int x = 0; _qObject->metaObject()->methodCount(); x++) {
        QMetaMethod method = _qObject->metaObject()->method(x);
        if(method.methodType() == QMetaMethod::Signal) {
            if(signalName.compare(method.name()) == 0) {
                signalMethodIndex = x;
                break;
            }
        }
    }

    // If signal not found, dump the registered signals for debugging.
    if(signalMethodIndex < 0) {
        qDebug() << "Signal not found:" << signalName;
        qDebug() << "Current signals:";
        for (int i = 0; i < _qObject->metaObject()->methodCount(); i++) {
            QMetaMethod method = _qObject->metaObject()->method(i);
            if (method.methodType() == QMetaMethod::Signal) {
                qDebug().nospace() << "\t" << method.methodSignature();
            }
        }
        return nullptr;
    }

    QSharedPointer<NetQObjectSignalConnection> signalConnection = QSharedPointer<NetQObjectSignalConnection>(new NetQObjectSignalConnection(callback));
    QMetaObject::Connection connection = QObject::connect(_qObject,
                                                          SIGNAL(testSignal()),
                                                          signalConnection.data(),
                                                          SLOT(signalRaised()));
    qDebug() << connection;
    return signalConnection;
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

Q_DECL_EXPORT NetVariantContainer* net_qobject_invokeMethod(NetQObjectContainer* qObjectContainer, LPWCSTR methodName)
{
    auto result = qObjectContainer->qObject->invokeMethod(QString::fromUtf16(methodName));
    if(result == nullptr) {
        return nullptr;
    }
    return new NetVariantContainer { result };
}

Q_DECL_EXPORT NetQObjectSignalConnectionContainer* net_qobject_attachSignal(NetQObjectContainer* qObjectContainer, LPWCSTR signalName, NetDelegateContainer netDelegate)
{
    auto signalConnection = qObjectContainer->qObject->attachSignal(QString::fromUtf16(signalName), netDelegate.delegate);
    return new NetQObjectSignalConnectionContainer{ signalConnection };
}

Q_DECL_EXPORT void net_qobject_signal_handler_destroy(NetQObjectSignalConnectionContainer* signalContainer)
{
    delete signalContainer;
}

}
