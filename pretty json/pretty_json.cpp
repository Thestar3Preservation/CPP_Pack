// 라이브러리에 관련된 내용은 다음을 참조 : http://www.codeproject.com/KB/recipes/JSON_Spirit.aspx
// 출처 : http://markharrison.net/stackoverflow/t2-json.cpp
// 별님에 의해 수정됨.

#include <sstream>
#include <iostream>
#include <json_spirit.h>
#include <json_spirit_stream_reader.h>

void PrintJsonPretty(const std::string &JsonStr);

static void pp(const json_spirit::Value &v, int lev);
template <typename T> static void pp(const json_spirit::Value &v, int lev);
static void indent (int lev);
static void pp_array(const json_spirit::Value &v, int lev);
static void pp_obj(const json_spirit::Value &v, int lev);
static void pp_null(const json_spirit::Value &v, int lev);
static void pretty_print(json_spirit::Stream_reader<std::istringstream, json_spirit::Value> &reader);

void indent (int lev) {
	for (int i = 0; i < lev; ++i) {
		std::cout << "  ";
	}
}

void pp_array(const json_spirit::Value &v, int lev) {
	indent(lev);
	std::cout << "[" << std::endl;
	
	const json_spirit::Array& a = v.get_array();
	for (int i = 0; i < a.size(); ++i) {
		pp(a[i],lev+1);
	}
	
	indent(lev);
	std::cout << "]" << std::endl;
}

void pp_obj(const json_spirit::Value &v, int lev) {
	int i;
	indent(lev);
	std::cout << "{" << std::endl;
	
	const json_spirit::Object& o = v.get_obj();
	for (i = 0; i < o.size(); ++i) {
		const json_spirit::Pair& p = o[i];
		
		indent(lev+1);
		std::cout << p.name_ << ":" << std::endl;
		pp(p.value_, lev+1);
	}
	
	indent(lev);
	std::cout << "}" << std::endl;
}

void pp_null(const json_spirit::Value &v, int lev) {
	indent(lev);
	std::cout << "null" << std::endl;
}

template <typename T> 
void pp(const json_spirit::Value &v, int lev) {
	indent(lev);
	std::cout << v.get_value<T>() << std::endl;
}

void pp(const json_spirit::Value &v, int lev=-1) {
	switch(v.type()) {
		case json_spirit::obj_type:    
			pp_obj(v, lev+1);   
			break;
		case json_spirit::array_type:  
			pp_array(v, lev+1);
			break;
		case json_spirit::str_type:    
			pp<std::string>(v, lev+1);  
			break;
		case json_spirit::bool_type:   
			pp<bool>(v, lev+1);
			break;
		case json_spirit::int_type:  
			pp<int>(v, lev+1); 
			break;
		case json_spirit::real_type: 
			pp<double>(v, lev+1); 
			break;
		case json_spirit::null_type: 
			pp_null(v, lev+1);
			break;
	}
}

void pretty_print(json_spirit::Stream_reader<std::istringstream, json_spirit::Value> &reader) {
	json_spirit::Value value;
	reader.read_next(value);
	pp(value);
}

extern "C" void PrintJsonPretty(
	const char *JsonStr
){
	std::istringstream is(JsonStr);
	json_spirit::Stream_reader<std::istringstream, json_spirit::Value> reader(is);
	pretty_print(reader);
}