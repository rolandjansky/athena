// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigBphysMuonCounter
 **
 **   Description: algo to count muons in whole event
 ** 
 **  author Olya Igonkina 
 **             
 **************************************************************************/ 
 
#include "TrigBphysMuonCounter.h"

#include "xAODMuon/MuonContainer.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "FourMomUtils/P4Helpers.h"


// Define the bins for acceptance-monitoring histogram
#define ACCEPT_hltExecute             0
#define ACCEPT_PassNEFMuons           1	


TrigBphysMuonCounter::TrigBphysMuonCounter(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator)
  ,m_BmmHypTot(0)
  
  , m_nEfMuon(0)
  , m_ptMuonMin()
  , m_mindR(0.005)
  , m_muonCollectionKey()
  //counters
  , m_countTotalEvents(0)
  , m_countPassedEvents(0)

{

  // Read cuts

  declareProperty("nEfMuon"        , m_nEfMuon    = 0 );
  declareProperty("ptMuonMin"     , m_ptMuonMin      );
  declareProperty("overlapdR"     , m_mindR    = 0.01  );  
  declareProperty("muonCollectionKey", m_muonCollectionKey  = "" );

  declareMonitoredStdContainer("Acceptance" , m_mon_Acceptance   , AutoClear);
  declareMonitoredVariable(    "nEFMuons",  m_mon_nEFMuons);
}

TrigBphysMuonCounter::~TrigBphysMuonCounter()
{ }




HLT::ErrorCode TrigBphysMuonCounter::hltInitialize()
{
    m_countTotalEvents = 0;
    m_countPassedEvents = 0;

    std::sort(m_ptMuonMin.begin(),m_ptMuonMin.end(), std::greater<float>());
    
   if (msgLvl() <= MSG::INFO) {

     msg() << MSG::INFO << "require at least "<< m_nEfMuon <<" EF Muons from with collectionKey  m_muonCollectionKey \"" <<   m_muonCollectionKey << "\" "<< endmsg;
      msg() << MSG::INFO << " Muons should have  pts ";	
      for(float pt :  m_ptMuonMin)  msg() << MSG::INFO << pt <<", ";
      msg() << MSG::INFO << endmsg;
     
   }
   if (msgLvl() <= MSG::INFO) {
     msg() << MSG::INFO << " Overlap removal dR<"<<m_mindR<< endmsg;
   }

  if ( timerSvc() ) {
    m_BmmHypTot = addTimer("TrigBphysMuonCounter");
  }
    
  return HLT::OK;
}

HLT::ErrorCode TrigBphysMuonCounter::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  MsgStream log(msgSvc(), name());
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigBphysMuonCounter -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events " << m_countTotalEvents <<  endmsg;
  msg() << MSG::INFO << "Passed events " << m_countPassedEvents<<  endmsg;

  return HLT::OK;
}



HLT::ErrorCode TrigBphysMuonCounter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output)
{
  // start monitoring
  beforeExecMonitors().ignore();
  m_countTotalEvents++;  
  m_mon_Acceptance.clear();

  m_mon_Acceptance.push_back( ACCEPT_hltExecute );
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigBphysMuonCounter hltExecute" << endmsg;


  //========  check if we have enough EF muons :  =====================
  std::vector<ElementLink<xAOD::MuonContainer> > efmuons; // just a collection of pointers, not copies
  bool passMuon = passNObjects<xAOD::MuonContainer, 
			  std::vector<ElementLink<xAOD::MuonContainer> > >( m_nEfMuon, m_ptMuonMin, 
									      inputTE, efmuons,  m_muonCollectionKey, m_mindR);
  if( !passMuon ){
    if ( timerSvc() )  m_BmmHypTot->stop();
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found "<<efmuons.size() <<" EF muons - fail (either number or pts are insufficient)"<<  endmsg; 
    afterExecMonitors().ignore();   
    return HLT::OK;
  }else{
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found "<<efmuons.size() <<" EF muons, require "<<m_nEfMuon<<" - accept "<<  endmsg; 
  }
  m_mon_Acceptance.push_back( ACCEPT_PassNEFMuons );
  m_mon_nEFMuons = m_nEfMuon;


  // record collection now
  if ( timerSvc() )  m_BmmHypTot->stop();
    
  HLT::TriggerElement* outputTE = addRoI(output);     
  outputTE->setActiveState(true);
  m_countPassedEvents++;


  // stop monitoring
  afterExecMonitors().ignore();   
  return HLT::OK;
}




