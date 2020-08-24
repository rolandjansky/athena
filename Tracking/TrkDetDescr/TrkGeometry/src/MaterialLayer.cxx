/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 //m_surfaceRepresentation(SharedObject<const Surface>(&surfaceRepresentation,true))
 
 /* 
  * The above line was setting the not delete ndel to true
  * do the same with no-op deleter for shared_ptr
  * Probably ownership might need some clean up here 
  */
m_surfaceRepresentation(SharedObject<const Surface>(&surfaceRepresentation,
                                                    do_not_delete<const Surface>))
{
    m_layerMaterialProperties.reset(mlprop.clone());
    m_layerThickness = 1.;
}

// constructor with arguments
Trk::MaterialLayer::MaterialLayer(const SharedObject<const Surface>& surfaceRepresentation, 
                                  const LayerMaterialProperties& mlprop):
 Trk::Layer(),                                     
 m_surfaceRepresentation(surfaceRepresentation)
{
    m_layerMaterialProperties.reset(mlprop.clone());
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
= default; 

Trk::MaterialLayer& Trk::MaterialLayer::operator =(const Trk::MaterialLayer& lay)
{
  if (this != &lay){
    Trk::Layer::operator=(lay);
    m_surfaceRepresentation = lay.m_surfaceRepresentation;
  }
  return (*this);
}

bool Trk::MaterialLayer::isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bcheck) const {
  return m_surfaceRepresentation.get()->isOnSurface(gp, bcheck);
}

