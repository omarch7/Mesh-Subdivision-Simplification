//
//  Tests.cpp
//  Mesh
//
//  Created by 奥玛 on 19/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "Mesh.hpp"
#include "Simplify.hpp"

int main_(int argc, const char * argv[]){
    Mesh *myMesh = new Mesh();
    
    myMesh->loadMeshFromFile("cow.obj");
    
    myMesh->simplifyTenPercent();
    
    
    
    return 0;
}