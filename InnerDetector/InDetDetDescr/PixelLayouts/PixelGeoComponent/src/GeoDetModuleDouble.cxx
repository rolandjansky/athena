/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoDetModuleDouble.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "PixelGeoComponent/GeoSimpleObject.h"
#include "PixelGeoComponent/GeoDetModuleEnv.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoDetModuleDouble::GeoDetModuleDouble( GeoFullPhysVol* vol):GeoDetModule(vol)
{ 
  m_innerEnvelope=0; m_outerEnvelope=0;
  m_auxServices.clear();
  m_numSides=2;
}
// InDet::GeoDetModuleDouble::GeoDetModuleDouble( GeoPhysVol* vol):GeoDetModule(vol)
// { 
//   m_innerEnvelope=0; m_outerEnvelope=0;
//   m_auxServices.clear();
// }
InDet::GeoDetModuleDouble::GeoDetModuleDouble(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule):
  GeoDetModule(vol, seqNum, trf, name, bFlag, etaModule)
{
  m_innerEnvelope=0; m_outerEnvelope=0;
  m_auxServices.clear();
  m_numSides=2;

//   this->setEtaModule(etaModule);
//   this->setPhiModule(phiModule);

}

InDet::GeoDetModuleDouble::~GeoDetModuleDouble()
{

  if(m_innerEnvelope) delete m_innerEnvelope;
  if(m_outerEnvelope) delete m_outerEnvelope;

  for (std::vector<GeoSimpleObject*>::const_iterator i=m_auxServices.begin(); i!=m_auxServices.end(); i++) {
    if(*i) delete (*i);
  }

  //  this->UnRef();
}


const std::vector<InDet::GeoComponent*> InDet::GeoDetModuleDouble::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  result.reserve(m_auxServices.size()+2);
  if(m_innerEnvelope) result.push_back(m_innerEnvelope);
  if(m_outerEnvelope) result.push_back(m_outerEnvelope);

  for (std::vector<GeoSimpleObject*>::const_iterator i=m_auxServices.begin(); i!=m_auxServices.end(); i++) {
    result.push_back(*i); // implicit cast to base class
  }

  return result;
}

const std::vector<InDet::GeoSensor*> InDet::GeoDetModuleDouble::sensors(int iSide) const
{
  if(iSide==0)
    return m_innerEnvelope->sensors();
  else
    return m_outerEnvelope->sensors();
}

int InDet::GeoDetModuleDouble::numActiveComponents(int iSide) const
{
  std::cout<<"simple "<<std::endl; 
  if(iSide==0)
    return m_innerEnvelope->numActiveComponents(0);

  return m_outerEnvelope->numActiveComponents(0);
}

const InDet::GeoSensor& InDet::GeoDetModuleDouble::activeComponent( int i, int iSide) const 
{
  if(iSide==0)
    return m_innerEnvelope->activeComponent(i);
  else
    return m_outerEnvelope->activeComponent(i);
}


const std::vector<InDet::GeoSimpleObject* > InDet::GeoDetModuleDouble::serviceComponent(int iSide) const
{
  if(iSide==0)
    return m_innerEnvelope->serviceComponent();
  else
    return m_outerEnvelope->serviceComponent();
}

void InDet::GeoDetModuleDouble::placeSensor( GeoSensor& /*sensor*/, const HepGeom::Transform3D& /*transform*/, int /*number*/, int /*sector*/)
{
//   // Inner module
//   if(sector==0){
//     if(m_innerEnvelope==0) return;
//     m_innerEnvelope->placeSensor(sensor,transform,number);
//     return;
//   }

//   // Outer module
//   if(sector==1){
//     if(m_outerEnvelope==0) return;
//     m_outerEnvelope->placeSensor(sensor,transform,number);
//     return;
//   }
}


void InDet::GeoDetModuleDouble::placeSensorService( GeoSimpleObject& service, const HepGeom::Transform3D& transform, int number, int sector)
{
  // Inner module
  if(sector==0){
    if(m_innerEnvelope==0) return;
    m_innerEnvelope->placeSensorService(service,transform,number);
    return;
  }

  // Outer module
  if(sector==1){
    if(m_outerEnvelope==0) return;
    m_outerEnvelope->placeSensorService(service,transform,number);
    return;
  }

}


