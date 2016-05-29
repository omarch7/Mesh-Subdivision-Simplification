//
//  Simplify.hpp
//  Mesh
//
//  Created by 奥玛 on 23/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#ifndef Simplify_hpp
#define Simplify_hpp

#include <stdio.h>
#include <vector>
#include "glm.hpp"
#include "SymmetricMatrix.hpp"

struct Triangle{
    int v[3];
    double err[4];
    int deleted, dirty = 0;
    glm::vec3 n;
};

struct Vertex{
    glm::vec3 p;
    int tStart, tCount;
    SymmetricMatrix q;
    int border;
};

struct Ref{
    int tid, tVertex;
};

class Simplify{
private:
    std::vector<Triangle> triangles;
    std::vector<Vertex> vertices;
    std::vector<Ref> refs;
    
    double vertexError(SymmetricMatrix q, double x, double y, double z);
    double calculateError(int idV1, int idV2, glm::vec3 &pResult);
    bool flipped(glm::vec3 p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted);
    void updateTriangles(int i0, Vertex &v, std::vector<int> &deleted, int &deletedTriangles);
    void updateMesh(int iteration);
    void compactMesh();
    
    double min(double a, double b);
public:
    Simplify();
    std::vector<Triangle>& getTrianglesVector();
    void simplifyMesh(int targetCount, double agressiveness=7);
    void addTriangle(Triangle triangle);
    void addVertex(Vertex vertex);
    
    std::vector<Vertex> getVertices();
    std::vector<Triangle> getTriangles();
};


#endif /* Simplify_hpp */
