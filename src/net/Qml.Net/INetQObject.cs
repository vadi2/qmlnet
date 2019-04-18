using System;

namespace Qml.Net
{
    public interface INetQObject : IDisposable
    {
        object GetProperty(string propertyName);

        void SetProperty(string propertyName, object value);

        object InvokeMethod(string methodName);
        
        IDisposable AttachSignal(string signalName, Delegate handler);
    }
}