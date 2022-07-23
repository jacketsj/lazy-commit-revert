#include "lcr.h"

#include <iostream>
#include <vector>

using namespace lcrt;

// simple test template function
template <typename T> void test_equal_ln(const T& a, const T& b, auto line) {
	static int test_no = 0;
	std::cout << "TEST " << ++test_no << ":";
	if (a == b)
		std::cout << "PASSED";
	else
		std::cout << "FAILED, line=" << line << ", " << a << " != " << b;
	std::cout << std::endl;
}

#define test_equal(a, b) test_equal_ln(a, b, __LINE__)

// do testing in main
int main() {
	// option 1: manually create a handler
	operation_handler handler;
	// option 1a: assign handler with constructor
	lcr<int> my_int(handler, 0);
	// option 1b: assign handler after (can be changed later)
	lcr<int> my_other_int(3);
	my_other_int.assign(handler);

	test_equal(my_int(), 0);
	test_equal(my_other_int(), 3);
	handler.commit();
	test_equal(my_int(), 0);
	test_equal(my_other_int(), 3);
	my_int() = -4;
	my_other_int() = 2;
	test_equal(my_int(), -4);
	test_equal(my_other_int(), 2);
	handler.revert();
	test_equal(my_int(), 0);
	test_equal(my_other_int(), 3);
	my_int() = -4;
	my_other_int() = 2;
	handler.commit();
	my_int() = 3;
	handler.revert();
	my_other_int() = 12;
	handler.commit();
	test_equal(my_other_int(), 12);
	my_other_int() = 3;
	test_equal(my_other_int(), 3);
	test_equal(my_int(), -4);
	handler.revert();
	test_equal(my_other_int(), 12);
	test_equal(my_int(), -4);

	// instead of manual operation_handler, use global variant
	lcr<int> my_lcr_int;
	// global variant is useful for vector, since it has a default constructor
	std::vector<lcr<int>> my_lcr_vec(10);
	global_handler.commit();
	my_lcr_vec[0]() = 2;
	my_lcr_vec[1]() = 3;
	my_lcr_vec[2]() = 4;
	my_lcr_vec[6]() = -10;
	global_handler.commit();
	my_lcr_vec[0]() = 12;
	my_lcr_vec[4]() = 8;
	my_lcr_vec[5]() = 9;
	my_lcr_vec.emplace_back(6);
	global_handler.revert();
	test_equal(my_lcr_vec[0](), 2);
	test_equal(my_lcr_vec[4](), 0);
	my_lcr_vec[0]() = 6;
	my_lcr_vec[4]() = 21;
	global_handler.commit();
	test_equal(my_lcr_vec[10](), 6);
	test_equal(my_lcr_vec[1](), 3);
	my_lcr_vec[2]() = 21;
	global_handler.revert();
	test_equal(my_lcr_vec[1](), 3);
	test_equal(my_lcr_vec[2](), 4);
}
