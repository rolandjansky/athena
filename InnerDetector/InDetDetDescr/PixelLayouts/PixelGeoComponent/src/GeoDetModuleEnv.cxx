/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoDetModuleEnv.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "PixelGeoComponent/GeoSimpleObject.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoDetModuleEnv::GeoDetModuleEnv( GeoFullPhysVol* vol):GeoComponent(vol)
{ 
  m_sensors.clear(); m_sensorServices.clear(); 
  m_sideId = -9999;
}
InDet::GeoDetModuleEnv::GeoDetModuleEnv(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int sideId):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{
  m_sensors.clear(); m_sensorServices.clear(); 
  m_sideId = sideId;
}

InDet::GeoDetModuleEnv::~GeoDetModuleEnv()
{

  for (std::vector<GeoSensor*>::const_iterator i=m_sensors.begin(); i!=m_sensors.end(); i++) {
    if(*i) delete (*i);
  }
  for (std::vector<GeoSimpleObject*>::const_iterator i=m_sensorServices.begin(); i!=m_sensorServices.end(); i++) {
    if(*i) delete (*i);
  }

  //  this->UnRef();
}

const std::vector<InDet::GeoComponent*> InDet::GeoDetModuleEnv::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  result.reserve( m_sensors.size()+m_sensorServices.size());
  for (std::vector<GeoSensor*>::const_iterator i=m_sensors.begin(); i!=m_sensors.end(); i++) {
    result.push_back(*i); // implicit cast to base class
  }
  for (std::vector<GeoSimpleObject*>::const_iterator i=m_sensorServices.begin(); i!=m_sensorServices.end(); i++) {
    result.push_back(*i); // implicit cast to base class
  }
  return result;
}

void InDet::GeoDetModuleEnv::placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform, int number, int sensorPos)
{
  GeoSensor *compNew=sensor.place(transform, number, sensorPos);
  m_sensors.push_back(compNew);
  connectComponent(*compNew);
}


void InDet::GeoDetModuleEnv::placeSensorService( GeoSimpleObject& modLayer, const HepGeom::Transform3D& transform, int number)
{
  GeoSimpleObject *compNew=modLayer.place(transform, number);
  m_sensorServices.push_back(compNew);
  connectComponent(*compNew);
}


InDet::GeoDetModuleEnv* InDet::GeoDetModuleEnv::place(const HepGeom::Transform3D& transform, int number, int sensorId)
{
  GeoDetModuleEnv* newComp = new InDet::GeoDetModuleEnv(this->physVolume<GeoFullPhysVol>(),number,transform,
							this->getNameTag(),this->getG4InsertFlag(), sensorId);
  
  int iCmpt=0;
  for (std::vector<GeoSensor*>::const_iterator it=m_sensors.begin(); it!=m_sensors.end(); it++) 
    newComp->placeSensor(*(*it),(*it)->getTransform(),iCmpt++,(*it)->getSensorPos());

  iCmpt=0;
  for (std::vector<GeoSimpleObject*>::const_iterator it=m_sensorServices.begin(); it!=m_sensorServices.end(); it++)
    newComp->placeSensorService(*(*it),(*it)->getTransform(),iCmpt++);

  return newComp;
}

// int InDet::GeoDetModuleEnv::getModuleType(i) const 
// {
  
//   return m_sensors[iSensor]->getModuleType(); 
// }

const GeoFullPhysVol* InDet::GeoDetModuleEnv::getSensorFullPhysVolume(int iSensor) const 
{
  return m_sensors[iSensor]->getSensorFullPhysVolume();
}

InDet::MinMaxHelper InDet::GeoDetModuleEnv::getSensorZminmax(const HepGeom::Transform3D& trf) const
{
  GeoComponentHelper compHelper(*(m_sensors[0]));
  return (compHelper.getZminmax(trf));

}

int InDet::GeoDetModuleEnv::getSensorPos(int iSensor) const
{
  return m_sensors[iSensor]->getSensorPos();
}
