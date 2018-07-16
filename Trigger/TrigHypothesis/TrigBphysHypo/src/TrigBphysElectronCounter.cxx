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
#define ACCEPT_PassNElectrons     1
#define ACCEPT_PassPtcut          3
#define ACCEPT_PassIsEM           2


TrigBphysElectronCounter::TrigBphysElectronCounter(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator)
  ,m_BmmHypTot(0)
  
  , m_nEfElectron(0)
  , m_ptElectronMin()
  , m_mindR(0.005)
  , m_electronCollectionKey()
  , m_outputTrackCollectionKey()
  , m_IsEMrequiredBits(0xF2) 
  , m_applyIsEM(true)
  , m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
  //counters
  , m_countTotalEvents(0)
  , m_countPassedEvents(0)  
{

// Read cuts

  declareProperty("nEfElectron"        ,   m_nEfElectron    = 0 );
  declareProperty("ptElectronMin"     ,    m_ptElectronMin      );
  declareProperty("overlapdR"     ,        m_mindR    = 0.01  );  
  declareProperty("electronCollectionKey", m_electronCollectionKey  = "" );
  declareProperty("outputTrackCollectionKey", m_outputTrackCollectionKey  = "BphysElectronCounter" );
  declareProperty("ApplyIsEM",             m_applyIsEM = true);
  declareProperty("IsEMrequiredBits",      m_IsEMrequiredBits = 0xF2);

  declareProperty("egammaElectronCutIDToolName",m_egammaElectronCutIDToolName="");
  declareProperty("AthenaElectronLHIDSelectorToolName", m_athElectronLHIDSelectorToolName="");
  declareProperty("UseAthenaElectronLHIDSelectorTool", m_useAthElectronLHIDSelector=false);
  declareProperty("LuminosityTool", m_lumiBlockMuTool, "Luminosity Tool");

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

  if (m_athElectronLHIDSelectorToolName=="") {
      ATH_MSG_DEBUG("Electron LH PID is disabled, no tool specified  "); 
       m_athElectronLHIDSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>();
  }
  else {
      m_athElectronLHIDSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>(m_athElectronLHIDSelectorToolName);
      if(m_athElectronLHIDSelectorTool.retrieve().isFailure()) {
	  ATH_MSG_ERROR("Unable to retrieve " << m_athElectronLHIDSelectorTool);
	  return HLT::BAD_JOB_SETUP; 

	  // For now, just try to retrieve the lumi tool, It is used only in LH, so retrieve only if needed
	  if (m_lumiBlockMuTool.retrieve().isFailure()) {
	    ATH_MSG_WARNING("Unable to retrieve Luminosity Tool");
	  } else {
	    ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
	  }
      } 
      else{
          ATH_MSG_DEBUG("Tool " << m_athElectronLHIDSelectorTool << " retrieved");
      }
  }
     
     
     ATH_MSG_INFO("require at least "<< m_nEfElectron <<" EF Electrons from collection " << m_electronCollectionKey  << ", with pts: ");
     if(msgLvl() <= MSG::INFO){	
        for(float pt :  m_ptElectronMin){ msg() << MSG::INFO << pt <<", ";}
        msg() << MSG::INFO << endmsg;
     }
     if( m_applyIsEM )  {
        ATH_MSG_INFO(" Electrons should have  isEM bits " << std::hex << "0x" << m_IsEMrequiredBits << std::dec);
      }else{
        ATH_MSG_INFO(" no isEM PID cuts are applied ");
       }
     
   ATH_MSG_INFO(" Overlap removal dR<"<<m_mindR);

  if ( timerSvc() ) {
    m_BmmHypTot = addTimer("TrigBphysElectronCounter");
  }




  return HLT::OK;
}

HLT::ErrorCode TrigBphysElectronCounter::hltFinalize()
{
  ATH_MSG_INFO("in finalize()");
  MsgStream log(msgSvc(), name());
  
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigBphysElectronCounter -------------|");
  ATH_MSG_INFO("Run on events " << m_countTotalEvents );
  ATH_MSG_INFO("Passed events " << m_countPassedEvents);

  return HLT::OK;
}



