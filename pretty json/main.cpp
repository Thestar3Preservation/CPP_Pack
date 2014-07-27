#include <string>
#include <iostream>
#include <getopt.h>
#include <dlfcn.h>
#include "UserFunctionPack.hpp"

#include "pretty_json_builtin.hpp"

static void ShowHelp(char*);
static int ParseOption(int, char*[]);
int main(int, char*[]);

enum { BUILTIN, BETA };
static int GV_Type = BUILTIN;

void ShowHelp(
	char *PrgramName
){
	using namespace std;
	cout <<"사용법 : "<<PrgramName<<" [JSON 표현식]"<<endl
		 <<"  또는 : "<<PrgramName<<" < 표준입력"<<endl
		 <<"  또는 : "<<PrgramName<<" clip"<<endl
		 <<"  JSON 표현식을 예쁘게 출력합니다."<<endl
		 <<"  만약, clip을 인자로 준다면 클립보드로 부터 json 표현식을 읽어옵니다."<<endl
		 <<endl
		 <<"  주의 : "<<endl
		 <<"   builtin 타입이던, beta타입이던 불분명한 json표현식은 정렬하지 못합니다."<<endl
		 <<endl
		 <<"  옵션 : "<<endl
		 <<"       --type [type]   내장을 선택하여 해당 방식대로 출력합니다. 이 옵션이 생략"<<endl
		 <<"                       되면 기본값으로 'builtin' 방식을 사용합니다."<<endl
		 <<"                       'builtin' 방식의 경우, pretty_json라이브러리의 것을 그대"<<endl
		 <<"                       로 사용합니다."<<endl
		 <<"                       'beta' 방식의 경우, 사용자가 작성한 코드로 JSON 표현식을"<<endl
		 <<"                       해석합니다. 이 방법은 누락되는 데이터가 존재하게 될수 있"<<endl
		 <<"                       습니다."<<endl
		 <<endl
		 <<"       --              이 이후의 인자는 옵션으로 해석하지 않습니다."<<endl
		 <<endl
		 <<"   -h, --help          도움말을 표시하고 끝냅니다."<<endl
		 <<endl
	;
}

/**프로그램 인자 목록으로 부터 옵션을 파싱합니다.
 * @return -1 : 프로그램 종료. 그 외의 값은 남은 인자의 수입니다.
 */
int ParseOption(
	int Argc,		///< 프로그램 인자의 수
	char *Argv[]	///< 프로그램 인자 목록
){
	//옵션처리
	int param_opt, option_index = 0;
	static struct option long_options[] =
	{
		{"type",	required_argument,	0,		0},
		{"help",	no_argument,		0,		'h'},
		{0,			0,					0,		0}
	};
	while( -1 != ( param_opt = getopt_long(Argc, Argv, "h", long_options, &option_index) ) ) {
		switch(param_opt) {
			case 0:
				if ( option_index == 0 ) {
					std::string s = optarg;
					if ( s == "builtin" ) {
						GV_Type = BUILTIN;
					}
					else if ( s == "beta" ) {
						GV_Type = BETA;
					}
					else {
						std::cerr <<"정의되지 않은 타입입니다!"<<std::endl;
						return -1;
					}
				}
				break;
			case 'h':
				ShowHelp(Argv[0]);
				break;
			case '?':
				return 1;
		}
	}
	
	return Argc - optind;
}

/**프로그램의 시작 부분
 */
int main(
	int Argc,		///< 프로그램 인자의 수
	char *Argv[]	///< 프로그램 인자 목록
) {
	std::string JsonStr;
	
	int RealArgc = ParseOption(Argc, Argv);
	if ( RealArgc == -1 ) {
		return 1;
	}
	
	if ( RealArgc == 0 ) {
		//표준입력으로 부터 값을 읽어옴.
		if ( ! ufp::readstdin(JsonStr) ){
			ShowHelp(Argv[0]);
			return 0;
		}
	}
	else if ( RealArgc > 1 ) {
		std::cerr <<"인자의 수가 부적절 합니다."<<std::endl;
		return 1;
	}
	else {
		if ( std::string("clip") == Argv[optind] ) {
			ufp::ClipOut(JsonStr);
		}
		else {
			JsonStr = Argv[optind];
		}
	}
	
	if ( GV_Type == BUILTIN ) {
		PrintPrettyJsonBuiltin(JsonStr);
	}
	else if ( GV_Type == BETA ) {
		std::string LibPath;
		if ( ufp::GetEnv("PATH_SOURCECODE", LibPath) ) {
			LibPath += "/";
		} 
		else {
			LibPath = "./";
		}
		LibPath += "pretty_json.so";
		void *lib_handle = dlopen(LibPath.c_str(), RTLD_LAZY);
		if(lib_handle == NULL){
			std::cerr <<"라이브러리를 로딩하는데 실패했습니다."<<std::endl
					  <<dlerror() << std::endl;
			return 2;
		}
		typedef void (*funct)(const char*);
		funct func = (funct)dlsym(lib_handle, "PrintJsonPretty");
		(*func)(JsonStr.c_str());
		dlclose(lib_handle); 
	}
	
	return 0;
}