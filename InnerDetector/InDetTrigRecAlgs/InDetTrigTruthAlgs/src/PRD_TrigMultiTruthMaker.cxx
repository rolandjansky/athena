/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTrigTruthAlgs/PRD_TrigMultiTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "StoreGate/StoreGateSvc.h"

#include <iterator>

namespace InDet {

//================================================================
PRD_TrigMultiTruthMaker::PRD_TrigMultiTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  HLT::FexAlgo(name,pSvcLocator),
  m_PRDTruthTool("InDet::PRD_MultiTruthBuilder"),
  m_doTruth(false)
{  
  declareProperty("PixelClusterContainerName",  m_PixelClustersName="PixelTrigClusters");
  declareProperty("SCTClusterContainerName",    m_SCTClustersName="SCT_TrigClusters");
  declareProperty("TRTDriftCircleContainerName",m_TRTDriftCircleContainerName="TRT_TrigDriftCircles");
  declareProperty("SimDataMapNamePixel",        m_simDataMapNamePixel="PixelSDO_Map");
  declareProperty("SimDataMapNameSCT",          m_simDataMapNameSCT="SCT_SDO_Map");
  declareProperty("SimDataMapNameTRT",          m_simDataMapNameTRT="TRT_SDO_Map");
  declareProperty("doTruthAss",                  m_doTruth);
  //declareProperty("TruthTool",               m_PRDTruthTool);

  declareMonitoredVariable("prdPIXsize", m_prdPIXsize   );
  declareMonitoredVariable("prdSCTsize", m_prdSCTsize   );
  declareMonitoredVariable("prdTRTsize", m_prdTRTsize   );

}

// Initialize method
// -----------------------------------------------------------------------------------------------------
HLT::ErrorCode PRD_TrigMultiTruthMaker::hltInitialize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "PRD_TrigMultiTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }

  msg() << MSG::INFO << "PRD_TrigMultiTruthMaker::initialize()" << endreq;
  
  //StatusCode sc;

  //----------------
  if (m_PRDTruthTool.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve InDet::PRD_MultiTruthBuilder Tool!" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  
  msg() << MSG::INFO << "PRD_MultiTurthMaker initialized successfully !" << endreq;

  //----------------
  return HLT::OK;
}

// -------------------------------------------------------------------------------
HLT::ErrorCode PRD_TrigMultiTruthMaker::hltFinalize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "PRD_TrigMultiTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }

  msg() << MSG::INFO << "PRD_TrigMultiTruthMaker finalized" << endreq;

