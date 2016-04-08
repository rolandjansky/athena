/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient object giving details of a (line) intersect with a DetectorElement
 ***************************************************************************/

#ifndef IPATGEOMETRY_DETECTORINTERSECT_H
# define IPATGEOMETRY_DETECTORINTERSECT_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GeoPrimitives/GeoPrimitives.h"

class DetectorIntersect
{

public:
    // constructor (no intersect)
    DetectorIntersect(void);
    // constructor (intersect on or near DetectorElement)
    DetectorIntersect(double	localEta,
		      double	localPhi,
		      bool      nearBoundary);
    // destructor
    ~DetectorIntersect(void);
    // forbidden copy constructor
    // forbidden assignment operator

    // has a valid intersect
    bool			hasIntersect (void) const;
    // local eta,phi- normalised st range (-1,+1) lies within detector
    double			localEta (void) const;
    double			localPhi (void) const;
    // intersect near detector boundary
    bool			nearBoundary (void) const;
    // intersect position (get/set)
    const Amg::Vector3D&	position (void) const;
    void			position (const Amg::Vector3D&);
    // print for debugging
    void			print (void) const;
    // radius for ordering
    double			radius (void) const;
    
private:
    bool			m_hasIntersect;
    double			m_localEta;
    double			m_localPhi;
    bool			m_nearBoundary;
    Amg::Vector3D	        m_position;
    double			m_radius;
    
    // copy, assignment: no semantics, no implementation
    DetectorIntersect (const DetectorIntersect&);

    DetectorIntersect &operator= (const DetectorIntersect&);
    
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
DetectorIntersect::DetectorIntersect(void)
    :	m_hasIntersect		(false)
{}

inline
DetectorIntersect::DetectorIntersect(double    	localEta,
				     double	localPhi,
				     bool	nearBoundary)
    :	m_hasIntersect		(true),
	m_localEta		(localEta),
	m_localPhi		(localPhi),
	m_nearBoundary		(nearBoundary),
	m_radius		(0.)
{}

inline
DetectorIntersect::~DetectorIntersect(void)
{}

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
DetectorIntersect::hasIntersect (void) const
{ return m_hasIntersect; }

inline double
DetectorIntersect::localEta (void) const
{ return m_localEta; }

inline double
DetectorIntersect::localPhi (void) const
{ return m_localPhi; }

inline bool
DetectorIntersect::nearBoundary (void) const
{ return m_nearBoundary; }

inline const Amg::Vector3D&
DetectorIntersect::position (void) const
{ return m_position; }

inline void
DetectorIntersect::position (const Amg::Vector3D& pos)
{
    m_position	= pos;
    m_radius	= pos.perp();
}

inline double
DetectorIntersect::radius (void) const
{ return m_radius; }

#endif // IPATGEOMETRY_DETECTORINTERSECT_H

