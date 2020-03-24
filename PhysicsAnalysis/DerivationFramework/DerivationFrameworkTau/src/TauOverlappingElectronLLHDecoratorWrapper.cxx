/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauOverlappingElectronLLHDecoratorWrapper.cxx
 * @author Z. Zinonos - zenon@cern.ch
 * @date November 2015
 */

#include "DerivationFrameworkTau/TauOverlappingElectronLLHDecoratorWrapper.h"

#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
#include "xAODTau/TauJetContainer.h"

namespace DerivationFramework {

  TauOverlappingElectronLLHDecoratorWrapper::TauOverlappingElectronLLHDecoratorWrapper(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t, n, p),
    m_tauContainerName("TauJets"),
    m_electronContainerName("Electrons"),
    m_TauOverlappingElectronLLHDecorator("TauAnalysisTools::TauOverlappingElectronLLHDecorator")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TauContainerName", m_tauContainerName);
    declareProperty("ElectronContainerName", m_electronContainerName);
    declareProperty("TauOverlappingElectronLLHDecorator", m_TauOverlappingElectronLLHDecorator);
  }

  StatusCode TauOverlappingElectronLLHDecoratorWrapper::initialize()
  {
    if (m_tauContainerName.empty()) {
      ATH_MSG_ERROR("No StoreGate key provided for tau container of the TauOverlappingElectronLLHDecorator wrapper tool!");
      return StatusCode::FAILURE;
    }

    if (m_electronContainerName.empty()) {
      ATH_MSG_ERROR("No StoreGate key provided for electron container of the TauOverlappingElectronLLHDecorator wrapper tool!");
      return StatusCode::FAILURE;
    }


    CHECK( m_TauOverlappingElectronLLHDecorator.retrieve() );

    ATH_MSG_VERBOSE("initialize() ...");

    return StatusCode::SUCCESS;
  }

  StatusCode TauOverlappingElectronLLHDecoratorWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauOverlappingElectronLLHDecoratorWrapper::addBranches() const
  {
    /** retrieve container */
    const xAOD::TauJetContainer* xTauContainer = evtStore()->retrieve< const xAOD::TauJetContainer >( m_tauContainerName );
    if( ! xTauContainer ) {
      ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_tauContainerName );
      return StatusCode::FAILURE;
    }

    for(auto xTau : *xTauContainer){
      CHECK( m_TauOverlappingElectronLLHDecorator->decorate(*xTau) );
    }

    return StatusCode::SUCCESS;
  }  
}

