#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>
#include <glib/gstdio.h>
#include "function/UserFunctionPack.hpp"

//인자 2 : 파일명
//인자 3 : 파일경로

// inline void Callback_Open( NotifyNotification *pNotify, gchar *action, gpointer user_data ) {
// 	int temp = system( string( "xdg-open " + s_quote( *((string*)user_data) ) ).c_str() );
// 	notify_notification_close(pNotify, NULL);
// }

int main(int argc, char *argv[]){
	std::string name, herf;
	std::string filepath = ufp::to_string(argv[3]) + "/" + ufp::to_string(argv[2]);
	if ( std::string(argv[2]).find("&") == string::npos ) {
		herf = filepath;
		name = argv[2];
	} else {
		g_mkdir("/tmp/deluge_ln", 0700);
		name = ufp::replaceAll(argv[2], "&", "＆");
		std::string tempname = ufp::dupev_name(name, "/tmp/deluge_ln");
		int temp = symlink( filepath.c_str(), string("/tmp/deluge_ln/" + tempname).c_str() );
		herf = "/tmp/deluge_ln/" + tempname;
	}

	notify_init("basic");
	NotifyNotification *delugeDown;
	delugeDown = notify_notification_new("토렌트가 다운로드 되었습니다.", std::string("토렌트 <a href='" + herf + "'><b>" + name + "</b></a>가 다운로드 완료되었습니다.").c_str(), NULL);
// 	notify_notification_add_action(delugeDown, "open", "대상 열기", NOTIFY_ACTION_CALLBACK(Callback_Open), &filepath, NULL);
	notify_notification_set_timeout(delugeDown, 5000);
	notify_notification_show(delugeDown, NULL);
}