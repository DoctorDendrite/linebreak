/*
 * ActionMap.cpp
 *
 *  Created on: Aug 30, 2018
 *      Author: Andrew
 */

#include "InstructionMap.h"

MachineState::MachineState(
	const std::string & unprocessed_text,
	std::ostream & output_stream,
	size_t line_threshold,
	size_t spaces_per_tab,
	char first_element):
		str(unprocessed_text.c_str()),
		cursor(str),
		out(output_stream),
		str_len(line_threshold),
		space_len(0),
		line_len(0),
		slack(spaces_per_tab),
		current_element(EMPTY_STACK_SYMBOL)
{
	current_element.push(first_element);
	(*on_leading_space)(*this);
}

instruction_ptr MachineState::on_leading_space = StartGettingLeadingSpace;
