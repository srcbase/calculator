#pragma once
#include "token.hpp"
#include <vector>

class calculator
{
	using ty	= long double;
	using token = basic_token<ty>;

	using token_list = std::vector<token>;

	auto parse_expression(std::string expression) -> token_list;

	auto evaluate_infix_expression(const token_list& list) -> ty;
	auto evaluate_postfix_expression(const token_list& list) -> ty;

	calculator() = default;

public:

	calculator(calculator&&) = delete;
	calculator(const calculator&) = delete;

	calculator& operator=(calculator&&) = delete;
	calculator& operator=(const calculator&) = delete;

	static auto evaluate(const std::string& infix_expression) -> ty;
};
