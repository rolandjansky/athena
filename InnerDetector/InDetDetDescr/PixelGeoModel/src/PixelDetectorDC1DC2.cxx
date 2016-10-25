/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This file is basically a concatenation of all the *.cxx files.
// This allows one to keep a stable copy of the DC2 version and
// allow development of future versions.

#include "PixelGeoModel/PixelDetectorDC1DC2.h"

using namespace PixelGeoDC2;


//---------------------------------------------------//
//                                                   //
// GeoPixelBarrel                                    //
//                                                   //
//---------------------------------------------------//


//#include "PixelGeoModel/GeoPixelBarrel.h"
//#include "PixelGeoModel/GeoPixelLayer.h"
//#include "PixelGeoModel/GeoPixelServices.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

using namespace std;

GeoVPhysVol* GeoPixelBarrel::Build( ) {

  //
  // create the Barrel Mother volume
  // 
  double rmin = gmt_mgr->PixelBarrelRMin();
  double rmax = gmt_mgr->PixelBarrelRMax();
  double halflength = gmt_mgr->PixelBarrelHalfLength();
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* barrelTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* barrelLog = new GeoLogVol("barrelLog",barrelTube,air);
  GeoPhysVol* barrelPhys = new GeoPhysVol(barrelLog);

  // Set numerology
  DDmgr->numerology().setNumLayers(gmt_mgr->PixelBarrelNLayer());

  //
  // Build the layers inside
  //
  GeoPixelLayer layer;
  string lname[3];
  lname[0] = "InnerLayer";
  lname[1] = "CenterLayer";
  lname[2] = "OuterLayer";
  for(int ii = 0; ii < gmt_mgr->PixelBarrelNLayer(); ii++) {
    //cout << "Layer" << ii << endl;
    gmt_mgr->SetCurrentLD(ii);
    if(gmt_mgr->isLDPresent()) {
      GeoAlignableTransform * xform = new GeoAlignableTransform(HepGeom::Transform3D()); 
      GeoVPhysVol* layerphys = layer.Build();
      GeoNameTag *tag = new GeoNameTag(lname[ii]);         
      barrelPhys->add(tag);
      barrelPhys->add(new GeoIdentifierTag(ii));
      barrelPhys->add(xform);
      barrelPhys->add(layerphys);
      
      // Store the transform (at level 1)
      Identifier id = gmt_mgr->getIdHelper()->wafer_id(0,ii,0,0);
      DDmgr->addAlignableTransform(1, id, xform, layerphys);
    } else {
      gmt_mgr->msg(MSG::DEBUG) << "Layer " << ii << " not built" << endmsg;
    }
  }
  if(gmt_mgr->DoServices() ) {
    //
    // Add the services inside the barrel volume
    //
    GeoPixelServices brlsvc = GeoPixelServices("/Pixel/PixelBarrel");
    for(int ii =0; ii< brlsvc.NCylinders(); ii++) {
      brlsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Barrel Service");
      CLHEP::Hep3Vector pos(0.,0.,brlsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
      barrelPhys->add(tag);
      barrelPhys->add(xform);
      barrelPhys->add(brlsvc.Build() );
    }
  }
  return barrelPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelCable                                     //
//                                                   //
//---------------------------------------------------//



//#include "PixelGeoModel/GeoPixelCable.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include <sstream>

GeoVPhysVol* GeoPixelCable::Build() {
  //
  // Dimensions
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = gmt_mgr->PixelCableWidth();
  GeoMaterial* cable = mat_mgr->getMaterial("pix::Cable");
  const GeoBox* cableBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logName = "cableLog";
  std::ostringstream o;
  o << m_moduleNumber;
  logName = logName+o.str();

  GeoLogVol* theCable = new GeoLogVol(logName,cableBox,cable);
  GeoPhysVol* cablePhys = new GeoPhysVol(theCable);
  return cablePhys;
}


double GeoPixelCable::Length() {
  //
  // The cable is running from the beginning of the module to which
  // it is attached to the end of the ladder,
  // zmax gives the end of the ladder and zmin is the begining of the module
  // in absolute value, so the cable lenght is the difference b/w the two
  // the TubeCables factory will have to position this cable in the correct
  // side.
  //
  double zmax = gmt_mgr->PixelCableZMax();
  double zmin = gmt_mgr->PixelCableZMin()+m_moduleNumber*gmt_mgr->PixelCableDeltaZ();
  return zmax-zmin-m_epsilon;
}

double GeoPixelCable::Thickness() {
  //
  // This is calculated from the CLHEP::rad length of the cables and their mass
  // I have to go back on this later when I'll have defined a material
  // manager, for the time being I get the thickness by atlsim, using dtree
  // anf hardwire the numbers in the code...
  // I have to come back on the thickness using the cables recipes
  //
  if(m_moduleNumber == 0) return 2.*0.0028412*CLHEP::cm;
  if(m_moduleNumber == 1) return 2.*0.0056795*CLHEP::cm;
  if(m_moduleNumber == 2) return 2.*0.0056830*CLHEP::cm;
  if(m_moduleNumber == 3) return 2.*0.0056763*CLHEP::cm;
  if(m_moduleNumber == 4) return 2.*0.0056752*CLHEP::cm;
  if(m_moduleNumber == 5) return 2.*0.0057058*CLHEP::cm;
  if(m_moduleNumber == 6) return 2.*0.0056818*CLHEP::cm;

  return 0.;

}

//---------------------------------------------------//
//                                                   //
// GeoPixelChip                                      //
//                                                   //
//---------------------------------------------------//


//
// This class builds one Chip to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//
//#include "PixelGeoModel/GeoPixelChip.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelChip::Build() {
  //
  // Dimensions
  //
  double length = gmt_mgr->PixelChipLength();
  double thickness = gmt_mgr->PixelChipThickness();
  double width = gmt_mgr->PixelChipWidth();
  GeoMaterial* chipMat = mat_mgr->getMaterial("pix::Chip");
  std::string LogName = "chipBrlLog";
  //
  // In G3 is like this...
  //
  if(gmt_mgr->isEndcap() ) {
    chipMat =  mat_mgr->getMaterial("std::Silicon");
    LogName = "chipECLog";
  }

  //std::cout << "DC1 chip thick" << thickness << std::endl;
  const GeoBox* chipBox = new GeoBox(thickness/2.,width/2.,length/2.);
  GeoLogVol* theChip = new GeoLogVol(LogName,chipBox,chipMat);
  GeoPhysVol* chipPhys = new GeoPhysVol(theChip);
  return chipPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelDisk                                      //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelDisk.h"
//#include "PixelGeoModel/GeoPixelSubDisk.h"
//#include "PixelGeoModel/GeoPixelDiskSupports.h"
//#include "PixelGeoModel/GeoPixelSiCrystal.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"

GeoPixelDisk::GeoPixelDisk() {

  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Dimensions from class methods.
  //
  double rmin = RMin();
  double rmax = RMax();
  double halflength = Thickness()/2.;
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  theDisk = new GeoLogVol("diskLog",diskTube,air);
}
GeoVPhysVol* GeoPixelDisk::Build( ) {
  //
  // Define the Sensor to be used here, so it will be the same for all the disk
  GeoPixelSiCrystal theSensor(false);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(theDisk);
  //
  // Place the disk sectors (on both sides):
  //
  GeoPixelSubDisk psd(theSensor);
  double zpos = -gmt_mgr->PixelECSiDz1()/2.;
  double angle = 360.*CLHEP::deg/ (float) gmt_mgr->PixelECNSectors1();
  CLHEP::Hep3Vector pos(0.,0.,zpos);

  // Set numerology
  gmt_mgr->SetEta(0);
  DDmgr->numerology().setNumRingsForDisk(gmt_mgr->GetLD(),1);
  DDmgr->numerology().setNumPhiModulesForDiskRing(gmt_mgr->GetLD(),0,gmt_mgr->PixelECNSectors1()*2);
 
  // Even modules
  gmt_mgr->SetEta(0);
  for (int ii = 0; ii <  gmt_mgr->PixelECNSectors1(); ii++) {
    gmt_mgr->SetPhi(ii);
    CLHEP::HepRotation rm;
    rm.rotateX(180.*CLHEP::deg );
    rm.rotateZ(ii*angle+angle/2.);
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,pos));
    GeoVPhysVol * modulePhys = psd.Build();
    GeoNameTag* tag = new GeoNameTag("DiskSector");
    diskPhys->add(tag);
    diskPhys->add(new GeoIdentifierTag(getPhiId() ) );
    diskPhys->add(xform);
    diskPhys->add(modulePhys);

    // Now store the xform by identifier:
    Identifier id = theSensor.getID();
    DDmgr->addAlignableTransform(0,id,xform,modulePhys);
  }

  // Odd modules
  gmt_mgr->SetEta(1);
  zpos = gmt_mgr->PixelECSiDz2()/2.;
  pos = CLHEP::Hep3Vector(0.,0.,zpos);
  for (int ii = 0; ii <  gmt_mgr->PixelECNSectors1(); ii++) {
    gmt_mgr->SetPhi(ii);
    CLHEP::HepRotation rm;
    rm.rotateZ((ii+1)*angle);
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,pos));
    GeoVPhysVol * modulePhys = psd.Build();
    GeoNameTag* tag = new GeoNameTag("DiskSector");
    diskPhys->add(tag);
    diskPhys->add(new GeoIdentifierTag(getPhiId() ) );
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
    CLHEP::Hep3Vector pos(0.,0.,pds.ZPos() );
    GeoNameTag* tag = new GeoNameTag("DiskSupport");
    GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
    diskPhys->add(tag);
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
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
  double tck = 2*(gmt_mgr->PixelBoardThickness()
                  +std::max(gmt_mgr->PixelHybridThickness(),gmt_mgr->PixelChipThickness()));
  tck += std::max(gmt_mgr->PixelECSiDz1(),gmt_mgr->PixelECSiDz2());
  return tck;
}


