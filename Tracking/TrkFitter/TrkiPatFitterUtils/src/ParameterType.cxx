/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 enumerated type of fitted parameters
 ***************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "TrkiPatFitterUtils/ParameterType.h"

namespace Trk
{    
    MsgStream& operator<< (MsgStream& log, ParameterType type)
    {
	switch (type)
	{
	case D0:
	    return log << " D0       ";
	case Z0:
	    return log << " Z0       ";
	case Phi0:
	    return log << " Phi0     ";
	case Theta0:
	    return log << " Theta0   ";
	case QOverP0:
	    return log << " QOverP0  ";
	case QOverP1:
	    return log << " QOverP1  ";
	case ParameterTypes:
	    return log << static_cast<int>(ParameterTypes);	
	default:
	    return log << " unknown type ";
	}    
    }
}	// end of namespace
