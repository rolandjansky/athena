/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_HIVEDATAOBJ_H
#define ATHEXHIVE_HIVEDATAOBJ_H

//sample data class for the Read/Write example
//it just wraps an int. Notice that is does not inherit from Gaudi DataObject

//a dummy base class to test the symlinks
class BaseClass { };

class HiveDataObj {
 
public: 
 
  HiveDataObj():m_val(0) {};
  HiveDataObj(int i): m_val(i) {};
  virtual ~HiveDataObj() = default;

  void val(int i) { m_val = i; }
  int val() const { return m_val; }

  typedef std::true_type thread_safe;

private:
  int m_val;
};

#include <iostream>
  inline std::ostream& operator<<(std::ostream& ost, const HiveDataObj& rhs) {
    ost << "HDO: " << rhs.val(); 
    return ost;
  }


//using the macros below we can assign an identifier (and a version) 
//to the type HiveDataObj
//This is required and checked at compile time when you try to record/retrieve
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( HiveDataObj , 37539154 , 1 )

#endif
