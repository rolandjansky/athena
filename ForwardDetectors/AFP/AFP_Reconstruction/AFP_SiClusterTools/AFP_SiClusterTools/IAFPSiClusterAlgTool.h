/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_IAFPSICLUSTERALGTOOL_H
#define AFP_SICLUSTERTOOLS_IAFPSICLUSTERALGTOOL_H 1

/**
 * @file   IAFPSiClusterAlgTool.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Header file for interface IAFPSiClusterAlgTool.
 */


// STL includes
#include <list>


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

#include "xAODForward/AFPSiHit.h"
#include "AFPSiClusterBasicObj.h"

/// Interface for tools with algorithms that clusters silicon pixel hits.
class IAFPSiClusterAlgTool : virtual public ::IAlgTool
{ 

 public:
  DeclareInterfaceID(IAFPSiClusterAlgTool, 1, 0);

  /** @brief Reconstruct clusters
   * 
   * This method does actual hits clustering. 
   *
   * @param[in] hits list of hits to be clustered
   * @param[out] outputClusters list of clusters created by the algorithm
   */
  virtual StatusCode doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) const = 0;

}; 

#endif //> !AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
