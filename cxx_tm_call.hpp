#ifndef __YZ11_HELPER_H
#define __YZ11_HELPER_H

#define CXX11_CURRENT_SOURCE_LOCATION __FILE__, __func__, __LINE__
#define CXX11_SOURCE_LOCATION_PARAMS const char *currentFileName, const char *currentFuncName, int currentLineNum
#define CXX11_SOURCE_LOCATION_ARGS currentFileName, currentFuncName, currentLineNum

#define RT_TYPE(Tx) typename std::enable_if<!std::is_void<Tx>::value,Tx>::type
#define RT_VOID(Tx) typename std::enable_if<std::is_void<Tx>::value,Tx>::type

#define CXX11_FN_AND_NAME(func) func, #func

template<typename _R, typename _F, typename... _T>
_R any_call(_F f, _T&&... vargs) {
    if(std::is_void<_R>::value)
        f(vargs...);
    else
        return (_R) f(vargs...);
}

#include <chrono>

struct ComplexReturnCodeWrapper;
namespace yz11_helper {
    typedef std::chrono::steady_clock std_clock;

    typedef ComplexReturnCodeWrapper return_value_wrapper;

    template<typename _T>
    using remove_cvr = typename std::remove_cv<typename std::remove_reference<_T>::type>::type;

    // https://riptutorial.com/cplusplus/example/3777/enable-if
    inline void zlog_elapsed_time(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        const char *fn, int elapsedTimeMs) {
        zLog(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:VOID, time:%dms }", fn, elapsedTimeMs);
    }

    template<typename _T>
    inline void zlog_elapsed_time(
        typename std::enable_if<
            (std::is_integral<_T>::value || std::is_same<remove_cvr<_T>,return_value_wrapper>::value)
            ,_T>::type& v,
        int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS, const char *fn,
        int elapsedTimeMs) {
        zLog(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:%d, time:%dms }", fn, (int)v, elapsedTimeMs);
    }

    template<typename _T, typename =
        typename std::enable_if<
            (!std::is_same<remove_cvr<_T>,return_value_wrapper>::value || std::is_floating_point<_T>::value)
        >::type>
    inline void zlog_elapsed_time(const _T v,
        const int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        const char *fn, int elapsedTimeMs) {
        zLog(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:%s, time:%dms }", fn, typeid(v).name(), elapsedTimeMs);
    }

    template<typename _T>
    inline RT_VOID(_T) assign(int& s) {
    }

    #if 0
    template<typename _T>
    void __yz11_any_intializer(_T& t, typename std::enable_if<std::is_arithmetic<_T>::value,int>::type = 0) { t = 0; }
    template<typename _T>
    void __yz11_any_intializer(_T t, typename std::enable_if<!std::is_arithmetic<_T>::value,int>::type = 0) {}
    #endif

    template<typename _T>
    inline typename std::enable_if<
        !std::is_void<_T>::value &&
        !std::is_arithmetic<_T>::value &&
        !std::is_assignable<_T,int>::value
        ,_T>::type
    assign(const int s) {
        _T d;
        return d;
    }

    template<typename _T>
    inline typename std::enable_if<std::is_arithmetic<_T>::value||std::is_assignable<_T,int>::value,_T>::type assign(int s) {
        _T d;
        d = s;
        return d;
    }

    template<typename _RetType, typename _F, typename... _T>
    RT_TYPE(_RetType) tm_call_zlog(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _F f, const char *fn, _T&&... vargs)
    {
        auto t0 = yz11_helper::std_clock::now();
        _RetType retVal = f(std::forward<_T>(vargs)...);
        int elapsedTimeMs = (int) std::chrono::duration_cast<std::chrono::milliseconds>(yz11_helper::std_clock::now() - t0).count();
        zlog_elapsed_time<_RetType>(retVal, level, tag, CXX11_SOURCE_LOCATION_ARGS, fn, elapsedTimeMs);
        return retVal;
    }

