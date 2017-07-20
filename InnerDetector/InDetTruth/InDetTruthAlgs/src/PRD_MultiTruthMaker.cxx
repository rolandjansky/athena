/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTruthAlgs/PRD_MultiTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include <iterator>

namespace InDet {

//================================================================
PRD_MultiTruthMaker::PRD_MultiTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator),
  m_PixelClustersName("PixelClusters"),
  m_SCTClustersName("SCT_Clusters"),
  m_TRTDriftCircleContainerName("TRT_DriftCircles"),
  m_simDataMapNamePixel("PixelSDO_Map"),
  m_simDataMapNameSCT("SCT_SDO_Map"),
  m_simDataMapNameTRT("TRT_SDO_Map"),
  m_PRDTruthNamePixel("PRD_MultiTruthPixel"),
  m_PRDTruthNameSCT("PRD_MultiTruthSCT"),
  m_PRDTruthNameTRT("PRD_MultiTruthTRT"),
  m_PRDTruthTool("InDet::PRD_MultiTruthBuilder")
{
  declareProperty("PixelClusterContainerName",  m_PixelClustersName);
  declareProperty("SCTClusterContainerName",    m_SCTClustersName);
  declareProperty("TRTDriftCircleContainerName",m_TRTDriftCircleContainerName);
  declareProperty("SimDataMapNamePixel",        m_simDataMapNamePixel);
  declareProperty("SimDataMapNameSCT",          m_simDataMapNameSCT);
  declareProperty("SimDataMapNameTRT",          m_simDataMapNameTRT);
  declareProperty("TruthNamePixel",             m_PRDTruthNamePixel);
  declareProperty("TruthNameSCT",               m_PRDTruthNameSCT);
  declareProperty("TruthNameTRT",               m_PRDTruthNameTRT);
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

  // Read Handle Key
  ATH_CHECK(m_PixelClustersName.initialize(not m_PixelClustersName.key().empty()));
  ATH_CHECK(m_simDataMapNamePixel.initialize(not m_simDataMapNamePixel.key().empty()));
  // Write Handle Key
  ATH_CHECK(m_PRDTruthNamePixel.initialize(not m_PRDTruthNamePixel.key().empty()));

  ATH_CHECK(m_SCTClustersName.initialize(not m_SCTClustersName.key().empty()));
  ATH_CHECK(m_simDataMapNameSCT.initialize(not m_simDataMapNameSCT.key().empty()));
  // Write Handle Key
  ATH_CHECK(m_PRDTruthNameSCT.initialize(not m_PRDTruthNameSCT.key().empty()));

  ATH_CHECK(m_TRTDriftCircleContainerName.initialize(not m_TRTDriftCircleContainerName.key().empty()));
  ATH_CHECK(m_simDataMapNameTRT.initialize(not m_simDataMapNameTRT.key().empty()));
  // Write Handle Key
  ATH_CHECK(m_PRDTruthNameTRT.initialize(not m_PRDTruthNameTRT.key().empty()));

  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode PRD_MultiTruthMaker::finalize()
{
  ATH_MSG_DEBUG ("PRD_MultiTruthMaker finalized");
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode PRD_MultiTruthMaker::execute_r(const EventContext &ctx) const {

  ATH_MSG_DEBUG ("PRD_MultiTruthMaker::execute()");

  // @TODO move below each to a tool just dealing with Pixel, SCT or TRT.
  // work on Pixels
  if(!m_PixelClustersName.key().empty() && !m_simDataMapNamePixel.key().empty() && !m_PRDTruthNamePixel.key().empty()) {

    SG::ReadHandle<InDet::SiClusterContainer> prdContainer(m_PixelClustersName,ctx);
    if (!prdContainer.isValid()) {
      ATH_MSG_ERROR ("Pixel Cluster Container " << m_PixelClustersName.key() << " NOT found");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Pixel Cluster Container " << m_PixelClustersName.key() << " found");

      // Retrieve the Pixel SDO map for this event
    SG::ReadHandle<InDetSimDataCollection>    simDataMap(m_simDataMapNamePixel,ctx);
    if (!simDataMap.isValid()) {
      ATH_MSG_ERROR ("Could NOT find the InDetSimDataPixel " << m_simDataMapNamePixel.key() << " map.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ("Found InDetSimDataPixel " << m_simDataMapNamePixel.key() << ", do association");

    // Create and fill the PRD truth structure
    SG::WriteHandle<PRD_MultiTruthCollection> prdt_pixels(m_PRDTruthNamePixel,ctx);
    if (prdt_pixels.record(std::make_unique<PRD_MultiTruthCollection>()).isFailure()) {
      ATH_MSG_ERROR ("PRD truth structure '" << m_PRDTruthNamePixel.key() << "' could not be registered in StoreGate !");
      return StatusCode::FAILURE;
    }
    addPRDCollections(&(*prdt_pixels), prdContainer->begin(), prdContainer->end(), &(*simDataMap), true);
    ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNamePixel.key() << "' is registered in StoreGate, size="<<prdt_pixels->size());
  }

  // work on SCT
  if(!m_SCTClustersName.key().empty() && !m_simDataMapNameSCT.key().empty() && !m_PRDTruthNameSCT.key().empty()) {
    // Retrieve the SCT SDO map for this event
    SG::ReadHandle<InDetSimDataCollection> simDataMap(m_simDataMapNameSCT,ctx);
    if (not simDataMap.isValid()) {
      ATH_MSG_ERROR ("Could NOT find the InDetSimDataSCT " << m_simDataMapNameSCT.key() << " map");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG ("Found InDetSimDataSCT " << m_simDataMapNameSCT.key() << ", do association");
    // retrieve SCT cluster container
    SG::ReadHandle<InDet::SiClusterContainer> prdContainer(m_SCTClustersName,ctx);
    if (not prdContainer.isValid()){
      ATH_MSG_ERROR ("SCT Cluster Container " << m_SCTClustersName.key() << " NOT found");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ("SCT Cluster Container " << m_SCTClustersName.key() << " found");

    // Create and fill the PRD truth structure
    SG::WriteHandle<PRD_MultiTruthCollection> prdt_sct(m_PRDTruthNameSCT);
    ATH_CHECK(prdt_sct.record(std::make_unique<PRD_MultiTruthCollection>()));
    addPRDCollections(prdt_sct.ptr(), prdContainer->begin(), prdContainer->end(), simDataMap.cptr(), false);
    ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNameSCT.key() << "' is registered in StoreGate, size="<<prdt_sct->size());
  }

  // work on TRT
  if(!m_TRTDriftCircleContainerName.key().empty() && !m_simDataMapNameTRT.key().empty() && !m_PRDTruthNameTRT.key().empty()) {

    SG::ReadHandle<InDet::TRT_DriftCircleContainer> trtContainer(m_TRTDriftCircleContainerName,ctx);
    if (!trtContainer.isValid()){
      ATH_MSG_ERROR ("TRT DriftCircle Container " << m_TRTDriftCircleContainerName.key() << " NOT found");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ("TRT DriftCirlce Container " << m_TRTDriftCircleContainerName.key() << " found");

    // Retrieve the TRT SDO map for this event
    SG::ReadHandle<InDetSimDataCollection>    simDataMap(m_simDataMapNameTRT,ctx);
    if (!simDataMap.isValid()) {
      ATH_MSG_ERROR ("Could NOT find the InDetSimDataTRT " << m_simDataMapNameTRT.key() << " map");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ("Found InDetSimDataTRT " << m_simDataMapNameTRT.key() << " , do association");

    // Fill the PRD truth structure
    SG::WriteHandle<PRD_MultiTruthCollection> prdt_trt(m_PRDTruthNameTRT,ctx);
    if (prdt_trt.record(std::make_unique<PRD_MultiTruthCollection>()).isFailure()) {
      ATH_MSG_ERROR ("PRD truth structure '" << m_PRDTruthNameTRT.key() << "' could not be registered in StoreGate !");
      return StatusCode::FAILURE;
    }
    addPRDCollections(&(*prdt_trt), trtContainer->begin(), trtContainer->end(), &(*simDataMap), false);
    ATH_MSG_DEBUG ("PRD truth structure '" << m_PRDTruthNameTRT.key() << "' is registered in StoreGate, size="<<prdt_trt->size());
  }

  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
template<class PRD_Container_Iterator>
void PRD_MultiTruthMaker::addPRDCollections(PRD_MultiTruthCollection* prdTruth,
				  PRD_Container_Iterator collections_begin,
				  PRD_Container_Iterator collections_end,
				  const InDetSimDataCollection* simDataMap,
				  bool pixels) const
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
			     bool pixels) const
{
  for (PRD_Collection_Iterator nextDatum = range_begin; nextDatum != range_end; nextDatum++){
    m_PRDTruthTool->addPrepRawDatum(prdTruth, *nextDatum, simDataMap, pixels);
  }
}


} // namespace InDet

//================================================================
//EOF
