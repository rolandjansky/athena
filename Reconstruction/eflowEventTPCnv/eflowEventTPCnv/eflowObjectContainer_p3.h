/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_P3_H
#define EFLOWOBJECTCONTAINER_P3_H

#include "eflowEventTPCnv/eflowObject_p3.h"
#include <vector>

class eflowObjectContainer_p3 {

 public:

  eflowObjectContainer_p3() {};

  ~eflowObjectContainer_p3() {};

  friend class eflowObjectContainerCnv_p3;
    
  // private:
    
  std::vector<eflowObject_p3> m_myPersVector;

};
#endif
