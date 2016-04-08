/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the geometry of a ring of silicon detector elements with similar
 properties (phi symmetric - forming part of a barrel layer or endcap wheel).
 By definition, a ring is formed for each phi sector parity to ensure that
 a track can only intersect with (at most) one detector per ring.
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONRING_H
# define IPATGEOMETRY_SILICONRING_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class DetectorShape;
class SiliconDetector;
class SiliconLayer;

class SiliconRing
{

public:
    // constructor with detector in a new ring
    SiliconRing (SiliconDetector*	detector,
		 const DetectorShape*	m_shape);
    // constructor for copyWithParity
    SiliconRing (const SiliconRing&	ring,
		 bool			parity);
    ~SiliconRing (void);
    
    // unconditional adding of a detector to the ring
    void					addDetector (SiliconDetector*	detector);
    // conditional add
    bool					addDetector (SiliconDetector*		detector,
							     const DetectorShape*	m_shape);
    SiliconRing*				copyWithParity (bool parity) const;
    const SiliconDetector*			detector (double phi) const;
    const std::vector<SiliconDetector*>&	detectors (void) const;
    void					determinePhiProperties (void);
    // a ring has undefined phi_parity if it is phi-hermetic (i.e. has phi-overlapping detectors).
    // It should be split into 2 rings with alternating detectors (a ring of each parity)
    bool					hasPhiParity (void) const;
    bool					isBarrel (void) const;
    bool					isPixel (void) const;
    bool					isStereo (void) const;
    const SiliconLayer&				layer (void) const;	// parent layer
    // even parity (boolean true) is defined as a ring containing the detector
    // straddling the +-PI boundary 
    bool					parity (void) const;
    void					print (void) const;
    double					rMax (void) const;
    double					rMin (void) const;
    void					refresh (void);    
    // note: rz means r in barrel, z in endcap
    //       zr means z in barrel, r in endcap
    double					rzCentre (void) const;
    void                        		setParent (const SiliconLayer* layer);
    const DetectorShape&       			shape (void) const;
    double					zMax (void) const;
    double					zMin (void) const;
    double					zrCentre (void) const;

    class rzOrder
    {
    public:
	bool		operator() (const SiliconRing*, const SiliconRing*) const;
    };

    class outwardsOrder
    {
    public:
	bool            operator() (const SiliconRing*, const SiliconRing*) const;
    };

private:
    static const double			s_shapeTolerance;
    static const double			s_sortTolerance;
    static const double			s_stereoTolerance;
    
    std::vector<SiliconDetector*>	m_detectors;
    bool				m_isPixel;
    bool				m_isStereo;
    const SiliconLayer*			m_parent;
    bool				m_parity;
    double				m_phiOffset;
    double				m_phiPeriod;
    double				m_rMax;
    double				m_rMin;
    const DetectorShape*	       	m_shape;
    double				m_sinStereo;
    double				m_zMax;
    double				m_zMin;

    // void and copy constructors, assignment: no semantics, no implementation
    SiliconRing (void);
    SiliconRing (const SiliconRing&);
    SiliconRing &operator= (const SiliconRing&);
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
SiliconRing::isBarrel(void) const
{ return (m_rMax-m_rMin < m_zMax-m_zMin); }

inline bool
SiliconRing::isPixel(void) const
{ return m_isPixel; }

inline bool
SiliconRing::isStereo(void) const
{ return m_isStereo; }

inline const SiliconLayer&
SiliconRing::layer (void) const
{ return *m_parent; }
    
inline bool
SiliconRing::parity(void) const
{ return m_parity; }

inline double
SiliconRing::rMax(void) const
{ return m_rMax; }

inline double
SiliconRing::rMin(void) const
{ return m_rMin; }

inline double
SiliconRing::rzCentre(void) const
{
    if (m_rMax-m_rMin < m_zMax-m_zMin)
    {
	return 0.5*(m_rMin + m_rMax);		// r in barrel
    }
    else
    {
	return 0.5*(m_zMin + m_zMax);		// z in endcap
    }
}

inline void
SiliconRing::setParent (const SiliconLayer* layer)
{ m_parent = layer; }

inline const DetectorShape&
SiliconRing::shape (void) const
{ return *m_shape; }

inline double
SiliconRing::zMax(void) const
{ return m_zMax; }

inline double
SiliconRing::zMin(void) const
{ return m_zMin; }   

inline double
SiliconRing::zrCentre(void) const
{
    if (m_rMax-m_rMin < m_zMax-m_zMin)
    {
	return 0.5*(m_zMin + m_zMax);		// z in barrel
    }
    else
    {
	return 0.5*(m_rMin + m_rMax);		// r in endcap
    }
}

inline bool
SiliconRing::rzOrder::operator() (const SiliconRing* p, const SiliconRing* q) const
{
    // order by minimum z (r) in barrel (endcap)
    // with -ve endcap preceding +ve (irrespective of r)
    if (p->isBarrel())
    {
	if (fabs(p->zMin() - q->zMin()) > s_sortTolerance)
	{
	    return (p->zMin() < q->zMin());
	}
	else
	{
	    return (p->rMin() < q->rMin());
	}
    }
    else
    {
	if (p->zMin()*q->zMin() < 0.)
	{
	    return (p->zMin() < 0.);
	}
	else if (fabs(p->rMin() - q->rMin()) > s_sortTolerance)
	{
	    return (p->rMin() < q->rMin());
	}
	else
	{
	    return (fabs(p->zMin()) < fabs(q->zMin()));
	}
    }	
}

inline bool
SiliconRing::outwardsOrder::operator() (const SiliconRing* p, const SiliconRing* q) const
{
    // firstly order pixel before SCT, barrel before endcap
    if (p->isPixel() != q->isPixel())
    {
	return p->isPixel();
    }
    else if (p->isBarrel() != q->isBarrel())
    {
	return p->isBarrel();
    }

    // then order by r (abs(z)) in barrel (endcap)
    if (p->isBarrel())
    {
	if (std::abs(p->rMin() - q->rMin()) > s_sortTolerance)
	{
	    return (p->rMin() < q->rMin());
	}
	else
	{
	    return (p->zMin() < q->zMin());
	}
    }
    else
    {
	if (std::abs(std::abs(p->zMin()) - std::abs(q->zMin())) > s_sortTolerance)
	{
	    return (std::abs(p->zMin()) < std::abs(q->zMin()));
	}
	else	
	{
	    return (p->rMin() < q->rMin());
	}
    }
}

#endif // IPATGEOMETRY_SILICONRING_H

