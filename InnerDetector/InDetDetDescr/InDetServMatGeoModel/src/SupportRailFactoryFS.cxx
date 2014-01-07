/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SupportRailFactoryFS.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoTransform.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"


#include <iostream>
#include <math.h>


SupportRailFactoryFS::SupportRailFactoryFS(StoreGateSvc *detStore,
					   ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_msg("SupportRailFactoryFS")
{
  
}


SupportRailFactoryFS::~SupportRailFactoryFS()
{
}



//## Other Operations (implementation)
void SupportRailFactoryFS::create(GeoPhysVol *motherP,GeoPhysVol *motherM)
{

  // Get the material manager:  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endreq;
   
  // Get the SvcLocator 
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
 
  DecodeVersionKey atlasVersionKey(geoModel, "ATLAS");
  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");
 
//  std::cout <<" Version!!="<<atlasVersionKey.tag()<<", "<<atlasVersionKey.node()<<'\n';
//  std::cout <<" Version!!="<<indetVersionKey.tag()<<", "<<indetVersionKey.node()<<'\n';
//---------------------------------------------------------------------------------
//     Long ribs (initial position is along X axis then move and rotate)
//
//      Th
//      ___
//     |  |
//     |  |  Wid
//     |  |
//     |__|
//
// Default(initial) values
//    
    double RMAX_ID      = 1150.0*CLHEP::mm -1.0*CLHEP::mm;   //Some safety margin
    double railLengthB  = 1600.0*CLHEP::mm;
    double railWidthB   = 5.5*CLHEP::mm;
    double railThickB   = 34.7*CLHEP::mm;
//
    double railLengthE  = 2600.0*CLHEP::mm;
    double railWidthE   = 14.*CLHEP::mm;
    double railThickE   = 14.7*CLHEP::mm;
//
    double suppLength   = 6800.0*CLHEP::mm;
    double suppWidth    = 54.*CLHEP::mm;
    double suppThick    = 22.6*CLHEP::mm;
//
//   Database 
//
    IRDBRecordset_ptr atls      = m_rdbAccess->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
    IRDBRecordset_ptr railrec   = m_rdbAccess->getRecordsetPtr("IDRails", indetVersionKey.tag(), indetVersionKey.node());
    IRDBRecordset_ptr endplate  = m_rdbAccess->getRecordsetPtr("EPShell", indetVersionKey.tag(), indetVersionKey.node());


//    const GeoMaterial*  steel = materialManager->getMaterial("std::Iron");
//    const GeoMaterial*  alum  = materialManager->getMaterial("std::Aluminium");
    const GeoMaterial*  steel = materialManager->getMaterial((*railrec)[0]->getString("MATERIAL"));
    const GeoMaterial*  alum  = materialManager->getMaterial((*railrec)[0]->getString("MATSUP"));

//  std::cout << "Rail Steel density="<<steel->getDensity()<<" Rad.length="<<steel->getRadLength()<<'\n';
//  std::cout << "Rail Aluminium density="<<alum->getDensity()<<" Rad.length="<<alum->getRadLength()<<'\n';




    
    double epsilon = 0.01;  // +Some safety margin
 
     RMAX_ID = (*atls)[0]->getDouble("IDETOR")*CLHEP::cm;  
     railLengthB = (*railrec)[0]->getDouble("LENGTHB")*CLHEP::mm;
     railWidthB  = (*railrec)[0]->getDouble("WIDTHB")*CLHEP::mm;
     railThickB  = (*railrec)[0]->getDouble("THICKB")*CLHEP::mm;
  //------------ Limited by EP ExternalShell
     railLengthE = (*endplate)[0]->getDouble("ZSTART")*CLHEP::mm  
                  +(*endplate)[0]->getDouble("ZSHIFT")*CLHEP::mm
                  +(*endplate)[0]->getDouble("ZGAP")*CLHEP::mm   - railLengthB*0.5;
     railWidthE  = (*railrec)[0]->getDouble("WIDTHE")*CLHEP::mm;
     railThickE  = (*railrec)[0]->getDouble("THICKE")*CLHEP::mm;
 
     suppLength = railLengthB + 2.*railLengthE;
     suppWidth  = (*railrec)[0]->getDouble("WIDTHSUP")*CLHEP::mm;
     suppThick  = (*railrec)[0]->getDouble("THICKSUP")*CLHEP::mm;
//
// To avoid rail corner outside ID envelope
     RMAX_ID = sqrt(RMAX_ID*RMAX_ID-suppWidth*suppWidth/4.)-epsilon;  

//--------------------------------------------------------------------------------

    const GeoBox* railBarrel = new GeoBox(railThickB*0.5, railWidthB*0.5, railLengthB*0.25);
    const GeoBox* railEndcap = new GeoBox(railThickE*0.5, railWidthE*0.5, railLengthE*0.5);
    const GeoBox* support    = new GeoBox(suppThick*0.5,  suppWidth*0.5,  suppLength*0.25);

    const GeoLogVol* railBarrelLog = new GeoLogVol("railBarrel",railBarrel,steel);
    const GeoLogVol* railEndcapLog = new GeoLogVol("railEndcap",railEndcap,steel);
    const GeoLogVol* supportLog    = new GeoLogVol("RailSupport",support,alum);
    GeoVPhysVol* railBarrelPhys = new GeoPhysVol(railBarrelLog);
    GeoVPhysVol* railEndcapPhys = new GeoPhysVol(railEndcapLog);
    GeoVPhysVol* supportPhys = new GeoPhysVol(supportLog);


    GeoTransform* pos = 0;
//
//    Barrel steel rails
//
    double safety=0.01; // to provide safety gap for G4

    CLHEP::Hep3Vector railBpos1P( ( RMAX_ID-suppThick-railThickB*0.5-safety), 0., railLengthB*0.25);
    CLHEP::Hep3Vector railBpos2P( (-RMAX_ID+suppThick+railThickB*0.5+safety), 0., railLengthB*0.25);
    CLHEP::Hep3Vector railBpos1M( ( RMAX_ID-suppThick-railThickB*0.5-safety), 0.,-railLengthB*0.25);
    CLHEP::Hep3Vector railBpos2M( (-RMAX_ID+suppThick+railThickB*0.5+safety), 0.,-railLengthB*0.25);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos1P));
    motherP->add(pos);
    motherP->add(railBarrelPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos2P));
    motherP->add(pos);
    motherP->add(railBarrelPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos1M));
    motherM->add(pos);
    motherM->add(railBarrelPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos2M));
    motherM->add(pos);
    motherM->add(railBarrelPhys);
