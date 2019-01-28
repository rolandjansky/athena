/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//
// New description of the pixel support frame (M.Zdrazil + AR) 
// The box part (of barrel)
//
#include "GeoPixelOldFrame.h"
#include "PixelLegacyManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/SystemOfUnits.h"

// Satisfy interface

GeoPixelOldFrame::GeoPixelOldFrame()
{
  m_legacyManager = m_gmt_mgr->legacyManager();
}

GeoVPhysVol* GeoPixelOldFrame::Build() 
{
  return 0;
}

GeoVPhysVol* GeoPixelOldFrame::BuildBox() {
  //
  std::string logName = "BoxFrBarOutLog";
  std::string volumeName = "Frame";
  int typenum = 1;
  if(m_legacyManager->isBarrelInSFrame() ) {
    logName = "BoxFrBarInLog";
    typenum = 0;
  }
  if(m_legacyManager->isEndcapInSFrame() ) {
    logName = "BoxFrEndcapLog";
    typenum = 2;
  }
  //
  // Dimensions
  //
  double length    = m_legacyManager->PixelBFrameHalfLength();
  double thickness = m_legacyManager->PixelBFrameHalfThickness();
  double width     = m_legacyManager->PixelBFrameHalfWidth();
  //
  const GeoBox* frameBox = new GeoBox(thickness,width,length);
  std::string matName = m_gmt_mgr->getMaterialName(volumeName, 0, typenum);
  const GeoMaterial* frameMat  = m_mat_mgr->getMaterialForVolume(matName, frameBox->volume());
  GeoLogVol* theFrame    = new GeoLogVol(logName,frameBox,frameMat);
  GeoPhysVol* framePhys  = new GeoPhysVol(theFrame);

  return framePhys;
}

//
// New description of the pixel support frame (M.Zdrazil + AR) 
// The trapezoid part (of barrel)
//

GeoVPhysVol* GeoPixelOldFrame::BuildTrap() {
  //
  std::string logName = "TrapFrBarOutLog";
  std::string volumeName = "Frame";
  int typenum = 4;
  if(m_legacyManager->isBarrelInSFrame()) {
    logName = "TrapFrBarInLog";
    typenum = 3;
  }
  if(m_legacyManager->isEndcapInSFrame()) {
    logName = "TrapFrEndcapLog";
    typenum = 5;
  }
  if(m_legacyManager->isEndConeSFrame()) {   
    logName = "TrapFrEndCone";
    volumeName = "EndCone";
    typenum = 0;
  }
  //
  // Dimensions
  //
  double al1 = 0.;
  double al2 = 0.;
  double phi = 0.;
  double length   = m_legacyManager->PixelTFrameHalfLength();
  double widthy   = m_legacyManager->PixelTFrameHalfWidthY();
  double widthxzn = m_legacyManager->PixelTFrameHalfWidthXzn();
  double widthxzp = m_legacyManager->PixelTFrameHalfWidthXzp();
  double tet      = m_legacyManager->PixelTFrameDzDr();
  //

  const GeoTrap* frameTrap  = new GeoTrap(length,tet,phi,widthy,widthxzn,widthxzp,al1,widthy,widthxzn,widthxzp,al2);
  std::string matName = m_gmt_mgr->getMaterialName(volumeName, 0, typenum);
  const GeoMaterial* frameMat  = m_mat_mgr->getMaterialForVolume(matName, frameTrap->volume());
  GeoLogVol* theFrame       = new GeoLogVol(logName,frameTrap,frameMat);
  GeoPhysVol* framePhys     = new GeoPhysVol(theFrame);
 
  return framePhys;
}



