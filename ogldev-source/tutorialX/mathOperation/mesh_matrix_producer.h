#ifndef MESH_MATRIX_PRODUCER_H
#define MESH_MATRIX_PRODUCER_H

#include "common_structures.h"

// DEFINES ////////////////////////////////////////////////
#define MAX_VERTEX_NUM 4092



// PROTOTYPES ///////////////////////////////////////////
// Delaunay triangulation functions
void InitMesh(MESH_PTR pMesh, int ver_num);
void UnInitMesh(MESH_PTR pMesh);

void AddBoundingBox(MESH_PTR pMesh);
void RemoveBoundingBox(MESH_PTR pMesh);;
void IncrementalDelaunay(MESH_PTR pMesh);

void Insert(MESH_PTR pMesh, int ver_index);
bool FlipTest(MESH_PTR pMesh, TRIANGLE * pTestTri);

REAL InCircle(VERTEX2D_PTR pa, VERTEX2D_PTR pb, VERTEX2D_PTR pp, VERTEX2D_PTR  pd);
REAL InTriangle(MESH_PTR pMesh, VERTEX2D_PTR pVer, TRIANGLE * pTri);

void InsertInTriangle(MESH_PTR pMesh, TRIANGLE * pTargetTri, int ver_index);
void InsertOnEdge(MESH_PTR pMesh, TRIANGLE * pTargetTri, int ver_index);

// Helper functions
void RemoveTriangleNode(MESH_PTR pMesh, TRIANGLE * pTri);
TRIANGLE * AddTriangleNode(MESH_PTR pMesh, TRIANGLE * pPrevTri, int i1, int i2, int i3);

// I/O functions
void Input(char* pFile, MESH_PTR pMesh);
void Output(char* pFile, MESH_PTR pMesh);

// GLOBALS ////////////////////////////////////////////////

//����м��Mesh
MESH * middleMeshCreate(MESH * originMesh, MESH * targetMesh, REAL originWeight);
//�������м�ת����origin��target�������
MESH_MATRIX * triangleMatrixCreate(MESH * originMesh, MESH * targetMesh, float originWeight);
//�������ص��λ�ò�����Ӧ�ľ���
TRIANGLE_MATRIX * findTriangle_MATRIX(VERTEX2D point);
//�жϵ��Ƿ��ڶ������
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
//����MESH��triangle_numֵ
void meshTriangleNumCount(MESH * const mesh);

int mesh_matrix_producer(const char * originPath, const char * targetPath);

#endif