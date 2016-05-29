//
//  Subdivide.hpp
//  Mesh
//
//  Created by 奥玛 on 5/27/16.
//  Copyright © 2016 THU. All rights reserved.
//

#ifndef Subdivide_hpp
#define Subdivide_hpp

#include <stdio.h>
#include <vector>
#include "glm.hpp"

struct SubTriangle{
    int v[3];
    glm::vec3 n;
    bool isNew = false;
};

struct SubVertex{
    glm::vec3 p;
    std::vector<int> faces;
};

class Subdivide{
private:
    std::vector<SubTriangle> triangles;
    std::vector<SubVertex> vertices;
    
    std::vector<SubTriangle> newTriangles;
    std::vector<SubVertex> newVertex;
    
    int calculateEdgeVertex(int v1Index, int v2Index);
    void splitTriangleIntoFour(int tIndex);
    SubTriangle createTriangle(int v1Index, int v2Index, int v3Index);
    void calculateNewPosition(int vIndex);
    double getAlpha(int n);
    
    
    int getNextVertex(int tIndex, int vIndex);
public:
    Subdivide();
    void addTriangle(SubTriangle triangle);
    void addVertex(SubVertex vertex);
    void subdivideMesh();
    
    std::vector<SubVertex> getVertices();
    std::vector<SubTriangle> getTriangles();
};

#endif /* Subdivide_hpp */
