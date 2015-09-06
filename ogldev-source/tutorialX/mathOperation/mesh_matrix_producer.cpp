//
// File: Delaunay.cpp
// Description: Delaunay class to triangluate points set in 2D. 
// TODO: The procedure uses Double List for holding data, it can be optimized by using another data structure such as DAG, Quad-edge, etc.
// Author: SoRoMan
// Date: 05/10/2007
// Version: 1.0
// History: 
//

// INCLUDES ///////////////////////////////////////////////
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "windows.h" // for time statistics

//ADDITION INCLUDES
#include <cassert>
#include "Matrices.h"

#include "mesh_matrix_producer.h"


#include "common_structures.h"

// DEFINES ////////////////////////////////////////////////
#define MAX_VERTEX_NUM 4092



// PROTOTYPES ///////////////////////////////////////////
// Delaunay triangulation functions
void InitMesh(MESH * pMesh, int ver_num);
void UnInitMesh(MESH * pMesh);

void AddBoundingBox(MESH * pMesh);
void RemoveBoundingBox(MESH * pMesh);
void IncrementalDelaunay(MESH * pMesh);

void Insert(MESH * pMesh, int ver_index);
bool FlipTest(MESH * pMesh, TRIANGLE * pTestTri);

REAL InCircle(VERTEX2D * pa, VERTEX2D * pb, VERTEX2D * pp, VERTEX2D *  pd);
REAL InTriangle(MESH * pMesh, VERTEX2D * pVer, TRIANGLE * pTri);

void InsertInTriangle(MESH * pMesh, TRIANGLE * pTargetTri, int ver_index);
void InsertOnEdge(MESH * pMesh, TRIANGLE * pTargetTri, int ver_index);

// Helper functions
void RemoveTriangleNode(MESH * pMesh, TRIANGLE * pTri);
TRIANGLE * AddTriangleNode(MESH * pMesh, TRIANGLE * pPrevTri, int i1, int i2, int i3);

// I/O functions
void Input(const char* pFile, MESH * pMesh);
void Output(const char* pFile, MESH * pMesh);

// GLOBALS ////////////////////////////////////////////////

//获得中间的Mesh
MESH * middleMeshCreate(MESH * originMesh, MESH * targetMesh, REAL originWeight);
//生成由中间转换到origin和target的逆矩阵
MESH_MATRIX * triangleMatrixCreate(MESH * originMesh, MESH * targetMesh, float originWeight);
//根据像素点的位置查找响应的矩阵
TRIANGLE_MATRIX * findTriangle_MATRIX(VERTEX2D point);
//判断点是否处于多边形中
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
//计算MESH中triangle_num值
void meshTriangleNumCount(MESH * const mesh);
//将坐标转换成OpenGL的坐标系的坐标
void mesh_for_opengl(MESH * mesh);

void mesh_init(const char * originPath, const char * targetPath, MESH * originMesh, MESH * targetMesh){
	if(originPath == NULL || targetPath == NULL){
		fprintf(stderr, "Usage:%s InputFileName OutputFileName\n");
		exit(1);
	}

	//获取图片的特征点，构建Delaunay三角剖分。
	Input(originPath, originMesh);
	IncrementalDelaunay(originMesh);
	mesh_for_opengl(originMesh);
	meshTriangleNumCount(originMesh);
	Input(targetPath, targetMesh);
	IncrementalDelaunay(targetMesh);
	mesh_for_opengl(targetMesh);
	meshTriangleNumCount(targetMesh);
}

// FUNCTIONS //////////////////////////////////////////////
MESH_MATRIX * middle_mesh_matrix_producer(MESH * originMesh, MESH * targetMesh, const float originWeight)
{
	//生成TRIANGLE_MATRIX矩阵组，用于顶点查询出使用哪组（2个）矩阵
	MESH_MATRIX * mesh_matrix = triangleMatrixCreate(originMesh, targetMesh, originWeight);
	return mesh_matrix;
}

void mesh_for_opengl(MESH * mesh){
	for(int i = 3; i < mesh->vertex_num + 3; i++){
		mesh->pVerArr[i].y = 1.0 - mesh->pVerArr[i].y;
	}
}

