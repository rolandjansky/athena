/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 distinguish type of measurement contributing to TrackFit
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_MEASUREMENTTYPE_H
# define TRKIPATFITTERUTILS_MEASUREMENTTYPE_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>

class MsgStream;

namespace Trk
{
    enum  MeasurementType
    {
	perigeeParameters,
	transverseVertex,
	vertex,
	pixelCluster,
	stripCluster,
	trapezoidCluster,
	driftCircle,
	pseudoMeasurement,
	barrelScatterer,
        endcapScatterer,
        calorimeterScatterer,
	barrelInert,
	endcapInert,
	energyDeposit,
	alignment,
	discontinuity,
	bremPoint,
	hole,
	materialDelimiter
    };

    MsgStream& operator<< (MsgStream& log, MeasurementType type);

}	// end of namespace

#endif	// TRKIPATFITTERUTILS_MEASUREMENTTYPE_H
