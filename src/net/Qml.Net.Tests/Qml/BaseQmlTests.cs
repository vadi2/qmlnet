﻿using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Moq;
using Qml.Net.Internal;
using Qml.Net.Internal.Behaviors;
using Qml.Net.Internal.Qml;

namespace Qml.Net.Tests.Qml
{
    public class TestContext
    {
        public void Quit()
        {
            Quited = true;
        }

        public bool Quited { get; set; }
    }

    public abstract class AbstractBaseQmlTests<TTypeToRegister> : BaseTests
    {
        private readonly QGuiApplication _coreApplication;
        protected readonly QQmlApplicationEngine qmlApplicationEngine;

        protected MockTypeCreator TypeCreator { get;  private set; }

        readonly List<Type> _registeredTypes = new List<Type>();
        static bool _testContextRegistered = false;

        protected AbstractBaseQmlTests()
        {
            _coreApplication = new QGuiApplication(new[] { "-platform", "offscreen" });
            qmlApplicationEngine = new QQmlApplicationEngine();
            TypeCreator = new MockTypeCreator();
            Net.TypeCreator.Current = TypeCreator;
            TypeCreator.SetInstance(typeof(TestContext), new TestContext());
            if (!_testContextRegistered)
            {
                Net.Qml.RegisterType<TestContext>("testContext");
                _testContextRegistered = true;
            }
        }

        protected virtual void RegisterType<T>()
        {
            if (_registeredTypes.Contains(typeof(T))) return;
            _registeredTypes.Add(typeof(T));
            Net.Qml.RegisterType<T>("tests");
        }

        protected bool ExecApplicationWithTimeout(int timeoutMs)
        {
            var testContext = TypeCreator.Create(typeof(TestContext)) as TestContext;
            return QTest.QWaitFor(() => testContext.Quited, timeoutMs);
        }

        protected void RunQmlTest(string instanceId, string componentOnCompletedCode)
        {
            NetTestHelper.RunQml(
                qmlApplicationEngine,
                string.Format(
                    @"
                    import QtQuick 2.0
                    import tests 1.0
                    {0} {{
                        id: {1}
                        property var testQObject: null
                        function runTest() {{
                            {2}
                        }}
                    }}
            ",
            typeof(TTypeToRegister).Name,
            instanceId,
            componentOnCompletedCode));
        }

        public override void Dispose()
        {
            qmlApplicationEngine.Dispose();
            _coreApplication.Dispose();

            Net.TypeCreator.Current = null;

            base.Dispose();
        }
    }

    public abstract class BaseQmlTests<T> : AbstractBaseQmlTests<T>
        where T : class
    {
        protected readonly Mock<T> Mock;

        protected BaseQmlTests()
        {
            RegisterType<T>();
            Mock = new Mock<T>();
            TypeCreator.SetInstance(typeof(T), Mock.Object);
        }
    }

    public abstract class BaseQmlTestsWithInstance<T> : AbstractBaseQmlTests<T>
        where T : class, new()
    {
        protected readonly T Instance;

        protected BaseQmlTestsWithInstance()
        {
            RegisterType<T>();
            Instance = new T();
            TypeCreator.SetInstance(typeof(T), Instance);
        }
    }

    public abstract class BaseQmlMvvmTestsWithInstance<T> : AbstractBaseQmlTests<T>
        where T : class, new()
    {
        protected readonly T Instance;

        protected BaseQmlMvvmTestsWithInstance()
        {
            InteropBehaviors.ClearQmlInteropBehaviors();
            InteropBehaviors.RegisterQmlInteropBehavior(new MvvmQmlInteropBehavior());

            RegisterType<T>();
            Instance = new T();
            TypeCreator.SetInstance(typeof(T), Instance);
        }

        public override void Dispose()
        {
            InteropBehaviors.ClearQmlInteropBehaviors();
            base.Dispose();
        }
    }
}