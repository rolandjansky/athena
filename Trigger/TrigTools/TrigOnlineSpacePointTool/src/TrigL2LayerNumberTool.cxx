/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrigL2LayerNumberTool.h"
#include "InDetReadoutGeometry/SiNumerology.h"

TrigL2LayerNumberTool::TrigL2LayerNumberTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AlgTool(t,n,p),
								     m_MaxSiliconLayerNum(-1),
								     m_OffsetEndcapPixels(-1),
								     m_OffsetBarrelSCT(-1),
								     m_OffsetEndcapSCT(-1) {
  declareInterface< ITrigL2LayerNumberTool >( this );

}

StatusCode TrigL2LayerNumberTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO <<"In initialize..."<<endreq;
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if ( sc.isFailure() ) { 
    athenaLog << MSG::FATAL << "DetStore service not found" << endreq; 
    return StatusCode::FAILURE; 
  }

  sc = detStore->retrieve(m_pixelManager);  
  if( sc.isFailure() ) {
    athenaLog << MSG::ERROR << "Could not retrieve Pixel DetectorManager from detStore."<<endreq; 
    return sc;
  } 
  sc = detStore->retrieve(m_sctManager);
  if( sc.isFailure() ) {
    athenaLog << MSG::ERROR << "Could not retrieve SCT DetectorManager from detStore." << endreq;
    return sc;
  } 

  //calculate the numbers


  const InDetDD::SiNumerology& pixSiNum = m_pixelManager->numerology(); 
  const InDetDD::SiNumerology& sctSiNum = m_sctManager->numerology(); 

  m_MaxSiliconLayerNum = pixSiNum.numLayers()+pixSiNum.numDisks()+sctSiNum.numLayers()+sctSiNum.numDisks();
  m_OffsetBarrelSCT = pixSiNum.numLayers();
  m_OffsetEndcapPixels = pixSiNum.numLayers()+sctSiNum.numLayers();
  m_OffsetEndcapSCT = pixSiNum.numLayers()+sctSiNum.numLayers()+pixSiNum.numDisks();
  
  
  athenaLog << MSG::INFO << "TrigL2LayerNumberTool initialized "<< endreq;

  report();
  return sc;
}

StatusCode TrigL2LayerNumberTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

void TrigL2LayerNumberTool::report() {

  MsgStream athenaLog(msgSvc(), name());

  athenaLog<<MSG::INFO<<"TrigL2 Layer numbering scheme:"<<endreq;
  athenaLog<<MSG::INFO<<"Total number of layers = "<<maxSiliconLayerNum()<<endreq;
  athenaLog<<MSG::INFO<<"OffsetEndcapPixels     = "<<offsetEndcapPixels()<<endreq;
  athenaLog<<MSG::INFO<<"OffsetBarrelSCT        = "<<offsetBarrelSCT()<<endreq;
  athenaLog<<MSG::INFO<<"OffsetEndcapSCT        = "<<offsetEndcapSCT()<<endreq;
}
