/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestElemLink.cxx
 * @brief Implementation of class AthenaPoolTestElemLink
 */
/***************************************************************************
 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestElemLink.cxx,v 1.2 2005-01-10 15:50:19 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestElemLink.h"
//#include "TrkTrack/Track.h"

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

AthenaPoolTestElemLink::AthenaPoolTestElemLink()
{
}

AthenaPoolTestElemLink::AthenaPoolTestElemLink(const ElementLink<DataVector<Trk::Track> >& track)
	:
	m_track(track),
	m_test(123)
{
}
    
AthenaPoolTestElemLink::~AthenaPoolTestElemLink (void)
{
}

std::string
AthenaPoolTestElemLink::printWhoYouAre   (void) const
{
    std::stringstream stream;
//    stream << "AthenaPoolTestElemLink::printWhoYouAre track, index, cachedElement - NO ACCESS TO DATA. test " << m_test;
    stream << "AthenaPoolTestElemLink::printWhoYouAre track, index, cachedElement " 
  	   << m_test << " " << m_track.index() << " " << m_track.cachedElement();
    stream << " and then, cptr, index, cachedElement " 
  	   << m_track.cptr() << " " << m_track.index() << " " << m_track.cachedElement();
    stream << " and the the track: " 
  	   << (**m_track);
    std::string result(stream.str());
    return (result);
}

