namespace Lux
{
    namespace Meta
    {
        template<class T, class... Args> concept IsConstructible = __is_constructible(T, Args...);

        template<class T> concept IsClass = __is_class(T);

        template<class T> class IsConstClass
        {
        public:
            constexpr static bool Value = false;
        };

        template<class T> class IsConstClass<const T>
        {
        public:
            constexpr static bool Value = true;
        };
        
        template<class T> concept IsConst = IsConstClass<T>::Value;

        template<class T> class IsReferenceClass
        {
        public:
            constexpr static bool Value = false;
        };

        template<class T> class IsReferenceClass<T&>
        {
        public:
            constexpr static bool Value = true;
        };

        template<class T> class IsReferenceClass<T&&>
        {
        public:
            constexpr static bool Value = true;
        };

        template<class T> concept IsReference = IsReferenceClass<T>::Value;

        template<class T> class RemoveConstClass
        {
        public:
            using Type = T;
        };

        template<class T> class RemoveConstClass<const T>
        {
        public:
            using Type = T;
        };

        template<class T> using RemoveConst = typename RemoveConstClass<T>::Type;

        template<class T> using AddConst = const RemoveConst<T>;

        template<class T> concept IsFunction = !IsConst<AddConst<T>> && !IsReference<T>;

        template<class T> class RemoveVolatileClass
        {
        public:
            using Type = T;
        };

        template<class T> class RemoveVolatileClass<volatile T>
        {
        public:
            using Type = T;
        };

        template<class T> using RemoveVolatile = typename RemoveVolatileClass<T>::Type;

        template<class T> using RemoveConstVolatile = RemoveConst<RemoveVolatile<T>>;

        template<class T> class IsMemberFunctionPointerClass
        {
        public:
            constexpr static bool Value = false;
        };

        template<class T, class U> class IsMemberFunctionPointerClass<T U::*>
        {
        public:
            constexpr static bool Value = IsFunction<T>;
        };

        template<class T> concept IsMemberFunctionPointer = IsMemberFunctionPointerClass<RemoveConstVolatile<T>>::Value;
    }

    template<class R, class... Args> class Delegate
    {
    public:
        inline constexpr Delegate() noexcept = default;
        
        inline constexpr R operator()(Args&&... args) const noexcept requires Meta::IsConstructible<R, Args...>
        {
            return R(args...);
        }

        template<class C> inline constexpr R operator()(C func, Args&&... args) const noexcept 
        {
            return (func)(args...);
        }

        template<class O, class C> inline constexpr R operator()(O& obj, C func, Args&&... args) const noexcept requires Meta::IsClass<O> && Meta::IsMemberFunctionPointer<C>
        {
            return (obj.*func)(args...);
        }
    };
}
