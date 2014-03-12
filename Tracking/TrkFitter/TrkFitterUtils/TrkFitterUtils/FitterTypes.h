/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// FitterTypes.h
//   Header file to encapsulate some standard types
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing at cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_FITTERTYPES_H
#define TRK_FITTERTYPES_H

#include <vector>
#include "DataModel/DataVector.h"

namespace Trk {

    //! switch to toggle quality processing after fit
	typedef bool RunOutlierRemoval;
	
	class PrepRawData;
    //! vector of clusters and drift circles
	typedef std::vector<const PrepRawData*>	      PrepRawDataSet;


	class MeasurementBase;
    //! vector of fittable measurements
	typedef std::vector<const MeasurementBase*>	  MeasurementSet;

	class RIO_OnTrack;
    //! vector of detector hits on a track
	typedef std::vector<const RIO_OnTrack*>	      RIO_OnTrackSet;

	class SpacePoint;
    //! vector of space points
	typedef std::vector<const SpacePoint*>	      SpacePointSet;
	
	class TrackStateOnSurface;
    //! vector of states of a track on a measurement surface
	typedef DataVector<const TrackStateOnSurface> ForwardTrajectory;
    //! vector of states of a track on a measurement surface
	typedef DataVector<const TrackStateOnSurface> SmoothedTrajectory;

} // end of namespace

#endif // TRK_FITTERTYPES_H
