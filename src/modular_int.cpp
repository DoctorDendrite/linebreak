/*
 * modular_int.cpp
 *
 *  Created on: Aug 30, 2018
 *      Author: Andrew
 */

#include "modular_int.h"

size_t return_mod(size_t cur, size_t mod)
{
	return mod;
}

size_t return_next(size_t cur, size_t mod)
{
	return cur - 1;
}

modular::modular(size_t modulus):
		_current(modulus),
		_modulus(modulus)
{
	_next_value[0] = return_mod;

	for (size_t i = 1; i < modulus; ++i)
		_next_value[i] = return_next;
}

size_t modular::current() const
{
	return _current;
}

size_t modular::modulus() const
{
	return _modulus;
}

void modular::decrement()
{
	_current = (*_next_value[_current - 1])(_current, _modulus);
}

void modular::reset()
{
	_current = _modulus;
}
