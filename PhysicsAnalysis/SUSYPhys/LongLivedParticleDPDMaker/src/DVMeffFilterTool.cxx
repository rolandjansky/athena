/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DVMissingETFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/DVMeffFilterTool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::DVMeffFilterTool::DVMeffFilterTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_MeffCut(1000000.),
  m_METoverMeffCutMin(0.3),
  m_METoverMeffCutMax(0.7),
  m_jetPtCut(40000.),
  m_jetEtaCut(2.7),
  m_METCut(80000.)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("MeffCut", m_MeffCut);
    declareProperty("jetPtCut", m_jetPtCut);
    declareProperty("jetEtaCut", m_jetEtaCut);
    declareProperty("METoverMeffCutMin", m_METoverMeffCutMin);
    declareProperty("METoverMeffCutMax", m_METoverMeffCutMax);
    declareProperty("METCut",m_METCut);
  }
  
// Destructor
DerivationFramework::DVMeffFilterTool::~DVMeffFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DVMeffFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_metSGKey.initialize());
     ATH_CHECK(m_jetSGKey.initialize());
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
     
     SG::ReadHandle<xAOD::MissingETContainer> metContainer(m_metSGKey);
     if( !metContainer.isValid() ) {
       msg(MSG::WARNING) << "No MET container found, will skip this event" << endmsg;
       return false;
     } 
     ///     msg(MSG::INFO)<<"size of  MET container is "<<metContainer->size()<<endmsg;

     if (metContainer->size() >= 1) {
       MET = metContainer->at(0)->met();
     }
  
     SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetSGKey);
     if( !jetContainer.isValid() ) {
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
  
