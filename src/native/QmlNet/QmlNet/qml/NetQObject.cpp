#include <QmlNet/qml/NetQObject.h>
#include <QmlNet/types/NetDelegate.h>
#include <QmlNet/types/Callbacks.h>
#include <QMetaMethod>
#include <QDebug>

NetQObjectSignalConnectionBase::NetQObjectSignalConnectionBase()
{

}

NetQObjectSignalConnectionBase::~NetQObjectSignalConnectionBase()
{

}

QMetaMethod NetQObjectSignalConnectionBase::getSignalHandler()
{
    return metaObject()->method(metaObject()->methodOffset());
}

void NetQObjectSignalConnectionBase::signalRaised()
{
    // Dummy, handled in NetQObjectSignalConnection::qt_metacall()
}

NetQObjectSignalConnection::NetQObjectSignalConnection(QSharedPointer<NetReference> delegate,
                                                       QObject* qObject)
    : _delegate(delegate),
      _qObject(qObject)
{

}

NetQObjectSignalConnection::~NetQObjectSignalConnection()
{

}

int NetQObjectSignalConnection::qt_metacall(QMetaObject::Call c, int id, void** a)
{
    if(c == QMetaObject::InvokeMetaMethod) {
        int offset = this->metaObject()->methodOffset();
        if(id < offset) {
            return QObject::qt_metacall(c, id, a);
        }

        if(this->metaObject()->indexOfSlot("signalRaised()") == id) {
            QMetaMethod signal = _qObject->metaObject()->method(senderSignalIndex());

            // Convert signal args to QVariantList
            QSharedPointer<NetVariantList> netParameters = QSharedPointer<NetVariantList>(new NetVariantList());
            for (int i = 0; i < signal.parameterCount(); ++i) {
                QVariant arg = QVariant(signal.parameterType(i), a[i+1]);
                netParameters->add(NetVariant::fromQVariant(&arg));
            }

            QmlNet::invokeDelegate(_delegate, netParameters);

            return -1;
        }
    }

    return id;
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

    int parameterCount = 0;
    if(parameters != nullptr) {
        parameterCount = parameters->count();
    }

    if(parameterCount > 10) {
        qWarning() << "Attempting to invoke a method with too many parameters: current: " << parameters->count() << " expected: <=10";
        // TODO: return error code indicating invalid call
        return nullptr;
    }

    QGenericReturnArgument returnValue;
    QGenericArgument val0 = QGenericArgument(nullptr);
    QGenericArgument val1 = QGenericArgument();
    QGenericArgument val2 = QGenericArgument();
    QGenericArgument val3 = QGenericArgument();
    QGenericArgument val4 = QGenericArgument();
    QGenericArgument val5 = QGenericArgument();
    QGenericArgument val6 = QGenericArgument();
    QGenericArgument val7 = QGenericArgument();
    QGenericArgument val8 = QGenericArgument();
    QGenericArgument val9 = QGenericArgument();

    QVariant val0Variant;
    QVariant val1Variant;
    QVariant val2Variant;
    QVariant val3Variant;
    QVariant val4Variant;
    QVariant val5Variant;
    QVariant val6Variant;
    QVariant val7Variant;
    QVariant val8Variant;
    QVariant val9Variant;

    if(parameterCount >= 1) {
        val0Variant = parameters->get(0)->toQVariant();
        val0 = QGenericArgument("QVariant", static_cast<void *>(&val0Variant));
    }
    if(parameterCount >= 2) {
        val1Variant = parameters->get(1)->toQVariant();
        val1 = QGenericArgument("QVariant", static_cast<void *>(&val1Variant));
    }
    if(parameterCount >= 3) {
        val2Variant = parameters->get(2)->toQVariant();
        val2 = QGenericArgument("QVariant", static_cast<void *>(&val2Variant));
    }
    if(parameterCount >= 4) {
        val3Variant = parameters->get(3)->toQVariant();
        val3 = QGenericArgument("QVariant", static_cast<void *>(&val3Variant));
    }
    if(parameterCount >= 5) {
        val4Variant = parameters->get(4)->toQVariant();
        val4 = QGenericArgument("QVariant", static_cast<void *>(&val4Variant));
    }
    if(parameterCount >= 6) {
        val5Variant = parameters->get(5)->toQVariant();
        val5 = QGenericArgument("QVariant", static_cast<void *>(&val5Variant));
    }
    if(parameterCount >= 7) {
        val6Variant = parameters->get(6)->toQVariant();
        val6 = QGenericArgument("QVariant", static_cast<void *>(&val6Variant));
    }
    if(parameterCount >= 8) {
        val7Variant = parameters->get(7)->toQVariant();
        val7 = QGenericArgument("QVariant", static_cast<void *>(&val7Variant));
    }
    if(parameterCount >= 9) {
        val8Variant = parameters->get(8)->toQVariant();
        val8 = QGenericArgument("QVariant", static_cast<void *>(&val8Variant));
    }
    if(parameterCount >= 10) {
        val9Variant = parameters->get(9)->toQVariant();
        val9 = QGenericArgument("QVariant", static_cast<void *>(&val9Variant));
    }

    if(!method.invoke(_qObject, Qt::DirectConnection, returnValue, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9)) {
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

    QSharedPointer<NetQObjectSignalConnection> signalConnection = QSharedPointer<NetQObjectSignalConnection>(new NetQObjectSignalConnection(delegate, _qObject));
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
