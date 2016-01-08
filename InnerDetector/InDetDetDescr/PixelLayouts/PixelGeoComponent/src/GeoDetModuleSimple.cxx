/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoDetModuleSimple.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "PixelGeoComponent/GeoSimpleObject.h"
#include "PixelGeoComponent/GeoDetModuleEnv.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoDetModuleSimple::GeoDetModuleSimple( GeoFullPhysVol* vol):GeoDetModule(vol)
{ 
  m_sensors.clear(); m_sensorServices.clear(); m_auxServices.clear();
  m_numSides=1;

}
// InDet::GeoDetModuleSimple::GeoDetModuleSimple( GeoPhysVol* vol):GeoDetModule(vol)
// { 
//   m_sensors.clear(); m_sensorServices.clear();  m_auxServices.clear();
// }

InDet::GeoDetModuleSimple::GeoDetModuleSimple(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule):
  GeoDetModule(vol, seqNum, trf, name, bFlag, etaModule)
{
  m_sensors.clear(); m_sensorServices.clear();  m_auxServices.clear();
  m_numSides=1;
}

InDet::GeoDetModuleSimple::~GeoDetModuleSimple()
{
  for (std::vector<GeoSensor*>::const_iterator i=m_sensors.begin(); i!=m_sensors.end(); i++) {
    if(*i) delete (*i);
  }

  for (std::vector<GeoSimpleObject*>::const_iterator i=m_sensorServices.begin(); i!=m_sensorServices.end(); i++) {
    if(*i) delete (*i);
  }
  for (std::vector<GeoSimpleObject*>::const_iterator i=m_auxServices.begin(); i!=m_auxServices.end(); i++) {
    if(*i) delete (*i);
  }

}

const std::vector<InDet::GeoComponent*> InDet::GeoDetModuleSimple::components() const
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

void InDet::GeoDetModuleSimple::placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform, int number, int sensorPos )
{
  GeoSensor *compNew=sensor.place(transform, number, sensorPos);
  m_sensors.push_back(compNew);
  connectComponent(*compNew);
}


void InDet::GeoDetModuleSimple::placeSensorService( GeoSimpleObject& modLayer, const HepGeom::Transform3D& transform, int number, int /*sector*/)
{
  GeoSimpleObject *compNew=modLayer.place(transform, number);
  m_sensorServices.push_back(compNew);
  connectComponent(*compNew);
}

void InDet::GeoDetModuleSimple::placeAuxService( GeoSimpleObject& service, const HepGeom::Transform3D& transform)
{
  //First resize vector if necessary
  unsigned int number=m_auxServices.size();
  m_auxServices.resize(number+1,0);
  
  //Insert new layer
  GeoSimpleObject* compNew=service.place(transform, number);
  m_auxServices[number]=compNew;
  connectComponent(*compNew);
  
}

void InDet::GeoDetModuleSimple::placeSensorEnvelope( GeoDetModuleEnv& /*sensorEnv*/, const HepGeom::Transform3D& /*transform*/, int /*sector*/, int /*sideId*/)
{
}

InDet::GeoDetModuleSimple* InDet::GeoDetModuleSimple::place(const HepGeom::Transform3D& transform, int number, int etaModule)
{
  GeoDetModuleSimple* newComp = new InDet::GeoDetModuleSimple(this->physVolume<GeoFullPhysVol>(),number,transform,
							      this->getNameTag(),this->getG4InsertFlag(),
							      etaModule);
  
  newComp->setModuleType(getModuleType());

  int iCmpt=0;
  for (std::vector<GeoSensor*>::const_iterator it=m_sensors.begin(); it!=m_sensors.end(); it++) 
    newComp->placeSensor(*(*it),(*it)->getTransform(),iCmpt++,(*it)->getSensorPos());

  iCmpt=0;
  for (std::vector<GeoSimpleObject*>::const_iterator it=m_sensorServices.begin(); it!=m_sensorServices.end(); it++)
    newComp->placeSensorService(*(*it),(*it)->getTransform(),iCmpt++);

  iCmpt=0;
  for (std::vector<GeoSimpleObject*>::const_iterator it=m_auxServices.begin(); it!=m_auxServices.end(); it++)
    newComp->placeAuxService(*(*it),(*it)->getTransform());
  
  return newComp;
}

// int InDet::GeoDetModuleSimple::getModuleType(int iSensor, int /*iSide*/) const 
// {
//   return m_sensors[iSensor]->getModuleType(); 
// }

const GeoFullPhysVol* InDet::GeoDetModuleSimple::getSensorFullPhysVolume(int iSensor, int /*iSide*/) const 
{
  return m_sensors[iSensor]->getSensorFullPhysVolume();
}

InDet::MinMaxHelper InDet::GeoDetModuleSimple::getSensorZminmax(const HepGeom::Transform3D& trf) const
{
  GeoComponentHelper compHelper(*(m_sensors[0]));
  return (compHelper.getZminmax(trf));

}

int InDet::GeoDetModuleSimple::getSensorPos(int iSensor, int /*iSide*/) const
{
  return m_sensors[iSensor]->getSensorPos(); 
}

int InDet::GeoDetModuleSimple::numActiveComponents(int iSide) const 
{ 
  std::cout<<"simple "<<std::endl; 
  iSide++; return m_sensors.size();
}
