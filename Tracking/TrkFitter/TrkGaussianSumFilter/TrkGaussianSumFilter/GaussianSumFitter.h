/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      GaussianSumFitter.h  -  description
      -----------------------------------
begin                : Monday 7th March 2005
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
decription           : Class for fitting according to the Gaussian Sum Filter
                       formalisation.
********************************************************************************** */

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include<atomic>

namespace Trk {

class IMultiStateMeasurementUpdator;
class MultiComponentStateOnSurface;
class IMultiComponentStateCombiner;
class IMultiStateExtrapolator;

class TrackFitInputPreparator;
class IForwardGsfFitter;
class IGsfSmoother;
class Track;
class FitQuality;

class GaussianSumFitter : virtual public ITrackFitter, public AthAlgTool {
 public:

  /** Constructor with parameters to be passed to AlgTool */
  GaussianSumFitter(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GaussianSumFitter() {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  using ITrackFitter::fit;

  /** Refit a track using the Gaussian Sum Filter */

  virtual Track* fit ( const Track&,
           const RunOutlierRemoval outlierRemoval = false,
           const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Fit a collection of 'PrepRawData' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an initial guess */
  virtual Track* fit ( const PrepRawDataSet&,
           const TrackParameters&,
           const RunOutlierRemoval outlierRemoval = false,
           const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Fit a collection of 'RIO_OnTrack' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an initial guess */
  virtual Track* fit ( const MeasurementSet&,
           const TrackParameters&,
           const RunOutlierRemoval outlierRemoval = false,
           const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Refit a track adding a PrepRawDataSet - Not done! */
  virtual Track* fit ( const Track&,
          const PrepRawDataSet&,
          const RunOutlierRemoval  runOutlier = false,
          const ParticleHypothesis  matEffects = nonInteracting)   const;

  /** Refit a track adding a RIO_OnTrack set
      - This has no form of outlier rejection and will use all hits on orginal track...
        i.e. very basic impleneation at the moment*/
  virtual Track* fit  ( const Track&,
          const MeasurementSet&,
          const RunOutlierRemoval  runOutlier = false,
          const ParticleHypothesis  matEffects = nonInteracting)   const;

 /** Combine two tracks by refitting - Not done! */
  virtual Track* fit  ( const Track&,
        const Track&,
        const RunOutlierRemoval  runOutlier = false,
        const ParticleHypothesis  matEffects = nonInteracting )   const;

  /** Save GSF Validation ntuple */
  void validationAction() const;

 private:

  /** Produces a perigee from a smoothed trajectory */
  const MultiComponentStateOnSurface* makePerigee ( const SmoothedTrajectory*,
                const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  //* Calculate the fit quality */
  const Trk::FitQuality* buildFitQuality(const Trk::SmoothedTrajectory& ) const;

 private:
  ToolHandle<IMultiStateExtrapolator> m_extrapolator;

  ToolHandle<IMultiStateMeasurementUpdator> m_updator;

  ToolHandle<IRIO_OnTrackCreator> m_rioOnTrackCreator;

  ToolHandle<IForwardGsfFitter> m_forwardGsfFitter;
  ToolHandle<IGsfSmoother> m_gsfSmoother;


  bool                                      m_reintegrateOutliers;

  bool                                      m_makePerigee;
  PropDirection                             m_directionToPerigee;
  bool                                      m_refitOnMeasurementBase;
  bool                                      m_doHitSorting;
  TrkParametersComparisonFunction*          m_trkParametersComparisonFunction;
  std::vector<double>                       m_sortingReferencePoint;

  ToolHandle<IMultiComponentStateCombiner>  m_stateCombiner;
  ServiceHandle<IChronoStatSvc>             m_chronoSvc;

  TrackFitInputPreparator*                  m_inputPreparator;


 // GSF Fit Statistics
  mutable std::atomic<int> m_FitPRD;             // Number of Fit PrepRawData Calls
  mutable std::atomic<int> m_FitMeasuremnetBase; // Number of Fit MeasurementBase Calls
  mutable std::atomic<int> m_FowardFailure;      // Number of Foward Fit Failures       
  mutable std::atomic<int> m_SmootherFailure;    // Number of Smoother Failures         
  mutable std::atomic<int> m_PerigeeFailure;     // Number of MakePerigee Failures  
  mutable std::atomic<int> m_fitQualityFailure;  // Number of Tracks that fail fit Quailty test     

  
};

} // end Trk namespace
