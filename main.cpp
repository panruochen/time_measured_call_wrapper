#include <string>
#include <memory>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void logPrint(int level, const char *tag, const char *fileName, const char *funcName, int lineNum,
	const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	printf("LVL%d %12s :: %s:%d :: ", level, tag, funcName, lineNum);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
}

#include "cxx_tm_call.hpp"

#define EFAIL   1
//#define ENOMEM  10

struct ComplexReturnCodeWrapper {
    float floatValue;
    int code;
	inline operator int () { return code; }
	inline ComplexReturnCodeWrapper(const int v) { code = v; }
	inline ComplexReturnCodeWrapper(const void *ptr) { code = ptr != nullptr ? 0 : -ENOMEM; }
	inline ComplexReturnCodeWrapper(bool v) { code = v ? 0 : -EFAIL; }
	inline ComplexReturnCodeWrapper() { code = -EFAIL; }
};

struct Struct1 {
    int a = 1;
    long b = 3;
    double c = 5;
    char data1[1024];
    std::string s;
};

ComplexReturnCodeWrapper readData(void *&data, const std::string& filename)
{
	data = nullptr;
    FILE *fp = fopen(filename.c_str(), "rb");
    ComplexReturnCodeWrapper ret;
    while(fp != NULL) {
    	if(fseek(fp, 0, SEEK_END) == -1) {
    		ret = ComplexReturnCodeWrapper(errno);
    		break;
    	}
    	
    	int size = ftell(fp);
		if(fseek(fp, 0, SEEK_SET) == -1) {
			ret = ComplexReturnCodeWrapper(errno);
			break;
		}
		
		data = malloc(size);
		if(fread(data, 1, size, fp) != size) {
			free(data);
			data = nullptr;
			ret = ComplexReturnCodeWrapper(errno);
			break;
		}

        ret = ComplexReturnCodeWrapper(0);
		break;
    }

    if(fp != nullptr)
		fclose(fp);

    static int msCount = 2;
    std::chrono::milliseconds ms(msCount);
    if(++msCount >= 32)
        msCount = 2;
    std::this_thread::sleep_for(ms);

	return ret;
}

void tmpString(std::string s) {
    std::string old = s;
    s = "True";
    std::cout << "Replace \"" << old << "\" to \"" << s << "\"\n";
}

void newString(std::string& d, const char *s) {
    d = s;
}

bool isOddV1(int v) {
    if(v % 2 == 0) {
        if(v < 0)
            throw v;
        return false;
    }
    return true;
}

bool isOddV2(int v) {
    if(v % 2 == 0) {
        if(v < 0) {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Num-%d", v);
            throw std::string(tmp);
        }
        return false;
    }
    return true;
}

void checkDigitalString(const std::string& s, int base = 10) {
    int upper = 10;
    if(base != 10 && base != 2 && base != 8 && base != 16) {
        char tmp[64];
        std::string err = "Invalid base ";
        snprintf(tmp, sizeof(tmp), "%d", base);
        err += tmp;
        return;
    }

    for(auto c : s) {
        if(base == 16 && (!(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F')))
            throw s;
        if(!(c >= '0' && c < '0' + base))
            throw s;
    }
}

Struct1 getStr1(int v) {
    Struct1 ret;
    ret.a = v;
    snprintf(ret.data1, sizeof(ret.data1), "%d", v);
    ret.s = ret.data1;

    static int msCount = 1;
    std::chrono::milliseconds ms(msCount);
    if(++msCount >= 32)
        msCount = 2;
    std::this_thread::sleep_for(ms);

    return ret;
}

int add(const std::string& s1, const std::string& s2, int& result) {
    for(auto c : s1)
        if(!(c >= '0' && c <= '9'))
            throw s1;
    for(auto c : s2)
        if(!(c >= '0' && c <= '9'))
            throw s2; 
    result = atoi(s1.c_str()) + atoi(s2.c_str());
    return 0;
}

int main() {
    int elapsedTime, len, iRet, v1;
    std::string s1;
    void *buf1;
    bool bRet;

    yz11_tm_call(elapsedTime, tmpString, s1);
    std::cout << "Line " << __LINE__ << " : " << s1 << "\n";

    YZ11_TM_CALL(elapsedTime, newString, s1, "StringS1");
    std::cout << "Line " << __LINE__ << " : " << s1 << "\n";

    len = YZ11_TM_CALL(elapsedTime, strlen, "Hello world");
    std::cout << "Line " << __LINE__ << " : " << len << "\n";
    len = YZ11_TM_CALL_WITH_LOGS(strlen, "Hello world");
    std::cout << "Line " << __LINE__ << " : " << len << "\n";

    elapsedTime = -1;
    YZ11_TM_CALL(elapsedTime, readData, buf1, "1.dat");
    std::cout << "Line " << __LINE__ << " : elapsed time " << elapsedTime << "ms\n";
    if(buf1 != nullptr) {
        free(buf1);
        buf1 = nullptr;
    }

    YZ11_TM_CALL_WITH_LOGS(readData, buf1, "1.dat");
    if(buf1 != nullptr) {
        free(buf1);
        buf1 = nullptr;
    }

    YZ11_TM_CALL_WITH_LOGS(readData, buf1, "2.dat");
    if(buf1 != nullptr) {
        free(buf1);
        buf1 = nullptr;
    }

    bRet = YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(isOddV1, false, 2);
    bRet = YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(isOddV2, false, -2);

    iRet = YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(add, -7009, "a", "1", v1);
    std::cout << "Line " << __LINE__ << " : Ret " << iRet << "\n";

    iRet = YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(add, -7009, "1234", "7890", v1);
    std::cout << "Line " << __LINE__ << " : Ret " << iRet << "\n";

    {
        auto ret = YZ11_TM_CALL_WITH_LOGS(getStr1, 98765);
        std::cout << "Line " << __LINE__ << " : Ret " << ret.s.c_str() << "\n";
    }

    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234", 2);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234", 8);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234", 10);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234", 16);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234", 7);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234ab", 16);
    YZ11_TM_CALL_CATCHING_EXCEPTION_WITH_LOGS(checkDigitalString, 0, "1234g", 16);

    return 0;
}
