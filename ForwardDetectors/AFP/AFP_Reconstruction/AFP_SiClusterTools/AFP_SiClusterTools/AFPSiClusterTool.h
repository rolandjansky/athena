/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

// Monitoring includes
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "AFP_SiClusterTools/IAFPSiClusterTool.h"
#include "AFP_SiClusterTools/IAFPSiClusterAlgTool.h"
#include "AFP_SiClusterTools/AFPSiClusterLayerBasicObj.h"
#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"

/**
 * @brief This class reconstructs clusters of pixels.
 * 
 * The clustering algorithm is defined in a tool that is used and can
 * be changed. 
 */
class AFPSiClusterTool
  : public extends<AthAlgTool, IAFPSiClusterTool>
{
public:
  AFPSiClusterTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~AFPSiClusterTool() override {}

  // Athena algtool's Hooks
  virtual StatusCode initialize() override;

  /// does nothing
  virtual StatusCode finalize() override;

  /// @copydoc IAFPSiClusterTool::clusterHits()
  StatusCode clusterHits(std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& clustr, const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_AFPSiHitContainerKey { this, "AFPSiHitsContainerName", "AFPSiHitContainer", "name of AFPSiHit container" };

  /// @brief Tool that does the clustering on hits
  ToolHandle<IAFPSiClusterAlgTool> m_clusterAlgToolHandle{this,"clusterAlgTool", "AFPSiClusterAlgTool","Tool with algorithm to perform actual clustering"};
  
  /// @brief Tool that creates xAOD and transforms rows and pixels to local coordinate system
  ToolHandle<IAFPSiRowColToLocalCSTool> m_rowColToLocalCSToolHandle{this,"rowColToLocalCSTool", "AFPSiRowColToLocalCSTool","Tool to rewrite clusters to xAOD and transform rows and columns to station local coordinates"}; 
  
  /// @ brief Monitoring tool
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};
  
  /// @brief Array defining number of stations and layers in each station
  ///
  /// The size of the array in argument defines number of stations,
  /// the value in each entry defines number of layers in the station. Default is 4 stations times 4 layers.
  Gaudi::Property<std::vector<int>> m_numberOfLayersInStations {this, "numberOfLayersInStations",{4,4,4,4}, "The size of the array in argument defines number of stations, the value in each entry defines number of layers in the station."};

  /// Runs AFPSiClusterLayerBasicObj::clearHitsAndClusters() on all layers in #my_layers
  StatusCode clearAllLayers(std::vector< std::vector<AFPSiClusterLayerBasicObj> > &my_layers) const;

  /// Reads hits from StoreGate and saves them to appropriate layers in #my_layers
  StatusCode fillLayersWithHits(std::vector< std::vector<AFPSiClusterLayerBasicObj> > &my_layers, const EventContext &ctx) const;

  /// Runs tool reconstructing clusters on each layer
  StatusCode clusterEachLayer(std::vector< std::vector<AFPSiClusterLayerBasicObj> > &my_layers) const;

  /// @brief Save clusters in xAOD::AFPSiHitsClusterContainer
  ///
  /// Calls m_rowColToLocalCSToolHandle on each
  /// reconstructed cluster in each layer. The tool calculates cluster
  /// coordinates based on row and column information and creates new
  /// xAOD::AFPSiHitsCluster object, which is added to the container.
  StatusCode saveToXAOD(std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& clustr, std::vector< std::vector<AFPSiClusterLayerBasicObj> > &my_layers, const EventContext &ctx) const;

  /// @brief Initialise size as well as station and layer ID for #my_layers.
  ///
  /// The size of the array in argument defines number of stations,
  /// the value in each entry defines number of layers in the station.
  std::vector< std::vector<AFPSiClusterLayerBasicObj> > initLayers (const std::vector<int>& layersInStations) const;
};


#endif // >  !AFP_SICLUSTERTOOLS_AFPSICLUSTERTOOL_H