void meshTriangleNumCount(MESH * const mesh){
	TRIANGLE * trianglePtr = mesh->pTriArr;
	int count = 0;
	while(trianglePtr != NULL){
		trianglePtr = trianglePtr->pNext;
		count++;
	}
	mesh->triangle_num = count;
}

MESH_MATRIX * triangleMatrixCreate(MESH * originMesh, MESH * targetMesh, float originWeight){
	//构建中间mesh
	MESH * mMesh = middleMeshCreate(originMesh, targetMesh, originWeight);
	//构建origin,middle,target矩阵
	Matrix3 * middleMatrix3 = new Matrix3[mMesh->triangle_num];
	Matrix3 * middleMatrix3_2 = new Matrix3[mMesh->triangle_num];
	Matrix3 * originMatrix3 = new Matrix3[originMesh->triangle_num];
	Matrix3 * targetMatrix3 = new Matrix3[targetMesh->triangle_num];
	
	TRIANGLE * middleTrianglePTR = mMesh->pTriArr;
	TRIANGLE * originTrianglePTR = originMesh->pTriArr;
	TRIANGLE * targetTrianglePTR = targetMesh->pTriArr;

	float point[3];
	for(int i = 0; i < mMesh->triangle_num; i++){
		point[0] = mMesh->pVerArr[middleTrianglePTR->i1].x;
		point[1] = mMesh->pVerArr[middleTrianglePTR->i1].y;
		point[2] = 1;
		middleMatrix3[i].setRow(0, point);
		middleMatrix3_2[i].setRow(0, point);
		point[0] = mMesh->pVerArr[middleTrianglePTR->i2].x;
		point[1] = mMesh->pVerArr[middleTrianglePTR->i2].y;
		point[2] = 1;
		middleMatrix3[i].setRow(1, point);
		middleMatrix3_2[i].setRow(1, point);
		point[0] = mMesh->pVerArr[middleTrianglePTR->i3].x;
		point[1] = mMesh->pVerArr[middleTrianglePTR->i3].y;
		point[2] = 1;
		middleMatrix3[i].setRow(2, point);
		middleMatrix3_2[i].setRow(2, point);

		point[0] = originMesh->pVerArr[originTrianglePTR->i1].x;
		point[1] = originMesh->pVerArr[originTrianglePTR->i1].y;
		point[2] = 1;
		originMatrix3[i].setRow(0, point);
		point[0] = originMesh->pVerArr[originTrianglePTR->i2].x;
		point[1] = originMesh->pVerArr[originTrianglePTR->i2].y;
		point[2] = 1;
		originMatrix3[i].setRow(1, point);
		point[0] = originMesh->pVerArr[originTrianglePTR->i3].x;
		point[1] = originMesh->pVerArr[originTrianglePTR->i3].y;
		point[2] = 1;
		originMatrix3[i].setRow(2, point);

		point[0] = targetMesh->pVerArr[originTrianglePTR->i1].x;
		point[1] = targetMesh->pVerArr[originTrianglePTR->i1].y;
		point[2] = 1;
		targetMatrix3[i].setRow(0, point);
		point[0] = targetMesh->pVerArr[originTrianglePTR->i2].x;
		point[1] = targetMesh->pVerArr[originTrianglePTR->i2].y;
		point[2] = 1;
		targetMatrix3[i].setRow(1, point);
		point[0] = targetMesh->pVerArr[originTrianglePTR->i3].x;
		point[1] = targetMesh->pVerArr[originTrianglePTR->i3].y;
		point[2] = 1;
		targetMatrix3[i].setRow(2, point);

		middleTrianglePTR = middleTrianglePTR->pNext;
		originTrianglePTR = originTrianglePTR->pNext;
		targetTrianglePTR = targetTrianglePTR->pNext;
	}
	//构建MESH_MATRIX矩阵
	MESH_MATRIX * mesh_matrix_ptr = new MESH_MATRIX();
	mesh_matrix_ptr->middleMesh = mMesh;
	mesh_matrix_ptr->triangle_matrix = new TRIANGLE_MATRIX[mMesh->triangle_num];
	for(int i = 0; i < mMesh->triangle_num; i++){
		mesh_matrix_ptr->triangle_matrix[i].index = i;
		mesh_matrix_ptr->triangle_matrix[i].originTransMatrix = middleMatrix3[i].invert() * originMatrix3[i];
		mesh_matrix_ptr->triangle_matrix[i].targetTransMatrix = middleMatrix3_2[i].invert() * targetMatrix3[i];
	}
	return mesh_matrix_ptr;
}

