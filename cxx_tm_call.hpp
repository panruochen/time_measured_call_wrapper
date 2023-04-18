/*
 *  A time measured function call wrapper library using c++11 metaprogramming.
 *
 *  Copyright (c) 2022-2023, Pan Ruochen <ijkxyz@msn.com>.
 */
#ifndef __YZ11_HELPER_H
#define __YZ11_HELPER_H

#define CXX11_CURRENT_SOURCE_LOCATION __FILE__, __func__, __LINE__
#define CXX11_SOURCE_LOCATION_PARAMS const char *currentFileName, const char *currentFuncName, int currentLineNum
#define CXX11_SOURCE_LOCATION_ARGS currentFileName, currentFuncName, currentLineNum

#define RT_TYPE(Tx) typename std::enable_if<!std::is_void<Tx>::value,Tx>::type
#define RT_VOID(Tx) typename std::enable_if<std::is_void<Tx>::value,Tx>::type

#define CXX11_FN_AND_NAME(func) func, #func

enum XLOG_LEVELS {
    XLOG_LEVEL_FATAL = 0,
    XLOG_LEVEL_ERROR = 1,
    XLOG_LEVEL_WARNING = 2,
    XLOG_LEVEL_INFO = 3,
    XLOG_LEVEL_DEBUG = 4,
    XLOG_LEVEL_VERBOSE = 5,
    XLOG_LEVEL_VERBOSE2 = 6,
};

#include <chrono>

struct ComplexReturnCodeWrapper;
namespace yz11_helper {
    typedef std::chrono::steady_clock std_clock;

    typedef ComplexReturnCodeWrapper return_value_wrapper;

    template<typename _T>
    using remove_cvr = typename std::remove_cv<typename std::remove_reference<_T>::type>::type;

    int calc_elapsed_time(const std::chrono::time_point<std_clock>& t0) {
        return (int) std::chrono::duration_cast<std::chrono::milliseconds>(yz11_helper::std_clock::now() - t0).count();
    }

    // https://riptutorial.com/cplusplus/example/3777/enable-if
    inline void print_elapsed_time(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        const char *fn, int elapsedTimeMs) {
        logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:VOID, time:%dms }", fn, elapsedTimeMs);
    }

    template<typename _T>
    inline void print_elapsed_time(
        typename std::enable_if<
            (std::is_integral<_T>::value || std::is_same<remove_cvr<_T>,return_value_wrapper>::value)
            ,_T>::type& v,
        int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS, const char *fn,
        int elapsedTimeMs) {
        logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:%d, time:%dms }", fn, (int)v, elapsedTimeMs);
    }

    template<typename _T, typename =
        typename std::enable_if<
            (!std::is_same<remove_cvr<_T>,return_value_wrapper>::value || std::is_floating_point<_T>::value)
        >::type>
    inline void print_elapsed_time(const _T v,
        const int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        const char *fn, int elapsedTimeMs) {
        logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS,
            "{ func:%s, ret:%s, time:%dms }", fn, typeid(v).name(), elapsedTimeMs);
    }

    template<typename _RetType, typename _F, typename... _T>
    RT_TYPE(_RetType) tm_call_with_logs(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _F f, const char *fn, _T&&... vargs)
    {
        auto t0 = std_clock::now();
        _RetType retVal = f(std::forward<_T>(vargs)...);
        print_elapsed_time<_RetType>(retVal, level, tag, CXX11_SOURCE_LOCATION_ARGS, fn, calc_elapsed_time(t0));
        return retVal;
    }

    template<typename _RetType, typename _F, typename... _T>
    RT_VOID(_RetType) tm_call_with_logs(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _F f, const char *fn, _T&&... vargs)
    {
        auto t0 = std_clock::now();
        f(std::forward<_T>(vargs)...);
        print_elapsed_time(level, tag, CXX11_SOURCE_LOCATION_ARGS, fn, calc_elapsed_time(t0));
    }

    template<typename _RetType, typename _C, typename _F, typename... _T>
    _RetType tm_call_catching_exception(int level, const char *tag, CXX11_SOURCE_LOCATION_PARAMS,
        _C c, _F f, const char *fn,
        typename std::conditional<std::is_void<_RetType>::value, int, _RetType>::type exceptionRetVal, _T&&... vargs)
    {
        try {
            return (_RetType) c(CXX11_SOURCE_LOCATION_ARGS, f, fn, std::forward<_T>(vargs)...);
        } catch(const int e) {
            if(level >= 0)
                logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS, "[1] exception caught on %s : %d", fn, e);
        } catch(const std::string& e) {
            if(level >= 0)
                logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS, "[2] exception caught on %s : %s", fn, e.c_str());
        } catch(...) {
            if(level >= 0)
                logPrint(level, tag, CXX11_SOURCE_LOCATION_ARGS, "(3) exception caught on %s", fn);
        }
        return (_RetType) exceptionRetVal;
    }
}; // End of yz11_helper

