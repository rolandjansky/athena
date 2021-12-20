/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_IAFPSIROWCOLTOLOCALCSTOOL_H
#define AFP_SICLUSTERTOOLS_IAFPSIROWCOLTOLOCALCSTOOL_H 1

/**
 * @file   IAFPSiRowColToLocalCSTool.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-24
 * 
 * @brief  Header file for interface IAFPSiRowColToLocalCSTool.
 */

// STL includes
#include <list>

// using ROOT transformation, because they allow to set the matrix manually
#include "Math/Transform3D.h"

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"

#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"

/// Interface for tools that translate pixels columns and rows to local position in station and saves output in xAOD format.
class IAFPSiRowColToLocalCSTool : virtual public ::IAlgTool
{ 

 public: 

  DeclareInterfaceID(IAFPSiRowColToLocalCSTool, 1, 0);

  /** 
   * @brief Make xAOD::AFPSiHitsCluster with local position.
   * 
   * @param[in] stationID ID number of station where the layer is
   * @param[in] layerID ID number of the layer with for which position is to be calculated
   * @param[in] cluster cluster to be saved in xAOD::AFPSiHitsCluster
   * @param[out] xAODContainer xAOD container to which the new xAOD cluster is added
   * 
   * @return xAOD cluster object with local coordinates which is added to the xAODContainer
   */
  virtual xAOD::AFPSiHitsCluster* newXAODLocal (const int stationID, const int layerID, const AFPSiClusterBasicObj& cluster, std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& xAODContainer) const = 0;

}; 


#endif //> !AFP_SICLUSTERTOOLS_IAFPSIROWCOLTOLOCALCSTOOL_H
