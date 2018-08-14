//
//  TestClass.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/15/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#ifndef __TopoCore__TestClass__
#define __TopoCore__TestClass__

#include <iostream>


class TestClass {
public:
    TestClass() :
    m_a(0)
    {
        std::cout << "Creating TestClass()" << std::endl;
    }
    TestClass(int a) :
    m_a(a)
    {
        std::cout << "Creating TestClass(" << m_a << ")" << std::endl;
    }
private:
    int m_a;
};


#endif /* defined(__TopoCore__TestClass__) */
