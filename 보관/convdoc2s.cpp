#include "UserFunctionPack.hpp"
#include <boost/filesystem.hpp>
// #include <copyfile.h>
//#include <boost/filesystem.hpp>
//#include <list>

//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/predicate.hpp>

// #include <glib.h>
// #include <unistd.h>
// #include <libnotify/notify.h>

//#include <boost/system/error_code.hpp>


using namespace boost;

char *name;
int error;
string logSAVE, logSAVE2, home;

void help(){
	using namespace std;
	cout <<"사용법 : "<<name<<" [옵션]... [대상 디렉토리]"<<endl
		 <<"대상 디렉토리 이하 모든 문서를 지정된 규칙에 따라 검색 가능한 문서로 변환합니다"<<endl
		 <<". 검색 불가능한 문서는 link폴더에 링크시켜 두고, 오류가 발생하며 오류 내역을 사"<<endl
		 <<"용자에게 보고합니다."<<endl
		 <<endl
		 <<"  -d, --download-video     유투브 동영상 태그를 발견하면, 동영상을 다운로드하여"<<endl
		 <<"                           내장 동영상으로 변환합니다(지원 확장자 : maff). Fire"<<endl
		 <<"                           fox의 모질라 압축 포멧 확장기능이 유투브 동영상을 다"<<endl
		 <<"                           운로드 하지 못하는 것을 보완합니다(일부 동영상은 누"<<endl
		 <<"                           락될수 있습니다. 사용후 전후 확인을 해주세요)."<<endl
		 <<"  -n, --edit-can-drag      작업하는 maff파일의 html파일에 존재하는 드래그 방지 "<<endl
		 <<"                           태그를 제거합니다."<<endl
		 <<"  -g, --report-gui         작업 진행과정과 결과를 GUI로 표현합니다."<<endl
		 <<"  -b, --no-bakup           원본 파일을 백업하지 않습니다."<<endl
		 <<"  -c, --trim-filename      확장자를 예쁘게 수정합니다. ex) .html.maff -> .maff"<<endl
		 <<"  -l, --no-link            링크 작업을 수행하지 않게 합니다."<<endl
		 <<"  -p, --no-report          작업 결과와 오류 내역을 보고하지 않습니다."<<endl
		 <<"  --                       옵션의 끝을 나타냅니다. 이 뒤의 인자는 옵션으로 해석"<<endl
		 <<"                           하지 않습니다."<<endl
		 <<"  -h, --help               이 도움말을 표시하고 끝냅니다."<<endl
		 <<"  -v, --version            버전 정보를 출력하고 끝냅니다."<<endl
		 <<endl
		 <<"bash로 작성된 convdoc2s 스크립트의 C++버전입니다. c++0x 규격으로 작성되었습니다."<<endl
	;
	exit(0);
}

void version(){
	using namespace std;
	cout <<"<<업데이트 내역>>"<<endl
		 <<endl
		 <<"3.3 : bash 스크립트로 작성된 코드를 c++로 재작성"<<endl
		 <<"3.2 : html문서의 charset 속성값을 수정하는 코드가 부적절한 범위로 charset을 찾아내는 문제를 해결."<<endl
		 <<"3.1 : html문서의 charset 속성값을 수정하는 코드를 MAFF파일에 대해 작업할때와, 그렇지 않을 경우를 나누어 작업하도록 수정하여, 작업 속도를 향상시킴."<<endl
		 <<"3.0 : html문서에 charset이 존재하지 않는 경우 charset을 표현하는 태그를 삽입하도록 수정."<<endl
		 <<"2.9 : link폴더에 문서가 복사되지 않던 문제해결."<<endl
		 <<"진행표시바를 올바르게 표현하도록 수정함."<<endl
		 <<"진행표시바가 작업 완료후 닫기지 않던 문제 해결."<<endl
		 <<"2.8 : 로그 파일이 저장되는 위치를 미리 설정하도록 변경함."<<endl
		 <<"로그를 생성하는 형식을 간결하게 수정."<<endl
		 <<"2.7 : 비디오를 video태그가 아닌 embed태그로 mplayer로 재생하도록 함."<<endl
		 <<"2.6 : avi, hidden을 검색제외 확장자에 추가."<<endl
		 <<"flash로된 유투브 동영상도 다운로드 가능하도록 수정."<<endl
		 <<"2.5 : 드래그 방지 코드 무력화 기능 추가."<<endl
		 <<"2.4 : 코드를 블럭화."<<endl
		 <<"동영상 처리 옵션이 켜진상태에서 동영상이 포함된 maff파일의 index.html파일이 날라가던 문제 해결."<<endl
		 <<"2.3 : 동영상 다운로드 기능을 maff 확장자에만 한정함. Firefox에서 다운로드되지 않고, webarchive에서 다운로드된 경우에는 이미 동영상 저장 기능이 포함되어 있기 떄문. 또한, Firefox의 모질라 압축 포멧 확장 프로그램은 wget으로 실제 html소스를 다운로드 한것과 내용이 다름. Firefox에서 현재 표현중인 태그와 이미지등으로 구성되어 나타나기 때문에 유투브 태그 형식이 원본과 다름. 즉, webarchive에서 다운받은 것과 Firefox에서 다운받은 maff파일은 서로 다른 방식으로 처리해야 함. 그런 이유로 webarchive에서는 처리하지 않고, 오직 Firefox에서 다운받은 것만 처리하도록 함."<<endl
		 <<"2.2 : youtube 동영상 태그가 발견되면, 해당 영상을 다운로드하여 내장 파일로 만드는 기능을 추가."<<endl
		 <<"2.1 : 병합 maff파일을 정상적으로 변환하지 못하던 문제 해결."<<endl
		 <<"2.0 : 비정상적인 maff파일이 존재할 경우, 작업진행 여부를 물음."<<endl
		 <<"1.9 : GUI로 진행현황과 결과를 알리는 옵션 추가."<<endl
		 <<"1.8 : 백업 기능을 비활성화시키는 옵션을 추가."<<endl
		 <<"1.7 : 확장자가 존재하지 않을 경우의 처리패턴 추가."<<endl
		 <<"1.6 : html문서의 charset이 수정되지 않던 문제 해결."<<endl
		 <<"1.5 : maff파일 변환안되던 문제 해결"<<endl
		 <<"1.4 : 확장자를 예쁘게 수정하는 옵션을 추가. .html.maff -> .maff 등으로..."<<endl
		 <<"1.3 : maff파일과 html파일을 변환할수 있게 함."<<endl
		 <<"1.2 : 작업 위치를 사용자가 설정할수 있도록 함."<<endl
		 <<"1.1 : 링크 작업과 보고서 보고를 하지 않게하는 옵션을 추가."<<endl
		 <<"1.0 : 버전, 도움말 페이지를 추가"<<endl
	;
	exit(0);
}

