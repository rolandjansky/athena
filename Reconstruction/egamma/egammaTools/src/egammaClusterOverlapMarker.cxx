/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaClusterOverlapMarker.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "egammaTopoClusterMap.h"
#include "xAODEgamma/Egamma.h"

#include "CandidateMatchHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloUtils/CaloCellList.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

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

  declareProperty("dEta",
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

  const xAOD::ElectronContainer    *elecs = 0;
  const xAOD::CaloClusterContainer *tcClusters = 0;
  
  StatusCode sc = evtStore()->retrieve(elecs, m_elecName);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input electron container found!");
    return StatusCode::SUCCESS;
  }
  
  sc = evtStore()->retrieve(tcClusters, m_tcName);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input EM topocluster container found!");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Retrieved cluster containers for overlap check!");
  
  float pi(4*atan(1));

  xAOD::ElectronContainer::const_iterator    elIter = elecs->begin();
  for (; elIter != elecs->end(); ++elIter) {

    int   seedIndex((*elIter)->caloCluster()->auxdata<int>("seedIndex"));
    const xAOD::CaloCluster *seedClus = tcClusters->at(seedIndex);
      
    //Need to get seed cluster info from electron.
    float dEta(fabs(seedClus->eta()-clus->eta()));
    float dPhi(remainder(fabs(seedClus->phi()-clus->phi()),2*pi));
      
    static SG::AuxElement::Decorator<bool> overlapSC ("overlapSC");
    if (dEta < m_dEta && dPhi < m_dPhi) {
      ATH_MSG_DEBUG(Form("SW   cluster with eta: %f, phi: %f",clus->eta(),clus->phi()));
      ATH_MSG_DEBUG(Form("Seed cluster overlaps with eta: %f, phi: %f",seedClus->eta(),seedClus->phi()));

      overlapSC(*clus) = true;
      break;
    }
    else {
      overlapSC(*clus) = false;
    }
  }

  ATH_MSG_DEBUG("Done overlap check!");

  return StatusCode::SUCCESS;

}
