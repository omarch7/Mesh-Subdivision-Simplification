//
//  main.cpp
//  Mesh
//
//  Created by 奥玛 on 16/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <iostream>
#include "Mesh.hpp"
#include <unistd.h>

Mesh *myMesh = NULL;

bool polygonWireFrame = false;

GLfloat xRot = 0.0f;
GLfloat xSpeed = 0.0f;
GLfloat yRot = 0.0f;
GLfloat ySpeed = 0.0f;
GLfloat zoom = -3.5f;
GLfloat zoomSpeed = 0.0f;

GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0};
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0};
GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0};
GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0};
GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0};
GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0};
GLfloat blankMaterial[] = {0.0, 0.0, 0.0};
GLfloat mShininess[] = {128};
GLfloat lightPos[] = {4.0f, 4.0f, 6.0f, 1.0f};
GLfloat lightPos2[] = {-4.0f, -4.0f, -6.0f, 1.0f};

int init(void){
    
    myMesh = new Mesh();
    
    glShadeModel(GL_FLAT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHT1);
    
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
    
    
    glLightfv(GL_LIGHT1, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT1, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuseLight);
    
    glEnable(GL_COLOR_MATERIAL);
    
    return myMesh->loadMeshFromFile("mannequin_small.obj");
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    
    
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotated(yRot, 0.0f, 1.0f, 0.0f);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
    
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    
    glColor3f(0.23f, 0.74f, 0.92f);

    glBegin(GL_TRIANGLES);
    
    for (int i = 0; i < myMesh->getTrianglesCount(); i++) {
        glm::vec3 normal = myMesh->getNormal(i);
        glNormal3f(normal.x, normal.y, normal.z);
        for (int j = 0; j < 3; j++) {
            glm::vec3 vertex = myMesh->getVertex(i, j);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    
    glEnd();
    
    glPopMatrix();
    
    xRot += xSpeed;
    yRot += ySpeed;
    zoom += zoomSpeed;
    
    glutSwapBuffers();
}

void reshape(int width, int height){
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'a':
            ySpeed -= 0.1;
            break;
        case 'd':
            ySpeed += 0.1;
            break;
        case 'w':
            xSpeed -= 0.1;
            break;
        case 's':
            xSpeed += 0.1;
            break;
        case 'i':
            zoomSpeed += 0.1;
            break;
        case 'o':
            zoomSpeed -= 0.1;
            break;
        case 'p':
            if(polygonWireFrame){
                polygonWireFrame = false;
                glEnable(GL_LIGHTING);
                glEnable(GL_COLOR_MATERIAL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }else{
                polygonWireFrame = true;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_COLOR_MATERIAL);
                glDisable(GL_LIGHTING);
            }
            break;
        case '-':
            std::cout << "Simplifying..." << std::endl;
            myMesh->simplifyTenPercent();
            std::cout << "Simplified!" << std::endl;
            break;
        case '+':
            std::cout << "Subdividing..." << std::endl;
            myMesh->subdivideOneStep();
            std::cout << "Subdivided" << std::endl;
            break;
        default:
            break;
    }
}


int main(int argc, const char * argv[]) {
    glutInit(&argc, const_cast<char**>(argv));
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 640) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - 480) / 2);
    glutCreateWindow("Mesh Subdivision & Simplifcation");
    if(!init()){
        std::cout << "Couldn't load the Obj file" << std::endl;
        return 0;
    }
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
