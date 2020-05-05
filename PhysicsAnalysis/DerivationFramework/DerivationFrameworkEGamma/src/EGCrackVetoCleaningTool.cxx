/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGCrackVetoCleaningTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//

#include "DerivationFrameworkEGamma/EGCrackVetoCleaningTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace DerivationFramework {

  EGCrackVetoCleaningTool::EGCrackVetoCleaningTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_sgName(""),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("ContainerName", m_containerName);
  }

  StatusCode EGCrackVetoCleaningTool::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of EGCrackVetoCleaningTool!");
      return StatusCode::FAILURE;
    }
    if (m_containerName!="Photons" && m_containerName!="Electrons" && m_containerName!="ForwardElectrons") {
      ATH_MSG_ERROR("Wrong container provided!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode EGCrackVetoCleaningTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode EGCrackVetoCleaningTool::addBranches() const
  {
    // retrieve container
    const xAOD::EgammaContainer* particles = evtStore()->retrieve< const xAOD::EgammaContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }

    // Decorator
    SG::AuxElement::Decorator< char > decoratorPass(m_sgName);

    // In principle could use meta-data to do the loop only if mc16a/data15+16
    
    // Write mask for each element and record to SG for subsequent selection
    for (xAOD::EgammaContainer::const_iterator pItr = particles->begin(); pItr!=particles->end(); ++pItr) {

      xAOD::Type::ObjectType type = (*pItr)->type();
      if (type!=xAOD::Type::Electron && type!=xAOD::Type::Photon) {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type (not electron nor photon) being passed to EGCrackVetoCleaningTool");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Electron && m_containerName!="Electrons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGCrackVetoCleaningTool");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Photon && m_containerName!="Photons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGCrackVetoCleaningTool");
	  return StatusCode::FAILURE;
      }
      
      const xAOD::Egamma* pCopy = *pItr;

      const xAOD::CaloCluster *clu = pCopy->caloCluster();
      bool maybeBug = clu->hasSampling(CaloSampling::EMB2) && clu->hasSampling(CaloSampling::EME2);
      const std::vector<const xAOD::CaloCluster*> assocC = xAOD::EgammaHelpers::getAssociatedTopoClusters(clu);
      double dRsatMax = -1.;
      if (assocC.size() > 1) {
        for (auto sclu : assocC) {
	  double dR = clu->p4().DeltaR(sclu->p4());
	  if (dR > dRsatMax)
	    dRsatMax = dR;
	}
      }
      decoratorPass(*pCopy) = !(maybeBug && assocC.size() > 1 && dRsatMax > 0.16); // (or dPhi > 0.15 better ?)
    }
    
    return StatusCode::SUCCESS;
  }
}