TRIANGLE_MATRIX * findTriangle_MATRIX(const MESH_MATRIX * mesh_matrix_ptr, const MESH * mMesh, const VERTEX2D point){

	TRIANGLE * meshTrianglePtr = mMesh->pTriArr;

	for(int i = 0; i < mMesh->triangle_num; i++){
		float vertx[3];
		float verty[3];
		for(int i = 0; i < 3; i++){
			vertx[i] = mMesh->pVerArr[meshTrianglePtr->i1].x;
			verty[i] = mMesh->pVerArr[meshTrianglePtr->i1].y;
		}
		if(!(pnpoly(3, vertx, verty, point.x, point.y) % 2)){
			return &(mesh_matrix_ptr->triangle_matrix[i]);
		}else{
			meshTrianglePtr = meshTrianglePtr->pNext;
		}
	}
	return NULL;
}

int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i, j, c = 0;
	for (i = 0, j = nvert-1; i < nvert; j = i++) {
	if ( ((verty[i]>testy) != (verty[j]>testy)) &&
		(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
		c = !c;
	}
	return c;
}

MESH * middleMeshCreate(MESH * originMesh, MESH * targetMesh, REAL originWeight){
	MESH * middleMesh;
	if(originMesh->triangle_num != targetMesh->triangle_num
		|| originMesh->vertex_num != targetMesh->vertex_num)
	{
		assert(false);
		return NULL;
	}

	middleMesh = new MESH();
	middleMesh->triangle_num = originMesh->triangle_num;
	middleMesh->vertex_num = originMesh->vertex_num;
	middleMesh->pVerArr = new VERTEX2D[middleMesh->vertex_num + 3];
	//middleMesh->pTriArr = (TRIANGLE *)malloc(middleMesh->triangle_num * sizeof(TRIANGLE));
	TRIANGLE * originTrianglePTR = originMesh->pTriArr;
	TRIANGLE * middleTrianglePTR;// = middleMesh->pTriArr;

	//填充数组
	for(int i = 0; i < middleMesh->vertex_num + 3; i++){
		middleMesh->pVerArr[i].x = originMesh->pVerArr[i].x * originWeight + targetMesh->pVerArr[i].x * (1 - originWeight);
		middleMesh->pVerArr[i].y = originMesh->pVerArr[i].y * originWeight + targetMesh->pVerArr[i].y * (1 - originWeight);
	}
	//填充双链表
	int k = 1;
	TRIANGLE * trianglePtr;
	for(int i = 0; i < middleMesh->triangle_num; i++){
		trianglePtr = new TRIANGLE();
		//trianglePtr = (TRIANGLE *)malloc(sizeof(TRIANGLE));
		trianglePtr->i1 = originTrianglePTR->i1;
		trianglePtr->i2 = originTrianglePTR->i2;
		trianglePtr->i3 = originTrianglePTR->i3;
		if(k == 1){
			middleMesh->pTriArr = trianglePtr;
			trianglePtr->pPrev = NULL;
		}else{
			middleTrianglePTR->pNext = trianglePtr;
			trianglePtr->pPrev = middleTrianglePTR;
		}
		middleTrianglePTR = trianglePtr;
		originTrianglePTR = originTrianglePTR->pNext;
		k++;
	}
	return middleMesh;
}

// The format of input file should be as follows:
// The First Line: amount of vertices (the amount of vertices/points needed to be triangulated)
// Other Lines: x y z (the vertices/points coordinates, z should be 0 for 2D)
// E.g. 
// 4
// -1 -1 0
// -1 1 0
// 1 1 0
// 1 -1 0
void Input(const char* pFile, MESH * pMesh)
{
	FILE* fp = fopen(pFile, "r");

	if (!fp)
	{
		fprintf(stderr,"Error:%s open failed\n", pFile);
		exit(1);
	}

	//int face;
	int amount;

	//fscanf( fp, "%d", &face);
	fscanf( fp, "%d", &amount);
	if (amount < 3)
	{
		fprintf(stderr,"Error:vertex amount should be greater than 2, but it is %d \n",amount);
		exit(1);
	}

	InitMesh(pMesh, amount);

	REAL x,y,z;
	for ( int j=3; j<amount+3; ++j)
	{
		fscanf( fp, "%lg %lg %lg", &x, &y, &z);
		((VERTEX2D *)(pMesh->pVerArr+j))->x = x;
		((VERTEX2D *)(pMesh->pVerArr+j))->y = y;
	}


	fclose(fp);
}

// Algorithm IncrementalDelaunay(V)
// Input: 由n个点组成的二维点集V
// Output: Delaunay三角剖分DT
//	1.add a appropriate triangle boudingbox to contain V ( such as: we can use triangle abc, a=(0, 3M), b=(-3M,-3M), c=(3M, 0), M=Max({|x1|,|x2|,|x3|,...} U {|y1|,|y2|,|y3|,...}))
//	2.initialize DT(a,b,c) as triangle abc
//	3.for i <- 1 to n 
//		do (Insert(DT(a,b,c,v1,v2,...,vi-1), vi))   
//	4.remove the boundingbox and relative triangle which cotains any vertex of triangle abc from DT(a,b,c,v1,v2,...,vn) and return DT(v1,v2,...,vn).
void IncrementalDelaunay(MESH * pMesh)
{
	// Add a appropriate triangle boudingbox to contain V
	AddBoundingBox(pMesh);

	// Get a vertex/point vi from V and Insert(vi)
	for (int i=3; i<pMesh->vertex_num+3; i++)
	{
		Insert(pMesh, i);
	}

	// Remove the bounding box
	RemoveBoundingBox(pMesh);
}

// The format of output file should be as follows:
// triangle index
// x1 y1 (the coordinate of first vertex of triangle)
// x2 y2 (the coordinate of second vertex of triangle)
// x3 y3 (the coordinate of third vertex of triangle)
void Output(const char* pFile, MESH * pMesh)
{
	FILE* fp = fopen(pFile, "w");
	if (!fp)
	{
		fprintf(stderr,"Error:%s open failed\n", pFile);

		UnInitMesh(pMesh);
		exit(1);
	}

	TRIANGLE * pTri = pMesh->pTriArr;
	int* pi;
	int vertex_index;
	int tri_index = 0;
	while(pTri != NULL)	
	{
		fprintf(fp, "Triangle: %d\n", ++tri_index);

		pi = &(pTri->i1);
		for (int j=0; j<3; j++)	
		{	
			vertex_index = *pi++;		
			fprintf(fp, "%lg %lg\n", ((VERTEX2D *)(pMesh->pVerArr+vertex_index))->x, ((VERTEX2D *)(pMesh->pVerArr+vertex_index))->y);
		}

		pTri = pTri->pNext;
	}

	fclose(fp);

	UnInitMesh(pMesh);
}


// Allocate memory to store vertices and triangles
void InitMesh(MESH * pMesh, int ver_num )
{
	// Allocate memory for vertex array
	pMesh->pVerArr = (VERTEX2D *)malloc((ver_num+3)*sizeof(VERTEX2D));
	if (pMesh->pVerArr == NULL)
	{
		fprintf(stderr,"Error:Allocate memory for mesh failed\n");
		exit(1);
	}

	pMesh->vertex_num = ver_num;

}

// Deallocate memory
void UnInitMesh(MESH * pMesh)
{
	// free vertices
	if(pMesh->pVerArr != NULL)
		free(pMesh->pVerArr);

	// free triangles
	TRIANGLE * pTri = pMesh->pTriArr;
	TRIANGLE * pTemp = NULL;
	while (pTri != NULL)
	{
		pTemp = pTri->pNext;
		free(pTri);
		pTri = pTemp;
	}
}

void AddBoundingBox(MESH * pMesh)
{
	REAL max = 0;
	REAL max_x = 0;
	REAL max_y = 0;
	REAL t;

	for (int i=3; i<pMesh->vertex_num+3; i++)
	{
		t = abs(((VERTEX2D *)(pMesh->pVerArr+i))->x);
		if (max_x < t)
		{
			max_x = t;
		}

		t = abs(((VERTEX2D *)(pMesh->pVerArr+i))->y);
		if (max_y < t)
		{
			max_y = t;
		}
	}

	max = max_x > max_y ? max_x:max_y;

	//TRIANGLE box;
	//box.v1 = VERTEX2D(0, 3*max);
	//box.v2 = VERTEX2D(-3*max, 3*max);
	//box.v3 = VERTEX2D(3*max, 0);

	VERTEX2D v1(0, 4*max);
	VERTEX2D v2(-4*max, -4*max);
	VERTEX2D v3(4*max, 0);

	// Assign to Vertex array
	*(pMesh->pVerArr) = v1;
	*(pMesh->pVerArr + 1) = v2;
	*(pMesh->pVerArr + 2) = v3;

	// add the Triangle boundingbox
	AddTriangleNode(pMesh, NULL, 0, 1, 2);
}

void RemoveBoundingBox(MESH * pMesh)
{
	int statify[3]={0,0,0};
	int vertex_index;
	int* pi;
	int k = 1;
	
	// Remove the first triangle-boundingbox
	//pMesh->pTriArr = pMesh->pTriArr->pNext;
	//pMesh->pTriArr->pPrev = NULL; // as head

	TRIANGLE * pTri = pMesh->pTriArr;
	TRIANGLE * pNext = NULL;
	while (pTri != NULL)
	{
		pNext = pTri->pNext;

		statify[0] = 0;
		statify[1] = 0;
		statify[2] = 0;

		pi = &(pTri->i1);
		for (int j=0, k = 1; j<3; j++, k*= 2)
		{			
			vertex_index = *pi++;

			if(vertex_index == 0 || vertex_index == 1 || vertex_index == 2) // bounding box vertex
			{
				statify[j] = k;
			}
		}

		switch(statify[0] | statify[1] | statify[2] )
		{
		case 0: // no statify
			break;
		case 1:
		case 2:
		case 4: // 1 statify, remove 1 triangle, 1 vertex
			RemoveTriangleNode(pMesh, pTri);		
			break;
		case 3:
		case 5:
		case 6: // 2 statify, remove 1 triangle, 2 vertices
			RemoveTriangleNode(pMesh, pTri);			
			break;
		case 7: // 3 statify, remove 1 triangle, 3 vertices
			RemoveTriangleNode(pMesh, pTri);
			break;
		default:
			break;
		}

		// go to next item
		pTri = pNext;
	}
}


// Return a positive value if the points pa, pb, and
// pc occur in counterclockwise order; a negative
// value if they occur in clockwise order; and zero
// if they are collinear. The result is also a rough
// approximation of twice the signed area of the
// triangle defined by the three points.
REAL CounterClockWise(VERTEX2D * pa, VERTEX2D * pb, VERTEX2D * pc)
{
	return ((pb->x - pa->x)*(pc->y - pb->y) - (pc->x - pb->x)*(pb->y - pa->y));
}

// Adjust if the point lies in the triangle abc
REAL InTriangle(MESH * pMesh, VERTEX2D * pVer, TRIANGLE * pTri)
{
	int vertex_index;
	VERTEX2D * pV1, *pV2, *pV3;

	vertex_index =pTri->i1;
	pV1 = (VERTEX2D *)(pMesh->pVerArr+vertex_index);
	vertex_index =pTri->i2;
	pV2 = (VERTEX2D *)(pMesh->pVerArr+vertex_index);
	vertex_index =pTri->i3;
	pV3 = (VERTEX2D *)(pMesh->pVerArr+vertex_index);

	REAL ccw1 = CounterClockWise(pV1, pV2, pVer);
	REAL ccw2 = CounterClockWise(pV2, pV3, pVer);
	REAL ccw3 = CounterClockWise(pV3, pV1, pVer);

	REAL r = -1;
	if (ccw1>0 && ccw2>0 && ccw3>0)
	{
		r = 1;
	}
	else if(ccw1*ccw2*ccw3 == 0 && (ccw1*ccw2 > 0 || ccw1*ccw3 > 0 || ccw2*ccw3 > 0) )
	{
		r = 0;
	}

	return r;
}

// Algorithm Insert(DT(a,b,c,v1,v2,...,vi-1), vi)
// 1.find the triangle vavbvc which contains vi // FindTriangle()
// 2.if (vi located at the interior of vavbvc)  
// 3.    then add triangle vavbvi, vbvcvi and vcvavi into DT // UpdateDT()
// FlipTest(DT, va, vb, vi)
// FlipTest(DT, vb, vc, vi)
// FlipTest(DT, vc, va, vi)
// 4.else if (vi located at one edge (E.g. edge vavb) of vavbvc) 
// 5.    then add triangle vavivc, vivbvc, vavdvi and vivdvb into DT (here, d is the third vertex of triangle which contains edge vavb) // UpdateDT()
// FlipTest(DT, va, vd, vi)
// FlipTest(DT, vc, va, vi)
// FlipTest(DT, vd, vb, vi)
// FlipTest(DT, vb, vc, vi)
// 6.return DT(a,b,c,v1,v2,...,vi)
void Insert(MESH * pMesh, int ver_index)
{
	VERTEX2D * pVer = (VERTEX2D *)(pMesh->pVerArr+ver_index);
	TRIANGLE * pTargetTri = NULL;
	TRIANGLE * pEqualTri1 = NULL;
	TRIANGLE * pEqualTri2 = NULL;

	int j = 0;
	TRIANGLE * pTri = pMesh->pTriArr;
	while (pTri != NULL)
	{
		REAL r = InTriangle(pMesh, pVer, pTri);
		if(r > 0) // should be in triangle
		{
			pTargetTri = pTri;
		}
		else if (r == 0) // should be on edge
		{
			if(j == 0)
			{
				pEqualTri1 = pTri;
				j++;				
			}
			else
			{
				pEqualTri2 = pTri;
			}
			
		}

		pTri = pTri->pNext;
	}

	if (pEqualTri1 != NULL && pEqualTri2 != NULL)
	{
		InsertOnEdge(pMesh, pEqualTri1, ver_index);
		InsertOnEdge(pMesh, pEqualTri2, ver_index);
	}
	else
	{
		InsertInTriangle(pMesh, pTargetTri, ver_index);
	}
}

void InsertInTriangle(MESH * pMesh, TRIANGLE * pTargetTri, int ver_index)
{
	int index_a, index_b, index_c;
	TRIANGLE * pTri = NULL;
	TRIANGLE * pNewTri = NULL;

	pTri = pTargetTri;	
	if(pTri == NULL)
	{
		return;
	}

	// Inset p into target triangle
	index_a = pTri->i1;
	index_b = pTri->i2;
	index_c = pTri->i3;

	// Insert edge pa, pb, pc
	for(int i=0; i<3; i++)
	{
		// allocate memory
		if(i == 0)
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_a, index_b, ver_index);
		}
		else if(i == 1)
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_b, index_c, ver_index);
		}
		else
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_c, index_a, ver_index);
		}

		// go to next item
		if (pNewTri != NULL)
		{
			pTri = pNewTri;
		}
		else
		{
			pTri = pTri;
		}
	}

	// Get the three sub-triangles
	pTri = pTargetTri;	
	TRIANGLE * pTestTri[3];
	for (int i=0; i< 3; i++)
	{
		pTestTri[i] = pTri->pNext;

		pTri = pTri->pNext;
	}

	// remove the Target Triangle
	RemoveTriangleNode(pMesh, pTargetTri);

	for (int i=0; i< 3; i++)
	{
		// Flip test
		FlipTest(pMesh, pTestTri[i]);
	}
}

