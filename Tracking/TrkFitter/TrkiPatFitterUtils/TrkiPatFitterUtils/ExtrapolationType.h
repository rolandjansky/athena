/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 enumerated type to give the purpose of an extrapolation iteration
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_EXTRAPOLATIONTYPE_H
# define TRKIPATFITTERUTILS_EXTRAPOLATIONTYPE_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>

class MsgStream;

namespace Trk
{
    enum  ExtrapolationType
    {
	FittedTrajectory,
	DeltaD0,
	DeltaZ0,
	DeltaPhi0,
	DeltaTheta0,
	DeltaQOverP0,
	DeltaQOverP1,
	ExtrapolationTypes
    };

    MsgStream& operator<< (MsgStream& log, ExtrapolationType type);

}	// end of namespace

#endif	// TRKIPATFITTERUTILS_EXTRAPOLATIONTYPE_H
