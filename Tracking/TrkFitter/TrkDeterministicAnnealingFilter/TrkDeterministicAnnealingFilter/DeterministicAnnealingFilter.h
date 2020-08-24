/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DeterministicAnnealingFilter.h
//   Header file for class DeterministicAnnealingFilter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_DETERMINISTICANNEALINGFILTER_H
#define TRK_DETERMINISTICANNEALINGFILTER_H

#include "TrkFitterUtils/FitterTypes.h"     // typedefs
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkFitterUtils/FitterStatusCode.h"    // needed for Trk::FitterStatusCode::Success
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"      // typedef
#include "TrkEventUtils/TrkParametersComparisonFunction.h"  // typedef

#include <memory>
class IAlgTool;

namespace Trk {

    class Track;                   // ATLAS standard track class
    class FitQuality;
    class PerigeeSurface;
    class IDynamicNoiseAdjustor;

    class IExtrapolator;           // Extrapolation Tool
    class IUpdator;                // If class for Estimator updating
    class ICompetingRIOsOnTrackTool;     // Interface for creation of compROT
//    class ITrackFitter;                  // Interface class for track fitters

    class IValidationNtupleTool;
//class IWeightCalculator;
    class IForwardKalmanFitter;    // Forward kalman fit for fitter
    class IKalmanSmoother;         // smoother logic for fitter
//    class KalmanOutlierLogic;      // Outlier logic for fitter
    class ProtoTrajectoryUtility;  // helper to analyse current trajectory
//    class TrackFitInputPreparator; // helper to fill internal trajectories

