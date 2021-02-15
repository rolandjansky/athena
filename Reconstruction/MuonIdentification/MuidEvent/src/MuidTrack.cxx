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

#include "MuidEvent/MuidTrack.h"
#include "TrkTrack/Track.h"

// constructor (copy from base)
MuidTrack::MuidTrack (Rec::MuonMatch&	muonMatch)
    :	MuonMatch	(muonMatch),
	m_muonCode	(0)
{}

// constructor
MuidTrack::MuidTrack (const Trk::Track*			combinedTrack,
		      const Trk::Track*			extrapolatedTrack,
		      const Trk::Track*			indetTrack,
		      const Trk::Track*			spectrometerTrack,
		      Rec::FieldIntegral		fieldIntegral,
		      double   				innerMatchChi2,
		      int				innerMatchDoF,
		      double   				innerMatchProb,
		      double				momentumBalanceSignificance,
		      double   				outerMatchChi2,
		      int				outerMatchDoF,
		      double   				outerMatchProb,
		      Rec::ScatteringAngleSignificance	scatteringAngleSignificance)
    :	MuonMatch	(combinedTrack,
			 extrapolatedTrack,
			 indetTrack,
			 spectrometerTrack,
			 fieldIntegral,
			 innerMatchChi2,
			 innerMatchDoF,
			 innerMatchProb,
			 momentumBalanceSignificance,
			 outerMatchChi2,
			 outerMatchDoF,
			 outerMatchProb,
			 scatteringAngleSignificance),
	m_muonCode	(0)
{
    // combined constructor
}

MuidTrack::~MuidTrack (void)
{}


