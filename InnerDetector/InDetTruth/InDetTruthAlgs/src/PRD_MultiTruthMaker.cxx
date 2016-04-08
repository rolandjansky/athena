/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTruthAlgs/PRD_MultiTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include <iterator>

namespace InDet {

//================================================================
PRD_MultiTruthMaker::PRD_MultiTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_PRDTruthTool("InDet::PRD_MultiTruthBuilder")
{  
  declareProperty("PixelClusterContainerName",  m_PixelClustersName="PixelClusters");
  declareProperty("SCTClusterContainerName",    m_SCTClustersName="SCT_Clusters");
  declareProperty("TRTDriftCircleContainerName",m_TRTDriftCircleContainerName="TRT_DriftCircles");
  declareProperty("SimDataMapNamePixel",        m_simDataMapNamePixel="PixelSDO_Map");
  declareProperty("SimDataMapNameSCT",          m_simDataMapNameSCT="SCT_SDO_Map");
  declareProperty("SimDataMapNameTRT",          m_simDataMapNameTRT="TRT_SDO_Map");
  declareProperty("TruthNamePixel",             m_PRDTruthNamePixel="PRD_MultiTruthPixel");
  declareProperty("TruthNameSCT",               m_PRDTruthNameSCT="PRD_MultiTruthSCT");
  declareProperty("TruthNameTRT",               m_PRDTruthNameTRT="PRD_MultiTruthTRT");
  declareProperty("PRDTruthTool",               m_PRDTruthTool);
}

// Initialize method
// -----------------------------------------------------------------------------------------------------
StatusCode PRD_MultiTruthMaker::initialize()
{
  ATH_MSG_INFO ("PRD_MultiTruthMaker::initialize()");
  
  if ( m_PRDTruthTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ("Failed to retrieve tool " << m_PRDTruthTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ("Retrieved tool " << m_PRDTruthTool);
  }
  
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode PRD_MultiTruthMaker::finalize() 
{
  ATH_MSG_DEBUG ("PRD_MultiTruthMaker finalized");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode PRD_MultiTruthMaker::execute() {

  ATH_MSG_DEBUG ("PRD_MultiTruthMaker::execute()");

  StatusCode sc;

  // work on Pixels
  if(!m_PixelClustersName.empty() && !m_simDataMapNamePixel.empty() && !m_PRDTruthNamePixel.empty()) {
    const InDet::SiClusterContainer* prdContainer=0;
    // retrieve Pixel cluster container
    sc = evtStore()->retrieve(prdContainer, m_PixelClustersName);
    if (sc.isFailure() || !prdContainer){
      ATH_MSG_DEBUG ("Pixel Cluster Container NOT found");
    } else {
      ATH_MSG_DEBUG("Pixel Cluster Container found");

      // Retrieve the Pixel SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = evtStore()->retrieve(simDataMap, m_simDataMapNamePixel);
      if (sc.isFailure() || !simDataMap ) {
	ATH_MSG_DEBUG ("Could NOT find the InDetSimDataPixel map");
	return StatusCode::SUCCESS;
      }	else {
	ATH_MSG_DEBUG ("Found InDetSimDataPixel, do association");

	// Create and fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_pixels = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_pixels, prdContainer->begin(), prdContainer->end(), simDataMap, true);
	
	// And register it with the StoreGate
	bool allow_modifications;
	sc=evtStore()->record(prdt_pixels, m_PRDTruthNamePixel, allow_modifications=false);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("PRD truth structure '" << m_PRDTruthNamePixel << "' could not be registered in StoreGate !");
	  return StatusCode::FAILURE;
	} else {
	  ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNamePixel << "' is registered in StoreGate, size="<<prdt_pixels->size());
	}
      }
    }
  }

  // work on SCT
  if(!m_SCTClustersName.empty() && !m_simDataMapNameSCT.empty() && !m_PRDTruthNameSCT.empty()) {
    const InDet::SiClusterContainer* prdContainer=0;
    // retrieve SCT cluster container
    sc = evtStore()->retrieve(prdContainer, m_SCTClustersName);
    if (sc.isFailure() || !prdContainer){
      ATH_MSG_DEBUG ("SCT Cluster Container NOT found");
    } else{
      ATH_MSG_DEBUG ("SCT Cluster Container found");
    
      // Retrieve the SCT SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = evtStore()->retrieve(simDataMap, m_simDataMapNameSCT);
      if (sc.isFailure() || !simDataMap ) {
	ATH_MSG_DEBUG ("Could NOT find the InDetSimDataSCT map");
      }	else {
	ATH_MSG_DEBUG ("Found InDetSimDataSCT, do association");
      
	// Create and fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_sct = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_sct, prdContainer->begin(), prdContainer->end(), simDataMap, false);

	// And register it with the StoreGate
	bool allow_modifications;
	sc=evtStore()->record(prdt_sct, m_PRDTruthNameSCT, allow_modifications=false);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("PRD truth structure '" << m_PRDTruthNameSCT << "' could not be registered in StoreGate !");
	  return StatusCode::FAILURE;
	} else {
	  ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNameSCT << "' is registered in StoreGate, size="<<prdt_sct->size());
	}
      }
    } 
  }

