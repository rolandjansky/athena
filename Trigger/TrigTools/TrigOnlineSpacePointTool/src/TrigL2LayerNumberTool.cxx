/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrigL2LayerNumberTool.h"
#include "InDetReadoutGeometry/SiNumerology.h"

TrigL2LayerNumberTool::TrigL2LayerNumberTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AthAlgTool(t,n,p),
								     m_MaxSiliconLayerNum(-1),
								     m_OffsetEndcapPixels(-1),
								     m_OffsetBarrelSCT(-1),
								     m_OffsetEndcapSCT(-1) {
  declareInterface< ITrigL2LayerNumberTool >( this );

}

StatusCode TrigL2LayerNumberTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();

  ATH_MSG_INFO("In initialize...");

  sc = detStore()->retrieve(m_pixelManager);  
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve Pixel DetectorManager from detStore."); 
    return sc;
  } 
  sc = detStore()->retrieve(m_sctManager);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve SCT DetectorManager from detStore.");
    return sc;
  } 

  //calculate the numbers


  const InDetDD::SiNumerology& pixSiNum = m_pixelManager->numerology(); 
  const InDetDD::SiNumerology& sctSiNum = m_sctManager->numerology(); 

  m_MaxSiliconLayerNum = pixSiNum.numLayers()+pixSiNum.numDisks()+sctSiNum.numLayers()+sctSiNum.numDisks();
  m_OffsetBarrelSCT = pixSiNum.numLayers();
  m_OffsetEndcapPixels = pixSiNum.numLayers()+sctSiNum.numLayers();
  m_OffsetEndcapSCT = pixSiNum.numLayers()+sctSiNum.numLayers()+pixSiNum.numDisks();
  
  
  ATH_MSG_INFO("TrigL2LayerNumberTool initialized ");

  report();
  return sc;
}

StatusCode TrigL2LayerNumberTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

void TrigL2LayerNumberTool::report() {

  ATH_MSG_INFO("TrigL2 Layer numbering scheme:");
  ATH_MSG_INFO("Total number of layers = "<<maxSiliconLayerNum());
  ATH_MSG_INFO("OffsetEndcapPixels     = "<<offsetEndcapPixels());
  ATH_MSG_INFO("OffsetBarrelSCT        = "<<offsetBarrelSCT());
  ATH_MSG_INFO("OffsetEndcapSCT        = "<<offsetEndcapSCT());
}
