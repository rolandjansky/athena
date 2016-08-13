/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __EvtInfo_cxx__
#include "MuonPtCalibNtupleMaker/EvtInfo.h"

EvtInfo::EvtInfo() {

  Reset();

}

void EvtInfo::Reset() {

  m_RunNumber = 0;
  m_LumiBlock = 0;
  m_ChannelNumber = 0;
  m_EvtNumber = 0;
  m_EventWeight = 0.;

}

void EvtInfo::Register( TTree* t ) {

  t->Branch( "RunNumber", &m_RunNumber );
  t->Branch( "LumiBlock", &m_LumiBlock );
  t->Branch( "ChannelNumber", &m_ChannelNumber );
  t->Branch( "EvtNumber", &m_EvtNumber );
  t->Branch( "EventWeight", &m_EventWeight );

}

void EvtInfo::Fill( const xAOD::EventInfo* ei ) {
   
  m_RunNumber = ei->runNumber();
  m_EvtNumber = ei->eventNumber();
  m_LumiBlock = ei->lumiBlock();
  m_ChannelNumber = ( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? ei->mcChannelNumber() : 0;
  m_EventWeight = ( ei->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? ei->mcEventWeight() : 1.;

}
