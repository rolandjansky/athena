/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelDisk.h"
#include "PixelGeoModel/GeoPixelModule.h"
#include "PixelGeoModel/GeoPixelDiskSupports.h"
#include "PixelGeoModel/GeoPixelSiCrystal.h"

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
#include <sstream>

GeoPixelDisk::GeoPixelDisk() {

}

GeoVPhysVol* GeoPixelDisk::Build( ) {

  //   
  // Dimensions from class methods.
  //
  double rmin = RMin();
  double rmax = RMax();
  double halflength = Thickness()/2.;
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  std::ostringstream ostr; ostr << gmt_mgr->GetLD();
  const GeoLogVol* theDisk = new GeoLogVol("Disk"+ostr.str(),diskTube,air);

  //
  // Define the Sensor to be used here, so it will be the same for all the disk
  GeoPixelSiCrystal theSensor(false);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(theDisk);
  //
  // Place the disk sectors (on both sides):
  //
  // Need to specify some eta. Assume all module the same
  gmt_mgr->SetEta(0);
  GeoPixelModule psd(theSensor);
  double zpos = gmt_mgr->PixelECSiDz1()/2.;

  // angle between two adjacent modules on one side of the disk
  // it is 360 deg / 24 modules = 15 deg	 	
  int nbECSector = gmt_mgr->PixelECNSectors1();
  if(nbECSector==0) return 0;
  double deltaPhi = 360.*CLHEP::deg/ (float) nbECSector;
 
  // This is the start angle of the even modules (3.75 deg):
  double startAngle = deltaPhi/4.;
  // Start angle could eventually come from the database...
  // double startAngle = gmt_mgr->PixelECStartPhi();

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = gmt_mgr->PixelDiskRMin() + 0.5*gmt_mgr->PixelBoardActiveLength();

  // for compatibility with older geometries we have to incorrectly rotate modules in endcap C
  bool oldGeometry = (gmt_mgr->dbVersion() < 1);

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
  gmt_mgr->SetEta(0);
  DDmgr->numerology().setNumRingsForDisk(gmt_mgr->GetLD(),1);
  DDmgr->numerology().setNumPhiModulesForDiskRing(gmt_mgr->GetLD(),0,gmt_mgr->PixelECNSectors1()*2);

  // Even modules (front of disk, delta z < 0)
  // and Odd modules (back of disk, delta z > 0)
  int pixelECNSectors1 = gmt_mgr->PixelECNSectors1();
  for (int ii = 0; ii <  pixelECNSectors1*2; ii++) {

    // Build both endcaps the same but re-number phiId in endcap C to get correct offline numbering.
    // Endcap C is obtained by rotating endcap A by 180 around y axis. Numbering goes in opposite direction
    // with module 0 becoming module 23. Mapping is 0<->23, 24<->47.

    // Add a test to get rid off division by zero coverity error
    int phiId = 0;
    if(pixelECNSectors1>0) phiId = (gmt_mgr->GetSide()>0) ? ii : (3*pixelECNSectors1-ii-1)%(pixelECNSectors1*2);

    gmt_mgr->SetPhi(phiId);

    double angle = ii*0.5*deltaPhi+startAngle;
    //if ( gmt_mgr->GetSide()<0 ) angle = 360*CLHEP::deg-(ii*deltaPhi+startAngle);
    int diskSide = (ii%2) ? +1 : -1; // even: -1, odd +1
    CLHEP::HepRotation rm;
    rm.rotateY(90*CLHEP::deg);
    if (oldGeometry && gmt_mgr->GetSide()<0) {
      if (diskSide > 0) rm.rotateX(180.*CLHEP::deg); // This is for compatibilty with older geomtries.
    } else {
      if (diskSide < 0) rm.rotateX(180.*CLHEP::deg); // depth axis points towards disk.
    } 
    rm.rotateZ(angle);
    CLHEP::Hep3Vector pos(moduleRadius,0.,diskSide*zpos);
    pos.rotateZ(angle);
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,pos));
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
    DDmgr->addAlignableTransform(0,id,xform,modulePhys);
  }

  //
  // Place the supports for the disks:
  //
  GeoPixelDiskSupports pds;
  for(int ii =0; ii< pds.NCylinders(); ii++) {
    pds.SetCylinder(ii);
    GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0, 0, pds.ZPos()) );
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
  }


  // Extra Material 
  InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  xMat.add(diskPhys,"PixelDisc");
  //Defined above as:
  // std::ostringstream ostr; ostr << gmt_mgr->GetLD();
  xMat.add(diskPhys,"PixelDisc"+ostr.str());
  if (gmt_mgr->GetSide()>0) { // EndcapA
    xMat.add(diskPhys,"PixelDiscA");
    xMat.add(diskPhys,"PixelDiscA"+ostr.str());
  } else {                   // EndcapC
    xMat.add(diskPhys,"PixelDiscC");
    xMat.add(diskPhys,"PixelDiscC"+ostr.str());
  }


  return diskPhys;
}

double GeoPixelDisk::RMin() {
  return gmt_mgr->PixelECCarbonRMin("Inner");
}

double GeoPixelDisk::RMax() {
  return gmt_mgr->PixelECCarbonRMax("Outer");
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
  //  return 10*CLHEP::mm;
  // GWG. It would be nice to get these numbers from the module itself to
  // ensure consistency.
  double safety = 0.01* CLHEP::mm; // This is the safety added to the module.
  double zClearance = 0.5 * CLHEP::mm; // Clearance for misalignments
  double tck = 2*(safety + 0.5*gmt_mgr->PixelBoardThickness()
                  + std::max(gmt_mgr->PixelHybridThickness(),
			     gmt_mgr->PixelChipThickness()+gmt_mgr->PixelChipGap())
		  );
  tck += std::max(gmt_mgr->PixelECSiDz1(),gmt_mgr->PixelECSiDz2());
  tck += 2.*zClearance;
  // TO DO: Add some safety (N.B. I think the module is slightly bigger because of some safety added there)
  return tck;
}


