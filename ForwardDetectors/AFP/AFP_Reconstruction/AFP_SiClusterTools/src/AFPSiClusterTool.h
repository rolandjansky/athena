/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERTOOL_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERTOOL_H 1

/**
 * @file   AFPSiClusterTool.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Header file for AFPSiClusterTool used for clustering hits.
 */

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h" 

#include "AFP_SiClusterTools/IAFPSiClusterTool.h"
#include "AFP_SiClusterTools/IAFPSiClusterAlgTool.h"
#include "AFP_SiClusterTools/AFPSiClusterLayerBasicObj.h"
#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"

/**
 * @brief This class reconstructs clusters of pixels.
 * 
 * The clustering algorithm is defined in a tool that is used and can
 * be changed. The 
 */
class AFPSiClusterTool
  : virtual public ::IAFPSiClusterTool,
    public ::AthAlgTool
{
public:
  AFPSiClusterTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  // Athena algtool's Hooks
  virtual StatusCode initialize();

  /// does nothing
  virtual StatusCode finalize();

  /// @copydoc IAFPSiClusterTool::clusterHits()
  StatusCode clusterHits();

private:
  std::string m_hitsContainerName;
  std::string m_clustersContainerName;
  
  ToolHandle<IAFPSiClusterAlgTool> m_clusterAlgToolHandle; ///< tool that does the clustering on hits
  ToolHandle<IAFPSiRowColToLocalCSTool> m_rowColToLocalCSToolHandle; ///< tool that creates xAOD and transforms rows and pixels to local coordinate system
  std::vector< std::vector<AFPSiClusterLayerBasicObj> > m_layers; ///< Array of layers with hits and clusters. Default is 4 stations times 4 layers.

  const xAOD::AFPSiHitContainer* m_hitsContainer; ///< container of pixels used to form the cluster

  /// @brief Array defining number of stations and layers in each station
  ///
  /// The size of the array in argument defines number of stations,
  /// the value in each entry defines number of layers in the station. Default is 4 stations times 4 layers.
  std::vector<int> m_numberOfLayersInStations;

  /// Runs AFPSiClusterLayerBasicObj::clearHitsAndClusters() on all layers in #m_layers
  void clearAllLayers();

  /// Reads hits from StoreGate and saves them to appropriate layers in #m_layers
  void fillLayersWithHits();

  /// Runs tool reconstructing clusters on each layer
  void clusterEachLayer();

  /// @brief Save clusters in xAOD format to StoreGate
  ///
  /// Creates xAOD::AFPSiHitsClusterContainer and registers it on
  /// StoreGate. Calls m_rowColToLocalCSToolHandle on each
  /// reconstructed cluster in each layer. The tool calculates cluster
  /// coordinates based on row and column information and creates new
  /// xAOD::AFPSiHitsCluster object, which is added to the container.
  void saveToXAOD();

  /// @brief Initialise size as well as station and layer ID in m_layers.
  ///
  /// The size of the array in argument defines number of stations,
  /// the value in each entry defines number of layers in the station.
  void initLayers (const std::vector<int>& layersInStations);
};


#endif // >  !AFP_SICLUSTERTOOLS_AFPSICLUSTERTOOL_H
