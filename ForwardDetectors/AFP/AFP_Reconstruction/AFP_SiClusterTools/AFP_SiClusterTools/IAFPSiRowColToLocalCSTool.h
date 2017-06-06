/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

static const InterfaceID IID_IAFPSiRowColToLocalCSTool("IAFPSiRowColToLocalCSTool", 1, 0);

/// Interface for tools that translate pixels columns and rows to local position in station and saves output in xAOD format.
class IAFPSiRowColToLocalCSTool : virtual public ::IAlgTool
{ 

 public: 
  /// Empty destructor
  virtual ~IAFPSiRowColToLocalCSTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

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
  virtual xAOD::AFPSiHitsCluster* newXAODLocal (const int stationID, const int layerID, const AFPSiClusterBasicObj& cluster, xAOD::AFPSiHitsClusterContainer* xAODContainer) = 0;

  /** 
   * @brief Fill xAOD::AFPSiHitsClusterContainer with clusters.
   * 
   * @param[in] stationID ID number of station where the layer is
   * @param[in] layerID ID number of the layer with for which position is to be calculated
   * @param[in] clusters clusters for which local position is to be calculated and are to be added to xAODContainer
   * @param[out] xAODContainer container to be filled with new clusters
   */
  virtual void fillXAODContainer (const int stationID, const int layerID, const std::list<AFPSiClusterBasicObj>& clusters, xAOD::AFPSiHitsClusterContainer* xAODContainer) = 0;


  //  virtual  std::vector< std::vector<HepGeom::Transform3D> >& transformations () = 0;
  virtual  std::vector< std::vector<ROOT::Math::Transform3D> >& transformations () = 0;
}; 

inline const InterfaceID& IAFPSiRowColToLocalCSTool::interfaceID() 
{ 
   return IID_IAFPSiRowColToLocalCSTool; 
}


#endif //> !AFP_SICLUSTERTOOLS_IAFPSIROWCOLTOLOCALCSTOOL_H
