#include "common_structures.h"

VERTEX2D::VERTEX2D()
	:x(0.0), y(0.0){
}

VERTEX2D::VERTEX2D(REAL realX, REAL realY)
	:x(realX), y(realY){
}

EDGE::EDGE()
	:v1(VERTEX2D()), v2(VERTEX2D()){
}

TRIANGLE::TRIANGLE():
	i1(0), i2(0), i3(0),
	pPrev(NULL), pNext(NULL)
{
}

TRIANGLE::~TRIANGLE(){
}

MESH::MESH()
	:vertex_num(0), triangle_num(0), pVerArr(NULL), pTriArr(NULL){
}

MESH::MESH(int vertexNum, int triangleNum){
	vertex_num = vertexNum;
	triangle_num = triangleNum;
	pVerArr = new VERTEX2D[vertex_num];
	//这里没有对pTriArr初始化，现在为NULL;
	pTriArr = NULL;
	//pTriArr = new TRIANGLE[triangle_num];
}

MESH::~MESH(){
	TRIANGLE * triNode;
	for(int i = 0; i < triangle_num - 1; i++){
		triNode = pTriArr;
		pTriArr = triNode->pNext;
		triNode->pNext->pPrev = pTriArr;
		delete triNode;
	}
	delete pTriArr;

	delete [] pVerArr;
}

MESH_MATRIX::MESH_MATRIX(){
}

MESH_MATRIX::~MESH_MATRIX(){
	delete middleMesh;
	delete [] triangle_matrix;
}