/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DVMissingETFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/DVMeffFilterTool.h"
#include <vector>
#include <string>
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"

// Constructor
DerivationFramework::DVMeffFilterTool::DVMeffFilterTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_metSGKey("MET_Calo"),
  m_jetSGKey("AntiKt4LCTopoJets"),
  m_MeffCut(1000000.),
  m_METoverMeffCutMin(0.3),
  m_METoverMeffCutMax(0.7),
  m_jetPtCut(40000.),
  m_jetEtaCut(2.7),
  m_METCut(80000.)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("METContainerKey", m_metSGKey);
    declareProperty("MeffCut", m_MeffCut);
    declareProperty("jetPtCut", m_jetPtCut);
    declareProperty("jetEtaCut", m_jetEtaCut);
    declareProperty("METoverMeffCutMin", m_METoverMeffCutMin);
    declareProperty("METoverMeffCutMax", m_METoverMeffCutMax);
    declareProperty("METCut",m_METCut);
    declareProperty("JetContainerKey", m_jetSGKey);
  }
  
// Destructor
DerivationFramework::DVMeffFilterTool::~DVMeffFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DVMeffFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::DVMeffFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::DVMeffFilterTool::eventPassesFilter() const
{
     ++m_ntot;

     double Meff=0.;
     double MET=0.;
     double totalJetPT = 0.;
     bool passesEvent=false;
     
     const xAOD::MissingETContainer* metContainer(0);
     StatusCode sc=evtStore()->retrieve(metContainer,m_metSGKey);
     if( sc.isFailure()  ||  !metContainer ) {
       msg(MSG::WARNING) << "No MET container found, will skip this event" << endmsg;
       return false;
     } 
     ///     msg(MSG::INFO)<<"size of  MET container is "<<metContainer->size()<<endmsg;

     if (metContainer->size() >= 1) {
       MET = metContainer->at(0)->met();
     }
  
     const xAOD::JetContainer* jetContainer(0);
     sc=evtStore()->retrieve(jetContainer,m_jetSGKey);
     if( sc.isFailure()  ||  !jetContainer ) {
       msg(MSG::WARNING) << "No jet container found, will skip this event" << endmsg;
       return false;
     }
     for (unsigned int i=0; i< jetContainer->size(); ++i) { 
       const xAOD::Jet* jet = jetContainer->at(i);
       if (( jet->pt() < m_jetPtCut) || (fabs(jet->eta())>m_jetEtaCut))  continue;
       totalJetPT += jet->pt();
     }
     
     Meff += MET;
     Meff += totalJetPT;

     
     if ((Meff > m_MeffCut) || ((MET > m_METCut) && (MET/Meff > m_METoverMeffCutMin ) && (MET/Meff < m_METoverMeffCutMax)))  {   //// NOTE: OR of these two requirements
       passesEvent=true;
       ++m_npass;
     }
     /// msg(MSG::INFO)<<" MET "<< MET<< " totalJetPT "<<totalJetPT<<" Meff "<<Meff<<" ratio "<< MET/Meff <<" "<<passesEvent<<endmsg;



     return passesEvent;


}  
  
