/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DeterministicAnnealingFilter.cxx
//   Source file for class DeterministicAnnealingFilter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkDeterministicAnnealingFilter/DeterministicAnnealingFilter.h"

//#include "GaudiKernel/ToolFactory.h"
//#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/PerigeeSurface.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"

#include "TrkValInterfaces/IValidationNtupleTool.h"

#include "TrkEventUtils/TrkParametersComparisonFunction.h"
//#include "TrkEventUtils/PrepRawDataComparisonFunction.h"
#include "TrkEventUtils/MeasurementBaseComparisonFunction.h"

#include "TrkFitterInterfaces/IForwardKalmanFitter.h"
#include "TrkFitterInterfaces/IKalmanSmoother.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "AtlasDetDescr/AtlasDetectorID.h" // for debug output only

#include <ext/algorithm>

#include <vector>
#include <string>

// constructor
Trk::DeterministicAnnealingFilter::DeterministicAnnealingFilter(const std::string& t,const std::string& n, const IInterface* p) :
AthAlgTool (t,n,p),
m_extrapolator(""),
m_updator("Trk::KalmanUpdator/DAF_KalmanUpdator"),
m_compROTcreator("Trk::CompetingRIOsOnTrackTool/DAF_CompetingRIOsOnTrackTool"),
//m_ITrackFitter(0),
m_forwardFitter("Trk::ForwardKalmanFitter", this),  // retrieve as private tool
m_smoother("Trk::KalmanSmoother", this),            // retrieve as private tool
m_dna(""),
m_FitterValidationTool(""),
m_doValidation(false),
m_haveValidationTool(false),
m_option_callValidationToolForFailedFitsOnly(false),
m_tparScaleSetter(0),
m_utility(0),
//m_inputPreparator(0),
m_directionToPerigee(Trk::oppositeMomentum),
m_fitStatistics(0),
m_failuresByIteration(0) {
    //    m_directionToPerigee = Trk::oppositeMomentum;
    m_option_sortingRefPoint.push_back(0.);
    m_option_sortingRefPoint.push_back(0.);
    m_option_sortingRefPoint.push_back(0.);
    m_option_annealingScheme.push_back(81.);
    m_option_annealingScheme.push_back(9.);
    m_option_annealingScheme.push_back(4.);
    m_option_annealingScheme.push_back(1.);
    m_option_annealingScheme.push_back(1.);
    m_option_annealingScheme.push_back(1.);
    m_trajectory.reserve(100);

    // job options
    declareProperty("ToolForExtrapolation",         m_extrapolator,             "Extrapolator used to calculate initial assignment probabilities and Kalman filter steps");
    declareProperty("ToolForCompetingROTsCreation", m_compROTcreator,           "CompetingRIOsOnTrackCreator used to update assignment probabilities of the measurements");
    declareProperty("ToolForUpdating",              m_updator,                  "Measurement updator used in Kalman filter steps");
    declareProperty("FitterValidationTool",         m_FitterValidationTool,     "Tool for fitter validation");
    declareProperty("callValidationToolForFailedFitsOnly",  m_option_callValidationToolForFailedFitsOnly,   "call the validation tool given by FitterValidationToolHandle only for failed fits");
    declareProperty("AnnealingScheme",              m_option_annealingScheme,   "Annealing scheme (vector<Temperature>) ");
    declareProperty("doHitSorting",                 m_option_doHitSorting=true, "Should hits be sorted before fit?");
    declareProperty("sortingReferencePoint",        m_option_sortingRefPoint,   "Reference point for hit sorting");
    declareProperty("generatePerigeeParameters",    m_option_generatePerigee=true,  "Should Perigee parameters be generated after fit?");
    declareProperty("OutlierCutValue",              m_option_OutlierCut=0.2,    "Cut value: Measurements with smaller assignment probability are marked as outliers on the final track");
    declareProperty("DropOutlierCutValue",          m_option_DropOutlierCut=1.e-20, "Cut value: Measurements with smaller assignment probability are ignored in the fit and dropped from the final track");
    declareProperty("ForwardKalmanFitter",          m_forwardFitter,            "Kalman Forward Filter tool");
    declareProperty("KalmanSmoother",               m_smoother,                 "Kalman smoother tool");
    declareProperty("DynamicNoiseAdjustorHandle",   m_dna,                      "Dynamic noise adjustment for electron E-loss");

    // AlgTool stuff
    declareInterface<ITrackFitter>( this );
}

// destructor
Trk::DeterministicAnnealingFilter::~DeterministicAnnealingFilter() {}


// initialize
StatusCode Trk::DeterministicAnnealingFilter::initialize() {
    StatusCode s = AthAlgTool::initialize();
    if (s.isFailure()) return s;

    StatusCode sc;

    if (msgLvl(MSG::DEBUG)) {
        //m_log << MSG::DEBUG<<"ToolForExtrapolation : "<< m_option_ExtrapTool<<endmsg;
        //m_log << MSG::DEBUG<<"ToolForCompetingROTsCreation: "<< m_option_compROTcreator<<endmsg;
        //m_log << MSG::DEBUG<<"ToolForUpdating             : "<< m_option_UpdatingTool<<endmsg;
        //m_log << MSG::DEBUG<<"ITrackFitterTool            : "<< m_option_KalmanFitter << (m_option_KalmanFitter=="none" ? " i.e. Kalman Forward Filter and Backward Smoother are used directly" : "")<<endmsg;
        //m_log << MSG::DEBUG<<"FitterValidationTool        : "<< m_option_FitterValidationTool<<endmsg;
        msg(MSG::DEBUG) <<"AnnealingScheme             : "<< m_option_annealingScheme<<endmsg;
        msg(MSG::DEBUG)<<"OutlierCutValue             : "<< m_option_OutlierCut<<endmsg;
        msg(MSG::DEBUG)<<"doHitSorting                : "<< m_option_doHitSorting<<endmsg;
        msg(MSG::DEBUG)<<"generatePerigeeParameters   : "<< m_option_generatePerigee<<endmsg;
    }
    // Get Extrapolator
    if ( ! m_extrapolator.empty() ) {
        sc = m_extrapolator.retrieve();
        if (sc.isFailure()) {
            ATH_MSG_FATAL( "Could not retrieve the extrapolator: "<< m_extrapolator );
            return sc;
        }
    } else {
        ATH_MSG_WARNING("No Tool for extrapolation given.");
        msg(MSG::WARNING) << "If ever a set of PrepRawData or MeasurementBase is given to a fit,"
        << " your code will fail." << endmsg;
    }

    // Get CompetingRIOsOnTrackTool
    sc = m_compROTcreator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve "<< m_compROTcreator <<" (for CompetingRIOsOnTrack creation) ");
        return sc;
    }
    // Get ITrackFitter resp. Kalman Forward Filter and Backward Smoother
    //if (m_option_KalmanFitter!="none") {
