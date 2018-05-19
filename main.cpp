#include "calculator.hpp"
#include "fmt.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iomanip>
#include <string>

namespace console_colors
{
	constexpr uint16_t white	= 15;
	constexpr uint16_t purple	= 13;
	constexpr uint16_t red		= 12;
	constexpr uint16_t cyan		= 11;
}

static const auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

auto clear_console() -> void
{
	const auto top_left = COORD{0, 0};
	auto screen			= CONSOLE_SCREEN_BUFFER_INFO{};
	auto written		= DWORD{};

	GetConsoleScreenBufferInfo(console_handle, &screen);

	FillConsoleOutputCharacterA(
		console_handle, ' ', screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);

	FillConsoleOutputAttribute(
		console_handle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, top_left, &written
	);

	SetConsoleCursorPosition(console_handle, top_left);
}

auto set_color(uint16_t color) -> void
{
	SetConsoleTextAttribute(console_handle, color);
}

auto print_expression_result(const std::string& expression) -> void
{
	fmt::print("   ");

	try
	{
		set_color(console_colors::purple);

		const auto result = calculator::evaluate(expression);
		fmt::print("= {}\n", result);
	}
	catch (const std::exception& argument)
	{
		set_color(console_colors::red);
		fmt::print("couldn't evaluate expression: {}\n", argument.what());
	}
	catch (...)
	{
		set_color(console_colors::red);
		fmt::print("couldn't evaluate expression: unknown exception\n");
	}
	
	set_color(console_colors::white);
}

auto main() -> int32_t
{
	std::wcout << std::setprecision(5) << std::showpoint << std::fixed;
	SetConsoleTitleA("calculator");

	for (std::string expression; true; )
	{
		set_color(console_colors::cyan);
		fmt::print(">> ");
		set_color(console_colors::white);

		if (std::getline(std::cin, expression))
		{
			if (expression == "c" || expression == "clear")
			{
				clear_console();
				continue;
			}

			print_expression_result(expression);
		}

		fmt::print("\n");
	}
}