int L_mkdir(string code){
	int Lerror = mkdir(code.c_str(), DEFAULT_MODE);
	if ( Lerror != 0 )
		error += 1;
	return Lerror;
}

//확장자의 문서를 추후 변환할 목적으로 링크 디렉토리에 링크시킵니다. 그리고, 오류가 존재한다면, 오류 메세지를 따로 저장합니다.
void ln_doc(string filepath, string filefullname, string dir){
	if ( ! O_link )
		return;
	string path(home + "/link/" + dir + "/");
	int error = symlink(filepath.c_str(), string(path + dupev_name(filefullname, path)).c_str());
	if ( error != 0 ) {
		cerr <<"링크 생성 중 오류 "<<strerror(errno)<<"가 발생했습니다!"<<endl;
		if ( O_report ) {
			ofstream SaveFile(logSAVE);
			SaveFile <<"링크 생성 중 오류 "<<strerror(errno)<<"가 발생했습니다!"<<endl;
			SaveFile.close();
		}
	}
}

void cp_doc(string filepath, string filefullname, string dir){
	if ( ! O_link )
		return;
	string path(home + "/link/" + dir + "_f/");
	filesystem::path p1(path), p2(path + dupev_name(filefullname, path));
	system::error_code ec;
	filesystem::copy_file(p1, p2, copy_option::fail_if_exists, ec);
	if ( ec != 0 ){
		cerr <<"복사 작업에 문제가 생겼습니다!"<<endl;
		exit(1);
	}
// 		cp -v "$filepath" "$Path/$(dupev_name "$Path/$filefullname")" 2>> "$logSAVE"
}

//에러 메시지를 처리합니다. 인자를 주면, 그것이 에러 메시지가 됩니다.
void report_error(string filepath, string mesg){
	ofstream SaveFile(logSAVE2);
	SaveFile <<"작업 대상의 이름 : "<<filepath<<endl
			 <<"에러 메시지 : "<<mesg<<endl
			 <<endl;
	SaveFile.close();
	cerr <<"\e[31;01mERROR : "<<mesg<<"\e[39;49;00m"<<endl;
}

//문서를 주어진 인자의 형식으로 변환시킵니다. 인자2는 변환대상이 될 대상의 확장자를 의미합니다.
int convert_doc(string to, string from){
	int count = 0;
	bool success = false;
	string ext;
	while(true){
		if ( ++count > 3 )
			break;
		system("unoconv --listener &>/dev/null &");
		//unoconv는 해당 문서가 위치하는 곳에 변환된 파일을 위치시킵니다.
		if ( system(string("unoconv -f " + to + " -- " + s_quote(filename + "." + from)).c_str()) == 0 ) {
			success = true;
			break;
		}
	}
	if ( success ) {
		if ( O_bakup ) {
			system(string("trash-put -- " + s_quote(filename + "." + from)).c_str());
		} else {
			unlink(string(filename + "." + from).c_str());
		}
	} else {
		report_error("converting doc : " + from + "->" + to + "으로의 형식변환에 문제가 생겼습니다.");
		ext = from;
		ln_doc("doc");
		return 1;
	}
	return 0;
}

