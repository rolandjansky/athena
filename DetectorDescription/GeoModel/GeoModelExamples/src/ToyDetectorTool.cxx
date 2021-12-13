/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BUILDVP1LIGHT
#include "ToyDetectorTool.h"
#include "ToyDetectorFactory.h"
#include "GeoModelExamples/ToyDetectorManager.h" 
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include <string>

ToyDetectorTool::ToyDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent )
{
}

ToyDetectorTool::~ToyDetectorTool()
{
  delete m_detector;  
}

StatusCode ToyDetectorTool::create()
{ 
  ATH_MSG_INFO("ToyDetectorTool::create() ...");

  // Locate the top level experiment
  GeoModelExperiment* theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

  ToyDetectorFactory theToyFactory(detStore().operator->());
  if(m_detector==nullptr) {
    // Build geometry
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=theExpt->getPhysVol();
      theToyFactory.create(world);
      printVolume(world);
    } 
    catch (const std::bad_alloc&) {
      ATH_MSG_FATAL("Could not Toy Detector Geometry!");
      return StatusCode::FAILURE; 
    }
    // Add detector manager to AtlasExperiment and also record it into DetStore
    theExpt->addManager(theToyFactory.getDetectorManager());
    if (detStore()->record(theToyFactory.getDetectorManager(),theToyFactory.getDetectorManager()->getName()).isSuccess()) return StatusCode::SUCCESS;
    ATH_MSG_FATAL("Failed to record Toy Detector Manager into Detector Store!");
  }
  else {
    ATH_MSG_FATAL("Attempt to build Toy Detector Geometry one more time. The geometry can be built only once!");
  }
  return StatusCode::FAILURE;
}

void ToyDetectorTool::printVolume(GeoPVConstLink volume, int level /*= 0*/)
{
  GeoVolumeCursor cursor(volume);
  while(!cursor.atEnd()) {
    GeoPVConstLink physChild = cursor.getVolume();
    GeoTrf::Transform3D position = cursor.getTransform();
    for(int k{0};k<level;++k) std::cout << "... ";
    std::cout << cursor.getName() << " " << (cursor.getId().isValid()?std::to_string(cursor.getId()).c_str():"N/A") 
	      << " Transform:" << "\n";
    for(int i{0};i<3;++i) {
      for(int j{0};j<4;++j) {
      	std::cout << position(i,j) << " ";
      }
      std::cout << "\n";
    }
    printVolume(physChild, level+1);
    cursor.next();
  }
}

#endif // BUILDVP1LIGHT
