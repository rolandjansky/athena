/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"


namespace Trk
{
	LinkToTrack::LinkToTrack() : ElementLink<TrackCollection>()
	{}

	LinkToTrack::LinkToTrack ( ElementLink<TrackCollection>& link ) : ElementLink<TrackCollection> ( link )
	{}

        const TrackParameters* LinkToTrack::parameters() const
	{
		if ( isValid() )
		{
			const Trk::Track * trk = this->cachedElement();
			if ( 0 != trk ) return dynamic_cast<const Trk::TrackParameters* > ( trk->perigeeParameters() );
			return 0;
		}
		return 0;
	}//end of parameters method

        Trk::LinkToTrack* Trk::LinkToTrack::clone() const
	{
		return new LinkToTrack ( *this );
	}

}//end of namespace definitions
