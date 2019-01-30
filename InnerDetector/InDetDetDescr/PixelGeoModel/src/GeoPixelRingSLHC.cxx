/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelRingSLHC.h"
#include "GeoPixelModule.h"
#include "GeoPixelSiCrystal.h"

// #include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
//#include <string>
#include <sstream>
//using std::string;

//#include <iostream>

GeoPixelRingSLHC::GeoPixelRingSLHC(GeoPixelSiCrystal& sensor)
  : m_sensor(sensor)
{
  //nop, code moved to build to prevent leak (coverity 115114)
}


/**
 * Each pixel disk will build 
 **/
//
// Upgrade pixel: each ring contains NRING rings, each ring with NMODULES modules
// Place the ring sectors (on both sides)
//
GeoVPhysVol* GeoPixelRingSLHC::Build() {
  //(sar) Original block was in c'tor...
  // Dimensions from class methods
  //
  double rmin = m_gmt_mgr->PixelRingRMin(); // Default is 0.01 Gaudi::Units::mm safety added
  double rmax = m_gmt_mgr->PixelRingRMax(); // Default is 0.01 Gaudi::Units::mm safety added
  double halflength = m_gmt_mgr->PixelRingThickness()/2.;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* ringTube = new GeoTube(rmin,rmax,halflength);
  GeoLogVol * ringLog = new GeoLogVol("ringLog",ringTube,air);
  //(sar)... until here
  
  GeoPixelModule gpmod(m_sensor);
  GeoFullPhysVol* ringPhys = new GeoFullPhysVol(ringLog);
  int idisk = m_gmt_mgr->GetLD();
  int iring = m_gmt_mgr->Eta();
  int nmodules = m_gmt_mgr->PixelDiskRingNModules();

  // in case no modules are defined for the ring
  if(nmodules==0) return ringPhys;

  // deltaPhi is angle between two adjacent modules regardless of side of the disk
  double deltaPhi = 360.*Gaudi::Units::deg / (double)nmodules;

  // This is the start angle of the even modules
  // Start angle could eventually come from the database...
  // double startAngle = m_gmt_mgr->PixelECStartPhi();
  double startAngle = deltaPhi*0.5;

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_gmt_mgr->PixelRingRcenter();

  // -1 means near IP (back), +1 means away from IP (front), 0 means alternate
  int ringSide = m_gmt_mgr->PixelRingSide();
  double stagger = m_gmt_mgr->PixelRingStagger();

  
  // loop over modules
  for (int imod=0; imod<nmodules; imod++) {

    // If ringside == 0 then we only build even modules if front side and odd modules if back side
    if (!ringSide && m_gmt_mgr->isDiskFront() &&   imod%2 )    continue; // skip odd module  
    if (!ringSide && m_gmt_mgr->isDiskBack()  && !(imod%2)) continue ;  // skip even module
    
    // Fi stagger is non zero, alternating modules are stagger low/high.
    double zpos = 0.5*stagger * (imod%2 ? 1 : -1);

    //
    // Build both endcaps the same but re-number phiId in endcap C to
    // get correct offline numbering.  Endcap C is obtained by
    // rotating endcap A by 180 around y axis.
    //
    // Numbering goes in opposite direction with module 0 becoming 23
    // (current layout).  E.g. mapping is 0<->23, 24<->47.
    //
    //current layout:       int phiId = (m_gmt_mgr->GetSide()>0) ? imod : (3*nmodules-imod-1)%(2*nmodules);
    int phiId = m_gmt_mgr->isAside() ? imod : (2*nmodules-imod-1)%(nmodules);
    m_gmt_mgr->SetPhi( phiId );

    double angle = imod*deltaPhi+startAngle;

    GeoTrf::Transform3D rm = GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    if( m_gmt_mgr->isDiskBack() ) rm = rm * GeoTrf::RotateX3D(180.*Gaudi::Units::deg);
    rm = rm * GeoTrf::RotateZ3D(angle);

    GeoTrf::Vector3D pos(moduleRadius,0,zpos);
    pos = GeoTrf::RotateZ3D(angle)*pos;
    GeoAlignableTransform* xform = new GeoAlignableTransform(GeoTrf::Translate3D(pos.x(),pos.y(),pos.z())*rm);
    GeoVPhysVol * modulePhys = gpmod.Build();
    std::ostringstream ostr; 
    ostr << "Disk" << idisk << "_Sector" << iring;
    ringPhys->add(new GeoNameTag(ostr.str()));
    ringPhys->add(new GeoIdentifierTag(phiId));
    ringPhys->add(xform);
    ringPhys->add(modulePhys);

    // Now store the xform by identifier:
    Identifier id = m_sensor.getID();
    m_DDmgr->addAlignableTransform(0,id,xform,modulePhys);
  }

  return ringPhys;
}


  // Note: the following comments refer to current atlas layouts
  //       They are left here for reference, just in case.
  // In Global coordinate system (X,Y,Z):
  //--------------------------------------
  // (for reference see GWG's June 20, 2005 Pixel Offline Mtg presentation, p.4)
  // The module has X axis (depth axis) out of the plane pointing
  // towards the chips. Z axis is the eta direction.  To place it on
  // the disk we rotate 90 deg around Y. This leaves the depth axis
  // pointing in the -ve Z direction. For the back of the disk this is
  // the correct orientation. For the front of the disk we rotate
  // 180 deg around X.


  // Looking from IP at module at ~12:00 (+ve global Y)
  //
  // For side A, Global axes: 
  //
  //    Y ^
  //      |
  //      |
  //  X<--x
  //
  // Z axis into page
  //

  // Module on front of disk (closest to IP)
  // MCC side
  // 
  //    0   F        ^z
  //    .   .        |
  //    .   .        |  
  //    .   .    <-- x     Module ref system (Hit frame)
  //    7   8    y

  // Module on back of disk (furthest from IP)
  // Still looking from IP.
  //
  // Chip side
  //    F   0        ^z
  //    .   .        |
  //    .   .        |  
  //    .   .        .-->  Module ref system (Hit frame)
  //    8   z           y
  //
  //

  // Even modules (front of disk, delta z < 0)
  // and Odd modules (back of disk, delta z > 0)