//
// This is to retrofit the convention in the pixelId
// \which is coming from atlsim...
//
int GeoPixelDisk::getPhiId() {
  int brl_ec=0;
  int phimod;
    if(gmt_mgr->isBarrel() ) brl_ec = 0;
    if(gmt_mgr->isEndcap() ) brl_ec = 2*gmt_mgr->GetSide();
    if(brl_ec == 2) {
      phimod = gmt_mgr->Phi()*2 + gmt_mgr->Eta();
    } else {
      //
      // The (eta) negative) endcap is more complicated, as there is an extra
      // rotation of the endcap as a whole around Y
      //
      phimod = 48-gmt_mgr->Phi()*2-gmt_mgr->Eta()-2;
      if (phimod == -1) phimod = 47;
    }
    return phimod;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelDiskSupports                              //
//                                                   //
//---------------------------------------------------//


//#include "PixelGeoModel/GeoPixelDiskSupports.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include <sstream>

GeoPixelDiskSupports::GeoPixelDiskSupports() {
  //
  // Initialize the vectors
  //
  //
  // zpos is not needed, but it's here just in case...
  //
  std::string ele[3] = {"Inner", "Central", "Outer"};
  for (int ii =0; ii<3; ii++) {
    double rmin = gmt_mgr->PixelECCarbonRMin(ele[ii]);
    double rmax = gmt_mgr->PixelECCarbonRMax(ele[ii]);
    double halflength = gmt_mgr->PixelECCarbonThickness(ele[ii])/2.;
    double zpos = 0.;
    std::string mat = gmt_mgr->PixelECCarbonMaterial(ele[ii]);
    _rmin.push_back(rmin);
    _rmax.push_back(rmax);
    _zpos.push_back(zpos);
    _halflength.push_back(halflength);
    material.push_back(mat);
  }
}


 
GeoVPhysVol* GeoPixelDiskSupports::Build( ) {
  GeoMaterial* supportMat = mat_mgr->getMaterial(material[_nframe]);
  //
  // Dimensions: (_nframe set with teh SetCylinder method)
  //
  double rmin = _rmin[_nframe];
  double rmax = _rmax[_nframe];
  double halflength = _halflength[_nframe];
  const GeoTube* supportTube = new GeoTube(rmin,rmax,halflength);
  std::string logName = "diskSupLog";
  std::ostringstream o;
  o << _nframe;
  logName = logName+o.str();
  GeoLogVol* theSupport = new GeoLogVol(logName,supportTube,supportMat);
  GeoPhysVol* supportPhys = new GeoPhysVol(theSupport);
  return supportPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelECCable                                   //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelECCable.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoPixelECCable::GeoPixelECCable() {
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Dimensions 
  //
  double rmin = gmt_mgr->PixelECCablesRMin();
  double rmax = gmt_mgr->PixelECCablesRMax();
  double thickness = gmt_mgr->PixelECCablesThickness();
  GeoMaterial* cableMat = mat_mgr->getMaterial("pix::ECCables");
  const GeoTube* cableTube = new GeoTube(rmin,rmax,thickness/2.);
  theECCable = new GeoLogVol("ECCableLog",cableTube,cableMat);
}

GeoVPhysVol* GeoPixelECCable::Build( ) {
  GeoPhysVol* cablePhys = new GeoPhysVol(theECCable);
  return cablePhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelEndCap                                    //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelEndCap.h"
//#include "PixelGeoModel/GeoPixelDisk.h"
//#include "PixelGeoModel/GeoPixelECCable.h"
//#include "PixelGeoModel/GeoPixelServices.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"


GeoVPhysVol* GeoPixelEndCap::Build( ) {

  //
  // create the Barrel Mother volume
  // 
  double rmin = gmt_mgr->PixelEndcapRMin();
  double rmax = gmt_mgr->PixelEndcapRMax();
  double halflength = (gmt_mgr->PixelEndcapZMax()-gmt_mgr->PixelEndcapZMin())/2.;
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* ecTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* ecLog = new GeoLogVol("EndCapLog",ecTube,air);
  GeoPhysVol* ecPhys = new GeoPhysVol(ecLog);

  // Set numerology
  DDmgr->numerology().setNumDisks(gmt_mgr->PixelEndcapNDisk());

  //
  // Place the disks and cables on both sides
  //
  GeoPixelDisk pd;
  GeoPixelECCable pecc;
  for(int ii = 0; ii < gmt_mgr->PixelEndcapNDisk(); ii++) {
    gmt_mgr->SetCurrentLD(ii);
    if(gmt_mgr->isLDPresent()) {
      //
      // The position is given w.r.t to the center of the detector!
      //
      // position of the endcap
      double ecz = (gmt_mgr->PixelEndcapZMax()+gmt_mgr->PixelEndcapZMin())/2.;
      //position of the disk
      double zdisk = gmt_mgr->PixelDiskPosition()-ecz;
      // place the disk
      CLHEP::Hep3Vector pos(0.,0.,zdisk);
      GeoNameTag* tag = new GeoNameTag("Disk");
      GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(CLHEP::HepRotation(), pos));
      GeoVPhysVol * diskPhys = pd.Build();
      ecPhys->add(tag);
      ecPhys->add(new GeoIdentifierTag(ii));
      ecPhys->add(xform);
      ecPhys->add(diskPhys);

      // Store the alignable transform
      int brl_ec = 2*gmt_mgr->GetSide();
      Identifier id = gmt_mgr->getIdHelper()->wafer_id(brl_ec,ii,0,0);
      DDmgr->addAlignableTransform(1, id, xform, diskPhys);

      //
      // place the cables twice for the two active parts
      //
      double dz = pd.Thickness()/2. + gmt_mgr->PixelECCablesDistance() ;
      pos = CLHEP::Hep3Vector(0.,0.,zdisk+dz);
      tag = new GeoNameTag("ECCables");
      GeoTransform * xformCablesPlus = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), pos));
      ecPhys->add(tag);
      ecPhys->add(xformCablesPlus);
      ecPhys->add(pecc.Build() );
      pos = CLHEP::Hep3Vector(0.,0.,zdisk-dz);
      tag = new GeoNameTag("ECCables");
      GeoTransform * xformCablesMinus = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), pos));
      ecPhys->add(tag);
      ecPhys->add(xformCablesMinus);
      ecPhys->add(pecc.Build() );
    } else {
      if(gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) << "Disk " << ii << " not built" << endmsg;

    }
  }
  if(gmt_mgr->DoServices() ) {
    //
    // Add the Endcap services inide the EC volume
    //
    GeoPixelServices ecsvc = GeoPixelServices("/Pixel/PixelEndcap");
    for(int ii =0; ii< ecsvc.NCylinders(); ii++) {
      ecsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Endcap Service");
      CLHEP::Hep3Vector pos(0.,0.,ecsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
      ecPhys->add(tag);
      ecPhys->add(xform);
      ecPhys->add(ecsvc.Build() );
    } 
  }
 return ecPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelEnvelope                                  //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelEnvelope.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

//#include "PixelGeoModel/GeoPixelBarrel.h"
//#include "PixelGeoModel/GeoPixelEndCap.h"
//#include "PixelGeoModel/GeoPixelServices.h"
//#include "PixelGeoModel/GeoPixelLadder.h"
//#include "PixelGeoModel/GeoPixelModule.h"
//#include "PixelGeoModel/GeoPixelTubeCables.h"

GeoVPhysVol* GeoPixelEnvelope::Build( ) {
  //
  // create the External volume where I'll put all the pixel detector
  // 
  double rmin = gmt_mgr->PixelRMin();
  double rmax = gmt_mgr->PixelRMax();
  double halflength = gmt_mgr->PixelHalfLength();
  //std::cout << "Material Manager " << mat_mgr << std::endl;
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* envelopeTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeTube,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);
  //
  // Add the Barrel:
  //
  DDmgr->numerology().addBarrel(0);
  gmt_mgr->SetBarrel();
  GeoPixelBarrel brl;
  GeoNameTag* tag = new GeoNameTag("Barrel");
  GeoVPhysVol* barrelPhys =  brl.Build() ;
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(0));
  envelopePhys->add(barrelPhys );
  if(gmt_mgr->DoServices() ) {
    //
    // Add the Barrel services outside the barrel volume
    //
    GeoPixelServices brlsvc = GeoPixelServices("/Pixel");
    for(int ii =0; ii< brlsvc.NCylinders(); ii++) {
      brlsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Barrel Service");
      CLHEP::Hep3Vector pos(0.,0.,brlsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(ii) );
      envelopePhys->add(xform);
    envelopePhys->add(brlsvc.Build() );
    }
  }
  //
  // Add the EndCaps
  //
  DDmgr->numerology().addEndcap(2);
  DDmgr->numerology().addEndcap(-2);
  gmt_mgr->SetEndcap();
  gmt_mgr->SetPos();
  GeoPixelEndCap pec;
  double zpos = (gmt_mgr->PixelEndcapZMax()+gmt_mgr->PixelEndcapZMin())/2.;
  CLHEP::Hep3Vector pos(0.,0.,zpos);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
  tag  = new GeoNameTag("EndCap 1");
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(2));
  envelopePhys->add(xform);
  envelopePhys->add(pec.Build() );
  gmt_mgr->SetNeg();
  pos = CLHEP::Hep3Vector(0.,0.,-zpos);
  CLHEP::HepRotation rm;
  rm.rotateX(180.*CLHEP::deg);
  xform = new GeoTransform(HepGeom::Transform3D(rm,pos));
  tag  = new GeoNameTag("EndCap 2");
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(-2));
  envelopePhys->add(xform);
  envelopePhys->add(pec.Build() );
  if(gmt_mgr->DoServices() ) {
    //
    // Add the Endcap services outside the EC volume
    //
    GeoPixelServices ecsvc = GeoPixelServices("/Pixel");
    for(int ii =0; ii< ecsvc.NCylinders(); ii++) {
      ecsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Endcap Service");
      CLHEP::Hep3Vector pos(0.,0.,ecsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
      envelopePhys->add(tag);
      envelopePhys->add(xform);
      envelopePhys->add(new GeoIdentifierTag(ii) );
      envelopePhys->add(ecsvc.Build() );
    }
  }
  return envelopePhys;

  //
  // Want other stuff?
  //
  //  GeoPixelLadder brl;
  //  GeoPixelTubeCables brl;
  // GeoPixelModule brl;


}


