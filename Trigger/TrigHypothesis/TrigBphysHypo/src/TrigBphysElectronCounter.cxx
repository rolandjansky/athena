// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigBphysElectronCounter
 **
 **   Description: algo to count electrons in whole event
 ** 
 **  author Olya Igonkina (Nikhef)
 **             
 **************************************************************************/ 
 
#include "TrigBphysElectronCounter.h"

#include "xAODEgamma/ElectronContainer.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "FourMomUtils/P4Helpers.h"


// Define the bins for acceptance-monitoring histogram
#define ACCEPT_hltExecute         0
#define ACCEPT_PassNEFElectrons   1



TrigBphysElectronCounter::TrigBphysElectronCounter(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator)
  ,m_BmmHypTot(0)
  
  , m_nEfElectron(0)
  , m_ptElectronMin()
  , m_mindR(0.005)
  , m_electronCollectionKey()
  , m_IsEMrequiredBits(0xF2)
  , m_applyIsEM(true)
  //counters
  , m_countTotalEvents(0)
  , m_countPassedEvents(0)  
{

// Read cuts

  declareProperty("nEfElectron"        ,   m_nEfElectron    = 0 );
  declareProperty("ptElectronMin"     ,    m_ptElectronMin      );
  declareProperty("overlapdR"     ,        m_mindR    = 0.01  );  
  declareProperty("electronCollectionKey", m_electronCollectionKey  = "" );
  declareProperty("ApplyIsEM",             m_applyIsEM = true);
  declareProperty("IsEMrequiredBits",      m_IsEMrequiredBits = 0xF2);

declareProperty("egammaElectronCutIDToolName",m_egammaElectronCutIDToolName="");


  declareMonitoredStdContainer("Acceptance" , m_mon_Acceptance   , AutoClear);
  declareMonitoredVariable(    "nEFElectrons",  m_mon_nEFElectrons);
}

TrigBphysElectronCounter::~TrigBphysElectronCounter()
{ }




HLT::ErrorCode TrigBphysElectronCounter::hltInitialize()
{
    m_countTotalEvents = 0;
    m_countPassedEvents = 0;

    // sort both list in decreasing pt
    //std::sort(IsEMrequiredBits.begin(), IsEMrequiredBits.end(), [&m_ptElectronMin](size_t i, size_t j) {return m_ptElectronMin[i] > m_ptElectronMin[j];});
    std::sort(m_ptElectronMin.begin(), m_ptElectronMin.end(), std::greater<float>());

   if (msgLvl() <= MSG::INFO) {

     if (m_egammaElectronCutIDToolName=="") {
       ATH_MSG_DEBUG("Electron IsEM PID is disabled, no tool specified "); 
       m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>();
       m_applyIsEM  = false;
	 } 
     else {
       m_egammaElectronCutIDTool=ToolHandle<IAsgElectronIsEMSelector>(m_egammaElectronCutIDToolName);    
       if(m_egammaElectronCutIDTool.retrieve().isFailure()) {
	 ATH_MSG_ERROR("Unable to retrieve " << m_egammaElectronCutIDTool<< " tool ");
	 return HLT::BAD_JOB_SETUP; 
       } 
       else {
	 ATH_MSG_DEBUG("Tool " << m_egammaElectronCutIDTool << " retrieved");
       }
     }
     
     msg() << MSG::INFO << "require at least "<< m_nEfElectron <<" EF Electrons from with collectionKey  m_electronCollectionKey \"" << m_electronCollectionKey  << "\" "<< endmsg;
      msg() << MSG::INFO << " Electrons should have  pts ";	
      for(float pt :  m_ptElectronMin)  msg() << MSG::INFO << pt <<", ";
      msg() << MSG::INFO << endmsg;
      if( m_applyIsEM )  {
        msg() << MSG::INFO << " Electrons should have  isEM bits " << std::hex << "0x" << m_IsEMrequiredBits << std::dec <<", " << endmsg;
      }else{
        msg() << MSG::INFO << " no isEM PID cuts are applied " << endmsg;
       }
     
   }
   if (msgLvl() <= MSG::INFO) {
     msg() << MSG::INFO << " Overlap removal dR<"<<m_mindR<< endmsg;
   }

  if ( timerSvc() ) {
    m_BmmHypTot = addTimer("TrigBphysElectronCounter");
  }




  return HLT::OK;
}

HLT::ErrorCode TrigBphysElectronCounter::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  MsgStream log(msgSvc(), name());
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigBphysElectronCounter -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events " << m_countTotalEvents <<  endmsg;
  msg() << MSG::INFO << "Passed events " << m_countPassedEvents<<  endmsg;

  return HLT::OK;
}



HLT::ErrorCode TrigBphysElectronCounter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output)
{
  // start monitoring
  beforeExecMonitors().ignore();
  m_countTotalEvents++;  
  m_mon_Acceptance.clear();

  m_mon_Acceptance.push_back( ACCEPT_hltExecute );
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigBphysElectronCounter hltExecute" << endmsg;


  //========  check if we have enough EF electrons :  =====================
  std::vector<ElementLink<xAOD::ElectronContainer> > efelectrons; // just a collection of pointers, not copies
  bool passElectron = passNObjects<xAOD::ElectronContainer, 
				   std::vector<ElementLink<xAOD::ElectronContainer> > >( m_nEfElectron, m_ptElectronMin, m_IsEMrequiredBits,
											 inputTE, efelectrons,  m_electronCollectionKey, m_mindR);
  if( !passElectron ){
    if ( timerSvc() )  m_BmmHypTot->stop();
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found "<<efelectrons.size() <<" EF electrons - fail (either number or pts or isEM are insufficient)"<<  endmsg; 
    afterExecMonitors().ignore();   
    return HLT::OK;
  }else{
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found "<<efelectrons.size() <<" EF electrons, require "<<m_nEfElectron<<" - accept "<<  endmsg; 
  }
  m_mon_Acceptance.push_back( ACCEPT_PassNEFElectrons );
  m_mon_nEFElectrons = m_nEfElectron;


  // record collection now
  if ( timerSvc() )  m_BmmHypTot->stop();
    
  HLT::TriggerElement* outputTE = addRoI(output);     
  outputTE->setActiveState(true);
  m_countPassedEvents++;


  // stop monitoring
  afterExecMonitors().ignore();   
  return HLT::OK;
}




