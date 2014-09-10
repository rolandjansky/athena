/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelDiskSLHC.h"
#include "PixelGeoModel/GeoPixelDiskSupports.h"
#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "PixelGeoModel/GeoPixelRingSLHC.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;
using std::abs;


GeoPixelDiskSLHC::GeoPixelDiskSLHC() 
{}

/**
 * Each pixel disk will build 
 **/
GeoVPhysVol* GeoPixelDiskSLHC::Build( ) {

  //
  // Build disk
  //
  int idisk = gmt_mgr->GetLD();
  double rmin = gmt_mgr->PixelDiskRMin(true); //including supports
  double rmax = gmt_mgr->PixelDiskRMax(true); //including supports
  double halflength = gmt_mgr->PixelDiskThickness() / 2.;
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* diskLog = new GeoLogVol("diskLog",diskTube,air);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(diskLog);

  //
  // Upgrade pixel: each disk contains NRING rings
  //
  int nrings = gmt_mgr->PixelDiskNRings();

  // Set numerology
  DDmgr->numerology().setNumRingsForDisk(gmt_mgr->GetLD(),nrings);

  for (int iring = 0; iring < nrings; iring++) {

    // offset from disk center to (front- or back-face) ring center
    gmt_mgr->SetEta( iring );
    // Set numerology
    DDmgr->numerology().setNumPhiModulesForDiskRing(gmt_mgr->GetLD(),iring,gmt_mgr->PixelDiskRingNModules());
    //
    // Define the sensor to be used, it will be the same for each ring
    //
    GeoPixelSiCrystal theSensor(false);


    double zpos = gmt_mgr->PixelRingZpos();

    // -1 means near IP, +1 means away from IP, 0 means alternate
    int ringSide = gmt_mgr->PixelRingSide();

    if (ringSide == 0 || ringSide == 1) {
      // front rings first (away from IP)
      gmt_mgr->setDiskFront();
      GeoPixelRingSLHC gpRingF(theSensor);
      GeoTransform* xfront = new GeoTransform( HepGeom::Translate3D(0, 0, zpos) );
      diskPhys->add( new GeoNameTag("PixelECRing") );
      diskPhys->add( new GeoIdentifierTag(2*iring) );
      diskPhys->add( xfront );
      diskPhys->add( gpRingF.Build() );
    }

    
    if (ringSide == 0 || ringSide == -1) {
      // then back rings (near IP)
      gmt_mgr->setDiskBack();
      GeoPixelRingSLHC gpRingB(theSensor);
      GeoTransform* xback = new GeoTransform( HepGeom::Translate3D(0, 0, -zpos) );
      diskPhys->add( new GeoNameTag("PixelECRing") );
      diskPhys->add( new GeoIdentifierTag(2*iring+1) ); // unique
      diskPhys->add( xback );
      diskPhys->add( gpRingB.Build() );
    }
  }
  //
  // Place the supports for the disks:
  //
  GeoPixelDiskSupports pds;
  for(int ii =0; ii< pds.NCylinders(); ii++) {
    pds.SetCylinder(ii);
    GeoNameTag* tag = new GeoNameTag("DiskSupport");
    GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0, 0, pds.ZPos()) );
    diskPhys->add(tag);
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
  }


  // Extra Material - use gmt_mgr from base class GeoVPixelFactory
  InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  xMat.add(diskPhys,"PixelDisk");

  std::ostringstream ostr;
  ostr <<"PixelDisk"<< idisk;
  xMat.add(diskPhys,ostr.str());
  if(gmt_mgr->isAside()) {
    // EndcapA
    xMat.add(diskPhys,"PixelDiskA");
    xMat.add(diskPhys,"PixelDiskA"+ostr.str());
  }
  else if(gmt_mgr->isCside()) {
    // EndcapC
    xMat.add(diskPhys,"PixelDiskC");
    xMat.add(diskPhys,"PixelDiskC"+ostr.str());
  }

  return diskPhys;
}