//---------------------------------------------------//
//                                                   //
// GeoPixelHybrid                                    //
//                                                   //
//---------------------------------------------------//

//
// This class builds one Hybrid to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//
//#include "PixelGeoModel/GeoPixelHybrid.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

GeoVPhysVol* GeoPixelHybrid::Build() {
  //
  // Dimensions
  //
  double length = gmt_mgr->PixelHybridLength();
  double thickness = gmt_mgr->PixelHybridThickness();
  double width = gmt_mgr->PixelHybridWidth();
  GeoMaterial* hybridMat = mat_mgr->getMaterial("pix::Hybrid");
  if(gmt_mgr->isEndcap() )hybridMat =  mat_mgr->getMaterial("std::Silicon");
  const GeoBox* hybridBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logName = "hybridECLog";
  if(gmt_mgr->isBarrel() ) logName = "hybridBrlLog";
  GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
  GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);
  return hybridPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelLadder                                    //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelLadder.h"
//#include "PixelGeoModel/GeoPixelModule.h"
//#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"


using std::max;

GeoPixelLadder::GeoPixelLadder(GeoPixelSiCrystal& theSensor) :
  m_theSensor(theSensor)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Length of the ladder is in the db
  //
  double halflength = gmt_mgr->PixelLadderHalfLength();
  //
  // The width is the maximum among the componenst widths
  //
  double width = max( max(
			  gmt_mgr->PixelBoardWidth(),
			  gmt_mgr->PixelHybridWidth()),
		      gmt_mgr->PixelChipWidth());
  //
  // The thickness has to be calculated
  //
  double thickness = this->Thickness();
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoBox* ladderBox = new GeoBox(thickness/2.,width/2.,halflength);
  theLadder = new GeoLogVol("ladderLog",ladderBox,air);
}
GeoVPhysVol* GeoPixelLadder::Build( ) {
  GeoPhysVol* ladderPhys = new GeoPhysVol(theLadder);
  //
  // Place the Modules
  //
  GeoPixelModule pm(m_theSensor);
  int HalfNModule = gmt_mgr->PixelNModule()/2;
  for(int ii = 0; ii < gmt_mgr->PixelNModule(); ii++) {
    //
    // Which of the two sides is it?
    //
    float side = 1.;
    int jj = ii-HalfNModule;
    gmt_mgr->SetEta(jj);
    if(jj < 0) {
      side = -1.;
      jj = -jj;
    }
//
// Shift down the central module, as its flag (from NOVA) is -1.
//
    float xpos = gmt_mgr->PixelModuleDrDistance()*gmt_mgr->PixelModuleShiftFlag(jj)/2.;
//
// Get the z position from the db
//
    float zpos = gmt_mgr->PixelModulePosition(jj)*side;
    CLHEP::Hep3Vector modulepos(xpos,0.,zpos);
//
//
//
    CLHEP::HepRotation rm;
    //
    // again change sign w.r.t. g4
    //
    rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(ii) );
//
// Place the Module
//
    GeoVPhysVol* modulephys = pm.Build() ;
    GeoNameTag *tag = new GeoNameTag("Module");         
    GeoAlignableTransform* xform;
    // OLD EXAMPLE FOR ALIGNEMENT!!
    //    if(gmt_mgr->IsAlign() ) {
    //      xform = new GeoSiAlTransform(HepGeom::Transform3D(rm,modulepos), pm.getID() );
    //    } else {
    //      xform = new GeoTransform(HepGeom::Transform3D(rm,modulepos));
    //    }
    xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,modulepos));
    ladderPhys->add(tag);
    ladderPhys->add(new GeoIdentifierTag(gmt_mgr->Eta() ) );
    ladderPhys->add(xform);
    ladderPhys->add(modulephys );

    // Now store the xform by identifier:
    Identifier id = m_theSensor.getID();
    DDmgr->addAlignableTransform(0,id,xform,modulephys);
    
  }
  return ladderPhys;
}


double GeoPixelLadder::Thickness() {
  //
  // The thickness of the ladderis calculated starting from the thickness of
  // its components: Board + Hybrid+Chips
  // then the distance of the modules from the center is added and
  // a factor is added to account for the tilt of ~1degree given to
  // have a z overlap of the single modules
  //
  double tckincl = (gmt_mgr->PixelBoardLength()+gmt_mgr->PixelBalcony())*
    sin(gmt_mgr->PixelModuleAngle())/2.;
  double thick = gmt_mgr->PixelBoardThickness()+
    gmt_mgr->PixelHybridThickness()+
    gmt_mgr->PixelChipThickness()+
    fabs(gmt_mgr->PixelModuleDrDistance())+
    tckincl;
  return thick;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelLadderStruct                              //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelLadderStruct.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"



GeoVPhysVol* GeoPixelLadderStruct::Build( ) {
  //
  // create the structure to hold the ladder
  // 
  //
  // Dimensions
  //
  double halflength = gmt_mgr->PixelLadderHalfLength();
  double thickness = gmt_mgr->PixelLadderThickness();
  double width = gmt_mgr->PixelLadderWidth();
  //
  // This is ladder material
  //
  GeoMaterial* ladderMat = mat_mgr->getMaterial("pix::Ladder");
  const GeoBox* ladderStructBox = new GeoBox(thickness/2.,width/2.,halflength);
  const GeoLogVol* ladderStructLog = new GeoLogVol("ladderStructLog",ladderStructBox,ladderMat);
  GeoPhysVol* ladderStructPhys = new GeoPhysVol(ladderStructLog);
  return ladderStructPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelLayer                                     //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelLayer.h"
//#include "PixelGeoModel/GeoPixelLadder.h"
//#include "PixelGeoModel/GeoPixelTubeCables.h"
//#include "PixelGeoModel/GeoPixelSiCrystal.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

GeoVPhysVol* GeoPixelLayer::Build() {

  //
  // create the three barrel layers
  // Build the sensor first to use the same for all the module in the layer
  bool isBLayer = false;
  if(gmt_mgr->GetLD() == 0) isBLayer = true;
  GeoPixelSiCrystal theSensor(isBLayer);
  //
  // This is the maximum possible w/o going out of the mother volume!
  //
  double LayerThickness = 8.499*CLHEP::mm;
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  //
  // Layer dimensions from the geometry manager
  //
  double rmin =  gmt_mgr->PixelLayerRadius()-LayerThickness;
  double rmax =  gmt_mgr->PixelLayerRadius()+LayerThickness;
  double halflength = gmt_mgr->PixelLadderHalfLength() + m_epsilon;
  std::string lname[3];
  lname[0] = "InnerLayerLog";
  lname[1] = "CenterLayerLog";
  lname[2] = "OuterLayerLog";
  const GeoTube* layerTube = new GeoTube(rmin,rmax,halflength); //solid
  const GeoLogVol* layerLog = new GeoLogVol(lname[gmt_mgr->GetLD()],layerTube,air); //log volume
  GeoFullPhysVol* layerPhys = new GeoFullPhysVol(layerLog); // phys vol
  //
  // Place the ladders:
  //
  GeoPixelLadder pl(theSensor);
  GeoPixelTubeCables ptc;
  int nsectors = gmt_mgr->NPixelSectors();
  double angle=360./nsectors*CLHEP::deg;
  double layerradius = gmt_mgr->PixelLayerRadius();
  double xcblpos =  layerradius + (pl.Thickness()/2.+ptc.Thickness()/2)/cos(gmt_mgr->PixelLadderTilt());
  CLHEP::Hep3Vector posladder(layerradius, 0.,0.);
  CLHEP::Hep3Vector postubecables(xcblpos, 0.,0.);
  posladder.rotateZ(angle/2.);
  postubecables.rotateZ(angle/2.);
  
  // Set numerology
  DDmgr->numerology().setNumPhiModulesForLayer(gmt_mgr->GetLD(),nsectors);
  DDmgr->numerology().setNumEtaModulesForLayer(gmt_mgr->GetLD(),gmt_mgr->PixelNModule());

  //
  // Place the volumes by rotation.
  //
  for(int ii = 0; ii < nsectors; ii++) {
    gmt_mgr->SetPhi(ii);
    CLHEP::HepRotation rm;
    //
    // change the sign w.r.t G4 (same sign as G3)
    //
    rm.rotateZ(( +(float) ii+0.5)*angle+gmt_mgr->PixelLadderTilt() );
    //
    // Place the ladders
    //
    GeoNameTag *tag = new GeoNameTag("Ladder");         
    GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,posladder));
    layerPhys->add(tag);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xform);
    GeoVPhysVol* ladderphys = pl.Build() ;
    layerPhys->add(ladderphys );
    posladder.rotateZ(angle);     
    if(gmt_mgr->DoServices() ) {
      //
      // Place the box w/ the tubes and cables for the layer
      //
      tag = new GeoNameTag("TubesAndCables");         
      xform = new GeoTransform(HepGeom::Transform3D(rm,postubecables));
      layerPhys->add(tag);
      layerPhys->add(xform);
      GeoVPhysVol* TCphys = ptc.Build() ;
      layerPhys->add(TCphys );
      postubecables.rotateZ(angle);
    }
  }
  return layerPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelModule                                    //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelModule.h"
