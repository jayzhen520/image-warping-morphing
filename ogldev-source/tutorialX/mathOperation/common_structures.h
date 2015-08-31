#ifndef COMMON_STRUCTURES_H
#define COMMON_STRUCTURES_H

#include "Matrices.h"

#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif

// TYPES //////////////////////////////////////////////////
typedef struct VERTEX2D_TYP
{
	REAL x;
	REAL y;

} VERTEX2D, *VERTEX2D_PTR;

typedef struct EDGE_TYP
{
	VERTEX2D v1;
	VERTEX2D v2;

} EDGE, *EDGE_PTR;

//typedef struct TRIANGLE_TYP
//{
//	int i1; // vertex index
//	int i2; 
//	int i3; 
//
//	TRIANGLE_TYP* pNext;
//	TRIANGLE_TYP* pPrev;
//
//
//
//} TRIANGLE, *TRIANGLE_PTR;

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

typedef struct MESH_TYP
{
	int vertex_num;
	int triangle_num;

	VERTEX2D_PTR pVerArr; // point to outer vertices arrary
	TRIANGLE * pTriArr; // point to outer triangles arrary

} MESH, *MESH_PTR;

//每一个三角形对应一组（两个）矩阵
typedef struct TRIANGLE_MATRIX_PAIR
{
	//三角形的索引
	unsigned short index;
	//此三角形对应的变换矩阵
	Matrix3 originTransMatrix;
	Matrix3 targetTransMatrix;
} TRIANGLE_MATRIX, *TRIANGLE_MATRIX_PTR;

typedef struct MESH_MATRIX_TYP
{
	MESH * middleMesh;
	
	TRIANGLE_MATRIX * triangle_matrix;

}MESH_MATRIX, MESH_MATRIX_PTR;

#endif