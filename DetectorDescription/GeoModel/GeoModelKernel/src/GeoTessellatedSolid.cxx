/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoTessellatedSolid::s_classType = "TessellatedSolid";
const ShapeType GeoTessellatedSolid::s_classTypeID = 0x21;

GeoTessellatedSolid::GeoTessellatedSolid()
{
}

GeoTessellatedSolid::~GeoTessellatedSolid()
{
  for(size_t i=0; i<m_facets.size(); ++i)
    m_facets[i]->unref();
}

double GeoTessellatedSolid::volume() const
{
  // -- ToDo
  return 0.;
}

const std::string& GeoTessellatedSolid::type() const
{
  return s_classType;
}

ShapeType GeoTessellatedSolid::typeID() const
{
  return s_classTypeID;
}

void GeoTessellatedSolid::exec(GeoShapeAction *action) const
{
  action->handleTessellatedSolid(this);
}

void GeoTessellatedSolid::addFacet(GeoFacet* facet)
{
  facet->ref();
  m_facets.push_back(facet);
}
  
GeoFacet* GeoTessellatedSolid::getFacet(size_t index) const
{
  return (index<m_facets.size() ? m_facets[index] : 0);
}

size_t GeoTessellatedSolid::getNumberOfFacets() const
{
  return m_facets.size();
}
