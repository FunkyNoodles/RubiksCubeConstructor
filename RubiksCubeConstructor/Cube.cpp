#include "Cube.h"

#include <iostream>
#include <random>
#include <math.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#define FOUND -1
#define MAX_STEPS 200 // It shouldn't take more than 200 steps to solve a 3by3 cube

std::vector<int> idaMoves;
long nodesExpanded = 0;

struct CubeCostCompare {
	inline bool operator() (const Cube & o1, const Cube & o2) {
		return (o1.f < o2.f);
	}
};

Cube::Cube(int cubeSize)
{
	this->cubeSize = cubeSize;
	cube = allocateMemory(cubeSize);
	buildMoveFunctions();
}

Cube::Cube(const Cube & c)
{
	this->g = c.g;
	this->f = c.f;
	this->cubeSize = c.getCubeSize();
	cube = allocateMemory(cubeSize);
	buildMoveFunctions();

	std::cout << "Copied" << std::endl;
	if (c.getCube()[0] == 0) {
		std::cout << c.getCube()[0] << std::endl;
	}
	for (int i = 0; i < NUM_FACES; ++i) {
		for (int j = 0; j < cubeSize; ++j) {
			for (int k = 0; k < cubeSize; ++k) {
				//std::cout << i << "\t" << j << "\t" << k << "\t" << c.getCube() << "\t" << c.getCube()[i] << std::endl;
				cube[i][j][k] = c.getCube()[i][j][k];
			}
		}
	}
}

bool Cube::isSolved()
{
	for (int i = 0; i < NUM_FACES; ++i) {
		Cube::Color tmp = cube[i][0][0];
		for (int j = 0; j < cubeSize; ++j) {
			for (int k = 0; k < cubeSize; ++k) {
				if (cube[i][j][k] != tmp) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Cube::operator==(const Cube & cube) const
{
	if (cube.getCubeSize() != this->getCubeSize()) {
		return false;
	}
	for (int i = 0; i < NUM_FACES; ++i) {
		for (int j = 0; j < cube.getCubeSize(); ++j) {
			for (int k = 0; k < cube.getCubeSize(); ++k) {
				if (cube.getCube()[i][j][k] != this->getCube()[i][j][k]) {
					return false;
				}
			}
		}
	}
	return true;
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

void Cube::aStar(Cube & goalState)
{
	std::priority_queue<Cube, std::vector<Cube>, CubeCostCompare> openQueue;
	std::unordered_set<Cube> openSet;
	std::unordered_set<Cube> closedSet;

	openSet.insert(*this);
	openQueue.push(*this);

	while (!openQueue.empty()) {
		Cube current = openQueue.top();
		if (current.isSolved()) {
			return;
		}
		openQueue.pop();
		openSet.erase(current);
		closedSet.insert(current);
		std::vector<Cube> successors = buildSuccessors(current);
		for (int i = 0; i < successors.size(); ++i) {
			Cube s = successors[i];
			//std::cout << s.g << std::endl;
			if (closedSet.count(s) != 0) {
				// The state has already been visited
				continue;
			}
			int tentativeG = current.g + 1;
			if (openSet.count(s) == 0) {
				// Found a new state
				s.printCube();
				openSet.insert(s);
				openQueue.push(s);
			}
			else if (tentativeG >= s.g) {
				continue;
			}
			std::cout << "Move: " <<  i << std::endl;
			s.g = tentativeG;
			s.f = s.g + s.getHeuristic(HeuristicType::MISPLACED, goalState);
		}
	}
}

void Cube::idaStar(Cube & goalState)
{
	int bound = getHeuristic(HeuristicType::MISPLACED, goalState);
	//bound = 20;
	while (true) {
		int t = idaStarSearch(*this, bound, goalState);
		//std::cout << t << std::endl;
		if (t == FOUND) {
			std::cout << "Move number: " << idaMoves.size() << std::endl;
			std::cout << "Nodes expanded: " << nodesExpanded << std::endl;
			for (int i = 0; i < idaMoves.size(); i++) {
				std::cout << idaMoves[i] << std::endl;
			}
			return;
		}
		bound = t;
	}
	
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
	std::cout << "Shuffling cube: " << std::endl;
	for (int i = 0; i < steps; ++i) {
		//std::cout << (int)uni(rng) << std::endl;
		int move = (int)uni(rng);
		std::cout << move << std::endl;
		(this->*moveFunctions[move])();
	}
	std::cout << "Shuffle Finished" << std::endl;
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

int Cube::getCubeSize() const
{
	return this->cubeSize;
}

Cube::Color *** Cube::getCube() const
{
	return this->cube;
}

Cube::~Cube()
{
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize; ++j) {
			delete[] cube[i][j];
			cube[i][j] = nullptr;
		}
		delete[] cube[i];
		cube[i] = nullptr;
	}
	delete[] cube;
	cube = nullptr;
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

std::vector<Cube> Cube::buildSuccessors(const Cube & c)
{
	std::vector<Cube> successors;
	for (int i = 0; i < moveFunctions.size(); ++i) {
		// Create a new cube and rotate it once with a rotation
		Cube s(c);
		s.g = s.g + 1;
		(s.*moveFunctions[i])();
		successors.push_back(s);
	}
	return successors;
}

int Cube::idaStarSearch(Cube & cur, int bound, Cube & goalState)
{
	int f = cur.g + getHeuristic(HeuristicType::MISPLACED, goalState);
	if (f > bound) {
		return f;
	}
	if (cur.isSolved()) {
		return FOUND;
	}
	int min = MAX_STEPS;
	std::vector<Cube> successors = buildSuccessors(cur);
	for (int i = 0; i < successors.size(); ++i) {
		Cube s = successors[i];
		//std::cout << s.g << std::endl;
		idaMoves.push_back(i);
		++nodesExpanded;
		int t = idaStarSearch(s, bound, goalState);
		if (t == FOUND) {
			return FOUND;
		}
		idaMoves.pop_back();
		if (t < min) {
			min = t;
		}
	}
	return min;
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
