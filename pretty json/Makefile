#########################매크로 정의 부분#########################
.PHONY		= all test release clean cleanall install compile
CXX		= g++
CPPFLAGS	= $(MAKE_LIBRARYPATH) -I"$(PATH_SOURCECODEFUNCTION)"
LIBS		= $(MAKE_USERFUNCTIONPACKLINK) -ljson_spirit
Objs		:= main.o pretty_json_builtin.o
Output		= PrettyJson

########################명령어 정의 부분###########################
all : test

#테스트용으로 컴파일
test : CPPFLAGS += -O0
test : compile

#출시용으로 컴파일
release : CPPFLAGS += -O3
release : compile

#생성된 임시 파일들을 삭제
clean :
	$(RM) $(Objs)

#생성된 임시 파일과 결과물까지 모두 삭제
cleanall : 
	$(RM) $(Objs) $(Output) pretty_json.so

#컴파일 하기
compile : $(Output) pretty_json.so

#설치하기
install : $(Output) pretty_json.so
	linkto run $(Output) prettyjson
	
#########################의존관계 부분#############################
$(Output) : $(Objs) 
	$(CXX) $(CPPFLAGS) -o $(Output) $(Objs) $(LIBS)
	
main.o: CPPFLAGS += -std=c++0x
	
pretty_json.so: pretty_json.cpp
	$(CXX) $(CPPFLAGS) -shared -fPIC -o pretty_json.so pretty_json.cpp -ldl

main.o: main.cpp pretty_json_builtin.hpp
	$(CXX) $(CPPFLAGS) -c main.cpp
	
pretty_json_builtin.o: pretty_json_builtin.cpp pretty_json_builtin.hpp
	$(CXX) $(CPPFLAGS) -c pretty_json_builtin.cpp