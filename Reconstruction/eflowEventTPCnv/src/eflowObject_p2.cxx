/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEventTPCnv/eflowObject_p2.h"

/** Constructor where we initialize the data members of this class **/
eflowObject_p2::eflowObject_p2() {

  //initialize the data members
  m_charge = 0;
  m_d0 = 0.0;
  m_eflowType = 0;
  m_eta = 0.0;
  m_e = 0.0;
  m_m = 0.0;
  m_nClus = 0;
  m_nTrack = 0;
  m_phi = 0.0;
  m_valid = 0;
  m_z0 = 0.0;

}
