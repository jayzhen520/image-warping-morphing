#ifndef MATRIX_DATABASE_H
#define MATRIX_DATABASE_H

#include "common_structures.h"
#include "Matrices.h"

class MatrixDatabase{
private:
	//
	MESH * originMesh;
	MESH * targetMesh;
	MESH * middleMesh;
	TRIANGLE_MATRIX_PAIR * triangle_matrix_pair;
public:
	MatrixDatabase();
	~MatrixDatabase();

	Matrix3 getOriginMatrix(VERTEX2D v2d);

	Matrix3 getTargetMatrix(VERTEX2D v2d);
};

#endif