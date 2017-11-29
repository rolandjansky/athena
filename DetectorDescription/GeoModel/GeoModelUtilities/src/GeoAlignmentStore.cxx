/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoAlignmentStore.h"

void GeoAlignmentStore::setDelta(const GeoAlignableTransform* axf, const HepGeom::Transform3D& xf)
{
  auto result = m_deltas.emplace(axf,xf);
  if(!result.second) {
    // Instertion did not take place! The alignment store already contains Delta corresponding to given GeoAlignableTransform
    throw ExcAlignmentStore("Attempted to overwrite Delta in the Alignment Store");    
  }

  m_absPositions.clear();
  m_defAbsPositions.clear();
}

const HepGeom::Transform3D* GeoAlignmentStore::getDelta(const GeoAlignableTransform* axf) const
{
  auto it = m_deltas.find(axf);
  if(it!=m_deltas.end()) {
    return &it->second;
  }
  else {
    return nullptr;
  }
}

void GeoAlignmentStore::setAbsPosition(const GeoVFullPhysVol* fpv, const HepGeom::Transform3D& xf) 
{
  auto result = m_absPositions.emplace(fpv,xf);
  if(!result.second) {
    // Instertion did not take place! The alignment store already contains AbsPosition corresponding to given GeoVFullPhysVol
    throw ExcAlignmentStore("Attempted to overwrite AbsPosition in the Alignment Store");    
  }
}
 
const HepGeom::Transform3D* GeoAlignmentStore::getAbsPosition(const GeoVFullPhysVol* fpv) const 
{
  auto it = m_absPositions.find(fpv);
  if(it!=m_absPositions.end()) {
    return &it->second;
  }
  else {
    return nullptr;
  }
}

void GeoAlignmentStore::setDefAbsPosition(const GeoVFullPhysVol* fpv, const HepGeom::Transform3D& xf) 
{
  auto result = m_defAbsPositions.emplace(fpv,xf);
  if(!result.second) {
    // Instertion did not take place! The alignment store already contains DefAbsPosition corresponding to given GeoVFullPhysVol
    throw ExcAlignmentStore("Attempted to overwrite DefAbsPosition in the Alignment Store");    
  }
}

const HepGeom::Transform3D* GeoAlignmentStore::getDefAbsPosition(const GeoVFullPhysVol* fpv) const 
{
  auto it = m_defAbsPositions.find(fpv);
  if(it!=m_defAbsPositions.end()) {
    return &it->second;
  }
  else {
    return nullptr;
  }
}