  return HLT::OK;
}
// ------------------------------------------------------------------------------
HLT::ErrorCode PRD_TrigMultiTruthMaker::hltExecute(const HLT::TriggerElement*,
                                                   HLT::TriggerElement* outputTE) {

  m_prdPIXsize = 0;
  m_prdSCTsize = 0;
  m_prdTRTsize = 0;

  int outputLevel = msgLvl();

  if(!m_doTruth) {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "PRD_TrigMultiTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }
 

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "PRD_TrigMultiTruthMaker::execute()" << endreq;

  StatusCode sc;

  // work on Pixels
  if(!m_PixelClustersName.empty() && !m_simDataMapNamePixel.empty()) {

    const InDet::SiClusterContainer* prdContainer=0;
    // retrieve Pixel cluster container
    sc = store()->retrieve(prdContainer, m_PixelClustersName);

    if (sc.isFailure() || !prdContainer){

      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Pixel Cluster Container NOT found"<< endreq;
    } 
    else {
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Pixel Cluster Container found" <<endreq;

      // Retrieve the Pixel SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = store()->retrieve(simDataMap, m_simDataMapNamePixel);

      if (sc.isFailure() || !simDataMap ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Could NOT find the InDetSimDataPixel map"<< endreq;
      }	else {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Found InDetSimDataPixel, do association" << endreq;

	// Create and fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_pixels = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_pixels, prdContainer->begin(), prdContainer->end(), simDataMap, true);

	// And register it with the StoreGate
	//bool allow_modifications;

	if ( HLT::OK !=  attachFeature(outputTE, prdt_pixels, "PRDTPixels") ) {
	  msg() << MSG::WARNING << "Could not attach feature to the TE" << endreq;
	}
	m_prdPIXsize = prdt_pixels->size();
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "PRD Pixel truth structure is registered in StoreGate, size="<< m_prdPIXsize << endreq;

      }
    }
  }

  // work on SCT
  if(!m_SCTClustersName.empty() && !m_simDataMapNameSCT.empty()) {
    const InDet::SiClusterContainer* prdContainer=0;
    // retrieve SCT cluster container
    sc = store()->retrieve(prdContainer, m_SCTClustersName);
    if (sc.isFailure() || !prdContainer){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "SCT Cluster Container NOT found"<< endreq;
    } else{
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "SCT Cluster Container found" <<endreq;
    
      // Retrieve the SCT SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = store()->retrieve(simDataMap, m_simDataMapNameSCT);
      if (sc.isFailure() || !simDataMap ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Could NOT find the InDetSimDataSCT map"<< endreq;
      }	else {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Found InDetSimDataSCT, do association" << endreq;
      
	// Create and fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_sct = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_sct, prdContainer->begin(), prdContainer->end(), simDataMap, false);

	if ( HLT::OK !=  attachFeature(outputTE, prdt_sct, "PRDTSCT") ) {
	  msg() << MSG::WARNING << "Could not attach feature to the TE" << endreq;
	}
	m_prdSCTsize = prdt_sct->size();
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "PRD SCT truth structure is registered in StoreGate, size="<< m_prdSCTsize << endreq;

      }
    } 
  }

  // work on TRT
  if(!m_TRTDriftCircleContainerName.empty() && !m_simDataMapNameTRT.empty() ) {
    const InDet::TRT_DriftCircleContainer* trtContainer= 0;
    // retrieve TRT DriftCircle container
    if (store()->contains<InDet::TRT_DriftCircleContainer>(m_TRTDriftCircleContainerName)){
      sc = store()->retrieve(trtContainer, m_TRTDriftCircleContainerName);
    }
    if (sc.isFailure() || !trtContainer){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "TRT DriftCircle Container NOT found"<< endreq;
    } else{
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "TRT DriftCircle Container found" <<endreq;
     
      // Retrieve the TRT SDO map for this event
      const InDetSimDataCollection*    simDataMap=0;
      sc = store()->retrieve(simDataMap, m_simDataMapNameTRT);
      if (sc.isFailure() || !simDataMap ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Could NOT find the InDetSimDataTRT map"<< endreq;
      } else {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Found InDetSimDataTRT, do association" << endreq;

	// Fill the PRD truth structure
	PRD_MultiTruthCollection *prdt_trt = new PRD_MultiTruthCollection;
	addPRDCollections(prdt_trt, trtContainer->begin(), trtContainer->end(), simDataMap, false);

	if ( HLT::OK !=  attachFeature(outputTE, prdt_trt, "PRDTTRT") ) {
	  msg() << MSG::WARNING << "Could not attach feature to the TE" << endreq;
	}
	m_prdTRTsize = prdt_trt->size();
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "PRD TRT truth structure is registered in StoreGate, size="<< m_prdTRTsize << endreq;
	
      }
    }
  }

    
  return HLT::OK;
}

// -----------------------------------------------------------------------------------------------------
template<class PRD_Container_Iterator>
void PRD_TrigMultiTruthMaker::addPRDCollections(PRD_MultiTruthCollection* prdTruth,
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
void PRD_TrigMultiTruthMaker::addPRDRange (PRD_MultiTruthCollection* prdTruth,
			     PRD_Collection_Iterator range_begin,
			     PRD_Collection_Iterator range_end,
			     const InDetSimDataCollection* simDataMap,
			     bool pixels) 
{
  for (PRD_Collection_Iterator nextDatum = range_begin; nextDatum != range_end; nextDatum++){
    m_PRDTruthTool->addPrepRawDatum(prdTruth, *nextDatum, simDataMap, pixels);
  }
}

} // end of namespace
//================================================================
//EOF
