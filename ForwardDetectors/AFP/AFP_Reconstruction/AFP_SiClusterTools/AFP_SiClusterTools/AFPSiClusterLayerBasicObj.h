/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERLAYERBASICOBJ_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERLAYERBASICOBJ_H 1

/**
 * @file   AFPSiClusterBasicObj.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Full definition of AFPSiClusterBasicObj.
 */

#include <list>
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"

#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"

/**
 *  @brief Class representing basic silicon layer with pixels and
 *  their clusters to be used for creating the clusters.
 * 
 *  Simple class contains station and layer ID as well as list of hits
 *  and clusters. This a helper class for clustering algorithm.
 */
class AFPSiClusterLayerBasicObj
{
public:
  AFPSiClusterLayerBasicObj (const int stationID = -1, const int layerID = -1) : 
    m_stationID(stationID), m_layerID(layerID) {}

  /// @copydoc AFPSiClusterLayerBasicObj::m_stationID
  int stationID() const {return m_stationID;}

  void setStationID(const int stationID) {m_stationID = stationID;}

  /// @copydoc AFPSiClusterLayerBasicObj::m_layerID
  int layerID() const {return m_layerID;}

  void setLayerID(const int layerID) {m_layerID = layerID;}

  /// @copydoc AFPSiClusterLayerBasicObj::m_hits
  std::list<const xAOD::AFPSiHit*>& hits() {return m_hits;}

  /// @copydoc AFPSiClusterLayerBasicObj::m_clusters
  std::list<AFPSiClusterBasicObj>& clusters() {return m_clusters;}

  /// Clear lists of hits (#m_hits) and clusters (#m_clusters)
  void clearHitsAndClusters() {m_hits.clear(); m_clusters.clear();}

protected:
  int m_stationID;		///< ID of station where the layer is mounted
  int m_layerID;		///< ID of the layer
  std::list<const xAOD::AFPSiHit*> m_hits; ///< list of pixel hits in this layer
  std::list<AFPSiClusterBasicObj> m_clusters; ///< list of clusters
};


#endif	// AFP_SICLUSTERTOOLS_AFPSICLUSTERLAYERBASICOBJ_H
