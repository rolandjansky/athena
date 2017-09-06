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
#include "xAODEventInfo/EventInfo.h"

#include "TTree.h"

namespace Trk {

class IMultiStateMeasurementUpdator;
class MultiComponentStateOnSurface;
class IMultiComponentStateCombiner;
class IMultiStateExtrapolator;

class TrackFitInputPreparator; 
class IForwardGsfFitter;
class IGsfOutlierLogic;
class IGsfSmoother;
class Track;
class IBremsstrahlungFinder;

#define TRKFGSF_VALSURFACES 100
#define TRKGSF_VALSTATES 24

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

#if 0
  /** Configuration of the Gaussian sum filter. */
  StatusCode configureTools(const IMultiStateMeasurementUpdator*, const IRIO_OnTrackCreator*);
#endif

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
  
  /**Add Data from a FrorwardTrajectory to an Ntuple*/
  void SaveMCSOSF(const Trk::ForwardTrajectory& ) const;

  /**Add Data from a SmoothedTrajectory to an Ntuple*/
  void SaveMCSOSS(const Trk::SmoothedTrajectory& ) const;


 private:
  ToolHandle<IMultiStateExtrapolator> m_extrapolator;
 
  ToolHandle<IMultiStateMeasurementUpdator> m_updator;

  ToolHandle<IRIO_OnTrackCreator> m_rioOnTrackCreator;

  ToolHandle<IForwardGsfFitter> m_forwardGsfFitter;
  ToolHandle<IGsfSmoother> m_gsfSmoother;
  ToolHandle<IGsfOutlierLogic> m_outlierLogic;

  bool                                      m_reintegrateOutliers;

  bool                                      m_makePerigee;
  PropDirection                             m_directionToPerigee;

  bool                                      m_runBremFinder;

  bool                                      m_refitOnMeasurementBase;

  bool                                      m_doHitSorting;
  TrkParametersComparisonFunction*          m_trkParametersComparisonFunction;
  std::vector<double>                       m_sortingReferencePoint;

  ToolHandle<IMultiComponentStateCombiner>  m_stateCombiner;
  ToolHandle<IBremsstrahlungFinder>         m_BremFind;
  ToolHandle<IBremsstrahlungFinder>         m_BremFind2;

  ServiceHandle<IChronoStatSvc>             m_chronoSvc;
  
  TrackFitInputPreparator*                  m_inputPreparator;
   
  
 // GSF Fit Statistics
 
  mutable int                     m_FitPRD;             // Number of Fit PrepRawData Calls
  mutable int                     m_FitMeasuremnetBase; // Number of Fit MeasurementBase Calls
  mutable int                     m_FowardFailure;      // Number of Foward Fit Failures       
  mutable int                     m_SmootherFailure;    // Number of Smoother Failures         
  mutable int                     m_PerigeeFailure;     // Number of MakePerigee Failures  
  mutable int                     m_fitQualityFailure;  // Number of Tracks that fail fit Quailty test     

 //------VALIDATION MODE SECTION ----------------------------------//
  
  static int                      s_Gsf_ValSurface;
  static int                      s_Gsf_ValStates;
  
  bool                            m_validationMode;            //!< boolean to switch to validation mode
  std::string                     m_validationTreeName;        //!< validation tree name - to be acessed by this from root
  std::string                     m_validationTreeDescription; //!< validation tree description - second argument in TTree
  std::string                     m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out


  TTree*                          m_validationTree;            //!< Root Validation Tree

  mutable int                     m_surfaceCounterF;           //!< counter for boundary surfaces hit   

  mutable int                     m_surfacesF;                            //!< associated Ntuple variable
  mutable float                   m_surfaceXF[TRKFGSF_VALSURFACES];       //!< x Position of interseciton with Surface
  mutable float                   m_surfaceYF[TRKFGSF_VALSURFACES];       //!< y Position of interseciton with Surface
  mutable float                   m_surfaceRF[TRKFGSF_VALSURFACES];       //!< Radius of interseciton with Surface
  mutable float                   m_surfaceZF[TRKFGSF_VALSURFACES];       //!< z Position of interseciton with Surface
  mutable int                     m_surfaceTypeF[TRKFGSF_VALSURFACES];    //!< Surface type
  mutable int                     m_surfaceNstatesF[TRKFGSF_VALSURFACES]; //!< Number of components on Surface  
  
  mutable float                   m_surfaceThetaF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];     //!< Track theta on Surface
  mutable float                   m_surfacePhiF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];       //!< Track Phi on Surface
  mutable float                   m_surfaceQoverPF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Track q over p on Surface
  mutable float                   m_surfaceWeightF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Component weight Surface
  mutable float                   m_surfaceErrThetaF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];  //!< Track theta error on Surface
  mutable float                   m_surfaceErrPhiF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Track phi error on Surface
  mutable float                   m_surfaceErrQoverPF[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES]; //!< Track q over p error on Surface

  mutable int                     m_surfaceCounterS;                      //!< counter for boundary surfaces hit   

  mutable int                     m_surfacesS;                            //!< associated Ntuple variable
  mutable float                   m_surfaceXS[TRKFGSF_VALSURFACES];       //!< x Position of interseciton with Surface
  mutable float                   m_surfaceYS[TRKFGSF_VALSURFACES];       //!< y Position of interseciton with Surface
  mutable float                   m_surfaceRS[TRKFGSF_VALSURFACES];       //!< Radius of interseciton with Surface
  mutable float                   m_surfaceZS[TRKFGSF_VALSURFACES];       //!< z Position of interseciton with Surface
  mutable int                     m_surfaceTypeS[TRKFGSF_VALSURFACES];    //!< Surface type
  mutable int                     m_surfaceNstatesS[TRKFGSF_VALSURFACES]; //!< Number of components on Surface  
  
  mutable float                   m_surfaceThetaS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];     //!< Track theta on Surface
  mutable float                   m_surfacePhiS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];       //!< Track Phi on Surface
  mutable float                   m_surfaceQoverPS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Track q over p on Surface
  mutable float                   m_surfaceWeightS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Component weight Surface
  mutable float                   m_surfaceErrThetaS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];  //!< Track theta error on Surface
  mutable float                   m_surfaceErrPhiS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES];    //!< Track phi error on Surface
  mutable float                   m_surfaceErrQoverPS[TRKFGSF_VALSURFACES][TRKGSF_VALSTATES]; //!< Track q over p error on Surface
  mutable int                     m_event_ID;

  SG::ReadHandleKey<xAOD::EventInfo> m_readKey;

};

} // end Trk namespace
