/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoFacet.h"

// ___________________ Triangular Facet ________________________
GeoTriangularFacet::GeoTriangularFacet(GeoFacetVertex v0
				       ,GeoFacetVertex v1
				       ,GeoFacetVertex v2
				       ,GeoFacetVertexType type) 
  : GeoFacet()
{
  m_nVertices = 3;
  m_vertexType = type;
  m_vertices.reserve(3);
  m_vertices[0] = v0;
  m_vertices[1] = v1;
  m_vertices[2] = v2;
}


GeoTriangularFacet::~GeoTriangularFacet()
{
}

// ___________________ Triangular Facet ________________________
GeoQuadrangularFacet::GeoQuadrangularFacet(GeoFacetVertex v0
					   ,GeoFacetVertex v1
					   ,GeoFacetVertex v2
					   ,GeoFacetVertex v3
					   ,GeoFacetVertexType type) 
  : GeoFacet()
{
  m_nVertices = 4;
  m_vertexType = type;
  m_vertices.reserve(4);
  m_vertices[0] = v0;
  m_vertices[1] = v1;
  m_vertices[2] = v2;
  m_vertices[3] = v3;
}


GeoQuadrangularFacet::~GeoQuadrangularFacet()
{
}
