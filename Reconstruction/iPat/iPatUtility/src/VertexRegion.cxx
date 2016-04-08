/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************
 Vertex region data-class
 
*****************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatUtility/VertexRegion.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

VertexRegion::VertexRegion (void)
    :	m_centre	(Amg::Vector3D(0.,0.,0.)),
	m_width		(Amg::Vector3D(0.,0.,0.))
{}

VertexRegion::VertexRegion (const Amg::Vector3D& centre,
			    const Amg::Vector3D& width)
    :	m_centre	(centre),
	m_width		(width)
{}

VertexRegion::VertexRegion (const VertexRegion& vertex)
    :	m_centre	(vertex.m_centre),
	m_width		(vertex.m_width)
{}

VertexRegion::~VertexRegion (void)
{}
