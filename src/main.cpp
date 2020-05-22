/*
 * main.cpp
 *
 *  Created on: Jul 20, 2018
 *      Author: Andrew
 */

#include <iostream>

#include "InstructionMap.h"

#define POSIX 1
#define WINDOWS 2

#ifndef PLATFORM
#define PLATFORM WINDOWS
#endif

#if PLATFORM == POSIX
#include <unistd.h>
#define ISATTY(fd) isatty(fd)
#define FILENO(fp) fileno(fp)
#elif PLATFORM == WINDOWS
#include <io.h>
#define ISATTY(fd) _isatty(fd)
#define FILENO(fp) _fileno(fp)
#endif

const int DEFAULT_TAB_SIZE = 4;
const int DEFAULT_LEN = 85;

std::string GetHelpMessage(const std::string & program_name)
{
	return
	"Usage: " + program_name + " [OPTIONS]... \"TEXT\"\n"
	"Print TEXT word-wrapped.\n"
	"Example: " + program_name + " --length=30 \"It's all I have to bring today\"\n"
	"\n"
	"Options:\n"
	"  --length=NUM              set the threshold length for a line in TEXT\n"
	"  --tabsize=NUM             set the number of spaces per tab\n"
	"  --no-leading-space        remove leading space from TEXT\n"
	"\n"
	"Other features in progress:\n"
	"  --ignore-tabs\n"
	"  --tab-to-space\n"
	"  --justify, --balance\n"
	"\n"
	"The default threshold length is 85. The default tab size is 4.\n"
	"The default tab size for most text editors is 4, but it's 8 on most\n"
	"command terminals.\n"
	"\n"
	"Exit status:\n"
	"  1 if the TEXT parameter is an empty string,\n"
	"  2 if the TABSIZE parameter is higher than MAX,\n"
	"  0 otherwise.\n";
}

typedef weak_ptr<submap> submap_ptr;
typedef std::unordered_map<char, submap_ptr> action_map;

bool InputRedirected();
std::string ToString(std::istream & in);

bool RunNext(
		action_map &,
		MachineState &);
void PopulateSubmap(
		submap &,
		instruction_ptr, instruction_ptr, instruction_ptr, instruction_ptr, instruction_ptr);
void BuildMachine(
		action_map &,
		submap &, submap &, submap &, submap &, submap &, submap &);

int main(int argc, const char * argv[])
{
	setlocale(LC_ALL, "");

	size_t tab_size = DEFAULT_TAB_SIZE;
	size_t len = DEFAULT_LEN;
	std::string str;
	std::string word;
	int number;

	for (int i = 1; i < argc; ++i)
	{
		std::stringstream buffer;
		buffer << argv[i];
		getline(buffer, word, '=');

		if (word == "--length")
		{
			buffer >> number;
			len = number >= 0 ? number : DEFAULT_LEN;
		}
		else if (word == "--tabsize")
		{
			buffer >> number;

			if (number > (signed int) MODULAR_INT_CAPACITY)
			{
				std::cout << "Error: Tab size cannot be higher than " << MODULAR_INT_CAPACITY << std::endl;
				return 2;
			}

			tab_size = number >= 0 ? number : DEFAULT_TAB_SIZE;
		}
		else if (word == "--help")
		{
			std::cout << GetHelpMessage(argv[0]);
			return 0;
		}
		else if (word == "--no-leading-space")
		{
			MachineState::on_leading_space = IgnoreLeadingSpace;
		}
		else
		{
			buffer.flush();
			str = buffer.str();
		}
	}

	if (str.length() == 0 && InputRedirected())
	{
		str = ToString(std::cin);
	}

	if (str.length() == 0)
	{
		std::cout << GetHelpMessage(argv[0]);
		return 1;
	}

	action_map machine;

	submap on_hyphens;
	submap on_terminators;
	submap on_newlines;
	submap on_spaces;
	submap on_tabs;

	BuildMachine(
		machine,
		on_hyphens, on_terminators, on_newlines,
		on_spaces, on_tabs, submap_ptr::default_value
	);

	MachineState rec(str, std::cout, len, tab_size, DEFAULT_ELEMENT);
	while (RunNext(machine, rec));
	return 0;
}

bool InputRedirected()
{
	return !ISATTY(FILENO(stdin));
}

