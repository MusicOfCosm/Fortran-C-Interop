#include <random>
#include <ctime>

unsigned int seed = (unsigned int)time(0);
std::mt19937 gen(seed);

int randint(int min, int max) {
	std::uniform_int_distribution<int> rdint(min, max);
	return rdint(gen);
}

float uniform(float min, float max) {
	std::uniform_real_distribution<float> rdfloat(min, max);
	return rdfloat(gen);
}