//     if (false) {
//         // get ITrackFitter
//         ListItem lKalmanTool(m_option_KalmanFitter);
//         sc=toolSvc()->retrieveTool(lKalmanTool.type(),lKalmanTool.name(),m_ITrackFitter);
//         if(sc.isFailure())   {
//             ATH_MSG_FATAL("Could not get "<<lKalmanTool.type()<<" (as Fitter) ");
//             m_ITrackFitter=0;
//             return sc;
//         } else {
//             m_log << MSG::DEBUG<<"Found ITrackFitter, \""
//             << lKalmanTool.type()<<"\"." <<endmsg;
//         }
//     } else {

    // Get Kalman sub-tools

    // Get Kalman measurement updator tool
    sc = m_updator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve "<< m_updator <<" (for measurement update) ");
        return sc;
    }
    // get Forward Filter and Backward Smoother as private tools
    sc = m_forwardFitter.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve "<< m_forwardFitter <<" (Kalman Forward Filter) ");
        return sc;
    }
    sc = m_smoother.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve "<< m_smoother <<" (Kalman Smoother) ");
        return sc;
    }
    if (!m_dna.empty()) {
      if (m_dna.retrieve().isFailure()) {
        ATH_MSG_ERROR( "DNA is configured but tool is not accessible - " << m_dna.typeAndName() );
        return StatusCode::FAILURE;
      } else ATH_MSG_INFO( "retrieved tool " << m_dna.typeAndName() );
    }

    // configure ForwardKalmanFitter
    sc = m_forwardFitter->configureWithTools((m_extrapolator?(&(*m_extrapolator)):0),
                                             &(*m_updator),
                                             0,  // no ROT creator needed!
                                             (!m_dna.empty()?(&(*m_dna)):0),  // dynamic noise adjustment tool
                                             0); // no alignable Surface Provider
    if(sc.isFailure()) return sc;
    // configure KalmanSmoother
    sc = m_smoother->configureWithTools( (m_extrapolator?(&(*m_extrapolator)):0),
                                         &(*m_updator),
                                         (!m_dna.empty()?(&(*m_dna)):0),  // dynamic noise adjustment tool
                                         0,       // no alignable Surface Provider
                                         true,    // always do smoothing
                                         false);  // no creation of FitQualityOnSurface objects, because the Kalman smoother
                                        //    does not take assignment probabilities into account,
                                        //    so the DAF has to create them after the final fit.
    if(sc.isFailure()) return sc;
    //}

    // set TrackParameter sorting function
    Amg::Vector3D refGP(m_option_sortingRefPoint[0],
                              m_option_sortingRefPoint[1],
                              m_option_sortingRefPoint[2]);
    m_tparScaleSetter = new Trk::TrkParametersComparisonFunction(refGP);
    ATH_MSG_INFO("The reference point for sorting will be "
            << refGP.x() << ", " << refGP.y() << ", " << refGP.z() << ".");
    if ( (refGP.x()!=0.0) || (refGP.y()!=0.0) || (refGP.z()!=0.0)) {
        m_directionToPerigee = Trk::anyDirection;
        ATH_MSG_INFO( "non-zero ref. point, so Perigee expected at "<< "any side of the track." );
    }

    // Get Validation Tool
    if ( ! m_FitterValidationTool.empty() ) {
        sc = m_FitterValidationTool.retrieve();
        if (sc.isFailure()) {
            ATH_MSG_FATAL( "Could not retrieve validation tool: "<< m_FitterValidationTool );
            return sc;
        }
        m_haveValidationTool = true;
        if (!m_option_callValidationToolForFailedFitsOnly) m_doValidation = true;
    } else {
        ATH_MSG_DEBUG("No Tool for validation given.");
        m_haveValidationTool = false;
        m_doValidation = false;
    }

    if (msgLvl(MSG::DEBUG)) {
        // Set up ATLAS ID helper to be able to identify the measurement's det-subsystem.
        const AtlasDetectorID* idHelper = 0;
        if (detStore()->retrieve(idHelper, "AtlasID").isFailure()) {
          ATH_MSG_ERROR ("Could not get AtlasDetectorID helper");
          return StatusCode::FAILURE;
         }
        m_utility = new ProtoTrajectoryUtility(idHelper);
    } else {
        m_utility = new ProtoTrajectoryUtility();
    }
//    m_inputPreparator = new TrackFitInputPreparator();

    std::vector<int> statVec(nStatIndex, 0);
    m_fitStatistics.resize(nFitStatusCodes, statVec);
    m_failuresByIteration.resize(m_option_annealingScheme.size(), statVec);

    return StatusCode::SUCCESS;
}



// finalize
StatusCode Trk::DeterministicAnnealingFilter::finalize() {
    delete m_tparScaleSetter;
    delete m_utility;
//    delete m_inputPreparator;
    ATH_MSG_INFO( " Fitter statistics for " << name() );
    if (msgLvl(MSG::INFO)) {
        int iw=9;
        std::cout << "-------------------------------------------------------------------------------" << std::endl;
        std::cout << "  track fits by eta range          ------All---Barrel---Trans.-- Endcap-- " << std::endl;
        std::vector<std::string> statusNames(0);
        statusNames.push_back("  Number of fitter calls          :");
        statusNames.push_back("  Number of successful track fits :");
        statusNames.push_back("  Number of calls with bad input  :");
        statusNames.push_back("  Number of extrapolation failures:");
        statusNames.push_back("  fits with failed forward filter :");
        statusNames.push_back("  fits with failed smoother       :");
        statusNames.push_back("  fits w/ failed outlier strategy :");
        statusNames.push_back("  fits w/ failed perigee making   :");
        for (unsigned int i=0; i<statusNames.size(); i++) {
            std::cout << (statusNames[i]) << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[i])[iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[i])[iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[i])[iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_fitStatistics[i])[iEndcap] << std::endl;
        }
        for (unsigned int i=0; i<m_option_annealingScheme.size(); i++) {
            std::cout << "  failure in annealing iteration " << i << ":" << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_failuresByIteration[i])[iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_failuresByIteration[i])[iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_failuresByIteration[i])[iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                    << (m_failuresByIteration[i])[iEndcap] << std::endl;
        }
        std::cout << "-------------------------------------------------------------------------------" << std::endl;
    }
    ATH_MSG_INFO( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}



