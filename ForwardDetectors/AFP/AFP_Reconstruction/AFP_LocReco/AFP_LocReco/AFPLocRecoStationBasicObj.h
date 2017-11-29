/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_LOCRECO_AFPLOCRECOSTATIONBASICOBJ_H
#define AFP_LOCRECO_AFPLOCRECOSTATIONBASICOBJ_H 1

/// @file   AFPLocRecoStationBasicObj.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-04-29
/// 
/// @brief  Full implementation of class AFPLocRecoStationBasicObj.


#include<vector>

#include "xAODForward/AFPSiHitsCluster.h"

/// @brief Helper class representing an AFP station used in track reconstruction.
///
/// Main purpose is to divide clusters into stations and planes. It is
/// just a wrapper for a 2D array of hits clusters.
class AFPLocRecoStationBasicObj
{
public:
  /// Does nothing
  AFPLocRecoStationBasicObj () {}

  /// Sets number of layers in the station
  AFPLocRecoStationBasicObj (const int numberOfLayers) {m_layersClusters.resize(numberOfLayers);}
  
  /// Returns vector layers, each layer is a vector of clusters
  std::vector< std::vector<const xAOD::AFPSiHitsCluster*> >& layers ()
  {return m_layersClusters;}

  /// Returns vector layers, each layer is a vector of clusters
  const std::vector< std::vector<const xAOD::AFPSiHitsCluster*> >& layers () const
  {return m_layersClusters;}

  /// @brief Returns vector of clusters in the layer with given ID.
  ///
  /// Throws out_of_range exception if layer ID is larger than size of the vector of layers.
  std::vector<const xAOD::AFPSiHitsCluster*>& clustersInLayer (const unsigned int layerID)
  {return m_layersClusters.at(layerID);}
  
  /// Clear clusters from each layer.
  void clear()
  { for (std::vector<const xAOD::AFPSiHitsCluster*>& layer : m_layersClusters) layer.clear();}

  /// Sets vector containing layers to the specified size
  void setNumberOfLayers(const unsigned int layersN)
  {m_layersClusters.resize(layersN);}
  
protected:
  std::vector< std::vector<const xAOD::AFPSiHitsCluster*> > m_layersClusters; ///< vector of layers, each layer contains a vector of pointers to clusters
};

#endif
