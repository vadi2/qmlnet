using System;
using System.Dynamic;
using System.Runtime.InteropServices;

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
                var property = _qObject.GetProperty(binder.Name);
                if (property == null)
                {
                    result = null;
                }
                else
                {
                    object unpacked = null;
                    Helpers.Unpackvalue(ref unpacked, property);
                    result = unpacked;
                }
                return true;
            }

            public override bool TrySetMember(SetMemberBinder binder, object value)
            {
                if (value == null)
                {
                    _qObject.SetProperty(binder.Name, null);
                }
                else
                {
                    using (var variant = new NetVariant())
                    {
                        Helpers.PackValue(value, variant);
                        _qObject.SetProperty(binder.Name, variant);
                    }
                }

                return true;
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
    }
}