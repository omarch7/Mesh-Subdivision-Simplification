//
//  Mesh.hpp
//  Mesh
//
//  Created by 奥玛 on 23/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <vector>
#include "glm.hpp"
#include "Simplify.hpp"
#include "Subdivide.hpp"


struct MeshVertex{
    glm::vec3 p;
};

struct MeshTriangle{
    int v[3];
    glm::vec3 n;
    bool isNew = false;
};

class Mesh{
private:
    std::vector<MeshVertex> vertices;
    std::vector<MeshTriangle> triangles;
    void calculateNormals();
public:
    Mesh();
    bool loadMeshFromFile(std::string filename);
    int getTrianglesCount();
    glm::vec3& getVertex(int i, int j);
    glm::vec3& getNormal(int i);
    bool isNewTriangle(int i);
    void simplifyTenPercent();
    void subdivideOneStep();
};


#endif /* Mesh_hpp */