//#include "PixelGeoModel/GeoPixelHybrid.h"
//#include "PixelGeoModel/GeoPixelChip.h"
//#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

using std::max;

GeoPixelModule::GeoPixelModule(GeoPixelSiCrystal& theSensor) :
  m_theSensor(theSensor)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //
  //
  // The Dimensions are in separate routines
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = this->Width();
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoBox* moduleBox = new GeoBox(thickness/2.,width/2.,length/2.);
  theModule = new GeoLogVol("moduleLog",moduleBox,air);
}

GeoVPhysVol* GeoPixelModule::Build( ) {
  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(theModule);
  //
  // Place the Si Crystal
  //
  GeoVPhysVol *theSi = m_theSensor.Build();
  _id = m_theSensor.getID();
  GeoNameTag *tag = new GeoNameTag("Si Crystal");         
  GeoTransform *xformsi = new GeoTransform(HepGeom::Transform3D());
  modulePhys->add(tag);
  modulePhys->add(new GeoIdentifierTag(100) );
  modulePhys->add(xformsi);
  modulePhys->add(theSi );
  //
  // Place the Hybrid
  //
  GeoPixelHybrid ph;
  double xpos = -0.5*(gmt_mgr->PixelBoardThickness()+gmt_mgr->PixelHybridThickness());
  CLHEP::Hep3Vector hybpos(xpos,0.,0.);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),hybpos) );
  tag = new GeoNameTag("Hybrid");
  modulePhys->add(tag);
  modulePhys->add(xform);
  modulePhys->add(ph.Build() );
  //
  // Place the Chip
  //
  GeoPixelChip pc;
  xpos = 0.5*(gmt_mgr->PixelBoardThickness()+gmt_mgr->PixelChipThickness())+gmt_mgr->PixelChipGap();
  CLHEP::Hep3Vector chippos(xpos,0.,0.);
  xform = new GeoTransform(HepGeom::Transform3D (CLHEP::HepRotation(),chippos) );
  tag = new GeoNameTag("Chip");
  modulePhys->add(tag);
  modulePhys->add(xform);
  modulePhys->add(pc.Build() );
  //
  // Add the silicon element to the list of detector elements...
  //
  return modulePhys;
}


double GeoPixelModule::Thickness() {
  //
  // The thickness of the module is given by the board thickness
  // plus the maximum b/w the hybrid and the chip+gap
  //
  double thick = gmt_mgr->PixelBoardThickness()+
    2*max(gmt_mgr->PixelHybridThickness(),
        gmt_mgr->PixelChipThickness()+gmt_mgr->PixelChipGap());
  //
  // Don't understand why....
  //
  return thick+.01;
}

double GeoPixelModule::Width() {
  double width = max( max(
                     gmt_mgr->PixelBoardWidth(),
                     gmt_mgr->PixelHybridWidth()),
                     gmt_mgr->PixelChipWidth());
  return width;
}
double GeoPixelModule::Length() {
  double length = max( max( max(
                                gmt_mgr->PixelHybridLength(),
                                gmt_mgr->PixelBoardLength()),
                            2*gmt_mgr->PixelBalcony()),
                       gmt_mgr->PixelChipLength());

  return length;
}

Identifier GeoPixelModule::getID() {
  return _id;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelServices                                  //
//                                                   //
//---------------------------------------------------//

//
// This class builds the Services as Cylinders
//
// The services are grouped in 4 groups (as in G3)
// I keep this grouping as it natural in the database.
// They are Barrel-Inside tracking volume
//          Barrel-Outside tracking volume
//          Endcap-Inside tracking volume
//          Edncap-Outside tracking volume
//
// The elements for each group are defined in initialize,
// with Barrel/endcap coming from the geometry manager and inside oputside,
// from the mother volume name.
// This class builds the n-th  service element, with n defined in SetCylinder
//
//
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: passed by argument
//
// Contains: nothing
//

//#include "PixelGeoModel/GeoPixelServices.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include <sstream>

GeoPixelServices::GeoPixelServices(std::string root) {
  //
  // Initialize the service geometry properly accordning to the
  // mother volume where it goes.
  // Of course services cannot be placed in other volumes.
  if(root == "/Pixel") {
    initialize("Outside");
    m_zone = "Out";
  } else if (root == "/Pixel/PixelBarrel" || root == "/Pixel/PixelEndcap" ) {
    initialize("Inside");
    m_zone = "In";
  } else {
    std::cerr << "Services can  not be placed in this volume!!!! " << root << std::endl;
  }
  _nframe = 0;
}

void GeoPixelServices::initialize(std::string a)
{
  //
  // Here I inizialize the geometry information contained in the vectors
  //

  //
  // Loop over the service elements:
  //
  int NServices =  gmt_mgr->PixelServiceNFrame(a);
  for(int ii = 0; ii<NServices ; ii++) {
    //
    // If the element is attached to a layer/disk check if the LD is present.
    //
    int LD = gmt_mgr->PixelServiceLD(a, ii);
    //
    // C counts from zero. So the LDs are 0,1,2. -1 correspond to a
    // service which is not attached to any LD.
    //
    if(LD != -1) gmt_mgr->SetCurrentLD(LD);
    if(LD ==-1 || gmt_mgr->isLDPresent() == true) {
      double zpos,halflength;
//
// Retrieve/calculate the parameters for the volume.
//
// Here the atlsim logic has to be interpreted, as this is going into
// the structure of the PBOI bank that I'm using
//
      std::string mat = gmt_mgr->PixelServiceMaterial(a, ii);
      double* r = gmt_mgr->PixelServiceR(a, ii);
      double* z = gmt_mgr->PixelServiceZ(a, ii);
      //
      // If zin is 0... (within 10^-5) this is a volume symmetric around
      // the origin
      //
      if(fabs(z[0]) < 0.000001) {
        zpos = 0.;
        halflength = fabs(z[1])-m_epsilon;
      } else {
          halflength = fabs(z[1]-z[0])/2.-m_epsilon;
          zpos = (z[1]+z[0])/2.+m_epsilon/2.;
      }
      //
      // Push the object into the vectors: (twice if needed)
      //
      //
      //
      bool twovolumes;
      if(zpos > 0.00001 && r[0] >0.) {
        twovolumes = true;
      } else {
        twovolumes = false;
      }
      // only once for the inside endcaps as I already have two endcaps!
      if(gmt_mgr->isEndcap() && a == "Inside") twovolumes = false;
      _rmin.push_back(fabs(r[0]));
      _rmax.push_back(r[1]);
      _zpos.push_back(zpos);
      _halflength.push_back(halflength);
      material.push_back(mat);
      if(twovolumes) {
        _rmin.push_back(fabs(r[0]));
        _rmax.push_back(r[1]);
        _zpos.push_back(-zpos);
        _halflength.push_back(halflength);
        material.push_back(mat);
      }
      delete[] r;
      delete[] z;
    }
  //
  // Delete r and z...
  //
  }
}

GeoVPhysVol* GeoPixelServices::Build( ) {
  GeoMaterial* serviceMat = mat_mgr->getMaterial(material[_nframe]);
  //
  // Dimensions: (_nframe set with teh SetCylinder method)
  //
  double rmin = _rmin[_nframe];
  double rmax = _rmax[_nframe];
  double halflength = _halflength[_nframe];
  const GeoTube* serviceTube = new GeoTube(rmin,rmax,halflength);
  std::string logName;
  if(gmt_mgr->isBarrel() ) logName = m_zone+"Brl";
  else logName = m_zone+"EC";
  std::ostringstream o;
  o << _nframe;
  logName = logName+o.str();
  GeoLogVol* theService = new GeoLogVol(logName,serviceTube,serviceMat);
  GeoPhysVol* servicePhys = new GeoPhysVol(theService);
  return servicePhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelSiCrystal                                 //
//                                                   //
//---------------------------------------------------//

//
// This class builds one Si Crystal to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//

//#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SiCommonItems.h"

#include <vector>

using namespace InDetDD;

GeoPixelSiCrystal::GeoPixelSiCrystal(bool isBLayer) {
  // 
  //Builds the design for this crystal
  m_isBLayer = isBLayer;
  //SiDetectorDesign::Axis etaAxis   = SiDetectorDesign::zAxis;
  //SiDetectorDesign::Axis phiAxis   = SiDetectorDesign::yAxis;
  //SiDetectorDesign::Axis depthAxis = SiDetectorDesign::xAxis;
  double thickness = gmt_mgr->PixelBoardThickness();
  int CircPerCol = gmt_mgr->DesignCircuitsPerColumn();
  int CircPerRow = gmt_mgr->DesignCircuitsPerRow();
  int CellRowPerCirc = gmt_mgr->DesignCellRowsPerCircuit(isBLayer);
  int CellColPerCirc = gmt_mgr->DesignCellColumnsPerCircuit(isBLayer);
  int DiodeRowPerCirc = gmt_mgr->DesignDiodeRowsPerCircuit(isBLayer);
  int DiodeColPerCirc = gmt_mgr->DesignDiodeColumnsPerCircuit(isBLayer);

  // Add the matrix in the same way as from AGDD... 
  //
  //double startRP = -gmt_mgr->DesignRPActiveArea()/2.;
  //double startZ =  -gmt_mgr->DesignZActiveArea()/2.;
  //double ColSize = gmt_mgr->DesignZActiveArea()/gmt_mgr->DesignCircuitsPerRow();
  double bigEtaPitch =  gmt_mgr->DesignPitchZ(isBLayer) + gmt_mgr->DesignGapZ()/2;
  double phiPitch = gmt_mgr->DesignPitchRP(isBLayer);
  double etaPitch = gmt_mgr->DesignPitchZ(isBLayer);

 
  PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
  PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, bigEtaPitch); 

  PixelDiodeMatrix * singleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							  bigCell, 
							  normalCell, 
							  DiodeColPerCirc-2,
							  bigCell);

  PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
						      0, singleChipRow, CircPerRow, 0);

  PixelDiodeMatrix * fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
						       0, singleRow, DiodeRowPerCirc, 0);

  PixelModuleDesign *p_barrelDesign2 = new PixelModuleDesign(thickness,
							     CircPerCol,
							     CircPerRow,
							     CellColPerCirc,
							     CellRowPerCirc,
							     DiodeColPerCirc,
							     DiodeRowPerCirc,
							     fullMatrix,
							     InDetDD::electrons);

  // Multiple connections (ganged pixels)
  if (gmt_mgr->NumberOfEmptyRows() > 0) {
    int minRow = gmt_mgr->EmptyRows(0);
    int maxRow = minRow;
 

    for (int iConnect = 0; iConnect < gmt_mgr->NumberOfEmptyRows(); iConnect++){
      minRow = std::min(minRow, gmt_mgr->EmptyRows(iConnect));
      minRow = std::min(minRow, gmt_mgr->EmptyRowConnections(iConnect));
      maxRow = std::max(maxRow, gmt_mgr->EmptyRows(iConnect));
      maxRow = std::max(minRow, gmt_mgr->EmptyRowConnections(iConnect));
    }
  
    std::vector <int> connections(maxRow-minRow+1);
    
    // We fill them all with a one to one correspondence first.
    for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
      connections[iRow] = iRow +  minRow;
    }
    
    // Now make the connections.
    for (int iConnect = 0; iConnect < gmt_mgr->NumberOfEmptyRows(); iConnect++){
      connections[gmt_mgr->EmptyRows(iConnect)-minRow] = gmt_mgr->EmptyRowConnections(iConnect);
    }
    
    //std::cout << "MinRow = " << minRow << std::endl;
    //std::cout << "MaxRow = " << maxRow << std::endl;
    //for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
    //  std::cout << iRow << " " << connections[iRow] << std::endl;
    //}

    p_barrelDesign2->addMultipleRowConnection(minRow, connections);

  } else {
    // No empty rows.
    std::cout << "GeoPixelSiCrystal: Warning - No ganged pixels" << std::endl;
  }
  

  m_design = p_barrelDesign2;

  DDmgr->addDesign(m_design);

}
GeoVPhysVol* GeoPixelSiCrystal::Build() {
  //
  // Dimensions
  //
  double length = gmt_mgr->PixelBoardLength();
  double thickness = gmt_mgr->PixelBoardThickness();
  double width = gmt_mgr->PixelBoardWidth();
  GeoMaterial* siMat = mat_mgr->getMaterial("std::Silicon");
  const GeoBox* siBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logname;
  if(m_isBLayer) logname = "siBLayLog";
  else logname = "siLog";
  GeoLogVol* theSi = new GeoLogVol(logname,siBox,siMat);
  GeoFullPhysVol* siPhys = new GeoFullPhysVol(theSi);
  //
  // Add this to the list of detector elements:
  //
  int brl_ec=0;
  //
  // Build the Identifier for the silicon:
  //
  if(gmt_mgr->isBarrel() ) brl_ec = 0;
  if(gmt_mgr->isEndcap() ) brl_ec = 2*gmt_mgr->GetSide();
  const PixelID *ppp = gmt_mgr->getIdHelper();
  Identifier idwafer;
  if(brl_ec == 0) {
    idwafer = ppp->wafer_id(brl_ec,gmt_mgr->GetLD(),gmt_mgr->Phi(),gmt_mgr->Eta());
  } else {
    int phimod;
    //
    // This is evaluating the phimod. It's a bit complicated as I have to 
    // assign the numbers corresponding to a counter-clockwise rotation
    // in the disk reference frame. For the (eta) positive endcap I just 
    // have to match the start position with the start of the rotation. 
    //
    if(brl_ec == 2) {
      phimod = gmt_mgr->Phi()*2 + gmt_mgr->Eta();
    } else {
      //
      // The (eta) negative) endcap is more complicated, as there is an extra
      // rotation of the endcap as a whole around Y
      //
      phimod = 48-gmt_mgr->Phi()*2-gmt_mgr->Eta()-2;
      if (phimod == -1) phimod = 47;
    }
      
    idwafer = ppp->wafer_id(brl_ec,gmt_mgr->GetLD(),phimod,0);
  }
  _id=idwafer;
  
  SiDetectorElement * element = new SiDetectorElement(idwafer, m_design, siPhys, gmt_mgr->commonItems());
  // Add conditions:  (do it here as eventually they will come from the cond DB)
  //SiliconConditions conditions(gmt_mgr->Temperature(m_isBLayer), gmt_mgr->Voltage(m_isBLayer), 0.);
  //element->setSiliconConditions(conditions);
  // Pass on G3ComptibleDigits option.
  // NO LONGER POSSIBLE. DC1 not supported anyway.
  // element->setG3CompatibleOrientation(gmt_mgr->G3CompatibleDigits());
  // add the element to the manager
  DDmgr->addDetectorElement(element);
  return siPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelSubDisk                                   //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelSubDisk.h"
//#include "PixelGeoModel/GeoPixelChip.h"
//#include "PixelGeoModel/GeoPixelHybrid.h"
//#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

GeoPixelSubDisk::GeoPixelSubDisk(GeoPixelSiCrystal & theSensor) :
m_theSensor(theSensor)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Dimensions from class methods.
  //
  double rmin = RMin(); 
  double rmax = RMax();
  double halflength = Thickness()/2.;
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTubs* SDTubs = new GeoTubs(rmin,rmax,halflength,-180.*CLHEP::deg/gmt_mgr->PixelECNSectors1()+0.000005,360.*CLHEP::deg/gmt_mgr->PixelECNSectors1()-0.00001);
  theSubDisk = new GeoLogVol("SubDiskLog",SDTubs,air);
}

