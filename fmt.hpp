#pragma once
#include <iostream>

namespace fmt
{
	template <typename T>
	auto print(const T* string) -> void
	{
		std::wcout << string;
	}

	template <typename T, typename CurrentArg, typename... Args>
	auto print(const T* string, CurrentArg&& current_arg, Args&&... args) -> void
	{
		for (auto string_it = string; *string_it; ++string_it)
		{
			if (string_it[0] == '{' && string_it[1] == '}')
			{
				std::wcout << current_arg;
				print(string_it + 2, args...);

				return;
			}

			std::wcout << *string_it;
		}
	}

	template <typename T, typename... Args>
	auto print(const std::basic_string<T>& string, Args&&... args) -> void
	{
		print(string.c_str(), std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	auto print(std::basic_string<T>&& string, Args&&... args) -> void
	{
		print(string.c_str(), std::forward<Args>(args)...);
	}
}