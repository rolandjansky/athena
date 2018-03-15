/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoSensor.h"
/*#include "InDetReadoutGeometry/PixelModuleDesign.h"*/

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

/*using namespace InDetDD;*/

InDet::GeoSensor::GeoSensor( GeoFullPhysVol* vol):GeoComponent(vol)
{ m_sensorPos=-9999;}
InDet::GeoSensor::GeoSensor( GeoPhysVol* vol):GeoComponent(vol)
{ m_sensorPos=-9999;}

InDet::GeoSensor::GeoSensor(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int sensorPos):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{
  m_sensorPos=sensorPos;
}

InDet::GeoSensor::~GeoSensor()
{
  //  this->UnRef();
}


InDet::GeoSensor* InDet::GeoSensor::place(const HepGeom::Transform3D& transform, int number, int sensorPos) const
{

  GeoSensor* newComp = new InDet::GeoSensor(this->physVolume<GeoFullPhysVol>(),number,transform,
					    this->getNameTag(),this->getG4InsertFlag(),
					    sensorPos);
  return newComp;
}

