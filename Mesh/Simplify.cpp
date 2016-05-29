//
//  Simplify.cpp
//  Mesh
//
//  Created by 奥玛 on 23/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#include <iostream>
#include "Simplify.hpp"

Simplify::Simplify(){
    triangles = std::vector<Triangle>();
    vertices = std::vector<Vertex>();
    refs = std::vector<Ref>();
}

double Simplify::vertexError(SymmetricMatrix q, double x, double y, double z){
    return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[4]*y*y + 2*q[5]*y*z + 2*q[6]*y + q[7]*z*z + 2*q[8]*z + q[9];
}

double Simplify::calculateError(int idV1, int idV2, glm::vec3 &pResult){
    
    SymmetricMatrix q = vertices[idV1].q + vertices[idV2].q;
    
    bool border = vertices[idV1].border & vertices[idV2].border;
    double error = 0;
    double det = q.det(0, 1, 2, 1, 4, 5, 2, 5, 7);
    
    if (det != 0 && !border) {
        pResult.x = -1 / det * (q.det(1, 2, 3, 4, 5, 6, 5, 7, 8));
        pResult.y = 1 / det * (q.det(0, 2, 3, 1, 5, 6, 2, 7, 8));
        pResult.z = -1 / det * (q.det(0, 1, 3, 1, 4, 6, 2, 5, 8));
        error = vertexError(q, pResult.x, pResult.y, pResult.z);
    }else{
        glm::vec3 p1 = vertices[idV1].p;
        glm::vec3 p2 = vertices[idV2].p;
        glm::vec3 p3 = glm::vec3((p1.x+p2.x)/2, (p1.y+p2.y)/2, (p1.z+p2.z)/2);
        double error1 = vertexError(q, p1.x, p1.y, p1.z);
        double error2 = vertexError(q, p2.x, p2.y, p2.z);
        double error3 = vertexError(q, p3.x, p3.y, p3.z);
        error = min(error1, min(error2, error3));
        if (error1 == error) {
            pResult = p1;
        }
        if (error2 == error){
            pResult = p2;
        }
        if (error3 == error){
            pResult = p3;
        }
    }
    return error;
}

bool Simplify::flipped(glm::vec3 p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted){
    int borderCount = 0;
    for(int i = 0; i < v0.tCount; i++){
        Triangle &t = triangles[refs[v0.tStart+i].tid];
        if(t.deleted){
            continue;
        }
        
        int s = refs[v0.tStart+i].tVertex;
        int id1 = t.v[(s+1)%3];
        int id2 = t.v[(s+2)%3];
        
        if (id1 == i1 || id2 == i1) {
            borderCount++;
            deleted[i] = 1;
            continue;
        }
        
        glm::vec3 d1 = glm::normalize(vertices[id1].p - p);
        glm::vec3 d2 = glm::normalize(vertices[id2].p - p);
        
        if(fabs(glm::dot(d1, d2)) > 0.999){
            return true;
        }
        
        glm::vec3 n = glm::normalize(glm::cross(d1, d2));
        deleted[i] = 0;
        if(glm::dot(n, t.n) < 0.2){
            return true;
        }
    }
    return false;
}

void Simplify::updateTriangles(int i0, Vertex &v, std::vector<int> &deleted, int &deletedTriangles){
    glm::vec3 p;
    for(int i = 0; i < v.tCount; i++){
        Ref &r = refs[v.tStart + i];
        Triangle &t = triangles[r.tid];
        
        if(t.deleted){
            continue;
        }
        
        if(deleted[i]){
            t.deleted = 1;
            deletedTriangles++;
            continue;
        }
        
        t.v[r.tVertex] = i0;
        t.dirty = 1;
        t.err[0] = calculateError(t.v[0], t.v[1], p);
        t.err[1] = calculateError(t.v[1], t.v[2], p);
        t.err[2] = calculateError(t.v[2], t.v[0], p);
        t.err[3] = min(t.err[0], min(t.err[1], t.err[2]));
        refs.emplace_back(r);
    }
}

