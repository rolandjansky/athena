/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_P5_H
#define EFLOWOBJECTCONTAINER_P5_H

#include "eflowEventTPCnv/eflowObject_p5.h"
#include <vector>

class eflowObjectContainer_p5 {

 public:

  eflowObjectContainer_p5() {};

  ~eflowObjectContainer_p5() {};

  friend class eflowObjectContainerCnv_p5;
  
  //private:
    
  std::vector<eflowObject_p5> m_myPersVector;

};
#endif
