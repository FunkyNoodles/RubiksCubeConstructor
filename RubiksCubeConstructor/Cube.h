#pragma once

#include <random>
#include <vector>
#include <list>

#define NUM_FACES 6
#define NUM_SIDES 4

enum class AbsoluteDirection
{
	FRONT,
	RIGHT,
	BACK,
	LEFT,
	TOP,
	BOTTOM
};

enum class RelativeDirection
{
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

enum class HeuristicType
{
	MISPLACED,
	TOTAL_MANHATTAN,
	EDGE_CORNER_MANHATTAN
};

class Cube
{
public:
	float f = 0.0;
	int g = 0;
	int moveToGetHere = 0;
	enum Color {
		COLOR0,
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5
	};

	Cube(int cubeSize);
	Cube(const Cube & cube);

	bool isSolved();

	// Overloads
	bool operator==(const Cube & cube) const;
	Cube & operator=(const Cube & cube);

	// Rotations
	void rotateF();
	void rotateFPrime();
	void rotateB();
	void rotateBPrime();
	void rotateU();
	void rotateUPrime();
	void rotateD();
	void rotateDPrime();
	void rotateR();
	void rotateRPrime();
	void rotateL();
	void rotateLPrime();

	// Search
	float getHeuristic(HeuristicType heuristicType, Cube & goalCube);
	void aStar(Cube & goalState);
	void idaStar(Cube & goalState);

	void shuffle(int steps);

	void printCube();

	int getCubeSize() const;
	Color *** getCube() const;
	~Cube();

private:
	int cubeSize;
	Color *** cube;
	std::vector<void(Cube::*) ()> moveFunctions;

	void cyclicRoll(Color & a, Color & b, Color & c, Color & d);
	void swap(Color & a, Color & b);

	std::vector<Cube> buildSuccessors(const Cube & c);

	int idaStarSearch(Cube & cur, int bound, Cube & goalState);

	Color *** allocateMemory(int cubeSize);
	void buildMoveFunctions();
};

//bool operator==(const Cube & lhs, const Cube & rhs) {
//	if (lhs.getCubeSize() != rhs.getCubeSize()) {
//		return false;
//	}
//	for (int i = 0; i < NUM_FACES; ++i) {
//		for (int j = 0; j < lhs.getCubeSize(); ++j) {
//			for (int k = 0; k < lhs.getCubeSize(); ++k) {
//				if (lhs.getCube()[i][j][k] != rhs.getCube()[i][j][k]) {
//					return false;
//				}
//			}
//		}
//	}
//	return true;
//}

namespace std {
	// A very simple hjash function to get going
	template <>
	struct hash<Cube> {
		size_t operator()(const Cube & cube) const noexcept {
			int primes[6] = { 2,3,5,7,11,13 };
			size_t h = 0;
			for (int i = 0; i < NUM_FACES; ++i) {
				for (int j = 0; j < cube.getCubeSize(); ++j) {
					for (int k = 0; k <cube.getCubeSize(); ++k) {
						h += primes[static_cast<int>(cube.getCube()[i][j][k])] * primes[i] * primes[j] * primes[k];
					}
				}
			}
			return h;
		}
	};
}