std::string ToString(std::istream & in)
{
	std::streambuf * ptr = in.rdbuf();
	std::istreambuf_iterator<char> cursor(ptr);
	std::istreambuf_iterator<char> eos;

	std::ostringstream str_buffer;

	while (cursor != eos)
	   str_buffer << *(cursor++);

	return str_buffer.str();
}

bool RunNext(action_map & machine, MachineState & rec)
{
	return (*(*machine[NextElement(rec)].ptr())[CurrentElement(rec)])(rec);
}

//  // Adapted from the following function
//
//  std::string WordWrap(MachineState & rec)
//  {
//  	char next_element;
//  	bool decision = true;
//
//  	while (decision)
//  	{
//  		next_element = NextElement(rec);
//
//  		switch (next_element)
//  		{
//  		case ' ':
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = AddSpaceToLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = MoveNext(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = StopGettingNextWord(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = SpaceAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = SpaceAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		case '\t':
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = AddTabToLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = MoveNext(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = StopGettingNextWord(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = SpaceAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = SpaceAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		case HYPHEN:
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = StopGettingLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = StartGettingNextWord(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = StopGettingNextWord(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = HyphenAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = HyphenAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		case '\n':
//  		case '\r':
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = StopGettingLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = StartGettingNextWord(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = StopGettingNextWord(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = NewlineAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = NewlineAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		case '\0':
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = StopGettingLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = StartGettingNextWord(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = StopGettingNextWord(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = NullTerminatorAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = NullTerminatorAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		default:
//  			switch (rec.current_element.top())
//  			{
//  			case LEADING_SPACE:
//  				decision = StopGettingLeadingSpace(rec);
//  				break;
//  			case IGNORE_SPACES:
//  				decision = StartGettingNextWord(rec);
//  				break;
//  			case NEXT_WORD:
//  				decision = AddToWordAndMoveNext(rec);
//  				break;
//  			case ' ':
//  			case '\t':
//  				decision = WordAfterSpace(rec);
//  				break;
//  			case '\n':
//  			case '\r':
//  			case '-':
//  				decision = WordAfterNonSpace(rec);
//  				break;
//  			}
//
//  			break;
//  		}
//  	}
//
//  	return rec.out.str();
//  }

void PopulateSubmap(
		submap & map,
		instruction_ptr on_space,
		instruction_ptr on_nonspace,
		instruction_ptr on_leading_space,
		instruction_ptr on_word_start,
		instruction_ptr on_word_end)
{
	map[' '] = on_space;
	map['\t'] = on_space;

	map['\n'] = on_nonspace;
	map['\r'] = on_nonspace;
	map['-'] = on_nonspace;

	map[LEADING_SPACE] = on_leading_space;
	map[IGNORE_SPACES] = on_word_start;
	map[NEXT_WORD] = on_word_end;
}

void BuildMachine(
		action_map & machine,
		submap & on_hyphens,
		submap & on_terminators,
		submap & on_newlines,
		submap & on_spaces,
		submap & on_tabs,
		submap & on_other)
{
	PopulateSubmap(
		on_other,
		SpaceAfterSpace,
		SpaceAfterNonSpace,
		StopGettingLeadingSpace,
		StartGettingNextWord,
		AddToWordAndMoveNext
	);

	PopulateSubmap(
		on_hyphens,
		HyphenAfterSpace,
		HyphenAfterNonSpace,
		StopGettingLeadingSpace,
		StartGettingNextWord,
		StopGettingNextWord
	);

	PopulateSubmap(
		on_terminators,
		NullTerminatorAfterSpace,
		NullTerminatorAfterNonSpace,
		StopGettingLeadingSpace,
		StartGettingNextWord,
		StopGettingNextWord
	);

	PopulateSubmap(
		on_newlines,
		NewlineAfterSpace,
		NewlineAfterNonSpace,
		StopGettingLeadingSpace,
		StartGettingNextWord,
		StopGettingNextWord
	);

	PopulateSubmap(
		on_spaces,
		SpaceAfterSpace,
		SpaceAfterNonSpace,
		AddSpaceToLeadingSpace,
		MoveNext,
		StopGettingNextWord
	);

	PopulateSubmap(
		on_tabs,
		SpaceAfterSpace,
		SpaceAfterNonSpace,
		AddTabToLeadingSpace,
		MoveNext,
		StopGettingNextWord
	);

	machine['-'] = on_hyphens;
	machine['\0'] = on_terminators;
	machine['\n'] = on_newlines;
	machine['\r'] = on_newlines;
	machine[' '] = on_spaces;
	machine['\t'] = on_tabs;
}
