/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ForwardKalmanFitter.h
//   Interface for the forward filter tool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang Liebig at cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IFORWARDKALMANFITTER_H
#define TRK_IFORWARDKALMANFITTER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkFitterUtils/KalmanMatEffectsController.h"

/////////////////////// Forward Declarations ///////////////////////////////////
namespace Trk {
  class IExtrapolator;            // extrapolation Tool
  class IUpdator;                 // interface for Estimator updating
  class IRIO_OnTrackCreator;      // interface for creation of ROTs
  class IDynamicNoiseAdjustor;    // interface for brem recovery
  class IAlignableSurfaceProvider;// interface for Kalman alignment
  class IMeasurementRecalibrator; // interface to re-do ROT creation
  class IKalmanPiecewiseAnnealingFilter; // interface for doing parts of filter with different concept
}


		
namespace Trk {

  static const InterfaceID InterfaceID_IForwardKalmanFitter ("IForwardKalmanFitter", 1, 0);

  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  /** @class IForwardKalmanFitter
      @brief Implementation of the iterative forward filtering part performed
      by the Kalman and Deterministic Annealing filter track fitters.

      It offers two methods: to filter a set of MeasurementBase and of PrepRawData
      objects, respectively. Note that it uses the private data object from the
      KalmanFitter main tool, the vector<ProtoTrackStateOnSurface>.

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
   */
  class IForwardKalmanFitter : virtual public IAlgTool {
  public:	
		
    /** configure the ForwardKalmanFitter with the options from the main tool. It needs:
        @param Extrapolator Defines which track model and material effects source for parameters extrapolation
        @param Updator      Defines the statistics for updating the estimator
        @param ROTCreator   Tool to correct clusters and driftcircles using track predictions
        @param DynamicNoiseAdjustor Tool to adjust noise if brem detected
        @param AlignableSurfaceProvider  Tool to configure KF as AlignmentKF
        @param IMeasurementRecalibrator Tool to encapsulate fine-tune calibrations
        @return reports through an athena StatusCode
    */
    virtual StatusCode configureWithTools(const IExtrapolator*, const IUpdator*,
                                          const IRIO_OnTrackCreator*,
                                          const IDynamicNoiseAdjustor* = nullptr,
                                          const IAlignableSurfaceProvider* = nullptr,
                                          const IMeasurementRecalibrator* = nullptr,
                                          const IKalmanPiecewiseAnnealingFilter* =nullptr ) = 0;
				
    /** @brief forward kalman filter working on a set of PrepRawData, for example
        intended for fitting during pattern regognition 

        @param trajectory     The trajectory information built on the fly from the PRD
        @param prepRawDataSet The vector of PrepRawData
        @param estimatedPars  The reference track parameters
        @param runOutlier     toggle if fwd-filter should protect itself against outlier
        @param kalMec        wraps the particle hypothesis, steering mat effects & breakpoints
        @param doDna          Flags if DNA should be performed in this first iteration

    */
    virtual FitterStatusCode fit(Trajectory&,const PrepRawDataSet&,
                                 const TrackParameters& estimatedParametersNearOrigine,
                                 const RunOutlierRemoval  runOutlier,
                                 const KalmanMatEffectsController& kalMec) const = 0;

    /** @brief forward kalman filter working on a set of MeasurementBase (i.e. final
        calibrated hits) or inside the OutlierLogic's iterations.
        
        @param trajectory    The vector of (fitter-internal) track states to manage fit data
        @param estimatedPars The reference track parameters
        @param runOutlier    toggle if fwd-filter should protect itself against outlier
        @param kalMec        wraps the particle hypothesis, steering mat effects & breakpoints
        @param recalibrate   Flags if ROTs should be re-made using the actual filter prediction
        @param filterStartState steers how the forward filter should resume:
           - 0  takes the reference parameters, sets a large default cov matrix
                and calculates *all* parameters during filtering
           - >0 takes parameters with their errors from the first non-outlier on trajectory
                *before* the filterStartState. If there is no meas't before, it takes the first
                 non-outlier on the trajectory. If there are no parameters on the trajectory,
                 it takes the (measured) referenceparameters as they are.
           - >1 re-starts the filter at this point, e.g. after an outlier was removed at
                this state. Calculates only the remaining parameters
        @return gives a FitterStatusCode enum back, telling what happened during filtering
    */
    virtual FitterStatusCode fit(Trajectory& trajectory,
                                 const TrackParameters& estimatedParametersNearOrigine,
                                 const RunOutlierRemoval  runOutlier,
                                 const KalmanMatEffectsController& kalMec,
                                 const bool               recalibrate=false,
                                 const int                filterStartState=0) const=0;

    /** method to prepare the forward filter by calculating seed parameters next to the
        first non-outlier state on the trajectory. 
        
        @param trajectory    The vector of fitter-internal track states, must not have any track pars
        @param estimatedPars The reference track parameters
        @param cov0          default cov matrix diagonal values (5) in case inputPar are non-measured */
    virtual FitterStatusCode enterSeedIntoTrajectory(Trajectory&            trajectory,
                                                     const TrackParameters& inputPar,
                                                     const std::vector<double>& cov0,
                                                     const Trk::KalmanMatEffectsController&,
                                                     bool  makeRefTrajectory) const=0;

    /** method telling if implementation uses dynamic or reference trajectory */
    virtual bool needsReferenceTrajectory() const=0;

private:
		
};

} // end of namespace

#endif // TRK_IFORWARDKALMANFITTER_H
