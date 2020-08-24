/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanSmoother.h
//   Header file for Kalman Smoother
//////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANSMOOTHER_H
#define TRK_KALMANSMOOTHER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterInterfaces/IKalmanSmoother.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
	
  class FitQuality;               //!< total track fit quality, comp. during smoothing
  class IExtrapolator;            //!< Extrapolation Tool
  class IUpdator;                 //!< Interface class for Estimator updating
  class IDynamicNoiseAdjustor;    //!< Interface for brem recovery
  class IAlignableSurfaceProvider;//!< Interface for Kalman alignment
  class DNA_MaterialEffects;      //!< dna data for logging method
  class ProtoTrajectoryUtility;  // helper to analyse current trajectory

  class IExtrapolationEngine;     //!< Extrapolation Engine

  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
	
  /** @brief implementation of the backward smoothing algorithm as part of
      the Kalman filter track fitter.

      This version works by running a backward filter (instead of
      Fruehwirth's smoother)
      which after combining with the forward filtered state provides
      the fitted track's parameters at each measurement surface.
  */
  class KalmanSmoother : virtual public IKalmanSmoother, public AthAlgTool {
  public:	
    //! standard AlgTool constructor */
    KalmanSmoother(const std::string&,const std::string&,const IInterface*);
    virtual ~KalmanSmoother();
		
    //! standard athena AlgTool initialisation
    StatusCode initialize();
    //! standard athena AlgTool finalising
    StatusCode finalize();
				
    /** @brief configures the backward-smoother with its tool members

    It needs:
    -  IExtrapolator: define which track model to be used for extrapolating tracks
    -  IUpdator: defines the statistics for updating the estimator
    -  IRIO_OnTrackCreator (optional): tool to correct clusters and drift circles using track predictions
    -  IDynamicNoiseAdjustor (optional): tool to extend KF to cope with brem
    -  IAlignableSurfaceProvider (optional): tool to extend KF to be alignment-KF
    */
    virtual StatusCode configureWithTools(Trk::IExtrapolator*, const Trk::IUpdator*,
                                          const Trk::IDynamicNoiseAdjustor*,
                                          const Trk::IAlignableSurfaceProvider*,
                                          const bool, const bool=true);
		
    /** @brief smoother function.
        
        Note that it uses the private data object from the
        KalmanFitter main tool, the vector<ProtoTrackStateOnSurface>.
    */
    virtual FitterStatusCode fit (Trajectory&,
                                  FitQuality*&,
                                  const KalmanMatEffectsController&) const;

    virtual FitterStatusCode fitWithReference (Trajectory&,
                                               FitQuality*&,
                                               const KalmanMatEffectsController&) const;

private:
    //! internal method to build up the track quality while filtering
    void addChi2IncrementAndDelete(FitQualityOnSurface*&,
                                   double&, int&) const; 
    //! internal method to find out if the track is in field (ndof5) or straight (ndof4)
    bool straightTrackModel(const Trk::TrackParameters& ) const;

    //! internal method printing the current state (debugging)
    void printGlobalParams(int istate, const std::string& ptype,
                           const TrackParameters*,
                           const DNA_MaterialEffects* = nullptr) const;

    /** @brief method trying to define inital error for bw smoother in a smart way.

        It scales by a factor (which is tunable, typically 200-1000).
        Do this only if all parameters were well constraint. */
    AmgSymMatrix(5)* initialiseSmoother (const AmgSymMatrix(5)&) const;

    //! method to seed the smoother with a fixed error matrix
    AmgSymMatrix(5)* initialiseSmootherFixed (const AmgSymMatrix(5)&) const;
      
    // list of tracking tools and fitter extension tools used in backward filter
    IExtrapolator*                   m_extrapolator;
    ToolHandle<IExtrapolationEngine>   m_extrapolationEngine;     
    bool                               m_useExEngine;
    const IUpdator*                  m_updator;
    const IDynamicNoiseAdjustor*     m_dynamicNoiseAdjustor;
    const IAlignableSurfaceProvider* m_alignableSurfaceProvider;

    // private members for configuration, steering, book-keeping
    ProtoTrajectoryUtility*    m_utility;
    bool                       m_doSmoothing;
    bool                       m_calculateFitQuality;
    std::vector<double>        m_initialErrors;

    // job options
    float                      m_initialCovSeedFactor;
    float                      m_option_relErrorLimit;

    /** smoother-specific statistics and validation */
    enum FitStatusCodes {Call, Success, StraightTrackModelUsed, BadInput, UpdateFailure, FitQualityFailure, MissingCovariance, ExtrapolationFailure, ExtrapolationFailureDueToSmallMomentum, CombineStatesFailure,  WeaklyConstraintForwardPar, ForwParOutsideSurfaceBounds, nFitStatusCodes};
    mutable std::vector< std::vector<int> > m_fitStatistics;
    enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, nStatIndex = 4};

    //! do bookkeeping about fitter calls and error situations
    void monitorTrackFits(FitStatusCodes, const double& eta) const;
    const ParticleMasses         m_particleMasses;
  };
		
} // end of namespace

#endif // TRK_KALMANSMOOTHER_H
