#include "calculator.hpp"
#include <algorithm>
#include <sstream>
#include <string>
#include <cctype>
#include <stack>
#include <map>

static std::map<char, op_type> char_to_operator
{
	{'+', op_type::add},
	{'-', op_type::sub},
	{'*', op_type::mul},
	{'/', op_type::div},
	{'^', op_type::pow},
	{'(', op_type::p_open},
	{')', op_type::p_close},
};

template <typename T>
__forceinline auto pop_get(std::stack<T>& stack) -> T
{
	if (stack.empty())
		throw std::invalid_argument("mismatched operators");

	const auto value = stack.top();
	stack.pop();

	return value;
}

auto calculator::parse_expression(std::string expression) -> token_list
{
	std::vector<token> result;
	if (expression.empty())
		throw std::invalid_argument{"expression is empty"};

	size_t	operand_begin{};
	bool	is_processing_operand{};
	bool	is_processing_negative{};

	expression.erase(std::remove_if(expression.begin(), expression.end(), [](char ch)
	{
		return isblank(ch) || ch == '\n' || ch == '\r' || ch == '=';
	}), expression.end());

	const auto parse_value = [&](std::string::iterator begin_it, std::string::iterator end_it)
	{
		std::stringstream conv_stream;
		conv_stream << std::string(begin_it, end_it);

		ty value{};
		conv_stream >> value;

		if (!conv_stream || conv_stream.bad() || conv_stream.fail())
			throw std::invalid_argument("invalid operand");

		result.emplace_back(value);
	};

	for (size_t i{}; i < expression.length(); ++i)
	{
		const auto ch = expression[i];

		if (!is_processing_operand && expression.size() > i + 2 && 
			expression[i + 0] == '(' && expression[i + 1] == '-' && isdigit(expression[i + 2]))
		{
			is_processing_operand	= true;
			is_processing_negative	= true;
			operand_begin			= i + 1;
		}
		else if (is_processing_operand && is_processing_negative)
		{
			if (ch == ')')
			{
				is_processing_operand	= false;
				is_processing_negative	= false;

				parse_value(expression.begin() + operand_begin, expression.begin() + i);
			}
		}
		else if (char_to_operator.find(ch) != char_to_operator.end())
		{
			if (is_processing_operand)
			{
				is_processing_operand	= false;
				is_processing_negative	= false;
				parse_value(expression.begin() + operand_begin, expression.begin() + i);
			}

			result.emplace_back(char_to_operator[ch]);
		}
		else if (!is_processing_operand)
		{
			is_processing_operand	= true;
			is_processing_negative	= false;
			operand_begin			= i;
		}
	}

	if (is_processing_operand)
		parse_value(expression.begin() + operand_begin, expression.end());

	if (is_processing_negative)
		throw std::invalid_argument("mismatched parentheses (3)");

	return result;
}

auto calculator::evaluate_infix_expression(const token_list& list) -> ty
{
	std::vector<token>  postfix;
	std::stack<op_type> stack;

	static const auto get_precedence = [](op_type type)
	{
		switch (type)
		{
		case op_type::add:
		case op_type::sub:
			return 1;

		case op_type::mul:
		case op_type::div:
			return 2;

		case op_type::pow:
			return 3;

		default:
			return 0;
		}
	};

	for (const auto& current_token : list)
	{
		if (current_token.is_operand())
		{
			postfix.emplace_back(current_token);
		}
		else if (current_token.get_operator() == op_type::p_open)
		{
			stack.emplace(current_token.get_operator());
		}
		else if (current_token.get_operator() == op_type::p_close)
		{
			while (!stack.empty() && stack.top() != op_type::p_open)
				postfix.emplace_back(pop_get(stack));

			if (stack.empty())
				throw std::invalid_argument("mismatched parentheses (0)");

			stack.pop();
		}
		else
		{
			while (!stack.empty() && stack.top() != op_type::p_open &&
				get_precedence(stack.top()) >= get_precedence(current_token.get_operator()))
			{
				postfix.emplace_back(pop_get(stack));
			}

			stack.emplace(current_token.get_operator());
		}
	}

	while (!stack.empty())
	{
		const auto op = pop_get(stack);
		if (op == op_type::p_open || op == op_type::p_close)
			throw std::invalid_argument("mismatched parentheses (1)");

		postfix.emplace_back(op);
	}

	return evaluate_postfix_expression(postfix);
}

auto calculator::evaluate_postfix_expression(const token_list& list) -> ty
{
	std::stack<ty> stack;
	if (list.empty())
		throw std::invalid_argument{"expression is empty"};

	for (const auto& current_token : list)
	{
		if (current_token.is_operator())
		{
			const auto operand_1 = pop_get(stack);
			const auto operand_2 = pop_get(stack);

			const auto op = current_token.get_operator();
			if (op == op_type::div && operand_1 == 0)
				throw std::invalid_argument("dividing by 0");

			switch (op)
			{
			case op_type::add: stack.emplace(operand_2 + operand_1); break;
			case op_type::sub: stack.emplace(operand_2 - operand_1); break;
			case op_type::mul: stack.emplace(operand_2 * operand_1); break;
			case op_type::div: stack.emplace(operand_2 / operand_1); break;
			case op_type::pow: stack.emplace(std::pow(operand_2, operand_1)); break;

			default:
				throw std::invalid_argument("invalid operator");
			}
		}
		else
		{
			stack.emplace(current_token.get_operand());
		}
	}

	return stack.top();
}

auto calculator::evaluate(const std::string& infix_expression) -> ty
{
	static calculator instance{};

	const auto parsed_expression = instance.parse_expression(infix_expression);
	const auto evaluated = instance.evaluate_infix_expression(parsed_expression);

	return evaluated;
}
