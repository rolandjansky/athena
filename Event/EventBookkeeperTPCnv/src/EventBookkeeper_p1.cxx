/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"

EventBookkeeper_p1::EventBookkeeper_p1()
{ 
  m_name="";
  m_description="";
  m_parent="";
  m_outputstream="";
  m_logic ="";
  m_nAcceptedEvents=0; 
  m_nWeightedAcceptedEvents=0; 
  m_cycle=0; 
  m_childrenEB.clear();
}

