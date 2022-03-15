/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// Class to store MuonDQAPatterns in of the same event          //
// Author:   N. Benekos(Illinois)                               // 
// March 2008                                                   //
////////////////////////////////////////////////////////////////// 
 
#ifndef MuonDQAEvent_h
#define MuonDQAEvent_h
 
#include <vector>
#include <iostream>
#include <string>
#include "MuonDQAEventInfo.h" 
 
namespace MuonDQA {
 
  /**
     @class MuonDQAEvent
     Container class storing all entries of the MuonDQA Monitoring histos.
     It contains the following:
     - MuonDQAEventInfo: run/event number, a tag holding information
     about type of reconstruction performed 
     @author Nectarios.Benekos@cern.ch 
  */
 
  class MuonDQAEvent{
  public: 
 
    MuonDQAEvent(); //!< Default constructor
    MuonDQAEvent( const MuonDQAEvent& event ) ; //!< Copy constructor
    ~MuonDQAEvent(); //!< Destructor
 
    MuonDQAEvent& operator=( const MuonDQAEvent& rhs ) ; //!< Assignment operator 
    const MuonDQAEventInfo& eventInfo() const { return m_eventInfo ;}; //!< retrieve the MuonDQAEventInfo.
    void setMuonDQAEventInfo( const MuonDQAEventInfo & eventInfo ) { m_eventInfo = eventInfo ; } ; //!< set the MuonDQAEventInfo
 
  private:
 
    MuonDQAEventInfo               m_eventInfo;   //!< MuonDQAEventInfo of the MuonDQAEvent
 
  } ;
 
   
}// namespace MuonDQA

#endif //MuonDQAEvent_h