HLT::ErrorCode TrigBphysElectronCounter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output)
{
  // start monitoring
  beforeExecMonitors().ignore();
  m_countTotalEvents++;  
  m_mon_Acceptance.clear();

  m_mon_Acceptance.push_back( ACCEPT_hltExecute );
  ATH_MSG_DEBUG(" In TrigBphysElectronCounter hltExecute");


  //========  check if we have enough EF electrons :  =====================
  std::vector<ElementLink<xAOD::ElectronContainer> > efelectrons; // just a collection of pointers, not copies
  int failedCut = passNObjects<xAOD::ElectronContainer, 
				   std::vector<ElementLink<xAOD::ElectronContainer> > >( m_nEfElectron, m_ptElectronMin, m_IsEMrequiredBits,
											 inputTE, efelectrons,  m_electronCollectionKey, m_mindR);
  if( failedCut != 0 ){
    if ( timerSvc() )  m_BmmHypTot->stop();
    if( failedCut == 1 ){
      ATH_MSG_DEBUG("Found "<<efelectrons.size() <<" EF electrons - fail N object cut ");
    }else if( failedCut == 2 ){
      ATH_MSG_DEBUG(" fail pt cuts ");
       m_mon_Acceptance.push_back( ACCEPT_PassNElectrons );
    }else if( failedCut == 3 ){
      ATH_MSG_DEBUG(" fail isEM cuts ");
       m_mon_Acceptance.push_back( ACCEPT_PassNElectrons );
       m_mon_Acceptance.push_back( ACCEPT_PassPtcut );
    }else {
       ATH_MSG_WARNING(" unknown return code!! Please contact developers ");
    }    
    afterExecMonitors().ignore();   
    return HLT::OK;
  }else{
    ATH_MSG_DEBUG("Found "<<efelectrons.size() <<" EF electrons, require "<<m_nEfElectron<<" - accept "); 
  }
  m_mon_Acceptance.push_back( ACCEPT_PassNElectrons );
  m_mon_Acceptance.push_back( ACCEPT_PassPtcut );
  m_mon_Acceptance.push_back( ACCEPT_PassIsEM );
  m_mon_nEFElectrons = m_nEfElectron;


  xAOD::TrackParticleContainer* outputTrackColl = new xAOD::TrackParticleContainer();
  xAOD::TrackParticleAuxContainer outputTrackCollAuxCont;
  outputTrackColl->setStore( &outputTrackCollAuxCont );
  outputTrackColl->reserve(m_nEfElectron);
    for( auto elec : efelectrons ){
      xAOD::TrackParticle *trk1 = new xAOD::TrackParticle();
      trk1->makePrivateStore( (*elec)->trackParticle());
      outputTrackColl->push_back(trk1);
    }
    ATH_MSG_DEBUG("size of output Track collection " << outputTrackColl->size() );

  
  // record collection now
  if ( timerSvc() )  m_BmmHypTot->stop();
    
  HLT::TriggerElement* outputTE = addRoI(output);     
  outputTE->setActiveState(true);
  
  m_countPassedEvents++;
  // add also containter with tracks for seeded EF Muon
  if (m_outputTrackCollectionKey!= "" && outputTrackColl && outputTrackColl->size()) {
    ATH_MSG_DEBUG("REGTEST: Store Bphys track Collection size: " << outputTrackColl->size() );
    HLT::ErrorCode sc = attachFeature(outputTE, outputTrackColl, m_outputTrackCollectionKey );
    if(sc != HLT::OK) {
      ATH_MSG_ERROR("Failed to store bphys track  Collection" );
      delete outputTrackColl; outputTrackColl = nullptr; // assume deletion responsibility
      afterExecMonitors().ignore();   
      return HLT::ERROR;
    }
  } else {
    ATH_MSG_DEBUG("REGTEST: no bphys track collection to store "  );
    delete outputTrackColl; outputTrackColl = nullptr;
  }


  // stop monitoring
  afterExecMonitors().ignore();   
  return HLT::OK;
}




