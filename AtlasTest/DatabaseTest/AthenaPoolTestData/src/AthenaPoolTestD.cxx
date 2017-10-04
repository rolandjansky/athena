/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestD.cxx
 * @brief Implementation of class AthenaPoolTestD
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestD.cxx,v 1.3 2005-01-10 15:50:19 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestD.h"

#include "CLHEP/Geometry/Point3D.h"
#include <string>
#include <sstream>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

AthenaPoolTestD::AthenaPoolTestD()
    :
    AthenaPoolTestC(),
    m_e(0),
    m_f(0)
{
}

AthenaPoolTestD::AthenaPoolTestD(int a, int b, int c, int d, int e, int f)
    :
    AthenaPoolTestC(a, b, c, d),
    m_e(e),
    m_f(f)
{
}
    
AthenaPoolTestD::~AthenaPoolTestD(void)
{
}

std::string
AthenaPoolTestD::printWhoYouAre(void) const
{
    std::stringstream stream;
    stream << "AthenaPoolTestD::printWhoYouAre D - e, f " 
	   << m_e << " " << m_f;
    std::string result(stream.str());
    result += " ";
    result += AthenaPoolTestC::printWhoYouAre();
    return (result);
}

