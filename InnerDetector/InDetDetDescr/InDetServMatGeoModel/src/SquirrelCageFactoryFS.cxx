/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SquirrelCageFactoryFS.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoTransform.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>


SquirrelCageFactoryFS::SquirrelCageFactoryFS(StoreGateSvc *detStore,
					     ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_msg("SquirrelCageFactoryFS")
{
  
}


SquirrelCageFactoryFS::~SquirrelCageFactoryFS()
{
}



//## Other Operations (implementation)
void SquirrelCageFactoryFS::create(GeoPhysVol *motherP, GeoPhysVol *motherM)
{

  // Get the material manager:
  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;
   
  DecodeVersionKey indetVersionKey("InnerDetector");
  
  IRDBRecordset_ptr cage = m_rdbAccess->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());

  // Get table version
  // This is a work around to allow running with older DB releases.
  std::string sqversionStr = m_rdbAccess->getChildTag("SquirrelCage", indetVersionKey.tag(), indetVersionKey.node());
  size_t pos = sqversionStr.find("-");
  int sqversion = 0;
  if (pos != std::string::npos && pos+1 != sqversionStr.size()) {
    std::istringstream tmpStr(sqversionStr.substr(pos+1));
    tmpStr >> sqversion;
  }
  msg(MSG::DEBUG) << sqversionStr << " : " << sqversion << endmsg;
  

//----------------------------------------------------------------------------------
//     Squirrel cage rings
//  Default (initial) ring parameters
//
    double rminInt    = 1074.0*Gaudi::Units::mm;
    double ringThick  = 4.0*Gaudi::Units::mm;
    double ringGap    = 20.*Gaudi::Units::mm;
    double ringWid    = 40.*Gaudi::Units::mm;
//
    rminInt    = (*cage)[0]->getDouble("RINGRMIN")*Gaudi::Units::mm;
    ringThick  = (*cage)[0]->getDouble("RINGTHICK")*Gaudi::Units::mm;
    ringGap    = (*cage)[0]->getDouble("RINGGAP")*Gaudi::Units::mm;
    ringWid    = (*cage)[0]->getDouble("RINGWIDTH")*Gaudi::Units::mm;
//
//--- Default (initial) z positions
    double zposFirstRing  = 805.0*Gaudi::Units::mm+161.0*Gaudi::Units::mm;
    double zposGap1  = 390.*Gaudi::Units::mm;
    double zposGap2  = 402.*Gaudi::Units::mm;
    double zposGap3  = 446.*Gaudi::Units::mm;
    double zposGap4  = 331.*Gaudi::Units::mm;
//
    zposFirstRing  = (*cage)[0]->getDouble("ZBASE")*Gaudi::Units::mm;
    zposGap1  = (*cage)[0]->getDouble("ZGAP1")*Gaudi::Units::mm;
    zposGap2  = (*cage)[0]->getDouble("ZGAP2")*Gaudi::Units::mm;
    zposGap3  = (*cage)[0]->getDouble("ZGAP3")*Gaudi::Units::mm;
    zposGap4  = (*cage)[0]->getDouble("ZGAP4")*Gaudi::Units::mm;
//
// Now support ring
    double rminSup    = 830.0*Gaudi::Units::mm;
    double supThick   = 90.0*Gaudi::Units::mm;
    double supWid     = 12.0*Gaudi::Units::mm;
//
    rminSup    = (*cage)[0]->getDouble("SUPRMIN")*Gaudi::Units::mm;
    supThick   = (*cage)[0]->getDouble("SUPTHICK")*Gaudi::Units::mm;
    supWid     = (*cage)[0]->getDouble("SUPWIDTH")*Gaudi::Units::mm;
//
    double zposSupRing  = zposFirstRing+ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
//
// Now support ribbon
    double ribWid     = 68.0*Gaudi::Units::mm ;
    ribWid = (*cage)[0]->getDouble("RIBWIDTH")*Gaudi::Units::mm;
    double ribLeng    = ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
    double ribThick = 0; 
    if (sqversion >= 3) ribThick = (*cage)[0]->getDouble("RIBTHICK")*Gaudi::Units::mm;
    double safety =0.01;
    double ribThickMax = ringGap - 2*safety;
    if (ribThick == 0 || ribThick > ribThickMax) {
      ribThick = ribThickMax;
    } 
    double rmidRib =  rminInt+ringThick+0.5*ringGap; // Place rib midway between internal and external support.

    GeoTube* ringInt = new GeoTube( rminInt, rminInt+ringThick, ringWid/2.);
    GeoTube* ringExt = new GeoTube( rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2.);
    GeoTube* ringSup = new GeoTube( rminSup, rminSup+supThick, supWid/2.);
// To avoid clashes
//
//  GeoBox*  ribSup  = new GeoBox( ribThick/2, ribWid/2, ribLeng/2.);
//
    double phiWid=ribWid/(rminInt+ringThick+ringGap/2.);
    //GeoTubs*  ribSup  = new GeoTubs( rminInt+ringThick+safety, rminInt+ringGap+ringThick-safety, 
    //                                 ribLeng/2., -phiWid/2.,phiWid);
    GeoTubs*  ribSup  = new GeoTubs( rmidRib-0.5*ribThick, rmidRib+0.5*ribThick, 
                                     ribLeng/2., -phiWid/2.,phiWid);

// Build the Phys Vol
//  const GeoMaterial*  ringMat = materialManager->getMaterial("std::Aluminium");
    const GeoMaterial*  ringMat = materialManager->getMaterial((*cage)[0]->getString("MATERIAL"));
//  std::cout << "Squirrel cage density="<<ringMat->getDensity()<<" Rad.length="<<ringMat->getRadLength()<<'\n';

    const GeoLogVol* ringLogInt = new GeoLogVol("SQringInt",ringInt,ringMat);
    const GeoLogVol* ringLogExt = new GeoLogVol("SQringExt",ringExt,ringMat);
    const GeoLogVol* ringLogSup = new GeoLogVol("SQringSup",ringSup,ringMat);
    const GeoLogVol* ribLogSup  = new GeoLogVol("SQribSup" ,ribSup ,ringMat);
    GeoVPhysVol* ringPhysInt = new GeoPhysVol(ringLogInt);
    GeoVPhysVol* ringPhysExt = new GeoPhysVol(ringLogExt);
    GeoVPhysVol* ringPhysSup = new GeoPhysVol(ringLogSup);
    GeoVPhysVol*  ribPhysSup = new GeoPhysVol(ribLogSup);
    
    GeoTrf::Translate3D  servpos1(0.,0., zposFirstRing+ringWid/2.);
    GeoTrf::Vector3D  servpos2(0.,0.,-zposFirstRing-ringWid/2.);
    GeoTrf::Translate3D  servpos3(0.,0., zposFirstRing+ringWid*3./2. + zposGap1);
    GeoTrf::Vector3D  servpos4(0.,0.,-zposFirstRing-ringWid*3./2. - zposGap1);
    GeoTrf::Translate3D  servpos5(0.,0., zposFirstRing+ringWid*5./2. + zposGap1 + zposGap2);
    GeoTrf::Vector3D  servpos6(0.,0.,-zposFirstRing-ringWid*5./2. - zposGap1 - zposGap2);
    GeoTrf::Translate3D  servpos7(0.,0., zposFirstRing+ringWid*7./2. + zposGap1 + zposGap2 + zposGap3);
    GeoTrf::Vector3D  servpos8(0.,0.,-zposFirstRing-ringWid*7./2. - zposGap1 - zposGap2 - zposGap3);
    GeoTrf::Translate3D  servpos9(0.,0., zposFirstRing+ringWid*9./2. + zposGap1 + zposGap2 + zposGap3 + zposGap4);
    GeoTrf::Vector3D servpos10(0.,0.,-zposFirstRing-ringWid*9./2. - zposGap1 - zposGap2 - zposGap3 - zposGap4);
    GeoTransform *xform1  = new GeoTransform(servpos1);
    GeoTransform *xform2  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos2));
    GeoTransform *xform3  = new GeoTransform(servpos3);
    GeoTransform *xform4  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos4));
    GeoTransform *xform5  = new GeoTransform(servpos5);
    GeoTransform *xform6  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos6));
    GeoTransform *xform7  = new GeoTransform(servpos7);
    GeoTransform *xform8  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos8));
    GeoTransform *xform9  = new GeoTransform(servpos9);
    GeoTransform *xform10 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos10));
    motherP->add(xform1);
    motherP->add(ringPhysInt);
    motherP->add(xform1);
    motherP->add(ringPhysExt);
    motherM->add(xform2);
    motherM->add(ringPhysInt);
    motherM->add(xform2);
    motherM->add(ringPhysExt);
    motherP->add(xform3);
    motherP->add(ringPhysInt);
    motherP->add(xform3);
    motherP->add(ringPhysExt);
    motherM->add(xform4);
    motherM->add(ringPhysInt);
    motherM->add(xform4);
    motherM->add(ringPhysExt);
    motherP->add(xform5);
    motherP->add(ringPhysInt);
    motherP->add(xform5);
    motherP->add(ringPhysExt);
    motherM->add(xform6);
    motherM->add(ringPhysInt);
    motherM->add(xform6);
    motherM->add(ringPhysExt);
    motherP->add(xform7);
    motherP->add(ringPhysInt);
    motherP->add(xform7);
    motherP->add(ringPhysExt);
    motherM->add(xform8);
    motherM->add(ringPhysInt);
    motherM->add(xform8);
    motherM->add(ringPhysExt);
    motherP->add(xform9);
    motherP->add(ringPhysInt);
    motherP->add(xform9);
    motherP->add(ringPhysExt);
    motherM->add(xform10);
    motherM->add(ringPhysInt);
    motherM->add(xform10);
    motherM->add(ringPhysExt);

