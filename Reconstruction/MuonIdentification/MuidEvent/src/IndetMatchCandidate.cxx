/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  IndetMatchCandidate

   Store association between tracks forming combined muon
   and information on the match quality
	 
   @author Alan.Poppleton@cern.ch
*/
///////////////////////////////////////////////////////////////////////////////

#include "MuidEvent/IndetMatchCandidate.h"
//#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

namespace Rec
{

// constructor (inDet track with candidate match to extrapolated track)
IndetMatchCandidate::IndetMatchCandidate (const Trk::Track*		extrapolatedTrack,
					  int				indetIndex,
					  const Trk::Track*		indetTrack,
					  const Trk::Perigee*		outgoingPerigee,
					  const Trk::Track*		outgoingTrack,
					  const Trk::Track*		spectrometerTrack)
    :	Rec::MuonMatch(0,extrapolatedTrack,indetTrack,spectrometerTrack),
	m_indetIndex				(indetIndex),
	m_outgoingPerigee			(outgoingPerigee),
	m_outgoingTrack				(outgoingTrack),
	m_simpleMatchChi2			(999999.)
{}

IndetMatchCandidate::~IndetMatchCandidate (void)
{
    if (m_outgoingPerigee	!= m_indetTrack->perigeeParameters())	delete m_outgoingPerigee;
    if (m_outgoingTrack		!= m_indetTrack)			delete m_outgoingTrack;
}

void
IndetMatchCandidate::combinedMatch (MatchType type, const Trk::Track* track)
{
    delete m_combinedTrack;
    m_combinedTrack	= track;
    m_matchType		= type;
}

void
IndetMatchCandidate::innerMatch(double matchChi2, int matchDoF, double matchProb, double simpleChi2) 
{
    m_innerMatchChi2	= matchChi2;
    m_innerMatchDoF	= matchDoF;
    m_innerMatchProb	= matchProb;
    m_simpleMatchChi2	= simpleChi2;
}	
    
}	// end of namespace
 
