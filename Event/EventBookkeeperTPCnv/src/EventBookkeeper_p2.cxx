/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#include "EventBookkeeperTPCnv/EventBookkeeper_p2.h"

EventBookkeeper_p2::EventBookkeeper_p2()
{ 
  m_name="";
  m_description="";
  m_inputstream="";
  m_outputstream="";
  m_logic ="";
  m_nAcceptedEvents=0; 
  m_nWeightedAcceptedEvents=0; 
  m_cycle=-1; 
  m_childrenEB = 0; 
}

EventBookkeeper_p2::EventBookkeeper_p2( const EventBookkeeper_p2& rhs )
  : m_childrenEB(0)
{
  *this = rhs;
}

EventBookkeeper_p2& EventBookkeeper_p2::operator=( const EventBookkeeper_p2& rhs )
{
  if (this != &rhs) {
    m_name=rhs.m_name;
    m_description=rhs.m_description;
    m_inputstream=rhs.m_inputstream;
    m_outputstream = rhs.m_outputstream;
    m_logic = rhs.m_logic;
    m_nAcceptedEvents=rhs.m_nAcceptedEvents;
    m_nWeightedAcceptedEvents=rhs.m_nWeightedAcceptedEvents;
    m_cycle=rhs.m_cycle;
    //Make a new deep copy of the children by calling the constructor iteratively (as this becomes the new owner of these objects)
    delete m_childrenEB;
    if (!rhs.m_childrenEB)
      m_childrenEB = 0;
    else {
      m_childrenEB = new std::vector<EventBookkeeper_p2*>; 
      for(unsigned int i=0; i<rhs.m_childrenEB->size(); i++){ 
        EventBookkeeper_p2* child=new EventBookkeeper_p2(*rhs.m_childrenEB->at(i));
        m_childrenEB->push_back(child); 
      }
    }
  }
  return *this;
}

EventBookkeeper_p2::~EventBookkeeper_p2()
{
  //Iteratively call the destructor of the children
  if (m_childrenEB) {
    for(unsigned int i=0; i<m_childrenEB->size(); i++){ delete m_childrenEB->at(i); }
    m_childrenEB->clear();
    delete m_childrenEB;
  }
}

