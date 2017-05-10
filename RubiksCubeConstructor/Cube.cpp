#include "Cube.h"

#include <iostream>

#define NUM_FACES 6
#define NUM_SIDES 4



Cube::Cube(int cubeSize)
{
	this->cubeSize = cubeSize;
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
}

void Cube::rotateF()
{
	Color * tmp = new Color[cubeSize];
	// Roate face
	for (int i = 0; i < cubeSize / 2; ++i) {
		for (int j = 0; j < (cubeSize + 1) / 2; ++j) {
			cyclicRoll(cube[0][i][j], cube[0][cubeSize - 1 - j][i], cube[0][cubeSize - 1 - i][cubeSize - 1 - j], cube[0][j][cubeSize - 1 - i]);
		}
	}
	// Rotate side
	for (int i = 0; i < NUM_SIDES; ++i) {

	}
}

void Cube::printCube()
{
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

void Cube::cyclicRoll(Color & a, Color & b, Color & c, Color & d)
{
	Color tmp = a;
	a = b;
	b = c;
	c = d;
	d = tmp;
}
