#include <iostream>
#include "BigInteger.h"

int main()
{
	bigint::BigInteger a("11111111111111111111111111111111111111111111111111111111111");
	bigint::BigInteger b("123456789012345678901234567890123456789012345678901234567890");

	std::cout << a + b << std::endl;
	std::cout << a - b << std::endl;
	std::cout << a * b << std::endl;
	std::cout << a / b << std::endl;
	std::cout << a % b << std::endl;

	// factorial 10000
	bigint::BigInteger c("1");
	for (int i = 1; i <= 10000; i++)
	{
		c = c * i;
	}
	std::cout << c << std::endl;
	return 0;
}