/*
 * ActionMap.h
 *
 *  Created on: Aug 20, 2018
 *      Author: Andrew
 */

#ifndef ACTIONMAP_H_
#define ACTIONMAP_H_

#include <sstream>
#include <string>
#include <unordered_map>

#include "modular_int.h"
#include "Templates.h"

const char LEADING_SPACE = 'A';
const char IGNORE_SPACES = 'B';
const char NEXT_WORD = 'C';
const char DEFAULT_ELEMENT = '-';
const char EMPTY_STACK_SYMBOL = 'E';

struct MachineState;
typedef bool (*instruction_ptr)(MachineState & rec);
typedef std::unordered_map<char, instruction_ptr> submap;

struct MachineState
{
	const char * str;
	const char * cursor;
	std::string leading_space;

	std::ostringstream line;
	std::ostringstream word;

	std::ostream & out;

	size_t str_len;
	size_t space_len;
	size_t line_len;

	modular slack;

	tiny_stack<char> current_element;
	std::string next_word;

	MachineState() = delete;
	MachineState(
		const std::string & unprocessed_text,
		std::ostream & output_stream,
		size_t line_threshold,
		size_t spaces_per_tab,
		char first_element);

	static instruction_ptr on_leading_space;
};

char CurrentElement(MachineState & rec);
char NextElement(MachineState & rec);
bool StartGettingLeadingSpace(MachineState & rec);
bool IgnoreLeadingSpace(MachineState & rec);

bool MoveNext(MachineState & rec);
bool AddToWordAndMoveNext(MachineState & rec);
bool HyphenAfterNonSpace(MachineState & rec);
bool HyphenAfterSpace(MachineState & rec);
bool SpaceAfterNonSpace(MachineState & rec);
bool SpaceAfterSpace(MachineState & rec);
// bool WordAfterNonSpace(MachineState & rec);
// bool WordAfterSpace(MachineState & rec);
bool NewlineAfterNonSpace(MachineState & rec);
bool NewlineAfterSpace(MachineState & rec);
bool NullTerminatorAfterNonSpace(MachineState & rec);
bool NullTerminatorAfterSpace(MachineState & rec);
bool StartGettingNextWord(MachineState & rec);
bool StopGettingNextWord(MachineState & rec);
bool StopGettingLeadingSpace(MachineState & rec);
bool AddSpaceToLeadingSpace(MachineState & rec);
bool AddTabToLeadingSpace(MachineState & rec);

#endif /* ACTIONMAP_H_ */
