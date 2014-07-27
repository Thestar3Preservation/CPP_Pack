#include "UserFunctionPack.hpp"

int main(int argc, char *argv[]) {
	std::string StrUtf8 = "안녕하세요."; //hello world! 문자열 인코딩 변환을 테스트 합니다.\n";
	std::cout <<"원본 : "<<StrUtf8<<std::endl;
	
	std::string Encoding = "UTF-32";
	
	std::wstring WstrUtf32 = ufp::iconv().SetTo(Encoding).SetSrc(StrUtf8).GetWstr();
	std::cout <<"WstrUtf32 Size : "<<WstrUtf32.size()<<std::endl;
	
	for(int i = 0; i < WstrUtf32.size(); i++) {
		std::string StrTemp = ufp::iconv().SetFrom(Encoding).SetSrc(WstrUtf32[i]).GetStr();
		std::cout <<i<<"["<<StrTemp.size()<<"] : "<<StrTemp<<std::endl;
	}
	std::cout <<"========================================================================"<<std::endl;
	std::string StrUtf8_2 = ufp::iconv().SetFrom(Encoding).SetSrc(WstrUtf32).GetStr();
	std::cout <<"StrUtf8_2 SIZE : "<<StrUtf8_2.size()<<" / "<<StrUtf8_2<<std::endl;
// 	for(int i=0; i < StrTemp.size(); i++) {
// 		std::cout<<i<<" : "<<StrTemp[i]<<std::endl;
// 	}
	
	std::wstring WstrUtf8 = ufp::iconv().SetSrc(StrUtf8).GetWstr();
	std::string StrTemp = ufp::iconv().SetSrc(WstrUtf8).GetStr();
	std::cout <<StrTemp<<std::endl;
}