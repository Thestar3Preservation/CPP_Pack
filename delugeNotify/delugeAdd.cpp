#include <libnotify/notify.h>
#include "function/UserFunctionPack.hpp"

int main(int argc, char *argv[]){
	notify_init("basic");
	NotifyNotification *delugeAdd;
	delugeAdd = notify_notification_new("토렌트가 추가 되었습니다.", std::string("토렌트 <b>" + ufp::replaceAll(argv[2], "&", "＆") + "</b>가 다운로드 목록에 추가되었습니다.").c_str(), NULL);
	notify_notification_set_timeout(delugeAdd, 5000);
	notify_notification_show(delugeAdd, NULL);
}