void InsertOnEdge(MESH * pMesh, TRIANGLE * pTargetTri, int ver_index)
{
	int index_a, index_b, index_c;
	TRIANGLE * pTri = NULL;
	TRIANGLE * pNewTri = NULL;

	pTri = pTargetTri;	
	if(pTri == NULL)
	{
		return;
	}

	// Inset p into target triangle
	index_a = pTri->i1;
	index_b = pTri->i2;
	index_c = pTri->i3;

	// Insert edge pa, pb, pc
	for(int i=0; i<3; i++)
	{
		// allocate memory
		if(i == 0)
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_a, index_b, ver_index);
		}
		else if(i == 1)
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_b, index_c, ver_index);
		}
		else
		{
			pNewTri = AddTriangleNode(pMesh, pTri, index_c, index_a, ver_index);
		}		

		// go to next item
		if (pNewTri != NULL)
		{
			pTri = pNewTri;
		}
		else
		{
			pTri = pTri;
		}
	}

	// Get the two sub-triangles
	pTri = pTargetTri;	
	TRIANGLE * pTestTri[2];
	for (int i=0; i< 2; i++)
	{
		pTestTri[i] = pTri->pNext;
		pTri = pTri->pNext;
	}

	// remove the Target Triangle
	RemoveTriangleNode(pMesh, pTargetTri);

	for (int i=0; i< 2; i++)
	{
		// Flip test
		FlipTest(pMesh, pTestTri[i]);
	}
}

