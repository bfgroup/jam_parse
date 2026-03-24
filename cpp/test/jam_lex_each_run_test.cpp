// Copyright René Ferdinand Rivera Morell
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <jam_parse/jam_lex.hpp>

#include <boost/core/lightweight_test.hpp>

int main()
{
	BOOST_TEST(true);
	static auto jam = R"jam(
# line comment

#| one line block comment |#

#|
multi-line
block comment
|#

actions bind break case class continue default else existing for if ignore in
include local module on piecemeal quietly return rule switch together updated
while

&& != || <= += ?= >= & ! | : = < { [ ( + ? > } ] ) ; *

$(value) @(value) $(val$(b)) $(a@(b))

	"quote $(a) += @(b) \" '"

)jam";
	struct token_t
	{
		std::size_t index;
		std::uint64_t token_index;
		std::size_t line;
		std::size_t column;
		const char * str;
	};
	static token_t tokens[] = {
		{ 0, 50, 0, 0, "\n" },
		{ 1, 51, 1, 0, "# line comment" },
		{ 2, 52, 1, 14, "\n\n" },
		{ 3, 51, 3, 0, "#| one line block comment |#" },
		{ 4, 52, 3, 28, "\n\n" },
		{ 5, 51, 5, 0, "#|\nmulti-line\nblock comment\n|#" },
		{ 6, 52, 8, 2, "\n\n" },
		{ 7, 24, 10, 0, "actions" },
		{ 8, 52, 10, 7, " " },
		{ 9, 25, 10, 8, "bind" },
		{ 10, 52, 10, 12, " " },
		{ 11, 26, 10, 13, "break" },
		{ 12, 52, 10, 18, " " },
		{ 13, 27, 10, 19, "case" },
		{ 14, 52, 10, 23, " " },
		{ 15, 28, 10, 24, "class" },
		{ 16, 52, 10, 29, " " },
		{ 17, 29, 10, 30, "continue" },
		{ 18, 52, 10, 38, " " },
		{ 19, 30, 10, 39, "default" },
		{ 20, 52, 10, 46, " " },
		{ 21, 31, 10, 47, "else" },
		{ 22, 52, 10, 51, " " },
		{ 23, 32, 10, 52, "existing" },
		{ 24, 52, 10, 60, " " },
		{ 25, 33, 10, 61, "for" },
		{ 26, 52, 10, 64, " " },
		{ 27, 34, 10, 65, "if" },
		{ 28, 52, 10, 67, " " },
		{ 29, 35, 10, 68, "ignore" },
		{ 30, 52, 10, 74, " " },
		{ 31, 36, 10, 75, "in" },
		{ 32, 52, 10, 77, "\n" },
		{ 33, 37, 11, 0, "include" },
		{ 34, 52, 11, 7, " " },
		{ 35, 38, 11, 8, "local" },
		{ 36, 52, 11, 13, " " },
		{ 37, 39, 11, 14, "module" },
		{ 38, 52, 11, 20, " " },
		{ 39, 40, 11, 21, "on" },
		{ 40, 52, 11, 23, " " },
		{ 41, 41, 11, 24, "piecemeal" },
		{ 42, 52, 11, 33, " " },
		{ 43, 42, 11, 34, "quietly" },
		{ 44, 52, 11, 41, " " },
		{ 45, 43, 11, 42, "return" },
		{ 46, 52, 11, 48, " " },
		{ 47, 44, 11, 49, "rule" },
		{ 48, 52, 11, 53, " " },
		{ 49, 45, 11, 54, "switch" },
		{ 50, 52, 11, 60, " " },
		{ 51, 46, 11, 61, "together" },
		{ 52, 52, 11, 69, " " },
		{ 53, 47, 11, 70, "updated" },
		{ 54, 52, 11, 77, "\n" },
		{ 55, 48, 12, 0, "while" },
		{ 56, 52, 12, 5, "\n\n" },
		{ 57, 0, 14, 0, "&&" },
		{ 58, 52, 14, 2, " " },
		{ 59, 2, 14, 3, "!=" },
		{ 60, 52, 14, 5, " " },
		{ 61, 4, 14, 6, "||" },
		{ 62, 52, 14, 8, " " },
		{ 63, 8, 14, 9, "<=" },
		{ 64, 52, 14, 11, " " },
		{ 65, 13, 14, 12, "+=" },
		{ 66, 52, 14, 14, " " },
		{ 67, 15, 14, 15, "?=" },
		{ 68, 52, 14, 17, " " },
		{ 69, 17, 14, 18, ">=" },
		{ 70, 52, 14, 20, " " },
		{ 71, 1, 14, 21, "&" },
		{ 72, 52, 14, 22, " " },
		{ 73, 3, 14, 23, "!" },
		{ 74, 52, 14, 24, " " },
		{ 75, 5, 14, 25, "|" },
		{ 76, 52, 14, 26, " " },
		{ 77, 6, 14, 27, ":" },
		{ 78, 52, 14, 28, " " },
		{ 79, 7, 14, 29, "=" },
		{ 80, 52, 14, 30, " " },
		{ 81, 9, 14, 31, "<" },
		{ 82, 52, 14, 32, " " },
		{ 83, 10, 14, 33, "{" },
		{ 84, 52, 14, 34, " " },
		{ 85, 11, 14, 35, "[" },
		{ 86, 52, 14, 36, " " },
		{ 87, 12, 14, 37, "(" },
		{ 88, 52, 14, 38, " " },
		{ 89, 14, 14, 39, "+" },
		{ 90, 52, 14, 40, " " },
		{ 91, 16, 14, 41, "?" },
		{ 92, 52, 14, 42, " " },
		{ 93, 18, 14, 43, ">" },
		{ 94, 52, 14, 44, " " },
		{ 95, 19, 14, 45, "}" },
		{ 96, 52, 14, 46, " " },
		{ 97, 20, 14, 47, "]" },
		{ 98, 52, 14, 48, " " },
		{ 99, 21, 14, 49, ")" },
		{ 100, 52, 14, 50, " " },
		{ 101, 22, 14, 51, ";" },
		{ 102, 52, 14, 52, " " },
		{ 103, 23, 14, 53, "*" },
		{ 104, 52, 14, 54, "\n\n" },
		{ 105, 49, 16, 0, "$(value)" },
		{ 106, 52, 16, 8, " " },
		{ 107, 49, 16, 9, "@(value)" },
		{ 108, 52, 16, 17, " " },
		{ 109, 49, 16, 18, "$(val$(b))" },
		{ 110, 52, 16, 28, " " },
		{ 111, 49, 16, 29, "$(a@(b))" },
		{ 112, 52, 16, 37, "\n\n\t" },
		{ 113, 49, 18, 1, "\"quote $(a) += @(b) \\\" '\"" },
		{ 114, 52, 18, 26, "\n\n" },
	};
	jam_parse::string_view v(jam);
	jam_parse::lexer lexer(v);
	std::size_t i = 0;
	for (auto token = lexer.next(jam_parse::token::value_t::all); token.known();
		token = lexer.next(jam_parse::token::value_t::all))
	{
		std::uint64_t tm = 1;
		int ti = 0;
		while ((tm & token.value) == 0)
		{
			tm <<= 1;
			ti += 1;
		}
		std::string s(token.view.begin(), token.view.end());
		for (auto j = s.find_first_of("\"\n\r\t"); j != s.npos;
			j = s.find_first_of("\"\n\r\t\\", j + 2))
		{
			switch (s[j])
			{
				case '\"': s.replace(j, 1, "\\\""); break;
				case '\n': s.replace(j, 1, "\\n"); break;
				case '\r': s.replace(j, 1, "\\r"); break;
				case '\t': s.replace(j, 1, "\\t"); break;
				case '\\': s.replace(j, 1, "\\\\"); break;
			}
		}
		std::cout
			<< "{" << i << "," << ti << "," << token.cursor_position.line << ","
			<< token.cursor_position.column << ",\"" << s << "\""
			<< "}," << "\n";
		BOOST_TEST_EQ(i, tokens[i].index);
		BOOST_TEST_EQ(ti, tokens[i].token_index);
		BOOST_TEST_EQ(token.cursor_position.line, tokens[i].line);
		BOOST_TEST_EQ(token.cursor_position.column, tokens[i].column);
		i += 1;
	}

	return boost::report_errors();
}
