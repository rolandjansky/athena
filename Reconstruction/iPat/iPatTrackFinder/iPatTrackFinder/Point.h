/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 SpacePoint with cached quantities computed wrt vertex - allows for offset
 -------------------------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATTRACKFINDER_POINT_H
# define IPATTRACKFINDER_POINT_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "EventPrimitives/EventPrimitives.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/SiCluster.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class Point
{
public:
    // constructor for SCT
    Point (const InDet::SiCluster&	cluster1,
	   const InDet::SiCluster&	cluster2,
	   IdentifierHash		detectorId1,
	   IdentifierHash		detectorId2,
	   const Amg::Vector3D&		globalPosition,
	   bool				isBarrel,
	   const Amg::Vector3D&		vertex); 
    // constructor for pixels
    Point (const InDet::SiCluster&	pixelCluster,
	   IdentifierHash		detectorId,
	   const Amg::Vector3D&		globalPosition,
	   bool				isBarrel,
	   const Amg::Vector3D&		vertex);
    // copy constructor
    Point (const Point& other);

    // makes point copy with phi changed by adding 2 PI
    Point*			copyAndWrapPhi (void) const;
    const InDet::SiCluster*     cluster1 (void) const;
    const InDet::SiCluster*     cluster2 (void) const;
    Identifier                  clusterId1 (void) const;
    Identifier                  clusterId2 (void) const;
    IdentifierHash		detectorId1 (void) const;
    IdentifierHash		detectorId2 (void) const;
    const Amg::Vector3D&	globalPosition (void) const;
    bool			inBarrel (void) const;
    bool			isPixel (void) const;
    double			phi (void) const;
    double			r (void) const;                    
    double			z (void) const;  
    // for debug
    void			print(void) const;
    
private:
    const InDet::SiCluster*	m_cluster1;
    const InDet::SiCluster*	m_cluster2;    		// Zero if this is a pixel
    Identifier                  m_clusterId1;
    Identifier                  m_clusterId2;
    int				m_detectorId1;		
    int				m_detectorId2;		// Zero if this is a pixel
    Amg::Vector3D		m_globalPosition;
    bool			m_isBarrel;
    bool			m_isPixel;
    double			m_phi;
    double			m_r;                    // These quantities are 
    double			m_z;  			// cached for efficiency,

    // void constructor and assignment operator: no semantics, no implementation
    Point (void);
    Point &operator= (const Point&);
  
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline Point*
Point::copyAndWrapPhi (void) const
{
    Point*	point	=  new Point(*this);
    point->m_phi	+= 2.0*M_PI;
    return point;
}

inline const InDet::SiCluster*
Point::cluster1 (void) const
{ return m_cluster1; }

inline const InDet::SiCluster*
Point::cluster2 (void) const
{ return m_cluster2; }

inline Identifier
Point::clusterId1 (void) const
{ return m_clusterId1; }

inline Identifier
Point::clusterId2 (void) const
{ return m_clusterId2; }

inline IdentifierHash
Point::detectorId1 (void) const
{ return m_detectorId1; }

inline IdentifierHash
Point::detectorId2 (void) const
{ return m_detectorId2; }

inline const Amg::Vector3D&
Point::globalPosition (void) const
{ return m_globalPosition; }

inline bool
Point::inBarrel (void) const
{ return m_isBarrel; }

inline bool
Point::isPixel (void) const
{ return m_isPixel; }

inline double
Point::phi (void) const
{ return m_phi; }

inline double 
Point::r (void) const
{ return m_r; }

inline double
Point::z (void) const
{ return m_z; }

#endif // IPATTRACKFINDER_POINT_H
