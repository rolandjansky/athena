/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESIDE_H
#define TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESIDE_H

// Base classes
namespace Trk{
	
	/** Enumerates the 'side' of the wire on which the tracks passed (i.e. resolves the amibiguity) 
	(based on the TRT_Side enum, from InDet::TRT_DriftCircleOnTrack, which this should eventually replace)
	@author edward.moyse@cern.ch
	*/
	enum DriftCircleSide 
	{
          /** it was not possible to determine the which side of the straw the track passed;*/
          NONE, 
          /** the drift radius is negative (see Trk::AtaStraightLine)*/
          LEFT, 
          /** the drift radius is positive (see Trk::AtaStraightLine)*/
          RIGHT
	};
}

#endif // TRK_TRKEVENTPRIMITIVES_DRIFTCIRCLESIDE_H