//
//    Endcap steel rails
//
    CLHEP::Hep3Vector railEpos1( ( RMAX_ID-suppThick-railThickE*0.5-safety), 0., (railLengthB+railLengthE)*0.5);
    CLHEP::Hep3Vector railEpos2( (-RMAX_ID+suppThick+railThickE*0.5+safety), 0., (railLengthB+railLengthE)*0.5);
    CLHEP::Hep3Vector railEpos3( ( RMAX_ID-suppThick-railThickE*0.5-safety), 0.,-(railLengthB+railLengthE)*0.5);
    CLHEP::Hep3Vector railEpos4( (-RMAX_ID+suppThick+railThickE*0.5+safety), 0.,-(railLengthB+railLengthE)*0.5);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos1));
    motherP->add(pos);
    motherP->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos2));
    motherP->add(pos);
    motherP->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos3));
    motherM->add(pos);
    motherM->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos4));
    motherM->add(pos);
    motherM->add(railEndcapPhys);
//
//    Rail supports
//
    
    CLHEP::Hep3Vector suppPos1P( ( RMAX_ID-suppThick*0.5), 0., suppLength*0.25);
    CLHEP::Hep3Vector suppPos2P( (-RMAX_ID+suppThick*0.5), 0., suppLength*0.25);
    CLHEP::Hep3Vector suppPos1M( ( RMAX_ID-suppThick*0.5), 0.,-suppLength*0.25);
    CLHEP::Hep3Vector suppPos2M( (-RMAX_ID+suppThick*0.5), 0.,-suppLength*0.25);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1P));
    motherP->add(pos);
    motherP->add(supportPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2P));
    motherP->add(pos);
    motherP->add(supportPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1M));
    motherM->add(pos);
    motherM->add(supportPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2M));
    motherM->add(pos);
    motherM->add(supportPhys);

}


