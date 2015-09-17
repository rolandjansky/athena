/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			IBremsstrahlungFinder.h  -  description
			---------------------------------------------
created              : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
description          : Interface for bremsstrahlung finding using the GSF
*********************************************************************************/


#ifndef Trk_IBremsstrahlungFinder_H
#define Trk_IBremsstrahlungFinder_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkFitterUtils/FitterTypes.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk{

class MultiComponentState;
class TrackStateOnSurface;
class Surface;
class QoverPBremFit;


//!< Interface ID for IBremsstrahlungFinder
static const InterfaceID IID_IBremsstrahlungFinder( "IBremsstrahlungFinder", 1, 0 );

class IBremsstrahlungFinder : virtual public IAlgTool{

 public:

  //!< Virtual destructor
  virtual ~IBremsstrahlungFinder(){};

  //!< IAlgTool-AlgTool interface
  static const InterfaceID& interfaceID() { return IID_IBremsstrahlungFinder; };

	//Find the brem points from a forward and smooth track fit
	virtual void BremFinder(const Trk::ForwardTrajectory&, const Trk::SmoothedTrajectory&) = 0;

	virtual void BremFinder(const Trk::ForwardTrajectory&, const Trk::SmoothedTrajectory&, bool) = 0;
	
	virtual void CombineParameters() = 0;

	virtual void GetBremLocations() = 0;

	virtual double GetBound(double, int) = 0;

	virtual double BremSignificance(double, QoverPBremFit*, QoverPBremFit*) = 0;

	virtual double GetPhiFromValue(double, QoverPBremFit*) = 0;
	
	virtual int ClosestMeasurement(double, QoverPBremFit*) = 0;

	virtual void CalibrateValue() = 0;
	
	virtual const Surface* ClosestSurface(double, double, double, const TrackParameters&) = 0; 
	
	virtual Amg::Vector3D SurfacePosition(const TrackParameters&, const Surface&, double, double, double) = 0;
	
	virtual StatusCode retrieveTrackingGeometry() = 0;
	
	virtual int GetNBrems() = 0;

	virtual void MakeTrackStateOnSurfaces () = 0;
	
	virtual const TrackStateOnSurface* GetEstimatedBremOnTrack(int) = 0;

};

} // end Trk namespace

#endif