//////////////////////////////////////////////
/// refit a track
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit(const Trk::Track&  inputTrack,
        const RunOutlierRemoval,
        const Trk::ParticleHypothesis matEffects) const {

    ATH_MSG_VERBOSE( "--> enter DeterministicAnnealingFilter::fit(Track,,)" );

    // protection against track not having any parameters
    if (inputTrack.trackParameters()->empty()) {
        ATH_MSG_FATAL( "need estimated track parameters near origin, reject fit" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    // protection against not having measurements on the input track
    if (!inputTrack.trackStateOnSurfaces() || inputTrack.trackStateOnSurfaces()->size() < 2) {
        ATH_MSG_WARNING( "called to refit empty track or track with too little information, reject fit" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }

    //  determine the Track Parameter which is the start of the trajectory,
    //  i.e. closest to the origine
    ATH_MSG_VERBOSE( "get track parameters near origine" );
    const TrackParameters* minPar = *(std::min_element(inputTrack.trackParameters()->begin(),
                                                        inputTrack.trackParameters()->end(),
                                                        *m_tparScaleSetter));
    
    if (msgLvl(MSG::VERBOSE)) {
        DataVector<const TrackParameters>::const_iterator it
        = inputTrack.trackParameters()->begin();
        DataVector<const TrackParameters>::const_iterator itEnd
        = inputTrack.trackParameters()->end();
        Amg::Vector3D refGP(  m_option_sortingRefPoint[0],
                                    m_option_sortingRefPoint[1],
                                    m_option_sortingRefPoint[2] );
        for( ; it!=itEnd; it++) {
            msg(MSG::VERBOSE) << " radius of TrackPar is: " <<
            (*it)->position().mag() << ", to ref is " <<
            ((*it)->position()-refGP).mag() << endmsg;
        }
        ATH_MSG_VERBOSE( " (those track parameters were not shown sorted)" );
    }

    //monitorTrackFits( Call, (minPar ? minPar->eta() : 100.) );

    // --------------------------------------------------------------
    // extract measurements from input track and
    // try to build CompetingRIOsOnTrack out of single RIO_OnTrack
    // (this makes sense e.g. for the TRT where you have ambiguities and to
    // allow the DAF-logic to identify outliers)
    // CompetingRIOsOnTrack which include several RIO_OnTrack can not be produced
    // here. That has to be done in pattern recognition.
    // in CompetingRIOsOnTrack the annealing factor is set again

    ATH_MSG_VERBOSE( "extract MeasurementSet from Track" );
    ATH_MSG_VERBOSE( "Track contains " << inputTrack.trackStateOnSurfaces()->size() << " TrackStateOnSurfaces" );
    DataVector<const TrackStateOnSurface>::const_iterator it    = inputTrack.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itEnd = inputTrack.trackStateOnSurfaces()->end();
    int istate=1;
    // loop through TrackStateOnSurfaces to find RIO_OnTrack or CompetingRIOsOnTrack
    for ( ; it!=itEnd; ++it, istate++) {
        if (!(*it)) {
            ATH_MSG_WARNING( "This track contains empty TrackStateOnSurface objects! Skipped them.." );
            continue;
        }
        if (!((*it)->measurementOnTrack())) {
            //if ((*it)->type(Trk::TrackStateOnSurface::Measurement)) {
            //    ATH_MSG_ERROR( "measurementOnTrack == Null for a measurement!" );
            //}
            ATH_MSG_VERBOSE( "Track state #" << (istate>=9?" ":" 0") << istate << " is other than measurement." );
            continue;
        }
        bool outlier = (*it)->type(TrackStateOnSurface::Outlier);
        ATH_MSG_VERBOSE( "Track state #" << (istate>9?" ":" 0") << istate << " is " << (outlier ? "outlier" : "a good" ) << " measurement." );
        //m_trajectory.push_back(ProtoTrackStateOnSurface(((*it)->measurementOnTrack()),outlier,false,istate));

        const TrackParameters* trkPar =  (*it)->trackParameters();
        // add state to the trajectory
        addToTrajectory( (*it)->measurementOnTrack(), trkPar, outlier, istate );
    } // end loop over TrackStateOnSurface

    // ------------------------
    // start the DAF procedure:

    Track* theTrack = 0;
    if (minPar) {
        ATH_MSG_VERBOSE( "got track parameters near origine" );
        // do the DAF fit
        theTrack = doDAFfitWithKalman(*minPar, matEffects);
    } else {
        ATH_MSG_ERROR( "could not get track parameters near origine" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
    }
    m_trajectory.clear();
    if (!theTrack) {
        // iterations failed:
        //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
        if (m_option_callValidationToolForFailedFitsOnly && (!m_doValidation) && m_haveValidationTool) {
            m_doValidation = true;
            if (fit(inputTrack, false, matEffects)) {
                ATH_MSG_WARNING( "Error: fit succeeded! Should not happen, if we repeat a failed fit!" );
            }
            m_doValidation = false;
        }
    }
    // ------------------------
    // return fitted track:
    return theTrack;
}


//////////////////////////////////////////////
/// fit PRD set
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit(const Trk::PrepRawDataSet&  ,// prepRDColl,
        const Trk::TrackParameters& ,// estimatedParametersNearOrigine,
        const RunOutlierRemoval     ,// runOutlier,
        const Trk::ParticleHypothesis  ) const {

    //bool verbose  = (m_log.level() <= MSG::VERBOSE);
    ATH_MSG_ERROR( "fit(PRDset, , ) not implemented" );
    return 0;
}

//////////////////////////////////////////////
/// fit measurement set
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit( const Trk::MeasurementSet&      inputMeasSet,
                                                    const Trk::TrackParameters&     estimatedStartParameters,
                                                    const RunOutlierRemoval,
                                                    const Trk::ParticleHypothesis   matEffects) const {

    ATH_MSG_VERBOSE( "--> entering DeterministicAnnealingFilter::fit(MeasurementSet,TrackParameters,,)" );
    if ( inputMeasSet.empty() ) {
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    //monitorTrackFits( Call, estimatedStartParameters.eta() );

    MeasurementSet::const_iterator it;
    MeasurementSet::const_iterator itEnd;
    MeasurementSet sortedHitSet;

    if (m_option_doHitSorting) {
        // input vector is const, so copy it before sorting.
        sortedHitSet = MeasurementSet(inputMeasSet);
        Trk::MeasurementBaseComparisonFunction* MeasB_CompFunc = new Trk::MeasurementBaseComparisonFunction(
                                                                                estimatedStartParameters.position(),
                                                                                estimatedStartParameters.momentum());
        // sort measColl in increasing distance from origin using STL sorting
        if ( !is_sorted( sortedHitSet.begin(), sortedHitSet.end(), *MeasB_CompFunc ) ) {
            sort( sortedHitSet.begin(), sortedHitSet.end(), *MeasB_CompFunc );
        }
        // some debug output
        if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << "-F- The list of MeasurementBase has been ordered along the initial direction." <<endmsg;
            MeasurementSet::const_iterator it1    = sortedHitSet.begin();
            MeasurementSet::const_iterator it1End = sortedHitSet.end();
            for( ; it1!=it1End; it1++) {
              msg(MSG::VERBOSE) << "-F- radius of globalPos() is " 
                                << (*it1)->globalPosition().mag() << ", transverse r "
                                << (*it1)->globalPosition().perp() << endmsg;
            }
        }
        delete MeasB_CompFunc;
        it    = sortedHitSet.begin();
        itEnd = sortedHitSet.end();
    } else {
        it    = inputMeasSet.begin();
        itEnd = inputMeasSet.end();
    }
    // fill measurements into fitter-internal trajectory: no outlier, external meas't

    for(int istate=1 ; it!=itEnd; it++, istate++) {
        // extrapolate estimated track parameters to surface of measurement
        const Trk::TrackParameters* extrapolatedTrkPar = m_extrapolator->extrapolate(estimatedStartParameters, (*it)->associatedSurface(),
                                                                Trk::alongMomentum, false, matEffects); // TODO: decide: perhaps do not use material interactions here
        if (!extrapolatedTrkPar) {
            ATH_MSG_WARNING( "extrapolation to measurement surface did not succeed during creation of ProtoTrajectory." );
        }
        // add state to the trajectory
        addToTrajectory( (*it),     // measurement
                        extrapolatedTrkPar,    // extrapolated track parameters, if extrapolation succeeded.
                        false,    // no outliers
                        istate );  // state number
        delete extrapolatedTrkPar;
    }
    // ------------------------
    // start the DAF procedure:
    Track* theTrack = doDAFfitWithKalman(estimatedStartParameters, matEffects);
    m_trajectory.clear();
    if (!theTrack) {
        // iterations failed:
        //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
        if (m_option_callValidationToolForFailedFitsOnly && (!m_doValidation) && m_haveValidationTool) {
            m_doValidation = true;
            if (fit(inputMeasSet, estimatedStartParameters, false, matEffects)) {
                ATH_MSG_WARNING( "Error: fit succeeded! Should not happen, if we repeat a failed fit!" );
            }
            m_doValidation = false;
        }
    }
    // ------------------------
    // return fitted track:
    return theTrack;
}

//////////////////////////////////////////////
/// fit track and PRD set
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit(const Track&,
        const PrepRawDataSet&,
        const RunOutlierRemoval,
        const ParticleHypothesis) const {
    //bool verbose  = (m_log.level() <= MSG::VERBOSE);
    ATH_MSG_ERROR( "fit(Track, PRDset, , ) not implemented" );
    return 0;
}

//////////////////////////////////////////////
/// fit track and measurement set
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit( const Trk::Track&               inputTrack,
                                                    const Trk::MeasurementSet&      addMeasColl,
                                                    const Trk::RunOutlierRemoval    runOutlier,
                                                    const Trk::ParticleHypothesis   matEffects ) const {

    // do not use base class method here, because it does an unnecessary deep-copy
    // of all measurements!
    // TODO: decide how to handle the extra information of the given track
    //       (outlier info, track parameters)

    ATH_MSG_VERBOSE( "--> entering DeterministicAnnealingFilter::fit(Track, MeasurementSet,,)" );
    // protection against track not having any parameters
    if (inputTrack.trackParameters()->empty()) {
        ATH_MSG_ERROR( "need estimated track parameters near origine, reject fit" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    // protection against not having Measurements
    if (inputTrack.measurementsOnTrack()->empty()) {
        ATH_MSG_ERROR( "try to fit track+vec<MB> with an empty track, reject fit" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    // protection, if empty MeasurementSet
    if (addMeasColl.empty()) {
        ATH_MSG_ERROR( "try to add an empty MeasurementSet to the track, reject fit" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    

    // create MeasurementBase subset, copy in additional MeasurementSet
    MeasurementSet hitColl;

    // collect MBs from Track
    ATH_MSG_VERBOSE( "add MeasurementBase objects from Track to new set" );
//    DataVector<const MeasurementBase>::const_iterator it    = inputTrack.measurementsOnTrack()->begin();
//    DataVector<const MeasurementBase>::const_iterator itEnd = inputTrack.measurementsOnTrack()->end(); 
//    for ( ; it!=itEnd; ++it) {
    DataVector<const TrackStateOnSurface>::const_iterator it    = inputTrack.trackStateOnSurfaces()->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itEnd = inputTrack.trackStateOnSurfaces()->end();
    // loop through TrackStateOnSurfaces to find RIO_OnTrack or CompetingRIOsOnTrack
    for ( ; it!=itEnd; ++it) {
        if (!(*it)) {
            ATH_MSG_WARNING( "This track contains empty TrackStateOnSurface objects! Skipped them.." );
            continue;
        }
        if (!((*it)->measurementOnTrack())) {
            continue;
        }
        if ( (*it)->type(TrackStateOnSurface::Outlier) ) {
            ATH_MSG_DEBUG( "outlier on track is removed from list of measurements" );
            continue;
        }
        hitColl.push_back ( (*it)->measurementOnTrack() );
    }
    // copy MBs from input list
    MeasurementSet::const_iterator itSet    = addMeasColl.begin();
    MeasurementSet::const_iterator itSetEnd = addMeasColl.end();
    for ( ; itSet!=itSetEnd; ++itSet) {
        if (!(*itSet)) {
            ATH_MSG_WARNING( "There is an empty MeasurementBase object in the track! Skip this object.." );
            continue;
        }
        hitColl.push_back ( (*itSet) );
    }

    // get TrkParameter closest to origine
    ATH_MSG_VERBOSE( "get track parameters near origine" );
    const TrackParameters* minPar = *(std::min_element( inputTrack.trackParameters()->begin(),
                                                        inputTrack.trackParameters()->end(),
                                                        *m_tparScaleSetter ));

    if (!minPar) {
        ATH_MSG_WARNING( "Cannot get valid track parameters from input track, reject fit!" );
        monitorTrackFits( Call, 100. );
        monitorTrackFits( BadInput, 100. );
        return 0;
    }
    // fit set of MeasurementBase using main method,
    //    start with first TrkParameter in inputTrack
    ATH_MSG_VERBOSE( "call fit(MBSet,TP,,)" );
    Track* fittedTrack = fit(hitColl,*minPar, runOutlier, matEffects);
    m_trajectory.clear();
    if (!fittedTrack) {
        // iterations failed:
        //    if m_option_callValidationToolForFailedFitsOnly repeat the track fit with calls of validation tool
        if (m_option_callValidationToolForFailedFitsOnly && (!m_doValidation) && m_haveValidationTool) {
            m_doValidation = true;
            if (fit(inputTrack, addMeasColl, runOutlier, matEffects)) {
                ATH_MSG_WARNING( "Error: fit succeeded! Should not happen, if we repeat a failed fit!" );
            }
            m_doValidation = false;
        }
    }
    return fittedTrack;
}

//////////////////////////////////////////////
/// fit spacepoint set
//////////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit(const SpacePointSet&,
        const TrackParameters&,
        const RunOutlierRemoval,
        const ParticleHypothesis) const {

    //bool verbose  = (m_log.level() <= MSG::VERBOSE);
    ATH_MSG_ERROR( "fit(SpacePointSet, , , ) does not make sense for the Deterministic Annealing Filter, return NULL" );
    return 0;
}
///////////////////////////////////////
// combined fit of two tracks
///////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::fit( const Trk::Track&,
                                                    const Trk::Track&,
                                                    const Trk::RunOutlierRemoval,
                                                    const Trk::ParticleHypothesis ) const {
    ATH_MSG_ERROR( "fit(Track, Track, ...) not implemented yet, return NULL pointer" );
    return 0;
}

/////////////////////////////////
/// doDAFfitWithIFitter -- the main procedure using ITrackFitter
/////////////////////////////////
// Trk::Track* Trk::DeterministicAnnealingFilter::doDAFfitWithIFitter(
//     const MeasurementSet& measSet,
//     const TrackParameters& trkPar,
//     const ParticleHypothesis matEffects) const {
//     
//     if (m_debuglevel) ATH_MSG_VERBOSE( "-----> enter DeterministicAnnealingFilter::doDAFfit()" );
// 
//     //----------------------------------
//     // do the first fit (i.e. annealingIteration==0):
//     // run KalmanFitter without OutlierLogic
//     Track* theTrack = m_ITrackFitter->fit(measSet, trkPar, false, matEffects);
//     if (!theTrack) {
//         ATH_MSG_WARNING( "Kalman fitter has rejected the track, return NULL" );
//         return 0;
//     }
//     if (m_FitterValidationTool) {
//         // write validation data for iteration 0
//         m_FitterValidationTool->writeTrackData(*theTrack, 0);
//     }
//     //----------------------------------
//     // do the annealing iterations:
//     Track* theNewTrack = 0;
//     for (unsigned int annealingIteration=1; annealingIteration < m_option_annealingScheme.size(); annealingIteration++) {
//         ATH_MSG_VERBOSE( "****** starting Annealing Iteration " << annealingIteration << "****** " );
//         // MeasurementSet for the next fit
//         MeasurementSet measColl;
//         // loop through TrackStateOnSurfaces to set annealingFactor in CompetingRIOsOnTrack
//         if (m_debuglevel) {
//             ATH_MSG_VERBOSE( "loop through TrackStateOnSurface to set AnnealingFactor to " << m_option_annealingScheme[annealingIteration] );
//             ATH_MSG_VERBOSE( "    and update CompetingRIOsOnTrack. "  );
//             ATH_MSG_VERBOSE( "Track contains " << theTrack->trackStateOnSurfaces()->size() << " TrackStateOnSurfaces" );
//         }
//         DataVector< const TrackStateOnSurface >::const_iterator it    = theTrack->trackStateOnSurfaces()->begin();
//         DataVector< const TrackStateOnSurface >::const_iterator itEnd = theTrack->trackStateOnSurfaces()->end();
//         for ( ; it!=itEnd; ++it) {
//             if (!((*it)->measurementOnTrack())) {
//                 if ((*it)->type(Trk::TrackStateOnSurface::Measurement)) {
//                     ATH_MSG_ERROR( "measurementOnTrack == Null for a measurement!" );
//                 }
//                 continue;
//             }
//             const TrackParameters* trkPar =  (*it)->trackParameters();
//             if (!trkPar) {
//                 if (m_debuglevel) ATH_MSG_VERBOSE( "current TrackStateOnSurface has no TrackParameters, just use it's MeasurementBase." );
//                 measColl.push_back((*it)->measurementOnTrack());
//                 continue; // next TrackStateOnSurface
//             }
//             // set annealing factor in CompetingRIOsOnTrack
//             const CompetingRIOsOnTrack* compROT = dynamic_cast<const CompetingRIOsOnTrack*>( (*it)->measurementOnTrack() );
//             if (compROT) {
//                 if (m_debuglevel) ATH_MSG_VERBOSE( "current MeasurementBase is a CompetingRIOsOnTrack: set annealing factor" );
//                 m_compROTcreator->updateCompetingROT(*compROT, *trkPar, m_option_annealingScheme[annealingIteration]);
//                 // use CompetingRIOsOnTrack
//                 measColl.push_back(compROT);
//                 continue; // next TrackStateOnSurface
//             } // end if (compROT)
//             // current MeasurementBase not added to measColl yet:
//             if (m_debuglevel) ATH_MSG_VERBOSE( "use MeasurementBase" );
//             measColl.push_back((*it)->measurementOnTrack());
//             //}
//         } // end for (TrackStateOnSurface iterator)
//         if (m_debuglevel) ATH_MSG_VERBOSE( "end of loop: MeasurementSet contains " << measColl.size() << " measurements" );
//         // get Track Parameter closest to origine
//         //ATH_MSG_VERBOSE( "get track parameters near origine" );
//         const TrackParameters* minPar = *(std::min_element(theTrack->trackParameters()->begin(),
//                                           theTrack->trackParameters()->end(),
//                                           *m_tparScaleSetter));
//         if (!minPar) {
//             ATH_MSG_ERROR( "Got no track parameters near origine, stop fitting" );
//             delete theTrack;
//             return 0;
//         }
//         if (m_debuglevel) ATH_MSG_VERBOSE( "Got track parameters near origine" );
//         // fit again with KalmanFitter:
//         theNewTrack = m_ITrackFitter->fit(measColl, *minPar, false, matEffects);
//         //        ATH_MSG_VERBOSE( "use theTrack->trackParameters()->begin() as initial parameters for KalmanFitter" );
//         //        theNewTrack = m_ITrackFitter->fit(measColl, *(*(theTrack->trackParameters()->begin())), false, matEffects);
//         if (!theNewTrack) {
//             ATH_MSG_WARNING( "Kalman fitter has rejected the track, return NULL" );
//             delete theTrack;
//             return 0;
//         }
//         if (m_FitterValidationTool) {
//             // write validation data for iteration with index annealingIteration
//             m_FitterValidationTool->writeTrackData(*theNewTrack, annealingIteration);
//         }
// 
//         // shift our attention to new track:
//         delete theTrack; // be careful: the pointers in measColl are not valid anymore, because they belonged to theTrack!
//         theTrack = theNewTrack;
//     } // end for: annealing iteration loop
//     // there we are...
//     ATH_MSG_INFO( "******* Made a track *********" );
//     return theTrack;
// }


/////////////////////////////////
/// doDAFfitWithKalman -- the main procedure using the Kalman sub-tools directly
/////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::doDAFfitWithKalman(
    const TrackParameters& trkPar,
    const ParticleHypothesis matEffects) const {

    monitorTrackFits( Call, trkPar.eta() );

    ATH_MSG_VERBOSE( "-----> enter DeterministicAnnealingFilter::doDAFfit()" );
    const TrackParameters* estimatedStartParameters = &trkPar;
    const TrackParameters* clonedParameters = 0;
    FitQuality* fitQual  = 0;
    KalmanMatEffectsController controlledMatEff(matEffects, !m_dna.empty());

    //----------------------------------
    // do the annealing iterations:
    for (unsigned int annealingIteration=0; annealingIteration < m_option_annealingScheme.size(); annealingIteration++) {
        ATH_MSG_VERBOSE( "****** starting Annealing Iteration " << annealingIteration << "****** " );
        // setting the new annealing factors and cleaning the trajectory from previous
        // track parameters is not needed in the first (annealingIteration == 0) iteration.
        if (annealingIteration > 0 ) {
            // loop over ProtoTrackStateOnSurfaces to set annealingFactor in CompetingRIOsOnTrack
            ATH_MSG_VERBOSE( "loop through TrackStateOnSurface to set AnnealingFactor to " << m_option_annealingScheme[annealingIteration] );
            ATH_MSG_VERBOSE( "    and update CompetingRIOsOnTrack. "  );
            Trk::Trajectory::iterator it = m_trajectory.begin();
            int i=0;
            for (; it!=m_trajectory.end(); it++, i++) {
                //get first parameters on the track:
                if ( !clonedParameters && !(it->isOutlier())) {
                    if (it->smoothedTrackParameters()) clonedParameters = it->smoothedTrackParameters()->clone();
                }
                const CompetingRIOsOnTrack* compROT = dynamic_cast<const CompetingRIOsOnTrack*>( it->measurement() );
                if (compROT) {
                    if ( !(it->isOutlier()) ) {
                        // set annealing factor in CompetingRIOsOnTrack which
                        // are not marked as outliers
                        const TrackParameters* smoothedTrkPar = it->smoothedTrackParameters();
                        if (!smoothedTrkPar) {
                            ATH_MSG_WARNING( "current ProtoTrackStateOnSurface has no TrackParameters: Can not update CompetingRIOsOnTrack" );
                            continue; // next ProtoTrackStateOnSurface
                        }
                        //if (m_debuglevel) ATH_MSG_VERBOSE( "current MeasurementBase is a CompetingRIOsOnTrack: set annealing factor" );
                        m_compROTcreator->updateCompetingROT(*compROT, *smoothedTrkPar, m_option_annealingScheme[annealingIteration]);
                    } else {
                        // set annealing factor in CompetingRIOsOnTrack which
                        // are marked as outliers (the Kalman forward Fitter marks
                        // measurements as outliers if the measurement update was bad,
                        // and no smoothed TrackParameters are created for them):
                        // extrapolate previous track parameters to the surface of the outlier:

                        // TODO search of the next valid TrackParameters can be coded more elegantly:
                        const Trk::TrackParameters* previousTrkPar = 0;
                        Trk::PropDirection direction = Trk::alongMomentum;
                        Trajectory::const_iterator it2 = it;
                        while (!previousTrkPar) {
                            if (it2 != m_trajectory.begin()) {
                                it2--;
                            } else {
                                break;
                            }
                            if (!(it2->isOutlier()) && (it2->smoothedTrackParameters())) {
                                previousTrkPar = it2->smoothedTrackParameters();
                            }
                        } // end while
                        if (!previousTrkPar) {
                            //previousTrkPar = estimatedStartParameters;
                            while (!previousTrkPar) {
                                if (!it->isOutlier() && (it->smoothedTrackParameters())) {
                                    previousTrkPar = it->smoothedTrackParameters();
                                } else {
                                    if (it == m_trajectory.end()) {
                                        ATH_MSG_WARNING( "can not update assignment probabilities: no useful parameters on track!" );
                                        break;
                                    }
                                }
                                ++it;
                            }
                            if (!previousTrkPar) {
                                continue;
                            }
                            direction = Trk::oppositeMomentum;
                        } // end if (!previousTrkPar)
                        // extrapolate to surface
                        if (previousTrkPar) {
                            const Trk::TrackParameters* extrapolatedTrkPar = m_extrapolator->extrapolate(*previousTrkPar, compROT->associatedSurface(),
                                                                                    direction, false, matEffects); // TODO: decide: perhaps do not use material interactions here
                            if (!extrapolatedTrkPar) {
                                ATH_MSG_WARNING( "Extrapolation to outlier surface did not succeed: Assignment probabilities of outlier cannot be updated" );
                            } else {
                                m_compROTcreator->updateCompetingROT(*compROT, *extrapolatedTrkPar, m_option_annealingScheme[annealingIteration]);
                                delete extrapolatedTrkPar;
                                extrapolatedTrkPar = 0;
                            }
                        }
                    }// end if outlier
                    // set the outlier-flag for measurements which have an extremely small assignment probability,
                    // i.e. they are not used by the forward filter and backward smoother (this saves time and avoids
                    // possible numerical instabilities due to large covariances of the weighted mean measurements)
                    // sum up all assignment probs:
                    double sumAssgnProb = 0.;
                    for (unsigned int index = 0; index < compROT->numberOfContainedROTs(); index++) {
                        sumAssgnProb += compROT->assignmentProbability(index);
                    }
                    if (sumAssgnProb < m_option_DropOutlierCut) {
                        // summed up assignment probability is below cut value,
                        //    mark as outlier
                        it->isOutlier(true);
                    } else {
                        // TODO: decide: do we re-introduce outliers which have a good assignment probability now?
                        it->isOutlier(false);
                    } // end if (sumAssgnProb < m_option_DropOutlierCut)

                    ATH_MSG_VERBOSE( "T"<< i << (it->isOutlier() ? ": X" : ": M" ) << " sumProb=" << sumAssgnProb );

                } // end if (compROT)
            } // end for (ProtoTrackStateOnSurface iterator)
            // get innermost Track Parameter (belonging to first measurement on the trajectory)
            // we need to clone them, because they will be deleted by clearFitResultsAfterOutlier(...)
            // before the new fitting process is started.            
            //clonedParameters = m_smoother->firstMeasurement(m_trajectory)->smoothedTrackParameters()->cloneWithoutError();
            estimatedStartParameters = clonedParameters;
            if (!estimatedStartParameters) {
                ATH_MSG_WARNING( "first measurement ProtoTrackStateOnSurface has no TrackParameters: Use initial track paramters for fit" );
                estimatedStartParameters = &trkPar;
            }

            // clear old track parameters and fit qualities from trajectory
            // delete old states here, instead of using clearFitResultsAfterOutlier() ...
            for (it = m_trajectory.begin(); it!=m_trajectory.end(); it++, i++) {
                if (it->forwardTrackParameters()) delete it->checkoutForwardPar();
                if (it->smoothedTrackParameters()) delete it->checkoutSmoothedPar();
                if (it->fitQuality()) delete it->checkoutFitQuality();
                if (it->dnaMaterialEffects()) delete it->checkoutDNA_MaterialEffects();
            }
            //m_smoother->clearFitResultsAfterOutlier(m_trajectory,fitQual,1);
            delete fitQual;
            fitQual = 0;
            ATH_MSG_VERBOSE( endmsg << "********** call forward kalman filter, iteration #"<< annealingIteration << " **********" << endmsg );
        } // end if(annealingIteration > 0 )

        // fit again with KalmanFitter:
        FitterStatusCode fitstatus = m_forwardFitter->fit(  m_trajectory,
                                                            *estimatedStartParameters,
                                                            false,
                                                            controlledMatEff,
                                                            false,   // do not recreate RIO_OnTrack (this would destroy the CompetingRIOsOnTrack)
                                                            -1);     // "-1" means to use estStartPar and ignore info on trajectory.
        if (msgLvl(MSG::VERBOSE)) m_utility->dumpTrajectory(m_trajectory, name());
        // protect against failed fit
        if (fitstatus.isFailure()) {
            ATH_MSG_DEBUG( "forward fitter #" << annealingIteration << " rejected fit" << endmsg << endmsg );
            monitorTrackFits( ForwardFilterFailure, estimatedStartParameters->eta(), annealingIteration);
            if (m_doValidation) callValidation(annealingIteration, matEffects, fitstatus);
            return 0;
        }
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endmsg
                                    << "********** Forward fit passed, now call smoother #"
                                    << annealingIteration
                                    << ". ************* " << endmsg << endmsg;

        // run backward smoother
        fitstatus = m_smoother->fit(m_trajectory, fitQual, controlledMatEff);
        // call validation tool for smoothed trajectory
        if (m_doValidation) callValidation(annealingIteration, matEffects, fitstatus);
        // protect against failed fit
        if (fitstatus.isFailure()) {
            ATH_MSG_DEBUG( "smoother #" << annealingIteration << " rejected fit" << endmsg << endmsg );
            monitorTrackFits( SmootherFailure, estimatedStartParameters->eta(), annealingIteration );
            return 0;
        }
        if (msgLvl(MSG::VERBOSE)) {
            msg(MSG::VERBOSE) << endmsg
                                 << "************ Backward smoother #" << annealingIteration
                                 << " passed. ************* " << endmsg << endmsg;
            m_utility->dumpTrajectory(m_trajectory, name());
        }
        //if (m_option_doValidationAction) m_extrapolator->validationAction();

        delete clonedParameters;
        clonedParameters = 0;
    } // end for: annealing iteration loop
    // FitQuality is calculate in makeTrack() to take assignment probabilties into account, delete the one
    // made by the smoother
    delete fitQual;
    fitQual = 0;
    return makeTrack(matEffects);
}


///////////////////////////////////////////
/// create a track object from the internal trajectory
///////////////////////////////////////////
Trk::Track* Trk::DeterministicAnnealingFilter::makeTrack(const Trk::ParticleHypothesis  matEffects) const
{
    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE( "--> enter DAF::makeTrack()" );
        m_utility->dumpTrajectory(m_trajectory, name());
    }

    // create EDM-style trajectory
    SmoothedTrajectory* finalTrajectory = new SmoothedTrajectory();
    // create perigee from internal trajectory
    if (m_option_generatePerigee) {
        const Trk::PerigeeSurface   perSurf; // default perigee for InDet tracks -- FIXME
        // Perigee is given as an additional TSoS
        const TrackStateOnSurface*  perState = internallyMakePerigee(perSurf,matEffects);
        if (perState) finalTrajectory->push_back( perState );
    }
    bool dnaFitPresent = m_utility->trajectoryHasMefot(m_trajectory);

    // loop over trajectory to create TrackStateOnSurfaces
    //     from ProtoTrackStateOnSurfaces
    Trajectory::iterator it = m_trajectory.begin();
    int i=0;
    double chi2 = 0.;
    double ndof = 0.;
    for(; it!=m_trajectory.end(); it++, i++) {
        // mark measurements with (nearly) vanishing assignment probabilities
        //   as outliers:
        double sumAssgnProb = 1.;
        const CompetingRIOsOnTrack* compROT = dynamic_cast<const CompetingRIOsOnTrack*>( it->measurement() );
        if (compROT) {
            // sum up all assignment probs:
            sumAssgnProb = 0.;
            for (unsigned int index = 0; index < compROT->numberOfContainedROTs(); index++) {
                sumAssgnProb += compROT->assignmentProbability(index);
            }
            if (sumAssgnProb < m_option_OutlierCut) {
                // sum of assignment probabilities is below cut value,
                //    mark as outlier
                it->isOutlier(true);
            }
        }// end if (is CompetingRIOsOnTrack)
        // do not use measurements with sumAssgnProb < m_option_DropOutlierCut:
        if (sumAssgnProb >= m_option_DropOutlierCut) {
            // FIXME: do not use outliers which are not CompetingRIOsOnTrack for chi2 calculation
            // calc chi increment for track state
            if (i != 0) {
                // calculate the chi2 increment (ignore first state, because it does not contain any
                // information => chi2==0; remark: forward states are predicted ones)
                if ( (it->forwardTrackParameters()) && (it->measurement()) ) {
                   if (msgLvl(MSG::VERBOSE)) {
                        ATH_MSG_VERBOSE( "locMeasPar #"<< i << ": " << it->measurement()->localParameters()[Trk::locX] << "; " << it->measurement()->localParameters() );
                        ATH_MSG_VERBOSE( "locMeasErr #"<< i << ": " << it->measurement()->localCovariance().inverse().eval() );
                        ATH_MSG_VERBOSE( "locTrkPar  #"<< i << ": " << it->forwardTrackParameters()->parameters()[Trk::locX] );
                        if (!it->forwardTrackParameters()->covariance()) {
                            ATH_MSG_VERBOSE( "locTrkPar  #"<< i << " has no covariance matrix");
                        } else {
                            ATH_MSG_VERBOSE( "locTrkCov  #"<< i << ": " << it->forwardTrackParameters()->covariance() );
                        }
                        ATH_MSG_VERBOSE( "surface #"<< i << ": " <<(&(it->measurement()->associatedSurface())) << " at (" << it->measurement()->associatedSurface().center().x() << ", " << it->measurement()->associatedSurface().center().y() << ", " << it->measurement()->associatedSurface().center().z() << ")" );
                    }
                    const Trk::FitQualityOnSurface* fQoS = m_updator->predictedStateFitQuality( *(it->forwardTrackParameters()),
                                                                                                it->measurement()->localParameters(),
                                                                                                it->measurement()->localCovariance().inverse().eval() );
                    if (fQoS) {
                        chi2 += fQoS->chiSquared() * sumAssgnProb;
                        ndof += double(fQoS->numberDoF()) * sumAssgnProb;
                        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "FQoS for state #"<<i<<" (chi2/ndof): " 
                                                << fQoS->chiSquared() << "/" << fQoS->numberDoF() 
                                                << " assgnProb=" << sumAssgnProb 
                                                << "=> " << (fQoS->chiSquared() * sumAssgnProb) << "/" << (double(fQoS->numberDoF()) * sumAssgnProb) << endmsg;
                        delete fQoS;
                    } else {
                        ATH_MSG_DEBUG( "calculation of FQoS failed for state #"<<i<<"; assgnProb=" << sumAssgnProb );
                    } // end if(fQoS)
                } else {
                    ATH_MSG_DEBUG( "no parameter: cannot calculate FQoS for state #"<<i<<"; assgnProb=" << sumAssgnProb );
                    //ATH_MSG_DEBUG( " F:" << it->forwardTrackParameters() << " M:" << (it->measurement()));
                } // end if (trkParameters and Measurement)
            } // end if i!=0

            // create TrackStateOnSurface from ProtoTrackStateOnSurface
            const TrackStateOnSurface* trkState = createStateFromProtoState(*it);
            if (trkState) {
            if (msgLvl(MSG::VERBOSE) && trkState->measurementOnTrack()) {
                msg(MSG::VERBOSE) << "TrackStateOnSurface: measurement: " << trkState->measurementOnTrack();
                const CompetingRIOsOnTrack* compRot = dynamic_cast<const CompetingRIOsOnTrack*> (trkState->measurementOnTrack());
                if (compRot) ATH_MSG_VERBOSE( " compRot maxProb=" << compRot->assignmentProbability(compRot->indexOfMaxAssignProb()) << " on surface " << &(compRot->associatedSurface()));
                else ATH_MSG_VERBOSE( " on surface " << &(trkState->measurementOnTrack()->associatedSurface()));
            }
             finalTrajectory->push_back( trkState );
            }else ATH_MSG_WARNING( "fitter inconsistency - no track state #"<<i<<" available!" );
        } else {
            // we do not put this measurement on the track, because it is far-off
            ATH_MSG_DEBUG( "drop track state #"<<i<<" from track; assgnProb=" << sumAssgnProb );
        } // end if (sumAssgnProb >= m_option_DropOutlierCut)
    } // end loop over track states
    //ATH_MSG_VERBOSE( "extracted " << i << " states from DAF internal trajectory." );
    if (msgLvl(MSG::VERBOSE)) m_utility->dumpTrajectory(m_trajectory, name());
    // create the fit quality
    Trk::FitQuality* FQ = new Trk::FitQuality( chi2, int(ndof) );
    // create the track
    Trk::TrackInfo info(TrackInfo::DeterministicAnnealingFilter, matEffects);
    if (!m_dna.empty()) info.setTrackProperties (TrackInfo::BremFit);
    if (dnaFitPresent)  info.setTrackProperties (TrackInfo::BremFitSuccessful);

    Trk::Track* fittedTrack = new Track(info, finalTrajectory, FQ);
//    Trk::Track* fittedTrack = new Track(Track::DeterministicAnnealingFilter, finalTrajectory, FQ);
    if (fittedTrack) {
        monitorTrackFits( Success, ((*finalTrajectory)[0]->trackParameters() ? (*finalTrajectory)[0]->trackParameters()->eta() : 100. ) );
    } else {
        monitorTrackFits( PerigeeMakingFailure, ((*finalTrajectory)[0]->trackParameters() ? (*finalTrajectory)[0]->trackParameters()->eta() : 100. ) );
    }
    // some debug output
    if (msgLvl(MSG::DEBUG)) {
        if (fittedTrack) {
            msg(MSG::DEBUG) << "********** done, track made with Chi2 = "
                    << fittedTrack->fitQuality()->chiSquared() << " / " 
                    << fittedTrack->fitQuality()->numberDoF() << " ********** " << endmsg << endmsg;
        } else {
            msg(MSG::DEBUG) << "####### trouble, NO track has been made. #########" 
                    << endmsg << endmsg;
        }
    }
    return fittedTrack;
}

///////////////////////////////////////////
/// create a perigee from smoothedTrajectory
///////////////////////////////////////////
// const Trk::TrackStateOnSurface* Trk::DeterministicAnnealingFilter::makePerigee(
//                                                                     const SmoothedTrajectory* trajectory,
//                                                                     const Trk::PerigeeSurface& perSurf,
//                                                                     const Trk::ParticleHypothesis  matEffects) const {
//     // strange effect: this line does only work with a _const_ smooTraj.
//     //        const Trk::TrackParameters* param = (smooTraj->back())->trackParameters();
//     const TrackStateOnSurface*  tmpTSOS = trajectory->back();
//     const Trk::TrackParameters* param   = tmpTSOS->trackParameters();
//     if (!param) {
//         ATH_MSG_FATAL( "empty TrackPar in TSoS - can not make Perigee." );
//         return 0;
//     }
//     // extrapolate to perigee
//     const Trk::TrackParameters* per = m_extrapolator->extrapolate(*param, perSurf, m_directionToPerigee, false, matEffects);
//     if (!per) {
//         ATH_MSG_WARNING( "Perigee-making failed: extrapolation did not succeed." );
//         return 0;
//     } else {
//         // 2004's Trk::Track approach (EJWM): put perigee into a TSoS.
//         return new TrackStateOnSurface(0 , per, 0,  0, TrackStateOnSurface::Perigee );
//     }
// }

///////////////////////////////////////////
/// create a perigee from internal trajectory
///////////////////////////////////////////
const Trk::TrackStateOnSurface* Trk::DeterministicAnnealingFilter::internallyMakePerigee(
                                                                        const Trk::PerigeeSurface&     perSurf,
                                                                        const Trk::ParticleHypothesis  matEffects) const {
    const Trk::TrackParameters* nearestParam   = 0;
    Trajectory::const_iterator it = m_trajectory.begin();
    while (!nearestParam) { // FIXME this can be coded more elegantly
        if (!it->isOutlier() && (it->smoothedTrackParameters())) {
            nearestParam = it->smoothedTrackParameters();
        } else {
            if (it == m_trajectory.end()) {
                ATH_MSG_ERROR( "Perigee-making failed: no useful parameters on track!" );
                return 0;
            }
        }
        ++it;
    }
    // extrapolate to perigee
    const Trk::TrackParameters* per = m_extrapolator->extrapolate(*nearestParam, perSurf,
                                                                  m_directionToPerigee, false, matEffects);
    if (!per) {
        ATH_MSG_WARNING( "Perigee-making failed: extrapolation did not succeed." );
        return 0;
    } else {
        ATH_MSG_VERBOSE( "Perigee parameters have been made." );
    }
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Perigee); 
    TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(0, per, 0, 0, typePattern); 
    return tsos;
}

//////////////////////////////////////////////
/// add a measurement (from input track or input measurement set) to internal trajectory */
//////////////////////////////////////////////
void Trk::DeterministicAnnealingFilter::addToTrajectory(const MeasurementBase* measurement,
                                                        const TrackParameters* trkPar,
                                                        const bool& isOutlier,
                                                        const int& istate) const {
    // current handling of outliers:
    //      - outliers which are CompetingRIOsOnTrack are taken into account again
    //        (not used as outliers)
    //      - outliers which are RIO_OnTrack but could be converted to CompetingRIOsOnTrack
    //        are taken into account again (not used as outliers)
    //      - outliers which could not be converted to CompetingRIOsOnTrack are not used in the fit

    // if we have a CompetingRIOsOnTrack, we have to clone it, because
    // the annealing procedure modifies its assignment probabilities.
    const CompetingRIOsOnTrack* compROT = dynamic_cast<const CompetingRIOsOnTrack*>( measurement );
    if (compROT) {
        ATH_MSG_VERBOSE( "current MeasurementBase is a CompetingRIOsOnTrack: clone and set annealing factor" );
        // TODO: decide about setting of annealingFactor, if no track parameters are given: extraploate?
        const CompetingRIOsOnTrack* newCompROT = compROT->clone();
        if (trkPar) {
            // set annealing factor in CompetingRIOsOnTrack
            m_compROTcreator->updateCompetingROT(*newCompROT, *trkPar, m_option_annealingScheme[0]);
        }
        // use CompetingRIOsOnTrack
        m_trajectory.push_back(ProtoTrackStateOnSurface(    newCompROT,
                                                            false,      // do not count as outlier
                                                            true,       // ProtoTrackState owns the measurement, because it was cloned here
                                                            istate) );  // state number
        return;
    } // end if (compROT)

    if (!trkPar) {
        ATH_MSG_DEBUG( "no TrackParameters for given measurement, so we can not produce CompetingRIOsOnTrack, just use it's MeasurementBase." );
        m_trajectory.push_back(ProtoTrackStateOnSurface( measurement,
                                                         isOutlier,   // isOutlier? (use flag of the input track)
                                                         false,     // ProtoTrackState does not own the measurement, it belongs to the input track
                                                         istate) ); // state number
        return;
    } //else {
    //    if (m_debuglevel) ATH_MSG_VERBOSE( "Got the TrackParameters." );
    //}

    // try to convert RIO_OnTrack in CompetingRIOsOnTrack
    const RIO_OnTrack* ROT = dynamic_cast<const RIO_OnTrack*>( measurement );
    if (ROT) {
        ATH_MSG_VERBOSE( "current MeasurementBase is a RIO_OnTrack: try to make CompetingRIOsOnTrack out of it" );
        // build  prepRawData list (with one entry, wow!):
        std::list< const PrepRawData* >* PRDlist = new std::list< const PrepRawData* >;
        PRDlist->push_back(ROT->prepRawData() );
        //const std::list< const PrepRawData* >* constPRDlist = PRDlist;
        // call CompetingRIOsOnTrackTool and try to create a CompetingRIOsOnTrack
        //const double beta = m_option_annealingScheme[0];
        const Trk::CompetingRIOsOnTrack* compROT = m_compROTcreator->createCompetingROT(*PRDlist, *trkPar, m_option_annealingScheme[0]);
        delete PRDlist;
        if (compROT) {
            // use CompetingRIOsOnTrack
            m_trajectory.push_back(ProtoTrackStateOnSurface(    compROT,
                                                                false,      // do not count as outlier
                                                                true,       // ProtoTrackState owns the measurement, because it was created here
                                                                istate) );  // state number
            ATH_MSG_VERBOSE( "CompetingRIOsOnTrack created: use it instead of RIO_OnTrack" );
            return;
        } else {
            // use RIO_OnTrack
            m_trajectory.push_back(ProtoTrackStateOnSurface(    ROT,
                                                                isOutlier,   // isOutlier? (use flag of the input track)
                                                                false,     // ProtoTrackState does not own the measurement, it belongs to the input track
                                                                istate) ); // state number
            ATH_MSG_VERBOSE( "CompetingRIOsOnTrack was not created: use original RIO_OnTrack" );
            return;
        }
    } // end if(ROT)
    // current MeasurementBase not added to trajectory yet:
    m_trajectory.push_back(ProtoTrackStateOnSurface(    measurement,
                                                        isOutlier,   // isOutlier? (use flag of the input track)
                                                        false,     // ProtoTrackState does not own the measurement, it belongs to the input track
                                                        istate) ); // state number
    ATH_MSG_VERBOSE( "type of MeasurementBase not identified, just use it" );
    return;
}


const Trk::TrackStateOnSurface* Trk::DeterministicAnnealingFilter::createStateFromProtoState(ProtoTrackStateOnSurface& protoState) const {
    if (!(protoState.measurement())) return 0;
    // set correct TrackStateOnSurfaceType
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    // create the TrackStateOnSurface
    const Trk::MaterialEffectsBase* mefot = protoState.dnaMaterialEffects() ?
      protoState.dnaMaterialEffects()->makeMEFOT() : 0;
    if (protoState.isOutlier()) {
        typePattern.set(TrackStateOnSurface::Outlier);
        // do not use the track parameters for outliers
        return new TrackStateOnSurface( protoState.checkoutMeasurement(),
                                          0,0,0,typePattern );
    } else {
        typePattern.set(TrackStateOnSurface::Measurement);
        return new TrackStateOnSurface( protoState.checkoutMeasurement(),   // the measurement
                                        protoState.checkoutSmoothedPar(),   // smoothed track parameter
                                        0,                                  // no fit quality
                                        mefot,                              // no material effects
                                        typePattern );                      // type pattern
    }
}


void Trk::DeterministicAnnealingFilter::monitorTrackFits(FitStatusCodes code, const double& eta, int iteration) const {
    // do not count if fit was repeated to write validation data of failed fit:
    if ( m_option_callValidationToolForFailedFitsOnly && m_doValidation ) return;

    //ATH_MSG_INFO( "Code:" << code << " eta=" << eta );
    ((m_fitStatistics[code])[iAll])++;
    if (fabs(eta) < 0.80 ) ((m_fitStatistics[code])[iBarrel])++;
    else if (fabs(eta) < 1.60) ((m_fitStatistics[code])[iTransi])++;
    else if (fabs(eta) < 2.10) ((m_fitStatistics[code])[iEndcap])++;
    if (code==ForwardFilterFailure || code==SmootherFailure || code==ExtrapolationFailure) {
        ((m_failuresByIteration[iteration])[iAll])++;
        if (fabs(eta) < 0.80 ) ((m_failuresByIteration[iteration])[iBarrel])++;
        else if (fabs(eta) < 1.60) ((m_failuresByIteration[iteration])[iTransi])++;
        else if (fabs(eta) < 2.10) ((m_failuresByIteration[iteration])[iEndcap])++;
    }
    return;
}

void Trk::DeterministicAnnealingFilter::callValidation( int iterationIndex,
                                                        const Trk::ParticleHypothesis  matEffects,
                                                        FitterStatusCode fitStatCode ) const
{
    ATH_MSG_DEBUG( "call validation for track iteration " << iterationIndex << "with status " << fitStatCode.getCode() << "/" << fitStatCode );
    // extrapolate to perigee at origin for validation data
    const Trk::PerigeeSurface   perSurf; // default perigee at origin
    const Trk::TrackParameters* nearestParam   = 0;
    Trajectory::const_iterator it = m_trajectory.begin();
    for ( ; it != m_trajectory.end(); it++ ) { // FIXME this can be coded more elegantly
        if (!it->isOutlier() && (it->smoothedTrackParameters())) {
            nearestParam = it->smoothedTrackParameters();
            break;
        }
    }
    if (!nearestParam) {
        for ( it = m_trajectory.begin(); it != m_trajectory.end(); it++ ) {
            if (!it->isOutlier() && (it->forwardTrackParameters())) {
                nearestParam = it->forwardTrackParameters();
                break;
            }
        }
    }
    const Trk::Perigee* per = 0;
    const Trk::TrackParameters* perPar = 0;
    if (nearestParam) {
        // extrapolate to perigee
        perPar = m_extrapolator->extrapolate(   *nearestParam, perSurf,
                                                m_directionToPerigee,
                                                false, matEffects);
        per = dynamic_cast<const Trk::Perigee*>(perPar);
    } else {
        ATH_MSG_WARNING( "Perigee-making for validation failed: no useful parameters on track!" );
    }
    // write validation data for iteration with index
    StatusCode sc = m_FitterValidationTool->writeProtoTrajectoryData(m_trajectory, iterationIndex, per, fitStatCode.getCode());
    // FICME. just ignore as this is only for validation.
    sc.ignore(); 
    delete perPar;
    return;
}

