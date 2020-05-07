/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGPhotonCleaningWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//

#include "DerivationFrameworkEGamma/EGPhotonCleaningWrapper.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include <EgammaAnalysisHelpers/PhotonHelpers.h>

namespace DerivationFramework {

  EGPhotonCleaningWrapper::EGPhotonCleaningWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_sgName("DFCommonPhotonsCleaning"),
    m_containerName("Photons")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("EGammaFudgeMCTool", m_fudgeMCTool);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode EGPhotonCleaningWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of EGPhotonCleaningWrapper");
      return StatusCode::FAILURE;
    }
    if (m_fudgeMCTool.name()!="") CHECK(m_fudgeMCTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode EGPhotonCleaningWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode EGPhotonCleaningWrapper::addBranches() const
  {
    // retrieve container
    const xAOD::PhotonContainer* photons = evtStore()->retrieve< const xAOD::PhotonContainer >( m_containerName );
    if( ! photons ) {
        ATH_MSG_ERROR ("Couldn't retrieve photons with key: " << m_containerName );
        return StatusCode::FAILURE;
    }

    // Decorator
    SG::AuxElement::Decorator< char > decoratorPass(m_sgName);
    SG::AuxElement::Decorator< char > decoratorPassDelayed(m_sgName+"NoTime");
    
    // Write mask for each element and record to SG for subsequent selection
    for (xAOD::PhotonContainer::const_iterator phItr = photons->begin(); phItr!=photons->end(); ++phItr) {
      
      bool passSelection = false;
      bool passSelectionDelayed = false;

      bool applyFF = (!m_fudgeMCTool.empty());

      if (!applyFF) {
	passSelection = PhotonHelpers::passOQquality(*phItr);
	passSelectionDelayed = PhotonHelpers::passOQqualityDelayed(*phItr);
      }
      else {
	// apply the shower shape corrections
	CP::CorrectionCode correctionCode = CP::CorrectionCode::Ok;
	xAOD::Photon* ph = 0;
	correctionCode = m_fudgeMCTool->correctedCopy(**phItr, ph);
	if (correctionCode==CP::CorrectionCode::Ok) {
	  passSelection = PhotonHelpers::passOQquality(ph);
	  passSelectionDelayed = PhotonHelpers::passOQqualityDelayed(ph);
	}
	else if (correctionCode==CP::CorrectionCode::Error) {
	    Error("addBranches()","Error applying fudge factors to current photon");
	}
	else if (correctionCode==CP::CorrectionCode::OutOfValidityRange) {
	    Warning("addBranches()","Current photon has no valid fudge factors due to out-of-range");
	}
	else {
	    Warning("addBranches()","Unknown correction code %d from ElectronPhotonShowerShapeFudgeTool",(int) correctionCode);
	}
	delete ph;
      }

      // decorate the original object
      if (passSelection)
	decoratorPass(**phItr) = 1;
      else 
	decoratorPass(**phItr) = 0;

      if (passSelectionDelayed) 
	decoratorPassDelayed(**phItr) = 1;
      else 
	decoratorPassDelayed(**phItr) = 0;
    }
    
    return StatusCode::SUCCESS;
  }
}
