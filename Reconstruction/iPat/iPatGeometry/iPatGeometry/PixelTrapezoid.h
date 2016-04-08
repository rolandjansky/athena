/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the non-projective trapezoidal shape of endcap pixel detector
 elements (with one corner 'clipped').
 ***************************************************************************/

#ifndef IPATGEOMETRY_PIXELTRAPEZOID_H
# define IPATGEOMETRY_PIXELTRAPEZOID_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatGeometry/DetectorShape.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PixelTrapezoid:public DetectorShape
{

public:
    // constructor
    PixelTrapezoid (double localEtaWidth,
		    double localPhiMax,
		    double localPhiMin);
    
    // implicit copy constructor
    // implicit destructor
    // implicit assignment operator

    DetectorIntersect*	intersect(double localEta,
				  double localPhi,
				  double rzTol,
				  double rphiTol,
				  double absStereo) const;
    bool		insideEta (double localEta) const;
    double      	localEtaWidth (void) const;
    double      	localPhiWidth (void) const;
    void		print (void) const;
    bool		sameShape (const DetectorShape& shape) const;
    
private:
    double		m_localEtaWidth;
    double		m_localPhiMax;
    double		m_localPhiMin;
    double		m_localPhiSlope;
    double		m_localPhiWidth;
};
//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
PixelTrapezoid::insideEta (double localEta) const
{ return (fabs(localEta) < m_localEtaWidth + s_boundaryTolerance); }

inline double
PixelTrapezoid::localEtaWidth (void) const
{ return m_localEtaWidth; }

inline double
PixelTrapezoid::localPhiWidth (void) const
{ return m_localPhiWidth; }

inline bool
PixelTrapezoid::sameShape (const DetectorShape& shape) const
{
    // sensitive areas equal
    if (fabs(m_localEtaWidth - shape.localEtaWidth())		< s_shapeTolerance
	&& fabs(m_localPhiWidth - shape.localPhiWidth())	< s_shapeTolerance)
    {
	return true;
    }
    else
    {
	return false;
    }
}

#endif // IPATGEOMETRY_PIXELTRAPEZOID_H

