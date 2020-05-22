/*
 * Templates.h
 *
 *  Created on: Aug 30, 2018
 *      Author: Andrew
 */

#ifndef TEMPLATES_H_
#define TEMPLATES_H_

const int STACK_CAPACITY = 16;

template <typename T>
class weak_ptr
{
private:
	T * _payload;
public:
	weak_ptr();
    weak_ptr & operator=(T & other);
    T * ptr() const;

    static T default_value;
};

template <typename T>
T weak_ptr<T>::default_value;

template <typename T>
weak_ptr<T>::weak_ptr(): _payload(&weak_ptr<T>::default_value) {}

template <typename T>
weak_ptr<T> & weak_ptr<T>::operator=(T & other)
{
	_payload = &other;
	return *this;
}

template <typename T>
T * weak_ptr<T>::ptr() const
{
	return _payload;
}

template <typename T>
class tiny_stack
{
private:
	T _payload[STACK_CAPACITY];
	int _cursor;
	static T _empty_stack_symbol;
public:
	tiny_stack(const T & empty_stack_symbol);
	const T & top() const;
	void push(const T & element);
	void pop();
	void replace(const T & element);
	bool empty() const;
	int size() const;
};

template <typename T>
T tiny_stack<T>::_empty_stack_symbol;

template <typename T>
tiny_stack<T>::tiny_stack(const T & empty_stack_symbol): _cursor(0)
{
	_empty_stack_symbol = empty_stack_symbol;
}

template <typename T>
const T & tiny_stack<T>::top() const
{
	return _payload[_cursor];
}

template <typename T>
void tiny_stack<T>::push(const T & element)
{
	_payload[++_cursor] = element;
}

template <typename T>
void tiny_stack<T>::pop()
{
	_cursor = _cursor - 1;
}

template <typename T>
void tiny_stack<T>::replace(const T & element)
{
	_payload[_cursor] = element;
}

template <typename T>
bool tiny_stack<T>::empty() const
{
	return _cursor == 0;
}

template <typename T>
int tiny_stack<T>::size() const
{
	return _cursor + 1;
}

#endif /* TEMPLATES_H_ */
