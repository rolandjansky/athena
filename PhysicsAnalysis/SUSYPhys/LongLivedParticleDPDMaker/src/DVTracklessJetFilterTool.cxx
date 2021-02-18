/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DVTracklessJetFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/DVTracklessJetFilterTool.h"
#include <vector>
#include <string>


// Constructor
DerivationFramework::DVTracklessJetFilterTool::DVTracklessJetFilterTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_ptCut(50000.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("JetPtCut", m_ptCut);	
    declareProperty("JetEtaCut", m_etaCut);	
    declareProperty("sumPtTrkCut", m_sumPtTrkCut);	
    declareProperty("nJetsRequired", m_nJetsRequired=1);
  }
  
// Destructor
DerivationFramework::DVTracklessJetFilterTool::~DVTracklessJetFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DVTracklessJetFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_jetSGKey.initialize());
     
     return StatusCode::SUCCESS;
     
}
StatusCode DerivationFramework::DVTracklessJetFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::DVTracklessJetFilterTool::eventPassesFilter() const
{
  ++m_ntot;
  bool passesEvent=false;

  int nJetsPassed=0;
  
  SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetSGKey); 
  if( !jetContainer.isValid() ) {
    msg(MSG::WARNING) << "No Jet container found, will skip this event" << endmsg;
    return false;
  }
  msg(MSG::DEBUG)<<"size of  Jet container is "<<jetContainer->size()<<endmsg;
  
  for (unsigned int i=0; i< jetContainer->size(); ++i) { 
    const xAOD::Jet* jet = jetContainer->at(i);
    if (( jet->pt() < m_ptCut) || (fabs(jet->eta())>m_etaCut))  continue;
    
    std::vector<float> sumPtTrkvec;
    jet->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec);
    if (sumPtTrkvec.size() > 0) {
      msg(MSG::DEBUG)<<"sumptTrk is "<<sumPtTrkvec.at(0)<<endmsg;
      if (sumPtTrkvec.at(0) < m_sumPtTrkCut) {
	nJetsPassed+=1;
      }    
    }
  }
  if (nJetsPassed >=m_nJetsRequired) {
    ++m_npass;
    passesEvent=true;
  }
  
  return passesEvent;
  
}  
  