GeoVPhysVol* GeoPixelSubDisk::Build( ) {
  GeoFullPhysVol* SDPhys = new GeoFullPhysVol(theSubDisk);
  //
  // Place the Si Crystal
  //
  double xpos = RMin()+gmt_mgr->PixelBoardLength()/2.;
  GeoNameTag* tag = new GeoNameTag("SiCrystal");
  CLHEP::HepRotation rm;
  rm.rotateY(90.*CLHEP::deg);
  rm.rotateX(180.*CLHEP::deg);
  CLHEP::Hep3Vector pos(xpos,0.,0.);
  GeoAlignableTransform* xformsi = new GeoAlignableTransform(HepGeom::Transform3D(rm,pos) );
  SDPhys->add(tag);
  SDPhys->add(new GeoIdentifierTag(200) );
  SDPhys->add(xformsi);
  SDPhys->add(m_theSensor.Build() );
  //
  // Place the Hybrid
  //
  GeoPixelHybrid ph;
  tag = new GeoNameTag("Hybrid");
  double zpos = 0.5*(gmt_mgr->PixelBoardThickness()+gmt_mgr->PixelHybridThickness())+m_epsilon/2.;
  pos = CLHEP::Hep3Vector(xpos,0.,zpos);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,pos) );
  SDPhys->add(tag);
  SDPhys->add(xform);
  SDPhys->add(ph.Build() );
//
// Place the Chip
//
  GeoPixelChip pc;
  tag = new GeoNameTag("Chip");
  zpos = -0.5*(gmt_mgr->PixelBoardThickness()+gmt_mgr->PixelChipThickness())-gmt_mgr->PixelChipGap();
  pos = CLHEP::Hep3Vector(xpos,0.,zpos);
  xform = new GeoTransform(HepGeom::Transform3D(rm,pos) );
  SDPhys->add(tag);
  SDPhys->add(xform);
  SDPhys->add(pc.Build() );
  return SDPhys;
}


double GeoPixelSubDisk::RMin() {
  return gmt_mgr->PixelDiskRMin()-0.5*(gmt_mgr->PixelBoardLength()-gmt_mgr->PixelBoardActiveLen());
}

double GeoPixelSubDisk::RMax() {
  return RMin()+sqrt(pow(gmt_mgr->PixelBoardLength(),2)+pow(gmt_mgr->PixelBoardWidth(),2));
}

