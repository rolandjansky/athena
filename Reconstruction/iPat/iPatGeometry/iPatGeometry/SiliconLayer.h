/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the geometry of a silicon barrel layer or endcap wheel.
 The layer is decomposed into a vector of rings representing phi symmetry.
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONLAYER_H
# define IPATGEOMETRY_SILICONLAYER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <string>
#include <utility>
#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class SiliconRing;

class SiliconLayer
{

public:
    typedef	std::vector<SiliconRing*>      	ring_vector;
    typedef	ring_vector::const_iterator    	ring_const_iterator;

    // constructor with ring in a new layer
    SiliconLayer (SiliconRing*	ring);
    // special inert material constructor
    SiliconLayer (std::string   name,
		  double	rzCentre, 
		  double	zrMin, 
		  double	zrMax);

    ~SiliconLayer (void);
        
    // implicit copy constructor
    // implicit assignment operator

    bool			addRing (SiliconRing*	ring);
    void			determineMaterial (std::string versionName);
    void			determineRzProperties (void);
    bool			isActive (void) const;
    bool			isBarrel (void) const;
    bool			isEndcap (void) const;
    bool			isInactive (void) const;
    bool			isPixel (void) const;
    bool			isStrip (void) const;

    // energy loss coefficient and radiation thickness
    std::pair<double,double>	materialProperties (void) const;
    std::pair<double,double>	materialProperties (const Amg::Vector3D&	intersect,
						    const Amg::Vector3D&	transverseDirection) const;

    // layer name e.g. "pixel", "SCT", "beamPipe" ...
    std::string			name (void) const;
    
    void			print (void) const;
    double			rMax (void) const;
    double			rMin (void) const;
    
    // refresh cached data whenever conditions change
    void			refresh (void);
    
    // returns vector of rz-compatible rings
    ring_vector*		rings (double cotTheta,
				       double r,
				       double z,
				       double zrTolerance) const;
    // note: rz means r in barrel, z in endcap
    //       zr means z in barrel, r in endcap
    double			rzCentre (void) const;
    const Trk::Surface*		surface (bool positiveHemisphere) const;
    double			zMax (void) const;
    double			zMin (void) const;
    double			zrCentre (void) const;
    
    class OutwardsOrder
    {
    public:
	bool		operator() (const SiliconLayer*, const SiliconLayer*) const;
    };
    
private:
    static const double		s_safetyDistance;
    
    bool			m_isActive;
    bool			m_isBarrel;
    bool			m_isPixel;
    bool			m_isStrip;
    std::pair<double,double>	m_materialProperties;
    int				m_multiple;
    std::string   		m_name;
    Trk::Surface*		m_negativeSurface;
    Trk::Surface*		m_positiveSurface;
    double			m_rMax;
    double			m_rMin;
    ring_vector			m_rings;
    double			m_rzCentre;
    double			m_tolerance;
    double			m_zMax;
    double			m_zMin;
    double                      m_zrBegin;
    double                      m_zrCentre;
    double			m_zrEnd;
    double                     	m_zrPeriod;
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
SiliconLayer::isActive (void) const
{ return m_isActive; }

inline bool
SiliconLayer::isBarrel (void) const
{ return m_isBarrel; }

inline bool
SiliconLayer::isEndcap (void) const
{ return !m_isBarrel; }

inline bool
SiliconLayer::isInactive (void) const
{ return !m_isActive; }

inline bool
SiliconLayer::isPixel (void) const
{ return m_isPixel; }

inline bool
SiliconLayer::isStrip (void) const
{ return m_isStrip; }

inline std::pair<double,double>
SiliconLayer::materialProperties (void) const
{ 
    // material precomputed for active layers
    assert(m_isActive);
    return m_materialProperties; 
}

inline std::string
SiliconLayer::name (void) const
{ return m_name; }

inline double
SiliconLayer::rMax (void) const
{ return m_rMax; }

inline double
SiliconLayer::rMin (void) const
{ return m_rMin; }

inline double
SiliconLayer::rzCentre(void) const
{ return m_rzCentre; }

inline const Trk::Surface*
SiliconLayer::surface(bool positiveHemisphere) const
{
    if (positiveHemisphere)
    {
	return m_positiveSurface;
    }
    else
    {
	return m_negativeSurface;
    }
}

inline double
SiliconLayer::zMax (void) const
{ return m_zMax; }

inline double
SiliconLayer::zMin (void) const
{ return m_zMin; }   

inline double
SiliconLayer::zrCentre(void) const
{ return m_zrCentre; }

inline bool
SiliconLayer::OutwardsOrder::operator() (const SiliconLayer* p, const SiliconLayer* q) const
{
    // put pixel layers first
    if (p->isPixel() != q->isPixel()) return p->isPixel();
    
    // otherwise by r (z) in barrel (endcap)
    if (p->isBarrel())
    {
	return (p->rMin() < q->rMin());
    }
    else
    {
	return (p->zMin() < q->zMin());
    }	
}

#endif // IPATGEOMETRY_SILICONLAYER_H

