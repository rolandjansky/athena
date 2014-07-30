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

EventBookkeeper_p1::EventBookkeeper_p1( const EventBookkeeper_p1& rhs )
{
  m_name=rhs.m_name;
  m_description=rhs.m_description;
  m_parent=rhs.m_parent;
  m_outputstream = rhs.m_outputstream;
  m_logic = rhs.m_logic;
  m_nAcceptedEvents=rhs.m_nAcceptedEvents;
  m_nWeightedAcceptedEvents=rhs.m_nWeightedAcceptedEvents;
  m_cycle=rhs.m_cycle;
    m_childrenEB=rhs.m_childrenEB;
}


