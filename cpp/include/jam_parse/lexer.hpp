// Copyright René Ferdinand Rivera Morell
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef JAM_PARSE_LEXER_HPP
#define JAM_PARSE_LEXER_HPP

#include <cctype>
#include <cstddef>
#include <cstdint>

#ifdef __cpp_lib_string_view
#include <string_view>
#else
#include "detail/ext_boost_core_string_view.hpp"
#endif

namespace jam_parse {

#ifdef __cpp_lib_string_view
using string_view = std::string_view;
#else
using string_view = boost::core::string_view;
#endif

struct cursor
{
	std::size_t line = 0;
	std::size_t column = 0;
};

#define TK0 \
	enum \
	{ \
		zero_line = __LINE__ \
	}
#define TK(name) \
	enum : std::uint64_t \
	{ \
		name##_index = (__LINE__ - zero_line - 1), \
		name = std::uint64_t(1) << name##_index \
	}; \
	static_assert( \
		name##_index < (8 * sizeof(std::uint64_t)), "Too many tokens.")

struct token
{
	struct value_t
	{
		public:
		TK0;
		TK(op_amper_amper); // 0
		TK(op_amper);
		TK(op_bang_equals);
		TK(op_bang);
		TK(op_bar_bar);
		TK(op_bar); // 5
		TK(op_colon);
		TK(op_equals);
		TK(op_langle_equals);
		TK(op_langle);
		TK(op_lbrace); // 10
		TK(op_lbracket);
		TK(op_lparen);
		TK(op_plus_equals);
		TK(op_plus);
		TK(op_question_equals); // 15
		TK(op_question);
		TK(op_rangle_equals);
		TK(op_rangle);
		TK(op_rbrace);
		TK(op_rbracket); // 20
		TK(op_rparen);
		TK(op_semic);
		TK(op_star);
		TK(kw_actions);
		TK(kw_bind); // 25
		TK(kw_break);
		TK(kw_case);
		TK(kw_class);
		TK(kw_continue);
		TK(kw_default); // 30
		TK(kw_else);
		TK(kw_existing);
		TK(kw_for);
		TK(kw_if);
		TK(kw_ignore); // 35
		TK(kw_in);
		TK(kw_include);
		TK(kw_local);
		TK(kw_module);
		TK(kw_on); // 40
		TK(kw_piecemeal);
		TK(kw_quietly);
		TK(kw_return);
		TK(kw_rule);
		TK(kw_switch); // 45
		TK(kw_together);
		TK(kw_updated);
		TK(kw_while);
		TK(value);
		TK(indent); // 50
		TK(comment);
		TK(space);
		TK(end);

		enum : std::uint64_t
		{
			all = (end << 1) - 1
		};

		enum : std::uint64_t
		{
			ops = (op_star << 1) - 1
		};

		enum : std::uint64_t
		{
			keywords = (kw_while << 1) - 1 - ops
		};

		enum : std::uint64_t
		{
			unknown = 0
		};

		value_t() = default;
		value_t(std::uint64_t t)
			: val(t)
		{}
		value_t(value_t &&) = default;
		value_t(const value_t &) = default;
		value_t & operator=(const value_t &) = default;
		operator std::uint64_t() const
		{
			return static_cast<decltype(this->val)>(this->val);
		}
		value_t & operator=(std::uint64_t v)
		{
			this->val = v;
			return *this;
		}

		private:
		std::uint64_t val;
	};

	string_view view;
	cursor cursor_position;
	value_t value;

	bool known() const { return !view.empty() && (value != value_t::unknown); }
};

#undef TK0
#undef TK

class lexer
{
	public:
	// construction, destruction, and assignment
	explicit lexer(string_view input)
		: input(input)
		, input_position(input.begin())
	{}

	// modifiers
	token next(token::value_t expected = {});

	private:
	string_view input;
	string_view::const_iterator input_position;
	cursor cursor_position;

	string_view::value_type peek(const string_view::iterator & p);
	string_view::value_type advance(string_view::iterator & p, cursor & c);
	token make_value_to(string_view::const_iterator e, cursor c, std::uint64_t t);

	token advance_whitespace(token::value_t t = token::value_t::space);
	token advance_str(const string_view::value_type * s,
		token::value_t t,
		bool (*tail_check)(string_view::value_type));
	token advance_op(const string_view::value_type * s, token::value_t t);
	token advance_kw(const string_view::value_type * s, token::value_t t);
	token advance_value();
	token advance_comment();
};

/*
Attempt to consume a token from the given expected token set. The token set is
a bit-mask of the tokens we are allowed to consider valid at this point.
*/
inline token lexer::next(token::value_t expected)
{
	token result;
	// At the beginning of a line we want to capture any whitespace as an
	// indent token.
	if (expected & expected.space)
	{
		result = advance_whitespace(cursor_position.column == 0
				? token::value_t(token::value_t::indent)
				: token::value_t(token::value_t::space));
		if (result.known()) return result;
	}
	// Comments..
	if (expected & expected.comment)
	{
		result = advance_comment();
		if (result.known()) return result;
	}
	// Operators.. Do longer to shorter greedy parsing.
	auto next_op_f = [&](token::value_t t, const string_view::value_type * s) {
		if ((expected & t)) result = advance_op(s, t);
		return result.known();
	};
	if (next_op_f(token::value_t::op_amper_amper, "&&")) return result;
	if (next_op_f(token::value_t::op_bang_equals, "!=")) return result;
	if (next_op_f(token::value_t::op_bar_bar, "||")) return result;
	if (next_op_f(token::value_t::op_langle_equals, "<=")) return result;
	if (next_op_f(token::value_t::op_plus_equals, "+=")) return result;
	if (next_op_f(token::value_t::op_question_equals, "?=")) return result;
	if (next_op_f(token::value_t::op_rangle_equals, ">=")) return result;
	if (next_op_f(token::value_t::op_amper, "&")) return result;
	if (next_op_f(token::value_t::op_bang, "!")) return result;
	if (next_op_f(token::value_t::op_bar, "|")) return result;
	if (next_op_f(token::value_t::op_colon, ":")) return result;
	if (next_op_f(token::value_t::op_equals, "=")) return result;
	if (next_op_f(token::value_t::op_langle, "<")) return result;
	if (next_op_f(token::value_t::op_lbrace, "{")) return result;
	if (next_op_f(token::value_t::op_lbracket, "[")) return result;
	if (next_op_f(token::value_t::op_lparen, "(")) return result;
	if (next_op_f(token::value_t::op_plus, "+")) return result;
	if (next_op_f(token::value_t::op_question, "?")) return result;
	if (next_op_f(token::value_t::op_rangle, ">")) return result;
	if (next_op_f(token::value_t::op_rbrace, "}")) return result;
	if (next_op_f(token::value_t::op_rbracket, "]")) return result;
	if (next_op_f(token::value_t::op_rparen, ")")) return result;
	if (next_op_f(token::value_t::op_semic, ";")) return result;
	if (next_op_f(token::value_t::op_star, "*")) return result;
	// Keywords.. Do longer to shorter greedy parsing.
	auto next_kw_f = [&](token::value_t t, const string_view::value_type * s) {
		if ((expected & t)) result = advance_kw(s, t);
		return result.known();
	};
	if (next_kw_f(token::value_t::kw_piecemeal, "piecemeal")) return result;
	if (next_kw_f(token::value_t::kw_continue, "continue")) return result;
	if (next_kw_f(token::value_t::kw_existing, "existing")) return result;
	if (next_kw_f(token::value_t::kw_together, "together")) return result;
	if (next_kw_f(token::value_t::kw_actions, "actions")) return result;
	if (next_kw_f(token::value_t::kw_default, "default")) return result;
	if (next_kw_f(token::value_t::kw_include, "include")) return result;
	if (next_kw_f(token::value_t::kw_quietly, "quietly")) return result;
	if (next_kw_f(token::value_t::kw_updated, "updated")) return result;
	if (next_kw_f(token::value_t::kw_ignore, "ignore")) return result;
	if (next_kw_f(token::value_t::kw_module, "module")) return result;
	if (next_kw_f(token::value_t::kw_return, "return")) return result;
	if (next_kw_f(token::value_t::kw_switch, "switch")) return result;
	if (next_kw_f(token::value_t::kw_break, "break")) return result;
	if (next_kw_f(token::value_t::kw_class, "class")) return result;
	if (next_kw_f(token::value_t::kw_local, "local")) return result;
	if (next_kw_f(token::value_t::kw_while, "while")) return result;
	if (next_kw_f(token::value_t::kw_bind, "bind")) return result;
	if (next_kw_f(token::value_t::kw_case, "case")) return result;
	if (next_kw_f(token::value_t::kw_else, "else")) return result;
	if (next_kw_f(token::value_t::kw_rule, "rule")) return result;
	if (next_kw_f(token::value_t::kw_for, "for")) return result;
	if (next_kw_f(token::value_t::kw_if, "if")) return result;
	if (next_kw_f(token::value_t::kw_in, "in")) return result;
	if (next_kw_f(token::value_t::kw_on, "on")) return result;
	// Values.. Are the catchall for anything else that isn't whitespace.
	if (expected & expected.value)
	{
		result = advance_value();
		if (result.known()) return result;
	}
	// Unrecognized returns unknown.
	return result;
}

/*
Returns the character at the position if the position is valid. Otherwise
returns `0`.
*/
inline string_view::value_type lexer::peek(const string_view::iterator & p)
{
	return p != input.end() ? *p : '\0';
}

/*
Advances the position `p` and the cursor `i` one "character" if the position is
valid. Otherwise does not alter the position and cursor. Returns the current
character, not the character it advanced to. If it did not advance returns `0`.
This will skip and convert end-of-line sequences tyo a single `'\n'` character.
*/
inline string_view::value_type lexer::advance(
	string_view::iterator & p, cursor & i)
{
	string_view::value_type c = '\0';
	if (p != input.end())
	{
		c = *p;
		p += 1;
		i.column += 1;
		// Parse the 4 variants of EOL to advance the line.
		if ((c == '\n') || (c == '\r'))
		{
			i.line += 1;
			i.column = 0;
			if (p != input.end()
				&& ((c == '\r' && *p == '\n') || (c == '\n' && *p == '\r')))
				p += 1;
			c = '\n'; // Normalize the EOL indicator.
		}
	}
	return c;
}

/*
Advances the lexer position and cursor to the given position and cursor and
returns a token of the given type `t`.
*/
inline token lexer::make_value_to(
	string_view::const_iterator p, cursor c, std::uint64_t t)
{
	token result {
		input.substr(input_position - input.begin(), p - input_position),
		cursor_position, token::value_t { t }
	};
	input_position = p;
	cursor_position = c;
	return result;
}

/*
Advances the lexer position and cursor past immediate whitespace but stopping
at an end-of-line, which is included. Returns a token of the given type `t`
if there is whitespace. Otherwise returns an unknown token.
*/
inline token lexer::advance_whitespace(token::value_t t)
{
	auto position = input_position;
	auto cursor = cursor_position;
	// Advance through whitespace until EOL.
	while (position != input.end() && std::isspace(*(position))
		&& (*position != '\n' || *position != '\r'))
		advance(position, cursor);
	// Also advance past the EOL.
	if (peek(position) == '\n' || peek(position) == '\r')
		advance(position, cursor);
	// Return uknown token if we didn't find whitespace.
	if (position == input_position) return {};
	return make_value_to(position, cursor, t);
}

/*
Advance the lexer position and cursor past a matching string `s`. Returns the
token representing the advanced string if it matches. Otherwise returns the
unknown token. Even if the string matches by itself, the `tail_check` for the
past-the-end character also needs to pass for a full match.
*/
inline token lexer::advance_str(const string_view::value_type * s,
	token::value_t t,
	bool (*tail_check)(string_view::value_type))
{
	if (s == nullptr || *s == '\0') return {};
	auto position = input_position;
	auto cursor = cursor_position;
	while (position != input.end() && *s == *position)
	{
		advance(position, cursor);
		s += 1;
	}
	// Check for valid parse and valid tail to return the token.
	if (position != input_position && *s == '\0'
		&& (position == input.end() || tail_check(*position)))
		return make_value_to(position, cursor, t);
	return {};
}

/*
Advance the lexer position and cursor past the given operator `s`. Ihe operator
can be any length. But must not be followed by any of the possible operator
characters.
*/
inline token lexer::advance_op(
	const string_view::value_type * s, token::value_t t)
{
	return advance_str(s, t, [](string_view::value_type c) {
		return (c != '~') && (c != '!') && (c != '@') && (c != '%')
			&& (c != '^') && (c != '&') && (c != '*') && (c != '(')
			&& (c != ')') && (c != '-') && (c != '=') && (c != '+')
			&& (c != '[') && (c != ']') && (c != '{') && (c != '}')
			&& (c != '|') && (c != ';') && (c != ':') && (c != ',') && (c != '<')
			&& (c != '.') && (c != '>') && (c != '/') && (c != '?');
	});
}

/*
Advance the lexer position and cursor past the given keyword `s`. The keyword
must terminated by a non-alpha character (as Jam keywords are all lowercase).
*/
inline token lexer::advance_kw(
	const string_view::value_type * s, token::value_t t)
{
	return advance_str(s, t, [](string_view::value_type c) {
		return !(('a' <= c) && (c <= 'z'));
	});
}

/*
Advances the lexer position and cursor past a Jam value. Jam values are the
catch-all for tokens. Jam values require special parsing to maintain correct
scoping of quotes and parenthesis. As the values can be quoted strings,
quoted strings with variable expansions in them, plain variable expansions,
or regular strings.
*/
inline token lexer::advance_value()
{
	auto position = input_position;
	auto cursor = cursor_position;
	bool inquote = false;
	std::size_t invarexpand = 0;
	while ((position != input.end())
		&& (inquote || (invarexpand > 0) || !std::isspace(peek(position))))
	{
		auto c = advance(position, cursor);
		if (position == input.end()) break;
		if (c == '"')
			inquote = !inquote;
		else if (c == '\\')
			advance(position, cursor);
		else if ((c == '$' || c == '@') && peek(position + 1) == '(')
		{
			invarexpand += 1;
			advance(position, cursor);
		}
		else if (invarexpand > 0 && c == '(')
			++invarexpand;
		else if (invarexpand > 0 && c == ')')
			--invarexpand;
	}
	if (inquote || invarexpand > 0) return {};
	return make_value_to(position, cursor, token::value_t::value);
}

/*
Advance the lexer position and cursor past a Jam comment. This is for both
line and block comments. The comment markers are included in the resulting
token. For block comments any EOLs in the block are also included.
*/
inline token lexer::advance_comment()
{
	auto position = input_position;
	auto cursor = cursor_position;
	if (peek(position) == '#')
	{
		advance(position, cursor);
		if (peek(position) == '|')
		{
			// #|..|# style comment.
			advance(position, cursor);
			while (peek(position) != '|' || peek(position + 1) != '#')
				advance(position, cursor);
			if (peek(position) == '|' && peek(position + 1) == '#')
			{
				advance(position, cursor);
				advance(position, cursor);
				return make_value_to(position, cursor, token::value_t::comment);
			}
		}
		else
		{
			// #..EOL style comment.
			for (auto c = peek(position); c != '\0' && c != '\n';
				c = peek(position))
				c = advance(position, cursor);
			return make_value_to(position, cursor, token::value_t::comment);
		}
	}
	return {};
}

} // namespace jam_parse

#endif
