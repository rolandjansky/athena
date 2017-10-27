/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiClusterTool.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-22
 * 
 * @brief  Source file for AFPSiClusterTool used for clustering hits.
 * 
 */

// STL includes

// FrameWork includes
#include "AFPSiClusterTool.h"
#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"

AFPSiClusterTool::AFPSiClusterTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : ::AthAlgTool(type, name, parent),
  m_clusterAlgToolHandle ("AFPSiClusterBasicNearestNeighbour"),
  m_rowColToLocalCSToolHandle ("AFPSiRowColToLocalCSTool")
{
  declareInterface<IAFPSiClusterTool>(this);

  declareProperty("hitsContainerName",
                  m_hitsContainerName = "AFPSiHitContainer",
                  "Name of the container with hits from which tracks are to be clustered");
  
  declareProperty("clustersContainerName",
                  m_clustersContainerName = "AFPSiHitsClusterContainer",
                  "Name of the container in which clusters of hits are saved");

  declareProperty("clusterAlgTool",
                  m_clusterAlgToolHandle,
                  "Tool with algorithm to perform actual clustering");
  
  declareProperty("rowColToLocalCSTool",
		  m_rowColToLocalCSToolHandle,
                  "Tool to rewrite clusters to xAOD and transform rows and columns to station local coordinates");  

  declareProperty("numberOfLayersInStations",
		  m_numberOfLayersInStations,
                  "Array defining number of stations and layers in each station. The size of the array in argument defines number of stations, the value in each entry defines number of layers in the station.");
}

void AFPSiClusterTool::initLayers (const std::vector<int>& layersInStations)
{
  // set number of stations
  m_layers.resize(layersInStations.size());

  // initialise number of layers in each station
  int stationID = 0;
  std::vector<std::vector<AFPSiClusterLayerBasicObj> >::iterator layersIter = m_layers.begin();
  for (const int layersN : layersInStations) {
    int layerID = 0;
    std::vector<AFPSiClusterLayerBasicObj>& layersVec = (*layersIter++);
    layersVec.resize(layersN);
    // set station and layer ID numbers
    for (AFPSiClusterLayerBasicObj& theLayer : layersVec) {
      theLayer.setStationID(stationID);
      theLayer.setLayerID(layerID++);
    }
    stationID++;
  } // close for over layersN
}

StatusCode AFPSiClusterTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // retrieve tools
  if (m_clusterAlgToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve " << m_clusterAlgToolHandle<<". Aborting AFPSiClusterTool initialisation");     
    return StatusCode::SUCCESS;   
  }

  ATH_MSG_INFO("Pixel clustering algorithm: "<<m_clusterAlgToolHandle);

  if (m_rowColToLocalCSToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve " << m_rowColToLocalCSToolHandle<<". Aborting AFPSiClusterTool initialisation");     
    return StatusCode::SUCCESS;   
  }

  ATH_MSG_INFO("Transformation of rows and columns to coordinates tool: "<<m_rowColToLocalCSToolHandle);

  // prepare array of layers
  if (m_numberOfLayersInStations.size() == 0) {
    // no job options configuration - use default values
    m_numberOfLayersInStations.resize(4, 4);
  }

  initLayers(m_numberOfLayersInStations);

  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterTool::finalize() 
{
  return StatusCode::SUCCESS;
}

StatusCode AFPSiClusterTool::clusterHits()
{
  clearAllLayers();
  fillLayersWithHits();
  clusterEachLayer();
  saveToXAOD();

  return StatusCode::SUCCESS;
}

void AFPSiClusterTool::clearAllLayers()
{
  for (std::vector<AFPSiClusterLayerBasicObj>& station : m_layers)
    for (AFPSiClusterLayerBasicObj& layer : station)
      layer.clearHitsAndClusters();
}

void AFPSiClusterTool::fillLayersWithHits()
{
  // retrieve hits
  if (evtStore()->retrieve(m_hitsContainer, m_hitsContainerName).isSuccess()) {
    try {
      // fill layers with hits
      for (xAOD::AFPSiHit* theHit : *m_hitsContainer)
	m_layers.at(theHit->stationID()).at(theHit->pixelLayerID()).hits().push_back(theHit);
    }
    catch (const std::out_of_range& outOfRange) {
      ATH_MSG_WARNING("Hit with station or pixel ID outside expected range. Aborting pixels clustering.");
      clearAllLayers();
    }
  }
  else
    ATH_MSG_WARNING("Failed to retrieve hits container.");
}

void AFPSiClusterTool::clusterEachLayer()
{
  for (std::vector<AFPSiClusterLayerBasicObj>& station : m_layers)
    for (AFPSiClusterLayerBasicObj& layer : station)
      m_clusterAlgToolHandle->doClustering(layer.hits(), layer.clusters()).ignore();
}

void AFPSiClusterTool::saveToXAOD()
{
  // create cluster containers and register in store gate
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  if (evtStore()->record(clusterContainer, m_clustersContainerName).isSuccess()) {
    xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
    if (evtStore()->record(clusterAuxContainer, m_clustersContainerName + "Aux.").isSuccess()) {
      clusterContainer->setStore(clusterAuxContainer);

      // fill xAOD container
      for (std::vector<AFPSiClusterLayerBasicObj>& station : m_layers)
	for (AFPSiClusterLayerBasicObj& layer : station) {
	  const int stationID = layer.stationID();
	  const int layerID = layer.layerID();
	  for (const AFPSiClusterBasicObj& theCluster : layer.clusters()) {
	    // create xAOD object and set cluster coordinates and errors
	    xAOD::AFPSiHitsCluster* xAODCluster = m_rowColToLocalCSToolHandle->newXAODLocal(stationID, layerID, theCluster, clusterContainer);

	    // add links to hits
	    for (const xAOD::AFPSiHit* theHit : theCluster.hits()) {
	      ElementLink< xAOD::AFPSiHitContainer >* hitLink = new ElementLink< xAOD::AFPSiHitContainer >; // will be taken over by the xAODCluster and deleted
	      hitLink->toContainedElement(*m_hitsContainer, theHit);
	      xAODCluster->addHitLink(*hitLink);
	    }
	  } // end for over layers
	} // end for over stations

    } // if (clusterContainerAux.isSuccess())
  } // if (clusterContainer.isSuccess())
}
