#include "Cube.h"

#include <random>
#include <math.h>
#include <queue>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>

#define FOUND -1
#define MAX_STEPS 200 // It shouldn't take more than 200 steps to solve a 3by3 cube

std::vector<int> idaMoves;
long nodesExpanded = 0;

struct CubeCostCompare {
	inline bool operator() (const Cube & o1, const Cube & o2) {
		return (o1.f > o2.f);
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
	this->moveToGetHere = c.moveToGetHere;
	this->cubeSize = c.getCubeSize();
	cube = allocateMemory(cubeSize);
	buildMoveFunctions();

	//std::cout << "Copied" << std::endl;
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

bool Cube::operator<(const Cube & cube) const
{
	return this->f < cube.f;
}

Cube & Cube::operator=(const Cube & cube)
{
	this->g = cube.g;
	this->f = cube.f;
	this->moveToGetHere = cube.moveToGetHere;
	this->cubeSize = cube.getCubeSize();
	this->cube = allocateMemory(cubeSize);
	buildMoveFunctions();

	for (int i = 0; i < NUM_FACES; ++i) {
		for (int j = 0; j < cubeSize; ++j) {
			for (int k = 0; k < cubeSize; ++k) {
				this->cube[i][j][k] = cube.getCube()[i][j][k];
			}
		}
	}
	return *this;
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

float Cube::getHeuristic(HeuristicType heuristicType, Cube & goalCube)
{
	float heuristic = 0;
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
		//heuristic = ceil((float)heuristic / 8);
		heuristic = (float)heuristic / 16;
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
	std::vector<Cube> closedSet;

	openSet.insert(*this);
	openQueue.push(*this);

	while (!openQueue.empty()) {
		Cube current = openQueue.top();
		/*std::cout << "Current cube" << std::endl;
		current.printCube();*/
		if (current.isSolved()) {
			closedSet.push_back(current);
			// Print solution
			std::cout << "Solution:" << std::endl;
			for (Cube c : closedSet) {
				c.printCube();
			}
			std::cout << "Took " << closedSet.size() - 1 << " moves" << std::endl;
			return;
		}
		std::cout << "Expanded: " << current.g << std::endl;
		openQueue.pop();
		openSet.erase(current);
		closedSet.push_back(current);
		std::vector<Cube> successors = buildSuccessors(current);
		for (int i = 0; i < successors.size(); ++i) {
			Cube s = successors[i];
			//s.printCube();
			//std::cout << s.g << std::endl;
			if (std::find(closedSet.begin(), closedSet.end(), s) != closedSet.end()) {
				// The state has already been visited
				continue;
			}
			int tentativeG = current.g + 1;
			if (openSet.count(s) == 0) {
				// Found a new state
				s.g = tentativeG;
				s.f = s.g + s.getHeuristic(HeuristicType::MISPLACED, goalState);
				openSet.insert(s);
				openQueue.push(s);
			}
			else if (tentativeG >= s.g) {
				continue;
			}
			//std::cout << "Move: " <<  i << std::endl;
			//s.g = tentativeG;
			//s.f = s.g + s.getHeuristic(HeuristicType::MISPLACED, goalState);
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

void Cube::printCubeCoords()
{
	std::cout << "--------------- Cube State (Coords) ---------------" << std::endl;
	// First face
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize + 1; ++j) {
			std::cout << " ";
		}
		for (int j = 0; j < cubeSize; ++j) {
			Coord coord = getCoord(4, i, j);
			coord.printCoord();
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
			Coord coord = getCoord(cubeIndex, i, j % cubeSize);
			coord.printCoord();
		}
		std::cout << std::endl;
	}
	// Last face
	for (int i = 0; i < cubeSize; ++i) {
		for (int j = 0; j < cubeSize + 1; ++j) {
			std::cout << " ";
		}
		for (int j = 0; j < cubeSize; ++j) {
			Coord coord = getCoord(5, i, j);
			coord.printCoord();
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
@param - face: the face(0th) index of cube struct
@param - row: the row(1st) index of cube struct
@param - col: the col(2nd) index of cube struct
Maps each color in the cube struct to a 3-D space,
easier to calculate Manhattan heuristic
*/
Cube::Coord Cube::getCoord(int face, int row, int col)
{
	Coord coord;
	switch (face) {
	case static_cast<int>(AbsoluteDirection::FRONT):
		coord.setCoord(col, 0, cubeSize - row - 1);
		break;
	case static_cast<int>(AbsoluteDirection::BACK):
		coord.setCoord(cubeSize - col - 1, cubeSize - 1, cubeSize - row - 1);
		break;
	case static_cast<int>(AbsoluteDirection::LEFT):
		coord.setCoord(0, cubeSize - col - 1, cubeSize - row - 1);
		break;
	case static_cast<int>(AbsoluteDirection::RIGHT):
		coord.setCoord(cubeSize - 1, col, cubeSize - row - 1);
		break;
	case static_cast<int>(AbsoluteDirection::TOP):
		coord.setCoord(col, cubeSize - row - 1, cubeSize - 1);
		break;
	case static_cast<int>(AbsoluteDirection::BOTTOM):
		coord.setCoord(col, row, 0);
		break;
	}
	return coord;
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
@param - c: the starting cube by reference
The helper function will give all possible next states of the cube passed in
*/
std::vector<Cube> Cube::buildSuccessors(const Cube & c)
{
	std::vector<Cube> successors;
	for (int i = 0; i < moveFunctions.size(); ++i) {
		// Create a new cube and rotate it once with a rotation
		Cube s(c);
		//s.g = s.g + 1;
		s.moveToGetHere = i;
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
