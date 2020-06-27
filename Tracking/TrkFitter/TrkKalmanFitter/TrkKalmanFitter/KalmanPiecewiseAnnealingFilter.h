/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KalmanPiecewiseAnnealingFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKKALMANPIECEWISEANNEALINGFILTER_H
#define TRKKALMANPIECEWISEANNEALINGFILTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkFitterInterfaces/IKalmanPiecewiseAnnealingFilter.h"
#include "TrkFitterInterfaces/IForwardKalmanFitter.h"
#include "TrkFitterInterfaces/IKalmanSmoother.h"

namespace Trk 
{
  class ICompetingRIOsOnTrackTool;

  /** @class KalmanPiecewiseAnnealingFilter
      @brief sub-filter with deterministic annealing capabilities, to be invoked
             by KalmanFitter in case an outlier removal or left-right solution is difficult

      mini-DAF embedded in normal kalman fitter to solve drift tube left-right
      ambiguities. At the level of information flow and properties it 
      - retrieves toolhandles of FKF, BKS, LR-CompRotTool, and diverse ROT creators
      - gets configured with extrapolator, updator, DNA and trajectory-utility
      - has jO of annealing scheme, max stepAhead
      - modifyable inputs: traj, iterator, predPar, updPar - DANGER
      - error condition: give back start point, let KF deal with problem

      @author  Wolfgang Liebig <Wolfgang.Liebig -at- cern.ch>
  */  

  class KalmanPiecewiseAnnealingFilter : virtual public IKalmanPiecewiseAnnealingFilter,
    public AthAlgTool
    {
    public:
      KalmanPiecewiseAnnealingFilter(const std::string&,const std::string&,const IInterface*);

      //! default destructor
      virtual ~KalmanPiecewiseAnnealingFilter ();
      
      //! standard Athena-Algorithm method
      virtual StatusCode initialize();
      //! standard Athena-Algorithm method
      virtual StatusCode finalize  ();

      //! pass tool handles from master tool (KF) to avoid need for multiple python config
      virtual StatusCode configureWithTools(const IExtrapolator*,
                                   const IUpdator*,
                                   const IMeasurementRecalibrator*,
                                   const ProtoTrajectoryUtility*,
                                   const IDynamicNoiseAdjustor* );

      /** @brief the main method: run DAF on the full trajectory, starting from first fittable
                 state and from parameters therein, do not do last smoother */
      virtual FitterStatusCode
        filterTrajectory (Trajectory& trajectory, const ParticleHypothesis&) const;

      /** @brief the main method: run DAF on a piece of the current trajectory, starting at iterator
                 and using upd/predPar, do not do last smoother */
      virtual FitterStatusCode
        filterTrajectoryPiece (Trajectory& trajectory,
                               Trajectory::iterator&,
                               const TrackParameters*&,
                               const TrackParameters*&,
                               int,
                               const ParticleHypothesis&) const;

    private:
      
      /** important - this needs documentation */

      bool annealingProblem_driftCircles(const FitQuality&,
                                         const FitQuality&,
                                         FitterStatusCode&,
                                         const int&,
                                         const bool& ) const;

      /** important - this needs documentation */

      bool annealingProblem_all(const FitQuality&,
                                const int&,
                                const bool&) const;
      bool annealingProblem_all(const FitQuality&,
                                const FitQuality&,
                                const int&,
                                const bool&) const;

      //////////////////////////////////////////////////////////////////////////
      // --- tool handle members, at the same time settable through job options
      //////////////////////////////////////////////////////////////////////////
      //! forward filter, simple and private instance to avoid clash with calling FKF
      ToolHandle< IForwardKalmanFitter >  m_forwardFitter;
      //! backward smoother
      ToolHandle< IKalmanSmoother >       m_smoother;
      //! rot creator for competing ROTs
      ToolHandle< ICompetingRIOsOnTrackTool > m_compRotTool;

      std::vector<double>                 m_option_annealingScheme;   // ann. scheme to be used
      double                              m_option_outlierCut;        // cut on ass. probability

      const IExtrapolator*                m_extrapolator;
      const IUpdator*                     m_updator;
      const IMeasurementRecalibrator*     m_recalibrator;
      const IDynamicNoiseAdjustor*        m_dynamicNoiseAdjustor;
      mutable Trajectory                  m_trajPiece;
      const ProtoTrajectoryUtility*       m_utility;
      mutable std::vector<double>         m_chi2DuringAnnealing;   // ann. scheme to be used
      const ParticleMasses                m_particleMasses;

    }; 
} // end of namespace

#endif 
