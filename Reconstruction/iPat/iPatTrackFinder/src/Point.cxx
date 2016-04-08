/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 SpacePoint with cached quantities computed wrt vertex - allows for offset
 ------------------------------------------------------------------------- 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< 1INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "iPatTrackFinder/Point.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

Point::Point (const InDet::SiCluster&	cluster1,
	      const InDet::SiCluster&	cluster2,
	      IdentifierHash		detectorId1,
	      IdentifierHash		detectorId2,
	      const Amg::Vector3D&	globalPosition,
	      bool			isBarrel,
	      const Amg::Vector3D&	vertex)
    :	m_cluster1		(&cluster1),
	m_cluster2		(&cluster2),
	m_clusterId1		(cluster1.identify()),
	m_clusterId2		(cluster2.identify()),
 	m_detectorId1		(detectorId1),
 	m_detectorId2		(detectorId2),
	m_globalPosition	(globalPosition),
 	m_isBarrel		(isBarrel),
	m_isPixel		(false),
	m_z			(globalPosition.z())
{
    Amg::Vector3D	offset	= globalPosition - vertex;
    m_phi			= offset.phi();
    m_r				= offset.perp();
}

// Special case for pixels.
Point::Point (const InDet::SiCluster&	pixelCluster,
	      IdentifierHash		detectorId,
	      const Amg::Vector3D&	globalPosition,
	      bool			isBarrel,
	      const Amg::Vector3D&	vertex)
    :	m_cluster1		(&pixelCluster),
	m_cluster2		(0),
	m_clusterId1            (pixelCluster.identify()),
	m_clusterId2            (Identifier()),
        m_detectorId1           (detectorId),
	m_detectorId2           (0),
	m_globalPosition	(globalPosition),
 	m_isBarrel		(isBarrel),
	m_isPixel		(true),
	m_z			(globalPosition.z())
{
    Amg::Vector3D	offset	= globalPosition - vertex;
    m_phi			= offset.phi();
    m_r				= offset.perp();
}

// copy constructor
Point::Point (const Point& other)
    :	m_cluster1		(other.m_cluster1),
	m_cluster2		(other.m_cluster2),
	m_clusterId1		(other.m_clusterId1),
        m_clusterId2		(other.m_clusterId2),
        m_detectorId1		(other.m_detectorId1),
        m_detectorId2		(other.m_detectorId2),
	m_globalPosition	(other.m_globalPosition),
	m_isBarrel		(other.m_isBarrel),
	m_isPixel		(other.m_isPixel),
	m_phi			(other.m_phi),
	m_r			(other.m_r),
	m_z			(other.m_z)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
Point::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed)
	      << "SpacePoint: r"<< std::setw(8) << std::setprecision(2)
	      << m_r
	      << "   phi"	<< std::setw(8) << std::setprecision(5)
	      << m_phi
	      << "   z"		<< std::setw(8) << std::setprecision(2)
	      << m_z
	      << "   Id "	<< std::setw(8) 
	      << m_clusterId1	<< " " ;
    if (m_cluster2)
    {
	std::cout << std::setw(8) << m_clusterId2 << " " ;
    }
    else
    {
	std::cout << "       ";
    }
    std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
}
