/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the trapezoidal shape of endcap SCT detector elements.
 Allows for a possible bond gap between two local-eta values.
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONTRAPEZOID_H
# define IPATGEOMETRY_SILICONTRAPEZOID_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "iPatGeometry/DetectorShape.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class SiliconTrapezoid:public DetectorShape
{

public:
    // constructor without bond gap
    SiliconTrapezoid (double localEtaWidth,
		      double localPhiWidth,
		      double rCentre);
    // constructor with bond gap
    SiliconTrapezoid (double localEtaWidth,
		      double localPhiWidth,
		      double rCentre,
		      double gapMin,
		      double gapMax);

    // implicit copy constructor
    // implicit destructor
    // implicit assignment operator

    bool		insideEta (double localEta) const;
    DetectorIntersect*	intersect(double localEta,
				  double localPhi,
				  double rzTol,
				  double rphiTol,
				  double absStereo) const;
    double      	localEtaWidth (void) const;
    double      	localPhiWidth (void) const;
    void		print (void) const;
    bool		sameShape (const DetectorShape& shape) const;
    
private:
    bool		m_gap;
    double              m_gapMax;
    double              m_gapMin;
    double		m_localEtaWidth;
    double		m_localPhiWidth;
    double		m_rCentre;
};
//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
SiliconTrapezoid::insideEta (double localEta) const
{ return (std::abs(localEta) < m_localEtaWidth + s_boundaryTolerance); }

inline double
SiliconTrapezoid::localEtaWidth (void) const
{ return m_localEtaWidth; }

inline double
SiliconTrapezoid::localPhiWidth (void) const
{ return m_localPhiWidth; }

inline bool
SiliconTrapezoid::sameShape (const DetectorShape& shape) const
{
    // sensitive areas equal
    if (std::abs(m_localEtaWidth - shape.localEtaWidth())	< s_shapeTolerance
	&& std::abs(m_localPhiWidth - shape.localPhiWidth())	< s_shapeTolerance)
    {
	return true;
    }
    else
    {
	return false;
    }
}

#endif // IPATGEOMETRY_SILICONTRAPEZOID_H

