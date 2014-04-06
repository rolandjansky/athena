/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********
eflowObjectContainer - original author Dan Tovey.
Added default constructor via this cxx file - M.Hodgkinson 31 May 2011
*************/

#include "eflowEvent/eflowObjectContainer.h"

eflowObjectContainer::eflowObjectContainer(SG::OwnershipPolicy ownPolicy )  :  DataVector<eflowObject>(ownPolicy){

  m_misset = 0.0;
  m_sumet = 0.0;
  m_ptx = 0.0;
  m_pty = 0.0;

  m_numef = 0.0;
  m_numphot = 0.0;
  m_numele = 0.0;
  m_nummuo = 0.0;

  m_circularity = -999.0;
  m_thrust = -999.0;
  m_oblateness = -999.0;

  m_valid = true;

}
