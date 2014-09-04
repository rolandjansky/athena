/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MYDATAOBJ_
#define _MYDATAOBJ_

//sample data class for the Read/Write example
//it just wraps an int. Notice that is does not inherit from Gaudi DataObject

namespace SGTutorial {

  class MyDataObj {
 
  public: 
 
    MyDataObj(): m_val(0) {}
    MyDataObj(int i): m_val(i) {}

    void val(int i) { m_val = i; }
    int val() const { return m_val; }

  private:
    int m_val;
  };
#include <iostream>
  inline std::ostream& operator<<(std::ostream& ost, const MyDataObj& rhs) {
    ost << "Val: " << rhs.val(); 
    return ost;
  }

} //end ns SGTutorial
//using the macros below we can assign an identifier (and a version) 
//to the type MyDataObj
//This is required and checked at compile time when you try to record/retrieve
#include "SGTools/CLASS_DEF.h"
CLASS_DEF(SGTutorial::MyDataObj, 9901, 1)


#endif


