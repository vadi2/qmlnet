#include <QmlNet/qml/NetTestHelper.h>
#include <QQmlComponent>
#include <QDebug>

TestQObject::TestQObject()
    : QObject(nullptr),
    _writeOnly(0),
    _readAndWrite(0),
    _propWithSignal(0)
{

}

TestQObject::~TestQObject()
{

}

int TestQObject::getReadOnly()
{
    return 3;
}

void TestQObject::setWriteOnly(int value)
{
    _writeOnly = value;
}

int TestQObject::getReadAndWrite()
{
    return _readAndWrite;
}

void TestQObject::setReadAndWrite(int value)
{
    _readAndWrite = value;
}

int TestQObject::getPropWithSignal()
{
    return _propWithSignal;
}

void TestQObject::setPropWithSignal(int value)
{
    if(value == _propWithSignal) {
        return;
    }
    _propWithSignal = value;
    emit propWithSignalChanged();
}

void TestQObject::testSlot()
{
    emit testSignal();
}

void TestQObject::testSlotWithArg(int arg)
{
    emit testSignalWithArg(arg);
}

extern "C" {

Q_DECL_EXPORT void net_test_helper_runQml(QQmlApplicationEngineContainer* qmlEngineContainer, LPWSTR qml) {
    QQmlComponent component(qmlEngineContainer->qmlEngine);
    QString qmlString = QString::fromUtf16(static_cast<const char16_t*>(qml));
    component.setData(qmlString.toUtf8(), QUrl());

    QObject *object = component.create();

    if(object == nullptr) {
        qWarning() << "Couldn't create qml object.";
        return;
    }

    QSharedPointer<TestQObject> testQObject = QSharedPointer<TestQObject>(new TestQObject());
    object->setProperty("testQObject", QVariant::fromValue(testQObject.data()));
    QMetaObject::invokeMethod(object, "runTest");

    delete object;
}

}
