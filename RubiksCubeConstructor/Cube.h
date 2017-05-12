#pragma once

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

class Cube
{
public:
	enum Color {
		COLOR0,
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5
	};
	Cube(int cubeSize);
	Cube(Cube & cube);

	// Rotations
	void rotateF();
	void rotateFPrime();
	void rotateU();
	void rotateUPrime();

	void printCube();

	int getCubeSize();
	Color *** getCube();
	~Cube();

private:
	int cubeSize;
	Color *** cube;

	void cyclicRoll(Color & a, Color & b, Color & c, Color & d);
	void swap(Color & a, Color & b);

	void allocateMemory(int cubeSize);
};

