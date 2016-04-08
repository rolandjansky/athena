/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINER_P1_H
#define EFLOWOBJECTCONTAINER_P1_H

#include "eflowEventTPCnv/eflowObject_p1.h"
#include <vector>

class eflowObjectContainer_p1 {

 public:

  eflowObjectContainer_p1();

  ~eflowObjectContainer_p1() {};

  friend class eflowObjectContainerCnv_p1;
    
  // private:
    
  std::vector<eflowObject_p1> m_myPersVector;

    double m_misset, m_sumet, m_ptx, m_pty, m_numef;

    double m_numphot, m_numele, m_nummuo;

    // Event Shape
    double m_circularity;
    double m_thrust;
    double m_oblateness;
    
    bool m_valid;


};
#endif
