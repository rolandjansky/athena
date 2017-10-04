/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestC.cxx
 * @brief Implementation of class AthenaPoolTestC
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestC.cxx,v 1.4 2005-01-10 15:50:19 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestC.h"

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

AthenaPoolTestC::AthenaPoolTestC()
    :
    m_a(0),
    m_b(0),
    m_c(0),
    m_d(0)
{
}

AthenaPoolTestC::AthenaPoolTestC(int a, int b, int c, int d)
    :
    m_a(a),
    m_b(b),
    m_c(c),
    m_d(d)
{
}
    
AthenaPoolTestC::~AthenaPoolTestC(void)
{
}

std::string
AthenaPoolTestC::printWhoYouAre(void) const
{
    std::stringstream stream;
    stream << "AthenaPoolTestC::printWhoYouAre C - a, b, c, d " 
	   << m_a << " " << m_b << " "
	   << m_c << " " << m_d;
    return (stream.str());
}

