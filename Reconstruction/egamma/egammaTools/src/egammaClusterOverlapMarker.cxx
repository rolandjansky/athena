/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaClusterOverlapMarker.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Egamma.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
egammaClusterOverlapMarker::egammaClusterOverlapMarker(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  AthAlgTool(type, name, parent)

{
  // Declare interface & properties.
  declareInterface<IegammaClusterOverlapMarker>(this);

  //Overlap cuts.
  declareProperty("dEta",
		  m_dEta = 0.05);

  declareProperty("dPhi",
		  m_dPhi = 0.1);

  //Object containers.
  declareProperty("ElectronName",
		  m_elecName = "Electrons");
  declareProperty("TopoClusterName",
		  m_tcName = "EMTopoCluster430");

}

//Destructor.
egammaClusterOverlapMarker::~egammaClusterOverlapMarker() {}

StatusCode egammaClusterOverlapMarker::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode egammaClusterOverlapMarker::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode egammaClusterOverlapMarker::execute(const xAOD::CaloCluster *clus)
					      {

  xAOD::ElectronContainer *elecs = 0;
  const xAOD::CaloClusterContainer *tcClusters = 0;
  
  StatusCode sc = evtStore()->retrieve(elecs, m_elecName);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input electron container found!");
    return StatusCode::FAILURE;
  }
  
  sc = evtStore()->retrieve(tcClusters, m_tcName);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input EM topocluster container found!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Retrieved cluster containers for overlap check!");
  
  ElementLink<xAOD::ElectronContainer> nullLink;

  for (unsigned int elIndex(0); elIndex < elecs->size(); elIndex++) {

    xAOD::Electron *elIter = elecs->at(elIndex);

    int   seedIndex(elIter->caloCluster()->auxdata<int>("seedIndex"));
    const xAOD::CaloCluster *seedClus = tcClusters->at(seedIndex);
      
    //Need to get seed cluster info from electron.
    float dEta(fabs(seedClus->eta()-clus->eta()));
    float dPhi(remainder(fabs(seedClus->phi()-clus->phi()),2*M_PI));
      
    ElementLink<xAOD::ElectronContainer> linkToEl(*elecs, elIndex);
    static SG::AuxElement::Decorator<ElementLink<xAOD::ElectronContainer> > overlapLink ("overlapLink");
    static SG::AuxElement::Decorator<bool> overlapSC ("overlapSC");

    if (dEta < m_dEta && dPhi < m_dPhi) {
      ATH_MSG_DEBUG(Form("SW   cluster with eta: %f, phi: %f",clus->eta(),clus->phi()));
      ATH_MSG_DEBUG(Form("Seed cluster overlaps with eta: %f, phi: %f",seedClus->eta(),seedClus->phi()));
      overlapLink(*clus) = linkToEl;
      overlapSC  (*clus) = true;
      break;
    }
    else {
      overlapLink(*clus) = nullLink;
      overlapSC  (*clus) = false;
    }
  }

  ATH_MSG_DEBUG("Done overlap check!");

  return StatusCode::SUCCESS;

}
