/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoClearAbsPosAction.h"
#include "GeoModelKernel/GeoVAlignmentStore.h"
#include "GeoSelClearAbsPosAction.h"
#include <set>

GeoAlignableTransform::GeoAlignableTransform (const HepGeom::Transform3D& transform)
  : GeoTransform(transform)
  , m_delta (nullptr)
{
}

GeoAlignableTransform::~GeoAlignableTransform()
{
  delete m_delta;
}

HepGeom::Transform3D GeoAlignableTransform::getTransform(const GeoVAlignmentStore* store) const
{
  const HepGeom::Transform3D* delta = (store==nullptr ? m_delta : store->getDelta(this));
  return GeoTransform::getTransform(nullptr) * (delta==nullptr ? HepGeom::Transform3D() : *delta);
}

void GeoAlignableTransform::setDelta (const HepGeom::Transform3D& delta, GeoVAlignmentStore* store)
{
  if(store==nullptr) {
    if(m_delta && (*m_delta == delta)) return;
    
    if(m_delta) {
      (*m_delta) = delta;
    }
    else {
      m_delta = new HepGeom::Transform3D(delta);
    }

    std::set<GeoGraphNode*> uniqueParents;
    for(GeoGraphNode* parent : m_parentList) {
      if(uniqueParents.find(parent)==uniqueParents.end()) {
	GeoSelClearAbsPosAction cc(this);
	parent->exec(&cc);
	uniqueParents.insert(parent);
      }
    }
  } // if(store==nullptr)
  else {
    store->setDelta(this,delta);
  }
}

void GeoAlignableTransform::clearDelta(GeoVAlignmentStore* store)
{
  // Does not make sence to clear deltas inside Alignment Store
  if(store!=nullptr) return;

  delete m_delta;
  m_delta = nullptr;
  
  std::set<GeoGraphNode*> uniqueParents;
  for(GeoGraphNode* parent : m_parentList) {
    if(uniqueParents.find(parent)==uniqueParents.end()) {
      GeoSelClearAbsPosAction cc(this);
      parent->exec (&cc);
      uniqueParents.insert(parent);
    }
  }
}

void GeoAlignableTransform::dockTo(GeoVPhysVol* parent)
{
  m_parentList.push_back (parent);
}
