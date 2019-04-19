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
        public void Exception_throw_when_using_invalid_methods()
        {
            AssertQObject(qObject =>
            {
                // Valid
                qObject.InvokeMethod("testSlot");
                Assert.Throws<Exception>(() => { qObject.InvokeMethod("nonexistant"); });
            });
        }

        [Fact]
        public void Exception_thrown_when_using_invalid_properties()
        {
            AssertQObject(qObject =>
            {
                // Valid
                qObject.SetProperty("readAndWrite", 3);
                Assert.Throws<Exception>(() => { qObject.SetProperty("nonexistant", 3); });
                Assert.Throws<Exception>(() => { qObject.GetProperty("nonexistant"); });
            });
        }

        [Fact]
        public void Exception_thrown_when_attaching_to_invalid_signal()
        {
            AssertQObject(qObject =>
            {
                // Valid
                var handler = qObject.AttachSignal("testSignal", parameters =>
                {
                    
                });
                handler.Dispose();
                Assert.Throws<Exception>(() => { qObject.AttachSignal("nonexistant", parameters => {}); });
            });
        }
        
        [Fact]
        public void Can_get_property_on_qobject()
        {
            AssertQObject(qObject =>
            {
                qObject.GetProperty("readOnly").Should().Be(3);
            });
        }
        
        [Fact]
        public void Can_set_property_on_qobject()
        {
            AssertQObject(qObject =>
            {
                // No real way to test this.
                // I suppose it doesn't throw, eh?
                qObject.SetProperty("writeOnly", 3);
            });
        }
        
        [Fact]
        public void Can_set_and_get_property_on_qobject()
        {
            AssertQObject(qObject =>
            {
                qObject.SetProperty("readAndWrite", 340);
                qObject.GetProperty("readAndWrite").Should().Be(340);
            });
        }

        [Fact]
        public void Can_get_set_object_name()
        {
            AssertQObject(qObject =>
            {
                var d = (dynamic) qObject;
                d.objectName = "testtt";
                ((string) d.objectName).Should().Be("testtt");
            });
        }

        [Fact]
        public void Can_set_random_types_on_property()
        {
            AssertQObject(qObject =>
            {
                qObject.GetProperty("variantProperty").Should().BeNull();
                
                qObject.SetProperty("variantProperty", true);
                qObject.GetProperty("variantProperty").Should().Be(true);
                
                qObject.SetProperty("variantProperty", 'T');
                qObject.GetProperty("variantProperty").Should().Be('T');
                
                qObject.SetProperty("variantProperty", int.MinValue);
                qObject.GetProperty("variantProperty").Should().Be(int.MinValue);
                
                qObject.SetProperty("variantProperty", uint.MaxValue);
                qObject.GetProperty("variantProperty").Should().Be(uint.MaxValue);
         
                qObject.SetProperty("variantProperty", long.MaxValue);
                qObject.GetProperty("variantProperty").Should().Be(long.MaxValue);

                qObject.SetProperty("variantProperty", ulong.MaxValue);
                qObject.GetProperty("variantProperty").Should().Be(ulong.MaxValue);

                qObject.SetProperty("variantProperty", float.MaxValue);
                qObject.GetProperty("variantProperty").Should().Be(float.MaxValue);

                qObject.SetProperty("variantProperty", double.MaxValue);
                qObject.GetProperty("variantProperty").Should().Be(double.MaxValue);

                qObject.SetProperty("variantProperty", "");
                qObject.GetProperty("variantProperty").Should().Be("");
                
                qObject.SetProperty("variantProperty", "test");
                qObject.GetProperty("variantProperty").Should().Be("test");

                var dateTime = DateTimeOffset.Now;
                dateTime = new DateTimeOffset(dateTime.Year, dateTime.Month, dateTime.Day, dateTime.Hour, dateTime.Minute, dateTime.Second, dateTime.Millisecond, dateTime.Offset);
                qObject.SetProperty("variantProperty", dateTime);
                var resultDateTime = (DateTimeOffset)qObject.GetProperty("variantProperty");
                resultDateTime = new DateTimeOffset(resultDateTime.Year, resultDateTime.Month, resultDateTime.Day, resultDateTime.Hour, resultDateTime.Minute, resultDateTime.Second, resultDateTime.Millisecond, resultDateTime.Offset);
                resultDateTime.Should().Be(dateTime);
                
                var o = new object();
                qObject.SetProperty("variantProperty", o);
                qObject.GetProperty("variantProperty").Should().BeSameAs(o);
                
                qObject.SetProperty("objectName", "ttttt");
                qObject.SetProperty("variantProperty", qObject);
                var result = qObject.GetProperty("variantProperty");
                result.Should().NotBeNull();
                var resultQObject = result as INetQObject;
                resultQObject.Should().NotBeNull();
                resultQObject.GetProperty("objectName").Should().Be("ttttt");
            });
        }

        [Fact]
        public void Can_return_random_types_from_method()
        {
            AssertQObject(qObject =>
            {
                qObject.InvokeMethod("testVariantReturn").Should().Be(null);
                
                qObject.SetProperty("variantProperty", true);
                qObject.InvokeMethod("testVariantReturn").Should().Be(true);
                
                qObject.SetProperty("variantProperty", 'T');
                qObject.InvokeMethod("testVariantReturn").Should().Be('T');
                
                qObject.SetProperty("variantProperty", int.MinValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(int.MinValue);
                
                qObject.SetProperty("variantProperty", uint.MaxValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(uint.MaxValue);
         
                qObject.SetProperty("variantProperty", long.MaxValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(long.MaxValue);

                qObject.SetProperty("variantProperty", ulong.MaxValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(ulong.MaxValue);

                qObject.SetProperty("variantProperty", float.MaxValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(float.MaxValue);

                qObject.SetProperty("variantProperty", double.MaxValue);
                qObject.InvokeMethod("testVariantReturn").Should().Be(double.MaxValue);

                qObject.SetProperty("variantProperty", "");
                qObject.InvokeMethod("testVariantReturn").Should().Be("");
                
                qObject.SetProperty("variantProperty", "test");
                qObject.InvokeMethod("testVariantReturn").Should().Be("test");

                var dateTime = DateTimeOffset.Now;
                dateTime = new DateTimeOffset(dateTime.Year, dateTime.Month, dateTime.Day, dateTime.Hour, dateTime.Minute, dateTime.Second, dateTime.Millisecond, dateTime.Offset);
                qObject.SetProperty("variantProperty", dateTime);
                var resultDateTime = (DateTimeOffset)qObject.InvokeMethod("testVariantReturn");
                resultDateTime = new DateTimeOffset(resultDateTime.Year, resultDateTime.Month, resultDateTime.Day, resultDateTime.Hour, resultDateTime.Minute, resultDateTime.Second, resultDateTime.Millisecond, resultDateTime.Offset);
                resultDateTime.Should().Be(dateTime);
                
                var o = new object();
                qObject.SetProperty("variantProperty", o);
                qObject.InvokeMethod("testVariantReturn").Should().BeSameAs(o);
                
                qObject.SetProperty("objectName", "ttttt");
                qObject.SetProperty("variantProperty", qObject);
                var result = qObject.InvokeMethod("testVariantReturn");
                result.Should().NotBeNull();
                var resultQObject = result as INetQObject;
                resultQObject.Should().NotBeNull();
                resultQObject.GetProperty("objectName").Should().Be("ttttt");
            });
        }

        [Fact]
        public void Can_set_qobject_on_global_context_property()
        {
            AssertQObject(qObject =>
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
            AssertQObject(qObject =>
            {
                // TODO: Assert
                qObject.InvokeMethod("testSlot");
            });
        }
        
        [Fact]
        public void Can_attach_signal_to_qobject()
        {
            AssertQObject(qObject =>
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
            AssertQObject(qObject =>
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
                
                handler.Dispose();
                raised = false;
                
                qObject.InvokeMethod("testSlotWithArg", 33);
                raised.Should().BeFalse();
            });
        }
        
        private void AssertQObject(Action<INetQObject> action)
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