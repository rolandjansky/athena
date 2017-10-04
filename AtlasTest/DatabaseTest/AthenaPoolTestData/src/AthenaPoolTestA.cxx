/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestA.cxx
 * @brief Implementation of class AthenaPoolTestA
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestA.cxx,v 1.4 2005-01-10 15:50:19 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestA.h"

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

AthenaPoolTestA::AthenaPoolTestA()
    :
    m_a(0),
    m_b(0)
{
}

AthenaPoolTestA::AthenaPoolTestA(int a, int b)
    :
    m_a(a),
    m_b(b)
{
}
    
AthenaPoolTestA::~AthenaPoolTestA(void)
{
}

std::string
AthenaPoolTestA::printWhoYouAre(void) const
{
    std::stringstream stream;
    stream << "AthenaPoolTestA::printWhoYouAre A - a, b " 
	   << m_a << " " << m_b;
    return (stream.str());
}

