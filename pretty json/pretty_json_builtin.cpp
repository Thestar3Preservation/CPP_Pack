#include <json_spirit.h>

#include "pretty_json_builtin.hpp"

void PrintPrettyJsonBuiltin(
	const std::string& JsonStr
){
	json_spirit::Value v;
	json_spirit::read(JsonStr, v);
	json_spirit::write(v, std::cout, json_spirit::pretty_print | json_spirit::raw_utf8 | json_spirit::remove_trailing_zeros);
	std::cout <<std::endl;
}