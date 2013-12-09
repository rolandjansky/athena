/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the rectangular shape of pixel and barrel SCT detector elements.
 Allows a possible bond gap at the eta centre of the detector.
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatGeometry/SiliconRectangle.h"

//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>

// 5mm tolerance used for crude checks around detector boundary
const double    DetectorShape::s_boundaryTolerance	= 5.0*Gaudi::Units::mm;

// 0.1 micron tolerance used for shape comparison
const double    DetectorShape::s_shapeTolerance       	= 0.1*Gaudi::Units::micrometer;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconRectangle::SiliconRectangle(double localEtaWidth,
				   double localPhiWidth)
    :	m_gap			(false),
	m_gapWidth		(0.),
	m_localEtaWidth      	(localEtaWidth),
	m_localPhiWidth      	(localPhiWidth)
{}

SiliconRectangle::SiliconRectangle(double localEtaWidth,
				   double localPhiWidth,
				   double gapWidth)                   
    :   m_gap			(true),
	m_gapWidth		(gapWidth),
	m_localEtaWidth      	(localEtaWidth),
	m_localPhiWidth      	(localPhiWidth)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

DetectorIntersect*
SiliconRectangle::intersect (double localEta,
			     double localPhi,
			     double rzTol,
			     double rphiTol,
			     double absStereo) const
{
    double absPhi = std::abs(localPhi);
    if (absPhi	> m_localPhiWidth + s_boundaryTolerance) return 0;
    double absEta = std::abs(localEta);
    double etaTol = rzTol + absStereo*rphiTol;
    if (absPhi		> m_localPhiWidth
	|| absEta	> m_localEtaWidth)
    {
	// use a conservative approx in boundary region
	double phiTol = rphiTol + absStereo*rzTol;
	if (absPhi    	> m_localPhiWidth + phiTol
	    || absEta	> m_localEtaWidth + etaTol)
	{
	    // no intersect
	    return 0;
	}
	else
	{
	    // intersect near outer (perimeter) boundary
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi/m_localPhiWidth,
					  true));
	}
    }
    else
    {
	// similar approx in bond-gap region
	if (m_gap
	    && absEta	< m_gapWidth + rzTol)
	{
	    // intersect in or near gap
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi/m_localPhiWidth,
					  true));
	}
	else
	{
	    // intersect in sensitive region
	    return (new DetectorIntersect(localEta/m_localEtaWidth,
					  localPhi/m_localPhiWidth,
					  false));
	}
    }
}

void
SiliconRectangle::print(void) const
{
    std::cout << " SiliconRectangle: " << std::setiosflags(std::ios::fixed)
	      << " localEtaWidth "
	      << std::setw(7) << std::setprecision(2) << m_localEtaWidth
	      << "   localPhiWidth "
	      << std::setw(7) << std::setprecision(2) << m_localPhiWidth;
    if (m_gap)
    {
	std::cout << "   bond-gap: localEta " << std::setiosflags(std::ios::fixed) 
		  << std::setw(5) << std::setprecision(1) << -m_gapWidth << " ->"
		  << std::setw(5) << std::setprecision(1) << m_gapWidth;

    }
    std::cout << std::endl;
}