///////////////////////
void GeoPixelOldFrame::BuildInBarrel(GeoFullPhysVol * parent) {
  // It is split into two parts : one is the barrel part and one
  // is part of the endcap frame.
  //
  
  // First part
  m_legacyManager->setBarrelInSFrame();
  double alpha = 45.*Gaudi::Units::deg;
  double w1    = m_legacyManager->PixelBarrelBFrameWidth();
  double w2    = m_legacyManager->PixelBarrelTFrameWidth();
  double off   = m_legacyManager->PixelBarrelFrameOffset();
  for (int ii = 0; ii < m_legacyManager->PixelBarrelNBFrame(); ii++) {
    GeoNameTag* tag = new GeoNameTag("Inside Box Frame");
    double thet = ((float)ii+0.5)*alpha;
    GeoTrf::Translation3D pos(w1*cos(thet),w1*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+2.5)*alpha);
    GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform);
    parent->add(BuildBox());
  }

  for (int ii = 0; ii < m_legacyManager->PixelBarrelNTFrame(); ii++) {
    GeoNameTag* tag = new GeoNameTag("Inside Trap Frame");
    double thet = ((float)ii+1.)*alpha;
    GeoTrf::Translation3D pos(w2*cos(thet),w2*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+3.0)*alpha);
    GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform);
    parent->add(BuildTrap());
  }
  //
  // Second part
  m_legacyManager->setBarrelOutSFrame();
  w1    = m_legacyManager->PixelBarrelBFrameWidth();
  w2    = m_legacyManager->PixelBarrelTFrameWidth();
  off   = m_legacyManager->PixelBarrelFrameOffset()+m_legacyManager->PixelBarrelFrameLength();
  double radi  = m_legacyManager->PixelBarrelFrameECRadius();
  double zpec  = m_legacyManager->PixelBarrelFrameECZPos();
  double alxec = m_legacyManager->PixelBarrelFrameECAlphaX();
  for (int ii = 0; ii < m_legacyManager->PixelBarrelNBFrame(); ii++) {
    GeoNameTag* tag = new GeoNameTag("Outside Box Frame");
    double thet = ((float)ii+0.5)*alpha;
    GeoTrf::Translation3D pos(w1*cos(thet),w1*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+2.5)*alpha);
    GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform);
    parent->add(BuildBox());
    pos.z()= -off;
    GeoTransform* xform2 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform2);
    parent->add(BuildBox());
  }
  for (int ii = 0; ii < m_legacyManager->PixelBarrelNTFrame(); ii++) {
    m_legacyManager->setBarrelOutSFrame(); 
    GeoNameTag* tag = new GeoNameTag("Outside Trap Frame");
    double thet = ((float)ii+1.)*alpha;
    GeoTrf::Translation3D pos(w2*cos(thet),w2*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+3.0)*alpha);
    GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform);
    parent->add(BuildTrap());
    pos.z() = -off;
    GeoTransform* xform2 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
    parent->add(tag);
    parent->add(xform2);
    parent->add(BuildTrap());
    //
    m_legacyManager->setEndConeSFrame();
    GeoNameTag* tag2 = new GeoNameTag("Outside End Cone Trap Frame");
    thet = ((float)ii+0.5)*alpha;
    GeoTrf::Translation3D posec(radi*cos(thet),radi*sin(thet),zpec);
    GeoTrf::Transform3D rm2 = GeoTrf::RotateZ3D(((float)ii+2.5)*alpha)*GeoTrf::RotateX3D(alxec);
    GeoTransform* xform3 = new GeoTransform(GeoTrf::Transform3D(posec*rm2));
    parent->add(tag2);
    parent->add(xform3);
    parent->add(BuildTrap());
    posec.z() = -zpec;
    GeoTrf::Transform3D rm3 = GeoTrf::RotateZ3D(((float)ii+2.5)*alpha)*GeoTrf::RotateX3D(-alxec);
    GeoTransform* xform4 = new GeoTransform(GeoTrf::Transform3D(posec*rm3));
    parent->add(tag2);
    parent->add(xform4);
    parent->add(BuildTrap());
  }
}

void GeoPixelOldFrame::BuildOutBarrel(GeoFullPhysVol * parent) {


  bool endcapAPresent  = m_gmt_mgr->partPresent("EndcapA");
  bool endcapCPresent  = m_gmt_mgr->partPresent("EndcapC");

  //
  // Add the pixel frame inside the endcap volume
  //
  m_legacyManager->setEndcapInSFrame();
  double alpha = 45.*Gaudi::Units::deg;
  double w1    = m_legacyManager->PixelEndcapBFrameWidth();
  double w2    = m_legacyManager->PixelEndcapTFrameWidth();
  double off   = m_legacyManager->PixelEndcapFrameOffset()+m_legacyManager->PixelEndcapFrameLength();

  for (int ii = 0; ii < m_legacyManager->PixelEndcapNBFrame(); ii++) {
    GeoNameTag* tag = new GeoNameTag("Outside Outer Endcap Box Frame");
    double thet = ((float)ii+0.5)*alpha;
    GeoTrf::Translation3D pos(w1*cos(thet),w1*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+2.5)*alpha);

    if (endcapAPresent) {
      GeoTransform* xform1 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      parent->add(tag);
      parent->add(xform1);
      parent->add(BuildBox());
    } 
    if (endcapCPresent) {
      pos.z() = -off;
      GeoTransform* xform2 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      parent->add(tag);
      parent->add(xform2);
      parent->add(BuildBox());
    }
  }

  for (int ii = 0; ii < m_legacyManager->PixelEndcapNTFrame(); ii++) {
    GeoNameTag* tag = new GeoNameTag("Outside Outer Endcap Trap Frame");
    double thet = ((float)ii+1.)*alpha;
    GeoTrf::Translation3D pos(w2*cos(thet),w2*sin(thet),off);
    GeoTrf::RotateZ3D rm(((float)ii+3.0)*alpha);
    if (endcapAPresent) {
      GeoTransform* xform1 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      parent->add(tag);
      parent->add(xform1);
      parent->add(BuildTrap());
    } 
    if (endcapCPresent) {
      pos.z() = -off;
      GeoTransform* xform2 = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      parent->add(tag);
      parent->add(xform2);
      parent->add(BuildTrap());
    }
  }
}

