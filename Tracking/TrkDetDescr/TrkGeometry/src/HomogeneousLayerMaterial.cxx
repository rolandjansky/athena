/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HomogeneousLayerMaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"

Trk::HomogeneousLayerMaterial::HomogeneousLayerMaterial() :
  Trk::LayerMaterialProperties(),
  m_fullMaterial(nullptr)
{}

Trk::HomogeneousLayerMaterial::HomogeneousLayerMaterial( const Trk::MaterialProperties& full,
                                                       double splitFactor) :
  Trk::LayerMaterialProperties(splitFactor),
  m_fullMaterial(full.clone())
{}
                 
Trk::HomogeneousLayerMaterial::HomogeneousLayerMaterial(const Trk::HomogeneousLayerMaterial& lmp) :
  LayerMaterialProperties(lmp.m_splitFactor),
  m_fullMaterial(lmp.m_fullMaterial ? lmp.m_fullMaterial->clone() : nullptr)
{}

Trk::HomogeneousLayerMaterial::~HomogeneousLayerMaterial()
= default;


Trk::HomogeneousLayerMaterial& Trk::HomogeneousLayerMaterial::operator=(const Trk::HomogeneousLayerMaterial& lmp)
{
  if (this!=&lmp) {    
    // now refill everything
    m_fullMaterial.reset( lmp.m_fullMaterial ? lmp.m_fullMaterial->clone() : nullptr);
    Trk::LayerMaterialProperties::m_splitFactor = lmp.m_splitFactor;
  }
  return (*this);
}

Trk::HomogeneousLayerMaterial& Trk::HomogeneousLayerMaterial::operator*=(double scale)
{
   // scale the sub-properties    
   if (m_fullMaterial)  
      (*m_fullMaterial) *= scale;

   return (*this);
}

MsgStream& Trk::HomogeneousLayerMaterial::dump( MsgStream& sl) const
{
  sl << "Trk::HomogeneousLayerMaterial : " << std::endl;
  if (m_fullMaterial) {
       sl << "   - fullMaterial         : " << *m_fullMaterial << std::endl;
  }
  sl << "   - split factor         : " << m_splitFactor << std::endl;

  return sl;
}

std::ostream& Trk::HomogeneousLayerMaterial::dump( std::ostream& sl) const
{
  sl << "Trk::HomogeneousLayerMaterial : " << std::endl;
  if (m_fullMaterial) {
       sl << "   - fullMaterial         : " << *m_fullMaterial << std::endl;
  }
  sl << "   - split factor         : " << m_splitFactor << std::endl; 
  return sl;
}
