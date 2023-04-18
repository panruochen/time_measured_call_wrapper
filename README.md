TIME MEASURED CALL WRAPPER
=========================

***time_measure_call_wrapper*** is a c++11 project demostrating a trick to wrap any function by counting the spent time and printing the return values.<br>
***此工程***演示了如何使用C++11模板元编程来封装任何函数调用，统计函数的调用时间并打印返回值。

BUILD （编译）
============
```
# lauch vs2015 x86|x64 Native Tool command prompt （启动vs2015 x86或者x64命令行提示符）
mkdir build
cd build
cmake ../
make or nmake
```

You should have g++ (5.4 or higher), clang or msvc2015 (or higher) installed.<br>
系统中应当安装了下列编译器之一：g++ （5.4或更高版本），clang-5.0，msvc2015 （或更高版本）

FUNCTION DESCRIPTIONS （函数功能描述）
==================================
### RetType YZ11_TM_CALL(int& eplasedTimeMs, func, ...)
Invoke a call to **func** with arguments, returning the time in milliseconds spent by the function call. Fox example:<br>
调用一个函数并返回其执行时间。例如：
```
int foo(int a, double b, const char *c) { ... }
//...
int elapsedTimeMs;
auto ret = YZ11_TM_CALL(eplasedTimeMs, foo, 1, 2.7, "3.00");
```

### RetType YZ11_TM_CALL_WITH_LOGS(func, ...)
Invoke a call to **func** with arguments, printing logs of time in milliseconds spent by the function call as well as 
the return value if it is an interger or ComplexReturnCodeWrapper type . Fox example:<br>
调用一个函数并打印其执行时间以及返回值（如果返回值为int或者ComplexReturnCodeWrapper类型）。例如：
```
int foo(int a, double b, const char *c) { ... }
auto ret = YZ11_TM_CALL_WITH_LOGS(foo, 1, 2.7, "3.00");
```

The function logPrint with the following prototype is required to print logs:<br>
logPrint函数的其原型如下：<br>
_void logPrint(int level, const char *tag, const char *fileName, const char *funcName, int lineNum, const char *fmt, ...)_

You can implement your own *logPrint* function.
你实现自己的logPrint函数。

### RetType YZ11_TM_CALL_CATCHING_EXCEPTION(func, exceptinalValue, ...)
Invoke a call to **func** with arguments, catching exceptions if any occurs. *exceptionalValue* is the value to returned
by the function while any exception occurs. If the return type is **void**, specify an integer for *exceptionalValue*.
Fox example:<br>
调用一个函数并捕捉异常。*exceptionalValue*用于指定异常发生时的返回值。如果返回值为void类型，需要指定一个整数作为占位符。例如：
```
int add(std::string& a, int b) {
	for(auto c : s) {
		if(!(c >= '0' && c <= '9')) {
			std::string s = "Invalid number ";
			s += a;
			throw s;
		}
	}
	return atoi(s.c_str()) + b;
}

auto ret = YZ11_TM_CALL_CATCHING_EXCEPTION(add, 0x80000000, "X1", 9876);
```

### RetType YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(func, exceptionalValue, ...)
Invoke a call to **func** with arguments, printing logs and catching exceptions. Fox example:<br>
```
int add(std::string& a, int b) {
	for(auto c : s) {
		if(!(c >= '0' && c <= '9')) {
			std::string s = "Invalid number ";
			s += a;
			throw s;
		}
	}
	return atoi(s.c_str()) + b;
}

auto ret = YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(add, 0x80000000, "X1", 9876);
```

About Me
========
Pan Ruochen <ijkxyz@msn.com>
