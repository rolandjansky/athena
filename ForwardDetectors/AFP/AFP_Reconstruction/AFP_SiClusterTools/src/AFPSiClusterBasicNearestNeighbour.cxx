/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiClusterBasicNearestNeighbour
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-05-02
 * 
 * @brief  Implementation file for AFPSiClusterBasicNearestNeighbour used for clustering hits.
 */


#include "AFPSiClusterBasicNearestNeighbour.h"
#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"
#include "xAODForward/AFPClusterAlgID.h"
#include "xAODForward/AFPSiHit.h"

// STL includes
#include <list>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

AFPSiClusterBasicNearestNeighbour::AFPSiClusterBasicNearestNeighbour(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : ::AthAlgTool(type, name, parent) {
  declareInterface<IAFPSiClusterAlgTool>(this);

  declareProperty("chargeThreshold",
                  m_chargeThreshold = 1000,
                  "Charge above which hits are used for clustering");
}

StatusCode AFPSiClusterBasicNearestNeighbour::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterBasicNearestNeighbour::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterBasicNearestNeighbour::doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters)
{
  std::list<const xAOD::AFPSiHit*> hitsAboveThreshold;

  // select only hits above charge threshold
  for (const xAOD::AFPSiHit* theHit : hits)
    if (theHit->depositedCharge() > m_chargeThreshold)
      hitsAboveThreshold.push_back(theHit);
  
  while (!hitsAboveThreshold.empty()) {
    const xAOD::AFPSiHit* theHit = hitsAboveThreshold.front();
    hitsAboveThreshold.pop_front();	// remove current hit from the list
    
    const int pixelHorizID = theHit->pixelHorizID();
    float horizID = pixelHorizID; 	// value used to make cluster may be not integer
    float vertID = theHit->pixelVertID(); 	// value used to make cluster may be not integer
    float charge = theHit->depositedCharge();
    outputClusters.emplace_back(theHit, xAOD::AFPClusterAlgID::nearestNeighbour);

    const std::list<const xAOD::AFPSiHit*>::iterator neighbourEnd = hitsAboveThreshold.end();
    std::list<const xAOD::AFPSiHit*>::iterator neighbourIter = hitsAboveThreshold.begin();
    while (neighbourIter != neighbourEnd) {
      const xAOD::AFPSiHit* neighbour = *neighbourIter;
      // check if pixels are next to each other
      if (abs(neighbour->pixelHorizID()-pixelHorizID) == 1) {
	const float neighbourCharge = neighbour->depositedCharge();
	// charge weighted mean position
	horizID = (horizID*charge + neighbour->pixelHorizID()*neighbourCharge)/(charge + neighbourCharge);
	vertID = (vertID*charge + neighbour->pixelVertID()*neighbourCharge)/(charge + neighbourCharge);

	charge += neighbourCharge;

	hitsAboveThreshold.erase(neighbourIter);

	// update cluster before finishing the loop
	AFPSiClusterBasicObj& theCluster = outputClusters.back();
	theCluster.setHorizID(horizID);
	theCluster.setVertID(vertID);
	theCluster.setCharge(charge);
	theCluster.hits().push_back(neighbour);
	break;
      }
      ++neighbourIter;
    }
    
  }

  
  return StatusCode::SUCCESS;
}
