/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AsgSelectionToolWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/AsgSelectionToolWrapper.h"
#include "xAODBase/IParticleContainer.h"
#include "PATCore/AcceptData.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  AsgSelectionToolWrapper::AsgSelectionToolWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_cut(""),
    m_sgName(""),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("AsgSelectionTool", m_tool);
    declareProperty("CutType", m_cut);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode AsgSelectionToolWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of invariant mass tool!");
      return StatusCode::FAILURE;
    }
    CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode AsgSelectionToolWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode AsgSelectionToolWrapper::addBranches() const
  {
    // retrieve container
    const xAOD::IParticleContainer* particles = evtStore()->retrieve< const xAOD::IParticleContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    // Decorator
    SG::AuxElement::Decorator< char > decorator(m_sgName);
    
    // Write mask for each element and record to SG for subsequent selection
    for (xAOD::IParticleContainer::const_iterator pItr = particles->begin(); pItr!=particles->end(); ++pItr) {
      auto theAccept = m_tool->accept(*pItr);  // asg::AcceptData or TAccept
      if(m_cut==""){
	bool pass_selection = (bool) theAccept;
	if(pass_selection) decorator(**pItr) = 1;
	else decorator(**pItr) = 0;
      }
      else{
	if (theAccept.getCutResult(m_cut)) {
	  decorator(**pItr) = 1;
	} else {
	  decorator(**pItr) = 0;
	}
      }
    }
        
    return StatusCode::SUCCESS;
  }
}
