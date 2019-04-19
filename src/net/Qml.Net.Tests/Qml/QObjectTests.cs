using System;
using System.Runtime.CompilerServices;
using FluentAssertions;
using FluentAssertions.Execution;
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
        public void Can_get_property_on_qobject()
        {
            Assert(qObject =>
            {
                qObject.GetProperty("readOnly").Should().Be(3);
            });
        }
        
        [Fact]
        public void Can_set_property_on_qobject()
        {
            Assert(qObject =>
            {
                // No real way to test this.
                // I suppose it doesn't throw, eh?
                qObject.SetProperty("writeOnly", 3);
            });
        }
        
        [Fact]
        public void Can_set_and_get_property_on_qobject()
        {
            Assert(qObject =>
            {
                qObject.SetProperty("readAndWrite", 340);
                qObject.GetProperty("readAndWrite").Should().Be(340);
            });
        }

        [Fact]
        public void Can_get_set_object_name()
        {
            Assert(qObject =>
            {
                var d = (dynamic) qObject;
                d.objectName = "testtt";
                ((string) d.objectName).Should().Be("testtt");
            });
        }

        [Fact]
        public void Can_set_qobject_on_global_context_property()
        {
            Assert(qObject =>
            {
                var d = (dynamic) qObject;
                var property = Guid.NewGuid().ToString().Replace("-", "");
                d.objectName = property;
                qmlApplicationEngine.SetContextProperty(property, d);
                var result = qmlApplicationEngine.GetContextProperty(property);
                (result is INetQObject).Should().BeTrue();
                result.Should().NotBeNull();
                ((string)d.objectName).Should().Be(property);
            });
        }

        [Fact]
        public void Can_invoke_method_on_qobject()
        {
            Assert(qObject =>
            {
                // TODO: Assert
                qObject.InvokeMethod("testSlot");
            });
        }
        
        [Fact]
        public void Can_attach_signal_to_qobject()
        {
            Assert(qObject =>
            {
                var raised = 0;
                var handler = qObject.AttachSignal("testSignal", parameters =>
                {
                    raised++;
                });
                handler.Should().NotBeNull();
                
                qObject.InvokeMethod("testSlot");
                raised.Should().Be(1);
                
                handler.Dispose();

                qObject.InvokeMethod("testSlot");
                raised.Should().Be(1);
            });
        }

        [Fact]
        public void Can_attach_signal_with_arg_to_qobject()
        {
            Assert(qObject =>
            {
                var raised = false;
                var handler = qObject.AttachSignal("testSignalWithArg", parameters =>
                {
                    raised = true;
                    parameters.Should().NotBeNull();
                    parameters.Count.Should().Be(1);
                    parameters[0].Should().Be(33);
                });

                qObject.InvokeMethod("testSlotWithArg", 33);
            });
        }

        private void Assert(Action<INetQObject> action)
        {
            Exception assertException = null;
            
            Mock.Setup(x => x.Method(It.IsAny<INetQObject>()))
                .Callback(new Action<dynamic>(x =>
                {
                    try
                    {
                        action(x as INetQObject);
                    }
                    catch (Exception ex)
                    {
                        assertException = ex;
                    }
                }));
            
            RunQmlTest(
                "test",
                @"
                    test.method(testQObject)
                ");

            Mock.Verify(x => x.Method(It.IsAny<INetQObject>()), Times.Once);
            if (assertException != null)
            {
                throw new Exception(assertException.Message, assertException);
            }
        }
    }
}