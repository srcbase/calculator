#pragma once
#include <cstdint>
#include <variant>

enum class op_type
{
	add,
	sub,
	mul,
	div,
	pow,
	p_open,
	p_close
};

template <typename T>
class basic_token
{
	enum class type
	{
		operand_,
		operator_
	} token_type;

	std::variant<T, op_type> token_content;

public:

	basic_token() = delete;

	basic_token(basic_token&&) = default;
	basic_token(const basic_token&) = default;

	basic_token& operator=(basic_token&&) = default;
	basic_token& operator=(const basic_token&) = default;
	
	basic_token(T value) :
		token_type(type::operand_), token_content(value)
	{
	}

	basic_token(op_type op) :
		token_type(type::operator_), token_content(op)
	{
	}

	auto is_operand() const -> bool
	{
		return token_type == type::operand_;
	}

	auto is_operator() const -> bool
	{
		return token_type == type::operator_;
	}

	auto get_operand() const -> T
	{
		return std::get<T>(token_content);
	}

	auto get_operator() const -> op_type
	{
		return std::get<op_type>(token_content);
	}
};