void InDet::GeoDetModuleDouble::placeAuxService( GeoSimpleObject& service, const HepGeom::Transform3D& transform)
{
  //First resize vector if necessary
  unsigned int number=m_auxServices.size();
  m_auxServices.resize(number+1,0);
  
  //Insert new layer
  GeoSimpleObject* compNew=service.place(transform, number);
  m_auxServices[number]=compNew;
  connectComponent(*compNew);
  
}


void InDet::GeoDetModuleDouble::placeSensorEnvelope( GeoDetModuleEnv& sensorEnv, const HepGeom::Transform3D& transform, int sector, int sideId)
{
  
  if(sector==0){
    GeoDetModuleEnv* compNew=sensorEnv.place(transform, 0, sideId);
    m_innerEnvelope=compNew;
    connectComponent(*compNew);
    return;
  }

  if(sector==1){
    GeoDetModuleEnv* compNew=sensorEnv.place(transform, 0, sideId);
    m_outerEnvelope=compNew;
    connectComponent(*compNew);
    return;
  }

}


InDet::GeoDetModuleDouble* InDet::GeoDetModuleDouble::place(const HepGeom::Transform3D& transform, int number, int etaModule)
{
  GeoDetModuleDouble* newComp = new InDet::GeoDetModuleDouble(this->physVolume<GeoFullPhysVol>(),number,transform,
							      this->getNameTag(),this->getG4InsertFlag(),
							      etaModule);

  newComp->setModuleType(getModuleType());

  if(m_innerEnvelope)
    newComp->placeSensorEnvelope(*m_innerEnvelope,m_innerEnvelope->getTransform(),0,m_innerEnvelope->getSideId());

  if(m_outerEnvelope)
    newComp->placeSensorEnvelope(*m_outerEnvelope,m_outerEnvelope->getTransform(),1,m_outerEnvelope->getSideId());

  for (std::vector<GeoSimpleObject*>::const_iterator it=m_auxServices.begin(); it!=m_auxServices.end(); it++)
     newComp->placeAuxService(*(*it),(*it)->getTransform());

//   std::cout<<"GeoDetModuleDouble : place "<<(m_innerEnvelope==0)<<" "<<(m_outerEnvelope==0)<<std::endl;
//   m_innerEnvelope->dump();
//   m_outerEnvelope->dump();
//   std::cout<<" ---- res ------"<<std::endl;
//   newComp->dump();
//   std::cout<<" ---------------"<<std::endl;

  return newComp;
}


// int InDet::GeoDetModuleDouble::getModuleType() const 
// {
//   if(iSide==0)
//     return m_innerEnvelope->getModuleType();
//   return m_outerEnvelope->getModuleType();

// }

const GeoFullPhysVol* InDet::GeoDetModuleDouble::getSensorFullPhysVolume(int iSensor, int iSide) const 
{
  if(iSide==0)
    return m_innerEnvelope->getSensorFullPhysVolume(iSensor);
  return m_outerEnvelope->getSensorFullPhysVolume(iSensor);

}

InDet::MinMaxHelper InDet::GeoDetModuleDouble::getSensorZminmax(const HepGeom::Transform3D& trf) const
{
  GeoComponentHelper compHelper(m_innerEnvelope->activeComponent(0));
  return (compHelper.getZminmax(trf));

}

const InDet::GeoDetModuleEnv& InDet::GeoDetModuleDouble::getModuleEnvelope(int iSide) const
{
  if(iSide==0) return *m_innerEnvelope;
  return *m_outerEnvelope;

}


int InDet::GeoDetModuleDouble::getEnvelopeSideId(int iSide) const
{
  if(iSide==0)
    return m_innerEnvelope->getSideId();
  return m_outerEnvelope->getSideId();
}


int InDet::GeoDetModuleDouble::getSensorPos(int iSensor, int iSide) const
{
  if(iSide==0)
    return m_innerEnvelope->getSensorPos(iSensor);
  return m_outerEnvelope->getSensorPos(iSensor);
}
