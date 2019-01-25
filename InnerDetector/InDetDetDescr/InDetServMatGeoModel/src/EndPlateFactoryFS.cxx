/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/EndPlateFactoryFS.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoBox.h"  
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


EndPlateFactoryFS::EndPlateFactoryFS(StoreGateSvc *detStore,
				     ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_msg("EndPlateFactoryFS")
{
  
}


EndPlateFactoryFS::~EndPlateFactoryFS()
{
}



//## Other Operations (implementation)
void EndPlateFactoryFS::create(GeoPhysVol *motherP, GeoPhysVol *motherM)
{

  // Get the material manager:
  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;
  
  // Get the SvcLocator 
  DecodeVersionKey indetVersionKey("InnerDetector");
  

   IRDBRecordset_ptr shell  = m_rdbAccess->getRecordsetPtr("EPShell",  indetVersionKey.tag(), indetVersionKey.node());
   IRDBRecordset_ptr insert = m_rdbAccess->getRecordsetPtr("EPInsert", indetVersionKey.tag(), indetVersionKey.node());
   IRDBRecordset_ptr ribs   = m_rdbAccess->getRecordsetPtr("EPRibs",   indetVersionKey.tag(), indetVersionKey.node());
//----------------------------------------------------------------------------------
//    std::string matName =  mat[(int) (*pbfi)[ii]->getFloat("MAT")];
//    const GeoMaterial* cylMat = materialManager->getMaterial(matName);
//    double rmin = (*pbfi)[ii]->getFloat("RIN")*Gaudi::Units::cm;
//----------------------------------------------------------------------------------
    double safety = 0.01*Gaudi::Units::mm;
    double maxRofEP = 1075.0*Gaudi::Units::mm - safety; // Interfere with TRT PatchPanel1
    double RibConnection = 550.0*Gaudi::Units::mm;

    maxRofEP      = (*shell)[0]->getDouble("EPMAXR")*Gaudi::Units::mm - safety; 
    RibConnection = (*ribs)[0]->getDouble("RIBCONNECTION")*Gaudi::Units::mm; 
//
//     Internal shell. Default (initial) values
//
    double rminInt = 425.*Gaudi::Units::mm;
    double rmaxInt = 1040.*Gaudi::Units::mm;
    double thickShell = 3.*Gaudi::Units::mm;
    double thick2     = 10.*Gaudi::Units::mm;
    double zposEP     = 3370.*Gaudi::Units::mm;
    double zleng      = 42.*Gaudi::Units::mm;
    rminInt    = (*shell)[0]->getDouble("RMININT")*Gaudi::Units::mm; 
    rmaxInt    = (*shell)[0]->getDouble("RMAXINT")*Gaudi::Units::mm;
    thickShell = (*shell)[0]->getDouble("THICKSHELL")*Gaudi::Units::mm;
    thick2     = (*shell)[0]->getDouble("THICKADD")*Gaudi::Units::mm;
    zposEP     = (*shell)[0]->getDouble("ZSTART")*Gaudi::Units::mm;
    zleng      = (*shell)[0]->getDouble("ZSHIFT")*Gaudi::Units::mm;
 
    GeoTube* shellInt1 = new GeoTube(rminInt,rminInt+thick2,zleng*0.5);
    GeoTube* shellInt2 = new GeoTube(rminInt,rmaxInt,thickShell*0.5);

    const GeoMaterial*  shellMat = materialManager->getMaterial((*shell)[0]->getString("MATERIAL"));

    const GeoLogVol* shellLogInt1 = new GeoLogVol("EPShellInt",shellInt1,shellMat);
    GeoVPhysVol* shellPhysInt1 = new GeoPhysVol(shellLogInt1);
    const GeoLogVol* shellLogInt2 = new GeoLogVol("EPShellInt",shellInt2,shellMat);
    GeoVPhysVol* shellPhysInt2 = new GeoPhysVol(shellLogInt2);

    GeoTrf::Translate3D servpos11(0.,0., zposEP+zleng*0.5);
    GeoTrf::Translate3D servpos12(0.,0., zposEP+zleng+thickShell*0.5);
    GeoTrf::Vector3D servpos21(0.,0., -zposEP-zleng*0.5);
    GeoTrf::Vector3D servpos22(0.,0., -zposEP-zleng-thickShell*0.5);

    GeoTransform *xform11 = new GeoTransform(servpos11);
    GeoTransform *xform12 = new GeoTransform(servpos12);
    GeoTransform *xform21 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos21));
    GeoTransform *xform22 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos22));
    motherP->add(xform11);
    motherP->add(shellPhysInt1);
    motherP->add(xform12);
    motherP->add(shellPhysInt2);
    motherM->add(xform21);
    motherM->add(shellPhysInt1);
    motherM->add(xform22);
    motherM->add(shellPhysInt2);