//문서의 인코딩을 UTF-8로 변경시킵니다. 첫번째 인자는 변환이 실패했을 경우, 링크되는 위치를 지정합니다. 만약 이미 UTF-8로 되어있다면, 종료코드 2를 리턴합니다. 해독이 불가능하다면 종료코드 1을 리턴하며, 변환되었을 경우 종료코드 1을 리턴합니다.
int convert_txt(string code){
	string encoding, data;
	fgetEncoding(filepath, encoding);
	if ( encoding.compare("UTF-8") ) {
		cout <<"이 문서는 이미 UTF-8로 인코딩되어 있습니다."<<endl;
		return 2;
	} else {
		if ( ! ( iconv("UTF-8", "UHC", ifstream(filepath), data) || iconv("UTF-8", encoding, ifstream(filepath), data) ) ) {
			report_error("convert_txt : 이 문서는 해독불가능한 문서입니다.");
			ln_doc(code);
			cp_doc(code);
			return 1;
		}
		cout <<"이 문서는 "<<encoding<<"에서 UTF-8로 변환되었습니다."<<endl;
		if ( backup_o ) {
			system(string("trash-put -- " + s_quote(filepath)).c_str());
		} else {
			unlink(filepath.c_str());
		}
		ofstream SaveFile(filepath);
		SaveFile <<data;
		SaveFile.close();
	}
	return 0;
}

