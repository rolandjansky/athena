/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiClusterAllNeighbours
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-12-07
 * 
 * @brief  Implementation file for AFPSiClusterAllNeighbours used for clustering hits.
 */


#include "AFP_SiClusterTools/AFPSiClusterAllNeighbours.h"
#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"
#include "xAODForward/AFPClusterAlgID.h"
#include "xAODForward/AFPSiHit.h"

// STL includes
#include <list>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

AFPSiClusterAllNeighbours::AFPSiClusterAllNeighbours(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : base_class(type, name, parent) {
}

StatusCode AFPSiClusterAllNeighbours::initialize()
{
  if(m_neighbourhoodType=="x" || m_neighbourhoodType=="X")        m_doOnlyHorz=true;
  else if(m_neighbourhoodType=="xy" || m_neighbourhoodType=="XY") m_doOnlyHorz=false;
  else
  {
    ATH_MSG_ERROR("neighbourhoodType is "<<m_neighbourhoodType<<" but allowed values are only \"X\" (only long-edge) or \"XY\" (both long- and short-edge); will abort");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterAllNeighbours::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterAllNeighbours::doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) const
{
  std::list<const xAOD::AFPSiHit*> hitsAboveThreshold;

  // select only hits above charge threshold
  for (const xAOD::AFPSiHit* theHit : hits)
    if (theHit->depositedCharge() > m_chargeThreshold)
      hitsAboveThreshold.push_back(theHit);
    
  while (!hitsAboveThreshold.empty())
  {    
    // add the first hit to a new proto-cluster
    std::list<const xAOD::AFPSiHit*> currentProtoCluster{hitsAboveThreshold.front()};    
    // and remove the hit from the list
    hitsAboveThreshold.pop_front();        
        
    // for each hit in the growing list of hits in the current proto-cluster
    for(const xAOD::AFPSiHit* currHit : currentProtoCluster)
    {
      // loop over remaining hits
      for(std::list<const xAOD::AFPSiHit*>::iterator remHit=hitsAboveThreshold.begin();remHit!=hitsAboveThreshold.end();)
      {
        if(abs(currHit->pixelVertID()-(*remHit)->pixelVertID())<=1 && abs(currHit->pixelHorizID()-(*remHit)->pixelHorizID())<=1)
        {
          if(m_doOnlyHorz && currHit->pixelVertID()!=(*remHit)->pixelVertID())
          {
          	++remHit;
          }
          else
          {
            // these hits are neighbours
            currentProtoCluster.push_back(*remHit);
            hitsAboveThreshold.erase(remHit++); // move iterator first, then erase the element
          }
        }
        else
        {
          ++remHit;
        }
      }
      // end loop over remaining hits, all neighbours of the current hit are added to currentProtoCluster
      // let's continue with the next hit in currentProtoCluster, possible the one that has been just added
    }
    // reach end of currentProtoCluster, no more hits to be added
    
    // calculate positions on plane and save the cluster
    outputClusters.emplace_back(0,0,0,xAOD::AFPClusterAlgID::allNeighbours);
    AFPSiClusterBasicObj& newCluster = outputClusters.back();
    double horizID = 0.;
    double vertID  = 0.;
    double charge  = 0.;
    for (const xAOD::AFPSiHit* currHit : currentProtoCluster)
    {
      charge  += currHit->depositedCharge();
      horizID += currHit->pixelHorizID() * currHit->depositedCharge();
      vertID  += currHit->pixelVertID() * currHit->depositedCharge();
      
      newCluster.hits().push_back(currHit);
    }
    
    newCluster.setHorizID(horizID/charge);
    newCluster.setVertID(vertID/charge);
    newCluster.setCharge(charge);
  }

  
  return StatusCode::SUCCESS;
}