//Support ring positioning

    GeoTrf::Translate3D  suppos1(0.,0., zposSupRing+supWid/2.);
    GeoTrf::Vector3D  suppos2(0.,0.,-zposSupRing-supWid/2.);
    GeoTransform *sform1  = new GeoTransform(suppos1);
    GeoTransform *sform2  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),suppos2));
    motherP->add(sform1);
    motherP->add(ringPhysSup);
    motherM->add(sform2);
    motherM->add(ringPhysSup);

//Support ribbon positioning


    GeoTrf::Vector3D  ribpos1( 0., 0., zposFirstRing+ribLeng/2.);
    GeoTrf::Vector3D  ribpos2( 0., 0.,-zposFirstRing-ribLeng/2.);
    GeoTransform *rform1  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0,0,0),ribpos1));
    GeoTransform *rform2  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,0.,M_PI),ribpos1));
    GeoTransform *rform3  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0,0,0),ribpos2));
    GeoTransform *rform4  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,0.,M_PI),ribpos2));


    motherP->add(rform1);
    motherP->add(ribPhysSup);
    motherP->add(rform2);
    motherP->add(ribPhysSup);
    motherM->add(rform3);
    motherM->add(ribPhysSup);
    motherM->add(rform4);
    motherM->add(ribPhysSup);

//    std::cout << "End of SquirrelCage building !!!" << zposFirstRing<< '\n';
//----------------------------------------------------------------------------------
}