double GeoPixelSubDisk::Thickness() {
  //
  // This is the sum of the Thickness of the board + twice the maximum
  // b/w the thickness of the hybrid and that of the electronics
  // twice for symmetric placement.
  //

  return gmt_mgr->PixelBoardThickness()+2*std::max(gmt_mgr->PixelHybridThickness(),gmt_mgr->PixelChipThickness())+2*m_epsilon;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelTubeCables                                //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoPixelTubeCables.h"
//#include "PixelGeoModel/GeoPixelLadderStruct.h"
//#include "PixelGeoModel/GeoPixelCable.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
// #include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

GeoPixelTubeCables::GeoPixelTubeCables() {
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Length is in the db
  //
  double halflength = gmt_mgr->PixelLadderHalfLength();
  //
  // The width is the maximum among the componenst widths
  //
  double width = std::max(gmt_mgr->PixelLadderWidth(),
                     gmt_mgr->PixelCableWidth());
  //
  // The thickness has to be calculated
  //
  double thickness = this->Thickness();
  GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoBox* solBox = new GeoBox(thickness/2.,width/2.,halflength+m_epsilon);
  theBox = new GeoLogVol("TubeCablesLog",solBox,air);
}


GeoVPhysVol* GeoPixelTubeCables::Build( ) {
  GeoPhysVol* TCPhys = new GeoPhysVol(theBox);
  //
  // Place the Ladder Structure. Don't understand why this is going in the
  // tube & cables section...
  //
  GeoPixelLadderStruct pls;
  GeoNameTag* tag = new GeoNameTag("LadderStructure");
  GeoVPhysVol* ladderstructPhys =  pls.Build() ;
  double xpos = 0.5*(-this->Thickness()+gmt_mgr->PixelLadderThickness());
  CLHEP::Hep3Vector pos(xpos,0.,0.);
  GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),pos));
  TCPhys->add(tag);
  TCPhys->add(xform);
  TCPhys->add(ladderstructPhys);
  //
  // Place the cables... ouch!
  //
  // I do it in a symmetric way, assuming that the routing of the
  // central module is done half on the right and half on the left.
  // thus I have 7 cables running on each side.
  //
  GeoPixelCable pc;
  double xcabshift = 0.;
  for(int ii = 0; ii <= gmt_mgr->PixelNModule()/2; ii++) {
    pc.SetModuleNumber(ii);
    double zcabpos = gmt_mgr->PixelCableZMax()-pc.Length()/2.+m_epsilon/2.;
    xcabshift += pc.Thickness()/2.;
    double xcabpos = - this->Thickness()/2. + gmt_mgr->PixelLadderThickness()+xcabshift;
    //
    // Place the cables on both sides.
    // piling them up. this is slightly different from what in G3
    // where the cables are not piled up, I don't see a good reason for
    // that. In G3 there is an empty space b/w the cable from the center
    // and the cable from the first module, as the shift is not calculated
    // in the same way.
    //
    xcabshift += pc.Thickness()/2.;
    CLHEP::Hep3Vector cablepos(xcabpos,0.,zcabpos);
    GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cablepos));
    GeoNameTag *tag = new GeoNameTag("Cable");         
    //
    // Left side
    //
    GeoVPhysVol *cablePhys1 =  pc.Build();
    TCPhys->add(tag);
    TCPhys->add(xform);
    //    TCPhys->add(new GeoIdentifierTag(ii) );
    TCPhys->add(cablePhys1);
    //
    // Right side
    //
    GeoVPhysVol *cablePhys2 =  pc.Build();
    cablepos = CLHEP::Hep3Vector(xcabpos,0.,-zcabpos);
    xform = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),cablepos));
    TCPhys->add(tag);
    TCPhys->add(xform);
    //    TCPhys->add(new GeoIdentifierTag(ii+100) );
    TCPhys->add(cablePhys2);
  }

  return TCPhys;
}


double GeoPixelTubeCables::Thickness() {
  //
  // The thickness of the box is calculated starting from the thickness of
  // its components: Ladder + cables
  //
  double thick = gmt_mgr->PixelLadderThickness()+
    gmt_mgr->PixelCableThickness();
  return thick;
}




//---------------------------------------------------//
//                                                   //
// GeoVPixelFactor                                   //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/GeoVPixelFactory.h"

using InDetDD::PixelDetectorManager;

PixelDetectorManager * GeoVPixelFactory::DDmgr = 0;

GeoVPixelFactory::GeoVPixelFactory() :   
  m_epsilon(0.0001)
 {
  gmt_mgr = PixelGeometryManager::GetPointer();
  mat_mgr = gmt_mgr->getMaterialManager();
}

GeoVPixelFactory::~GeoVPixelFactory()
{}


void GeoVPixelFactory::SetDDMgr(PixelDetectorManager* mgr) {
  DDmgr = mgr;
}


//---------------------------------------------------//
//                                                   //
// OraclePixGeoManager                               //
//                                                   //
//---------------------------------------------------//

#include "GeoModelKernel/GeoMaterial.h"
//#include "PixelGeoModel/OraclePixGeoManager.h"

// to permit access to StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

// Joe's Material Manager
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

//
// Get the pixelDD Manager from SG.
//
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
//
// 
//
#include "CLHEP/Units/PhysicalConstants.h"

using InDetDD::PixelDetectorManager; 


using namespace std;

OraclePixGeoManager::OraclePixGeoManager()
  : m_eta(0),
    m_phi(0),
    currentLD(0),
    BarrelEndcap(0),
    _side(0),
    m_services(true),  
    m_g3CompatibleDigits(false), 
    m_initialLayout(false), 
    m_dc1Geometry(false),
    m_alignable(true),
    m_magFieldFromNova(true),
    m_useMagFieldSvc(false),
    m_commonItems(0),
    m_pDDmgr(0),
    m_pMatMgr(0)
{


  // Get service locator from Bootstrap
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using ORACLE PIXEL GEOMETRY MANAGER" << endmsg;

  // Get the DetStore 
  StatusCode sc = svcLocator->service("DetectorStore", m_pDetStore );
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate DetectorStore" << endmsg;  

 
  IRDBAccessSvc *rdbSvc;
  sc = svcLocator->service("RDBAccessSvc",rdbSvc);
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;


  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endmsg;

  // Get version tag and node for Pixel.
  DecodeVersionKey versionKey(geoModel, "Pixel");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();

  m_versionTag = rdbSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node(), false);


