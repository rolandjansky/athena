/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibStl/DeleteObject.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"

#include <iostream>
#include <algorithm>

namespace MuonCalib {

  MuonCalibEvent::MuonCalibEvent( )
    : m_patternVec(0), m_eventInfo(),  m_triggerTimeInfo(0), m_rawColl(0), m_rawTriggerColl(0), m_truthColl(0),m_rpcSlLogicContainer(0)
  {}

  MuonCalibEvent::MuonCalibEvent( MuonCalibEvent::MCPVec patvec )
    : m_patternVec(patvec), m_eventInfo(),  m_triggerTimeInfo(0), m_rawColl(0), m_rawTriggerColl(0), m_truthColl(0),m_rpcSlLogicContainer(0)
  {}

  MuonCalibEvent::MuonCalibEvent( MuonCalibEvent::MCPVec patvec, const MuonCalibRawHitCollection *Coll ) 
    : m_patternVec(patvec), m_eventInfo(), m_triggerTimeInfo(0), m_rawColl(Coll), m_rawTriggerColl(0), m_truthColl(0),m_rpcSlLogicContainer(0)
  {}

  MuonCalibEvent::MuonCalibEvent( MuonCalibEvent::MCPVec patvec, 
				  const MuonCalibEventInfo eventInfo,
				  const MuonCalibRawHitCollection *Coll )
    : m_patternVec(patvec), m_eventInfo(eventInfo), m_triggerTimeInfo(0), m_rawColl(Coll), m_rawTriggerColl(0), m_truthColl(0),m_rpcSlLogicContainer(0)
  {}
  
  MuonCalibEvent::~MuonCalibEvent() {
    std::for_each( patternBegin(), patternEnd(), DeleteObject() ) ;
    m_patternVec.clear();
    delete m_triggerTimeInfo; 
    delete m_rawColl;
    delete m_rawTriggerColl;
    delete m_truthColl;
  }

  MuonCalibEvent::MuonCalibEvent( const MuonCalibEvent &event ) {
    m_eventInfo = event.eventInfo()  ;

    MCPVecCit it = event.patternBegin() ;
    MCPVecCit it_end = event.patternEnd() ;
    for( ; it!=it_end;++it){
      m_patternVec.push_back( new MuonCalibPattern(**it) ) ; 
    }
    m_rawColl =  event.rawHitCollection() ? new MuonCalibRawHitCollection(*(event.rawHitCollection())): NULL; 
    m_rawTriggerColl =  event.rawTriggerHitCollection() ? new MuonCalibRawTriggerHitCollection(*(event.rawTriggerHitCollection())) : NULL; 
    m_truthColl = event.calibTruthCollection() ? new MuonCalibTruthCollection(*(event.calibTruthCollection())) : NULL;
    m_triggerTimeInfo = event.triggerTimeInfo() ? new MuonCalibTriggerTimeInfo(*(event.triggerTimeInfo())) : NULL;
    m_rpcSlLogicContainer = event.m_rpcSlLogicContainer;
  }

  MuonCalibEvent& MuonCalibEvent::operator=( const MuonCalibEvent &rhs ) {
    if(this!=&rhs){
      m_eventInfo = rhs.eventInfo() ;

      std::for_each( patternBegin(), patternEnd(), DeleteObject() ) ;
      m_patternVec.clear();

      MCPVec temp_patternVec;  
      MCPVecCit it = rhs.patternBegin() ;
      MCPVecCit it_end = rhs.patternEnd() ;
      for( ; it!=it_end;++it){
	temp_patternVec.push_back( new MuonCalibPattern(**it) ) ;
      }
      m_patternVec = temp_patternVec ;
      if(m_truthColl) { delete m_truthColl; m_truthColl=NULL;}
      m_truthColl = new MuonCalibTruthCollection(*rhs.calibTruthCollection());
      if(m_rawColl) { delete m_rawColl; m_rawColl=NULL;}
      m_rawColl = new MuonCalibRawHitCollection(*rhs.rawHitCollection());
      if(m_rawTriggerColl) { delete m_rawTriggerColl; m_rawTriggerColl=NULL;}
      m_rawTriggerColl = new MuonCalibRawTriggerHitCollection(*rhs.rawTriggerHitCollection());
      if(m_triggerTimeInfo) { delete m_triggerTimeInfo; m_triggerTimeInfo=NULL;}
      m_triggerTimeInfo = new MuonCalibTriggerTimeInfo(*rhs.triggerTimeInfo()); 
      m_rpcSlLogicContainer = rhs.rpcSectorLogicContainer();
      
    }
    return (*this) ;
  }
  
  int MuonCalibEvent::numberOfPatterns() const { 
    return m_patternVec.size(); 
  } 
  
  int MuonCalibEvent::numberOfSegments() const { 
    int segs = 0;
    MCPVecCit pat_it = patternBegin();
    MCPVecCit pat_it_end = patternEnd();
    for(;pat_it!=pat_it_end;++pat_it) segs += (*pat_it)->muonSegments();
    
    return segs; 
  } 
  
  int MuonCalibEvent::numberOfHits() const {
    int hits = 0;
    MCPVecCit pat_it = patternBegin();
    MCPVecCit pat_it_end = patternEnd();
    for(;pat_it!=pat_it_end;++pat_it){
      hits += (*pat_it)->nmdtHits()%1000;
      hits += ( (*pat_it)->nmdtHits()- ( (*pat_it)->nmdtHits()%1000 )  )/1000;
      hits += (*pat_it)->nrpcHits()%1000;
      hits += ( (*pat_it)->nrpcHits()- ( (*pat_it)->nrpcHits()%1000 )  )/1000;
      hits += (*pat_it)->ntgcHits()%1000;
      hits += ( (*pat_it)->ntgcHits()- ( (*pat_it)->ntgcHits()%1000 )  )/1000;
      hits += (*pat_it)->ncscHits()%1000;  
      hits += ( (*pat_it)->ncscHits()- ( (*pat_it)->ncscHits()%1000 )  )/1000;
    }
    return hits; 
  } 
    
} //namespace MuonCalib
