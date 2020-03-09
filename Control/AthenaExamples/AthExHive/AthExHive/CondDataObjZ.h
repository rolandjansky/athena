/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_CONDDATAOBJZ_H
#define CONDALGS_CONDDATAOBJZ_H

class CondDataObjZ {
 
public: 
 
  CondDataObjZ():m_val(0) {};
  CondDataObjZ(float f): m_val(f) {};
  virtual ~CondDataObjZ(){};

  void val(float f) { m_val = f; }
  float val() const { return m_val; }

private:
  float m_val;
};

#include <iostream>
inline std::ostream& operator<<(std::ostream& ost, const CondDataObjZ& rhs) {
  ost << rhs.val(); 
  return ost;
}


//using the macros below we can assign an identifier (and a version) 
//to the type CondDataObjZ
//This is required and checked at compile time when you try to record/retrieve
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CondDataObjZ , 6664395 , 1 )
CLASS_DEF( CondCont<CondDataObjZ> , 210255841 , 1 )


#endif
