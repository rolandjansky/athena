/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelDisk.h"
#include "GeoPixelModule.h"
#include "GeoPixelDiskSupports.h"
#include "GeoPixelSiCrystal.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

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
#include <sstream>

GeoPixelDisk::GeoPixelDisk() {

}

GeoVPhysVol* GeoPixelDisk::Build( ) {
  // Need to specify some eta. Assume all module the same
  m_gmt_mgr->SetEta(0);
  // angle between two adjacent modules on one side of the disk
  // it is 360 deg / 24 modules = 15 deg	 	
  int nbECSector = m_gmt_mgr->PixelECNSectors1();
  if(nbECSector==0){
    return 0;
  }
  //   
  // Dimensions from class methods.
  //
  double rmin = RMin();
  double rmax = RMax();
  double halflength = Thickness()*0.5;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  std::ostringstream ostr; 
  ostr << m_gmt_mgr->GetLD();
  const GeoLogVol* theDisk = new GeoLogVol("Disk"+ostr.str(),diskTube,air);
  //
  // Define the Sensor to be used here, so it will be the same for all the disk
  GeoPixelSiCrystal theSensor(false);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(theDisk);
  //
  // Place the disk sectors (on both sides):
  //
  // Need to specify some eta. Assume all module the same
  GeoPixelModule psd(theSensor);
  double zpos = m_gmt_mgr->PixelECSiDz1()*0.5;
  double deltaPhi = 360.*Gaudi::Units::deg/ (float) nbECSector;
  // This is the start angle of the even modules (3.75 deg):
  double startAngle = deltaPhi*0.25;
  // Start angle could eventually come from the database...
  // double startAngle = m_gmt_mgr->PixelECStartPhi();

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_gmt_mgr->PixelDiskRMin() + 0.5*m_gmt_mgr->PixelBoardActiveLength();

  // for compatibility with older geometries we have to incorrectly rotate modules in endcap C
  bool oldGeometry = (m_gmt_mgr->dbVersion() < 1);

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

  // Set numerology
  m_gmt_mgr->SetEta(0);
  m_DDmgr->numerology().setNumRingsForDisk(m_gmt_mgr->GetLD(),1);
  m_DDmgr->numerology().setNumPhiModulesForDiskRing(m_gmt_mgr->GetLD(),0,m_gmt_mgr->PixelECNSectors1()*2);

  // Even modules (front of disk, delta z < 0)
  // and Odd modules (back of disk, delta z > 0)
  int pixelECNSectors1 = m_gmt_mgr->PixelECNSectors1();
  for (int ii = 0; ii <  pixelECNSectors1*2; ii++) {

    // Build both endcaps the same but re-number phiId in endcap C to get correct offline numbering.
    // Endcap C is obtained by rotating endcap A by 180 around y axis. Numbering goes in opposite direction
    // with module 0 becoming module 23. Mapping is 0<->23, 24<->47.

    // Add a test to get rid off division by zero coverity error
    int phiId = 0;
    if(pixelECNSectors1>0) phiId = (m_gmt_mgr->GetSide()>0) ? ii : (3*pixelECNSectors1-ii-1)%(pixelECNSectors1*2);

    m_gmt_mgr->SetPhi(phiId);

    double angle = ii*0.5*deltaPhi+startAngle;
    //if ( m_gmt_mgr->GetSide()<0 ) angle = 360*Gaudi::Units::deg-(ii*deltaPhi+startAngle);
    int diskSide = (ii%2) ? +1 : -1; // even: -1, odd +1


    GeoTrf::Transform3D rmX(GeoTrf::Transform3D::Identity());
    if (oldGeometry && m_gmt_mgr->GetSide()<0) {
      if (diskSide > 0) rmX = GeoTrf::RotateX3D(180.*Gaudi::Units::deg); // This is for compatibilty with older geomtries.
    } else {
      if (diskSide < 0) rmX = GeoTrf::RotateX3D(180.*Gaudi::Units::deg); // depth axis points towards disk.
    } 
    GeoTrf::Transform3D rm = GeoTrf::RotateZ3D(angle) * rmX * GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    GeoTrf::Vector3D pos(moduleRadius,0.,diskSide*zpos);
    pos = GeoTrf::RotateZ3D(angle)*pos;
    GeoAlignableTransform* xform = new GeoAlignableTransform(GeoTrf::Translate3D(pos.x(),pos.y(),pos.z())*rm);
    GeoVPhysVol * modulePhys = psd.Build();
    std::ostringstream nameTag; 
    nameTag << "ModuleEC" << phiId;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    diskPhys->add(tag);
    diskPhys->add(new GeoIdentifierTag(phiId));
    diskPhys->add(xform);
    diskPhys->add(modulePhys);

    // Now store the xform by identifier:
    Identifier id = theSensor.getID();
    m_DDmgr->addAlignableTransform(0,id,xform,modulePhys);
  }

  //
  // Place the supports for the disks:
  //
  GeoPixelDiskSupports pds;
  for(int ii =0; ii< pds.NCylinders(); ii++) {
    pds.SetCylinder(ii);
    GeoTransform* xform = new GeoTransform( GeoTrf::Translate3D(0, 0, pds.ZPos()) );
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
  }


  // Extra Material 
  InDetDD::ExtraMaterial xMat(m_gmt_mgr->distortedMatManager());
  xMat.add(diskPhys,"PixelDisc");
  //Defined above as:
  // std::ostringstream ostr; ostr << m_gmt_mgr->GetLD();
  xMat.add(diskPhys,"PixelDisc"+ostr.str());
  if (m_gmt_mgr->GetSide()>0) { // EndcapA
    xMat.add(diskPhys,"PixelDiscA");
    xMat.add(diskPhys,"PixelDiscA"+ostr.str());
  } else {                   // EndcapC
    xMat.add(diskPhys,"PixelDiscC");
    xMat.add(diskPhys,"PixelDiscC"+ostr.str());
  }


  return diskPhys;
}

double GeoPixelDisk::RMin() {
  return m_gmt_mgr->PixelECCarbonRMin("Inner");
}

double GeoPixelDisk::RMax() {
  return m_gmt_mgr->PixelECCarbonRMax("Outer");
}

double GeoPixelDisk::Thickness() {
  //
  // This can be calculated as the minimum thickness possible
  // as for the layers I use a number which is enough to contain
  // the inner part of the detector.
  //
  // 7-1 I switch to the minimum thickness possible as the cables are right
  // outside this volume.
  //
  //  return 10*Gaudi::Units::mm;
  // GWG. It would be nice to get these numbers from the module itself to
  // ensure consistency.
  double safety = 0.01* Gaudi::Units::mm; // This is the safety added to the module.
  double zClearance = 0.5 * Gaudi::Units::mm; // Clearance for misalignments
  double tck = 2*(safety + 0.5*m_gmt_mgr->PixelBoardThickness()
                  + std::max(m_gmt_mgr->PixelHybridThickness(),
			     m_gmt_mgr->PixelChipThickness()+m_gmt_mgr->PixelChipGap())
		  );
  tck += std::max(m_gmt_mgr->PixelECSiDz1(),m_gmt_mgr->PixelECSiDz2());
  tck += 2.*zClearance;
  // TO DO: Add some safety (N.B. I think the module is slightly bigger because of some safety added there)
  return tck;
}