//----------------------------------------------------------------------------------
//
//     External shell (default/initial values)

    double zgap     = 50.*Gaudi::Units::mm;
    double rminExt  = 250.*Gaudi::Units::mm;
    double rmaxExt  = maxRofEP;
    
    zgap    = (*shell)[0]->getDouble("ZGAP")*Gaudi::Units::mm;
    rminExt = (*shell)[0]->getDouble("RMINEXT")*Gaudi::Units::mm;


    const GeoTube*   shellExt    = new GeoTube(rminExt,rmaxExt,thickShell/2.);
    const GeoLogVol* shellLogExt = new GeoLogVol("EPShellExt",shellExt,shellMat);
    GeoVPhysVol* shellPhysExt = new GeoPhysVol(shellLogExt);

    GeoTrf::Translate3D servpos3(0.,0., zposEP+zleng+thickShell+zgap+thickShell/2.);
    GeoTrf::Vector3D servpos4(0.,0.,-zposEP-zleng-thickShell-zgap-thickShell/2.);
    GeoTransform *xform3 = new GeoTransform(servpos3);
    GeoTransform *xform4 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos4));

    motherP->add(xform3);
    motherP->add(shellPhysExt);
    motherM->add(xform4);
    motherM->add(shellPhysExt);

//----------------------------------------------------------------------------------
//
//     Insert - default (initial) values

    double zthick  = 16.0*Gaudi::Units::mm;
    double zinsert = 3018.*Gaudi::Units::mm-zthick;
    double rminins = 252.*Gaudi::Units::mm;
    double rmaxins = 435.*Gaudi::Units::mm;
    double rthick  = 5.*Gaudi::Units::mm;
    double zlengi   = 410.*Gaudi::Units::mm;

    zthick  = (*insert)[0]->getDouble("ZTHICK")*Gaudi::Units::mm;
    zinsert = (*insert)[0]->getDouble("ZINSERT")*Gaudi::Units::mm;
    rminins = (*insert)[0]->getDouble("RMININS")*Gaudi::Units::mm;
    rmaxins = (*insert)[0]->getDouble("RMAXINS")*Gaudi::Units::mm;
    rthick  = (*insert)[0]->getDouble("RTHICK")*Gaudi::Units::mm;
    zlengi  = (*insert)[0]->getDouble("ZLENGINS")*Gaudi::Units::mm;

    GeoTube* Insert1 = new GeoTube(rminins,rmaxins+rthick,zthick*0.5);
    GeoTube* Insert2 = new GeoTube(rmaxins,rmaxins+rthick,(zlengi-zthick)*0.5);

    const GeoMaterial*  insertMat = materialManager->getMaterial((*insert)[0]->getString("MATERIAL"));

    const GeoLogVol* InsertLog1 = new GeoLogVol("EPInsert",Insert1,insertMat);
    GeoVPhysVol* InsertPhys1 = new GeoPhysVol(InsertLog1);
    const GeoLogVol* InsertLog2 = new GeoLogVol("EPInsert",Insert2,insertMat);
    GeoVPhysVol* InsertPhys2 = new GeoPhysVol(InsertLog2);

    GeoTrf::Translate3D servpos51(0.,0., zinsert+zthick*0.5);
    GeoTrf::Translate3D servpos52(0.,0., zinsert+(zthick+zlengi)*0.5);
    GeoTrf::Vector3D servpos61(0.,0., -zinsert-zthick*0.5);
    GeoTrf::Vector3D servpos62(0.,0., -zinsert-(zthick+zlengi)*0.5);
    GeoTransform *xform51 = new GeoTransform(servpos51);
    GeoTransform *xform52 = new GeoTransform(servpos52);
    GeoTransform *xform61 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos61));
    GeoTransform *xform62 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,M_PI,0.),servpos62));

    motherP->add(xform51);
    motherP->add(InsertPhys1);
    motherP->add(xform52);
    motherP->add(InsertPhys2);
    motherM->add(xform61);
    motherM->add(InsertPhys1);
    motherM->add(xform62);
    motherM->add(InsertPhys2);


//----------------------------------------------------------------------------------
//
//     Short ribs - default (initial) values
//     Initial position is along X axis then move and rotate
//
    double ribY   = 12.0*Gaudi::Units::mm;
    double ribZ   = zgap - safety;
    double ribX   = 380.0*Gaudi::Units::mm;
    double posX   = 550.0*Gaudi::Units::mm+ribX/2.;

    ribY   = (*ribs)[0]->getDouble("SHORTWID")*Gaudi::Units::mm;
    ribZ   = zgap - safety;
    ribX   = (*ribs)[0]->getDouble("SHORTLENG")*Gaudi::Units::mm;
    posX   = (*ribs)[0]->getDouble("SHORTRSTART")*Gaudi::Units::mm + ribX/2.;

    const GeoBox* ribShort = new GeoBox(ribX/2., ribY/2., ribZ/2.);


    const GeoMaterial*  ribsMat = materialManager->getMaterial((*ribs)[0]->getString("MATERIAL"));
