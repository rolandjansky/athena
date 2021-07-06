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

#ifndef MUIDEVENT_INDETMATCHCANDIDATE_H
#define MUIDEVENT_INDETMATCHCANDIDATE_H

#include "MuidEvent/MuonMatch.h"
#include "TrkParameters/TrackParameters.h"


namespace Rec
{

class IndetMatchCandidate: public Rec::MuonMatch
{
public:
    
    // constructor
    IndetMatchCandidate (const Trk::Track*		extrapolatedTrack,
			 int				indetIndex,
			 const Trk::Track*		indetTrack,
			 const Trk::Perigee*		outgoingPerigee,
			 const Trk::Track*		outgoingTrack,
			 const Trk::Track*		spectrometerTrack);
    
    // destructor
    ~IndetMatchCandidate();

    // outgoing indet track
    const Trk::Perigee*		perigee (void) const;
    const Trk::Track&		track (void) const;
    
    // set methods
    void			combinedMatch (MatchType type, const Trk::Track* track = 0);
    void			innerMatch (double	matchChi2,
					    int		matchDoF,
					    double	matchProb,
					    double	simpleChi2);
    
    // accessors
    int				indetIndex (void) const;
    double			simpleMatchChi2 (void) const;
    
private:
    int					m_indetIndex;
    const Trk::Perigee*			m_outgoingPerigee;
    const Trk::Track*			m_outgoingTrack;
    double				m_simpleMatchChi2;

};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const Trk::Perigee*
IndetMatchCandidate::perigee (void) const
{ return m_outgoingPerigee; }

inline const Trk::Track&
IndetMatchCandidate::track (void) const
{ return *m_outgoingTrack; }
   
inline int
IndetMatchCandidate::indetIndex (void) const
{ return m_indetIndex; }

inline double
IndetMatchCandidate::simpleMatchChi2 (void) const
{ return m_simpleMatchChi2; }	

}	// end of namespace

#endif // MUIDEVENT_INDETMATCHCANDIDATE_H


