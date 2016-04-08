/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient object giving details of a (line) intersect with a DetectorElement
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "iPatGeometry/DetectorIntersect.h"

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
DetectorIntersect::print(void) const
{
    if (m_hasIntersect)
    {
	if (m_nearBoundary)
	{
	    std::cout << " DetectorIntersect:   nearBoundary";
	}
	else
	{
	    std::cout << " DetectorIntersect: insideDetector";
	}
	std::cout << std::setiosflags(std::ios::fixed) << "   r,z"
		  << std::setw(8) << std::setprecision(3) << m_position.perp()
		  << std::setw(10) << std::setprecision(3) << m_position.z()
		  << "   local eta,phi "
		  << std::setw(8) << std::setprecision(3) << m_localEta 
		  << std::setw(10) << std::setprecision(3) << m_localPhi
		  << std::endl;
    }
    else
    {
	std::cout << " DetectorIntersect: outsideDetector";
    }
}