  // work on TRT
  if(!m_TRTDriftCircleContainerName.empty() && !m_simDataMapNameTRT.empty() && !m_PRDTruthNameTRT.empty()) {
    const InDet::TRT_DriftCircleContainer* trtContainer;
    // retrieve TRT DriftCircle container
    sc = evtStore()->retrieve(trtContainer, m_TRTDriftCircleContainerName);
    if (sc.isFailure() || !trtContainer){
      ATH_MSG_DEBUG ("TRT DriftCircle Container NOT found");
    } else{
      ATH_MSG_DEBUG ("TRT DriftCirlce Container found");
     
      // Retrieve the TRT SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = evtStore()->retrieve(simDataMap, m_simDataMapNameTRT);
      if (sc.isFailure() || !simDataMap ) {
	ATH_MSG_DEBUG ("Could NOT find the InDetSimDataTRT map");
      } else {
	ATH_MSG_DEBUG ("Found InDetSimDataTRT, do association");

	// Fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_trt = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_trt, trtContainer->begin(), trtContainer->end(), simDataMap, false);

	// And register it with the StoreGate
	bool allow_modifications;
	sc=evtStore()->record(prdt_trt, m_PRDTruthNameTRT, allow_modifications=false);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("PRD truth structure '" << m_PRDTruthNameTRT << "' could not be registered in StoreGate !");
	  return StatusCode::FAILURE;
	} else {
	  ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNameTRT << "' is registered in StoreGate, size="<<prdt_trt->size());
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
template<class PRD_Container_Iterator>
void PRD_MultiTruthMaker::addPRDCollections(PRD_MultiTruthCollection* prdTruth,
				  PRD_Container_Iterator collections_begin,
				  PRD_Container_Iterator collections_end,
				  const InDetSimDataCollection* simDataMap,
				  bool pixels)
{
  for (PRD_Container_Iterator colNext=collections_begin; colNext != collections_end; ++colNext) {
    addPRDRange(prdTruth, (*colNext)->begin(), (*colNext)->end(), simDataMap, pixels);
  }
}

// -----------------------------------------------------------------------------------------------------
// Adds PRDs in the range to prdTruth.
template<class PRD_Collection_Iterator>
void PRD_MultiTruthMaker::addPRDRange (PRD_MultiTruthCollection* prdTruth,
			     PRD_Collection_Iterator range_begin,
			     PRD_Collection_Iterator range_end,
			     const InDetSimDataCollection* simDataMap,
			     bool pixels) 
{
  for (PRD_Collection_Iterator nextDatum = range_begin; nextDatum != range_end; nextDatum++){
    m_PRDTruthTool->addPrepRawDatum(prdTruth, *nextDatum, simDataMap, pixels);
  }
}


} // namespace InDet

//================================================================
//EOF
