/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiClusterSingleHitTool.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Implementation file for AFPSiClusterSingleHitTool used for clustering hits.
 */


#include "AFP_SiClusterTools/AFPSiClusterSingleHitTool.h"
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
    : base_class(type, name, parent) {
}

StatusCode AFPSiClusterSingleHitTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterSingleHitTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterSingleHitTool::doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) const
{
  for (const xAOD::AFPSiHit* theHit : hits)
    if (theHit->depositedCharge() > m_chargeThreshold)
      outputClusters.emplace_back(theHit, xAOD::AFPClusterAlgID::singleHit);

  return StatusCode::SUCCESS;
}
