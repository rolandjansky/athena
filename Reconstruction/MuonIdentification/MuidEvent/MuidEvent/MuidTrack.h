/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuidTrack

 simple inheritance from MuonMatch:
 to provide backwards compatibility for clients based on old iPatTrack
 Dummy methods
	 
   @author Alan.Poppleton@cern.ch
*/
///////////////////////////////////////////////////////////////////////////////

#ifndef MUIDEVENT_MUIDTRACK_H
# define MUIDEVENT_MUIDTRACK_H

#include "MuidEvent/MuonMatch.h"

namespace Trk
{
    class Track;
}

class MuidTrack: public Rec::MuonMatch
{
public:
    // constructor (copy from base)
    MuidTrack (Rec::MuonMatch&	muonMatch);

    // constructor with track associations
    MuidTrack (const Trk::Track*		combinedTrack,
	       const Trk::Track*		extrapolatedTrack,
	       const Trk::Track*		indetTrack,
	       const Trk::Track*		spectrometerTrack,
	       Rec::FieldIntegral		fieldIntegral = Rec::FieldIntegral(),
	       double				innerMatchChi2			= 0.,
	       int				innerMatchDoF			= 0,
	       double				innerMatchProb			= 0.,
	       double				momentumBalanceSignificance	= 0.,
	       double				outerMatchChi2			= 0.,
	       int				outerMatchDoF			= 0,
	       double				outerMatchProb			= 0.,
	       Rec::ScatteringAngleSignificance
	       scatAngleSignificance = Rec::ScatteringAngleSignificance());

    // destructor
    virtual ~MuidTrack();

    // deprecated match accessors (return inner match quantities)
    double			matchChi2 (void) const;
    int				matchDoF (void) const;
    double			matchProb (void) const;

    // deprecated get/set methods for obsolete muonCode
    int				muonCode (void) const;
    void			muonCode (int value);

private:
    int					m_muonCode;
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
MuidTrack::matchChi2 (void) const
{ return m_innerMatchChi2; }	

inline int
MuidTrack::matchDoF (void) const
{ return m_innerMatchDoF; }	

inline double
MuidTrack::matchProb (void) const
{ return m_innerMatchProb; }	
 
inline int
MuidTrack::muonCode (void) const
{ return m_muonCode; }	

inline void
MuidTrack::muonCode (int value)
{ m_muonCode = value; }	

#endif // MUIDEVENT_MUIDTRACK_H


