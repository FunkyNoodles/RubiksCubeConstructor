#pragma once
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
	void rotateF();
	void printCube();
	~Cube();

private:
	int cubeSize;
	Color *** cube;

	void cyclicRoll(Color & a, Color & b, Color & c, Color & d);
};

