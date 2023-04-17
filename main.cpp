#include <string>
#include <memory>
#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void zLog(int level, const char *tag, const char *fileName, const char *funcName, int lineNum,
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

struct StringStatusCode {
    std::string s;
    void set(bool v) { s = v ? "OKAY" : "FAILED"; }
    void set(int v) { s = v == 0 ? "OKAY" : "FAILED"; }
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
	return ret;
}

void tmpString(std::string s) {
    std::string old = s;
    s = "True";
    std::cout << "Replace \"" << old << "\" to \"" << s << "\"\n";
}

int main() {
    int elapsedTime, retInt, len;
    std::string s1;

    yz11_tm_call(elapsedTime, tmpString, s1);
    std::cout << "Line " << __LINE__ << " : " << s1 << "\n";

    len = yz11_tm_call(elapsedTime, strlen, "Hello world");
    std::cout << "Line " << __LINE__ << " : " << len << "\n";
    len = yz11_tm_call_zlog(CXX11_CURRENT_SOURCE_LOCATION, CXX11_FN_AND_NAME(strlen), "Hello world");
    std::cout << "Line " << __LINE__ << " : " << len << "\n";

#if 0
    m1 = yz11_tm_call(elapsedTime, cv::imread, imFileName, cv::IMREAD_UNCHANGED);
    printf("Used %dMS, M %dx%d\n", elapsedTime, m1.cols, m1.rows);
    yz11_tm_call(elapsedTime, cv::imencode, ".jpg",
        m1, outBuf, std::vector<int>(cv::IMWRITE_JPEG_QUALITY, 90));
    printf("Used %dMS\n", elapsedTime);

    m1 = yz11_algo_call_catch_exception(
        CXX11_FN_AND_NAME(cv::imread), imFileName, cv::IMREAD_COLOR);
    checkMatEmpty(m1, __LINE__);
    m1 = yz11_algo_call_catch_exception_zlog(CXX11_CURRENT_SOURCE_LOCATION,
        CXX11_FN_AND_NAME(cv::imread), imFileName, cv::IMREAD_COLOR); checkMatEmpty(m1, __LINE__);

    yz11_tm_call_nozlog(CXX11_CURRENT_SOURCE_LOCATION,
        CXX11_FN_AND_NAME(cv4_imread), m1, imFileName, cv::IMREAD_COLOR); checkMatEmpty(m1, __LINE__);
    yz11_tm_call_zlog(CXX11_CURRENT_SOURCE_LOCATION,
        CXX11_FN_AND_NAME(cv4_imread), m1, imFileName, cv::IMREAD_COLOR); checkMatEmpty(m1, __LINE__);

    yz11_tm_call_zlog(CXX11_CURRENT_SOURCE_LOCATION,
        CXX11_FN_AND_NAME(cv3_imread), m1, imFileName, cv::IMREAD_COLOR);
#endif

#if 0
#define MULTI_CALLS(func, ...) \
    yz11_tm_call(elapsedTime, func, ##__VA_ARGS__); \
    printf("%s:%d: %s() spent %dMS\n", __func__, __LINE__, #func, elapsedTime); \
    YZ11_TM_CALL_ZLOG(func, ##__VA_ARGS__); \
    YZ11_ALGO_CALL_CATCH_EXCEPTION(func, ##__VA_ARGS__); \
    YZ11_ALGO_CALL_CATCH_EXCEPTION_ZLOG(func, ##__VA_ARGS__);

    YZ11_ALGO_CALL_CATCH_EXCEPTION(cv::imread, imFileName, cv::IMREAD_COLOR);

    MULTI_CALLS(cv::imread, imFileName, cv::IMREAD_COLOR);
    MULTI_CALLS(cv3_imread, m1, imFileName, cv::IMREAD_COLOR); checkMatEmpty(m1, __LINE__);
    MULTI_CALLS(cv4_imread, m1, imFileName, cv::IMREAD_COLOR); checkMatEmpty(m1, __LINE__, false);
    printf("Mat %dx%d\n", m1.cols, m1.rows);
    MULTI_CALLS(cv::imencode, ".jpg", m1, outBuf, std::vector<int>(cv::IMWRITE_JPEG_QUALITY,90));

    retInt = YZ11_ALGO_CALL_CATCH_EXCEPTION_ZLOG(checkMat, m1);
    printf("ret %d\n", retInt);
    m1.release();
    retInt = YZ11_ALGO_CALL_CATCH_EXCEPTION_ZLOG(checkMat, m1);
    printf("ret %d\n", retInt);
#endif

    return 0;
}

