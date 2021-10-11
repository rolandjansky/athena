/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYOBJ_H
#define PYANALYSISEXAMPLES_MYOBJ_H

#include <string>
#include <iostream>
#include <typeinfo>

#include "AthenaKernel/CLASS_DEF.h"

namespace AthPyEx {

class MyObj
{
public:
  MyObj (const std::string &name) : m_a(0) { m_name = name; }
  virtual ~MyObj () {}

  const std::string name () { return m_name; }
  template <class T> void setA(T a)
  {
    std::cout << " C++ setA " << typeid(a).name() << " " << a << std::endl;
    m_a = (int)a;
  }
      
  int  getA() { return m_a;}

private:
  std::string m_name;
  int m_a;
};

} // namespace AthPyEx

CLASS_DEF(AthPyEx::MyObj, 28807578, 1)

#endif

