/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/PixelServMatFactory.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"


#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"


#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "CxxUtils/checker_macros.h"

#include <iostream>


PixelServMatFactory::PixelServMatFactory(InDetDD::AthenaComps * athenaComps)
  : InDetDD::SubDetectorFactoryBase(athenaComps)
{}


PixelServMatFactory::~PixelServMatFactory()
{
}


//## Other Operations (implementation)
void PixelServMatFactory::create(GeoPhysVol *mother)
{
  msg(MSG::DEBUG) << "Building Pixel Service Material" << endmsg; 
  
  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  DecodeVersionKey pixelVersionKey(geoDbTagSvc(),"Pixel");

  IRDBRecordset_ptr pixelGenServices = rdbAccessSvc()->getRecordsetPtr("PixelGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Doesn't exist at time of writing but we include it to keep things general
  IRDBRecordset_ptr weightTable = rdbAccessSvc()->getRecordsetPtr("PixelWeights", pixelVersionKey.tag(), pixelVersionKey.node());

  IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("InDetServMatScaling", indetVersionKey.tag(), indetVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
  m_materialManagerUnique = std::make_unique<InDetMaterialManager>("PixelMaterialManager", getAthenaComps());
  m_materialManager = m_materialManagerUnique.get();
  m_materialManager->addWeightTable(weightTable, "pix");
  m_materialManager->addScalingTable(scalingTable);
  
  // Build general services:
  //

  for (unsigned int ii =0; ii < pixelGenServices->size(); ii++) {
    
    InDetDD::GenericTubeMaker tubeHelper((*pixelGenServices)[ii]);
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    }
    logName = "Pix" + logName;
   
    const GeoShape* serviceTube = serviceTubeTmp;

    std::string materialName = tubeHelper.materialName();
    //const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = materialManager()->getMaterial(materialName);
   
    const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
    GeoVPhysVol* servPhys = new GeoPhysVol(servLog);

    tubeHelper.placeVolume(mother, servPhys);

  }
}

