/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ScattererParameters are the value of the TrackParameters at a given 
 scattering plane, along with the local scattering angles and errors
 -------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "iPatTrackParameters/ScattererParameters.h"

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
ScattererParameters::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed)
	      << "  r,z "
	      << std::setw(8) << std::setprecision(3) << m_position.perp()
	      << std::setw(8) << std::setprecision(3) << m_position.z()
	      << "  phi,dzdr "
	      << std::setw(7) << std::setprecision(4) << phi()
	      << std::setw(8) << std::setprecision(4) << m_cotTheta
	      << "  pt"
	      << std::setw(8) << std::setprecision(2) << pt()
	      << std::resetiosflags(std::ios::fixed);
}

