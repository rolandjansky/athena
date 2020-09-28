/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelDiskSLHC.h"
#include "GeoPixelDiskSupports.h"
#include "GeoPixelSiCrystal.h"
#include "GeoPixelRingSLHC.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;
using std::abs;


GeoPixelDiskSLHC::GeoPixelDiskSLHC(InDetDD::PixelDetectorManager* ddmgr,
                                   PixelGeometryManager* mgr)
  : GeoVPixelFactory (ddmgr, mgr)
{}

/**
 * Each pixel disk will build 
 **/
GeoVPhysVol* GeoPixelDiskSLHC::Build( ) {

  //
  // Build disk
  //
  int idisk = m_gmt_mgr->GetLD();
  double rmin = m_gmt_mgr->PixelDiskRMin(true); //including supports
  double rmax = m_gmt_mgr->PixelDiskRMax(true); //including supports
  double halflength = m_gmt_mgr->PixelDiskThickness() / 2.;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* diskLog = new GeoLogVol("diskLog",diskTube,air);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(diskLog);

  //
  // Upgrade pixel: each disk contains NRING rings
  //
  int nrings = m_gmt_mgr->PixelDiskNRings();

  // Set numerology
  m_DDmgr->numerology().setNumRingsForDisk(m_gmt_mgr->GetLD(),nrings);

  for (int iring = 0; iring < nrings; iring++) {

    // offset from disk center to (front- or back-face) ring center
    m_gmt_mgr->SetEta( iring );
    // Set numerology
    m_DDmgr->numerology().setNumPhiModulesForDiskRing(m_gmt_mgr->GetLD(),iring,m_gmt_mgr->PixelDiskRingNModules());
    //
    // Define the sensor to be used, it will be the same for each ring
    //
    GeoPixelSiCrystal theSensor(m_DDmgr, m_gmt_mgr, false);


    double zpos = m_gmt_mgr->PixelRingZpos();

    // -1 means near IP, +1 means away from IP, 0 means alternate
    int ringSide = m_gmt_mgr->PixelRingSide();

    if (ringSide == 0 || ringSide == 1) {
      // front rings first (away from IP)
      m_gmt_mgr->setDiskFront();
      GeoPixelRingSLHC gpRingF(m_DDmgr, m_gmt_mgr, theSensor);
      GeoTransform* xfront = new GeoTransform( GeoTrf::Translate3D(0, 0, zpos) );
      diskPhys->add( new GeoNameTag("PixelECRing") );
      diskPhys->add( new GeoIdentifierTag(2*iring) );
      diskPhys->add( xfront );
      diskPhys->add( gpRingF.Build() );
    }

    
    if (ringSide == 0 || ringSide == -1) {
      // then back rings (near IP)
      m_gmt_mgr->setDiskBack();
      GeoPixelRingSLHC gpRingB(m_DDmgr, m_gmt_mgr, theSensor);
      GeoTransform* xback = new GeoTransform( GeoTrf::Translate3D(0, 0, -zpos) );
      diskPhys->add( new GeoNameTag("PixelECRing") );
      diskPhys->add( new GeoIdentifierTag(2*iring+1) ); // unique
      diskPhys->add( xback );
      diskPhys->add( gpRingB.Build() );
    }
  }
  //
  // Place the supports for the disks:
  //
  GeoPixelDiskSupports pds (m_DDmgr, m_gmt_mgr);
  for(int ii =0; ii< pds.NCylinders(); ii++) {
    pds.SetCylinder(ii);
    GeoNameTag* tag = new GeoNameTag("DiskSupport");
    GeoTransform* xform = new GeoTransform( GeoTrf::Translate3D(0, 0, pds.ZPos()) );
    diskPhys->add(tag);
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
  }


  // Extra Material - use m_gmt_mgr from base class GeoVPixelFactory
  InDetDD::ExtraMaterial xMat(m_gmt_mgr->distortedMatManager());
  xMat.add(diskPhys,"PixelDisk");

  std::ostringstream ostr;
  ostr <<"PixelDisk"<< idisk;
  xMat.add(diskPhys,ostr.str());
  if(m_gmt_mgr->isAside()) {
    // EndcapA
    xMat.add(diskPhys,"PixelDiskA");
    xMat.add(diskPhys,"PixelDiskA"+ostr.str());
  }
  else if(m_gmt_mgr->isCside()) {
    // EndcapC
    xMat.add(diskPhys,"PixelDiskC");
    xMat.add(diskPhys,"PixelDiskC"+ostr.str());
  }

  return diskPhys;
}
