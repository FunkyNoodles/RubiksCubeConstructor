#include "Cube.h"

#include <iostream>
#include <random>
#include <math.h>


Cube::Cube(int cubeSize)
{
	this->cubeSize = cubeSize;
	cube = allocateMemory(cubeSize);
	buildMoveFunctions();
}

Cube::Cube(Cube & c)
{
	this->cubeSize = c.getCubeSize();
	cube = allocateMemory(cubeSize);
	buildMoveFunctions();
	for (int i = 0; i < NUM_FACES; ++i) {
		for (int j = 0; j < cubeSize; ++j) {
			for (int k = 0; k < cubeSize; ++k) {
				cube[i][j][k] = c.getCube()[i][j][k];
			}
		}
	}
}

void Cube::rotateF()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::FRONT)][i][j], 
				cube[static_cast<int>(AbsoluteDirection::FRONT)][cubeSize - 1 - j][i], 
				cube[static_cast<int>(AbsoluteDirection::FRONT)][cubeSize - 1 - i][cubeSize - 1 - j], 
				cube[static_cast<int>(AbsoluteDirection::FRONT)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::LEFT)][i][cubeSize - 1];
		cube[static_cast<int>(AbsoluteDirection::LEFT)][i][cubeSize - 1] = cube[static_cast<int>(AbsoluteDirection::BOTTOM)][0][i];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::TOP)][cubeSize - 1][cubeSize - i - 1], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::RIGHT)][cubeSize - i - 1][0], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BOTTOM)][0][cubeSize - i - 1], tmp[cubeSize - i - 1]);
	}
	delete[] tmp;
}

void Cube::rotateFPrime()
{
	rotateF();
	rotateF();
	rotateF();
}

void Cube::rotateB()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::BACK)][i][j],
				cube[static_cast<int>(AbsoluteDirection::BACK)][cubeSize - 1 - j][i],
				cube[static_cast<int>(AbsoluteDirection::BACK)][cubeSize - 1 - i][cubeSize - 1 - j],
				cube[static_cast<int>(AbsoluteDirection::BACK)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::RIGHT)][i][cubeSize - 1];
		cube[static_cast<int>(AbsoluteDirection::RIGHT)][i][cubeSize - 1] = cube[static_cast<int>(AbsoluteDirection::BOTTOM)][cubeSize - 1][cubeSize - 1- i];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::TOP)][0][i], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::LEFT)][cubeSize - i - 1][0], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BOTTOM)][cubeSize - 1][cubeSize - i - 1], tmp[i]);
	}
	delete[] tmp;
}

void Cube::rotateBPrime()
{
	rotateB();
	rotateB();
	rotateB();
}

void Cube::rotateU()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::TOP)][i][j], 
				cube[static_cast<int>(AbsoluteDirection::TOP)][cubeSize - 1 - j][i], 
				cube[static_cast<int>(AbsoluteDirection::TOP)][cubeSize - 1 - i][cubeSize - 1 - j], 
				cube[static_cast<int>(AbsoluteDirection::TOP)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::FRONT)][0][i];
		cube[static_cast<int>(AbsoluteDirection::FRONT)][0][i] = cube[static_cast<int>(AbsoluteDirection::RIGHT)][0][i];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::LEFT)][0][i], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BACK)][0][i], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::RIGHT)][0][i], tmp[i]);
	}
	delete[] tmp;
}

void Cube::rotateUPrime()
{
	rotateU();
	rotateU();
	rotateU();
}

void Cube::rotateD()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::BOTTOM)][i][j],
				cube[static_cast<int>(AbsoluteDirection::BOTTOM)][cubeSize - 1 - j][i],
				cube[static_cast<int>(AbsoluteDirection::BOTTOM)][cubeSize - 1 - i][cubeSize - 1 - j],
				cube[static_cast<int>(AbsoluteDirection::BOTTOM)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::FRONT)][cubeSize - 1][i];
		cube[static_cast<int>(AbsoluteDirection::FRONT)][cubeSize - 1][i] = cube[static_cast<int>(AbsoluteDirection::LEFT)][cubeSize - 1][i];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::RIGHT)][cubeSize - 1][i], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BACK)][cubeSize - 1][i], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::LEFT)][cubeSize - 1][i], tmp[i]);
	}
	delete[] tmp;
}

void Cube::rotateDPrime()
{
	rotateD();
	rotateD();
	rotateD();
}

void Cube::rotateR()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::RIGHT)][i][j],
				cube[static_cast<int>(AbsoluteDirection::RIGHT)][cubeSize - 1 - j][i],
				cube[static_cast<int>(AbsoluteDirection::RIGHT)][cubeSize - 1 - i][cubeSize - 1 - j],
				cube[static_cast<int>(AbsoluteDirection::RIGHT)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::FRONT)][i][cubeSize - 1];
		cube[static_cast<int>(AbsoluteDirection::FRONT)][i][cubeSize - 1] = cube[static_cast<int>(AbsoluteDirection::BOTTOM)][i][cubeSize - 1];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::TOP)][i][cubeSize - 1], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BACK)][cubeSize - 1 - i][0], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BOTTOM)][i][cubeSize - 1], tmp[i]);
	}
	delete[] tmp;
}

void Cube::rotateRPrime()
{
	rotateR();
	rotateR();
	rotateR();
}

