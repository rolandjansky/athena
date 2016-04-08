/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the non-projective trapezoidal shape of endcap pixel detector
 elements (with one corner 'clipped').
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "iPatGeometry/PixelTrapezoid.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

PixelTrapezoid::PixelTrapezoid(double localEtaWidth,
			       double localPhiMax,
			       double localPhiMin)
    :   m_localEtaWidth      	(localEtaWidth),
	m_localPhiMax      	(localPhiMax),
	m_localPhiMin      	(localPhiMin),
	m_localPhiWidth      	(-localPhiMin)
{
    // have to hard-wire phi-width at bottom
    // (full-width at localEta = 0)/full-bottom-width)/(localEta at bottom)
    m_localPhiSlope = (-2.*localPhiMin-0.720)/localEtaWidth;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

DetectorIntersect*
PixelTrapezoid::intersect(double localEta,
			  double localPhi,
			  double rzTol,
			  double rphiTol,
			  double absStereo) const
{
    // crude check using maximum phi coverage
    if (localPhi	<  m_localPhiMin - s_boundaryTolerance
	|| localPhi	>  m_localPhiMax + s_boundaryTolerance) 
	return 0;

    // apply specific trapezoidal shape
    double etaTol		= rzTol + absStereo*rphiTol;
    double phiTol		= rphiTol + absStereo*rzTol;
    double localPhiMax	= m_localPhiSlope*localEta - m_localPhiMin;
    double normalisedEta      	= localEta/m_localEtaWidth;
    double normalisedPhi      	= -1. +2.*(localPhi - m_localPhiMin) /
				  (localPhiMax - m_localPhiMin);
    
    if (std::abs(normalisedEta)   	< 1.0
	&& std::abs(normalisedPhi)	< 1.0)
    {
	return (new DetectorIntersect(normalisedEta,normalisedPhi,false));
    }
    else if (std::abs(localEta)	> m_localEtaWidth + etaTol
	     || localPhi	<  m_localPhiMin - phiTol
	     || localPhi	>  localPhiMax + phiTol)
    {
	return 0;
    }
    else
    {
	// near boundary
	return (new DetectorIntersect(normalisedEta,normalisedPhi,true));
    }
}
	    
void
PixelTrapezoid::print(void) const
{
    std::cout << " PixelTrapezoid: " << std::setiosflags(std::ios::fixed)
	      << " localEtaWidth "
	      << std::setw(7) << std::setprecision(3) << m_localEtaWidth 
	      << "   localPhiMin "
	      << std::setw(7) << std::setprecision(3) << m_localPhiMin
	      << "   localPhiMax "
	      << std::setw(7) << std::setprecision(3) << m_localPhiMax
	      << "   localPhiSlope "
	      << std::setw(7) << std::setprecision(3) << m_localPhiSlope
	      << std::endl;
}
