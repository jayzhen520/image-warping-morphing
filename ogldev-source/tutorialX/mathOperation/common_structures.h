#ifndef COMMON_STRUCTURES_H
#define COMMON_STRUCTURES_H

#include "Matrices.h"

#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif

// TYPES //////////////////////////////////////////////////
class VERTEX2D
{
public:
	VERTEX2D();
	VERTEX2D(REAL x, REAL y);
	REAL x;
	REAL y;

};

class EDGE
{
public:
	EDGE();
	VERTEX2D v1;
	VERTEX2D v2;
};

class TRIANGLE
{
public:
	int i1;
	int i2;
	int i3;
	TRIANGLE *pNext;
	TRIANGLE *pPrev;
public:
	TRIANGLE();
	~TRIANGLE();
};

class MESH
{
public:
	int vertex_num;
	int triangle_num;

	VERTEX2D * pVerArr; // point to outer vertices arrary
	TRIANGLE * pTriArr; // point to outer triangles arrary
public:
	MESH(int vertexNum, int triangleNum);
	MESH();
	~MESH();
};

//每一个三角形对应一组（两个）矩阵
class TRIANGLE_MATRIX
{
public:
	//三角形的索引
	unsigned short index;
	//此三角形对应的变换矩阵
	Matrix3 originTransMatrix;
	Matrix3 targetTransMatrix;



};

class MESH_MATRIX
{
public:
	MESH * middleMesh;
	
	TRIANGLE_MATRIX * triangle_matrix;
public:
	MESH_MATRIX();
	~MESH_MATRIX();
	
};

#endif