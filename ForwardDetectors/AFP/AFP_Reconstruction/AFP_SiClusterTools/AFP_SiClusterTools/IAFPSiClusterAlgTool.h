/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"

#include "xAODForward/AFPSiHit.h"
#include "AFPSiClusterBasicObj.h"

static const InterfaceID IID_IAFPSiClusterAlgTool("IAFPSiClusterAlgTool", 1, 0);

/// Interface for tools with algorithms that clusters silicon pixel hits.
class IAFPSiClusterAlgTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPSiClusterAlgTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  /** @brief Reconstruct clusters
   * 
   * This method does actual hits clustering. 
   *
   * @param[in] hits list of hits to be clustered
   * @param[out] outputClusters list of clusters created by the algorithm
   */
  virtual StatusCode doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) = 0;

}; 

inline const InterfaceID& IAFPSiClusterAlgTool::interfaceID() 
{ 
   return IID_IAFPSiClusterAlgTool; 
}


#endif //> !AFP_SICLUSTERTOOLS_IAFPSICLUSTERTOOL_H
