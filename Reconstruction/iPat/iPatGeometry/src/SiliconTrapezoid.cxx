/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the trapezoidal shape of endcap SCT detector elements.
 Allows a possible bond gap near the eta centre of the detector.
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "iPatGeometry/SiliconTrapezoid.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>


SiliconTrapezoid::SiliconTrapezoid(double localEtaWidth,
				   double localPhiWidth,
				   double rCentre)                   
    :	m_gap			(false),
	m_gapMax	        (0.),
	m_gapMin		(0.),
	m_localEtaWidth      	(localEtaWidth),
	m_localPhiWidth      	(localPhiWidth),
	m_rCentre		(rCentre)
{}

SiliconTrapezoid::SiliconTrapezoid(double localEtaWidth,
				   double localPhiWidth,
				   double rCentre,
				   double gapMin,
				   double gapMax)
    :   m_gap			(true),
	m_gapMax	        (gapMax),
	m_gapMin		(gapMin),
	m_localEtaWidth      	(localEtaWidth),
	m_localPhiWidth      	(localPhiWidth),
	m_rCentre		(rCentre)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

DetectorIntersect*
SiliconTrapezoid::intersect (double localEta,
			     double localPhi,
			     double rzTol,
			     double rphiTol,
			     double absStereo) const
{
    // projective phi-shape
    double projection	= m_rCentre/(m_rCentre + localEta);
    double absPhi	= std::abs(localPhi)*projection;
    if (absPhi	       	> m_localPhiWidth + s_boundaryTolerance) return 0;
    
    double absEta	= std::abs(localEta);
    double etaTol	= rzTol + absStereo*rphiTol;
    if (absEta		> m_localEtaWidth
	|| absPhi     	> m_localPhiWidth)
    {
	// use a conservative approx in boundary region
	double phiTol = rphiTol + absStereo*rzTol;
	if (absEta	> m_localEtaWidth + etaTol
	    || absPhi 	> m_localPhiWidth + phiTol)
        {
	    // no intersect
	    return 0;
	}
	else
	{
	    // intersect near outer (perimeter) boundary
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi*projection/m_localPhiWidth,
					  true));
	}
    }
    else
    {
	if (m_gap
	    && localEta	< m_gapMax + etaTol
	    && localEta	> m_gapMin - etaTol)
	{
	    // intersect in or near gap
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi*projection/m_localPhiWidth,
					  true));
	}
	else
	{
	    // intersect in sensitive region
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi*projection/m_localPhiWidth,
					  false));
	}
    }
}
	    
void
SiliconTrapezoid::print(void) const
{
    std::cout << " SiliconTrapezoid: " << std::setiosflags(std::ios::fixed)
	      << " localEtaWidth "
	      << std::setw(7) << std::setprecision(2) << m_localEtaWidth 
	      << "   localPhiWidth "
	      << std::setw(7) << std::setprecision(2) << m_localPhiWidth;
    if (m_gap)
    {
	std::cout << "   bond-gap: localEta " << std::setiosflags(std::ios::fixed) 
		  << std::setw(5) << std::setprecision(1) << m_gapMin << " ->"
		  << std::setw(5) << std::setprecision(1) << m_gapMax;
    }
    std::cout << std::endl;
}