    template<typename _RetType, typename _F, typename... _T>
    RT_VOID(_RetType) tm_call_zlog(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _F f, const char *fn, _T&&... vargs)
    {
        auto t0 = yz11_helper::std_clock::now();
        f(std::forward<_T>(vargs)...);
        int elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std_clock::now() - t0).count();
        zlog_elapsed_time(level, tag, CXX11_SOURCE_LOCATION_ARGS, fn, elapsedTimeMs);
    }

    template<typename _RetType, typename _C, typename _F, typename... _T>
    _RetType algo_call_catch_exception(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _C c, _F f, const char *fn, _T&&... vargs)
    {
        int retInt, elapsedTimeMs = -1;
        try {
            return (_RetType) c(CXX11_SOURCE_LOCATION_ARGS, f, fn, std::forward<_T>(vargs)...);
        } catch(const cv::Exception &e) {
            ZLOGW("[1] exception caught on %s : %s", fn, e.what());
            retInt = IRS2_ERROR_OPENCV_EXCEPTION;
        } catch(const std::string& e) {
            ZLOGW("[2] exception caught on %s : %s", fn, e.c_str());
            retInt = IRS2_ERROR_ALGO_EXCEPTION;
        } catch(...) {
            ZLOGW("(3) exception caught on %s", fn);
            retInt = -8997;
        }
        return (_RetType) yz11_helper::assign<_RetType>(retInt);
    }
};

template<typename _F, typename... _T>
auto yz11_tm_call(int& elapsedTimeMs, _F& f, _T&&... vargs) -> RT_TYPE(decltype(f(vargs...)))
{
    typedef RT_TYPE(decltype(f(vargs...))) RetType;
    auto t0 = yz11_helper::std_clock::now();
    RetType retVal = f(std::forward<_T>(vargs)...);
	elapsedTimeMs = (int) std::chrono::duration_cast<std::chrono::milliseconds>(yz11_helper::std_clock::now() - t0).count();
    return retVal;
}

template<typename _F, typename... _T>
auto yz11_tm_call(int& elapsedTimeMs, _F& f, _T&&... vargs) -> RT_VOID(decltype(f(vargs...)))
{
    auto t0 = yz11_helper::std_clock::now();
    f(std::forward<_T>(vargs)...);
	elapsedTimeMs = (int) std::chrono::duration_cast<std::chrono::milliseconds>(yz11_helper::std_clock::now() - t0).count();
}

template<typename _F, typename... _T>
auto yz11_tm_call_zlog(CXX11_SOURCE_LOCATION_PARAMS,
    _F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
//    std::cout << "TYPE: " << typeid(std::declval<RetType>()).name() << "\n";
//    RetType v; return v;
    return (RetType) yz11_helper::tm_call_zlog<RetType>(3, NULL, CXX11_SOURCE_LOCATION_ARGS, f, fn, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto yz11_tm_call_nozlog(CXX11_SOURCE_LOCATION_PARAMS,
    _F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    int elapsedTimeMs;
    return (RetType) yz11_tm_call(elapsedTimeMs, f, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto yz11_algo_call_catch_exception(_F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    auto c = yz11_tm_call_nozlog<_F,_T&&...>;
	return (RetType) yz11_helper::algo_call_catch_exception<RetType,decltype(c),_F,_T...>(-1, nullptr, CXX11_CURRENT_SOURCE_LOCATION,
        c, f, fn, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto yz11_algo_call_catch_exception_zlog(CXX11_SOURCE_LOCATION_PARAMS,
    _F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    auto c = yz11_tm_call_zlog<_F,_T&&...>;
	return (RetType) yz11_helper::algo_call_catch_exception<RetType,decltype(c),_F,_T...>(IRS_LOG_DEBUG, IRS2_LOG_TAG_ALGO2, CXX11_SOURCE_LOCATION_ARGS,
        c, f, fn, std::forward<_T>(vargs)...);
}

#define YZ11_TM_CALL_ZLOG(func, ...) \
    yz11_tm_call_zlog(CXX11_CURRENT_SOURCE_LOCATION, CXX11_FN_AND_NAME(func), ##__VA_ARGS__)
#define YZ11_ALGO_CALL_CATCH_EXCEPTION(func, ...) \
    yz11_algo_call_catch_exception(CXX11_FN_AND_NAME(func), ##__VA_ARGS__)
#define YZ11_ALGO_CALL_CATCH_EXCEPTION_ZLOG(func, ...) \
    yz11_algo_call_catch_exception_zlog(CXX11_CURRENT_SOURCE_LOCATION, CXX11_FN_AND_NAME(func), ##__VA_ARGS__)

//	ZLOG_TAG_IF(LOG_ENABLED(Algo2), IRS2_LOG_TAG_ALGO2,
//        "{ algo:" #func ", ret:%d, time:%ums }", (int) ret, elapsedTimeMs);
//    zLogIf(LOG_ENABLED(Algo2), IRS_LOG_DEBUG,
//    static_assert(std::is_convertible_v<_R, int>, "why");

#undef RT_TYPE
#undef RT_VOID

#endif
