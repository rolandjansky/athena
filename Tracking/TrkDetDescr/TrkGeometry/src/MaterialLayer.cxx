/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/MaterialLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkSurfaces/Surface.h"

// constructor with arguments
Trk::MaterialLayer::MaterialLayer(const Surface& surfaceRepresentation, 
                                  const LayerMaterialProperties& mlprop):
 Trk::Layer(),                                     
 m_surfaceRepresentation(SharedObject<const Surface>(&surfaceRepresentation,true))
{
    m_layerMaterialProperties = mlprop.clone();
    m_layerThickness = 1.;
}

// constructor with arguments
Trk::MaterialLayer::MaterialLayer(const SharedObject<const Surface>& surfaceRepresentation, 
                                  const LayerMaterialProperties& mlprop):
 Trk::Layer(),                                     
 m_surfaceRepresentation(surfaceRepresentation)
{
    m_layerMaterialProperties = mlprop.clone();
    m_layerThickness = 1.;
}

// copy constructor
Trk::MaterialLayer::MaterialLayer(const Trk::MaterialLayer& lay):
 Trk::Layer(lay),
 m_surfaceRepresentation(lay.m_surfaceRepresentation)
{
    m_layerThickness = 1.;
}

// destructor 
Trk::MaterialLayer::~MaterialLayer()
{} 

Trk::MaterialLayer& Trk::MaterialLayer::operator =(const Trk::MaterialLayer& lay)
{
  if (this != &lay){
    Trk::Layer::operator=(lay);
    m_surfaceRepresentation = lay.m_surfaceRepresentation;
  }
  return (*this);
}

bool Trk::MaterialLayer::isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bcheck) const {
  return m_surfaceRepresentation.getPtr()->isOnSurface(gp, bcheck);
}

