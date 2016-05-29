//
//  SymmetricMatrix.hpp
//  Mesh
//
//  Created by 奥玛 on 23/5/16.
//  Copyright © 2016 THU. All rights reserved.
//

#ifndef SymmetricMatrix_hpp
#define SymmetricMatrix_hpp

#include <stdio.h>

class SymmetricMatrix{
private:
    double m[10];
public:
    SymmetricMatrix(double c=0);
    SymmetricMatrix(double m11, double m12, double m13, double m14,
                                double m22, double m23, double m24,
                                            double m33, double m34,
                                                        double m44);
    SymmetricMatrix(double a, double b, double c, double d);
    double operator[](int c)const;
    double det(int a11, int a12, int a13,
               int a21, int a22, int a23,
               int a31, int a32, int a33);
    const SymmetricMatrix operator+(const SymmetricMatrix& n) const;
    SymmetricMatrix& operator+=(const SymmetricMatrix& n);
};

#endif /* SymmetricMatrix_hpp */
