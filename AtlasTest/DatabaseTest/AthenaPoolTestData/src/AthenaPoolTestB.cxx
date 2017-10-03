/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestB.cxx
 * @brief Implementation of class AthenaPoolTestB
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestB.cxx,v 1.5 2005-01-10 15:50:19 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestB.h"

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

AthenaPoolTestB::AthenaPoolTestB()
    :
    m_fa(7.0),
    m_c(0),
    m_da(13.0),
    m_d(0)
{}

AthenaPoolTestB::AthenaPoolTestB(int c, int d)
    :
    m_fa(7.0),
    m_c(c),
    m_da(13.0),
    m_d(d)
{}
    
AthenaPoolTestB::~AthenaPoolTestB(void)
{}

std::string
AthenaPoolTestB::printWhoYouAre(void) const
{
    std::stringstream stream;
    stream << "AthenaPoolTestB::printWhoYouAre B - c, d " 
	   << m_c << " " << m_d << " fa, da " << m_fa << " " << m_da;
    return (stream.str());
}