//  std::cout << "Ribs mat. den.="<<ribsMat->getDensity()<<" Rad.length="<<ribsMat->getRadLength()<<'\n';
//----------
    const GeoLogVol* ribShortLog = new GeoLogVol("ShortRib",ribShort,ribsMat);
    GeoVPhysVol* ribShortPhys = new GeoPhysVol(ribShortLog);


    GeoTransform *xrib;

    for (int ip=1; ip<12; ip++){
      if( ip==3 || ip==6 || ip==9) continue;
      double angl= ip*M_PI/6.;
      GeoTrf::Vector3D ribpos_pos( posX*cos(angl), posX*sin(angl), zposEP+zleng+thickShell+zgap/2.);
      GeoTrf::Vector3D ribpos_neg( posX*cos(angl), posX*sin(angl),-zposEP-zleng-thickShell-zgap/2.);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_pos));
      motherP->add(xrib);
      motherP->add(ribShortPhys);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_neg));
      motherM->add(xrib);
      motherM->add(ribShortPhys);
    }

//---------------------------------------------------------------------------------
//     Long ribs (initial position is along X axis then move and rotate)
//
    double ribY1   = 20.0*Gaudi::Units::mm;
    double ribZ1   = zgap - safety;
    double ribX1   = RibConnection-250.*Gaudi::Units::mm;
    double posX1   = 250.*Gaudi::Units::mm+ribX1/2.;

    double ribY2   = 30.0*Gaudi::Units::mm;
    double ribZ2   = zgap - safety;
    double ribX2   = maxRofEP - RibConnection;
    double posX2   = RibConnection+ribX2/2.;

    ribY1   = (*ribs)[0]->getDouble("LONGWID1")*Gaudi::Units::mm;
    ribZ1   = zgap - safety;
    ribX1   = RibConnection - (*ribs)[0]->getDouble("LONGLENG1")*Gaudi::Units::mm;  // LONGLENG1 is a RMIN of ribs
    posX1   = (*ribs)[0]->getDouble("LONGLENG1")*Gaudi::Units::mm + ribX1/2.;       // It's determined by Pixel volume -> so 250.0!!!
 
    ribY2   = (*ribs)[0]->getDouble("LONGWID2")*Gaudi::Units::mm;
    ribZ2   = zgap - safety;
    ribX2   = maxRofEP - RibConnection;
    posX2   = RibConnection+ribX2/2.;



    const GeoBox* ribLong1 = new GeoBox(ribX1/2.       , ribY1/2., ribZ1/2.);
    const GeoBox* ribLong2 = new GeoBox(ribX2/2.-safety, ribY2/2., ribZ2/2.);

    const GeoLogVol* ribLong1Log = new GeoLogVol("LongRib1",ribLong1,shellMat);
    const GeoLogVol* ribLong2Log = new GeoLogVol("LongRib2",ribLong2,shellMat);
    GeoVPhysVol* ribLong1Phys = new GeoPhysVol(ribLong1Log);
    GeoVPhysVol* ribLong2Phys = new GeoPhysVol(ribLong2Log);


    for (int ip=0; ip<4; ip++){
      double angl= ip*M_PI/2.;
// 1st part
      GeoTrf::Vector3D ribpos_pos1( posX1*cos(angl), posX1*sin(angl), zposEP+zleng+thickShell+zgap/2.);
      GeoTrf::Vector3D ribpos_neg1( posX1*cos(angl), posX1*sin(angl),-zposEP-zleng-thickShell-zgap/2.);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_pos1));
      motherP->add(xrib);
      motherP->add(ribLong1Phys);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_neg1));
      motherM->add(xrib);
      motherM->add(ribLong1Phys);
// 2nd part
      GeoTrf::Vector3D ribpos_pos2( posX2*cos(angl), posX2*sin(angl), zposEP+zleng+thickShell+zgap/2.);
      GeoTrf::Vector3D ribpos_neg2( posX2*cos(angl), posX2*sin(angl),-zposEP-zleng-thickShell-zgap/2.);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_pos2));
      motherP->add(xrib);
      motherP->add(ribLong2Phys);
      xrib = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(-angl,0.,0.),ribpos_neg2));
      motherM->add(xrib);
      motherM->add(ribLong2Phys);
    }
}


