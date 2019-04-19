#include <QmlNet/qml/NetQObject.h>
#include <QmlNet/types/NetDelegate.h>
#include <QmlNet/types/Callbacks.h>
#include <QMetaMethod>
#include <QDebug>

NetQObjectSignalConnection::NetQObjectSignalConnection(QSharedPointer<NetReference> delegate)
    : _delegate(delegate)
{

}

NetQObjectSignalConnection::~NetQObjectSignalConnection()
{

}

QMetaMethod NetQObjectSignalConnection::getSignalHandler()
{
    return metaObject()->method(metaObject()->methodOffset());
}

void NetQObjectSignalConnection::signalRaised()
{
    QmlNet::invokeDelegate(_delegate, QSharedPointer<NetVariantList>(new NetVariantList()));
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

QSharedPointer<NetVariant> NetQObject::invokeMethod(QString methodName, QSharedPointer<NetVariantList> parameters)
{
    // Find the best method
    int methodIndex = -1;
    QMetaMethod method;
    for(int x = 0; x < _qObject->metaObject()->methodCount(); x++) {
        method = _qObject->metaObject()->method(x);
        if(method.methodType() == QMetaMethod::Slot || method.methodType() == QMetaMethod::Method) {
            if(methodName.compare(method.name()) == 0) {
                methodIndex = x;
                break;
            }
        }
    }

    if(methodIndex == -1) {
        qDebug() << "method not found: " << methodName;
        _qObject->dumpObjectInfo();
        return nullptr;
    }

//    QGenericReturnArgument returnValue,
//    QGenericArgument val0 = QGenericArgument(nullptr),
//    QGenericArgument val1 = QGenericArgument(),
//    QGenericArgument val2 = QGenericArgument(),
//    QGenericArgument val3 = QGenericArgument(),
//    QGenericArgument val4 = QGenericArgument(),
//    QGenericArgument val5 = QGenericArgument(),
//    QGenericArgument val6 = QGenericArgument(),
//    QGenericArgument val7 = QGenericArgument(),
//    QGenericArgument val8 = QGenericArgument(),
//    QGenericArgument val9 = QGenericArgument()
    QGenericReturnArgument returnValue;
    int v = parameters->get(0)->getInt();
    QGenericArgument val0 = QGenericArgument("Int", static_cast<void *>(&v));
    if(!method.invoke(_qObject, Qt::DirectConnection, returnValue, val0)) {
        return nullptr;
    }

    return nullptr;
}

QSharedPointer<NetQObjectSignalConnection> NetQObject::attachSignal(QString signalName, QSharedPointer<NetReference> delegate)
{
    int signalMethodIndex = -1;
    QMetaMethod signalMethod;
    for(int x = 0; _qObject->metaObject()->methodCount(); x++) {
        signalMethod = _qObject->metaObject()->method(x);
        if(signalMethod.methodType() == QMetaMethod::Signal) {
            if(signalName.compare(signalMethod.name()) == 0) {
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
        _qObject->dumpObjectInfo();
        return nullptr;
    }

    QSharedPointer<NetQObjectSignalConnection> signalConnection = QSharedPointer<NetQObjectSignalConnection>(new NetQObjectSignalConnection(delegate));
    qDebug() << signalConnection->getSignalHandler().name();
    QMetaObject::Connection connection = QObject::connect(_qObject,
                                                          signalMethod,
                                                          signalConnection.data(),
                                                          signalConnection->getSignalHandler());

    if(!connection) {
        qDebug() << "Couldn't connect";
        return nullptr;
    }

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

Q_DECL_EXPORT NetVariantContainer* net_qobject_invokeMethod(NetQObjectContainer* qObjectContainer, LPWCSTR methodName, NetVariantListContainer* parametersContainer)
{
    QSharedPointer<NetVariantList> parameters = nullptr;
    if(parametersContainer != nullptr) {
        parameters = parametersContainer->list;
    }
    auto result = qObjectContainer->qObject->invokeMethod(QString::fromUtf16(methodName), parameters);
    if(result == nullptr) {
        return nullptr;
    }
    return new NetVariantContainer { result };
}

Q_DECL_EXPORT NetQObjectSignalConnectionContainer* net_qobject_attachSignal(NetQObjectContainer* qObjectContainer, LPWCSTR signalName, NetReferenceContainer delegate)
{
    auto signalConnection = qObjectContainer->qObject->attachSignal(QString::fromUtf16(signalName), delegate.instance);
    return new NetQObjectSignalConnectionContainer{ signalConnection };
}

Q_DECL_EXPORT void net_qobject_signal_handler_destroy(NetQObjectSignalConnectionContainer* signalContainer)
{
    delete signalContainer;
}

}