// Precondition: the triangle satisfies CCW order
// Algorithm FlipTest(DT(a,b,c,v1,v2,...,vi), va, vb, vi)
// 1.find the third vertex (vd) of triangle which contains edge vavb // FindThirdVertex()
// 2.if(vi is in circumcircle of abd)  // InCircle()
// 3.    then remove edge vavb, add new edge vivd into DT // UpdateDT()
//		  FlipTest(DT, va, vd, vi)
//		  FlipTest(DT, vd, vb, vi)

bool FlipTest(MESH * pMesh, TRIANGLE * pTestTri)
{
	bool flipped = false;

	int index_a = pTestTri->i1;
	int index_b = pTestTri->i2;
	int index_p = pTestTri->i3;

	int statify[3]={0,0,0};
	int vertex_index;
	int* pi;
	int k = 1;

	// find the triangle which has edge consists of start and end
	TRIANGLE * pTri = pMesh->pTriArr;

	int index_d = -1;
	while (pTri != NULL)
	{
		statify[0] = 0;
		statify[1] = 0;
		statify[2] = 0;

		pi = &(pTri->i1);
		for (int j=0, k = 1; j<3; j++, k*= 2)
		{
			vertex_index = *pi++;
			if(vertex_index == index_a || vertex_index == index_b)
			{
				statify[j] = k;
			}
		}

		switch(statify[0] | statify[1] | statify[2] )
		{
			case 3:
				if(CounterClockWise((VERTEX2D *)(pMesh->pVerArr+index_a), (VERTEX2D *)(pMesh->pVerArr+index_b), (VERTEX2D *)(pMesh->pVerArr+pTri->i3)) < 0)
				{
					index_d = pTri->i3;
				}
				
				break;
			case 5:
				if(CounterClockWise((VERTEX2D *)(pMesh->pVerArr+index_a), (VERTEX2D *)(pMesh->pVerArr+index_b), (VERTEX2D *)(pMesh->pVerArr+pTri->i2)) < 0)
				{
					index_d = pTri->i2;
				}
				
				break;
			case 6: 
				if(CounterClockWise((VERTEX2D *)(pMesh->pVerArr+index_a), (VERTEX2D *)(pMesh->pVerArr+index_b), (VERTEX2D *)(pMesh->pVerArr+pTri->i1)) < 0)
				{
					index_d = pTri->i1;
				}

				break;

			default:
				break;
		}

		if (index_d != -1)
		{
			VERTEX2D * pa = (VERTEX2D *)(pMesh->pVerArr+index_a);
			VERTEX2D * pb = (VERTEX2D *)(pMesh->pVerArr+index_b);
			VERTEX2D * pd = (VERTEX2D *)(pMesh->pVerArr+index_d);
			VERTEX2D * pp = (VERTEX2D *)(pMesh->pVerArr+index_p);
			
			if(InCircle( pa, pb, pp, pd) < 0) // not local Delaunay
			{
				flipped = true;

				// add new triangle adp,  dbp, remove abp, abd.
				// allocate memory for adp
				TRIANGLE * pT1 = AddTriangleNode(pMesh, pTestTri, pTestTri->i1, index_d, pTestTri->i3);				
				// allocate memory for dbp
				TRIANGLE * pT2 = AddTriangleNode(pMesh, pT1, index_d, pTestTri->i2, index_p);				
				// remove abp
				RemoveTriangleNode(pMesh, pTestTri);
				// remove abd
				RemoveTriangleNode(pMesh, pTri);

				FlipTest(pMesh, pT1); // pNewTestTri satisfies CCW order
				FlipTest(pMesh, pT2); // pNewTestTri2  satisfies CCW order

				break;
			}			
		}

		// go to next item	
		pTri = pTri->pNext;
	}

	return flipped;
}

