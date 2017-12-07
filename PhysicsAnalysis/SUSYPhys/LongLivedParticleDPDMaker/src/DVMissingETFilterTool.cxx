/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DVMissingETFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/DVMissingETFilterTool.h"
#include <vector>
#include <string>
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"

// Constructor
DerivationFramework::DVMissingETFilterTool::DVMissingETFilterTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_metSGKey("MET_LocHadTopo"),
  m_metCut(50000.0),
  m_jetPtCut(50000.0),
  m_applyDeltaPhiCut(false),
  m_deltaPhiCut(2.),
  m_jetSGKey("AntiKt4LCTopoJets")
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("METContainerKey", m_metSGKey);
    declareProperty("METCut", m_metCut);
    declareProperty("JetPtCut", m_jetPtCut);
    declareProperty("applyDeltaPhiCut", m_applyDeltaPhiCut);
    declareProperty("DeltaPhiCut", m_deltaPhiCut);
    declareProperty("JetContainerKey", m_jetSGKey);
  }
  
// Destructor
DerivationFramework::DVMissingETFilterTool::~DVMissingETFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DVMissingETFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::DVMissingETFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::DVMissingETFilterTool::eventPassesFilter() const
{
     ++m_ntot;

     const xAOD::MissingETContainer* metContainer(0);
     StatusCode sc=evtStore()->retrieve(metContainer,m_metSGKey);
     if( sc.isFailure()  ||  !metContainer ) {
       msg(MSG::WARNING) << "No MET container found, will skip this event" << endmsg;
       return false;
     } 
     ///  msg(MSG::INFO)<<"size of  MET container is "<<metContainer->size()<<endmsg;

     if (metContainer->size() ==1) {
	 
       double MET = metContainer->at(0)->met();
       bool passesEvent= (MET > m_metCut) ;


       
       if (m_applyDeltaPhiCut) {
	 double phiMET=metContainer->at(0)->phi();
	 const xAOD::JetContainer* jetContainer(0);
	 StatusCode sc=evtStore()->retrieve(jetContainer,m_jetSGKey);
	 if( sc.isFailure()  ||  !jetContainer ) {
	   msg(MSG::WARNING) << "No jet container found, will skip this event" << endmsg;
	   return false;
	 }
	 if ((jetContainer->size() > 0) && (jetContainer->at(0)->pt() > m_jetPtCut)){
	   double phiJet = jetContainer->at(0)->phi();
	   double deltaPhi = fabs(phiMET-phiJet);
	   if (deltaPhi > M_PI) deltaPhi = 2.0*M_PI - deltaPhi;
	   passesEvent = passesEvent && (deltaPhi > m_deltaPhiCut);
	 }
       }
       
       if (passesEvent) {
	 ++m_npass;
       }
       return passesEvent;
     }
     return false;

}  
  
