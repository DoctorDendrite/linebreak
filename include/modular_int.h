/*
 * modular_int.h
 *
 *  Created on: Aug 30, 2018
 *      Author: Andrew
 */

#ifndef MODULAR_INT_H_
#define MODULAR_INT_H_

#include <stdio.h>

const size_t MODULAR_INT_CAPACITY = 16;

class modular
{
private:
	typedef size_t (*_integer_return)(size_t, size_t);
	_integer_return _next_value[MODULAR_INT_CAPACITY];
	size_t _current;
	size_t _modulus;
public:
	modular() = delete;
	modular(size_t modulus);
	size_t current() const;
	size_t modulus() const;
	void decrement();
	void reset();
};

#endif /* MODULAR_INT_H_ */
