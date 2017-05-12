/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiClusterSingleHitTool.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Implementation file for AFPSiClusterSingleHitTool used for clustering hits.
 */


#include "AFPSiClusterSingleHitTool.h"
#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"
#include "xAODForward/AFPClusterAlgID.h"
#include "xAODForward/AFPSiHit.h"

// STL includes
#include <list>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

AFPSiClusterSingleHitTool::AFPSiClusterSingleHitTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : ::AthAlgTool(type, name, parent) {
  declareInterface<IAFPSiClusterAlgTool>(this);

  declareProperty("chargeThreshold",
                  m_chargeThreshold = 1000,
                  "Charge above which hits are used for clustering");
}

StatusCode AFPSiClusterSingleHitTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterSingleHitTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterSingleHitTool::doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters)
{
  for (const xAOD::AFPSiHit* theHit : hits)
    if (theHit->depositedCharge() > m_chargeThreshold)
      outputClusters.emplace_back(theHit, xAOD::AFPClusterAlgID::singleHit);

  return StatusCode::SUCCESS;
}
