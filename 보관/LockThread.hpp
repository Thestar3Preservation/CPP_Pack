#include <pthread.h> //-lpthread

class LockThread {
private:
	pthread_mutex_t mutex_lock, mutex_anchor;
	bool LockState;
public:
	inline LockThread();
	inline ~LockThread();
	inline void lock(bool);
	inline void toggle();
	inline void point();
	inline bool state();
};

//쓰레드를 잠굽니다. 쓰레드1과 쓰레드2가 존재하고, 쓰레드1에서 잠금설정을 합니다. 그리고 쓰레드2에서 잠금대기 함수가 호출되는 시점에서 쓰레드2는 일시정지됩니다. 쓰레드2는 쓰레드1 또는 기타 쓰레드에서 잠금설정이 해제되기 전까지 잠금상태에 있게 됩니다.
LockThread::LockThread(){
	mutex_lock = PTHREAD_MUTEX_INITIALIZER;
	mutex_anchor = PTHREAD_MUTEX_INITIALIZER;
	//pthread_mutex_init(&mutex_lock, NULL);
	LockState = false;
}
LockThread::~LockThread(){
	pthread_mutex_destroy(&mutex_lock);
}
void LockThread::anchor(){
	pthread_mutex_lock(&mutex_anchor);
}
void LockThread::lock(bool sel){
	if ( sel ) {
// 		if ( state() )
// 			return;
		pthread_mutex_lock(&mutex_lock);
	} else
		pthread_mutex_unlock(&mutex_lock);
	LockState = sel;
}
void LockThread::toggle(){
	lock(!state());
}
void LockThread::point(){
	if ( ! state() )
		return;
	pthread_mutex_lock(&mutex_lock);
	pthread_mutex_unlock(&mutex_lock);
}
bool LockThread::state(){
	return LockState;
}

//위 함수는 쓰레드 2개 초과에서 비정상적으로 동작합니다.
//멀티쓰레드 환경에서 잠금 코드를 구현하려면, 잠금 대상 쓰레드 외의 쓰레드에서 이미 lock된 mutex에 다시 lock하는 상황을 처리해야 합니다.