/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatFactoryDC2.h"

// Pixel, SCT and TRT factories
#include "InDetServMatGeoModel/PixelServMatFactoryDC2.h"
#include "InDetServMatGeoModel/SCT_ServMatFactoryDC2.h"
#include "InDetServMatGeoModel/TRT_ServMatFactoryDC2.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoShapeUnion.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

InDetServMatFactoryDC2::InDetServMatFactoryDC2(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_manager(0),
  m_msg("InDetServMatFactoryDC2")
{
  
}


InDetServMatFactoryDC2::~InDetServMatFactoryDC2()
{

}



//## Other Operations (implementation)
void InDetServMatFactoryDC2::create(GeoPhysVol *world)
{
  if (not world){
    msg(MSG::FATAL) << "GeoPhysVol pointer 'world' is null" << endmsg;
    return;
  }
  // create a new det manager
  m_manager = new InDetDD::InDetServMatManager();

  // Get the material manager:
  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;


  DecodeVersionKey atlasVersionKey("ATLAS");
  DecodeVersionKey indetVersionKey("InnerDetector");
  DecodeVersionKey sctVersionKey("SCT");
  DecodeVersionKey trtVersionKey("TRT");


  msg(MSG::DEBUG) << "Building InDet Service Material with ATLAS Version Tag: " << atlasVersionKey.tag() << endmsg;
  msg(MSG::DEBUG) << "                                with InDet Version Tag: " << indetVersionKey.tag() << " at Node: "
      << indetVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with SCT   Version Tag: " << sctVersionKey.tag() << " at Node: "
      << sctVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with TRT   Version Tag: " << trtVersionKey.tag() << " at Node: "
      << trtVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << " InDetServices Version " << m_rdbAccess->getChildTag("InDetServices", indetVersionKey.tag(), indetVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " SCT           Version " << m_rdbAccess->getChildTag("SCT", sctVersionKey.tag(), sctVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " TRT           Version " << m_rdbAccess->getChildTag("TRT", trtVersionKey.tag(), trtVersionKey.node()) << endmsg;


  IRDBRecordset_ptr atls = m_rdbAccess->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());

  IRDBRecordset_ptr pbfi = m_rdbAccess->getRecordsetPtr("PBFISERV",     indetVersionKey.tag(), indetVersionKey.node());

  IRDBRecordset_ptr zscg = m_rdbAccess->getRecordsetPtr("ZSCG",         sctVersionKey.tag(), sctVersionKey.node());
  IRDBRecordset_ptr sctg = m_rdbAccess->getRecordsetPtr("SCTG",         sctVersionKey.tag(), sctVersionKey.node());

  IRDBRecordset_ptr trtb = m_rdbAccess->getRecordsetPtr("TRTB",         trtVersionKey.tag(), trtVersionKey.node());
  IRDBRecordset_ptr trtg = m_rdbAccess->getRecordsetPtr("TRTG",         trtVersionKey.tag(), trtVersionKey.node());


  //
  // Numbers of common use:
  // (end of TRT volume from TRT_GeoModel)

//  double epsilon = 0.001;

  double endZOfBTRT =                (*trtb)[0]->getDouble("ZLEN")*Gaudi::Units::cm;
  double endZOfBSCT =                (*sctg)[0]->getDouble("HLEN")*Gaudi::Units::cm;
  double begZOfFSCT =                (*zscg)[0]->getDouble("ZLOEND")*Gaudi::Units::cm;
  double endZOfFSCT =                (*zscg)[0]->getDouble("ZHIEND")*Gaudi::Units::cm;
  double endZOfFTRT =                ((*trtb)[15]->getDouble("ZPOSA") +
				      ((*trtb)[15]->getDouble("ZLEN")+(*trtb)[15]->getDouble("ZGAP"))/2.
				      + ((*trtb)[2]->getDouble("ZPOSA")-(*trtb)[1]->getDouble("ZPOSA"))*3 
				      + (*trtb)[1]->getDouble("ZLEN")/2.)*Gaudi::Units::cm;
 
  double endZOfIDet =                (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;

  // This is endOfEndCapVolumeAB 
  //double begZOfSCTServInTRT = ((trtb[7].zposa + (trtb[7].zlen + trtb[7].zgap)/2.) +
  //			       (trtb[8].zposa - trtb[7].zposa)*7 + trtb[7].zlen/2.)*Gaudi::Units::cm;

  // This is beginningOfEndCapVolumeC 
  //  double endZOfSCTServInTRT = (trtb[15].zposa + (trtb[15].zlen + trtb[15].zgap)/2. - 
  //			       trtb[1].zlen/2.)*Gaudi::Units::cm;

  // The SCT services go from 2755.306 to 2775.306 Gaudi::Units::mm 
  // The TRT has a gap from 2712.25 to 2829.75 mm
  // We hard wire an envelope for these services instead.
  double begZOfSCTServInTRT = 2755. * Gaudi::Units::mm;
  double endZOfSCTServInTRT = 2776. * Gaudi::Units::mm;

  //std::cout << "Begin SCT services " << begZOfSCTServInTRT << std::endl;
  //std::cout << "End SCT services " << endZOfSCTServInTRT << std::endl;


  double outROfPixel =               (*zscg)[0]->getDouble("RINEND")*Gaudi::Units::cm;
  double inROfFTRT  =                (*trtb)[15]->getDouble("RI")*Gaudi::Units::cm;
  double outROfFSCT =                (*zscg)[0]->getDouble("ROUEND")*Gaudi::Units::cm;
  double outROfBSCT =                (*sctg)[0]->getDouble("RMAX")*Gaudi::Units::cm;
  double outROfPixelCables =         (*pbfi)[0]->getFloat("ROUT")*Gaudi::Units::cm;
  double outROfIDet =                (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double outROfTRT =                 (*trtg)[0]->getDouble("RMAX")*Gaudi::Units::cm;

  //
  // Create the envelope for the Pixel Services:
  //
  const GeoMaterial* air = materialManager->getMaterial("std::Air");
  GeoPcon* pixServP = new GeoPcon(0.,2*M_PI);
  GeoPcon* pixServM = new GeoPcon(0.,2*M_PI);

  // Plane 1: Start at the end of the SCT endcap
  pixServP->addPlane(endZOfFSCT, outROfPixel, inROfFTRT);
  pixServM->addPlane(-endZOfFSCT, outROfPixel, inROfFTRT);

  // Plane 2 and 3: at the end of the TRT endcap
  pixServP->addPlane(endZOfFTRT, outROfPixel, inROfFTRT);
  pixServP->addPlane(endZOfFTRT, outROfPixel, outROfIDet);  
  pixServM->addPlane(-endZOfFTRT, outROfPixel, inROfFTRT);
  pixServM->addPlane(-endZOfFTRT, outROfPixel, outROfIDet);  

  // Plane 4 at the end of the ID
  pixServP->addPlane(endZOfIDet, outROfPixel, outROfIDet);
  pixServM->addPlane(-endZOfIDet, outROfPixel, outROfIDet);
  const GeoShapeUnion *ServVolAux = new GeoShapeUnion(pixServP, pixServM);

  // This is the volume for the TRT/SCT services
  GeoPcon *sctTrtServ = new GeoPcon(0.,2*M_PI);
  
  // THE BEGINNING
  sctTrtServ->addPlane(-endZOfIDet,outROfPixelCables,outROfIDet);

   // End of Ecap SCT
  sctTrtServ->addPlane(-endZOfFSCT,outROfPixelCables,outROfIDet); 
  sctTrtServ->addPlane(-endZOfFSCT,outROfTRT,outROfIDet);

  // SCT services in TRT endcap
  sctTrtServ->addPlane(-endZOfSCTServInTRT,outROfTRT,outROfIDet);
  sctTrtServ->addPlane(-endZOfSCTServInTRT,outROfFSCT,outROfIDet);
  sctTrtServ->addPlane(-begZOfSCTServInTRT,outROfFSCT,outROfIDet);
  sctTrtServ->addPlane(-begZOfSCTServInTRT,outROfTRT,outROfIDet);

  // Beginning of Ecap SCT
  sctTrtServ->addPlane(-begZOfFSCT,outROfTRT,outROfIDet); 
  sctTrtServ->addPlane(-begZOfFSCT,outROfPixel,outROfIDet);

  // End of Barrel SCT
  sctTrtServ->addPlane(-endZOfBSCT,outROfPixel,outROfIDet);
  sctTrtServ->addPlane(-endZOfBSCT,outROfBSCT,outROfIDet);

  // Plane, end of barrel TRT
  sctTrtServ->addPlane(-endZOfBTRT,outROfBSCT,outROfIDet);
  sctTrtServ->addPlane(-endZOfBTRT,outROfTRT,outROfIDet);


  // THE OTHER SIDE
  // Plane, end of barrel TRT
  sctTrtServ->addPlane(endZOfBTRT,outROfTRT,outROfIDet);
  sctTrtServ->addPlane(endZOfBTRT,outROfBSCT,outROfIDet);

  // End of Barrel SCT
  sctTrtServ->addPlane(endZOfBSCT,outROfBSCT,outROfIDet);
  sctTrtServ->addPlane(endZOfBSCT,outROfPixel,outROfIDet);

  // Beginning of Ecap SCT
  sctTrtServ->addPlane(begZOfFSCT,outROfPixel,outROfIDet);
  sctTrtServ->addPlane(begZOfFSCT,outROfTRT,outROfIDet);

  // SCT services in TRT endcap
  sctTrtServ->addPlane(begZOfSCTServInTRT,outROfTRT,outROfIDet);
  sctTrtServ->addPlane(begZOfSCTServInTRT,outROfFSCT,outROfIDet);
  sctTrtServ->addPlane(endZOfSCTServInTRT,outROfFSCT,outROfIDet);
  sctTrtServ->addPlane(endZOfSCTServInTRT,outROfTRT,outROfIDet);

  // End of Ecap SCT
  sctTrtServ->addPlane(endZOfFSCT,outROfTRT,outROfIDet);
  sctTrtServ->addPlane(endZOfFSCT,outROfPixelCables,outROfIDet);
  
  // The end
  sctTrtServ->addPlane(endZOfIDet,outROfPixelCables,outROfIDet);

  const GeoShapeUnion *ServVol = new GeoShapeUnion(sctTrtServ, ServVolAux);
  const GeoLogVol* ServLog = new GeoLogVol("ServLog",ServVol,air);
  GeoPhysVol* ServPhys = new GeoPhysVol(ServLog);



  // Add this to the world and register it to the manager so GeoG4 will 
  // find it.
  //
  GeoNameTag *tag = new GeoNameTag("InDetServMat");         
  world->add(tag);
  world->add(ServPhys);
  m_manager->addTreeTop(ServPhys);
  // Pixel Services:
  PixelServMatFactoryDC2 PSMF(m_detStore,m_rdbAccess);
  PSMF.create(ServPhys);

  // SCT Services:
  SCT_ServMatFactoryDC2 SSMF(m_detStore,m_rdbAccess);
  SSMF.create(ServPhys);

  // TRT Services:
  TRT_ServMatFactoryDC2 TSMF(m_detStore,m_rdbAccess);
  TSMF.create(ServPhys);

}


const InDetDD::InDetServMatManager* InDetServMatFactoryDC2::getDetectorManager () const {
  return m_manager;
}

