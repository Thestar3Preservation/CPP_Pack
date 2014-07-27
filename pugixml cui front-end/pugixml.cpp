#include <string>
#include <iostream>
#include "pugixml.hpp"
#include <getopt.h>
#include <pcrecpp.h>

int GV_ProgramArgc;
char **GV_ProgramArgv;
char *GV_ProgramName;
std::string GV_XpathExpression;
std::string GV_XhtmlFilePath;
int GV_ExitCode = -1;

static int GV_Option_Help = false;
static bool GV_Option_Silent = false;

void PrintHelp(){
	using namespace std;
	cout <<"사용법 : "<<GV_ProgramName<<" [xpath 표현식] [xhtml 파일]"<<endl
		 <<"  또는 : "<<GV_ProgramName<<" [xpath 표현식]"<<endl
		 <<"첫번째 형식은 xhtml 형식의 파일을 xpath 표현식을 따라 가공하여 표준출력으로 보냅니다."<<endl
		 <<"두번째 형식은 표준입력으로 부터 정보를 읽어온뒤, xpath 표현식을 따라 가공하여 표준출력으로 보냅니다."<<endl
		 <<endl
		 <<"옵션 :"<<endl
		 <<"  -s, --silent   오류 메시지를 출력하지 않습니다."<<endl
		 <<"      --help     도움말을 출력하고 종료합니다."<<endl
		 ;
}

bool ParseOptionToArgument(){
	int OptParam, OptionIndex = 0;
	static struct option LongOptions[] = {
		{"help",		no_argument,	&GV_Option_Help, 	true},
		{"silent",		no_argument,	0,					's'},
		{0,				0,				0,					0}
	};
	while( -1 != ( OptParam = getopt_long(GV_ProgramArgc, GV_ProgramArgv, "s", LongOptions, &OptionIndex) ) ) {
		switch( OptParam ) {
			case 's':
				GV_Option_Silent = true;
				break;
			case '?':
				std::cerr <<"오류 : 잘못된 옵션"<<std::endl;
				GV_ExitCode = 1;
				return false;
		}
	}
	return true;
}

bool SetGlobalOption(){
	if ( GV_Option_Help ) {
		PrintHelp();
		GV_ExitCode = 0;
		return false;
	}
	
	return true;
}

bool InitializeToArgument(int argc, char *argv[]){
	GV_ProgramArgc = argc;
	GV_ProgramArgv = argv;
	GV_ProgramName = GV_ProgramArgv[0];
	
	if ( !ParseOptionToArgument() ) {
		return false;
	}
	
	if ( GV_ProgramArgc == 1 ) {
		GV_Option_Help = true;
	}
	
	if ( !SetGlobalOption() ) {
		return false;
	}
	
	int RealArgumentCount = GV_ProgramArgc - optind;
	switch ( RealArgumentCount ) {
		case 1:
			GV_XpathExpression = GV_ProgramArgv[optind];
			GV_XhtmlFilePath = "/dev/stdin";
			break;
		case 2:
			GV_XpathExpression = GV_ProgramArgv[optind];
			GV_XhtmlFilePath = GV_ProgramArgv[ optind + 1 ];
			break;
		default:
			std::cerr <<"인자의 수가 부적절합니다."<<std::endl;
			GV_ExitCode = 1;
			return false;
	}
	
	return true;
}

bool CheckXpathSelectAttribute(std::string XpathExpression){
	//표현식에서 이스케이프된 내용들을 제거
	pcrecpp::RE("\\\\\"", PCRE_UTF8).GlobalReplace("", &XpathExpression);
	pcrecpp::RE("\\\\'", PCRE_UTF8).GlobalReplace("", &XpathExpression);
	pcrecpp::RE("\"[^\"]*\"", PCRE_UTF8).GlobalReplace("", &XpathExpression);
	pcrecpp::RE("'[^']*'", PCRE_UTF8).GlobalReplace("", &XpathExpression);
	return pcrecpp::RE("^.*/@[^/]*$", PCRE_UTF8).PartialMatch(XpathExpression);
}

pugi::xml_parse_result LoadContent(pugi::xml_document &FileContext, std::string FilePath){
	pugi::xml_parse_result Result;
	if ( FilePath == "/dev/stdin" ) {
		Result = FileContext.load(std::cin);
	} else {
		Result = FileContext.load_file(FilePath.c_str());
	}
	return Result;
}

bool PrintXhtmlFollowXpath(std::string XpathExpression, std::string FilePath){
	//xhtml을 파싱하기 위해 FilePath의 파일을 읽어옴.
	pugi::xml_document FileContext;
	pugi::xml_parse_result Result = LoadContent(FileContext, FilePath);
	
	//읽어왔을때 오류가 존재하면
	if ( Result.status != pugi::status_ok && Result.status != pugi::status_end_element_mismatch ) {
		if ( ! GV_Option_Silent )
			std::cerr <<"Error description: "<<Result.description()<<std::endl;
		GV_ExitCode = 2;
		return false;
	}
	
	//xpath가 속성을 지정하였는가?
	if ( CheckXpathSelectAttribute(XpathExpression) ) { 
		//속성이름을 얻음
		std::string AttributeName;
		pcrecpp::RE("^.*/@([^/]*)$", PCRE_UTF8).PartialMatch(XpathExpression, &AttributeName);
		
		//속성이름 앞의 표현식을 얻음
		std::string PreXpath = XpathExpression; 
		pcrecpp::RE("/@([^/]*)$", PCRE_UTF8).Replace("", &PreXpath); 
		
		//xpath 표현식대로 문서를 파싱
		pugi::xpath_node_set Tools = FileContext.select_nodes(PreXpath.c_str());
		
		//만약 결과가 존재하지 않는다면 종료
		if ( Tools.empty() ) {
			GV_ExitCode = 3;
			return false;
		}
		
		//존재한다면 찾아낸 노드에 존재하는 속성들을 출력
		for (pugi::xpath_node_set::const_iterator it = Tools.begin(); it != Tools.end(); ++it)
			std::cout<<it->node().attribute(AttributeName.c_str()).value()<<std::endl;
	} else {
		//xpath 표현식대로 문서를 파싱
		pugi::xpath_node_set Tools = FileContext.select_nodes(XpathExpression.c_str());
		
		//만약 결과가 존재하지 않는다면 종료
		if ( Tools.empty() ) {
			GV_ExitCode = 3;
			return false;
		}
		
		//존재한다면 찾아낸 노드의 구조를 그대로 출력
		for (pugi::xpath_node_set::const_iterator it = Tools.begin(); it != Tools.end(); ++it)
			it->node().print(std::cout);
	}
	
	return true;
}

int main(int argc, char *argv[]) {
	if ( !InitializeToArgument(argc, argv) ) {
		return GV_ExitCode;
	}

	if ( !PrintXhtmlFollowXpath(GV_XpathExpression, GV_XhtmlFilePath) ) {
		return GV_ExitCode;
	}
	
	return (GV_ExitCode == -1) ? 0 : GV_ExitCode;
}
