/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the rectangular shape of pixel and barrel SCT detector elements.
 Allows a possible bond gap at the eta centre of the detector.
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONRECTANGLE_H
# define IPATGEOMETRY_SILICONRECTANGLE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "iPatGeometry/DetectorShape.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class SiliconRectangle:public DetectorShape
{

public:
    // constructor with pixel parameters
    SiliconRectangle (double localEtaWidth,
		      double localPhiWidth);
    // constructor with SCT parameters
    SiliconRectangle (double localEtaWidth,
		      double localPhiWidth,
		      double gapWidth);
    
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
    double		m_gapWidth;
    double		m_localEtaWidth;
    double		m_localPhiWidth;
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
SiliconRectangle::insideEta (double localEta) const
{ return (std::abs(localEta) < m_localEtaWidth + s_boundaryTolerance); }

inline double
SiliconRectangle::localEtaWidth (void) const
{ return m_localEtaWidth; }

inline double
SiliconRectangle::localPhiWidth (void) const
{ return m_localPhiWidth; }

inline bool
SiliconRectangle::sameShape (const DetectorShape& shape) const
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

#endif // IPATGEOMETRY_SILICONRECTANGLE_H

