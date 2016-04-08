/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_GeoM/ZDC_DetTool.h"
#include "ZDC_GeoM/ZDC_DetFactory.h" 
#include "ZDC_GeoM/ZDC_DetManager.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

ZDC_DetTool::ZDC_DetTool(const std::string& type, const std::string& name, const IInterface* parent)
 : GeoModelTool(type, name, parent)
{

  if (msgLevel(MSG::DEBUG))
    msg(MSG::DEBUG) << "INSIDE CONSTRUCTOR OF DETTOOL"                            << endreq
		    << "INSIDE CONSTRUCTOR OF DETTOOL string& type "      << type << endreq
		    << "INSIDE CONSTRUCTOR OF DETTOOL std::string& name " << name << endreq;
}

ZDC_DetTool::~ZDC_DetTool()
{
  // This will need to be modified once we register the Toy DetectorNode in the Transient Detector Store
  
  if (0 != m_detector) {

    delete m_detector;
    m_detector = 0;
  }
}

StatusCode ZDC_DetTool::create( StoreGateSvc* detStore )
{ 
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " Building ZDC geometry " << endreq;
  
  // Locate the top level experiment node  
  DataHandle<GeoModelExperiment> theExpt; 
  
  if (StatusCode::SUCCESS != detStore->retrieve(theExpt, "ATLAS")) {

    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << " Could not find GeoModelExperiment ATLAS " << endreq; 
    return (StatusCode::FAILURE); 
  } 
  
  ZDC_DetFactory theZDCFactory(detStore);
  
  if (0 == m_detector) { // Create the ZDCDetectorNode instance
    
    try { 
      // This strange way of casting is to avoid an utterly brain damaged compiler warning.
      GeoPhysVol* world = &*theExpt->getPhysVol();
      theZDCFactory.create(world);  
    } 
    catch (std::bad_alloc) {
      
      if (msgLevel(MSG::FATAL)) msg(MSG::FATAL) << "Could not create new ZDC DetectorNode!" << endreq;
      return StatusCode::FAILURE; 
    }
    
    // Register the ZDC DetectorNode instance with the Transient Detector Store
    theExpt->addManager(theZDCFactory.getDetectorManager());
    //detStore->record(theZDCFactory.getDetectorManager(),theZDCFactory.getDetectorManager()->getName());
    //return StatusCode::SUCCESS;
    if(detStore->record(theZDCFactory.getDetectorManager(),theZDCFactory.getDetectorManager()->getName())==StatusCode::SUCCESS){
      return StatusCode::SUCCESS;}
    else{
      msg(MSG::FATAL) << "Could not register ZDC detector manager" << endreq;}

  }
  
  return StatusCode::FAILURE;
}
