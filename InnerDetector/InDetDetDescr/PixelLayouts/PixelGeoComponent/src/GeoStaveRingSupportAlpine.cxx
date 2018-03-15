/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoStaveRingSupportAlpine.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoStaveRingSupportAlpine::GeoStaveRingSupportAlpine( GeoFullPhysVol* vol):GeoComponent(vol)
{}
InDet::GeoStaveRingSupportAlpine::GeoStaveRingSupportAlpine( GeoPhysVol* vol):GeoComponent(vol)
{}
InDet::GeoStaveRingSupportAlpine::GeoStaveRingSupportAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag):
  GeoComponent(vol, seqNum, trf, name, bFlag)
{}

InDet::GeoStaveRingSupportAlpine*  InDet::GeoStaveRingSupportAlpine::place( const HepGeom::Transform3D& transform, int number) const
{

  GeoStaveRingSupportAlpine* newComp = new InDet::GeoStaveRingSupportAlpine(this->physVolume<GeoPhysVol>(),number,transform,
									    this->getNameTag(),this->getG4InsertFlag());
  return newComp;
}