/////////////////////////////////////////////////////////
//
// Gets the structures from the det store
//
/////////////////////////////////////////////////////////

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;

  atls = rdbSvc->getRecordsetPtr("AtlasMother",geoModel->atlasVersion(), "ATLAS");
  PixelBarrelGeneral = rdbSvc->getRecordsetPtr("PixelBarrelGeneral",     detectorKey, detectorNode);
  PixelBarrelService = rdbSvc->getRecordsetPtr("PixelBarrelService",     detectorKey, detectorNode);
  PixelCommon        = rdbSvc->getRecordsetPtr("PixelCommon",            detectorKey, detectorNode);
  PixelDisk          = rdbSvc->getRecordsetPtr("PixelDisk",              detectorKey, detectorNode);
  PixelEndcapGeneral = rdbSvc->getRecordsetPtr("PixelEndcapGeneral",     detectorKey, detectorNode);
  PixelEndcapService = rdbSvc->getRecordsetPtr("PixelEndcapService",     detectorKey, detectorNode);
  PixelLayer         = rdbSvc->getRecordsetPtr("PixelLayer",             detectorKey, detectorNode);
  PixelModule        = rdbSvc->getRecordsetPtr("PixelModule",            detectorKey, detectorNode);
  PixelStave         = rdbSvc->getRecordsetPtr("PixelStave",             detectorKey, detectorNode);


  // These are for the design
  pxbi = rdbSvc->getRecordsetPtr("PXBI",     detectorKey, detectorNode);
  pxei = rdbSvc->getRecordsetPtr("PXEI",     detectorKey, detectorNode);
  pdch = rdbSvc->getRecordsetPtr("PDCH",     detectorKey, detectorNode);
  pxbd = rdbSvc->getRecordsetPtr("PXBD",     detectorKey, detectorNode);
  plor = rdbSvc->getRecordsetPtr("PLOR",     detectorKey, detectorNode);
  plrn = rdbSvc->getRecordsetPtr("PLRN",     detectorKey, detectorNode);
   
  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "... Record Sets retrieved." << endmsg;

  // cache the number of inner frames
  
  m_barrelInFrames =  (*PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
  m_endcapInFrames =  (*PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
 //
 // Get the mat manager from the store:
 //
 DataHandle<StoredMaterialManager> materialManager;
 if (StatusCode::SUCCESS == m_pDetStore->retrieve(materialManager, std::string("MATERIALS"))) {
   m_pMatMgr = materialManager.ptr();
 }
 //
 // Print the version number for the barrel and endcap geometry (from NOVA)
 //  
 //cout << "Instantiating Pixel Detector" << endl;
 //cout << "Barrel Version " << this->PixelBarrelMajorVersion() << "." << this->PixelBarrelMinorVersion() << endl;
 //cout << "Endcap Version " << this->PixelEndcapMajorVersion() << "." << this->PixelEndcapMinorVersion() << endl;
}

StoredMaterialManager* OraclePixGeoManager::getMaterialManager()
{
  return m_pMatMgr;
}


/////////////////////////////////////////////////////////
//
// Setting of Layer/Disk and Barrel/EndCap
//
/////////////////////////////////////////////////////////
void OraclePixGeoManager::SetCurrentLD(int i)
{
  if(isBarrel() ) {
    if(i <= PixelBarrelNLayer()) {
      currentLD=i;
    } else {
      cout << "Layer set out of bounds, defaulting it to 0" << endl;
      currentLD = 0;
      std::cout<<" Current layer set to "<<i<<std::endl;
    }
  }
  if(isEndcap() ) {
     if (i<= PixelEndcapNDisk() ) {
       currentLD=i;
     } else {
       cout << "Disk set out of bounds, defaulting it to 0" << endl;
       currentLD = 0;
       std::cout<<" Current disk set to "<<i<<std::endl;
     }
  } 
}



void OraclePixGeoManager::SetBarrel() {
  //cout << "Setting Barrel" << endl;
  BarrelEndcap = 0;
}
void OraclePixGeoManager::SetEndcap() {
  BarrelEndcap = 1;
  //cout << "Setting Endcap" << endl;
}
/////////////////////////////////////////////////////////
//
// Check if the current layer/disk has to be retrieved
// By default in MySQL all the three layers have to be retrieved
// If m_initialLayout is true we ignore NOVA for layer/disk 1.
// For initial layout layer=1 and disk=1 (2nd layer and disk) is missing.
//
/////////////////////////////////////////////////////////
bool OraclePixGeoManager::isLDPresent() {
  if(isBarrel()) {
    if (m_initialLayout && currentLD == 1) return false;
    std::ostringstream A;
    A << "_" << currentLD;
    if((*PixelBarrelGeneral)[0]->getInt("USELAYER"+A.str()) == 1) {
      return true;
    } else {
      return false;
    } 
  }
  if(isEndcap() ) {
    if (m_initialLayout && currentLD == 1) return false;
    std::ostringstream A;
    A << "_" << currentLD;
    if((*PixelEndcapGeneral)[0]->getInt("USEDISK"+A.str()) == 1) {
      return true;
    } else {
      return false;
    }
  }    
  return false;
}


bool OraclePixGeoManager::isBarrel() {
  if(BarrelEndcap == 0) {
    return true;
  } else {
    return false;
  }
}
bool OraclePixGeoManager::isEndcap() {
  if(BarrelEndcap == 1) {
    return true;
  } else {
    return false;
  }
  return false;
}
/////////////////////////////////////////////////////////
//
// Calculate Thickness. This is used for the materials
// which thickness is given in % of r.l.
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::CalculateThickness(double tck,string mat) {
  GeoMaterial* material =  m_pMatMgr->getMaterial(mat);
  double rl = material->getRadLength();
  material->ref();
  material->unref();
  return -1.*rl*tck/100.;
}
/////////////////////////////////////////////////////////
//
// Si Boards Parameters:
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelBoardWidth() 
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("BOARDWIDTH")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("BOARDWIDTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelBoardLength() 
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("BOARDLENGTH")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("BOARDLENGTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelBoardThickness() 
{
  if (m_dc1Geometry && isBarrel() && (currentLD == 0)) {
    return 200*CLHEP::micrometer;
  }
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("BOARDTHICK")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("BOARDTHICK")*CLHEP::cm;
  return 0.;
}
double  OraclePixGeoManager::PixelBoardActiveLen() 
{
  if(isEndcap()) return (*pxei)[currentLD]->getDouble("DRACTIVE")*CLHEP::cm;
  if(isBarrel()) return (*pxbi)[currentLD]->getDouble("DZELEB")*CLHEP::cm;
  return 0.;
}
/////////////////////////////////////////////////////////
//
// Hybrid Parameters:
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelHybridWidth() 
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("HYBRIDWIDTH")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("HYBRIDWIDTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelHybridLength() 
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("HYBRIDLENGTH")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("HYBRIDLENGTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelHybridThickness() 
{
  double thick=0.;
  string mat;
  if(isBarrel()){
    // bad data name in db; here is worked around.  Should be DXHYBR
    thick =(*PixelModule)[currentLD]->getDouble("HYBRIDTHICK"); 
    mat = "pix::Hybrid";
  }
  if(isEndcap()){
    thick =(*PixelModule)[currentLD+3]->getDouble("HYBRIDTHICK"); 
    mat = "std::Silicon";
  }
  // if it is negative is given in % of r.l.
  if(thick > 0.) { 
    return thick*CLHEP::cm;
  }
  return CalculateThickness(thick,mat);

} 
/////////////////////////////////////////////////////////
//
// Chip Parameters:
//
/////////////////////////////////////////////////////////

double OraclePixGeoManager::PixelChipWidth()
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("CHIPWIDTH")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("CHIPWIDTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipLength()
{
  if(isBarrel())return (*PixelModule)[currentLD]->getDouble("CHIPLENGTH")*CLHEP::cm;
  if(isEndcap())return (*PixelModule)[currentLD+3]->getDouble("CHIPLENGTH")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipGap()
{
  if(isBarrel()) return (*PixelModule)[currentLD]->getDouble("CHIPGAP")*CLHEP::cm;
  if(isEndcap()) return (*PixelModule)[currentLD+3]->getDouble("CHIPGAP")*CLHEP::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipThickness() {
  double thick=0.;
  string mat;
  if(isBarrel()){
    thick =(*PixelModule)[currentLD]->getDouble("CHIPTHICK"); 
    mat = "pix::Chip";
  }
  if(isEndcap()){
    thick =(*PixelModule)[currentLD+3]->getDouble("CHIPTHICK"); 
    mat = "std::Silicon";
  } 
  // if it is negative is given in % of r.l.
  if(thick > 0.) { 
    return thick*CLHEP::cm;
  } 
  return CalculateThickness(thick,mat);

}

/////////////////////////////////////////////////////////
//
// Pixel Disks carbon structure
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelECCarbonRMin(string a) {
  if(a == "Inner") {
    return (*PixelDisk)[currentLD]->getDouble("SUP1RMIN")*CLHEP::cm;
  } else if (a == "Central") {
    return (*PixelDisk)[currentLD]->getDouble("SUP2RMIN")*CLHEP::cm;
  }
  return (*PixelDisk)[currentLD]->getDouble("SUP3RMIN")*CLHEP::cm;
}

double OraclePixGeoManager::PixelECCarbonRMax(string a) {
  if(a == "Inner") {
    return (*PixelDisk)[currentLD]->getDouble("SUP1RMAX")*CLHEP::cm;
  } else if (a == "Central") {
    return (*PixelDisk)[currentLD]->getDouble("SUP2RMAX")*CLHEP::cm;
  } else {
    return (*PixelDisk)[currentLD]->getDouble("SUP3RMAX")*CLHEP::cm;
  }
}

double OraclePixGeoManager::PixelECCarbonThickness(string a) {
  double tck;
  int imat;
  string mat[4] = {"std::Berillia","std::Carbon","pix::ECSevices","pix::Disk"} ;
  if(a == "Inner") {
    tck = (*PixelDisk)[currentLD]->getDouble("SUP1THICK");
    imat =(*PixelDisk)[currentLD]->getInt("SUP1MAT")-1;
  } else if (a == "Central") {
    tck = (*PixelDisk)[currentLD]->getDouble("SUP2THICK");
    imat =(*PixelDisk)[currentLD]->getInt("SUP2MAT")-1;
  } else {
    tck = (*PixelDisk)[currentLD]->getDouble("SUP3THICK");
    imat =(*PixelDisk)[currentLD]->getInt("SUP3MAT")-1;
  }
  if(tck>0.) {
    return tck*CLHEP::cm;
  } 
  return CalculateThickness(tck,mat[imat]);
}
string OraclePixGeoManager::PixelECCarbonMaterial(string a) {
  string mat[4] = {"std::Berillia","std::Carbon","pix::ECSevices","pix::Disk"} ; 
  int imat;
  if(a == "Inner") {
    imat = (*PixelDisk)[currentLD]->getInt("SUP1MAT")-1;
  } else if (a == "Central") {
    imat = (*PixelDisk)[currentLD]->getInt("SUP2MAT")-1;    
  } else {
    imat = (*PixelDisk)[currentLD]->getInt("SUP3MAT")-1;
  }
  return mat[imat];
}


/////////////////////////////////////////////////////////
//
// Central Services (PBOI)
//
/////////////////////////////////////////////////////////
//
// This is complicated in the DB...
// If Rmin < 0 this means that the cylinder has to be placed only once
// So I return RMin with its sign, for further processing by the service
// methods.
// If RMax is <0 the thickness is given in % of r.l. which have to be 
// calculated by the method calculatethickness
//
// If Zmin and Zmax have DIFFERENT sign, then the thickness is given in 
// % of r.l....
//

int OraclePixGeoManager::PixelServiceNFrame(string a) {
  if(isBarrel() ) {
    if(a == "Inside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
    if(a == "Outside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEOUT");
  }
  if(isEndcap() ) {
    if(a == "Inside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
    if(a == "Outside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEOUT");   
  }
  return 0;
}

double*  OraclePixGeoManager::PixelServiceR(string a, int _n) {
  double rmin=0.;
  double rmax=0.;
  double *r = new double[2];
  if(isBarrel()) {
    if(a == "Inside") {
      rmin = (*PixelBarrelService)[_n]->getDouble("RIN");
      rmax = (*PixelBarrelService)[_n]->getDouble("ROUT");
    }
    else {
      rmin = (*PixelBarrelService)[_n+m_barrelInFrames]->getDouble("RIN");
      rmax = (*PixelBarrelService)[_n+m_barrelInFrames]->getDouble("ROUT");
    }
  }
  if(isEndcap()) {
    if(a == "Inside") {
      rmin = (*PixelEndcapService)[_n]->getDouble("RIN");
      rmax = (*PixelEndcapService)[_n]->getDouble("ROUT");
    }
    else {
      rmin = (*PixelEndcapService)[_n+m_endcapInFrames]->getDouble("RIN");
      rmax = (*PixelEndcapService)[_n+m_endcapInFrames]->getDouble("ROUT");
    }
  }
  // If this is negative this is the thickness of the cyl in % of r.l.
  r[0] = rmin*CLHEP::cm;
  if(rmax > 0) {
    r[1] = rmax*CLHEP::cm;
  } else {
    string material = PixelServiceMaterial(a,_n);
    r[1] = fabs(rmin*CLHEP::cm)+CalculateThickness(rmax,material);
  }
  return r;
}


double* OraclePixGeoManager::PixelServiceZ(string a,int _n) {
  double* z = new double[2];
  if(isBarrel()) {
    if(a == "Inside") {
      z[0] = (*PixelBarrelService)[_n]->getDouble("ZIN");
      z[1] = (*PixelBarrelService)[_n]->getDouble("ZOUT");
    }
    else {
      z[0] = (*PixelBarrelService)[_n+m_barrelInFrames]->getDouble("ZIN");
      z[1] = (*PixelBarrelService)[_n+m_barrelInFrames]->getDouble("ZOUT");
    }
  }
  if(isEndcap()) {
    if(a == "Inside") {
      z[0] = (*PixelEndcapService)[_n]->getDouble("ZIN");
      z[1] = (*PixelEndcapService)[_n]->getDouble("ZOUT");
    }
    else {
      z[0] = (*PixelEndcapService)[_n+m_endcapInFrames]->getDouble("ZIN");
      z[1] = (*PixelEndcapService)[_n+m_endcapInFrames]->getDouble("ZOUT");
    }
  }
  z[0] = z[0] *CLHEP::cm;
  if(z[0]*(z[1]) > -0.000000001) { // same sign and z[0] > 0.
    z[1] = z[1] *CLHEP::cm;
  } else {
    string material = PixelServiceMaterial(a,_n);
    z[1] = z[0] * (1 + CalculateThickness(z[1],material)/fabs(z[0]));
  }
  if(isEndcap() && a == "Inside" ) { // Translate to the ecnter of EndCap
    double center = ((*PixelEndcapGeneral)[0]->getDouble("ZMAX")+(*PixelEndcapGeneral)[0]->getDouble("ZMIN"))/2.*CLHEP::cm;
    z[0] = z[0]-center;
    z[1] = z[1]-center;
  }
  return z;
}


int OraclePixGeoManager::PixelServiceLD(string a,int _n) {
  if(isBarrel()) {
    if(a == "Inside") {return (*PixelBarrelService)[_n]->getInt("LAYERNUM")-1;}
    else {return (*PixelBarrelService)[_n+m_barrelInFrames]->getInt("LAYERNUM")-1;}
  }
  if(isEndcap()) {
    if(a == "Inside") {return (*PixelEndcapService)[_n]->getInt("LAYERNUM")-1;}
    else {return (*PixelEndcapService)[_n+m_endcapInFrames]->getInt("LAYERNUM")-1;}
  }
  return 0;
}

string OraclePixGeoManager::PixelServiceMaterial(string a,int _n) {
  int imat;
  if(isBarrel()) {
    string mat[11] = {
      "std::Berillia",
      "std::Carbon",
      "pix::Services",
      "pix::Titanium",
      "pix::MatPP11",
      "pix::MatPP12",
      "pix::MatPP13",
      "pix::MatPP14",
      "pix::MatPP15",
      "pix::MatPP16",
      "pix::MatPP17"};
    if(a == "Inside") {imat  = (*PixelBarrelService)[_n]->getInt("MATERIAL")-1;}
    else {imat  = (*PixelBarrelService)[_n+m_barrelInFrames]->getInt("MATERIAL")-1;}
    return mat[imat];
  }
  if(isEndcap()) {
    string mat[4] = {"std::Berillia","std::Carbon","pix::ECServices","pix::Disk"};
    if(a == "Inside") {imat  = (*PixelEndcapService)[_n]->getInt("MATERIAL")-1;}
    else {imat =(*PixelEndcapService)[_n+m_endcapInFrames]->getInt("MATERIAL")-1;}
    return mat[imat];
  }
  return "None";
}
bool OraclePixGeoManager::DoServices() {
  return m_services;
}

bool OraclePixGeoManager::G3CompatibleDigits() const {
  return m_g3CompatibleDigits;
}

bool OraclePixGeoManager::InitialLayout() const {
  return m_initialLayout;
}

bool OraclePixGeoManager::DC1Geometry() const {
  return m_dc1Geometry;
}

bool OraclePixGeoManager::Alignable() const {
  return m_alignable;
}


PixelDetectorManager* OraclePixGeoManager::GetPixelDDManager() {
  if(m_pDDmgr == NULL) {
  //
  // retrieve the pointer to the DD manager
  //
    m_pDetStore->retrieve(m_pDDmgr);  
    StatusCode sc = m_pDetStore->retrieve(m_pDDmgr);  
    if (sc.isFailure()) {
      std::cout << "Cannot retrieve PixelDetectorManager" << std::endl;
    } 
  }
  return m_pDDmgr;
}  


void  OraclePixGeoManager::SetDetElementName(std::string name) {
  m_elementsObjectName = name;
}

std::string  OraclePixGeoManager::GetDetElementName() {
  return m_elementsObjectName;
}

void  OraclePixGeoManager::SetDesignName(std::string name) {
  m_designsObjectName = name;
}

std::string  OraclePixGeoManager::GetDesignName() {
  return m_designsObjectName;
}

void  OraclePixGeoManager::SetBarrelModuleName(std::string name){
  m_barrelDesignName = name;
}

std::string  OraclePixGeoManager::GetBarrelModuleName(){
  return m_barrelDesignName;
}

void  OraclePixGeoManager::SetEndcapModuleName(std::string name) {
  m_endcapDesignName = name;
}

std::string  OraclePixGeoManager::GetEndcapModuleName() {
  return m_endcapDesignName;
}

void  OraclePixGeoManager::SetLayer0ModuleName(std::string name) {
  m_blayerDesignName = name;
}

std::string  OraclePixGeoManager::GetLayer0ModuleName() {
  return m_blayerDesignName;
}


double OraclePixGeoManager::PixelLadderThickness() 
{
  double tck = (*PixelStave)[0]->getDouble("SUPPORTTHICK");
  if( tck > 0.) {
    return tck*CLHEP::cm;
  } else {
    return CalculateThickness(tck,"pix::Ladder");
  }
}

double OraclePixGeoManager::PixelECCablesThickness() 
{
  double tck =  (*PixelDisk)[currentLD]->getDouble("CABLETHICK");
  if( tck > 0.) {
    return tck*CLHEP::cm;
  } else {
    return CalculateThickness(tck,"pix::ECCables");
  }
}

int OraclePixGeoManager::DesignCellRowsPerCircuit(bool isBLayer)
{
  double ActiveArea = DesignRPActiveArea();
  double GapRP = DesignGapRP();
  double pitchRP = DesignPitchRP(isBLayer);
  return static_cast<int>((ActiveArea - GapRP)/pitchRP + 0.5);
}

int OraclePixGeoManager::DesignCellColumnsPerCircuit(bool isBLayer)
{
  double ActiveArea = DesignZActiveArea();
  double GapZ = DesignGapZ();
  double pitchZ = DesignPitchZ(isBLayer); 
  int CPR = DesignCircuitsPerRow();
  return static_cast<int>((ActiveArea - GapZ*CPR)/(CPR*pitchZ) + 0.5);      
}

int OraclePixGeoManager::DesignDiodeRowsPerCircuit(bool isBLayer)
{
  double ActiveArea = DesignRPActiveArea();
  double pitchRP = DesignPitchRP(isBLayer);
  return static_cast<int>((ActiveArea)/pitchRP + 0.5);
}

int OraclePixGeoManager::DesignDiodeColumnsPerCircuit(bool isBLayer)
{
  double ActiveArea = DesignZActiveArea();
  double GapZ = DesignGapZ();
  double pitchZ = DesignPitchZ(isBLayer) ;
  int CPR = DesignCircuitsPerRow();
  return static_cast<int>((ActiveArea - GapZ*CPR)/(CPR*pitchZ) + 0.5);
}

  // Ganged Pixels
int OraclePixGeoManager::NumberOfEmptyRows()
{
  return static_cast<int>((*pdch)[0]->getInt("NYCONNEC"));
}

int OraclePixGeoManager::EmptyRows(int index)
{
  std::ostringstream A;
  A << "_" << index;
 return static_cast<int>((*pdch)[0]->getInt("JYEMPTY"+A.str())) - 1;
}  

int OraclePixGeoManager::EmptyRowConnections(int index)
{
  std::ostringstream A;
  A << "_" << index;
  return static_cast<int>((*pdch)[0]->getInt("JYCONNEC"+A.str())) - 1;
}

double OraclePixGeoManager::Voltage(bool isBLayer){
  // override B-layer voltage for DC1 geometry by 
  // value in old DB (approx ratio of thicknesses (200/250 = 0.8)
  // 97.1*0.8 = 77.68. In Nova its 77.7.
  if (isBLayer && m_dc1Geometry) return 77.7*CLHEP::volt; 
  if(isBLayer) { return (*plor)[0]->getDouble("VOLTAGE")*CLHEP::volt;}
  return (*plor)[1]->getDouble("VOLTAGE")*CLHEP::volt;
}

double OraclePixGeoManager::Temperature(bool isBLayer){
  if(isBLayer) { return (*plor)[0]->getDouble("TEMPC")*CLHEP::kelvin+CLHEP::STP_Temperature;}
  return (*plor)[1]->getDouble("TEMPC")*CLHEP::kelvin+CLHEP::STP_Temperature;
}

const HepGeom::Vector3D<double> & 
OraclePixGeoManager::magneticField(bool isBLayer) const
{
  if (m_magFieldFromNova) {
    if(isBLayer) { 
      m_magField = HepGeom::Vector3D<double>(0, 0, (*plrn)[0]->getDouble("BFIELD") * CLHEP::tesla);
    } else {
      m_magField = HepGeom::Vector3D<double>(0, 0, (*plrn)[1]->getDouble("BFIELD") * CLHEP::tesla);
    }
  }
  return m_magField;
}

void 
OraclePixGeoManager::setMagneticField(const HepGeom::Vector3D<double> & field)
{
  m_magField = field;
  m_magFieldFromNova = false;
  m_useMagFieldSvc = false;
}

bool
OraclePixGeoManager::useMagneticFieldSvc() const
{
  return m_useMagFieldSvc;
}

void 
OraclePixGeoManager::setUseMagneticFieldSvc(bool flag)
{
  m_useMagFieldSvc = flag;
}


InDetDD::SiCommonItems * 
OraclePixGeoManager::commonItems() const
{
  return m_commonItems;
}


void 
OraclePixGeoManager::setCommonItems(InDetDD::SiCommonItems * commonItems)
{
  m_commonItems = commonItems;
}

const PixelID * 
OraclePixGeoManager::getIdHelper() 
{
  return dynamic_cast<const PixelID *>(m_commonItems->getIdHelper());
}


//---------------------------------------------------//
//                                                   //
// PixelGeometryManager                              //
//                                                   //
//---------------------------------------------------//

//#include "PixelGeoModel/PixelGeometryManager.h"
//#include "PixelGeoModel/NovaPixGeoManager.h"
//#include "PixelGeoModel/OraclePixGeoManager.h"
#include <stdexcept>
PixelGeometryManager* PixelGeometryManager::s_geometry_manager = 0;

using namespace std;

PixelGeometryManager::PixelGeometryManager()
{
}
PixelGeometryManager::~PixelGeometryManager()
{
}

void PixelGeometryManager::init() {
  s_geometry_manager = new OraclePixGeoManager();
}


PixelGeometryManager *PixelGeometryManager::GetPointer() {

  
  if (!s_geometry_manager) {
    init();
  }
  return s_geometry_manager;
}
