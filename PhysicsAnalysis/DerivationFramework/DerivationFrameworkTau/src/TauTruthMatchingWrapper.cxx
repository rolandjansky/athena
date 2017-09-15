/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauTruthMatchingWrapper.cxx
// Author: Evelina Bouhova-Thacker (e.bouhova@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTau/TauTruthMatchingWrapper.h"
#include "xAODTracking/Vertex.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "xAODTau/TauJetContainer.h"

namespace DerivationFramework {

  TauTruthMatchingWrapper::TauTruthMatchingWrapper(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tauContainerName("TauJets"),
    m_tTauTruthMatchingTool("TauAnalysisTools::TauTruthMatchingTool")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TauContainerName", m_tauContainerName);
    declareProperty("TauTruthMatchingTool", m_tTauTruthMatchingTool);
  }

  StatusCode TauTruthMatchingWrapper::initialize()
  {
    CHECK( m_tTauTruthMatchingTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TauTruthMatchingWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauTruthMatchingWrapper::addBranches() const
  {
    // retrieve container
    const xAOD::TauJetContainer* xTauContainer = evtStore()->retrieve< const xAOD::TauJetContainer >( m_tauContainerName );
    if( ! xTauContainer ) {
      ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_tauContainerName );
      return StatusCode::FAILURE;
    }

    for(auto xTau : *xTauContainer)
      m_tTauTruthMatchingTool->applyTruthMatch(*xTau);
    
    return StatusCode::SUCCESS;
  }  
}
