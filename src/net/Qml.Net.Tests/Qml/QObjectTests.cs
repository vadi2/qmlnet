using System;
using FluentAssertions;
using Moq;
using Xunit;

namespace Qml.Net.Tests.Qml
{
    public class QObjectTests : BaseQmlTests<QObjectTests.QObjectTestsQml>
    {
        public class QObjectTestsQml
        {
            public virtual void Method(dynamic value)
            {
            }
        }

        [Fact]
        public void Can_get_qobject()
        {
            INetQObject qObject = null;
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x => qObject = x));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
            qObject.Should().NotBeNull();
        }

        [Fact]
        public void Can_get_property_on_qobject()
        {
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    ((int) x.readOnly).Should().Be(3);
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
        }
        
        [Fact]
        public void Can_set_property_on_qobject()
        {
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    // No real way to test this.
                    // I suppose it doesn't throw, eh?
                    x.writeOnly = 0;
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
        }
        
        [Fact]
        public void Can_set_and_get_property_on_qobject()
        {
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    x.readAndWrite = 340;
                    ((int) x.readAndWrite).Should().Be(340);
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
        }

        [Fact]
        public void Can_get_set_object_name()
        {
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    x.objectName = "testtt";
                    ((string) x.objectName).Should().Be("testtt");
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
        }

        [Fact]
        public void Can_set_qobject_on_global_context_property()
        {
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    var property = Guid.NewGuid().ToString().Replace("-", "");
                    x.objectName = property;
                    qmlApplicationEngine.SetContextProperty(property, x);
                    var result = qmlApplicationEngine.GetContextProperty(property);
                    (result is INetQObject).Should().BeTrue();
                    result.Should().NotBeNull();
                    ((string)((dynamic) result).objectName).Should().Be(property);
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
        }
    }
}