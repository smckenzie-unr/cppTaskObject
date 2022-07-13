#include <cstdlib>
#include <iostream>
#include <fstream>

#include "taskNode.h"

using namespace std;

template <typename T>
void add_numbers(T& A, const T& B, const T& C) noexcept
{
	A = B + C;
}

void multiply_integers(signed int& A, const signed int& B, const signed int& C) noexcept
{
	A = B * C;
}

int main(void)
{
	taskNode* _taskNode(new taskNode);
	double A(0.0), B(3.14), C(2.71);
	signed int _A(0), _B(15), _C(15);

	cout << "Raw ID: " << _taskNode->get_rawID() << " Set ID: " << _taskNode->get_ID() << endl;

	_taskNode->install_standalone_task(&add_numbers<double>, A, B, C);
	_taskNode->run_task();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	cout << "Number A: " << A << endl;

	B *= 2.0;
	C *= 2.0;

	_taskNode->run_task();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	cout << "Number A: " << A << endl;

	_taskNode->install_standalone_task(&multiply_integers, _A, _B, _C);

	for (int idx = 0; idx < 50; idx++)
	{
		_taskNode->run_task();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		cout << "Number _A: " << _A << endl;

		_B += 2;
	}

	std::this_thread::sleep_for(std::chrono::seconds(15));

	delete _taskNode;
	_taskNode = nullptr;

	return 0;
}