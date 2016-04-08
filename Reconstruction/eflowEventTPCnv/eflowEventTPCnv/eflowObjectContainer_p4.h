/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_P4_H
#define EFLOWOBJECTCONTAINER_P4_H

#include "eflowEventTPCnv/eflowObject_p4.h"
#include <vector>

class eflowObjectContainer_p4 {

 public:

  eflowObjectContainer_p4() {};

  ~eflowObjectContainer_p4() {};

  friend class eflowObjectContainerCnv_p4;
    
  // private:
    
  std::vector<eflowObject_p4> m_myPersVector;

};
#endif