// In circle test, use vector cross product
REAL InCircle(VERTEX2D * pa, VERTEX2D * pb, VERTEX2D * pp, VERTEX2D *  pd)
{
	REAL det;
	REAL alift, blift, plift, bdxpdy, pdxbdy, pdxady, adxpdy, adxbdy, bdxady;

	REAL adx = pa->x - pd->x;
	REAL ady = pa->y - pd->y;

	REAL bdx = pb->x - pd->x;
	REAL bdy = pb->y - pd->y;

	REAL pdx = pp->x - pd->x;
	REAL pdy = pp->y - pd->y;

	bdxpdy = bdx * pdy;
	pdxbdy = pdx * bdy;
	alift = adx * adx + ady * ady;

	pdxady = pdx * ady;
	adxpdy = adx * pdy;
	blift = bdx * bdx + bdy * bdy;

	adxbdy = adx * bdy;
	bdxady = bdx * ady;
	plift = pdx * pdx + pdy * pdy;

	det = alift * (bdxpdy - pdxbdy)
		+ blift * (pdxady - adxpdy)
		+ plift * (adxbdy - bdxady);

	return -det;
}

// Remove a node from the triangle list and deallocate the memory
void RemoveTriangleNode(MESH * pMesh, TRIANGLE * pTri)
{
	if (pTri == NULL)
	{
		return;
	}

	// remove from the triangle list
	if (pTri->pPrev != NULL)
	{
		pTri->pPrev->pNext = pTri->pNext;
	}
	else // remove the head, need to reset the root node
	{
		pMesh->pTriArr = pTri->pNext;
	}

	if (pTri->pNext != NULL)
	{
		pTri->pNext->pPrev = pTri->pPrev;
	}

	// deallocate memory
	free(pTri);	
}