int main(int argc, char *argv[]){
	//변수 선언 및 초기화
	name = argv[0];
	bool O_downloadvideo = false, O_editcandrag = false, O_reportgui = false, O_bakup = true, O_trimfilename = false, O_link = true, O_report = true;

	if( argc == 1 )
		help();

	//옵션처리
	int param_opt, option_index = 0;
	static struct option long_options[] =
	{
		{"download-video",	no_argument,	0, 	'd'},
		{"edit-can-drag",	no_argument,	0, 	'n'},
		{"report-gui",		no_argument,	0, 	'g'},
		{"no-bakup",		no_argument,	0, 	'b'},
		{"trim-filename",	no_argument,	0, 	'c'},
		{"no-link",			no_argument,	0, 	'l'},
		{"no-report",		no_argument,	0, 	'p'},
		{"help",			no_argument,	0,	'h'},
		{"version",			no_argument,	0,	'v'},
		{0,					0,				0,	0}
	};
	while( -1 != ( param_opt = getopt_long(argc, argv, "dngbclphv", long_options, &option_index) ) ) {
		switch(param_opt) {
			case 'd':
				O_downloadvideo = true;
				break;
			case 'n':
				O_editcandrag = true;
				break;
			case 'g':
				O_reportgui = true;
				break;
			case 'b':
				O_bakup = false;
				break;
			case 'c':
				O_trimfilename = true;
				break;
			case 'l':
				O_link = false;
				break;
			case 'p':
				O_report = false;
				break;
			case 'h':
				help();
				break;
			case 'v':
				version();
				break;
			case '?':
				return 1;
				break;
		}
	}

	//처리후에도 인자가 존재하는가? 그렇다면 작업시작. 없다면 표준입력을 체크
	if(optind < argc)
		home = realpath(argv[optind], NULL);
	else
		help();

	//위치를 home으로 변경
	if ( chdir(home.c_str()) != 0 ) {
		cerr <<"작업위치를 바꾸는데 실패했습니다!"<<endl;
		return 1;
	}
	int pid; //이곳에 이 프로세서의 PID를 담음.
	std::cout <<"작업위치 : "<<home<<std::endl;
	if ( boost::filesystem::exists("link") || boost::filesystem::exists("error.log") ) {
		std::cerr <<"link 폴더와 error.log파일은 이 폴더에 존재하지 않아야 합니다."<<endl;
		ufp::system("notify-send --urgency=critical 'link 폴더와 error.log파일은 이 폴더에 존재하지 않아야 합니다.'");
		return 1;
	}
	if ( O_link ) {
		//나중에 문서 파일들을 링크시켜 따로 처리할 것들을 저장할 공간을 생성합니다.
		error = 0;
		L_mkdir("link");
		L_mkdir("link/hwp");
		L_mkdir("link/hwp_f");
		L_mkdir("link/mht");
		L_mkdir("link/mht_f");
		L_mkdir("link/html");
		L_mkdir("link/html_f");
		L_mkdir("link/txt");
		L_mkdir("link/txt_f");
		L_mkdir("link/doc");
		L_mkdir("link/doc_f");
		if ( error > 0 ) {
			cerr <<"link 폴더 작성 중 오류가 생겼습니다!"<<endl;
			return 1;
		}
	}

	//만약, maff파일이 2kb이하이면, 그 파일은 저장이 완료되지 않은 비정상적인 파일이 가능성이 높습니다. 이 파일은 사전에 확인하여 처리하여 주세요.
	if ( O_report ) {
		//// 	if [ $report_o = y ] && mafflist=( $(find . -type f -size -2k -iname '*.maff') ) && [ -n "$(echo "$mafflist")" ]; then
		cout <<endl<<">>2k이하의 크기를 가진 maff문서 목록<<"<<endl;
		for (string i : mafflist)
			cout <<i<<endl;
		if ( O_reportgui ) {
			//zenity --list --title '문서 일괄 변환' --text '아래 문서들은 정상적으로 저장되지 않음이 의심되는 파일입니다.\n그래도 작업하시겠습니까?' --column '목록' "${mafflist[@]}" || exit
		} else {
			cout <<"위 문서들은 정상적으로 저장되지 않음이 의심되는 파일입니다."<<endl<<"그래도 작업하시겠습니까? (Y/N) : ";
			char reply = getchar(); //read -n 1 -r
			if ( reply = 'y' )
				cout <<"그대로 작업합니다."<<endl;
			else
				return 1;
		}
	}

	//임시공간을 생성하고, 변수에 등록함.
	string tmpdir("/tmp/");
	tmpdir += dupev_name("conver2mp", "/tmp", true);
	L_mkdir(tmpdir);

	//작업 진행 현황을 알림.
// 	filelist=$(find "$PWD" -type f)
	if ( O_reportgui ) {
		int count = 0;
// 		filelist_max=$(wc -l <<<"$filelist")
// 		echo 0 > /tmp/.progress-$ppid
		while(true) {
			usleep(50000);
// 			temp=$(</tmp/.progress-$ppid)
			if ( temp == "END" ) {
// 				echo 100
			} else {
				cal = ( temp / filelist_max ) * 100;
// 				cal=${cal%.*}
				if ( cal.empty() ) {
// 					echo 0
				} else if ( cal == "100" ) {
// 					echo 99
				} else {
// 					echo $cal
				}
			}
		}
// 		 | { if ! zenity --progress --auto-close --percentage=0 --title='일괄 문서 변환' --text='문서 변환 중...' --width 430; then
// 			pstree -p $ppid | grep -oP '(?<=\()[0-9]+(?=\))' | tac | xargs -I{} kill -9 {}
// 		#|| kill $$; } &
// 		fi; } &
	}

	//로그 파일이 저장되는 위치를 지정합니다.
	if ( O_report ) {
		logSAVE = home + "/link/error.log";
		logSAVE2 = home + "/error.log";
	}

	//지정된 디렉토리의 하위 모든 파일 마다
	for (string filepath : filelist) {
		//문서의 확장자를 정리합니다.
		string temp;
		if ( O_trimfilename && RE("^([^\0]+)\.+html?\.+maff$", PCRE_MULTILINE | PCRE_UTF8 | PCRE_CASELESS).PartialMatch(filepath, &temp) ) {
			string ch_filepath = dirname(filepath) + "/" + dupev_name(basename(temp), dirname(filepath));
			rename(filepath, ch_filepath);
			filepath = ch_filepath;
		}
		string filefullname;
		filefullname = basename(filepath);
		fileext = ex_ext(filepath);
		fileextLOW = ex_ext(filepath, true);
		filename = ex_name(filepath);
		cout <<endl<<"작업 대상의 이름 : "<<filepath<<endl; //작업 위치를 알림.
		//무시하는 경우
		if ( RE("exe|css|js|rdf|jpg|jpeg|png|gif|swf|desktop|pdf|cbz|sh|mp4|ico|avi|hidden|bmp|wma|odt", PCRE_UTF8 | PCRE_CASELESS).FullMatch(fileextLOW) == 1 ) {
			cout <<fileextLOW<<" 확장자는 처리 대상에서 제외되어 있습니다."<<endl;
		//docx를 doc으로 변환하고, doc을 odt로 변환시킴. 그 뒤, odt가 아닌 모든 파일을 제거.
		} else if ( fileextLOW.compare("docx") ) {
			convert_doc("doc", fileext) && convert_doc("odt", "doc");
		//doc파일을 odt로 변환시킴. 나머지 모두 삭제.
		} else if ( fileextLOW.compare("doc") ) {
			convert_doc("odt", fileext);
		//hwp만 파일만 남기고, 나머지는 삭제시킴. hwp파일은 링크시켜 따로 처리함.
		} else if ( fileextLOW.compare("hwp") ) {
			ln_doc("hwp");
			cp_doc("hwp");
		//mht파일만 남기고 링크시킴. maff로 일괄변환
		} else if ( fileextLOW.compare("mht") ) {
			ln_doc("mht");
			cp_doc("mht");
		//htm, html문서를 남기고, 인코딩을 변경시킨다. 나머지는 삭제처리.
		} else if ( RE("htm|html", PCRE_UTF8 | PCRE_CASELESS).FullMatch(fileextLOW) == 1 ) {
			//문서의 인코딩을 UTF-8로 변경
			int temp = convert_txt("html");

			if ( UPzip.compare("maff") ) {
				//html문서의 charset 속성값을 수정 : MAFF전용. 범용에 비해 작업이 간결함.
	// 			if check=$(grep -iP 'charset\s*=\s*[a-z_0-9-]+' <"$filepath") && grep -ivP 'charset\s*=\s*UTF-8' <<<"$check"; then
	// 				code=$(perl -0pe 'use encoding "utf8"; $/=undef; s/charset\s*=\s*[a-z_0-9-]+/charset=UTF-8/i' <"$filepath")
	// 				if [ $backup_o = y ]; then
	// 					trash-put -- "$filepath"
	// 				else
	// 					rm -- "$filepath"
	// 				fi
	// 				echo "$code" >"$filepath"
	// 			fi
	// 		else
	// 			#html문서의 charset 속성값을 수정 : 범용
	// 			if [ $temp != 1 ]; then
	// 				code=$(<"$filepath")
	// 				unset change
	// 				if temp=$(grep -Ei -m1 'charset= *"? *[a-z_0-9-]+' <<<"$code"); then
	// 					if grep -vFi 'charset= *"? *UTF-8' <<<"$temp"; then
	// 						change=y
	// 						code=$(perl -0pe 'use encoding "utf8"; s/charset= *("?) *[a-z_0-9-]+/charset=\1UTF-8/i' <<<"$code")
	// 					fi
	// 				else
	// 					change=y
	// 					if grep -iE -m1 '< *HEAD( +[^>]*)*>' <<<"$code"; then
	// 						code=$(perl -0pe 'use encoding "utf8"; s#(< *HEAD( +[^>]*)*>)#\1 <META charset="utf-8" />#i' <<<"$code")
	// 					elif grep -iE -m1 '< *HTML( +[^>]*)*>' <<<"$code"; then
	// 						code=$(perl -0pe 'use encoding "utf8"; s#(< *HTML( +[^>]*)*>)#\1 <META charset="utf-8" />#i' <<<"$code")
	// 					elif grep -iE -m1 '<! *DOCTYPE( +[^>]*)*>' <<<"$code"; then
	// 						code=$(perl -0pe 'use encoding "utf8"; s#(<! *DOCTYPE( +[^>]*)*>)#\1 <META charset="utf-8" />#i' <<<"$code")
	// 					else
	// 						code='<META charset="utf-8" />'$'\n'$code
	// 					fi
	// 				fi
	// 				if [ "$change" = y ]; then
	// 					if [ $backup_o = y ]; then
	// 						trash-put -- "$filepath"
	// 					else
	// 						rm -- "$filepath"
	// 					fi
	// 					echo "$code" > "$filepath"
	// 				fi
	// 			fi
			}
		//txt파일만 남기고, 인코딩을 모두 UTF-8로 변환한다.
		} else if ( fileextLOW.compare("txt") ) {
			convert_txt("txt");
		} else if ( fileextLOW.compare("cap") ) {
			convert_txt("txt");
			ufp::dupev_mv(filepath, ufp::dirname(filepath) + "/" + filename + ".txt");
		} else if ( RE("maff|webarchive", PCRE_UTF8 | PCRE_CASELESS).FullMatch(fileextLOW) == 1 ) {
			ufp::system("unzip -- " + ufp::s_quote(filepath) + " -d " + ufp::s_quote(tmp));
			if( O_bakup ) {
				TrashPut(filepath);
			} else {
				unlink(filepath.c_str());
			}
			if ( chdir(tmpdir.c_str()) != 0 ) {
				cerr <<"작업위치를 바꾸는데 실패했습니다!"<<endl;
				return 1;
			}
			DIR *dir_info = opendir(".");
			struct dirent *dir_entry;
			list<string> targetnames;
			if (NULL != dir_info) {
				while(dir_entry = readdir(dir_info)) {
					string temp(dir_entry->d_name);
					if ( is_directory(temp) || temp.compare(".") || temp.compare("..") )
						continue;
					targetnames.push_back(temp);
				}
			} else {
				cerr <<"화일 목록을 가져올수 없습니다!"<<endl;
			}
			for(string targetname : targetnames) {
				videoDown_o_temp = none;
				DragAssent_o_temp = none;
				op = nullptr;
				if ( DragAssent_o_temp != n ) {
					DragAssent_o_temp = SUB;
					op.push_back("-n");
				}
				if ( O_downloadvideo != n ) {
					videoDown_o_temp = SUB;
					op.push_back("-d");
				}
				if ( fileextLOW.compare("maff") ) {
					if ( is_regular_file(targetname/"index.rdf") ) {
						ifstream ReadFile(targetname/"index.rdf");
						bool ch = false;
						string temp;
						while(getline(ReadFile, temp)) {
							if ( RE("<MAF:charset RDF:resource=\"UTF-8\"/>", PCRE_UTF8 | PCRE_CASELESS).PartialMatch(temp) ){
								ch = true;
							}
						}
						ReadFile.close();
						if ( ch ) {
							ch = false;
							ifstream ReadFile(targetname/"index.rdf");
							while(getline(ReadFile, temp)) {
								if ( RE("(<MAF:charset RDF:resource=\")[^\"]+(\"/>)", PCRE_UTF8 | PCRE_CASELESS).GlobalReplace("\\1UTF-8\\2", &temp) ){
									ch = true;
								}
								code += temp;
								code += "\n";
							}
							if ( ch ) {
								ofstream SaveFile(targetname/"index.rdf");
								SaveFile <<code;
								SaveFile.close();
							}
							ReadFile.close();
						}
					}
					system("UPzip=" + fileextLOW + " DragAssent_o=" + DragAssent_o_temp + " videoDown_o=" + videoDown_o_temp + " " + s_quote(argv[0]) + " -b -p -l -- " + targetname);
				} else {
					string ops;
					for(string i : op)
						ops += i + " ";
					system("UPzip=" + fileextLOW + " " + s_quote(argv[0]) + " " + ops + " -b -p -l -- " + targetname);
				}
			}
			ufp::system("zip -0r " + ufp::s_quote(filepath) + " " + [&targetnames]()->std::string{ std::string Result; for(std::string i : targetnames){ Result += ufp::s_quote(Result) + " "; }; return Result; }());
			ufp::system("rm -r -- .* *"); //remove_all(".");
			if ( chdir(home.c_str()) != 0 ) {
				cerr <<"작업위치를 바꾸는데 실패했습니다!"<<endl;
				return 1;
			}
		//존재하지 않는 패턴일 경우를 보고.
		} else {
			if ( fileextLOW.empty() ) {
				cout <<"이 파일은 확장자를 가지지 않은 파일입니다. 이런 파일은 처리하지 않습니다."<<endl;
			} else {
				cout <<"이 확장자는 처리 대상에 존재하지 않는 새로운 확장자입니다."<<endl;
				if ( O_report ) {
					not_reported_ext.push_back(fileext);
				}
			}
		}
		if ( gui_o ) {
			std::ofstream OutFile("/tmp/.progress-" + ppid, ios::app);
			OutFile <<(++count)<<std::endl;
			OutFile.close();
		}
	}

	//문서 내에 존재하는 외부 참조 동영상을 로컬 디스크에 저장함
	{
		if ( chdir(home.c_str()) != 0 ) {
			cerr <<"작업위치를 바꾸는데 실패했습니다!"<<endl;
			return 1;
		}

		if ( videoDown_o = SUB && is_regular_file("index.html") && is_directory("index_files") && is_regular_file("index.rdf") ) {
			for ( boost::filesystem::recursive_directory_iterator end, codefilepath("./"); codefilepath != end; ++codefilepath ) {
				if( ! is_regular_file(*codefilepath) )
					continue;
				if ( ! RE("\.html$", PCRE_UTF8 | PCRE_CASELESS).PartialMatch(codefilepath->path().leaf()) )
					continue;
				cout << *codefilepath << std::endl;
	// 			grep -iE '/cache\.php(-[0-9]+)?\.html$' <<<"$codefilepath" && continue
	// 			code=$(<"$codefilepath")
	// 			filedir=$(dirname "$codefilepath")
	//
	// 			#HTML5 형식으로된 youtube동영상 추출
	// 			for i in $(grep --text -zEio '< *iframe[^>]+src *= *"[^"]+\.html"[^>]+>[^>]+ */ *iframe *>' <<<"$code"); do
	// 				link=$(grep -Eio 'src *= *"[^"]+"' <<<"$i" | sed -e 's/^src *= *"//' -e 's/"$//')
	// 				grep -Fi -e html5player -e www-embed-player -e 'http://www.youtube.com/' "$filedir/$link" || continue
	// 				uri=$(grep -m1 -iE '< *link[^>]+>' -- "$filedir/$link" | grep -iEo 'href *= *"http://www.youtube.com/[^"]+"' | sed -r -e 's/^[^"]+"//' -e 's/"$//')
	// 				source=$(youtube-dl --get-title --get-filename "$uri") || continue
	// 				rm -- "$filedir/$link"
	// 				savedir=$(dirname -- "$filedir/$link")
	// 				if [ "$savedir" = . ]; then
	// 					savedir=index_files
	// 					linkdir=../index_files
	// 				else
	// 					linkdir=../$(dirname -- "$link")
	// 				fi
	// 				savename=$(sed -n 2p <<<"$source" | dupev_name -i -p "$savedir")
	// 				youtube-dl --no-part --output "$savedir/$savename" "$uri"
	// 				temp="<div><p><a href=\"$(html_escape $uri)\" target=\"_blank\">$(head -n1 <<<"$source" | html_escape)</a></p><embed $(grep -iEo -m1 -e "width *= *\"?[0-9]+\"?" -e "height *= *\"?[0-9]+\"?" <<<"$i" | tr '\n' ' ') showcontrols=\"true\" showstatusbar=\"true\" autostart=\"false\" src=\"$(html_escape "$linkdir/$savename")\" type=\"application/x-mplayer2\"/></embed></div>"
	// 				code=$(perl -0pe 'use encoding "utf8"; open(TEXT, "'<(echo "$temp")"\"); \$data = <TEXT>; s#$(g_quote "$i")#\$data#i" <<<"$code")
	// 			done
	//
	// 			#SWF 형식으로된 youtube동영상 추출. : object
	// 			{
	// 				unset list
	// 				tmpcode=$code
	// 				while true; do
	// 					i=$(grep --text -zPio '< *object[^>]+>[^\0]*?< */ *object *>' <<<"$tmpcode") || break
	// 					list+=("$(perl -0pe 's#(< */ *object *>)[^\0]*$#\1#' <<<"$i")")
	// 					tmpcode=$(perl -0pe 's#^[^\0]+?< */ *object *>([^\0]*)$#\1#' <<<"$i")
	// 				done
	// 				for((c=0; c<${#list[@]}; c++)); do
	// 					i=${list[$c]}
	// 					grep -Ei -e '<param value="[^.]+.swf" name="movie">' -e 'application/x-shockwave-flash' <<<"$i" || continue
	// 					link=$(grep -Pio '(?<=src=")[^"]+?\.swf(?=")' <<<"$i" | head -n1 | html_unescape)
	// 					swfdump "$filedir/$link" | grep -F 'com_google_youtube_application_SwfProxy' || continue
	// 					uri="https://www.youtube.com/watch?v=$(ex_name "$link")"
	// 					source=$(youtube-dl --get-title --get-filename "$uri") || continue
	// 					rm -- "$filedir/$link"
	// 					savedir=$(dirname -- "$filedir/$link")
	// 					if [ "$savedir" = . ]; then
	// 						savedir=index_files
	// 						linkdir=../index_files
	// 					else
	// 						linkdir=../$(dirname -- "$link")
	// 					fi
	// 					savename=$(sed -n 2p <<<"$source" | dupev_name -i -p "$savedir")
	// 					youtube-dl --no-part --output "$savedir/$savename" "$uri"
	// 					temp="<div><p><a href=\"$(html_escape $uri)\" target=\"_blank\">$(head -n1 <<<"$source" | html_escape)</a></p><embed $(grep -iEo -m1 -e "width *= *\"?[0-9]+\"?" -e "height *= *\"?[0-9]+\"?" <<<"$i" | tr '\n' ' ') showcontrols=\"true\" showstatusbar=\"true\" autostart=\"false\" src=\"$(html_escape "$linkdir/$savename")\" type=\"application/x-mplayer2\"/></embed></div>"
	// 					code=$(perl -0pe 'use encoding "utf8"; open(TEXT, "'<(echo "$temp")"\"); \$data = <TEXT>; s#$(g_quote "$i")#\$data#i" <<<"$code")
	// 				done
	// 			}
	//
	// 			#SWF 형식으로된 youtube동영상 추출. : embed
	// 			for i in $(grep -Eio '<embed[^>]+src="[^.]+.swf"[^>]*>' <<<"$code"); do
	// 				grep -Fi 'type="application/x-shockwave-flash"' <<<"$i" || continue
	// 				link=$(grep -Pio '(?<=src=")[^"]+?\.swf(?=")' <<<"$i" | head -n1 | html_unescape)
	// 				swfdump "$filedir/$link" | grep -F 'com_google_youtube_application_SwfProxy' || continue
	// 				uri="https://www.youtube.com/watch?v=$(ex_name "$link")"
	// 				source=$(youtube-dl --get-title --get-filename "$uri") || continue
	// 				rm -- "$filedir/$link"
	// 				savedir=$(dirname -- "$filedir/$link")
	// 				if [ "$savedir" = . ]; then
	// 					savedir=index_files
	// 					linkdir=../index_files
	// 				else
	// 					linkdir=../$(dirname -- "$link")
	// 				fi
	// 				savename=$(sed -n 2p <<<"$source" | dupev_name -i -p "$savedir")
	// 				youtube-dl --no-part --output "$savedir/$savename" "$uri"
	// 				temp="<div><p><a href=\"$(html_escape $uri)\" target=\"_blank\">$(head -n1 <<<"$source" | html_escape)</a></p><embed $(grep -iEo -m1 -e "width *= *\"?[0-9]+\"?" -e "height *= *\"?[0-9]+\"?" <<<"$i" | tr '\n' ' ') showcontrols=\"true\" showstatusbar=\"true\" autostart=\"false\" src=\"$(html_escape "$linkdir/$savename")\" type=\"application/x-mplayer2\"/></embed></div>"
	// 				code=$(perl -0pe 'use encoding "utf8"; open(TEXT, "'<(echo "$temp")"\"); \$data = <TEXT>; s#$(g_quote "$i")#\$data#i" <<<"$code")
	// 			done
	//
	// 			cat <<< "$code" > "$codefilepath"
			}
		}
	}

	//드래그 방지 태그를 제거.
	{
		if ( chdir(home.c_str()) != 0 ) {
			cerr <<"작업위치를 바꾸는데 실패했습니다!"<<endl;
			return 1;
		}

		//일반 페이지인 경우
		recursive_directory_iterator end;
		string temp;
		for( recursive_directory_iterator it(current_path()); it!=end; ++it ) {
			if( ! is_regular_file(*it) )
				continue;
			if ( ! RE("\.(html|htm)$", PCRE_UTF8 | PCRE_CASELESS).PartialMatch(it->path().leaf()) )
				continue;
			ifstream ReadFile(it->path());
			bool ch = false;
			string code;
			while(getline(ReadFile, temp)) {
				if ( RE("-moz-user-select: none;", PCRE_UTF8 | PCRE_CASELESS).GlobalReplace("", &temp) ){
					ch = true;
				}
				code += temp;
				code += "\n";
			}
			ReadFile.close();
			if ( ch ) {
				ofstream SaveFile(it->path());
				SaveFile <<code;
				SaveFile.close();
			}
		}

		//네이버 인쇄 페이지의 경우
		if ( DragAssent_o = SUB && is_regular_file("index.rdf") && is_regular_file("index.html") ) {
			ifstream ReadFile("index.rdf");
			bool work = false;
			while(getline(ReadFile, temp)) {
				if ( RE("\"네이버 [^ ]+ :: 포스트 내용 Print\"",  PCRE_UTF8 | PCRE_CASELESS).PartialMatch(temp) ) {
					work = true;
					break;
				}
			}
			ReadFile.close();
			if ( work ) {
				ifstream ReadFile("index.html");
				string code;
				while(getline(ReadFile, temp)) {
					code += temp;
					code += "\n";
				}
				ReadFile.close();
				RE("< *div +class *= *\"?clickprevent\"?.*?< */ *div *>",  PCRE_UTF8 | PCRE_CASELESS | PCRE_MULTILINE).GlobalReplace("", &code);
				ofstream SaveFile("index.html");
				SaveFile <<code;
				SaveFile.close();
			}
		}
	}

	//작업 결과 보고 및 작업종료 처리
	if ( O_reportgui ) {
// 	[ $gui_o = y ] && echo END > /tmp/.progress-$ppid
		percentage = 100;
	}
	//ln 오류 내역이 내용이 존재하지 않을 경우 삭제처리
	if ( O_report && file_size(home/"link/error.log") == 0 ) {
		remove(home/"link/error.log");
	}
	//만약, 링크된 것이 존재하지 않는 폴더가 존재한다면, 해당 빈 폴더는 삭제합니다.
	if ( O_link ) {
		if ( exists("link") ) {
			list<string> ls;
			DIR *dir_info = opendir("link");
			struct dirent *dir_entry;
			if (NULL != dir_info) {
				while(dir_entry = readdir(dir_info))
					ls.push_back(dir_entry->d_name);
				if(ls.size() == 2) {
					remove("link");
				} else {
					for(string i : ls){
						if(is_directory(i)){
							dir_info = opendir("link/" + i);
							if (NULL != dir_info) {
								int count = 0;
								while(dir_entry = readdir(dir_info)) {
									if ( ++count > 2 )
										break;
								}
								if ( count == 2 ) {
									remove("link/" + i);
								}
							} else {
								cerr <<"화일 목록을 가져올수 없습니다!"<<endl;
							}
						}
					}
				}
			} else {
				cerr <<"화일 목록을 가져올수 없습니다!"<<endl;
			}
		}
	}
	if ( O_report ) {
		if ( O_reportgui ) {
			//notify-send '문서 일괄 변환이 끝났습니다.'
		}
		//만약 보고되지 않은 확장자가 존재한다면 보고서를 작성함.
		if ( ! not_reported_ext.empty() ) {
			ofstream SaveFile(home + "/not_reported_ext.log");
			SaveFile <<"등록되지 않은 확장자가 존재합니다!"<<endl;
			auto sortfunc =
			[=,&](string str1, string str2) -> bool {
				auto begin = str1.begin();
				transform(begin, str1.end(), begin, ::tolower);
				if ( str1.compare(str2) < 0 )
					return true;
				return false;
			};
			auto uniquefunc =
			[=,&](string str1, string str2) -> bool {
				return boost::iequals(str1, str2);
			};
			not_reported_ext.sort(sortfunc());
			not_reported_ext.unique(uniquefunc());
			for(string i : not_reported_ext)
				SaveFile <<not_reported_ext<<endl;
			SaveFile.close();
		}
		//만약 오류 보고서가 존재한다면, 해당 보고서를 염.
		auto existopen =
		[&](string error_path){
			if ( exists(error_path) ) {
				system("xdg-open " + s_quote(error_path) + " &");
			}
		};
		existopen(home + "/link/error.log");
		existopen(home + "/error.log");
		existopen(home + "/not_reported_ext.log");
		existopen(home + "/link")
	}

	remove_all(tmpdir);
	return 0;
}