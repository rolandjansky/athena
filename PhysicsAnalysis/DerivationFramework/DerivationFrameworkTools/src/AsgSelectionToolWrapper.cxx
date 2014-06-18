/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AsgSelectionToolWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/AsgSelectionToolWrapper.h"
#include "xAODBase/IParticleContainer.h"
#include "PATCore/TAccept.h"
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
    std::vector<int> *mask = new std::vector<int>();
    // retrieve container
    const xAOD::IParticleContainer* particles = evtStore()->retrieve< const xAOD::IParticleContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    // Write mask for each element and record to SG for subsequent selection
    for (xAOD::IParticleContainer::const_iterator pItr = particles->begin(); pItr!=particles->end(); ++pItr) {
      Root::TAccept theAccept(m_tool->accept(*pItr)); 
      if(m_cut==""){
	bool pass_selection = (bool) theAccept;
	if(pass_selection) mask->push_back(1);
	else mask->push_back(0);
      }
      else{
	if (theAccept.getCutResult(m_cut)) {
	  mask->push_back(1);
	} else {
	  mask->push_back(0);
	}
      }
    }
        
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }
    CHECK(evtStore()->record(mask, m_sgName));      
    return StatusCode::SUCCESS;
  }  

}
