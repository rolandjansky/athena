/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ForwardRefTrackKalmanFitter.h
//   Header file for forward Kalman Fit
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_FORWARDREFTRACKKALMANFITTER_H
#define TRK_FORWARDREFTRACKKALMANFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterInterfaces/IForwardKalmanFitter.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterUtils/FitterStatusCode.h"

/////////////////////// Forward Declarations ///////////////////////////////////
//
class AtlasDetectorID;

namespace Trk {

  class IExtrapolator;            //!< Extrapolation Tool
  class IUpdator;                 //!< Interface for Estimator updating
  class IRIO_OnTrackCreator;      //!< Interface for creation of ROTs
  class IAlignableSurfaceProvider;//!< Interface for Kalman alignment
  class IMeasurementRecalibrator; //!< Interface to re-do ROT creation
  class IKalmanPiecewiseAnnealingFilter;
  class Surface;                  //!< Hit surface, needed in private function
  class TrackStateOnSurface;
  class TrackingVolume;
  class DNA_MaterialEffects;      //!< dna data for logging method
  class ProtoTrajectoryUtility;   //!< helper class for analysing PTSoS vector.
}
		
namespace Trk {
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  /** @brief implementation of the iterative forward filtering part
      performed during the Kalman filter track fitter.

      It is implemented as an athena AlgTool and offers two methods:
      to filter a set of Trk::MeasurementBase and of Trk::PrepRawData
      objects, respectively. Note that it uses the private data object
      from the
      KalmanFitter main tool, the vector<Trk::ProtoTrackStateOnSurface>.
      
      @author S. Fleischmann, W. Liebig
   */
  class ForwardRefTrackKalmanFitter : virtual public IForwardKalmanFitter, public AthAlgTool {
public:	
    //! standard AlgTool constructor
    ForwardRefTrackKalmanFitter(const std::string&,const std::string&,const IInterface*);
    virtual ~ForwardRefTrackKalmanFitter();
		
    /** @brief configure the ForwardRefTrackKalmanFitter with the options from the main tool.

        It needs:
        -  IExtrapolator: define which track model to be used for extrapolating tracks
        -  IUpdator: defines the statistics for updating the estimator
        -  IRIO_OnTrackCreator (optional): tool to correct clusters and drift circles using track predictions
        -  IDynamicNoiseAdjustor (optional): tool to extend KF to cope with brem
        -  IAlignableSurfaceProvider (optional): tool to extend KF to be alignment-KF
    */
    virtual StatusCode configureWithTools(const Trk::IExtrapolator*, const Trk::IUpdator*,
                                          const Trk::IRIO_OnTrackCreator*,
                                          const Trk::IDynamicNoiseAdjustor* = nullptr,
                                          const Trk::IAlignableSurfaceProvider* =nullptr,
                                          const Trk::IMeasurementRecalibrator* = nullptr,
                                          const Trk::IKalmanPiecewiseAnnealingFilter* =nullptr);

    //! standard athena AlgTool initialize
    StatusCode initialize();
    //! standard athena AlgTool finalize
    StatusCode finalize();	
			
    //! forward kalman filter working on a set of PrepRawData (eg. fitting during pattern regognition)
    virtual FitterStatusCode fit(Trajectory&,const PrepRawDataSet&,
                                 const TrackParameters& estimatedParametersNearOrigine,
                                 const RunOutlierRemoval,
                                 const KalmanMatEffectsController& ) const;

    /** @brief forward kalman filter working on a set of MeasurementBase (i.e.
        final calibrated hits) or inside the OutlierLogic's iterations.

    */
    virtual FitterStatusCode fit(Trajectory&,
                                 const TrackParameters& estimatedParametersNearOrigine,
                                 const RunOutlierRemoval,
                                 const KalmanMatEffectsController&,
                                 const bool  recalibrate=false,
                                 const int   filterStartState=0) const;
    virtual FitterStatusCode enterSeedIntoTrajectory(Trajectory&            trajectory,
                                                     const TrackParameters& inputPar,
                                                     const std::vector<double>& cov0,
                                                     const Trk::KalmanMatEffectsController&,
                                                     bool  createRefTrajectory) const;

    virtual bool needsReferenceTrajectory() const {return true;}

private:
		
    //! internal method printing the current state
    void printGlobalParams(int istate, const std::string& ptype,
                           const TrackParameters&,
                           const AmgVector(5)&) const;

    // all toolhandles are passed in through a configure method, for reasons of
    // consistency among the KF-subtools and to avoid recursive tool retrieval
    const IExtrapolator*         m_extrapolator;
    const IUpdator*              m_updator;
    const IRIO_OnTrackCreator*   m_ROTcreator;
    const IDynamicNoiseAdjustor* m_dynamicNoiseAdjustor;
    const IMeasurementRecalibrator* m_recalibrator;
    mutable const Trk::TrackingVolume*  m_idEnclosingVolume;
    mutable const Trk::TrackingVolume*  m_msCompleteVolume;
    const AtlasDetectorID*       m_idHelper;
    ProtoTrajectoryUtility*      m_utility;
    double                       m_StateChiSquaredPerNumberDoFPreCut;
    const ParticleMasses         m_particleMasses;

};

} // end of namespace

#endif // TRK_FORWARDREFTRACKKALMANFITTER_H
