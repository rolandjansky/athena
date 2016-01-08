/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoSimpleObject.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"


InDet::GeoSimpleObject::GeoSimpleObject( GeoPhysVol* vol):GeoComponent(vol)
{}

InDet::GeoSimpleObject::GeoSimpleObject( GeoVPhysVol* vol):GeoComponent((GeoPhysVol*)vol)
{}

InDet::GeoSimpleObject::GeoSimpleObject(InDet::GeoComponent* comp):
  GeoComponent(comp->physVolume<GeoPhysVol>(), comp->getSeqNumber(), comp->getTransform(), comp->getNameTag(), comp->getG4InsertFlag())
{

//   std::cout<<"SIMPLE object "<<std::endl;
//   this->dump();
  
  const std::vector<InDet::GeoComponent*> vec=comp->components();
  //  std::cout<<"SIMPLE object "<<vec.size()<<std::endl;
  
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
      this->placeAsChild(*(*i),(*i)->getTransform3D(),(*i)->getSeqNumber());
  }

  //  std::cout<<"  -> final object"<<std::endl;
  //  this->dump();

}

InDet::GeoSimpleObject::GeoSimpleObject(InDet::GeoComponent* comp, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(comp->physVolume<GeoPhysVol>(), comp->getSeqNumber(), trf, name, bFlag)
{

//   std::cout<<"SIMPLE object "<<std::endl;
//   this->dump();
  
  const std::vector<InDet::GeoComponent*> vec=comp->components();
  //  std::cout<<"SIMPLE object "<<vec.size()<<std::endl;
  
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    this->placeAsChild(*(*i),(*i)->getTransform3D(),(*i)->getSeqNumber());
  }

  //  std::cout<<"  -> final object"<<std::endl;
  //  this->dump();

}


InDet::GeoSimpleObject::GeoSimpleObject(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{}

InDet::GeoSimpleObject::GeoSimpleObject(GeoVPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(dynamic_cast<GeoPhysVol*>(vol), seqNum, trf, name, bFlag)
{}


InDet::GeoSimpleObject::~GeoSimpleObject() 
{

  //  std::cout<<"Delete GeoSimpleObject : "<<name()<<std::endl; 

  //  if(m_transform3D) delete m_transform3D;
  //  this->UnRef();
}

InDet::GeoSimpleObject*  InDet::GeoSimpleObject::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoSimpleObject* newComp = new InDet::GeoSimpleObject(this->physVolume<GeoPhysVol>(),number,transform,
								  this->getNameTag(),this->getG4InsertFlag());

  const std::vector<InDet::GeoComponent*> vec=components();
  for (std::vector<GeoComponent*>::const_iterator i=vec.begin(); i!=vec.end(); i++) {
    newComp->placeAsChild(*(*i),(*i)->getTransform3D(),(*i)->getSeqNumber());
  }

  return newComp;
}

