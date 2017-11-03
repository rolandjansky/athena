/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ToyDetectorTool.h"
#include "ToyDetectorFactory.h" 
#include "GeoModelExamples/ToyDetectorManager.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

/**
 ** Constructor(s)
 **/
ToyDetectorTool::ToyDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
: GeoModelTool( type, name, parent )
{
}

/**
 ** Destructor
 **/
ToyDetectorTool::~ToyDetectorTool()
{
	// This will need to be modified once we register the Toy DetectorNode in
	// the Transient Detector Store
	if ( 0 != m_detector ) {
		delete m_detector;
		m_detector = 0;
	}
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode
ToyDetectorTool::create()
{ 
  MsgStream log(msgSvc(), name()); 
  // 
  // Locate the top level experiment node 
  // 
  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" )) { 
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endmsg; 
    return (StatusCode::FAILURE); 
  } 

  ToyDetectorFactory theToyFactory(detStore().operator->());
  if ( 0 == m_detector ) {
    // Create the ToyDetectorNode instance
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      theToyFactory.create(world);
    } catch (std::bad_alloc) {
      log << MSG::FATAL << "Could not create new ToyDetectorNode!" << endmsg;
      return StatusCode::FAILURE; 
    }
    // Register the ToyDetectorNode instance with the Transient Detector Store
    theExpt->addManager(theToyFactory.getDetectorManager());
    if (detStore()->record(theToyFactory.getDetectorManager(),theToyFactory.getDetectorManager()->getName())!=StatusCode::SUCCESS) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
