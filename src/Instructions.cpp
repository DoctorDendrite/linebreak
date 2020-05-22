/*
 * Actions.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: Andrew
 */

#include "InstructionMap.h"

char NextElement(MachineState & rec)
{
	return *rec.cursor;
}

char CurrentElement(MachineState & rec)
{
	return rec.current_element.top();
}

void NewStreams(MachineState & rec)
{
	rec.line.str("");
	rec.line_len = 0;
	rec.word.str("");
}

void ReplaceCurrentElement(MachineState & rec, char next_element)
{
	rec.current_element.replace(next_element);
}

void ReplaceCurrentElement(MachineState & rec)
{
	ReplaceCurrentElement(rec, NextElement(rec));
}

bool StartGettingLeadingSpace(MachineState & rec)
{
	rec.line.str("");
	rec.space_len = 0;
	ReplaceCurrentElement(rec, DEFAULT_ELEMENT);
	rec.current_element.push(NEXT_WORD);
	rec.current_element.push(LEADING_SPACE);
	return true;
}

bool IgnoreLeadingSpace(MachineState & rec)
{
	rec.line.str("");
	rec.space_len = 0;
	ReplaceCurrentElement(rec, DEFAULT_ELEMENT);
	rec.current_element.push(NEXT_WORD);
	rec.current_element.push(IGNORE_SPACES);
	return true;
}

bool MoveNext(MachineState & rec)
{
	++rec.cursor;
	return true;
}

bool AddToWordAndMoveNext(MachineState & rec)
{
	rec.word << NextElement(rec);
	return MoveNext(rec);
}

bool StopGettingLeadingSpace(MachineState & rec)
{
	rec.leading_space = rec.line.str();
	rec.current_element.pop();
	rec.slack.reset();
	NewStreams(rec);
	return true;
}

bool AddSpaceToLeadingSpace(MachineState & rec)
{
	rec.space_len = rec.space_len + 1;
	rec.line << NextElement(rec);
	rec.slack.decrement();
	return MoveNext(rec);
}

bool AddTabToLeadingSpace(MachineState & rec)
{
	rec.space_len += rec.slack.current();
	rec.slack.reset();
	rec.line << NextElement(rec);
	return MoveNext(rec);
}

bool StartGettingNextWord(MachineState & rec)
{
	ReplaceCurrentElement(rec, NEXT_WORD);
	return true;
}

bool StopGettingNextWord(MachineState & rec)
{
	rec.next_word = rec.word.str();
	rec.current_element.pop();
	return true;
}

bool Excedes(int space_length, int line_length, int word_length, int max_length)
{
	return space_length + line_length + word_length > max_length;
}

bool HyphenAfterNonSpace(MachineState & rec)
{
	//           ....
	// asdf asdf-asdf-
	//          ^    ^
	//          c    n

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 1, rec.str_len)) // '+ 1' from the next hyphen
	{
		// asdf asdf-
		// asdf-

		rec.out << rec.leading_space << rec.line.str() << std::endl;
		rec.line.str("");
		rec.line_len = 0;
	}

	// asdf asdf-asdf-

	rec.line << rec.next_word << NextElement(rec);
	rec.line_len += rec.next_word.length() + 1;

	ReplaceCurrentElement(rec);
	rec.current_element.push(NEXT_WORD);
	rec.word.str("");
	return MoveNext(rec);
}

bool HyphenAfterSpace(MachineState & rec)
{
	//           ....
	// asdf asdf asdf-
	//          ^    ^
	//          c    n

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 2, rec.str_len)) // '+ 2' from the current space and the next hyphen
	{
		// asdf asdf
		// asdf-

		rec.out << rec.leading_space << rec.line.str() << std::endl;
		rec.line.str("");
		rec.line_len = 0;
	}
	else
	{
		// asdf asdf asdf-

		rec.line << " ";
		rec.line_len += 1;
	}

	rec.line << rec.next_word << NextElement(rec);
	rec.line_len += rec.next_word.length() + 1;

	ReplaceCurrentElement(rec);
	rec.current_element.push(NEXT_WORD);
	rec.word.str("");
	return MoveNext(rec);
}

bool SpaceAfterNonSpace(MachineState & rec)
{
	//           ....
	// asdf asdf-asdf
	//          ^    ^
	//          c    n

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length(), rec.str_len)) // '+ 0' nothing to add
	{
		// asdf asdf-
		// asdf

		rec.out << rec.leading_space << rec.line.str() << std::endl;
		rec.line.str("");
		rec.line_len = 0;
	}

	// asdf asdf-asdf

	rec.line << rec.next_word;
	rec.line_len += rec.next_word.length();

	ReplaceCurrentElement(rec);
	rec.current_element.push(IGNORE_SPACES);
	rec.word.str("");
	return true;
}

