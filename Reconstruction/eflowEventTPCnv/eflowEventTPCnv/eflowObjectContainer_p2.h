/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_P2_H
#define EFLOWOBJECTCONTAINER_P2_H

#include "eflowEventTPCnv/eflowObject_p2.h"
#include <vector>

class eflowObjectContainer_p2 {

 public:

  eflowObjectContainer_p2();

  ~eflowObjectContainer_p2() {};

  friend class eflowObjectContainerCnv_p2;
    
  // private:
    
  std::vector<eflowObject_p2> m_myPersVector;

  double m_misset, m_sumet, m_ptx, m_pty, m_numef;

  double m_numphot, m_numele, m_nummuo;

  // Event Shape
  double m_circularity;
  double m_thrust;
  double m_oblateness;
  
  bool m_valid;


};
#endif
