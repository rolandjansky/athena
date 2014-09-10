/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelModule.h"
#include "PixelGeoModel/GeoPixelHybrid.h"
#include "PixelGeoModel/GeoPixelChip.h"
#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"

using std::max;

GeoPixelModule::GeoPixelModule(GeoPixelSiCrystal& theSensor) :
  m_theSensor(theSensor)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //
  // The module orientation is
  // x normal to the detector.
  // y in the phi direction
  // z in the eta (z) direction

  m_isModule3D=m_theSensor.GetModule3DFlag();

  //
  // The Dimensions are in separate routines
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = this->Width();
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");

  const GeoShape * moduleShape = 0;
  const GeoBox* moduleBox = new GeoBox(thickness/2.,width/2.,length/2.);
  if (ThicknessP() == ThicknessN()) {
    moduleShape = moduleBox;
  } else {
    // Shift so the center of the box is the center of the sensor.
    double shift = 0.5 * (ThicknessP() - ThicknessN());
    const GeoShape & shiftedBox = (*moduleBox) << HepGeom::TranslateX3D(shift);
    moduleShape = &shiftedBox;
  }
  std::string logName = gmt_mgr->isBarrel() ? "ModuleBrl" : "ModuleEC";
  theModule = new GeoLogVol(logName,moduleShape,air);


}

GeoVPhysVol* GeoPixelModule::Build( ) {
  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(theModule);
  //
  // Place the Si Crystal
  //
  GeoVPhysVol *theSi = m_theSensor.Build();
  _id = m_theSensor.getID();
  std::string sensorName = gmt_mgr->isBarrel() ? "SensorBrl" : "SensorEC";
  GeoNameTag *tag = new GeoNameTag(sensorName);         
  modulePhys->add(tag);
  // We give the barrel sensors an id of 100 and endcap sensors an id of 200 so that they can
  // can be distinguished in the G4 sensitive detector. For SLHC endcap we set it to 300.
  int idTag = 100; // barrel (same for slhc/non slhc)
  if (gmt_mgr->isEndcap()) {
    idTag = (gmt_mgr->slhc()) ? 300 : 200; // endcap (300: slhc, 200: non slhc)
  }
  modulePhys->add(new GeoIdentifierTag(idTag) );
  //Sensor is centered so we don't need the transform.
  //GeoTransform *xformsi = new GeoTransform(HepGeom::Transform3D());
  //modulePhys->add(xformsi);
  modulePhys->add(theSi );
  //
  // Place the Hybrid
  //
  if (gmt_mgr->PixelHybridThickness(m_isModule3D)>0.00001*CLHEP::mm){
    GeoPixelHybrid ph(m_isModule3D);
    double hybxpos = -0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)+gmt_mgr->PixelHybridThickness(m_isModule3D));
    GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(hybxpos));
    modulePhys->add(xform);
    modulePhys->add(ph.Build() );
  }

  //
  // Place the Chip
  //
  GeoPixelChip pc(m_isModule3D);
  double chipxpos = 0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)+gmt_mgr->PixelChipThickness(m_isModule3D))+gmt_mgr->PixelChipGap(m_isModule3D);
  GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(chipxpos));
  modulePhys->add(xform);
  modulePhys->add(pc.Build() );
  //
  // Add the silicon element to the list of detector elements...
  //
  return modulePhys;
}


double GeoPixelModule::ThicknessN() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessN
  // is the max of ThicknessP and thickness from the module center to
  // the outer surface of the hybrid plus some safety.
  //


  double safety = 0.01*CLHEP::mm; 
  double thickn = 0.5 * gmt_mgr->PixelBoardThickness(m_isModule3D)+ gmt_mgr->PixelHybridThickness(m_isModule3D) + safety;
  double thick = max(thickn, ThicknessP()); 

  return thick;
}

double GeoPixelModule::ThicknessP() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessP
  // is thickness from the module center to the outer surface of the
  // chips plus some safety.

  double safety = 0.01*CLHEP::mm;
  double thick = 0.5 * gmt_mgr->PixelBoardThickness(m_isModule3D) +
    gmt_mgr->PixelChipThickness(m_isModule3D)+gmt_mgr->PixelChipGap(m_isModule3D) + safety;

  return thick;
}

double GeoPixelModule::Thickness() {
  // This is total thickness of the module envelope
  return ThicknessP()+ThicknessN();
}

double GeoPixelModule::Width() {
  double width = max( max(
                     gmt_mgr->PixelBoardWidth(m_isModule3D),
                     gmt_mgr->PixelHybridWidth(m_isModule3D)),
                     gmt_mgr->PixelChipWidth(m_isModule3D));
  return width;
}
double GeoPixelModule::Length() {
  // balcony is zero

  //  std::cout<<m_isModule3D<<" "<<gmt_mgr->PixelHybridLength(m_isModule3D)<<" "<<gmt_mgr->PixelBoardLength(m_isModule3D)<<" "<<gmt_mgr->PixelChipLength(m_isModule3D)<<std::endl;

  double length = max( max( //max(
                                gmt_mgr->PixelHybridLength(m_isModule3D),
                                gmt_mgr->PixelBoardLength(m_isModule3D)),
		       //2*gmt_mgr->PixelBalcony()),
                       gmt_mgr->PixelChipLength(m_isModule3D));

  return length;
}

Identifier GeoPixelModule::getID() {
  return _id;
}
