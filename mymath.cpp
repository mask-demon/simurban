#include "mymath.h"

#include <cmath>

const int g_nsigma = 2;
const int g_fineness = 1024;
double g_memory[g_nsigma * g_fineness];

void init_mymath() {
	for(int i = 0; i < g_nsigma * g_fineness; i++) g_memory[i] = -1;  // Negative: Not yet calculated
}

double gauss(double x) {
	x = std::abs(x);
	if(x > g_nsigma) return 0;
	int n = x * g_fineness;
	if(n >= g_nsigma * g_fineness) return 0;
	if(g_memory[n] < 0) g_memory[n] = exp(((double)n / g_fineness) * ((double)n / g_fineness) / 2);
	return g_memory[n];
}

inline int mod(int n, int m) {
	return n >= 0? n % m: -((-n) % m);
}
