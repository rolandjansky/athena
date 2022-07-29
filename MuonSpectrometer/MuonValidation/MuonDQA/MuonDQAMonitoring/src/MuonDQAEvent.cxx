/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAEvent.h"
 


namespace MuonDQA {
 

  MuonDQAEvent::MuonDQAEvent(): m_eventInfo()
  {}

  MuonDQAEvent::~MuonDQAEvent() {
   }
 
  MuonDQAEvent::MuonDQAEvent( const MuonDQAEvent& event ) 
  {
    m_eventInfo = event.eventInfo()  ;
  }

  MuonDQAEvent& MuonDQAEvent::operator=( const MuonDQAEvent& rhs ) 
  {
    if(this!=&rhs){
      m_eventInfo = rhs.eventInfo() ; 
    }
    return (*this) ;
  }
   
} //namespace MuonDQA