bool SpaceAfterSpace(MachineState & rec)
{
	//           ....
	// asdf asdf asdf
	//          ^    ^
	//          c    n

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 1, rec.str_len)) // '+ 1' from current space
	{
		// asdf asdf
		// asdf

		rec.out << rec.leading_space << rec.line.str() << std::endl;
		rec.line.str("");
		rec.line_len = 0;
	}
	else
	{
		// asdf asdf asdf

		rec.line << " ";
		rec.line_len += 1;
	}

	rec.line << rec.next_word;
	rec.line_len += rec.next_word.length();

	ReplaceCurrentElement(rec);
	rec.current_element.push(IGNORE_SPACES);
	rec.word.str("");
	return true;
}

//bool WordAfterNonSpace(MachineState & rec)
//{
//	//           ....
//	// asdf asdf-asdf
//	//          ^    ^
//	//          c    n
//
//	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length(), rec.str_len)) // '+ 0' nothing to add
//	{
//		// asdf asdf-
//		// asdf
//
//		rec.out << rec.leading_space << rec.line.str() << std::endl;
//		rec.line.str("");
//		rec.line_len = 0;
//	}
//
//	// asdf asdf-asdf
//
//	rec.line << rec.next_word;
//	rec.line_len += rec.next_word.length();
//
//	ReplaceCurrentElement(rec);
//	rec.current_element.push(NEXT_WORD);
//	rec.word.str("");
//	return true;
//}
//
//bool WordAfterSpace(MachineState & rec)
//{
//	//           ....
//	// asdf asdf asdf
//	//          ^    ^
//	//          c    n
//
//	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 1, rec.str_len)) // '+ 1' from current space
//	{
//		// asdf asdf
//		// asdf
//
//		rec.out << rec.leading_space << rec.line.str() << std::endl;
//		rec.line.str("");
//		rec.line_len = 0;
//	}
//	else
//	{
//		// asdf asdf asdf
//
//		rec.line << " ";
//		rec.line_len += 1;
//	}
//
//	rec.line << rec.next_word;
//	rec.line_len += rec.next_word.length();
//
//	ReplaceCurrentElement(rec);
//	rec.current_element.push(NEXT_WORD);
//	rec.word.str("");
//	return true;
//}

bool NewlineAfterNonSpace(MachineState & rec)
{
	//           ....
	// asdf asdf-asdf
	//          ^    ^
	//          c    n

	rec.out << rec.leading_space << rec.line.str();

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length(), rec.str_len)) // '+ 0' nothing to add
	{
		// asdf asdf-
		// asdf

		rec.out << std::endl << rec.leading_space;
	}

	// asdf asdf-asdf

	rec.out << rec.next_word << std::endl;;
	rec.word.str("");
	(*MachineState::on_leading_space)(rec);
	return MoveNext(rec);
}

bool NewlineAfterSpace(MachineState & rec)
{
	//           ....
	// asdf asdf asdf
	//          ^    ^
	//          c    n

	rec.out << rec.leading_space << rec.line.str();

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 1, rec.str_len)) // '+ 1' from current space
	{
		// asdf asdf
		// asdf

		rec.out << std::endl << rec.leading_space;
	}
	else
	{
		// asdf asdf asdf

		rec.out << " ";
	}

	rec.out << rec.next_word << std::endl;
	rec.word.str("");
	(*MachineState::on_leading_space)(rec);
	return MoveNext(rec);
}

bool NullTerminatorAfterNonSpace(MachineState & rec)
{
	//           ....
	// asdf asdf-asdf
	//          ^    ^
	//          c    n

	rec.out << rec.leading_space << rec.line.str();

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length(), rec.str_len)) // '+ 0' nothing to add
	{
		// asdf asdf-
		// asdf

		rec.out << std::endl << rec.leading_space;
	}

	// asdf asdf-asdf

	rec.out << rec.next_word;
	rec.current_element.pop();
	return false;
}

bool NullTerminatorAfterSpace(MachineState & rec)
{
	//           ....
	// asdf asdf asdf
	//          ^    ^
	//          c    n

	rec.out << rec.leading_space << rec.line.str();

	if (Excedes(rec.space_len, rec.line_len, rec.next_word.length() + 1, rec.str_len)) // '+ 1' from current space
	{
		// asdf asdf
		// asdf

		rec.out << std::endl << rec.leading_space;
	}
	else
	{
		// asdf asdf asdf

		rec.out << " ";
	}

	rec.out << rec.next_word;
	rec.current_element.pop();
	return false;
}
