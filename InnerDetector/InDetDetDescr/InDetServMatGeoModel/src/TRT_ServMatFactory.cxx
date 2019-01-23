/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/TRT_ServMatFactory.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <iostream>

TRT_ServMatFactory::TRT_ServMatFactory(const InDetDD::AthenaComps * athenaComps)
  : InDetDD::SubDetectorFactoryBase(athenaComps)
{
}


TRT_ServMatFactory::~TRT_ServMatFactory()
{
  // It owns the material manager
  delete m_materialManager;
}



//## Other Operations (implementation)
void TRT_ServMatFactory::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building TRT Service Material" << endmsg;

  //double epsilon = 0.002;
  
  DecodeVersionKey atlasVersionKey(geoDbTagSvc(),"ATLAS");
  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  DecodeVersionKey trtVersionKey(geoDbTagSvc(),"TRT");

  std::string railversion =  rdbAccessSvc()->getChildTag("IDDetailedRail",indetVersionKey.tag(),indetVersionKey.node());
  if(!railversion.empty()) {
    // ______________________________________________________________________________________________________________________
    // Geometry with ID rail

  IRDBRecordset_ptr atls = rdbAccessSvc()->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr cage = rdbAccessSvc()->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr trtGenServices = rdbAccessSvc()->getRecordsetPtr("TRTGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Doesn't exist at time of writing but we include it to keep things general
  IRDBRecordset_ptr weightTable = rdbAccessSvc()->getRecordsetPtr("TrtWeights", trtVersionKey.tag(), trtVersionKey.node());
  
  IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("InDetServMatScaling", indetVersionKey.tag(), indetVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
  m_materialManager = new InDetMaterialManager("TRT_MaterialManager", getAthenaComps());
  m_materialManager->addWeightTable(weightTable, "trt");
  m_materialManager->addScalingTable(scalingTable);

  //VK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;

//VK 26.03.2007  Construct a gap for SquirrelCage ribbon
  double  rminInt    = (*cage)[0]->getDouble("RINGRMIN")*Gaudi::Units::mm;

//created by Adam Agocs 
  IRDBRecordset_ptr commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon",indetVersionKey.tag(), indetVersionKey.node());
  if(commonParameters->size()==0)
    commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon","IDDetRailCommon-00");

  double yWidthUSP1    = (*commonParameters)[0]->getDouble("YWIDTHUSP1");
  double yRailSup      = (*commonParameters)[0]->getDouble("YRAILSUP");
  double yRailSupPart3 = (*commonParameters)[0]->getDouble("YRAILSUPPART3");
  double gapOfRSF      = (*commonParameters)[0]->getDouble("GAPOFRSF");
  double yWidthTRSB    = (*commonParameters)[0]->getDouble("YWIDTHTRSB");

  double coordY = yWidthUSP1/2. - yRailSup + (yRailSupPart3 - gapOfRSF/2.);
 
  double phiTop = asin((yWidthTRSB + coordY) / (rminInt));
  double phiBot = asin((yWidthTRSB/2. - coordY) / (rminInt));

  const GeoShape*  railGap1  = new GeoTubs( rminInt, outROfIDet, endZOfIDet,  -phiBot,      phiBot + phiTop);
  const GeoShape*  railGap2  = new GeoTubs( rminInt, outROfIDet, endZOfIDet,  M_PI - phiTop, phiBot + phiTop);
  railGap1->ref(); 
  railGap2->ref(); 

  for (unsigned int ii =0; ii < trtGenServices->size(); ii++) {
 
    InDetDD::GenericTubeMaker tubeHelper((*trtGenServices)[ii]);
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    }
    logName = "Trt" + logName;

    const GeoShape* serviceTube = serviceTubeTmp;
    
    if( tubeHelper.volData().maxRadius() > rminInt && tubeHelper.volData().phiDelta() > 359.9*Gaudi::Units::degree)  
    {
      // Subtract RailGap out of services
      serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap2).subtract(*railGap1);
    }    

    std::string materialName = tubeHelper.materialName();
    //const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = m_materialManager->getMaterial(materialName);
   
    const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
    GeoVPhysVol* servPhys = new GeoPhysVol(servLog);
    
    if (tubeHelper.name() == "FwdServ")
    {
      GeoTransform *xform1  = new GeoTransform(GeoTrf::Translate3D(0, 0, tubeHelper.volData().zMid() ));
      GeoTransform *xform2  = new GeoTransform(GeoTrf::Translate3D(0, 0, -tubeHelper.volData().zMid() ));
      mother->add(xform1);   mother->add(servPhys);   mother->add(xform2);   mother->add(servPhys);
    }
    else {
      tubeHelper.placeVolume(mother, servPhys);
    }
  }
  railGap1->unref(); 
  railGap2->unref(); 
  }
  else {
    // ______________________________________________________________________________________________________________________
    // Geometry without ID rail
  IRDBRecordset_ptr atls = rdbAccessSvc()->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr cage = rdbAccessSvc()->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr trtGenServices = rdbAccessSvc()->getRecordsetPtr("TRTGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Doesn't exist at time of writing but we include it to keep things general
  IRDBRecordset_ptr weightTable = rdbAccessSvc()->getRecordsetPtr("TrtWeights", trtVersionKey.tag(), trtVersionKey.node());
  
  IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("InDetServMatScaling", indetVersionKey.tag(), indetVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
  m_materialManager = new InDetMaterialManager("TRT_MaterialManager", getAthenaComps());
  m_materialManager->addWeightTable(weightTable, "trt");
  m_materialManager->addScalingTable(scalingTable);

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



    const GeoShape* serviceTube = serviceTubeTmp;


    if (tubeHelper.name() == "FwdServ") { 
      const GeoShape*  ribSup1  = new GeoTubs( tubeHelper.volData().rmin(), tubeHelper.volData().rmax(), 0.5*tubeHelper.volData().length(), 
					       -phiWidSQ/2.,      phiWidSQ);
      const GeoShape*  ribSup2  = new GeoTubs( tubeHelper.volData().rmin(), tubeHelper.volData().rmax(), 0.5*tubeHelper.volData().length(), 
					       -phiWidSQ/2.+M_PI, phiWidSQ);
      serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*ribSup1).subtract(*ribSup2);
    } else if( tubeHelper.volData().maxRadius() > minRofGap && tubeHelper.volData().phiDelta() > 359.9*Gaudi::Units::degree)  {
      // Subtract RailGap out of services
      serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap2).subtract(*railGap1);
    }    

    std::string materialName = tubeHelper.materialName();
    //const GeoMaterial* material = materialManager()->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = materialManager()->getMaterial(materialName);
   
    const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
    GeoVPhysVol* servPhys = new GeoPhysVol(servLog);
    
    tubeHelper.placeVolume(mother, servPhys);

  }

  railGap1->unref(); 
  railGap2->unref(); 
  }
}

