//
//  Mesh.cpp
//  Mesh
//
//  Created by 奥玛 on 23/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "Mesh.hpp"

Mesh::Mesh(){
    vertices = std::vector<MeshVertex>();
    triangles = std::vector<MeshTriangle>();
}

bool Mesh::loadMeshFromFile(std::string filename){
    std::ifstream file;
    
    file.open(filename);
    
    if(file.is_open()){
        std::string line;
        while (getline(file, line)) {
            if(line.substr(0, 2) == "v "){
                std::istringstream s(line.substr(2));
                MeshVertex vertex;
                s >> vertex.p.x;
                s >> vertex.p.y;
                s >> vertex.p.z;
                vertices.emplace_back(vertex);
            }else if(line.substr(0, 2) == "f "){
                std::istringstream s(line.substr(2));
                MeshTriangle triangle;
                s >> triangle.v[0];
                s >> triangle.v[1];
                s >> triangle.v[2];
                triangle.v[0]--;
                triangle.v[1]--;
                triangle.v[2]--;
                triangle.n = glm::normalize(glm::cross(vertices[triangle.v[1]].p - vertices[triangle.v[0]].p, vertices[triangle.v[2]].p - vertices[triangle.v[0]].p));
                triangles.emplace_back(triangle);
            }
        }
        return true;
    }else{
        return false;
    }
}

void Mesh::simplifyTenPercent(){
    Simplify *simplify = new Simplify();
    for (int i = 0; i < vertices.size(); i++) {
        Vertex vertex;
        vertex.p = vertices[i].p;
        simplify->addVertex(vertex);
    }
    
    for (int i = 0; i < triangles.size(); i++) {
        Triangle triangle;
        for (int j = 0; j < 3; j++) {
            triangle.v[j] = triangles[i].v[j];
        }
        simplify->addTriangle(triangle);
    }
    std::cout << "Size Vectors " << vertices.size() << std::endl;
    std::cout << "Size Triangles " << triangles.size() << std::endl;
    
    simplify->simplifyMesh(triangles.size() * 0.90);
    
    
    vertices.clear();
    triangles.clear();
    

    for (int i = 0; i < simplify->getVertices().size(); i++) {
        MeshVertex vertex;
        vertex.p = simplify->getVertices()[i].p;
        vertices.emplace_back(vertex);
    }
    
    std::cout << "Size New Vectors " << vertices.size() << std::endl;
    
    std::vector<Triangle> newTriangles = simplify->getTriangles();
    
    std::cout << "Size New Triangles " << newTriangles.size() << std::endl;
    
    for (int i = 0; i < newTriangles.size(); i++){
        MeshTriangle triangle;
        triangle.n = newTriangles[i].n;
        for(int j = 0; j < 3; j++){
            triangle.v[j] =  newTriangles[i].v[j];
        }
        triangles.emplace_back(triangle);
    }
    
    delete simplify;
}

void Mesh::subdivideOneStep(){
    Subdivide *subdivide = new Subdivide();
    
    for (int i = 0; i < vertices.size(); i++) {
        SubVertex vertex;
        vertex.p = vertices[i].p;
        subdivide->addVertex(vertex);
    }
    
    for (int i = 0; i < triangles.size(); i++) {
        SubTriangle triangle;
        for (int j = 0; j < 3; j++) {
            triangle.v[j] = triangles[i].v[j];
        }
        triangle.n = triangles[i].n;
        subdivide->addTriangle(triangle);
    }
    
    std::cout << "Size Vectors " << vertices.size() << std::endl;
    std::cout << "Size Triangles " << triangles.size() << std::endl;
    
    subdivide->subdivideMesh();
    
    vertices.clear();
    triangles.clear();
    
    for (int i = 0; i < subdivide->getVertices().size(); i++) {
        MeshVertex vertex;
        vertex.p = subdivide->getVertices()[i].p;
        vertices.emplace_back(vertex);
    }
    
    std::cout << "Size New Vectors " << vertices.size() << std::endl;
    
    std::vector<SubTriangle> newTriangles = subdivide->getTriangles();
    
    std::cout << "Size New Triangles " << newTriangles.size() << std::endl;
    
    for (int i = 0; i < newTriangles.size(); i++) {
        MeshTriangle triangle;
        for (int j = 0; j < 3; j++) {
            triangle.v[j] = newTriangles[i].v[j];
        }
        triangle.n = newTriangles[i].n;
        triangle.isNew = newTriangles[i].isNew;
        triangles.emplace_back(triangle);
    }
    
    delete subdivide;
}

int Mesh::getTrianglesCount(){
    return (int)triangles.size();
}

glm::vec3& Mesh::getVertex(int i, int j){
    return vertices[triangles[i].v[j]].p;
}

glm::vec3& Mesh::getNormal(int i){
    return triangles[i].n;
}

bool Mesh::isNewTriangle(int i){
    return triangles[i].isNew;
}