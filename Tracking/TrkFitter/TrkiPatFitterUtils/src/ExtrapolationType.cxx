/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 enumerated type to give the purpose of an extrapolation iteration
 ***************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"

namespace Trk
{    
    MsgStream& operator<< (MsgStream& log, ExtrapolationType type)
    {
	switch (type)
	{
	case FittedTrajectory:
	    return log << " FittedTrajectory";
	case DeltaD0:
	    return log << " DeltaD0         ";
	case DeltaZ0:
	    return log << " DeltaZ0         ";
	case DeltaPhi0:
	    return log << " DeltaPhi0       ";
	case DeltaTheta0:
	    return log << " DeltaTheta0     ";
	case DeltaQOverP0:
	    return log << " DeltaQOverP0    ";
	case DeltaQOverP1:
	    return log << " DeltaQOverP1    ";
	case ExtrapolationTypes:
	    return log << static_cast<int>(ExtrapolationTypes);	
	default:
	    return log << " unknown type    ";
	}    
    }
}	// end of namespace