template<typename _F, typename... _T>
auto yz11_tm_call(int& elapsedTimeMs, _F& f, _T&&... vargs) -> RT_TYPE(decltype(f(vargs...)))
{
    typedef RT_TYPE(decltype(f(vargs...))) RetType;
    auto t0 = yz11_helper::std_clock::now();
    RetType retVal = f(std::forward<_T>(vargs)...);
	elapsedTimeMs = yz11_helper::calc_elapsed_time(t0);
    return retVal;
}

template<typename _F, typename... _T>
auto yz11_tm_call(int& elapsedTimeMs, _F& f, _T&&... vargs) -> RT_VOID(decltype(f(vargs...)))
{
    auto t0 = yz11_helper::std_clock::now();
    f(std::forward<_T>(vargs)...);
	elapsedTimeMs = yz11_helper::calc_elapsed_time(t0);
}

template<typename _F, typename... _T>
auto yz11_tm_call_with_logs(CXX11_SOURCE_LOCATION_PARAMS,
    _F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
//    std::cout << "TYPE: " << typeid(std::declval<RetType>()).name() << "\n";
//    RetType v; return v;
    return (RetType) yz11_helper::tm_call_with_logs<RetType>(3, NULL, CXX11_SOURCE_LOCATION_ARGS, f, fn, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto __yz11_tm_call(CXX11_SOURCE_LOCATION_PARAMS,
    _F f, const char *fn, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    int elapsedTimeMs;
    return (RetType) yz11_tm_call(elapsedTimeMs, f, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto yz11_tm_call_catch_exception(_F f, const char *fn,
    typename std::conditional<std::is_void<typename std::result_of<_F(_T...)>::type>::value, int,
        typename std::result_of<_F(_T...)>::type>::type exceptionRetVal,

//    typename std::result_of<_F(_T...)>::type exceptionRetVal,
    _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    auto c = __yz11_tm_call<_F,_T&&...>;
	return (RetType) yz11_helper::tm_call_catching_exception<RetType,decltype(c),_F,_T...>(-1, nullptr, CXX11_CURRENT_SOURCE_LOCATION,
        c, f, fn, exceptionRetVal, std::forward<_T>(vargs)...);
}

template<typename _F, typename... _T>
auto yz11_tm_call_catch_exception_with_logs(CXX11_SOURCE_LOCATION_PARAMS, _F f, const char *fn,
    typename std::conditional<std::is_void<typename std::result_of<_F(_T...)>::type>::value, int,
        typename std::result_of<_F(_T...)>::type>::type exceptionRetVal, _T&&... vargs) -> decltype(f(vargs...))
{
    typedef decltype(f(vargs...)) RetType;
    auto c = yz11_tm_call_with_logs<_F,_T&&...>;
	return (RetType) yz11_helper::tm_call_catching_exception<RetType,decltype(c),_F,_T...>(XLOG_LEVEL_DEBUG, "*TM*", CXX11_SOURCE_LOCATION_ARGS,
        c, f, fn, exceptionRetVal, std::forward<_T>(vargs)...);
}

/* These macros make the APIs easier to use */
#define YZ11_TM_CALL(elapsedTimeMs, func, ...) \
    yz11_tm_call(elapsedTimeMs, func, ##__VA_ARGS__)
#define YZ11_TM_CALL_WITH_LOGS(func, ...) \
    yz11_tm_call_with_logs(CXX11_CURRENT_SOURCE_LOCATION, CXX11_FN_AND_NAME(func), ##__VA_ARGS__)
#define YZ11_TM_CALL_CATCHING_EXCEPTION(func, exceptionalValue, ...) \
    yz11_tm_call_catch_exception(CXX11_FN_AND_NAME(func), exceptionalValue, ##__VA_ARGS__)
#define YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(func, exceptionalValue, ...) \
    yz11_tm_call_catch_exception_with_logs(CXX11_CURRENT_SOURCE_LOCATION, CXX11_FN_AND_NAME(func), exceptionalValue, ##__VA_ARGS__)

#undef RT_TYPE
#undef RT_VOID

#endif
