/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NavigationLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/NavigationLayer.h"
#include "TrkSurfaces/Surface.h"

// constructor with arguments
Trk::NavigationLayer::NavigationLayer(Trk::Surface* surfaceRepresentation, 
                                      Trk::Layer* previous,
                                      Trk::Layer* next,
                                      Trk::BinUtility* binUtil):
 Trk::Layer(),                                     
 m_surfaceRepresentation(surfaceRepresentation)
{
  Trk::Layer::m_previousLayer = previous;
  Trk::Layer::m_nextLayer     = next;
  Trk::Layer::m_binUtility    = binUtil;
  Trk::Layer::m_layerType     = Trk::passive;
}


// constructor with arguments
Trk::NavigationLayer::NavigationLayer(Trk::Surface* surfaceRepresentation, 
                                      double thickness):
 Trk::Layer(),                                     
 m_surfaceRepresentation(surfaceRepresentation)
{
  Trk::Layer::m_layerThickness = thickness;
}

// copy constructor
Trk::NavigationLayer::NavigationLayer(const Trk::NavigationLayer& lay):
 Trk::Layer(lay),
 m_surfaceRepresentation(lay.m_surfaceRepresentation->clone())
{
  Trk::Layer::m_previousLayer = lay.m_previousLayer;
  Trk::Layer::m_nextLayer     = lay.m_nextLayer;
  Trk::Layer::m_binUtility    = lay.m_binUtility;
}

// destructor - only deletes surface representation
Trk::NavigationLayer::~NavigationLayer()
{ delete m_surfaceRepresentation; } 

Trk::NavigationLayer& Trk::NavigationLayer::operator =(const Trk::NavigationLayer& lay)
{
 if (this != &lay){
  Trk::Layer::operator=(lay);
  delete m_surfaceRepresentation;
  m_surfaceRepresentation     = lay.m_surfaceRepresentation->clone();
 }
 return (*this);
}

bool Trk::NavigationLayer::isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bcheck) const {
  return m_surfaceRepresentation->isOnSurface(gp, bcheck);
}

