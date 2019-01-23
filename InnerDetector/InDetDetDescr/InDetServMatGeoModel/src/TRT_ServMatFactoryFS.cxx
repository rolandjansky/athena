/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/TRT_ServMatFactoryFS.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <iostream>

TRT_ServMatFactoryFS::TRT_ServMatFactoryFS(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_materialManager(0),
  m_msg("TRT_ServMatFactoryFS")
{
  
}


TRT_ServMatFactoryFS::~TRT_ServMatFactoryFS()
{
  delete m_materialManager;
}



//## Other Operations (implementation)
void TRT_ServMatFactoryFS::create(GeoPhysVol *motherP, GeoPhysVol *motherM)
{

  msg(MSG::DEBUG) << "Building TRT Service Material" << endmsg;

  //double epsilon = 0.002;
  

  DecodeVersionKey atlasVersionKey("ATLAS");
  DecodeVersionKey indetVersionKey("InnerDetector");
  DecodeVersionKey trtVersionKey("TRT");

  IRDBRecordset_ptr atls = m_rdbAccess->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr cage = m_rdbAccess->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr trtGenServices = m_rdbAccess->getRecordsetPtr("TRTGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Doesn't exist at time of writing but we include it to keep things general
  IRDBRecordset_ptr weightTable = m_rdbAccess->getRecordsetPtr("TrtWeights", trtVersionKey.tag(), trtVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
  m_materialManager = new InDetMaterialManager("TRT_MaterialManager", m_detStore, weightTable, "trt");


//VK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;
  double minRofGap  =       1050.0;
  double phiWid=70./outROfIDet;    double safetyGap=1.;
  const GeoShape* railGap1=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
					-phiWid/2.,phiWid);
  const GeoShape* railGap2=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap ,
					-phiWid/2.+M_PI,phiWid);
  // In case they don't get used. 
  railGap1->ref(); 
  railGap2->ref(); 


//VK 26.03.2007  Construct a gap for SquirrelCage ribbon
  double  rminInt    = (*cage)[0]->getDouble("RINGRMIN")*Gaudi::Units::mm;
  double  ringThick  = (*cage)[0]->getDouble("RINGTHICK")*Gaudi::Units::mm;
  double  ringGap    = (*cage)[0]->getDouble("RINGGAP")*Gaudi::Units::mm;
  double  ribWid = (*cage)[0]->getDouble("RIBWIDTH")*Gaudi::Units::mm;
  double phiWidSQ=ribWid/(rminInt+ringThick+ringGap/2.);



  
  // Build general services:
  //

  for (unsigned int ii =0; ii < trtGenServices->size(); ii++) {
 
    InDetDD::GenericTubeMaker tubeHelper((*trtGenServices)[ii]);
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    }
    logName = "Trt" + logName;

    const GeoTube* serviceTube = dynamic_cast<const GeoTube*>(serviceTubeTmp);
    if(serviceTube==0) continue;

    const GeoTubs* tubeUp = 0;
    const GeoTubs* tubeDown = 0;

    if (tubeHelper.name() == "FwdServ") { 
      tubeUp   = new GeoTubs(serviceTube->getRMin(),serviceTube->getRMax(),serviceTube->getZHalfLength(),phiWidSQ*0.5,M_PI-phiWidSQ);
      tubeDown = new GeoTubs(serviceTube->getRMin(),serviceTube->getRMax(),serviceTube->getZHalfLength(),M_PI+phiWidSQ*0.5,M_PI-phiWidSQ);
    } else if( tubeHelper.volData().maxRadius() > minRofGap)  {
      // Subtract RailGap out of services
      tubeUp   = new GeoTubs(serviceTube->getRMin(),serviceTube->getRMax(),serviceTube->getZHalfLength(),phiWid*0.5,M_PI-phiWid);
      tubeDown = new GeoTubs(serviceTube->getRMin(),serviceTube->getRMax(),serviceTube->getZHalfLength(),M_PI+phiWid*0.5,M_PI-phiWid);
    }    

    if(tubeUp==0) continue;

    std::string materialName = tubeHelper.materialName();
    //const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = m_materialManager->getMaterial(materialName);
   
    const GeoLogVol* servLogUp = new GeoLogVol(logName,tubeUp,material);
    const GeoLogVol* servLogDown = new GeoLogVol(logName,tubeDown,material);
    GeoPhysVol* servPhysUp = new GeoPhysVol(servLogUp);
    GeoPhysVol* servPhysDown = new GeoPhysVol(servLogDown);
    tubeHelper.placeVolTwoSide(motherP,motherM,servPhysUp);
    tubeHelper.placeVolTwoSide(motherP,motherM,servPhysDown);

    serviceTube->unref();
  }

  railGap1->unref(); 
  railGap2->unref(); 

}