void Cube::rotateL()
{
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[static_cast<int>(AbsoluteDirection::LEFT)][i][j],
				cube[static_cast<int>(AbsoluteDirection::LEFT)][cubeSize - 1 - j][i],
				cube[static_cast<int>(AbsoluteDirection::LEFT)][cubeSize - 1 - i][cubeSize - 1 - j],
				cube[static_cast<int>(AbsoluteDirection::LEFT)][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	Color * tmp = new Color[cubeSize];
	for (int i = 0; i < cubeSize; ++i) {
		tmp[i] = cube[static_cast<int>(AbsoluteDirection::FRONT)][i][0];
		cube[static_cast<int>(AbsoluteDirection::FRONT)][i][0] = cube[static_cast<int>(AbsoluteDirection::TOP)][i][0];
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BOTTOM)][i][0], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::BACK)][cubeSize - 1 - i][cubeSize - 1], tmp[i]);
	}
	for (int i = 0; i < cubeSize; ++i) {
		swap(cube[static_cast<int>(AbsoluteDirection::TOP)][i][0], tmp[i]);
	}
	delete[] tmp;
}

void Cube::rotateLPrime()
{
	rotateL();
	rotateL();
	rotateL();
}

int Cube::getHeuristic(HeuristicType heuristicType, Cube & goalCube)
{
	int heuristic = 0;
	switch (heuristicType)
	{
	case HeuristicType::MISPLACED:
		for (int i = 0; i < NUM_FACES; ++i) {
			for (int j = 0; j < cubeSize; ++j) {
				for (int k = 0; k < cubeSize; ++k) {
					if (cube[i][j][k] != goalCube.getCube()[i][j][k]) {
						++heuristic;
					}
				}
			}
		}
		heuristic = ceil((float)heuristic / 21);
		break;
	case HeuristicType::TOTAL_MANHATTAN:
		break;
	case HeuristicType::EDGE_CORNER_MANHATTAN:
		break;
	default:
		break;
	}
	return heuristic;
}

/*
@param - steps: the number of moves the shuffle function will perform
This functions will shuffle the cube @{steps} amount of times
*/
void Cube::shuffle(int steps)
{
	// For random number generator
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, moveFunctions.size() - 1);
	for (int i = 0; i < steps; ++i) {
		//std::cout << (int)uni(rng) << std::endl;
		(this->*moveFunctions[(int)uni(rng)])();
	}
}

void Cube::printCube()
{
	std::cout << "--------------- Cube State ---------------" << std::endl;
	// First face
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize + 1; ++j) {
			std::cout << " ";
		}
		for (int j = 0; j < cubeSize; ++j) {
			std::cout << cube[4][i][j];
		}
		std::cout << std::endl;
	}
	// Four faces
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize * NUM_SIDES; ++j) {
			if (j > 0 && j % cubeSize == 0) {
				std::cout << " ";
			}
			int cubeIndex = (j / 3 + 3) % NUM_SIDES;
			std::cout << cube[cubeIndex][i][j % cubeSize];
		}
		std::cout << std::endl;
	}
	// Last face
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize + 1; ++j) {
			std::cout << " ";
		}
		for (int j = 0; j < cubeSize; ++j) {
			std::cout << cube[5][i][j];
		}
		std::cout << std::endl;
	}
}

int Cube::getCubeSize()
{
	return this->cubeSize;
}

Cube::Color *** Cube::getCube()
{
	return cube;
}

Cube::~Cube()
{
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize; ++j) {
			delete[] cube[i][j];
		}
		delete[] cube[i];
	}
	delete[] cube;
}

/*
@param - a: the first Color by reference
@param - b: the second Color by reference
@param - c: the third Color by reference
@param - d: the fourth Color by reference
The helper function takes four Colors and performs a roll operation,
used to rotating a square matrix clockwise by 90 degrees
*/
void Cube::cyclicRoll(Color & a, Color & b, Color & c, Color & d)
{
	Color tmp = a;
	a = b;
	b = c;
	c = d;
	d = tmp;
}

/*
@param - a: the first Color by reference
@param - b: the second Color by reference
The helper function takes two Colors by refrence and swaps them
*/
void Cube::swap(Color & a, Color & b)
{
	Color tmp = a;
	a = b;
	b = tmp;
}

/* 
@param - cubeSize: the side length of the cube
Helper function for the constructor to allocate memory for
the cube data structure
*/
Cube::Color *** Cube::allocateMemory(int cubeSize)
{
	Color *** cube;
	cube = new Color **[NUM_FACES];
	for (int i = 0; i < NUM_FACES; ++i) {
		cube[i] = new Color *[cubeSize];
		for (int j = 0; j < cubeSize; ++j) {
			cube[i][j] = new Color[cubeSize];
			// Initialize to a solved cube
			for (int k = 0; k < cubeSize; ++k) {
				cube[i][j][k] = static_cast<Color>(i);
			}
		}
	}
	return cube;
}

/*
The helper function will build a list of all the functions
that are used to perform rotations on a cube,
called by the constructors
*/
void Cube::buildMoveFunctions()
{
	moveFunctions.push_back(&Cube::rotateF);
	moveFunctions.push_back(&Cube::rotateFPrime);
	moveFunctions.push_back(&Cube::rotateB);
	moveFunctions.push_back(&Cube::rotateBPrime);
	moveFunctions.push_back(&Cube::rotateU);
	moveFunctions.push_back(&Cube::rotateUPrime);
	moveFunctions.push_back(&Cube::rotateD);
	moveFunctions.push_back(&Cube::rotateDPrime);
	moveFunctions.push_back(&Cube::rotateR);
	moveFunctions.push_back(&Cube::rotateRPrime);
	moveFunctions.push_back(&Cube::rotateL);
	moveFunctions.push_back(&Cube::rotateLPrime);
}
