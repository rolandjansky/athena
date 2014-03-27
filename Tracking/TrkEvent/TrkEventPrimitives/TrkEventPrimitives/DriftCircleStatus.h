/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESTATUS_H
#define TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESTATUS_H

// Base classes
namespace Trk{
	
	/** Enumerates the 'status' of the wire on which the tracks passed 
	(based on the TRT_Side enum, from InDet::TRT_DriftCircleOnTrack, which this should eventually replace)
	@author edward.moyse@cern.ch
	*/
	enum DriftCircleStatus
	{
          /** sign of drift radius has been determined */
          DECIDED, 
          /** sign of drift radius has not been determined */
          UNDECIDED, 
          /** the measurement is corrupt, but for some reason has been left on a track.*/
          CORRUPT,
          /** drift time was not used - drift radius is 0.*/
          NODRIFTTIME
	};
}

#endif // TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESIDE_H

