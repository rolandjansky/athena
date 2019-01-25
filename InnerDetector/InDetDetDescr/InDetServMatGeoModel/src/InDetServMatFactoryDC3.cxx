/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatFactoryDC3.h"

// Pixel, SCT and TRT factories
#include "InDetServMatGeoModel/PixelServMatFactoryDC3.h"
#include "InDetServMatGeoModel/SCT_ServMatFactoryDC3.h"
#include "InDetServMatGeoModel/TRT_ServMatFactoryDC3.h"
#include "InDetServMatGeoModel/EndPlateFactory.h"
#include "InDetServMatGeoModel/SupportRailFactory.h"
#include "InDetServMatGeoModel/SquirrelCageFactory.h"

// Extra material
#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"

// Material manager
#include "InDetGeoModelUtils/InDetMaterialManager.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoShapeUnion.h"  
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

// RDBAccessSvc includes
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/PhysicalConstants.h"
#include <iostream>

InDetServMatFactoryDC3::InDetServMatFactoryDC3(const InDetDD::AthenaComps * athenaComps)
  : InDetDD::DetectorFactoryBase(athenaComps),
    m_manager(0)
{}


InDetServMatFactoryDC3::~InDetServMatFactoryDC3()
{

}



//## Other Operations (implementation)
void InDetServMatFactoryDC3::create(GeoPhysVol *world )
{


  // create a new det manager
  m_manager = new InDetDD::InDetServMatManager();

  // Get the material manager:
  
  DecodeVersionKey sctVersionKey(geoDbTagSvc(),"SCT");
  DecodeVersionKey trtVersionKey(geoDbTagSvc(),"TRT");
  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  DecodeVersionKey atlasVersionKey(geoDbTagSvc(),"ATLAS");


  msg(MSG::DEBUG) << "Building InDet Service Material with ATLAS Version Tag: " << atlasVersionKey.tag() << endmsg;
  msg(MSG::DEBUG) << "                                with InDet Version Tag: " << indetVersionKey.tag() << " at Node: "
      << indetVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with TRT   Version Tag: " << trtVersionKey.tag() << " at Node: "
      << trtVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with SCT   Version Tag: " << sctVersionKey.tag() << " at Node: "
      << sctVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << " InDetServices Version " << rdbAccessSvc()->getChildTag("InDetServices", indetVersionKey.tag(), indetVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " TRT           Version " << rdbAccessSvc()->getChildTag("TRT", trtVersionKey.tag(), trtVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " SCT           Version " << rdbAccessSvc()->getChildTag("SCT", sctVersionKey.tag(), sctVersionKey.node()) << endmsg;


  IRDBRecordset_ptr atls = rdbAccessSvc()->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());

  IRDBRecordset_ptr servEnvelopeTable = 
      rdbAccessSvc()->getRecordsetPtr("InDetServEnvelope", indetVersionKey.tag(), indetVersionKey.node());
  const IRDBRecord *envelopes = (*servEnvelopeTable)[0];

  InDetMaterialManager * materialManager = new InDetMaterialManager("InDetServMatMaterialManager", getAthenaComps());


  double ZMaxBrlTRT =        envelopes->getDouble("ZMAXBRLTRT") * Gaudi::Units::mm;
  double ZMaxBrlSCT =        envelopes->getDouble("ZMAXBRLSCT") * Gaudi::Units::mm;
  double ZMinFwdSCTandTRT =  envelopes->getDouble("ZMINFWDSCTANDTRT") * Gaudi::Units::mm;
  double ZMinSCTServInTRT =  envelopes->getDouble("ZMINSCTSERVINTRT") * Gaudi::Units::mm;
  double ZMaxSCTServInTRT =  envelopes->getDouble("ZMAXSCTSERVINTRT") * Gaudi::Units::mm;
  double ZMinPixServ    =    envelopes->getDouble("ZMINPIXSERV") * Gaudi::Units::mm;
  double ZMaxFwdTRTC =       envelopes->getDouble("ZMAXFWDTRTC") * Gaudi::Units::mm;
  double ZMaxIDet =          (*atls)[0]->getDouble("IDETZMX") * Gaudi::Units::cm;  // 3490 mm

  double RMinBrlSCT =        envelopes->getDouble("RMINBRLSCT") * Gaudi::Units::mm;
  double RMaxBrlTRT =        envelopes->getDouble("RMAXBRLTRT") * Gaudi::Units::mm;
  double RMinBrlTRT =        envelopes->getDouble("RMINBRLTRT") * Gaudi::Units::mm;
  double RMaxFwdTRT =        envelopes->getDouble("RMAXFWDTRT") * Gaudi::Units::mm;
  double RMaxFwdSCT =        envelopes->getDouble("RMAXFWDSCT") * Gaudi::Units::mm;
  double RMaxFwdTRTC =       envelopes->getDouble("RMAXFWDTRTC") * Gaudi::Units::mm;
  double RMinPixServ =       envelopes->getDouble("RMINPIXSERV") * Gaudi::Units::mm;
  double RMaxPixServ =       envelopes->getDouble("RMAXPIXSERV") * Gaudi::Units::mm;
  double RMaxIDet =          (*atls)[0]->getDouble("IDETOR") * Gaudi::Units::cm; // 1147 mm



  //
  // Create the envelope for the Pixel Services:
  //
  const GeoMaterial* air = materialManager->getMaterial("std::Air");
  GeoPcon* pixServP = new GeoPcon(0.,2*Gaudi::Units::pi);
  GeoPcon* pixServM = new GeoPcon(0.,2*Gaudi::Units::pi);

  // Plane 1: Start at the end of the SCT endcap
  pixServP->addPlane(ZMinPixServ,  RMinPixServ, RMaxPixServ);
  pixServM->addPlane(-ZMinPixServ, RMinPixServ, RMaxPixServ);

  // Plane 2 and 3: at the end of the TRT endcap 
  pixServP->addPlane(ZMaxFwdTRTC, RMinPixServ, RMaxPixServ);
  pixServP->addPlane(ZMaxFwdTRTC, RMinPixServ, RMaxIDet);  
  pixServM->addPlane(-ZMaxFwdTRTC, RMinPixServ, RMaxPixServ);
  pixServM->addPlane(-ZMaxFwdTRTC, RMinPixServ, RMaxIDet);  

  // Plane 4 at the end of the ID
  pixServP->addPlane(ZMaxIDet, RMinPixServ, RMaxIDet);
  pixServM->addPlane(-ZMaxIDet, RMinPixServ, RMaxIDet);

  const GeoShapeUnion *ServVolAux = new GeoShapeUnion(pixServP, pixServM);

  // This is the volume for the TRT/SCT services
  GeoPcon *sctTrtServ = new GeoPcon(0.,2*Gaudi::Units::pi);
  
  // THE BEGINNING
  sctTrtServ->addPlane(-ZMaxIDet, RMaxFwdTRTC, RMaxIDet);

  // SCT services in TRT endcap
  sctTrtServ->addPlane(-ZMaxSCTServInTRT, RMaxFwdTRTC, RMaxIDet);
  sctTrtServ->addPlane(-ZMaxSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServ->addPlane(-ZMinSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServ->addPlane(-ZMinSCTServInTRT, RMaxFwdTRT, RMaxIDet);

  // Beginning of Ecap SCT and TRT
  sctTrtServ->addPlane(-ZMinFwdSCTandTRT, RMaxFwdTRT, RMaxIDet);
  sctTrtServ->addPlane(-ZMinFwdSCTandTRT, RMinBrlSCT, RMaxIDet);

  // End of Barrel SCT (SCT barrel services)
  sctTrtServ->addPlane(-ZMaxBrlSCT, RMinBrlSCT, RMaxIDet);
  sctTrtServ->addPlane(-ZMaxBrlSCT, RMinBrlTRT, RMaxIDet);

  // Plane, end of barrel TRT (TRT barrel services)
  sctTrtServ->addPlane(-ZMaxBrlTRT, RMinBrlTRT, RMaxIDet);
  sctTrtServ->addPlane(-ZMaxBrlTRT, RMaxBrlTRT, RMaxIDet);


  // THE OTHER SIDE
  // Plane, end of barrel TRT (TRT barrel services)
  sctTrtServ->addPlane(ZMaxBrlTRT, RMaxBrlTRT, RMaxIDet);
  sctTrtServ->addPlane(ZMaxBrlTRT, RMinBrlTRT, RMaxIDet);

  // End of Barrel SCT (SCT barrel services)
  sctTrtServ->addPlane(ZMaxBrlSCT, RMinBrlTRT, RMaxIDet);
  sctTrtServ->addPlane(ZMaxBrlSCT, RMinBrlSCT, RMaxIDet);

  // Beginning of Ecap SCT and TRT 
  sctTrtServ->addPlane(ZMinFwdSCTandTRT, RMinBrlSCT, RMaxIDet);
  sctTrtServ->addPlane(ZMinFwdSCTandTRT, RMaxFwdTRT, RMaxIDet);

  // SCT services in TRT endcap
  sctTrtServ->addPlane(ZMinSCTServInTRT, RMaxFwdTRT, RMaxIDet);
  sctTrtServ->addPlane(ZMinSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServ->addPlane(ZMaxSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServ->addPlane(ZMaxSCTServInTRT, RMaxFwdTRTC, RMaxIDet);

  // The end
  sctTrtServ->addPlane(ZMaxIDet, RMaxFwdTRTC, RMaxIDet);

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
  PixelServMatFactoryDC3 PSMF(getAthenaComps(), materialManager);
  PSMF.create(ServPhys);

  // SCT Services:
  SCT_ServMatFactoryDC3 SSMF(getAthenaComps(), materialManager);
  SSMF.create(ServPhys);

  // TRT Services:
  TRT_ServMatFactoryDC3 TSMF(getAthenaComps(), materialManager);
  TSMF.create(ServPhys);

  // EP:
  EndPlateFactory EPMF(getAthenaComps(), materialManager);
  EPMF.create(ServPhys);

  // Rails:
  SupportRailFactory RAMF(getAthenaComps(), materialManager);
  RAMF.create(ServPhys);

  // SquirrelCage:
  SquirrelCageFactory SQCF(getAthenaComps(), materialManager);
  SQCF.create(ServPhys);

  // Extra material
  InDetDD::DistortedMaterialManager xMatManager;
  InDetDD::ExtraMaterial xMat(&xMatManager);
  xMat.add(ServPhys, "InDetServMat");

  delete materialManager;
}


const InDetDD::InDetServMatManager* InDetServMatFactoryDC3::getDetectorManager () const {
  return m_manager;
}

