/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAEvent.h"
 
#include <iostream>
#include <algorithm>

namespace MuonDQA {
 

  MuonDQAEvent::MuonDQAEvent(): m_eventInfo()
  {}

  MuonDQAEvent::~MuonDQAEvent() {
    //std::cout << "MuonDQAEvent::destructor " << std::endl;
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

