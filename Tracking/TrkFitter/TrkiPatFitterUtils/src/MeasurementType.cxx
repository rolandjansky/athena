/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 distinguish type of measurement contributing to TrackFit
 ***************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "TrkiPatFitterUtils/MeasurementType.h"

namespace Trk
{
    MsgStream& operator<< (MsgStream& log, MeasurementType type)
    {
	switch (type)
	{
	case perigeeParameters:
	    return log << " perigeeParameters";	
	case transverseVertex:
	    return log << " transverseVertex ";
	case vertex:
	    return log << " vertex           ";
	case pixelCluster:
	    return log << " pixelCluster     ";
	case stripCluster:
	    return log << " stripCluster     ";
	case trapezoidCluster:
	    return log << " trapezoidCluster ";    
	case driftCircle:
	    return log << " driftCircle      ";
	case pseudoMeasurement:
	    return log << " pseudoMeas       ";
	case barrelScatterer:
	    return log << " barrelScatterer  ";
	case endcapScatterer:
	    return log << " endcapScatterer  ";
	case calorimeterScatterer:
	    return log << " calorimeterScat  ";
	case barrelInert:
	    return log << " barrelInert      ";
	case endcapInert:
	    return log << " endcapInert      ";    
	case energyDeposit:
	    return log << " energyDeposit    ";
	case alignment:
	    return log << " alignment        ";
	case discontinuity:
	    return log << " discontinuity    ";
	case bremPoint:
	    return log << " bremPoint        ";
	case hole:
	    return log << " hole             ";
	case materialDelimiter:
	    return log << " materialDelimiter";
	    
	default:
	    return log << " unknown type     ";
	}    
    }
}	// end of namespace
