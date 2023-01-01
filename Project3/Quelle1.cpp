#include <iostream>
#include <intrin.h>

int main()
{
	__m128d v1 = { 1.0, 2.0 };
	__m128d v2 = { 3.0, 4.0 };
	__m128d v3 = _mm_add_pd(v1, v2);
	double d[2];
	_mm_storeu_pd(d, v3);
	std::cout << "Value: " << d[0] << std::endl;
}