void Simplify::updateMesh(int iteration){
    if(iteration>0){
        int dst = 0;
        for (int i = 0; i < triangles.size(); i++) {
            if (!triangles[i].deleted) {
                triangles[dst++] = triangles[i];
            }
        }
        triangles.resize(dst);
    }
    
    if (iteration==0) {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i].q = SymmetricMatrix(0.0f);
        }
        
        for (int i = 0; i < triangles.size(); i++){
            Triangle &t = triangles[i];
            glm::vec3 n, p[3];
            
            for (int j = 0; j < 3; j++) {
                p[j] = vertices[t.v[j]].p;
            }
            
            n = glm::normalize(glm::cross(p[1] - p[0], p[2] - p[0]));
            t.n = n;
            
            for (int j = 0; j < 3; j++){
                vertices[t.v[j]].q = vertices[t.v[j]].q + SymmetricMatrix(n.x, n.y, n.z, -glm::dot(n, p[0]));
            }
        }
        
        for(int i = 0; i < triangles.size(); i++){
            Triangle &t = triangles[i];
            glm::vec3 p;
            
            for(int j = 0; j < 3; j++){
                t.err[j] = calculateError(t.v[j], t.v[(j+1)%3], p);
            }
            t.err[3] = min(t.err[0], min(t.err[1], t.err[2]));
        }
    }
    
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].tStart = 0;
        vertices[i].tCount = 0;
    }
    
    for (int i = 0; i < triangles.size(); i++) {
        Triangle &t = triangles[i];
        for (int j = 0; j < 3; j++) {
            vertices[t.v[j]].tCount++;
        }
    }
    int tStart = 0;
    for (int i = 0; i < vertices.size(); i++) {
        Vertex &v = vertices[i];
        v.tStart = tStart;
        tStart += v.tCount;
        v.tCount = 0;
    }
    
    refs.resize(triangles.size()*3);
    for (int i = 0; i < triangles.size(); i++) {
        Triangle &t = triangles[i];
        for(int j = 0; j < 3; j++){
            Vertex &v = vertices[t.v[j]];
            refs[v.tStart + v.tCount].tid = i;
            refs[v.tStart + v.tCount].tVertex = j;
            v.tCount++;
        }
    }
    
    if (iteration==0) {
        std::vector<int> vCount, vIds;
        
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i].border = 0;
        }
        
        for (int i = 0; i < vertices.size(); i++) {
            Vertex &v = vertices[i];
            vCount.clear();
            vIds.clear();
            
            for (int j = 0; j < v.tCount; j++) {
                int k = refs[v.tStart + j].tid;
                Triangle &t = triangles[k];
                for (int k = 0; k < 3; k++) {
                    int ofs = 0, id = t.v[k];
                    while (ofs < vCount.size()) {
                        if(vIds[ofs]==id){
                            break;
                        }
                        ofs++;
                    }
                    if (ofs == vCount.size()) {
                        vCount.emplace_back(1);
                        vIds.emplace_back(id);
                    }else{
                        vCount[ofs]++;
                    }
                }
            }
            
            for (int j = 0; j < vCount.size(); j++) {
                if (vCount[j] == 1) {
                    vertices[vIds[j]].border = 1;
                }
            }
        }
    }
}

void Simplify::compactMesh(){
    int dst = 0;
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].tCount = 0;
    }
    for (int i = 0; i < triangles.size(); i++) {
        if (!triangles[i].deleted) {
            Triangle &t = triangles[i];
            triangles[dst++] = t;
            for (int j = 0; j < 3; j++) {
                vertices[t.v[j]].tCount = 1;
            }
        }
    }
    triangles.resize(dst);
    dst = 0;
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].tCount) {
            vertices[i].tStart = dst;
            vertices[dst].p = vertices[i].p;
            dst++;
        }
    }
    for (int i = 0; i < triangles.size(); i++) {
        Triangle &t = triangles[i];
        for (int j = 0; j < 3; j++) {
            t.v[j] = vertices[t.v[j]].tStart;
        }
    }
    vertices.resize(dst);
}

void Simplify::simplifyMesh(int targetCount, double agressiveness){
    for (int i = 0; i < triangles.size(); i++) {
        triangles[i].deleted = 0;
    }
    
    int deletedTriangles = 0;
    std::vector<int> deleted0, deleted1;
    int triangleCount = (int)triangles.size();
    
    for (int iter = 0; iter < 100; iter++) {
        if (triangleCount - deletedTriangles <= targetCount) {
            break;
        }
        if(iter%5 == 0){
            updateMesh(iter);
        }
        for (int i = 0; i < triangles.size(); i++) {
            triangles[i].dirty = 0;
        }
        
        double threshold = 0.000000001*pow(double(iter+3),agressiveness);
        
        for (int i = 0; i < triangles.size(); i++) {
            Triangle &t = triangles[i];
            
            if (t.err[3]>threshold) {
                continue;
            }
            if (t.deleted) {
                continue;
            }
            if (t.dirty) {
                continue;
            }
            
            for (int j = 0; j < 3; j++) {
                if (t.err[j] < threshold) {
                    int i0 = t.v[j];
                    Vertex &v0 = vertices[i0];
                    int i1 = t.v[(j+1)%3];
                    Vertex &v1 = vertices[i1];
                    
                    if (v0.border != v1.border) {
                        continue;
                    }
                    
                    glm::vec3 p;
                    calculateError(i0, i1, p);
                    
                    deleted0.resize(v0.tCount);
                    deleted1.resize(v1.tCount);
                    
                    if (flipped(p, i0, i1, v0, v1, deleted0)) {
                        continue;
                    }
                    
                    if (flipped(p, i1, i0, v1, v0, deleted1)) {
                        continue;
                    }
                    
                    v0.p = p;
                    v0.q = v1.q + v0.q;
                    
                    int tStart = (int)refs.size();
                    updateTriangles(i0, v0, deleted0, deletedTriangles);
                    updateTriangles(i0, v1, deleted1, deletedTriangles);
                    
                    int tCount = (int)refs.size() - tStart;
                    
                    if (tCount <= v0.tCount) {
                        if (tCount) {
                            memcpy(&refs[v0.tStart], &refs[tStart], tCount*sizeof(Ref));
                        }
                    }else{
                        v0.tStart = tStart;
                    }
                    v0.tCount = tCount;
                    break;
                }
            }
            if (triangleCount - deletedTriangles <= targetCount) {
                break;
            }
        }
    }
    compactMesh();
}

void Simplify::addTriangle(Triangle triangle){
    triangles.emplace_back(triangle);
}

void Simplify::addVertex(Vertex vertex){
    vertices.emplace_back(vertex);
}

std::vector<Vertex> Simplify::getVertices(){
    return vertices;
}

std::vector<Triangle> Simplify::getTriangles(){
    std::vector<Triangle> newTriangles = std::vector<Triangle>();
    for (int i = 0; i < triangles.size(); i++) {
        if (!triangles[i].deleted) {
            newTriangles.emplace_back(triangles[i]);
        }
    }
    return newTriangles;
}

double Simplify::min(double a, double b){
    return a < b ? a : b;
}