// Create a new node and add it into triangle list
TRIANGLE * AddTriangleNode(MESH * pMesh, TRIANGLE * pPrevTri, int i1, int i2, int i3)
{
	// test if 3 vertices are co-linear
	if(CounterClockWise((VERTEX2D *)(pMesh->pVerArr+i1), (VERTEX2D *)(pMesh->pVerArr+i2), (VERTEX2D *)(pMesh->pVerArr+i3)) == 0)
	{
		return NULL;
	}

	// allocate memory
	TRIANGLE * pNewTestTri = (TRIANGLE *)malloc(sizeof(TRIANGLE));

	pNewTestTri->i1 = i1;
	pNewTestTri->i2 = i2;
	pNewTestTri->i3 = i3;

	// insert after prev triangle
	if (pPrevTri == NULL) // add root
	{
		pMesh->pTriArr = pNewTestTri;
		pNewTestTri->pNext = NULL;
		pNewTestTri->pPrev = NULL;
	}
	else
	{
		pNewTestTri->pNext = pPrevTri->pNext;
		pNewTestTri->pPrev = pPrevTri;

		if(pPrevTri->pNext != NULL)
		{
			pPrevTri->pNext->pPrev = pNewTestTri;
		}

		pPrevTri->pNext = pNewTestTri;
	}
	
	return pNewTestTri;
}

