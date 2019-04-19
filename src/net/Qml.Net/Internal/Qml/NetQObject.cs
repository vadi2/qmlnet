using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Runtime.InteropServices;
using Qml.Net.Internal.Types;

namespace Qml.Net.Internal.Qml
{
    internal class NetQObject : BaseDisposable
    {
        public NetQObject(IntPtr handle, bool ownsHandle = true) : base(handle, ownsHandle)
        {
        }

        protected override void DisposeUnmanaged(IntPtr ptr)
        {
            Interop.NetQObject.Destroy(ptr);
        }

        public NetVariant GetProperty(string propertyName)
        {
            var result = Interop.NetQObject.GetProperty(Handle, propertyName);
            return result != IntPtr.Zero ? new NetVariant(result) : null;
        }

        public void SetProperty(string propertyName, NetVariant value)
        {
            Interop.NetQObject.SetProperty(Handle, propertyName, value?.Handle ?? IntPtr.Zero);
        }

        public NetVariant InvokeMethod(string methodName)
        {
            var result = Interop.NetQObject.InvokeMethod(Handle, methodName);
            return result != IntPtr.Zero ? new NetVariant(result) : null;
        }

        public NetQObjectSignalConnection AttachSignal(string signalName, Del del)
        {
            using (var delReference = NetReference.CreateForObject(del))
            {
                var result = Interop.NetQObject.AttachSignal(Handle, signalName, delReference.Handle);
                return result == IntPtr.Zero ? null : new NetQObjectSignalConnection(result);
            }
        }
        
        public dynamic AsDynamic()
        {
            return new NetQObjectDynamic(this);
        }

        internal class NetQObjectDynamic : DynamicObject, INetQObject
        {
            readonly NetQObject _qObject;

            public NetQObjectDynamic(NetQObject qObject)
            {
                _qObject = qObject;
            }

            public void Dispose()
            {
                _qObject.Dispose();
            }

            public NetQObject QObject => _qObject;
            
            public override bool TryGetMember(GetMemberBinder binder, out object result)
            {
                result = GetProperty(binder.Name);
                // TODO: Check if this was actually a property
                return true;
            }

            public override bool TrySetMember(SetMemberBinder binder, object value)
            {
                SetProperty(binder.Name, value);
                // TODO: Check if this was actually a property
                return true;
            }

            public object GetProperty(string propertyName)
            {
                var property = _qObject.GetProperty(propertyName);
                if (property == null)
                {
                    return null;
                }

                object unpacked = null;
                Helpers.Unpackvalue(ref unpacked, property);
                property.Dispose();
                return unpacked;
            }

            public void SetProperty(string propertyName, object value)
            {
                if (value == null)
                {
                    _qObject.SetProperty(propertyName, null);
                }
                else
                {
                    using (var variant = new NetVariant())
                    {
                        Helpers.PackValue(value, variant);
                        _qObject.SetProperty(propertyName, variant);
                    }
                }
            }

            public object InvokeMethod(string methodName)
            {
                using (var result = _qObject.InvokeMethod(methodName))
                {
                    if (result == null)
                    {
                        return null;
                    }
                    object unpacked = null;
                    Helpers.Unpackvalue(ref unpacked, result);
                    result.Dispose();
                    return unpacked;
                }
            }

            public IDisposable AttachSignal(string signalName, SignalHandler handler)
            {
                var del = new Del();
                del.Invoked += parameters =>
                {
                    var result = new List<object>();
                    
                    var parametersCount = parameters.Count;
                    for (var x = 0; x < parametersCount; x++)
                    {
                        using (var parameter = parameters.Get(x))
                        {
                            object parameterValue = null;
                            Helpers.Unpackvalue(ref parameterValue, parameter);
                            result.Add(parameterValue);
                        }
                    }

                    handler(result);
                };
                
                return _qObject.AttachSignal(signalName, del);
            }
        }
    }
    
    internal class NetQObjectInterop
    {
        [NativeSymbol(Entrypoint = "net_qobject_destroy")]
        public DestroyDel Destroy { get; set; }

        public delegate void DestroyDel(IntPtr qObject);
        
        [NativeSymbol(Entrypoint = "net_qobject_getProperty")]
        public GetPropertyDel GetProperty { get; set; }

        public delegate IntPtr GetPropertyDel(IntPtr qObject, [MarshalAs(UnmanagedType.LPWStr)] string propertyName);

        [NativeSymbol(Entrypoint = "net_qobject_setProperty")]
        public SetPropertyDel SetProperty { get; set; }
        
        public delegate IntPtr SetPropertyDel(IntPtr qObject, [MarshalAs(UnmanagedType.LPWStr)] string propertyName, IntPtr netVariant);

        [NativeSymbol(Entrypoint = "net_qobject_invokeMethod")]
        public InvokeMethodDel InvokeMethod { get; set; }
        
        public delegate IntPtr InvokeMethodDel(IntPtr qObject, [MarshalAs(UnmanagedType.LPWStr)] string methodName);
        
        [NativeSymbol(Entrypoint = "net_qobject_attachSignal")]
        public AttachSignalDel AttachSignal { get; set; }
        
        public delegate IntPtr AttachSignalDel(IntPtr qObject, [MarshalAs(UnmanagedType.LPWStr)] string signalName, IntPtr del);
    }
}