    //class FitterStatusCode;
    class ProtoTrackStateOnSurface;
    typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;


/** Main Fitter tool providing the implementation for the different
 *  fitting, extension and refitting use cases. It manages and calls
 *  the other tools.
 *
 *  There are two modes of operation: One can use a general Trk::ITrackFitter as underlying
 *  track fitting tool or use the Kalman fitter sub-tools directly. The second method
 *  is the standard (and prefered) mode, because it avoids extensive deep-copies of tracks
 *  and is therefore much faster than using the general ITrackFitter interface.
 */
class DeterministicAnnealingFilter : virtual public ITrackFitter, public AthAlgTool {
public:
    // standard AlgTool methods
    DeterministicAnnealingFilter(const std::string&,const std::string&,const IInterface*);
    DeterministicAnnealingFilter();
    ~DeterministicAnnealingFilter();
    // standard Athena methods
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    /*
     * Bring in default impl with
     * EventContext for now
     */
    using ITrackFitter::fit;
    /** refit a track (the main method; it uses most information from pattern recognition):
    This function should normally be called with tracks containing
    Trk::CompetingRIOsOnTrack.
    If non CompetingRIOsOnTrack are given it might include the creation of
    Trk::CompetingRIOsOnTrack depending on the jobOptions.
    runOutlier has no meaning in all of the DAF's fitting methods */
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** refit a track adding a PrepRawDataSet: Trk::ICompetingRIOsOnTrackCreator is used
    to create Trk::CompetingRIOsOnTrack out of the additional PrepRawData first.
    Be careful: The dicision which PrepRawData should compete against each other can
    not be done here.
     - NOT IMPLEMENTED YET!!! */
    virtual std::unique_ptr<Track> fit(
      const EventContext&,
      const Track&,
      const PrepRawDataSet&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** fit a track to a PrepRawDataSet: Trk::ICompetingRIOsOnTrackCreator is used
    to create Trk::CompetingRIOsOnTrack out of the additional PrepRawData first.
    Be careful: The dicision which PrepRawData should compete against each other can
    not be done here.
    - NOT IMPLEMENTED YET!!! */
    virtual std::unique_ptr<Track> fit(
      const EventContext&,
      const PrepRawDataSet&,
      const TrackParameters&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** re-fit a track, adding a fittable measurement set:
         This function should normally be called with a MeasurementSet containing
        Trk::CompetingRIOsOnTrack
        - NOT IMPLEMENTED YET!!!*/
    virtual std::unique_ptr<Track> fit(
      const EventContext&,
      const Track&,
      const MeasurementSet&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** fit a track to a set of measurementBase.
        This function should normally be called with a MeasurementSet containing
        Trk::CompetingRIOsOnTrack.
        The TrackParameters is a first estimate for the track.
        If non Trk::CompetingRIOsOnTrack are given the TrackParameters are
        used for computing the initial assignment probabilities.*/
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const MeasurementSet&,
      const TrackParameters&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** combined track fit:
        - NOT implemented yet!!!*/
    virtual std::unique_ptr<Track> fit(const EventContext&,
                                       const Track&,
                                       const Track&,
                                       const RunOutlierRemoval,
                                       const ParticleHypothesis) const override;
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    //    StatusCode myGetTool(IToolSvc*&,
    //                         const std::string&,
    //                         IAlgTool*&);         //!< makes tool retrieving nicer

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
    ToolHandle< IExtrapolator >                     m_extrapolator;             //!< extrapolator used to calc initial assignment probs and used by Kalman sub-tools
    //std::string                         m_option_ExtrapTool;        //!< jobOption: name and instance of Extrapolator Tool

    ToolHandle< IUpdator >                          m_updator;                  //!< measurement updator used by the Kalman forward filter and backward smoother
    //std::string                         m_option_UpdatingTool;      //!< jobOption: name and instance of Measurement Updator Tool

    ToolHandle< ICompetingRIOsOnTrackTool >         m_compROTcreator;           //!< CompetingRIOsOnTrackTool to recalc assignment probabilities and to create CompetingRIOsOnTrack
    //std::string                         m_option_compROTcreator;    //!< jobOption: name and instance of CompetingRIOsOnTrackTool

    //ToolHandle< const ITrackFitter* >               m_ITrackFitter;             //!< pointer to ITrack Fitter
    //std::string                         m_option_KalmanFitter;      //!< jobOption: DEACTIVATED IN THE CURRENT VERSION! 
    //Name and instance of the ITrackFitter. Default and best choice: "none". If set to "none", the Kalman Forward Filter and Backward Smoother are used directly (recommended for speed reasons).

    ToolHandle< IForwardKalmanFitter >              m_forwardFitter;            //!< the tool handle for the Kalman forward filter
    //std::string                         m_option_FwFilterInstance;  //!< jobOption: instance of the Kalman forward filter
    ToolHandle< IKalmanSmoother >                   m_smoother;                 //!< the tool handle for the Kalman backward smoother
    //std::string                         m_option_SmootherInstance;  //!< jobOption: instance of the Kalman backward smoother

    ToolHandle <IDynamicNoiseAdjustor>              m_dna;

    ToolHandle< IValidationNtupleTool >             m_FitterValidationTool;     //!< Validation tool 
    mutable bool                                    m_doValidation;             //!< call validation tool after the current iteration of the fit
    bool                                            m_haveValidationTool;       //!< validation tool given (checking the m_FitterValidationTool pointer provokes the ToolHandle to try to retrieve())?
    bool                                            m_option_callValidationToolForFailedFitsOnly;   //!< Validation tool is called for failed fits only

    TrkParametersComparisonFunction*    m_tparScaleSetter;
    ProtoTrajectoryUtility*             m_utility;                  //!< ProtoTrajectoryUtility for debug output of trajectory
//    TrackFitInputPreparator*            m_inputPreparator;          //!< input preparator to ease implementation of all fit() routines
    std::vector<double>                 m_option_sortingRefPoint;   //!< jobOption: reference point for measurement sorting
    std::vector<double>                 m_option_annealingScheme;   //!< jobOption: annealing scheme to be used in the annealing process

    bool                                m_option_doHitSorting;      //!< jobOption: sort measurements?
    bool                                m_option_generatePerigee;   //!< jobOption: generate Perigee parameters
    double                              m_option_OutlierCut;        //!< jobOption: cut on assignment probability: flag a measurement as outlier, if sum of assignment probabilties of a Trk::CompetingRIOsOnTrack is below this value (BUT: all measurements are used in the fit according to their assignment probabilities). Set to 0. to deactivate the cut
    double                              m_option_DropOutlierCut;    //!< jobOption: cut on assignment probability: drop a measurement from the track at the end of the annealing procedure, if sum of assignment probabilties of a Trk::CompetingRIOsOnTrack is below this value. Set to 0. to deactivate the cut

    mutable Trajectory                  m_trajectory;               //!< trajectory of Trk::ProtoTrackStateOnSurface
    
    Trk::PropDirection                  m_directionToPerigee;

    enum FitStatusCodes
    {
      Call,
      Success,
      BadInput,
      ExtrapolationFailure,
      ForwardFilterFailure,
      SmootherFailure,
      OutlierFailure,
      PerigeeMakingFailure,
      NoTrkParsToUpdateCompROT,
      nFitStatusCodes
    };
    mutable std::vector< std::vector<int> > m_fitStatistics;
    mutable std::vector< std::vector<int> > m_failuresByIteration;
    enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, nStatIndex = 4};
    std::vector<float> m_etabounds;

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////

//     Track* doDAFfitWithIFitter(
//         const MeasurementSet&,
//         const TrackParameters&,
//         const ParticleHypothesis matEffects=Trk::nonInteracting) const;

    Track* doDAFfitWithKalman(
      const EventContext& ctx,
      const TrackParameters&,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const;
    /** method to create a track from internal trajectory which is common to all interfaces */
    Track*                     makeTrack(const ParticleHypothesis) const;
//    /** special method to build Perigee parameters from the track */
//    const TrackStateOnSurface* makePerigee(const SmoothedTrajectory*,
//                                           const PerigeeSurface&,
//                                           const ParticleHypothesis matEffects=Trk::nonInteracting) const;

    /** special method to build Perigee parameters from the internal KF trajectory */
    const TrackStateOnSurface* internallyMakePerigee(const PerigeeSurface&,
                                                     const ParticleHypothesis matEffects=Trk::nonInteracting) const;

    /** add a measurement (from input track or input measurement set) to internal trajectory */
    void addToTrajectory(const MeasurementBase*,
                         const TrackParameters*,
                         const bool& isOutlier,
                         const int& istate) const;

    /** create a Trk::TrackStateOnSurface from Trk::ProtoTrackStateOnSurface*/
    const Trk::TrackStateOnSurface* createStateFromProtoState(Trk::ProtoTrackStateOnSurface&) const;

    /** do statistics on track fits*/
    void monitorTrackFits(FitStatusCodes, const double& eta, int iteration=0) const;

    /** call the validation tool */
    void callValidation(
      const EventContext& ctx,
      int iterationIndex,
      const Trk::ParticleHypothesis matEffects,
      FitterStatusCode fitStatCode = Trk::FitterStatusCode::Success) const;
};

} // end of namespace

#endif // TRK_DETERMINISTICANNEALINGFILTER_H
