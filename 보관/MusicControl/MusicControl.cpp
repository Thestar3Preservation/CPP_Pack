#include "function.h"
#include <sqlite3.h>
#include <gio/gio.h>
#include <libnotify/notify.h>

const char* PATH_MUSIC = "/home/user/Home/Music/";
GDBusError error;
GDBusConnection *conn;

inline string control(const string &argv1, const string &argv2) {
	string temp = "org.bansheeproject.Banshee." + argv1;
	GDBusMessage *msg = g_dbus_message_new_method_call("org.bansheeproject.Banshee", replaceAll("/" + temp, ".", "/").c_str(), temp.c_str(), argv2.c_str());
	GDBusMessage *msg2 =  g_dbus_message_new_method_reply(msg);
	g_object_unref(msg);
	g_object_unref(msg2);
	return "";
}

inline string controld(const string &argv1, const string &argv2, const string &argv3) {
	string temp = "org.bansheeproject.Banshee." + argv1;
	GDBusMessage *msg = g_dbus_message_new_method_call("org.bansheeproject.Banshee", replaceAll("/" + temp, ".", "/").c_str(), temp.c_str(), argv2.c_str());
	g_dbus_message_set_body(msg, g_variant_new());
	g_dbus_message_new_method_reply(msg);
	g_object_unref(msg);
	return "";
}

inline string getPath(){
	string temp = control("PlayerEngine", "GetCurrentUri");
	RE("(?<=\"file://).*(?=\")", PCRE_UTF8).PartialMatch(temp, &temp);
	return url_decoding(temp);
}

inline void stop(){ control("PlayerEngine", "Close"); }
inline void play(){ control("PlayerEngine", "Play"); }
inline void next(){
// 	control PlaybackController Next $FALSE;
}

inline void F_exit(){
	g_dbus_connection_flush(conn, NULL, NULL, NULL);
	g_dbus_connection_close(conn, NULL, NULL, NULL);
	exit(0);
}

inline void notify(string message){
	notify_init("basic");
	NotifyNotification *mesg = notify_notification_new(message.c_str(), "", NULL);
	notify_notification_set_timeout(mesg, 3000);
	notify_notification_set_urgency(mesg, NOTIFY_URGENCY_LOW);
	notify_notification_show(mesg, NULL);
}

int main(int argc, char *argv[]) {
	if ( argc == 1 )
		F_exit();

	string cmd = argv[1];
// 	g_dbus_error_init(&error);
	g_bus_get(G_BUS_TYPE_SESSION, /*conn*/NULL, NULL, NULL);

	if ( cmd.compare("load") == 0 ) {
		stop();
		sqlite3 *mdb;
		sqlite3_open("~/.config/banshee-1/banshee.db", &mdb);
		sqlite3_exec(mdb, "DELETE FROM coretracks where PrimarySourceID = (SELECT PrimarySourceID FROM CorePrimarySources WHERE StringID = 'FileSystemQueueSource-file-system-queue')", NULL, NULL, NULL);
		sqlite3_close(mdb);
		string cmdline = "banshee --play-enqueued";
		for (int i=1; i<argc; i++)
			cmdline += " " + s_quote(argv[i]);
		system(cmdline.c_str());
		play();
	} else if ( cmd.compare("ToggleRepeat") == 0 ) {
		const int Repeat_NO = 0; //반복 기능 해제 상태
		const int Repeat_ALL = 1; //모두 반복 상태
		const int Repeat_ONE = 2; //한 곡 반복 상태
		auto SetRepeat = [](int s){ /*control PlaybackController SetRepeatMode int32:$1*/; };
		string temp = control("PlaybackController", "GetRepeatMode");
		RE("[0-9]+$", PCRE_UTF8 | PCRE_MULTILINE).PartialMatch(temp, &temp);
		switch( atoi(temp.c_str()) ){
			case Repeat_NO:
				SetRepeat(Repeat_ALL);
				notify("밴시 : 모두 반복이 설정되었습니다.");
				break;
			case Repeat_ALL:
				SetRepeat(Repeat_ONE);
				notify("밴시 : 한 곡 반복이 설정되었습니다.");
				break;
			case Repeat_ONE:
				SetRepeat(Repeat_NO);
				notify("밴시 : 반복 기능이 해제되었습니다.");
				break;
		}
	} else if ( cmd.compare("ToggleShuffle") == 0 ) {
		auto SetShuffleMode = [](){ /*control PlaybackController SetShuffleMode string:$1*/; };
		string temp = control("PlaybackController", "GetShuffleMode");
		RE("(?<=\").*(?=\")", PCRE_UTF8).PartialMatch(temp, &temp);
		if ( temp.compare("song") == 0 ) {
			SetShuffleMode("off");
			notify("밴시 : 순서 섞기 기능이 해제되었습니다.");
		} else {
			SetShuffleMode("song");
			notify("밴시 : 순서 섞기 기능이 설정되었습니다.");
		}
	} else if ( cmd.compare("TogglePlayStop") == 0 ) {
		control("PlayerEngine", "TogglePlaying");
	} else if ( cmd.compare("DeletePlayTrack") == 0 ) {
		string TEMP = getPath();
		next();
		trash-put(TEMP);
	} else if ( cmd.compare("AddMusicList") == 0 ) {
		string T1 = getPath();
		string T2 = PATH_MUSIC + basename(T1);
		bool ch = false;
		if ( access(T2.c_str(), F_OK) == 0 )
			ch = ( fmd5sum(T1) == fmd5sum(T2) );
		if ( ch ) {
			notify("사용자 음악 폴더에 이미 추가된 파일입니다");
		} else {
			notify("사용자 음악 폴더에 현재 재생중인 파일을 복사합니다.");
			dupev_cp(T1, PATH_MUSIC);
		}
	} else if ( cmd.compare("previous") == 0 ) {
		//control PlaybackController Previous $FALSE
	} else if ( cmd.compare("next") == 0 ) {
		next();
	} else if ( cmd.compare("stop") == 0 ) {
		stop();
	} else if ( cmd.compare("play") == 0 ) {
		play();
	} else if ( cmd.compare("hide") == 0 ) {
		control("ClientWindow", "Hide");
	} else if ( cmd.compare("show") == 0 ) {
		control("ClientWindow", "Present");
	}

	F_exit();
}