/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 PerigeeParameters describes 5 track parameters with their covariance
 matrix in the perigree representation
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "AthenaKernel/Units.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
PerigeeParameters::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed | std::ios::right)
	      << "  a0,z0 "
	      << std::setw(6) << std::setprecision(2) << m_transverseImpact
	      << std::setw(8) << std::setprecision(2) << m_position.z()
	      << "  phi,dzdr "
	      << std::setw(7) << std::setprecision(4) << std::atan2(m_sinPhi,m_cosPhi)
	      << std::setw(8) << std::setprecision(4) << m_cotTheta
	      << "  pt"
	      << std::setw(8) << std::setprecision(2) << pt()*Athena::Units::GeV
	      << std::resetiosflags(std::ios::fixed);
}

