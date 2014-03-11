/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanSmoother.h
//   Interface header file for Kalman Smoother
//////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig at cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IKALMANSMOOTHER_H
#define TRK_IKALMANSMOOTHER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkFitterUtils/KalmanMatEffectsController.h"

namespace Trk {
	
  class FitQuality;               //!< total track fit quality, comp. during smoothing
  class IExtrapolator;            //!< Extrapolation Tool
  class IUpdator;                 //!< Interface class for Estimator updating
  class IDynamicNoiseAdjustor;    //!< Interface for brem recovery
  class IAlignableSurfaceProvider;//!< Interface for Kalman alignment

  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  static const InterfaceID InterfaceID_IKalmanSmoother ("IKalmanSmoother", 1, 0);
	
  /** @class IKalmanSmoother

      interface for the backward smoothing algorithm as part of
      the Kalman filter track fitter. 

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */
  class IKalmanSmoother : virtual public IAlgTool { 
  public:	
				
      /** configure the backward Kalman Smoother
        needs:   Propagator			   - define which track model to be used for extrapolatin
                 Updator			   - defines the statistics for updating the estimator
                 DynamicNoiseAdjustor  - tool to adjust noise if brem detected
                 AlignableSurfaceProvider  - tool to configure KF as AlignmentKF
      */
      virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*,
                                            const IDynamicNoiseAdjustor*,
                                            const IAlignableSurfaceProvider*,
                                            const bool, const bool=true) = 0;
		
      /** smoother function. Note that it uses the private data object from the
          KalmanFitter main tool, the vector<ProtoTrackStateOnSurface>.
      */
      virtual FitterStatusCode fit (Trajectory&,
                                    FitQuality*&,
                                    const KalmanMatEffectsController& ) const = 0;

      virtual FitterStatusCode fitWithReference (Trajectory&,
                                                 FitQuality*&,
                                                 const KalmanMatEffectsController&) const = 0;

  };
		
} // end of namespace

#endif // TRK_IKALMANSMOOTHER_H
