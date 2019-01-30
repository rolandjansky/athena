/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file is basically a concatenation of all the *.cxx files.
// This allows one to keep a stable copy of the DC2 version and
// allow development of future versions.

#include "PixelDetectorDC1DC2.h"

#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"


#include <vector>


using namespace PixelGeoDC2;


//---------------------------------------------------//
//                                                   //
// GeoPixelBarrel                                    //
//                                                   //
//---------------------------------------------------//


GeoVPhysVol* GeoPixelBarrel::Build( ) {

  //
  // create the Barrel Mother volume
  // 
  double rmin = m_gmt_mgr->PixelBarrelRMin();
  double rmax = m_gmt_mgr->PixelBarrelRMax();
  double halflength = m_gmt_mgr->PixelBarrelHalfLength();
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* barrelTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* barrelLog = new GeoLogVol("barrelLog",barrelTube,air);
  GeoPhysVol* barrelPhys = new GeoPhysVol(barrelLog);

  // Set numerology
  m_DDmgr->numerology().setNumLayers(m_gmt_mgr->PixelBarrelNLayer());

  //
  // Build the layers inside
  //
  GeoPixelLayer layer;
  std::string lname[3];
  lname[0] = "InnerLayer";
  lname[1] = "CenterLayer";
  lname[2] = "OuterLayer";
  for(int ii = 0; ii < m_gmt_mgr->PixelBarrelNLayer(); ii++) {
    //cout << "Layer" << ii << endl;
    m_gmt_mgr->SetCurrentLD(ii);
    if(m_gmt_mgr->isLDPresent()) {
      GeoAlignableTransform * xform = new GeoAlignableTransform(GeoTrf::Transform3D::Identity());
      GeoVPhysVol* layerphys = layer.Build();
      GeoNameTag *tag = new GeoNameTag(lname[ii]);         
      barrelPhys->add(tag);
      barrelPhys->add(new GeoIdentifierTag(ii));
      barrelPhys->add(xform);
      barrelPhys->add(layerphys);
      
      // Store the transform (at level 1)
      Identifier id = m_gmt_mgr->getIdHelper()->wafer_id(0,ii,0,0);
      m_DDmgr->addAlignableTransform(1, id, xform, layerphys);
    } else {
      m_gmt_mgr->msg(MSG::DEBUG) << "Layer " << ii << " not built" << endmsg;
    }
  }
  if(m_gmt_mgr->DoServices() ) {
    //
    // Add the services inside the barrel volume
    //
    GeoPixelServices brlsvc = GeoPixelServices("/Pixel/PixelBarrel");
    for(int ii =0; ii< brlsvc.NCylinders(); ii++) {
      brlsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Barrel Service");
      GeoTrf::Translate3D pos(0.,0.,brlsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(pos);
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



GeoVPhysVol* GeoPixelCable::Build() {
  //
  // Dimensions
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = m_gmt_mgr->PixelCableWidth();
  const GeoMaterial* cable = m_mat_mgr->getMaterial("pix::Cable");
  const GeoBox* cableBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logName = std::string("cableLog")+std::to_string(m_moduleNumber);;
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
  double zmax = m_gmt_mgr->PixelCableZMax();
  double zmin = m_gmt_mgr->PixelCableZMin()+m_moduleNumber*m_gmt_mgr->PixelCableDeltaZ();
  return zmax-zmin-m_epsilon;
}

double GeoPixelCable::Thickness() {
  //
  // This is calculated from the Gaudi::Units::rad length of the cables and their mass
  // I have to go back on this later when I'll have defined a material
  // manager, for the time being I get the thickness by atlsim, using dtree
  // anf hardwire the numbers in the code...
  // I have to come back on the thickness using the cables recipes
  //
  if(m_moduleNumber == 0) return 2.*0.0028412*Gaudi::Units::cm;
  if(m_moduleNumber == 1) return 2.*0.0056795*Gaudi::Units::cm;
  if(m_moduleNumber == 2) return 2.*0.0056830*Gaudi::Units::cm;
  if(m_moduleNumber == 3) return 2.*0.0056763*Gaudi::Units::cm;
  if(m_moduleNumber == 4) return 2.*0.0056752*Gaudi::Units::cm;
  if(m_moduleNumber == 5) return 2.*0.0057058*Gaudi::Units::cm;
  if(m_moduleNumber == 6) return 2.*0.0056818*Gaudi::Units::cm;

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

GeoVPhysVol* GeoPixelChip::Build() {
  //
  // Dimensions
  //
  double length = m_gmt_mgr->PixelChipLength();
  double thickness = m_gmt_mgr->PixelChipThickness();
  double width = m_gmt_mgr->PixelChipWidth();
  const GeoMaterial* chipMat = m_mat_mgr->getMaterial("pix::Chip");
  std::string LogName = "chipBrlLog";
  //
  // In G3 is like this...
  //
  if(m_gmt_mgr->isEndcap() ) {
    chipMat =  m_mat_mgr->getMaterial("std::Silicon");
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


GeoPixelDisk::GeoPixelDisk() {

  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Dimensions from class methods.
  //
  double rmin = RMin();
  double rmax = RMax();
  double halflength = Thickness()*0.5;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* diskTube = new GeoTube(rmin,rmax,halflength);
  m_theDisk = new GeoLogVol("diskLog",diskTube,air);
  m_theDisk->ref();
}
GeoPixelDisk::~GeoPixelDisk() {
  m_theDisk->unref();
}

GeoVPhysVol* GeoPixelDisk::Build( ) {
  //
  // Define the Sensor to be used here, so it will be the same for all the disk
  GeoPixelSiCrystal theSensor(false);
  GeoFullPhysVol* diskPhys = new GeoFullPhysVol(m_theDisk);
  //
  // Place the disk sectors (on both sides):
  //
  GeoPixelSubDisk psd(theSensor);
  double zpos = -m_gmt_mgr->PixelECSiDz1()/2.;
  double angle = 360.*Gaudi::Units::deg/ (float) m_gmt_mgr->PixelECNSectors1();
  GeoTrf::Translation3D pos(0.,0.,zpos);

  // Set numerology
  m_gmt_mgr->SetEta(0);
  m_DDmgr->numerology().setNumRingsForDisk(m_gmt_mgr->GetLD(),1);
  m_DDmgr->numerology().setNumPhiModulesForDiskRing(m_gmt_mgr->GetLD(),0,m_gmt_mgr->PixelECNSectors1()*2);
 
  // Even modules
  m_gmt_mgr->SetEta(0);
  for (int ii = 0; ii <  m_gmt_mgr->PixelECNSectors1(); ii++) {
    m_gmt_mgr->SetPhi(ii);
    GeoTrf::Transform3D rm = GeoTrf::RotateZ3D(ii*angle+angle/2.)*GeoTrf::RotateX3D(180.*Gaudi::Units::deg);
    GeoAlignableTransform* xform = new GeoAlignableTransform(GeoTrf::Transform3D(pos*rm));
    GeoVPhysVol * modulePhys = psd.Build();
    GeoNameTag* tag = new GeoNameTag("DiskSector");
    diskPhys->add(tag);
    diskPhys->add(new GeoIdentifierTag(getPhiId() ) );
    diskPhys->add(xform);
    diskPhys->add(modulePhys);

    // Now store the xform by identifier:
    Identifier id = theSensor.getID();
    m_DDmgr->addAlignableTransform(0,id,xform,modulePhys);
  }

  // Odd modules
  m_gmt_mgr->SetEta(1);
  zpos = m_gmt_mgr->PixelECSiDz2()/2.;
  pos = GeoTrf::Translation3D(0.,0.,zpos);
  for (int ii = 0; ii <  m_gmt_mgr->PixelECNSectors1(); ii++) {
    m_gmt_mgr->SetPhi(ii);
    GeoTrf::RotateZ3D rm((ii+1)*angle);
    GeoAlignableTransform* xform = new GeoAlignableTransform(GeoTrf::Transform3D(pos*rm));
    GeoVPhysVol * modulePhys = psd.Build();
    GeoNameTag* tag = new GeoNameTag("DiskSector");
    diskPhys->add(tag);
    diskPhys->add(new GeoIdentifierTag(getPhiId() ) );
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
    GeoTrf::Translate3D pos(0.,0.,pds.ZPos() );
    GeoNameTag* tag = new GeoNameTag("DiskSupport");
    GeoTransform* xform = new GeoTransform(pos);
    diskPhys->add(tag);
    diskPhys->add(xform);
    diskPhys->add(pds.Build() );
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
  double tck = 2*(m_gmt_mgr->PixelBoardThickness()
                  +std::max(m_gmt_mgr->PixelHybridThickness(),m_gmt_mgr->PixelChipThickness()));
  tck += std::max(m_gmt_mgr->PixelECSiDz1(),m_gmt_mgr->PixelECSiDz2());
  return tck;
}


//
// This is to retrofit the convention in the pixelId
// \which is coming from atlsim...
//
int GeoPixelDisk::getPhiId() {
  int brl_ec=0;
  int phimod;
    if(m_gmt_mgr->isBarrel() ) brl_ec = 0;
    if(m_gmt_mgr->isEndcap() ) brl_ec = 2*m_gmt_mgr->GetSide();
    if(brl_ec == 2) {
      phimod = m_gmt_mgr->Phi()*2 + m_gmt_mgr->Eta();
    } else {
      //
      // The (eta) negative) endcap is more complicated, as there is an extra
      // rotation of the endcap as a whole around Y
      //
      phimod = 48-m_gmt_mgr->Phi()*2-m_gmt_mgr->Eta()-2;
      if (phimod == -1) phimod = 47;
    }
    return phimod;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelDiskSupports                              //
//                                                   //
//---------------------------------------------------//


GeoPixelDiskSupports::GeoPixelDiskSupports() {
  //
  // Initialize the vectors
  //
  //
  // zpos is not needed, but it's here just in case...
  //
  std::string ele[3] = {"Inner", "Central", "Outer"};
  for (int ii =0; ii<3; ii++) {
    double rmin = m_gmt_mgr->PixelECCarbonRMin(ele[ii]);
    double rmax = m_gmt_mgr->PixelECCarbonRMax(ele[ii]);
    double halflength = m_gmt_mgr->PixelECCarbonThickness(ele[ii])/2.;
    double zpos = 0.;
    std::string mat = m_gmt_mgr->PixelECCarbonMaterial(ele[ii]);
    m_rmin.push_back(rmin);
    m_rmax.push_back(rmax);
    m_zpos.push_back(zpos);
    m_halflength.push_back(halflength);
    m_material.push_back(mat);
  }
}


 
GeoVPhysVol* GeoPixelDiskSupports::Build( ) {
  const GeoMaterial* supportMat = m_mat_mgr->getMaterial(m_material[m_nframe]);
  //
  // Dimensions: (m_nframe set with teh SetCylinder method)
  //
  double rmin = m_rmin[m_nframe];
  double rmax = m_rmax[m_nframe];
  double halflength = m_halflength[m_nframe];
  const GeoTube* supportTube = new GeoTube(rmin,rmax,halflength);
  std::string logName = std::string("diskSupLog")+std::to_string(m_nframe);
  GeoLogVol* theSupport = new GeoLogVol(logName,supportTube,supportMat);
  GeoPhysVol* supportPhys = new GeoPhysVol(theSupport);
  return supportPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelECCable                                   //
//                                                   //
//---------------------------------------------------//

GeoPixelECCable::GeoPixelECCable() {
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Dimensions 
  //
  double rmin = m_gmt_mgr->PixelECCablesRMin();
  double rmax = m_gmt_mgr->PixelECCablesRMax();
  double thickness = m_gmt_mgr->PixelECCablesThickness();
  const GeoMaterial* cableMat = m_mat_mgr->getMaterial("pix::ECCables");
  const GeoTube* cableTube = new GeoTube(rmin,rmax,thickness*0.5);
  m_theECCable = new GeoLogVol("ECCableLog",cableTube,cableMat);
  m_theECCable->ref();
}

GeoVPhysVol* GeoPixelECCable::Build( ) {
  GeoPhysVol* cablePhys = new GeoPhysVol(m_theECCable);
  return cablePhys;
}

GeoPixelECCable::~GeoPixelECCable(){
  m_theECCable->unref();
}

//---------------------------------------------------//
//                                                   //
// GeoPixelEndCap                                    //
//                                                   //
//---------------------------------------------------//


GeoVPhysVol* GeoPixelEndCap::Build( ) {

  //
  // create the Barrel Mother volume
  // 
  double rmin = m_gmt_mgr->PixelEndcapRMin();
  double rmax = m_gmt_mgr->PixelEndcapRMax();
  double halflength = (m_gmt_mgr->PixelEndcapZMax()-m_gmt_mgr->PixelEndcapZMin())/2.;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* ecTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* ecLog = new GeoLogVol("EndCapLog",ecTube,air);
  GeoPhysVol* ecPhys = new GeoPhysVol(ecLog);

  // Set numerology
  m_DDmgr->numerology().setNumDisks(m_gmt_mgr->PixelEndcapNDisk());

  //
  // Place the disks and cables on both sides
  //
  GeoPixelDisk pd;
  GeoPixelECCable pecc;
  for(int ii = 0; ii < m_gmt_mgr->PixelEndcapNDisk(); ii++) {
    m_gmt_mgr->SetCurrentLD(ii);
    if(m_gmt_mgr->isLDPresent()) {
      //
      // The position is given w.r.t to the center of the detector!
      //
      // position of the endcap
      double ecz = (m_gmt_mgr->PixelEndcapZMax()+m_gmt_mgr->PixelEndcapZMin())/2.;
      //position of the disk
      double zdisk = m_gmt_mgr->PixelDiskPosition()-ecz;
      // place the disk
      GeoTrf::Translate3D pos(0.,0.,zdisk);
      GeoNameTag* tag = new GeoNameTag("Disk");
      GeoAlignableTransform* xform = new GeoAlignableTransform(pos);
      GeoVPhysVol * diskPhys = pd.Build();
      ecPhys->add(tag);
      ecPhys->add(new GeoIdentifierTag(ii));
      ecPhys->add(xform);
      ecPhys->add(diskPhys);

      // Store the alignable transform
      int brl_ec = 2*m_gmt_mgr->GetSide();
      Identifier id = m_gmt_mgr->getIdHelper()->wafer_id(brl_ec,ii,0,0);
      m_DDmgr->addAlignableTransform(1, id, xform, diskPhys);

      //
      // place the cables twice for the two active parts
      //
      double dz = pd.Thickness()/2. + m_gmt_mgr->PixelECCablesDistance() ;
      pos = GeoTrf::Translate3D(0.,0.,zdisk+dz);
      tag = new GeoNameTag("ECCables");
      GeoTransform * xformCablesPlus = new GeoTransform(pos);
      ecPhys->add(tag);
      ecPhys->add(xformCablesPlus);
      ecPhys->add(pecc.Build() );
      pos = GeoTrf::Translate3D(0.,0.,zdisk-dz);
      tag = new GeoNameTag("ECCables");
      GeoTransform * xformCablesMinus = new GeoTransform(pos);
      ecPhys->add(tag);
      ecPhys->add(xformCablesMinus);
      ecPhys->add(pecc.Build() );
    } else {
      if(m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) << "Disk " << ii << " not built" << endmsg;

    }
  }
  if(m_gmt_mgr->DoServices() ) {
    //
    // Add the Endcap services inide the EC volume
    //
    GeoPixelServices ecsvc = GeoPixelServices("/Pixel/PixelEndcap");
    for(int ii =0; ii< ecsvc.NCylinders(); ii++) {
      ecsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Endcap Service");
      GeoTrf::Translate3D pos(0.,0.,ecsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(pos);
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


GeoVPhysVol* GeoPixelEnvelope::Build( ) {
  //
  // create the External volume where I'll put all the pixel detector
  // 
  double rmin = m_gmt_mgr->PixelRMin();
  double rmax = m_gmt_mgr->PixelRMax();
  double halflength = m_gmt_mgr->PixelHalfLength();
  //std::cout << "Material Manager " << m_mat_mgr << std::endl;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTube* envelopeTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeTube,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);
  //
  // Add the Barrel:
  //
  m_DDmgr->numerology().addBarrel(0);
  m_gmt_mgr->SetBarrel();
  GeoPixelBarrel brl;
  GeoNameTag* tag = new GeoNameTag("Barrel");
  GeoVPhysVol* barrelPhys =  brl.Build() ;
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(0));
  envelopePhys->add(barrelPhys );
  if(m_gmt_mgr->DoServices() ) {
    //
    // Add the Barrel services outside the barrel volume
    //
    GeoPixelServices brlsvc = GeoPixelServices("/Pixel");
    for(int ii =0; ii< brlsvc.NCylinders(); ii++) {
      brlsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Barrel Service");
      GeoTrf::Translate3D pos(0.,0.,brlsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(pos);
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(ii) );
      envelopePhys->add(xform);
    envelopePhys->add(brlsvc.Build() );
    }
  }
  //
  // Add the EndCaps
  //
  m_DDmgr->numerology().addEndcap(2);
  m_DDmgr->numerology().addEndcap(-2);
  m_gmt_mgr->SetEndcap();
  m_gmt_mgr->SetPos();
  GeoPixelEndCap pec;
  double zpos = (m_gmt_mgr->PixelEndcapZMax()+m_gmt_mgr->PixelEndcapZMin())/2.;
  GeoTrf::Translate3D pos(0.,0.,zpos);
  GeoTransform* xform = new GeoTransform(pos);
  tag  = new GeoNameTag("EndCap 1");
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(2));
  envelopePhys->add(xform);
  envelopePhys->add(pec.Build() );
  m_gmt_mgr->SetNeg();
  GeoTrf::RotateX3D rm(180.*Gaudi::Units::deg);
  xform = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translation3D(0.,0.,-zpos)*rm));
  tag  = new GeoNameTag("EndCap 2");
  envelopePhys->add(tag);
  envelopePhys->add(new GeoIdentifierTag(-2));
  envelopePhys->add(xform);
  envelopePhys->add(pec.Build() );
  if(m_gmt_mgr->DoServices() ) {
    //
    // Add the Endcap services outside the EC volume
    //
    GeoPixelServices ecsvc = GeoPixelServices("/Pixel");
    for(int ii =0; ii< ecsvc.NCylinders(); ii++) {
      ecsvc.SetCylinder(ii);
      GeoNameTag* tag = new GeoNameTag("Outside Endcap Service");
      GeoTrf::Translate3D pos(0.,0.,ecsvc.ZPos() );
      GeoTransform* xform = new GeoTransform(pos);
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

GeoVPhysVol* GeoPixelHybrid::Build() {
  //
  // Dimensions
  //
  double length = m_gmt_mgr->PixelHybridLength();
  double thickness = m_gmt_mgr->PixelHybridThickness();
  double width = m_gmt_mgr->PixelHybridWidth();
  const GeoMaterial* hybridMat = m_mat_mgr->getMaterial("pix::Hybrid");
  if(m_gmt_mgr->isEndcap() )hybridMat =  m_mat_mgr->getMaterial("std::Silicon");
  const GeoBox* hybridBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logName = "hybridECLog";
  if(m_gmt_mgr->isBarrel() ) logName = "hybridBrlLog";
  GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
  GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);
  return hybridPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelLadder                                    //
//                                                   //
//---------------------------------------------------//


GeoPixelLadder::GeoPixelLadder(GeoPixelSiCrystal& theSensor) :
  m_theSensor(theSensor)
{
  using std::max;

  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Length of the ladder is in the db
  //
  double halflength = m_gmt_mgr->PixelLadderHalfLength();
  //
  // The width is the maximum among the componenst widths
  //
  double width = max( max(
			  m_gmt_mgr->PixelBoardWidth(),
			  m_gmt_mgr->PixelHybridWidth()),
		      m_gmt_mgr->PixelChipWidth());
  //
  // The thickness has to be calculated
  //
  double thickness = this->Thickness();
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoBox* ladderBox = new GeoBox(thickness*0.5,width*0.5,halflength);
  m_theLadder = new GeoLogVol("ladderLog",ladderBox,air);
  m_theLadder->ref();
}
GeoPixelLadder::~GeoPixelLadder(){
  m_theLadder->unref();
}

GeoVPhysVol* GeoPixelLadder::Build( ) {
  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);
  //
  // Place the Modules
  //
  GeoPixelModule pm(m_theSensor);
  int HalfNModule = m_gmt_mgr->PixelNModule()/2;
  for(int ii = 0; ii < m_gmt_mgr->PixelNModule(); ii++) {
    //
    // Which of the two sides is it?
    //
    float side = 1.;
    int jj = ii-HalfNModule;
    m_gmt_mgr->SetEta(jj);
    if(jj < 0) {
      side = -1.;
      jj = -jj;
    }
//
// Shift down the central module, as its flag (from NOVA) is -1.
//
    float xpos = m_gmt_mgr->PixelModuleDrDistance()*m_gmt_mgr->PixelModuleShiftFlag(jj)/2.;
//
// Get the z position from the db
//
    float zpos = m_gmt_mgr->PixelModulePosition(jj)*side;
    GeoTrf::Translation3D modulepos(xpos,0.,zpos);
//
//
//
    //
    // again change sign w.r.t. g4
    //
    GeoTrf::RotateY3D rm(m_gmt_mgr->PixelModuleAngle()*m_gmt_mgr->PixelModuleAngleSign(ii) );
//
// Place the Module
//
    GeoVPhysVol* modulephys = pm.Build() ;
    GeoNameTag *tag = new GeoNameTag("Module");         
    GeoAlignableTransform* xform;
    // OLD EXAMPLE FOR ALIGNEMENT!!
    //    if(m_gmt_mgr->IsAlign() ) {
    //      xform = new GeoSiAlTransform(GeoTrf::Transform3D(rm,modulepos), pm.getID() );
    //    } else {
    //      xform = new GeoTransform(GeoTrf::Transform3D(rm,modulepos));
    //    }
    xform = new GeoAlignableTransform(GeoTrf::Transform3D(modulepos*rm));
    ladderPhys->add(tag);
    ladderPhys->add(new GeoIdentifierTag(m_gmt_mgr->Eta() ) );
    ladderPhys->add(xform);
    ladderPhys->add(modulephys );

    // Now store the xform by identifier:
    Identifier id = m_theSensor.getID();
    m_DDmgr->addAlignableTransform(0,id,xform,modulephys);
    
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
  double tckincl = (m_gmt_mgr->PixelBoardLength()+m_gmt_mgr->PixelBalcony())*
    sin(m_gmt_mgr->PixelModuleAngle())/2.;
  double thick = m_gmt_mgr->PixelBoardThickness()+
    m_gmt_mgr->PixelHybridThickness()+
    m_gmt_mgr->PixelChipThickness()+
    fabs(m_gmt_mgr->PixelModuleDrDistance())+
    tckincl;
  return thick;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelLadderStruct                              //
//                                                   //
//---------------------------------------------------//


GeoVPhysVol* GeoPixelLadderStruct::Build( ) {
  //
  // create the structure to hold the ladder
  // 
  //
  // Dimensions
  //
  double halflength = m_gmt_mgr->PixelLadderHalfLength();
  double thickness = m_gmt_mgr->PixelLadderThickness();
  double width = m_gmt_mgr->PixelLadderWidth();
  //
  // This is ladder material
  //
  const GeoMaterial* ladderMat = m_mat_mgr->getMaterial("pix::Ladder");
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


GeoVPhysVol* GeoPixelLayer::Build() {

  //
  // create the three barrel layers
  // Build the sensor first to use the same for all the module in the layer
  bool isBLayer = false;
  if(m_gmt_mgr->GetLD() == 0) isBLayer = true;
  GeoPixelSiCrystal theSensor(isBLayer);
  //
  // This is the maximum possible w/o going out of the mother volume!
  //
  double LayerThickness = 8.499*Gaudi::Units::mm;
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  //
  // Layer dimensions from the geometry manager
  //
  double rmin =  m_gmt_mgr->PixelLayerRadius()-LayerThickness;
  double rmax =  m_gmt_mgr->PixelLayerRadius()+LayerThickness;
  double halflength = m_gmt_mgr->PixelLadderHalfLength() + m_epsilon;
  std::string lname[3];
  lname[0] = "InnerLayerLog";
  lname[1] = "CenterLayerLog";
  lname[2] = "OuterLayerLog";
  const GeoTube* layerTube = new GeoTube(rmin,rmax,halflength); //solid
  const GeoLogVol* layerLog = new GeoLogVol(lname[m_gmt_mgr->GetLD()],layerTube,air); //log volume
  GeoFullPhysVol* layerPhys = new GeoFullPhysVol(layerLog); // phys vol
  //
  // Place the ladders:
  //
  GeoPixelLadder pl(theSensor);
  GeoPixelTubeCables ptc;
  int nsectors = m_gmt_mgr->NPixelSectors();
  double angle=360./nsectors*Gaudi::Units::deg;
  double layerradius = m_gmt_mgr->PixelLayerRadius();
  double xcblpos =  layerradius + (pl.Thickness()/2.+ptc.Thickness()/2)/cos(m_gmt_mgr->PixelLadderTilt());
  GeoTrf::Vector3D posladder(layerradius, 0.,0.);
  posladder = GeoTrf::RotateZ3D(angle/2.)*posladder;
  GeoTrf::Vector3D postubecables(xcblpos, 0.,0.);
  postubecables = GeoTrf::RotateZ3D(angle/2.)*postubecables;

  // Set numerology
  m_DDmgr->numerology().setNumPhiModulesForLayer(m_gmt_mgr->GetLD(),nsectors);
  m_DDmgr->numerology().setNumEtaModulesForLayer(m_gmt_mgr->GetLD(),m_gmt_mgr->PixelNModule());

  //
  // Place the volumes by rotation.
  //
  for(int ii = 0; ii < nsectors; ii++) {
    m_gmt_mgr->SetPhi(ii);
    //
    // change the sign w.r.t G4 (same sign as G3)
    //
    GeoTrf::RotateZ3D rm(( +(float) ii+0.5)*angle+m_gmt_mgr->PixelLadderTilt() );
    //
    // Place the ladders
    //
    GeoNameTag *tag = new GeoNameTag("Ladder");         
    GeoTransform* xform = new GeoTransform(GeoTrf::Translate3D(posladder.x(),posladder.y(),posladder.z())*rm);
    layerPhys->add(tag);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xform);
    GeoVPhysVol* ladderphys = pl.Build() ;
    layerPhys->add(ladderphys );
    posladder = GeoTrf::RotateZ3D(angle)*posladder;     
    if(m_gmt_mgr->DoServices() ) {
      //
      // Place the box w/ the tubes and cables for the layer
      //
      tag = new GeoNameTag("TubesAndCables");         
      xform = new GeoTransform(GeoTrf::Translate3D(postubecables.x(),postubecables.y(),postubecables.z())*rm);
      layerPhys->add(tag);
      layerPhys->add(xform);
      GeoVPhysVol* TCphys = ptc.Build() ;
      layerPhys->add(TCphys );
      postubecables = GeoTrf::RotateZ3D(angle)*postubecables;
    }
  }
  return layerPhys;
}


//---------------------------------------------------//
//                                                   //
// GeoPixelModule                                    //
//                                                   //
//---------------------------------------------------//


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
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoBox* moduleBox = new GeoBox(thickness*0.5,width*0.5,length*0.5);
  m_theModule = new GeoLogVol("moduleLog",moduleBox,air);
  m_theModule->ref();
}
GeoPixelModule::~GeoPixelModule(){
  m_theModule->unref();
}

GeoVPhysVol* GeoPixelModule::Build( ) {
  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(m_theModule);
  //
  // Place the Si Crystal
  //
  GeoVPhysVol *theSi = m_theSensor.Build();
  m_id = m_theSensor.getID();
  GeoNameTag *tag = new GeoNameTag("Si Crystal");         
  GeoTransform *xformsi = new GeoTransform(GeoTrf::Transform3D::Identity());
  modulePhys->add(tag);
  modulePhys->add(new GeoIdentifierTag(100) );
  modulePhys->add(xformsi);
  modulePhys->add(theSi );
  //
  // Place the Hybrid
  //
  GeoPixelHybrid ph;
  double xpos = -0.5*(m_gmt_mgr->PixelBoardThickness()+m_gmt_mgr->PixelHybridThickness());
  GeoTrf::Translate3D hybpos(xpos,0.,0.);
  GeoTransform* xform = new GeoTransform(hybpos);
  tag = new GeoNameTag("Hybrid");
  modulePhys->add(tag);
  modulePhys->add(xform);
  modulePhys->add(ph.Build() );
  //
  // Place the Chip
  //
  GeoPixelChip pc;
  xpos = 0.5*(m_gmt_mgr->PixelBoardThickness()+m_gmt_mgr->PixelChipThickness())+m_gmt_mgr->PixelChipGap();
  GeoTrf::Translate3D chippos(xpos,0.,0.);
  xform = new GeoTransform(chippos);
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
  using std::max;
  //
  // The thickness of the module is given by the board thickness
  // plus the maximum b/w the hybrid and the chip+gap
  //
  double thick = m_gmt_mgr->PixelBoardThickness()+
    2*max(m_gmt_mgr->PixelHybridThickness(),
        m_gmt_mgr->PixelChipThickness()+m_gmt_mgr->PixelChipGap());
  //
  // Don't understand why....
  //
  return thick+.01;
}

double GeoPixelModule::Width() {
  using std::max;
  double width = max( max(
                     m_gmt_mgr->PixelBoardWidth(),
                     m_gmt_mgr->PixelHybridWidth()),
                     m_gmt_mgr->PixelChipWidth());
  return width;
}
double GeoPixelModule::Length() {
  using std::max;
  double length = max( max( max(
                                m_gmt_mgr->PixelHybridLength(),
                                m_gmt_mgr->PixelBoardLength()),
                            2*m_gmt_mgr->PixelBalcony()),
                       m_gmt_mgr->PixelChipLength());

  return length;
}

Identifier GeoPixelModule::getID() {
  return m_id;
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
  m_nframe = 0;
}

void GeoPixelServices::initialize(std::string a)
{
  //
  // Here I inizialize the geometry information contained in the vectors
  //

  //
  // Loop over the service elements:
  //
  int NServices =  m_gmt_mgr->PixelServiceNFrame(a);
  for(int ii = 0; ii<NServices ; ii++) {
    //
    // If the element is attached to a layer/disk check if the LD is present.
    //
    int LD = m_gmt_mgr->PixelServiceLD(a, ii);
    //
    // C counts from zero. So the LDs are 0,1,2. -1 correspond to a
    // service which is not attached to any LD.
    //
    if(LD != -1) m_gmt_mgr->SetCurrentLD(LD);
    if(LD ==-1 || m_gmt_mgr->isLDPresent() == true) {
      double zpos,halflength;
//
// Retrieve/calculate the parameters for the volume.
//
// Here the atlsim logic has to be interpreted, as this is going into
// the structure of the PBOI bank that I'm using
//
      std::string mat = m_gmt_mgr->PixelServiceMaterial(a, ii);
      double* r = m_gmt_mgr->PixelServiceR(a, ii);
      double* z = m_gmt_mgr->PixelServiceZ(a, ii);
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
      if(m_gmt_mgr->isEndcap() && a == "Inside") twovolumes = false;
      m_rmin.push_back(fabs(r[0]));
      m_rmax.push_back(r[1]);
      m_zpos.push_back(zpos);
      m_halflength.push_back(halflength);
      m_material.push_back(mat);
      if(twovolumes) {
        m_rmin.push_back(fabs(r[0]));
        m_rmax.push_back(r[1]);
        m_zpos.push_back(-zpos);
        m_halflength.push_back(halflength);
        m_material.push_back(mat);
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
  const GeoMaterial* serviceMat = m_mat_mgr->getMaterial(m_material[m_nframe]);
  //
  // Dimensions: (m_nframe set with teh SetCylinder method)
  //
  double rmin = m_rmin[m_nframe];
  double rmax = m_rmax[m_nframe];
  double halflength = m_halflength[m_nframe];
  const GeoTube* serviceTube = new GeoTube(rmin,rmax,halflength);
  std::string logName;
  if(m_gmt_mgr->isBarrel() ) logName = m_zone+"Brl";
  else logName = m_zone+"EC";
  logName = logName+std::to_string(m_nframe);
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

using namespace InDetDD;

GeoPixelSiCrystal::GeoPixelSiCrystal(bool isBLayer) {
  // 
  //Builds the design for this crystal
  m_isBLayer = isBLayer;
  //SiDetectorDesign::Axis etaAxis   = SiDetectorDesign::zAxis;
  //SiDetectorDesign::Axis phiAxis   = SiDetectorDesign::yAxis;
  //SiDetectorDesign::Axis depthAxis = SiDetectorDesign::xAxis;
  double thickness = m_gmt_mgr->PixelBoardThickness();
  int CircPerCol = m_gmt_mgr->DesignCircuitsPerColumn();
  int CircPerRow = m_gmt_mgr->DesignCircuitsPerRow();
  int CellRowPerCirc = m_gmt_mgr->DesignCellRowsPerCircuit(isBLayer);
  int CellColPerCirc = m_gmt_mgr->DesignCellColumnsPerCircuit(isBLayer);
  int DiodeRowPerCirc = m_gmt_mgr->DesignDiodeRowsPerCircuit(isBLayer);
  int DiodeColPerCirc = m_gmt_mgr->DesignDiodeColumnsPerCircuit(isBLayer);

  // Add the matrix in the same way as from AGDD... 
  //
  //double startRP = -m_gmt_mgr->DesignRPActiveArea()/2.;
  //double startZ =  -m_gmt_mgr->DesignZActiveArea()/2.;
  //double ColSize = m_gmt_mgr->DesignZActiveArea()/m_gmt_mgr->DesignCircuitsPerRow();
  double bigEtaPitch =  m_gmt_mgr->DesignPitchZ(isBLayer) + m_gmt_mgr->DesignGapZ()/2;
  double phiPitch = m_gmt_mgr->DesignPitchRP(isBLayer);
  double etaPitch = m_gmt_mgr->DesignPitchZ(isBLayer);

 
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
  if (m_gmt_mgr->NumberOfEmptyRows() > 0) {
    int minRow = m_gmt_mgr->EmptyRows(0);
    int maxRow = minRow;
 

    for (int iConnect = 0; iConnect < m_gmt_mgr->NumberOfEmptyRows(); iConnect++){
      minRow = std::min(minRow, m_gmt_mgr->EmptyRows(iConnect));
      minRow = std::min(minRow, m_gmt_mgr->EmptyRowConnections(iConnect));
      maxRow = std::max(maxRow, m_gmt_mgr->EmptyRows(iConnect));
      maxRow = std::max(minRow, m_gmt_mgr->EmptyRowConnections(iConnect));
    }
  
    std::vector <int> connections(maxRow-minRow+1);
    
    // We fill them all with a one to one correspondence first.
    for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
      connections[iRow] = iRow +  minRow;
    }
    
    // Now make the connections.
    for (int iConnect = 0; iConnect < m_gmt_mgr->NumberOfEmptyRows(); iConnect++){
      connections[m_gmt_mgr->EmptyRows(iConnect)-minRow] = m_gmt_mgr->EmptyRowConnections(iConnect);
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

  m_DDmgr->addDesign(m_design);

}
GeoVPhysVol* GeoPixelSiCrystal::Build() {
  //
  // Dimensions
  //
  double length = m_gmt_mgr->PixelBoardLength();
  double thickness = m_gmt_mgr->PixelBoardThickness();
  double width = m_gmt_mgr->PixelBoardWidth();
  const GeoMaterial* siMat = m_mat_mgr->getMaterial("std::Silicon");
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
  if(m_gmt_mgr->isBarrel() ) brl_ec = 0;
  if(m_gmt_mgr->isEndcap() ) brl_ec = 2*m_gmt_mgr->GetSide();
  const PixelID *ppp = m_gmt_mgr->getIdHelper();
  Identifier idwafer;
  if(brl_ec == 0) {
    idwafer = ppp->wafer_id(brl_ec,m_gmt_mgr->GetLD(),m_gmt_mgr->Phi(),m_gmt_mgr->Eta());
  } else {
    int phimod;
    //
    // This is evaluating the phimod. It's a bit complicated as I have to 
    // assign the numbers corresponding to a counter-clockwise rotation
    // in the disk reference frame. For the (eta) positive endcap I just 
    // have to match the start position with the start of the rotation. 
    //
    if(brl_ec == 2) {
      phimod = m_gmt_mgr->Phi()*2 + m_gmt_mgr->Eta();
    } else {
      //
      // The (eta) negative) endcap is more complicated, as there is an extra
      // rotation of the endcap as a whole around Y
      //
      phimod = 48-m_gmt_mgr->Phi()*2-m_gmt_mgr->Eta()-2;
      if (phimod == -1) phimod = 47;
    }
      
    idwafer = ppp->wafer_id(brl_ec,m_gmt_mgr->GetLD(),phimod,0);
  }
  m_id=idwafer;
  
  SiDetectorElement * element = new SiDetectorElement(idwafer, m_design, siPhys, m_gmt_mgr->commonItems());
  // Add conditions:  (do it here as eventually they will come from the cond DB)
  //SiliconConditions conditions(m_gmt_mgr->Temperature(m_isBLayer), m_gmt_mgr->Voltage(m_isBLayer), 0.);
  //element->setSiliconConditions(conditions);
  // Pass on G3ComptibleDigits option.
  // NO LONGER POSSIBLE. DC1 not supported anyway.
  // element->setG3CompatibleOrientation(m_gmt_mgr->G3CompatibleDigits());
  // add the element to the manager
  m_DDmgr->addDetectorElement(element);
  return siPhys;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelSubDisk                                   //
//                                                   //
//---------------------------------------------------//


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
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoTubs* SDTubs = new GeoTubs(rmin,rmax,halflength,-180.*Gaudi::Units::deg/m_gmt_mgr->PixelECNSectors1()+0.000005,360.*Gaudi::Units::deg/m_gmt_mgr->PixelECNSectors1()-0.00001);
  m_theSubDisk = new GeoLogVol("SubDiskLog",SDTubs,air);
  m_theSubDisk->ref();
}
GeoPixelSubDisk::~GeoPixelSubDisk(){
  m_theSubDisk->unref();
}


GeoVPhysVol* GeoPixelSubDisk::Build( ) {
  GeoFullPhysVol* SDPhys = new GeoFullPhysVol(m_theSubDisk);
  //
  // Place the Si Crystal
  //
  double xpos = RMin()+m_gmt_mgr->PixelBoardLength()/2.;
  GeoNameTag* tag = new GeoNameTag("SiCrystal");
  GeoTrf::Transform3D rm = GeoTrf::RotateX3D(180.*Gaudi::Units::deg)*GeoTrf::RotateY3D(90.*Gaudi::Units::deg);
  GeoTrf::Translation3D pos(xpos,0.,0.);
  GeoAlignableTransform* xformsi = new GeoAlignableTransform(GeoTrf::Transform3D(pos*rm));
  SDPhys->add(tag);
  SDPhys->add(new GeoIdentifierTag(200) );
  SDPhys->add(xformsi);
  SDPhys->add(m_theSensor.Build() );
  //
  // Place the Hybrid
  //
  GeoPixelHybrid ph;
  tag = new GeoNameTag("Hybrid");
  double zpos = 0.5*(m_gmt_mgr->PixelBoardThickness()+m_gmt_mgr->PixelHybridThickness())+m_epsilon/2.;
  pos = GeoTrf::Translation3D(xpos,0.,zpos);
  GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm) );
  SDPhys->add(tag);
  SDPhys->add(xform);
  SDPhys->add(ph.Build() );
//
// Place the Chip
//
  GeoPixelChip pc;
  tag = new GeoNameTag("Chip");
  zpos = -0.5*(m_gmt_mgr->PixelBoardThickness()+m_gmt_mgr->PixelChipThickness())-m_gmt_mgr->PixelChipGap();
  pos = GeoTrf::Translation3D(xpos,0.,zpos);
  xform = new GeoTransform(GeoTrf::Transform3D(pos*rm) );
  SDPhys->add(tag);
  SDPhys->add(xform);
  SDPhys->add(pc.Build() );
  return SDPhys;
}


double GeoPixelSubDisk::RMin() {
  return m_gmt_mgr->PixelDiskRMin()-0.5*(m_gmt_mgr->PixelBoardLength()-m_gmt_mgr->PixelBoardActiveLen());
}

double GeoPixelSubDisk::RMax() {
  return RMin()+sqrt(pow(m_gmt_mgr->PixelBoardLength(),2)+pow(m_gmt_mgr->PixelBoardWidth(),2));
}

double GeoPixelSubDisk::Thickness() {
  //
  // This is the sum of the Thickness of the board + twice the maximum
  // b/w the thickness of the hybrid and that of the electronics
  // twice for symmetric placement.
  //

  return m_gmt_mgr->PixelBoardThickness()+2*std::max(m_gmt_mgr->PixelHybridThickness(),m_gmt_mgr->PixelChipThickness())+2*m_epsilon;
}

//---------------------------------------------------//
//                                                   //
// GeoPixelTubeCables                                //
//                                                   //
//---------------------------------------------------//


GeoPixelTubeCables::GeoPixelTubeCables() {
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Length is in the db
  //
  double halflength = m_gmt_mgr->PixelLadderHalfLength();
  //
  // The width is the maximum among the componenst widths
  //
  double width = std::max(m_gmt_mgr->PixelLadderWidth(),
                     m_gmt_mgr->PixelCableWidth());
  //
  // The thickness has to be calculated
  //
  double thickness = this->Thickness();
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoBox* solBox = new GeoBox(thickness*0.5,width*0.5,halflength+m_epsilon);
  m_theBox = new GeoLogVol("TubeCablesLog",solBox,air);
  m_theBox->ref();
}
GeoPixelTubeCables::~GeoPixelTubeCables(){
  m_theBox->unref();
}


GeoVPhysVol* GeoPixelTubeCables::Build( ) {
  GeoPhysVol* TCPhys = new GeoPhysVol(m_theBox);
  //
  // Place the Ladder Structure. Don't understand why this is going in the
  // tube & cables section...
  //
  GeoPixelLadderStruct pls;
  GeoNameTag* tag = new GeoNameTag("LadderStructure");
  GeoVPhysVol* ladderstructPhys =  pls.Build() ;
  double xpos = 0.5*(-this->Thickness()+m_gmt_mgr->PixelLadderThickness());
  GeoTrf::Translate3D pos(xpos,0.,0.);
  GeoTransform* xform = new GeoTransform(pos);
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
  for(int ii = 0; ii <= m_gmt_mgr->PixelNModule()/2; ii++) {
    pc.SetModuleNumber(ii);
    double zcabpos = m_gmt_mgr->PixelCableZMax()-pc.Length()/2.+m_epsilon/2.;
    xcabshift += pc.Thickness()/2.;
    double xcabpos = - this->Thickness()/2. + m_gmt_mgr->PixelLadderThickness()+xcabshift;
    //
    // Place the cables on both sides.
    // piling them up. this is slightly different from what in G3
    // where the cables are not piled up, I don't see a good reason for
    // that. In G3 there is an empty space b/w the cable from the center
    // and the cable from the first module, as the shift is not calculated
    // in the same way.
    //
    xcabshift += pc.Thickness()/2.;
    GeoTrf::Translate3D cablepos(xcabpos,0.,zcabpos);
    GeoTransform* xform = new GeoTransform(cablepos);
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
    cablepos = GeoTrf::Translate3D(xcabpos,0.,-zcabpos);
    xform = new GeoTransform(cablepos);
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
  double thick = m_gmt_mgr->PixelLadderThickness()+
    m_gmt_mgr->PixelCableThickness();
  return thick;
}




//---------------------------------------------------//
//                                                   //
// GeoVPixelFactor                                   //
//                                                   //
//---------------------------------------------------//

using InDetDD::PixelDetectorManager;

PixelDetectorManager * GeoVPixelFactory::m_DDmgr = 0;

GeoVPixelFactory::GeoVPixelFactory() :   
  m_epsilon(0.0001)
 {
  m_gmt_mgr = PixelGeometryManager::GetPointer();
  m_mat_mgr = m_gmt_mgr->getMaterialManager();
}

GeoVPixelFactory::~GeoVPixelFactory()
{}


void GeoVPixelFactory::SetDDMgr(PixelDetectorManager* mgr) {
  m_DDmgr = mgr;
}


//---------------------------------------------------//
//                                                   //
// OraclePixGeoManager                               //
//                                                   //
//---------------------------------------------------//


using InDetDD::PixelDetectorManager; 


using namespace std;

OraclePixGeoManager::OraclePixGeoManager()
  : m_eta(0),
    m_phi(0),
    m_currentLD(0),
    m_BarrelEndcap(0),
    m_side(0),
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


  // Get version tag and node for Pixel.
  DecodeVersionKey versionKey("Pixel");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();

  m_versionTag = rdbSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node());


/////////////////////////////////////////////////////////
//
// Gets the structures from the det store
//
/////////////////////////////////////////////////////////

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;

  DecodeVersionKey versionKeyAtlas("Pixel");
  m_atls = rdbSvc->getRecordsetPtr("AtlasMother",versionKeyAtlas.tag(), versionKeyAtlas.node());
  m_PixelBarrelGeneral = rdbSvc->getRecordsetPtr("PixelBarrelGeneral",     detectorKey, detectorNode);
  m_PixelBarrelService = rdbSvc->getRecordsetPtr("PixelBarrelService",     detectorKey, detectorNode);
  m_PixelCommon        = rdbSvc->getRecordsetPtr("PixelCommon",            detectorKey, detectorNode);
  m_PixelDisk          = rdbSvc->getRecordsetPtr("PixelDisk",              detectorKey, detectorNode);
  m_PixelEndcapGeneral = rdbSvc->getRecordsetPtr("PixelEndcapGeneral",     detectorKey, detectorNode);
  m_PixelEndcapService = rdbSvc->getRecordsetPtr("PixelEndcapService",     detectorKey, detectorNode);
  m_PixelLayer         = rdbSvc->getRecordsetPtr("PixelLayer",             detectorKey, detectorNode);
  m_PixelModule        = rdbSvc->getRecordsetPtr("PixelModule",            detectorKey, detectorNode);
  m_PixelStave         = rdbSvc->getRecordsetPtr("PixelStave",             detectorKey, detectorNode);


  // These are for the design
  m_pxbi = rdbSvc->getRecordsetPtr("PXBI",     detectorKey, detectorNode);
  m_pxei = rdbSvc->getRecordsetPtr("PXEI",     detectorKey, detectorNode);
  m_pdch = rdbSvc->getRecordsetPtr("PDCH",     detectorKey, detectorNode);
  m_pxbd = rdbSvc->getRecordsetPtr("PXBD",     detectorKey, detectorNode);
  m_plor = rdbSvc->getRecordsetPtr("PLOR",     detectorKey, detectorNode);
  m_plrn = rdbSvc->getRecordsetPtr("PLRN",     detectorKey, detectorNode);
   
  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "... Record Sets retrieved." << endmsg;

  // cache the number of inner frames
  
  m_barrelInFrames =  (*m_PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
  m_endcapInFrames =  (*m_PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
 //
 // Get the mat manager from the store:
 //
 const StoredMaterialManager* materialManager = nullptr;
 if (StatusCode::SUCCESS == m_pDetStore->retrieve(materialManager, std::string("MATERIALS"))) {
   m_pMatMgr = materialManager;
 }
 //
 // Print the version number for the barrel and endcap geometry (from NOVA)
 //  
 //cout << "Instantiating Pixel Detector" << endl;
 //cout << "Barrel Version " << this->PixelBarrelMajorVersion() << "." << this->PixelBarrelMinorVersion() << endl;
 //cout << "Endcap Version " << this->PixelEndcapMajorVersion() << "." << this->PixelEndcapMinorVersion() << endl;
}

const StoredMaterialManager* OraclePixGeoManager::getMaterialManager()
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
      m_currentLD=i;
    } else {
      cout << "Layer set out of bounds, defaulting it to 0" << endl;
      m_currentLD = 0;
      std::cout<<" Current layer set to "<<i<<std::endl;
    }
  }
  if(isEndcap() ) {
     if (i<= PixelEndcapNDisk() ) {
       m_currentLD=i;
     } else {
       cout << "Disk set out of bounds, defaulting it to 0" << endl;
       m_currentLD = 0;
       std::cout<<" Current disk set to "<<i<<std::endl;
     }
  } 
}



void OraclePixGeoManager::SetBarrel() {
  //cout << "Setting Barrel" << endl;
  m_BarrelEndcap = 0;
}
void OraclePixGeoManager::SetEndcap() {
  m_BarrelEndcap = 1;
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
  const std::string uscore("_");
  if(isBarrel()) {
    if (m_initialLayout && m_currentLD == 1) return false;
    std::string a=uscore+std::to_string(m_currentLD);
    if((*m_PixelBarrelGeneral)[0]->getInt("USELAYER"+a) == 1) {
      return true;
    } else {
      return false;
    } 
  }
  if(isEndcap() ) {
    if (m_initialLayout && m_currentLD == 1) return false;
    std::string a=uscore+std::to_string(m_currentLD);
    if((*m_PixelEndcapGeneral)[0]->getInt("USEDISK"+a) == 1) {
      return true;
    } else {
      return false;
    }
  }    
  return false;
}


bool OraclePixGeoManager::isBarrel() {
  if(m_BarrelEndcap == 0) {
    return true;
  } else {
    return false;
  }
}
bool OraclePixGeoManager::isEndcap() {
  if(m_BarrelEndcap == 1) {
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
  const GeoMaterial* material =  m_pMatMgr->getMaterial(mat);
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
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("BOARDWIDTH")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("BOARDWIDTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelBoardLength() 
{
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("BOARDLENGTH")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("BOARDLENGTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelBoardThickness() 
{
  if (m_dc1Geometry && isBarrel() && (m_currentLD == 0)) {
    return 200*Gaudi::Units::micrometer;
  }
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("BOARDTHICK")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("BOARDTHICK")*Gaudi::Units::cm;
  return 0.;
}
double  OraclePixGeoManager::PixelBoardActiveLen() 
{
  if(isEndcap()) return (*m_pxei)[m_currentLD]->getDouble("DRACTIVE")*Gaudi::Units::cm;
  if(isBarrel()) return (*m_pxbi)[m_currentLD]->getDouble("DZELEB")*Gaudi::Units::cm;
  return 0.;
}
/////////////////////////////////////////////////////////
//
// Hybrid Parameters:
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelHybridWidth() 
{
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("HYBRIDWIDTH")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("HYBRIDWIDTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelHybridLength() 
{
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("HYBRIDLENGTH")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("HYBRIDLENGTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelHybridThickness() 
{
  double thick=0.;
  string mat;
  if(isBarrel()){
    // bad data name in db; here is worked around.  Should be DXHYBR
    thick =(*m_PixelModule)[m_currentLD]->getDouble("HYBRIDTHICK"); 
    mat = "pix::Hybrid";
  }
  if(isEndcap()){
    thick =(*m_PixelModule)[m_currentLD+3]->getDouble("HYBRIDTHICK"); 
    mat = "std::Silicon";
  }
  // if it is negative is given in % of r.l.
  if(thick > 0.) { 
    return thick*Gaudi::Units::cm;
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
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("CHIPWIDTH")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("CHIPWIDTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipLength()
{
  if(isBarrel())return (*m_PixelModule)[m_currentLD]->getDouble("CHIPLENGTH")*Gaudi::Units::cm;
  if(isEndcap())return (*m_PixelModule)[m_currentLD+3]->getDouble("CHIPLENGTH")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipGap()
{
  if(isBarrel()) return (*m_PixelModule)[m_currentLD]->getDouble("CHIPGAP")*Gaudi::Units::cm;
  if(isEndcap()) return (*m_PixelModule)[m_currentLD+3]->getDouble("CHIPGAP")*Gaudi::Units::cm;
  return 0.;
}
double OraclePixGeoManager::PixelChipThickness() {
  double thick=0.;
  string mat;
  if(isBarrel()){
    thick =(*m_PixelModule)[m_currentLD]->getDouble("CHIPTHICK"); 
    mat = "pix::Chip";
  }
  if(isEndcap()){
    thick =(*m_PixelModule)[m_currentLD+3]->getDouble("CHIPTHICK"); 
    mat = "std::Silicon";
  } 
  // if it is negative is given in % of r.l.
  if(thick > 0.) { 
    return thick*Gaudi::Units::cm;
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
    return (*m_PixelDisk)[m_currentLD]->getDouble("SUP1RMIN")*Gaudi::Units::cm;
  } else if (a == "Central") {
    return (*m_PixelDisk)[m_currentLD]->getDouble("SUP2RMIN")*Gaudi::Units::cm;
  }
  return (*m_PixelDisk)[m_currentLD]->getDouble("SUP3RMIN")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelECCarbonRMax(string a) {
  if(a == "Inner") {
    return (*m_PixelDisk)[m_currentLD]->getDouble("SUP1RMAX")*Gaudi::Units::cm;
  } else if (a == "Central") {
    return (*m_PixelDisk)[m_currentLD]->getDouble("SUP2RMAX")*Gaudi::Units::cm;
  } else {
    return (*m_PixelDisk)[m_currentLD]->getDouble("SUP3RMAX")*Gaudi::Units::cm;
  }
}

double OraclePixGeoManager::PixelECCarbonThickness(string a) {
  double tck;
  int imat;
  string mat[4] = {"std::Berillia","std::Carbon","pix::ECSevices","pix::Disk"} ;
  if(a == "Inner") {
    tck = (*m_PixelDisk)[m_currentLD]->getDouble("SUP1THICK");
    imat =(*m_PixelDisk)[m_currentLD]->getInt("SUP1MAT")-1;
  } else if (a == "Central") {
    tck = (*m_PixelDisk)[m_currentLD]->getDouble("SUP2THICK");
    imat =(*m_PixelDisk)[m_currentLD]->getInt("SUP2MAT")-1;
  } else {
    tck = (*m_PixelDisk)[m_currentLD]->getDouble("SUP3THICK");
    imat =(*m_PixelDisk)[m_currentLD]->getInt("SUP3MAT")-1;
  }
  if(tck>0.) {
    return tck*Gaudi::Units::cm;
  } 
  return CalculateThickness(tck,mat[imat]);
}
string OraclePixGeoManager::PixelECCarbonMaterial(string a) {
  string mat[4] = {"std::Berillia","std::Carbon","pix::ECSevices","pix::Disk"} ; 
  int imat;
  if(a == "Inner") {
    imat = (*m_PixelDisk)[m_currentLD]->getInt("SUP1MAT")-1;
  } else if (a == "Central") {
    imat = (*m_PixelDisk)[m_currentLD]->getInt("SUP2MAT")-1;    
  } else {
    imat = (*m_PixelDisk)[m_currentLD]->getInt("SUP3MAT")-1;
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
    if(a == "Inside") return (*m_PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
    if(a == "Outside") return (*m_PixelBarrelGeneral)[0]->getInt("NFRAMEOUT");
  }
  if(isEndcap() ) {
    if(a == "Inside") return (*m_PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
    if(a == "Outside") return (*m_PixelEndcapGeneral)[0]->getInt("NFRAMEOUT");   
  }
  return 0;
}

double*  OraclePixGeoManager::PixelServiceR(string a, int n) {
  double rmin=0.;
  double rmax=0.;
  double *r = new double[2];
  if(isBarrel()) {
    if(a == "Inside") {
      rmin = (*m_PixelBarrelService)[n]->getDouble("RIN");
      rmax = (*m_PixelBarrelService)[n]->getDouble("ROUT");
    }
    else {
      rmin = (*m_PixelBarrelService)[n+m_barrelInFrames]->getDouble("RIN");
      rmax = (*m_PixelBarrelService)[n+m_barrelInFrames]->getDouble("ROUT");
    }
  }
  if(isEndcap()) {
    if(a == "Inside") {
      rmin = (*m_PixelEndcapService)[n]->getDouble("RIN");
      rmax = (*m_PixelEndcapService)[n]->getDouble("ROUT");
    }
    else {
      rmin = (*m_PixelEndcapService)[n+m_endcapInFrames]->getDouble("RIN");
      rmax = (*m_PixelEndcapService)[n+m_endcapInFrames]->getDouble("ROUT");
    }
  }
  // If this is negative this is the thickness of the cyl in % of r.l.
  r[0] = rmin*Gaudi::Units::cm;
  if(rmax > 0) {
    r[1] = rmax*Gaudi::Units::cm;
  } else {
    string material = PixelServiceMaterial(a,n);
    r[1] = fabs(rmin*Gaudi::Units::cm)+CalculateThickness(rmax,material);
  }
  return r;
}


double* OraclePixGeoManager::PixelServiceZ(string a,int n) {
  double* z = new double[2];
  if(isBarrel()) {
    if(a == "Inside") {
      z[0] = (*m_PixelBarrelService)[n]->getDouble("ZIN");
      z[1] = (*m_PixelBarrelService)[n]->getDouble("ZOUT");
    }
    else {
      z[0] = (*m_PixelBarrelService)[n+m_barrelInFrames]->getDouble("ZIN");
      z[1] = (*m_PixelBarrelService)[n+m_barrelInFrames]->getDouble("ZOUT");
    }
  }
  if(isEndcap()) {
    if(a == "Inside") {
      z[0] = (*m_PixelEndcapService)[n]->getDouble("ZIN");
      z[1] = (*m_PixelEndcapService)[n]->getDouble("ZOUT");
    }
    else {
      z[0] = (*m_PixelEndcapService)[n+m_endcapInFrames]->getDouble("ZIN");
      z[1] = (*m_PixelEndcapService)[n+m_endcapInFrames]->getDouble("ZOUT");
    }
  }
  z[0] = z[0] *Gaudi::Units::cm;
  if(z[0]*(z[1]) > -0.000000001) { // same sign and z[0] > 0.
    z[1] = z[1] *Gaudi::Units::cm;
  } else {
    string material = PixelServiceMaterial(a,n);
    z[1] = z[0] * (1 + CalculateThickness(z[1],material)/fabs(z[0]));
  }
  if(isEndcap() && a == "Inside" ) { // Translate to the ecnter of EndCap
    double center = ((*m_PixelEndcapGeneral)[0]->getDouble("ZMAX")+(*m_PixelEndcapGeneral)[0]->getDouble("ZMIN"))/2.*Gaudi::Units::cm;
    z[0] = z[0]-center;
    z[1] = z[1]-center;
  }
  return z;
}


int OraclePixGeoManager::PixelServiceLD(string a,int n) {
  if(isBarrel()) {
    if(a == "Inside") {return (*m_PixelBarrelService)[n]->getInt("LAYERNUM")-1;}
    else {return (*m_PixelBarrelService)[n+m_barrelInFrames]->getInt("LAYERNUM")-1;}
  }
  if(isEndcap()) {
    if(a == "Inside") {return (*m_PixelEndcapService)[n]->getInt("LAYERNUM")-1;}
    else {return (*m_PixelEndcapService)[n+m_endcapInFrames]->getInt("LAYERNUM")-1;}
  }
  return 0;
}

string OraclePixGeoManager::PixelServiceMaterial(string a,int n) {
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
    if(a == "Inside") {imat  = (*m_PixelBarrelService)[n]->getInt("MATERIAL")-1;}
    else {imat  = (*m_PixelBarrelService)[n+m_barrelInFrames]->getInt("MATERIAL")-1;}
    return mat[imat];
  }
  if(isEndcap()) {
    string mat[4] = {"std::Berillia","std::Carbon","pix::ECServices","pix::Disk"};
    if(a == "Inside") {imat  = (*m_PixelEndcapService)[n]->getInt("MATERIAL")-1;}
    else {imat =(*m_PixelEndcapService)[n+m_endcapInFrames]->getInt("MATERIAL")-1;}
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
  double tck = (*m_PixelStave)[0]->getDouble("SUPPORTTHICK");
  if( tck > 0.) {
    return tck*Gaudi::Units::cm;
  } else {
    return CalculateThickness(tck,"pix::Ladder");
  }
}

double OraclePixGeoManager::PixelECCablesThickness() 
{
  double tck =  (*m_PixelDisk)[m_currentLD]->getDouble("CABLETHICK");
  if( tck > 0.) {
    return tck*Gaudi::Units::cm;
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
  return static_cast<int>((*m_pdch)[0]->getInt("NYCONNEC"));
}

int OraclePixGeoManager::EmptyRows(int index)
{
 const std::string a=std::string("_")+std::to_string(index);
 return static_cast<int>((*m_pdch)[0]->getInt("JYEMPTY"+a)) - 1;
}  

int OraclePixGeoManager::EmptyRowConnections(int index)
{
  const std::string a=std::string("_")+std::to_string(index);
  return static_cast<int>((*m_pdch)[0]->getInt("JYCONNEC"+a)) - 1;
}

double OraclePixGeoManager::Voltage(bool isBLayer){
  // override B-layer voltage for DC1 geometry by 
  // value in old DB (approx ratio of thicknesses (200/250 = 0.8)
  // 97.1*0.8 = 77.68. In Nova its 77.7.
  if (isBLayer && m_dc1Geometry) return 77.7*Gaudi::Units::volt; 
  if(isBLayer) { return (*m_plor)[0]->getDouble("VOLTAGE")*Gaudi::Units::volt;}
  return (*m_plor)[1]->getDouble("VOLTAGE")*Gaudi::Units::volt;
}

double OraclePixGeoManager::Temperature(bool isBLayer){
  if(isBLayer) { return (*m_plor)[0]->getDouble("TEMPC")*Gaudi::Units::kelvin+Gaudi::Units::STP_Temperature;}
  return (*m_plor)[1]->getDouble("TEMPC")*Gaudi::Units::kelvin+Gaudi::Units::STP_Temperature;
}

const GeoTrf::Vector3D & 
OraclePixGeoManager::magneticField(bool isBLayer) const
{
  if (m_magFieldFromNova) {
    if(isBLayer) { 
      m_magField = GeoTrf::Vector3D(0, 0, (*m_plrn)[0]->getDouble("BFIELD") * Gaudi::Units::tesla);
    } else {
      m_magField = GeoTrf::Vector3D(0, 0, (*m_plrn)[1]->getDouble("BFIELD") * Gaudi::Units::tesla);
    }
  }
  return m_magField;
}

void 
OraclePixGeoManager::setMagneticField(const GeoTrf::Vector3D & field)
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
