/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoBarrelAlpine.h"
#include "PixelGeoComponent/GeoLayerAlpine.h"
#include "PixelGeoComponent/GeoStaveRingSupportAlpine.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include <sstream>

InDet::GeoBarrelAlpine::GeoBarrelAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{ m_layers.clear(); m_supports.clear(); }
InDet::GeoBarrelAlpine::GeoBarrelAlpine( GeoPhysVol* vol):GeoComponent(vol)
{ m_layers.clear(); m_supports.clear(); }
InDet::GeoBarrelAlpine::GeoBarrelAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ m_layers.clear(); m_supports.clear(); }
InDet::GeoBarrelAlpine::GeoBarrelAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{ m_layers.clear(); m_supports.clear(); }


const std::vector<InDet::GeoComponent*> InDet::GeoBarrelAlpine::components() const
{
  std::vector<InDet::GeoComponent*> result; 
  result.reserve( m_layers.size() + m_supports.size());
  for (std::vector<GeoLayerAlpine*>::const_iterator i=m_layers.begin(); i!=m_layers.end(); i++) {
    result.push_back( dynamic_cast<InDet::GeoComponent*>(*i)); // implicit cast to base class
  }
  for (std::vector<GeoStaveRingSupportAlpine*>::const_iterator i=m_supports.begin(); i!=m_supports.end(); i++) {
    result.push_back( dynamic_cast<InDet::GeoComponent*>(*i)); // implicit cast to base class
  }
  return result;
}

void InDet::GeoBarrelAlpine::placeLayer( GeoLayerAlpine& layer, const HepGeom::Transform3D& transform, int number)
{

  //First resize vector if necessary
  if((int)m_layers.size()<=number)
    {
      m_layers.resize(number+1,0);
    }

  //Insert new layer
  GeoLayerAlpine *compNew=layer.place(transform, number);
  m_layers[number]=compNew;
  connectComponent(*compNew);

}

void InDet::GeoBarrelAlpine::placeSupport( GeoStaveRingSupportAlpine& support, const HepGeom::Transform3D& transform, int number)
{

  //First resize vector if necessary
  if((int)m_supports.size()<=number)
    {
      m_supports.resize(number+1,0);
    }

  //Insert new module 
  GeoStaveRingSupportAlpine *compNew=support.place(transform, number);
  m_supports[number]=compNew;
  connectComponent(*compNew);

}

InDet::GeoBarrelAlpine*  InDet::GeoBarrelAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoBarrelAlpine* newComp = 0;
  if(isPhysVol())
    newComp = new InDet::GeoBarrelAlpine(this->physVolume<GeoPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());
  else
    newComp = new InDet::GeoBarrelAlpine(this->physVolume<GeoFullPhysVol>(),number,transform,this->getNameTag(),this->getG4InsertFlag());

  for (int i=0; i<(int)m_layers.size(); i++)
    newComp->placeLayer(*m_layers[i],m_layers[i]->getTransform3D(),i);

  for (int i=0; i<(int)m_supports.size(); i++)
    newComp->placeSupport(*m_supports[i],m_supports[i]->getTransform3D(),i);

  return newComp;
}



