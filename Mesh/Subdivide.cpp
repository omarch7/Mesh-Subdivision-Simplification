//
//  Subdivide.cpp
//  Mesh
//
//  Created by 奥玛 on 5/27/16.
//  Copyright © 2016 THU. All rights reserved.
//

#include <iostream>
#include "Subdivide.hpp"

Subdivide::Subdivide(){
    vertices = std::vector<SubVertex>();
    triangles = std::vector<SubTriangle>();
    newTriangles = std::vector<SubTriangle>();
}

void Subdivide::addVertex(SubVertex vertex){
    vertices.emplace_back(vertex);
}

void Subdivide::addTriangle(SubTriangle triangle){
    triangles.emplace_back(triangle);
    int tIndex = (int)triangles.size() - 1;
    for (int i = 0; i < 3; i++) {
        vertices[triangles[tIndex].v[i]].faces.emplace_back(tIndex);
    }
}

void Subdivide::insertCenterTriangles(int tIndex){
    SubTriangle &t = triangles[tIndex];
    int edgesVertices[3];
    for (int i = 0; i < 3; i++) {
        edgesVertices[i] =  calculateEdgeVertex(t.v[i], t.v[(i+1)%3]);
    }
    SubTriangle centerTriangle = createTriangle(edgesVertices[0], edgesVertices[1], edgesVertices[2]);
    triangles.emplace_back(centerTriangle);
}

void Subdivide::insertCornerTriangles(int tIndex, int numTriangles){
    SubTriangle &original = triangles[tIndex];
    SubTriangle &center = triangles[tIndex + numTriangles];
    triangles.emplace_back(createTriangle(original.v[1], center.v[1], center.v[0]));
    triangles.emplace_back(createTriangle(original.v[2], center.v[2], center.v[1]));
    original = createTriangle(original.v[0], center.v[0], center.v[2]);
}

SubTriangle Subdivide::createTriangle(int v1Index, int v2Index, int v3Index){
    SubTriangle triangle;
    triangle.v[0] = v1Index;
    triangle.v[1] = v2Index;
    triangle.v[2] = v3Index;
    triangle.n = glm::normalize(glm::cross(vertices[v2Index].p - vertices[v1Index].p, vertices[v3Index].p - vertices[v1Index].p));
    triangle.isNew = true;
    return triangle;
}

int Subdivide::calculateEdgeVertex(int v1Index, int v2Index){
    SubVertex &v1 = vertices[v1Index];
    SubVertex &v2 = vertices[v2Index];
    SubVertex nV;
    nV.p = glm::vec3((v1.p.x+v2.p.x)/2, (v1.p.y + v2.p.y) / 2, (v1.p.z + v2.p.z) / 2);
    vertices.emplace_back(nV);
    return (int)vertices.size() - 1;
}

//int Subdivide::calculateEdgeVertex(int v1Index, int v2Index, int v3Index, int tIndex){
//    
//    int neighborFace = getNeighbor(v1Index, v2Index, tIndex);
//    int vIndex;
//    if (neighborFace > tIndex) {
//        SubVertex &nV = vertices[getThirdVertex(v1Index, v2Index, tIndex)];
//        glm::vec3 edgePart = vertices[v1Index].p + vertices[v2Index].p;
//        glm::vec3 neighborPart = nV.p + vertices[v3Index].p;
//        SubVertex eV;
//        eV.p = ((edgePart * (3.0f/8.0f)) + (neighborPart * (1.0f/8.0f)));
//        vIndex = (int)vertices.size();
//        vertices.emplace_back(eV);
//    }else{
//        SubTriangle &t = triangles[neighborFace + numTriangles];
//        SubTriangle &nT = triangles[neighborFace];
//        
//        if (nT.v[0] == v2Index) {
//            vIndex = t.v[0];
//        }else if (nT.v[1] == v2Index){
//            vIndex = t.v[1];
//        }else{
//            vIndex = t.v[2];
//        }
//    }
//    return vIndex;
//}


int Subdivide::getNeighbor(int v1Index, int v2Index, int tIndex){
    int neighbor = 0;
    SubVertex &v1 = vertices[v1Index];
    SubVertex &v2 = vertices[v2Index];
    for (int i = 0; i < v1.faces.size(); i++) {
        for (int j = 0; j < v2.faces.size(); j++) {
            if ((v1.faces[i] == v2.faces[j]) && v1.faces[i] != tIndex) {
                neighbor = v1.faces[i];
                break;
            }
        }
    }
    return neighbor;
}

int Subdivide::getThirdVertex(int v1Index, int v2Index, int tIndex){
    int index = 0;
    bool found = false;
    SubTriangle &t = triangles[tIndex];
    
    while ((index < 2) && !found) {
        if ((t.v[index] == v1Index) || (t.v[index] == v2Index)) {
            index++;
        }else{
            found = true;
        }
    }
    return t.v[index];
}


void Subdivide::subdivideMesh(){
    for (int i = 0; i < vertices.size(); i++) {
        calculateNewPosition(i);
    }
    numTriangles = (int)triangles.size();
    for (int i = 0; i < numTriangles; i++) {
        insertCenterTriangles(i);
    }
    for (int i = 0; i < numTriangles; i++) {
        insertCornerTriangles(i, numTriangles);
    }
}

void Subdivide::calculateNewPosition(int vIndex){
    SubVertex &vertex = vertices[vIndex];
    double alpha = getAlpha((int)vertex.faces.size());
    vertex.p = glm::vec3(vertex.p.x * (1.0f - ((double)vertex.faces.size() * alpha)), vertex.p.y * (1.0f - ((double)vertex.faces.size() * alpha)), vertex.p.z * (1.0f - ((double)vertex.faces.size() * alpha)));
    SubVertex newVertex;
    for (int i = 0; i < vertex.faces.size(); i++) {
        int eIndex = getNextVertex(vertex.faces[i], vIndex);
        newVertex.p += vertices[eIndex].p;
    }
    newVertex.p = glm::vec3(newVertex.p.x * alpha, newVertex.p.y * alpha, newVertex.p.z * alpha);
    vertex.p += newVertex.p;
}

int Subdivide::getNextVertex(int tIndex, int vIndex){
    int eIndex = 0;
    if (triangles[tIndex].v[0]==vIndex) {
        eIndex = triangles[tIndex].v[2];
    }else if (triangles[tIndex].v[1]==vIndex){
        eIndex = triangles[tIndex].v[0];
    }else{
        eIndex = triangles[tIndex].v[1];
    }
    return eIndex;
}

double Subdivide::getAlpha(int n){
    double alpha;
    if (n > 3) {
        double center = (0.375 + (0.25 * cos((2.0f * M_PI) / (double) n)));
        alpha = (0.625 - (center * center)) / double(n);
    }else{
        alpha = 3.0f / 16.0f;
    }
    return alpha;
}

std::vector<SubVertex> Subdivide::getVertices(){
    return vertices;
}

std::vector<SubTriangle> Subdivide::getTriangles(){
    return triangles;
}