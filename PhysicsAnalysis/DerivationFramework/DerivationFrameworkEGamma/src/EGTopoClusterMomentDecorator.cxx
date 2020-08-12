/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTopoClusterMomentDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//

#include "DerivationFrameworkEGamma/EGTopoClusterMomentDecorator.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace DerivationFramework {

  EGTopoClusterMomentDecorator::EGTopoClusterMomentDecorator(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName", m_containerName);
  }


  StatusCode EGTopoClusterMomentDecorator::initialize()
  {
    if (m_containerName!="Photons" && m_containerName!="Electrons")
    {
      ATH_MSG_ERROR("Wrong container provided!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode EGTopoClusterMomentDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }


  StatusCode EGTopoClusterMomentDecorator::addBranches() const
  {
    // retrieve container
    const xAOD::EgammaContainer* particles = evtStore()->retrieve< const xAOD::EgammaContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }


    // Decorators
    static const SG::AuxElement::Decorator< float  > decoratorEngPos("TOPOCLUS_ENG_POS");
    static const SG::AuxElement::Decorator< float  > decoratorFirstEngDens("TOPOCLUS_FIRST_ENG_DENS");
    static const SG::AuxElement::Decorator< float  > decoratorIsolation("TOPOCLUS_ISOLATION");
    static const SG::AuxElement::Decorator< float  > decoratorNBadCells("TOPOCLUS_N_BAD_CELLS");
    static const SG::AuxElement::Decorator< float  > decoratorSecondLambda("TOPOCLUS_SECOND_LAMBDA");
    static const SG::AuxElement::Decorator< float  > decoratorSecondR("TOPOCLUS_SECOND_R");
    static const SG::AuxElement::Decorator< float  > decoratorAverageLArQ("TOPOCLUS_AVG_LAR_Q");
    static const SG::AuxElement::Decorator< float  > decoratorAverageTileQ("TOPOCLUS_AVG_TILE_Q");
    static const SG::AuxElement::Decorator< float  > decoratorBadLArQFrac("TOPOCLUS_BADLARQ_FRAC");
    static const SG::AuxElement::Decorator< float  > decoratorCenterLambda("TOPOCLUS_CENTER_LAMBDA");
    static const SG::AuxElement::Decorator< float  > decoratorCenterMag("TOPOCLUS_CENTER_MAG");
    static const SG::AuxElement::Decorator< float  > decoratorEMProbability("TOPOCLUS_EM_PROBABILITY");
    static const SG::AuxElement::Decorator< float  > decoratorEngBadCells("TOPOCLUS_ENG_BAD_CELLS");
    static const SG::AuxElement::Decorator< float  > decoratorEngFracMax("TOPOCLUS_ENG_FRAC_MAX");


    // Write decorations for each element and record to SG
    for (xAOD::EgammaContainer::const_iterator pItr = particles->begin(); pItr!=particles->end(); ++pItr) {

      xAOD::Type::ObjectType type = (*pItr)->type();
      if (type!=xAOD::Type::Electron && type!=xAOD::Type::Photon) {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type (not electron nor photon) being passed to EGTopoClusterMomentDecorator");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Electron && m_containerName!="Electrons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGTopoClusterMomentDecorator");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Photon && m_containerName!="Photons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGTopoClusterMomentDecorator");
	  return StatusCode::FAILURE;
      }
      
      const xAOD::Egamma* pCopy = *pItr;
      const xAOD::CaloCluster *clu = pCopy->caloCluster();
      if (clu == nullptr) {
	ATH_MSG_ERROR ("addBranches(): egamma object has no associated caloCluster");
	return StatusCode::FAILURE;
      }
      const std::vector<const xAOD::CaloCluster*> assocC = xAOD::EgammaHelpers::getAssociatedTopoClusters(clu);
      if (assocC.size()==0) {
	ATH_MSG_ERROR ("addBranches(): egamma object has no associated TopoClusters");
	return StatusCode::FAILURE;
      }
      const xAOD::CaloCluster *topoclu = assocC[0];
      if (topoclu == nullptr) {
	ATH_MSG_ERROR ("addBranches(): egamma object has no associated TopoClusters");
	return StatusCode::FAILURE;
      }

      decoratorEngPos(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::ENG_POS);
      decoratorFirstEngDens(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::FIRST_ENG_DENS);
      decoratorIsolation(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::ISOLATION);
      decoratorNBadCells(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::N_BAD_CELLS);
      decoratorSecondLambda(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::SECOND_LAMBDA);
      decoratorSecondR(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::SECOND_R);
      decoratorAverageLArQ(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::AVG_LAR_Q);
      decoratorAverageTileQ(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::AVG_TILE_Q);
      decoratorBadLArQFrac(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::BADLARQ_FRAC);
      decoratorCenterLambda(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::CENTER_LAMBDA);
      decoratorCenterMag(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::CENTER_MAG);
      decoratorEMProbability(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::EM_PROBABILITY);
      decoratorEngBadCells(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::ENG_BAD_CELLS);
      decoratorEngFracMax(*pCopy) = topoclu->getMomentValue(xAOD::CaloCluster::ENG_FRAC_MAX);
    }
    
    return StatusCode::SUCCESS;
  }
}
