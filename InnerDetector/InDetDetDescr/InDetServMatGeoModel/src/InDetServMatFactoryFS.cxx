/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatFactoryFS.h"

// Pixel, SCT and TRT factories
#include "InDetServMatGeoModel/PixelServMatFactoryFS.h"
#include "InDetServMatGeoModel/SCT_ServMatFactoryFS.h"
#include "InDetServMatGeoModel/TRT_ServMatFactoryFS.h"
#include "InDetServMatGeoModel/EndPlateFactoryFS.h"
#include "InDetServMatGeoModel/SupportRailFactoryFS.h"
#include "InDetServMatGeoModel/SquirrelCageFactoryFS.h"

// Extra material
#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"

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

#include "GaudiKernel/PhysicalConstants.h"

#include <iostream>

InDetServMatFactoryFS::InDetServMatFactoryFS(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_manager(0),
  m_msg("InDetServMatFactoryFS")
{
  
}


InDetServMatFactoryFS::~InDetServMatFactoryFS()
{

}



//## Other Operations (implementation)
void InDetServMatFactoryFS::create(GeoPhysVol *world )
{

  // create a new det manager
  m_manager = new InDetDD::InDetServMatManager();

  // Get the material manager:
  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;
  
  DecodeVersionKey sctVersionKey("SCT");
  DecodeVersionKey trtVersionKey("TRT");
  DecodeVersionKey indetVersionKey("InnerDetector");
  DecodeVersionKey atlasVersionKey("ATLAS");


  msg(MSG::DEBUG) << "Building InDet Service Material with ATLAS Version Tag: " << atlasVersionKey.tag() << endmsg;
  msg(MSG::DEBUG) << "                                with InDet Version Tag: " << indetVersionKey.tag() << " at Node: "
      << indetVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with TRT   Version Tag: " << trtVersionKey.tag() << " at Node: "
      << trtVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << "                                with SCT   Version Tag: " << sctVersionKey.tag() << " at Node: "
      << sctVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << " InDetServices Version " << m_rdbAccess->getChildTag("InDetServices", indetVersionKey.tag(), indetVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " TRT           Version " << m_rdbAccess->getChildTag("TRT", trtVersionKey.tag(), trtVersionKey.node()) << endmsg;
  msg(MSG::DEBUG) << " SCT           Version " << m_rdbAccess->getChildTag("SCT", sctVersionKey.tag(), sctVersionKey.node()) << endmsg;


  IRDBRecordset_ptr atls = m_rdbAccess->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());

  IRDBRecordset_ptr servEnvelopeTable = 
      m_rdbAccess->getRecordsetPtr("InDetServEnvelope", indetVersionKey.tag(), indetVersionKey.node());
  const IRDBRecord *envelopes = (*servEnvelopeTable)[0];


  double safety = 0.001 * Gaudi::Units::mm;

  double ZMaxBrlTRT =        envelopes->getDouble("ZMAXBRLTRT") * Gaudi::Units::mm;
  double ZMaxBrlSCT =        envelopes->getDouble("ZMAXBRLSCT") * Gaudi::Units::mm;
  double ZMinFwdSCTandTRT =  envelopes->getDouble("ZMINFWDSCTANDTRT") * Gaudi::Units::mm;
  double ZMinSCTServInTRT =  envelopes->getDouble("ZMINSCTSERVINTRT") * Gaudi::Units::mm;
  double ZMaxSCTServInTRT =  envelopes->getDouble("ZMAXSCTSERVINTRT") * Gaudi::Units::mm;
  double ZMinPixServ    =    envelopes->getDouble("ZMINPIXSERV") * Gaudi::Units::mm;
  double ZMaxFwdTRTC =       envelopes->getDouble("ZMAXFWDTRTC") * Gaudi::Units::mm;
  double ZMaxIDet =          (*atls)[0]->getDouble("IDETZMX") * Gaudi::Units::cm + safety;  // 3470 mm

  double RMinBrlSCT =        envelopes->getDouble("RMINBRLSCT") * Gaudi::Units::mm;
  double RMaxBrlTRT =        envelopes->getDouble("RMAXBRLTRT") * Gaudi::Units::mm;
  double RMinBrlTRT =        envelopes->getDouble("RMINBRLTRT") * Gaudi::Units::mm;
  double RMaxFwdTRT =        envelopes->getDouble("RMAXFWDTRT") * Gaudi::Units::mm;
  double RMaxFwdSCT =        envelopes->getDouble("RMAXFWDSCT") * Gaudi::Units::mm;
  double RMaxFwdTRTC =       envelopes->getDouble("RMAXFWDTRTC") * Gaudi::Units::mm;
  double RMinPixServ =       envelopes->getDouble("RMINPIXSERV") * Gaudi::Units::mm;
  double RMaxPixServ =       envelopes->getDouble("RMAXPIXSERV") * Gaudi::Units::mm;
  double RMaxIDet =          (*atls)[0]->getDouble("IDETOR") * Gaudi::Units::cm + safety; // 1147 mm

  // Since the TRT Wheel C space is not used in some versions and is used by some 
  // other services we simplify the volume.
  // The 1mm is just to ensure the dimensions are considered equal if they differ
  // by less than 1mm.

  // join1 is True if the pixel service volume and SCT/TRT volume are joined 
  //       (ie boolean volume not needed) 
  // join2 is True if RMaxFwdTRTC and RMaxPixServ are ~ the same as RMinPixServ
  //       in which case a couple of planes are not needed in the PCON.

  bool join1 = false; 
  bool join2 = false; 

  if (std::abs(RMaxFwdTRTC - RMinPixServ) <= 1*Gaudi::Units::mm){
    join1 = true;
    join2 = true;
    RMaxFwdTRTC = RMinPixServ;
    RMaxPixServ = RMinPixServ;
  } else if ((RMaxFwdTRTC - 1*Gaudi::Units::mm) <=  RMaxPixServ) {
    join1 = true;
    RMaxPixServ = RMaxFwdTRTC;
  }
    

  //
  // Create the envelope for the Pixel Services:
  // Only need if join1 if false.

  GeoPcon* pixServP = 0;
  GeoPcon* pixServM = 0;
    
  if (!join1) { 
    pixServP = new GeoPcon(0.,2*Gaudi::Units::pi);
    pixServM = new GeoPcon(0.,2*Gaudi::Units::pi);
    
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
  }

  // This is the volume for the TRT/SCT services
  GeoPcon *sctTrtServP = new GeoPcon(0.,2*Gaudi::Units::pi);
  GeoPcon *sctTrtServM = new GeoPcon(0.,2*Gaudi::Units::pi);
  
  // Pixel Services
  if (join1) {
    // THE BEGINNING
    sctTrtServM->addPlane(-ZMaxIDet,    RMinPixServ, RMaxIDet);
    // PP1 region. 
    if (!join2) { 
      sctTrtServM->addPlane(-ZMinPixServ, RMinPixServ, RMaxIDet);
      sctTrtServM->addPlane(-ZMinPixServ, RMaxPixServ, RMaxIDet);
    }
  } else {
    // THE BEGINNING
    sctTrtServM->addPlane(-ZMaxIDet, RMaxFwdTRTC, RMaxIDet);
  }

  // SCT services in TRT endcap
  sctTrtServM->addPlane(-ZMaxSCTServInTRT, RMaxFwdTRTC, RMaxIDet);
  sctTrtServM->addPlane(-ZMaxSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServM->addPlane(-ZMinSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServM->addPlane(-ZMinSCTServInTRT, RMaxFwdTRT, RMaxIDet);

  // Beginning of Ecap SCT and TRT
  sctTrtServM->addPlane(-ZMinFwdSCTandTRT, RMaxFwdTRT, RMaxIDet);
  sctTrtServM->addPlane(-ZMinFwdSCTandTRT, RMinBrlSCT, RMaxIDet);

  // End of Barrel SCT (SCT barrel services)
  sctTrtServM->addPlane(-ZMaxBrlSCT, RMinBrlSCT, RMaxIDet);
  sctTrtServM->addPlane(-ZMaxBrlSCT, RMinBrlTRT, RMaxIDet);

  // Plane, end of barrel TRT (TRT barrel services)
  sctTrtServM->addPlane(-ZMaxBrlTRT, RMinBrlTRT, RMaxIDet);
  sctTrtServM->addPlane(-ZMaxBrlTRT, RMaxBrlTRT, RMaxIDet);
  sctTrtServM->addPlane(0,RMaxBrlTRT, RMaxIDet);

  // THE OTHER SIDE
  sctTrtServP->addPlane(0,RMaxBrlTRT, RMaxIDet);
  // Plane, end of barrel TRT (TRT barrel services)
  sctTrtServP->addPlane(ZMaxBrlTRT, RMaxBrlTRT, RMaxIDet);
  sctTrtServP->addPlane(ZMaxBrlTRT, RMinBrlTRT, RMaxIDet);

  // End of Barrel SCT (SCT barrel services)
  sctTrtServP->addPlane(ZMaxBrlSCT, RMinBrlTRT, RMaxIDet);
  sctTrtServP->addPlane(ZMaxBrlSCT, RMinBrlSCT, RMaxIDet);

  // Beginning of Ecap SCT and TRT 
  sctTrtServP->addPlane(ZMinFwdSCTandTRT, RMinBrlSCT, RMaxIDet);
  sctTrtServP->addPlane(ZMinFwdSCTandTRT, RMaxFwdTRT, RMaxIDet);

  // SCT services in TRT endcap
  sctTrtServP->addPlane(ZMinSCTServInTRT, RMaxFwdTRT, RMaxIDet);
  sctTrtServP->addPlane(ZMinSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServP->addPlane(ZMaxSCTServInTRT, RMaxFwdSCT, RMaxIDet);
  sctTrtServP->addPlane(ZMaxSCTServInTRT, RMaxFwdTRTC, RMaxIDet);

  // Pixel Services
  if (join1) {
    // PP1 region
    if (!join2) { 
      sctTrtServP->addPlane(ZMinPixServ, RMaxPixServ, RMaxIDet);
      sctTrtServP->addPlane(ZMinPixServ, RMinPixServ, RMaxIDet);    
    }
    // The end
    sctTrtServP->addPlane(ZMaxIDet,    RMinPixServ, RMaxIDet);
  } else {

    // The end
    sctTrtServP->addPlane(ZMaxIDet, RMaxFwdTRTC, RMaxIDet);
  }

  const GeoShape * ServVolP = 0;
  const GeoShape * ServVolM = 0;


  if (pixServP == 0) {
    ServVolP = sctTrtServP;
    ServVolM = sctTrtServM;
  } else {
    ServVolP = new GeoShapeUnion(sctTrtServP, pixServP);
    ServVolM = new GeoShapeUnion(sctTrtServM, pixServM);
  }

  const GeoMaterial* air = materialManager->getMaterial("std::Air");
  const GeoLogVol* ServLogM = new GeoLogVol("ServLog",ServVolM,air);
  GeoPhysVol* ServPhysM = new GeoPhysVol(ServLogM);
  const GeoLogVol* ServLogP = new GeoLogVol("ServLog",ServVolP,air);
  GeoPhysVol* ServPhysP = new GeoPhysVol(ServLogP);



  // Add this to the world and register it to the manager so GeoG4 will 
  // find it.
  //
  GeoNameTag *tag = new GeoNameTag("InDetServMat");         
  world->add(tag);
  world->add(ServPhysM);
  m_manager->addTreeTop(ServPhysM);
  world->add(tag);
  world->add(ServPhysP);
  m_manager->addTreeTop(ServPhysP);
  // Pixel Services:
  PixelServMatFactoryFS PSMF(m_detStore,m_rdbAccess);
  PSMF.create(ServPhysP,ServPhysM);

  // SCT Services:
  SCT_ServMatFactoryFS SSMF(m_detStore,m_rdbAccess);
  SSMF.create(ServPhysP,ServPhysM);

  // TRT Services:
  TRT_ServMatFactoryFS TSMF(m_detStore,m_rdbAccess);
  TSMF.create(ServPhysP,ServPhysM);

  // EP:
  EndPlateFactoryFS EPMF(m_detStore,m_rdbAccess);
  EPMF.create(ServPhysP,ServPhysM);

  // Rails:
  SupportRailFactoryFS RAMF(m_detStore,m_rdbAccess);
  RAMF.create(ServPhysP,ServPhysM);

  // SquirrelCage:
  SquirrelCageFactoryFS SQCF(m_detStore,m_rdbAccess);
  SQCF.create(ServPhysP,ServPhysM);

  // Extra material
  InDetDD::DistortedMaterialManager xMatManager;
  InDetDD::ExtraMaterial xMat(&xMatManager);
  // ------ !!!! Is it a right way to do that????
  xMat.add(ServPhysP, "InDetServMat");
  xMat.add(ServPhysM, "InDetServMat");
}


const InDetDD::InDetServMatManager* InDetServMatFactoryFS::getDetectorManager () const {
  return m_manager;
}

