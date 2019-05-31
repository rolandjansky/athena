/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      GaussianSumFitter.cxx  -  description
      -------------------------------------
begin                : Monday 7th March 2005
author               : amorley atkinson
email                : Anthony.Morley@cern.ch Tom.Atkinson@cern.ch
decription           : Implementation code for Gaussian Sum Fitter class
********************************************************************************** */

#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkGaussianSumFilter/IForwardGsfFitter.h"
#include "TrkGaussianSumFilter/IGsfSmoother.h"

#include "TrkEventUtils/MeasurementBaseComparisonFunction.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"

#include "GaudiKernel/Chrono.h"

#include <vector>
#include <algorithm>



Trk::GaussianSumFitter::GaussianSumFitter(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_extrapolator("Trk::GsfExtrapolator/GsfExtrapolator"),
  m_updator("Trk::GsfMeasurementUpdator/GsfMeasurementUpdator"),
  m_rioOnTrackCreator("Trk::RioOnTrackCreator/RIO_OnTrackCreator"),
  m_forwardGsfFitter("Trk::ForwardGsfFitter/ForwardGsfFitter"),
  m_gsfSmoother("Trk::GsfSmoother/GsfSmoother"),
  m_reintegrateOutliers(false),
  m_makePerigee(true),
  m_directionToPerigee(Trk::oppositeMomentum),
  m_refitOnMeasurementBase(true),
  m_doHitSorting(true),
  m_trkParametersComparisonFunction(0),
  m_stateCombiner("Trk::MultiComponentStateCombiner"),
  m_chronoSvc("ChronoStatSvc", name),
  m_inputPreparator(nullptr),
  m_FitPRD{0},
  m_FitMeasuremnetBase{0},
  m_FowardFailure{0},
  m_SmootherFailure{0},
  m_PerigeeFailure{0},
  m_fitQualityFailure{0}
{

  declareInterface<ITrackFitter>(this);

  declareProperty("ToolForExtrapolation",     m_extrapolator              );
  declareProperty("MeasurementUpdatorType",   m_updator                   );
  declareProperty("ToolForROTCreation",       m_rioOnTrackCreator         );
  declareProperty("ReintegrateOutliers",      m_reintegrateOutliers       );
  declareProperty("MakePerigee",              m_makePerigee               );
  declareProperty("RefitOnMeasurementBase",   m_refitOnMeasurementBase    );
  declareProperty("DoHitSorting",             m_doHitSorting              );
  declareProperty("SortingReferencePoint",    m_sortingReferencePoint     );
  declareProperty("StateCombiner",            m_stateCombiner             );
  declareProperty("GsfSmoother",m_gsfSmoother);  
  declareProperty("ForwardGsfFitter",m_forwardGsfFitter);

  // Estrablish reference point as origin
  m_sortingReferencePoint.push_back(0.);
  m_sortingReferencePoint.push_back(0.);
  m_sortingReferencePoint.push_back(0.);
}

StatusCode Trk::GaussianSumFitter::initialize()
{

  StatusCode sc;

  // Request the Chrono Service
  ATH_CHECK( m_chronoSvc.retrieve() );

  // Request the GSF forward fitter - hardwired type and instanace name for the GSF
  ATH_CHECK( m_forwardGsfFitter.retrieve());

  // Request the GSF smoother - hardwired type and instance name for the GSF
  ATH_CHECK( m_gsfSmoother.retrieve() );

  // Request the GSF measurement updator - hardwired type and instance name for the GSF
  ATH_CHECK ( m_updator.retrieve() );

  // Request the GSF extrapolator
  ATH_CHECK ( m_extrapolator.retrieve() );

  // Request the state combiner
  ATH_CHECK ( m_stateCombiner.retrieve() );

  // Request the RIO_OnTrack creator
  // No need to return if RioOnTrack creator tool, only if PrepRawData is used in fit
  if( m_rioOnTrackCreator.retrieve().isFailure() ){

    if (!m_refitOnMeasurementBase){
      msg(MSG::FATAL) << "Attempting to use PrepRawData with no RIO_OnTrack creator tool provided... Exiting!" << endmsg;
      return StatusCode::FAILURE;
    }

    else
      ATH_MSG_INFO( "Request to retrieve the RIO_OnTrack Creator failed but track is fit at the MeasurementBase level... Continuing!");

  }

  // Initialise the closest track parameters search algorithm
  Amg::Vector3D referencePosition( m_sortingReferencePoint[0],
           m_sortingReferencePoint[1],
           m_sortingReferencePoint[2] );

  m_trkParametersComparisonFunction = new Trk::TrkParametersComparisonFunction( referencePosition );

  if (!m_trkParametersComparisonFunction) {
    msg(MSG::FATAL) << "Request to instansiate the ClosestTrackParametersFinder object failed... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Configure forward fitter
  sc = m_forwardGsfFitter->configureTools(m_extrapolator, m_updator, m_rioOnTrackCreator );

  if ( sc.isFailure() ) {
    msg(MSG::FATAL) << "Could not configure the forwards GSF fitter... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Configure smoother
  sc = m_gsfSmoother->configureTools(m_extrapolator, m_updator);

  if ( sc.isFailure() ) {
    msg(MSG::FATAL) << "Could not configure the GSF smoother... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  //GSF Statistics Setup;
  m_FitPRD              = 0;      // Number of Fit PrepRawData Calls
  m_FitMeasuremnetBase  = 0;      // Number of Fit MeasurementBase Calls
  m_FowardFailure       = 0;      // Number of Foward Fit Failures:       
  m_SmootherFailure     = 0;      // Number of Smoother Failures:         
  m_PerigeeFailure      = 0;      // Number of MakePerigee Failures:  
  m_fitQualityFailure   = 0;      
 
  m_inputPreparator = new TrackFitInputPreparator();

  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GaussianSumFitter::finalize()
{


  // Delete dynamic memory
  delete m_trkParametersComparisonFunction;
  delete m_inputPreparator;

  msg(MSG::INFO) << "-----------------------------------------------" << endmsg;
  msg(MSG::INFO) << "            Some Brief GSF Statistics          " << endmsg;
  msg(MSG::INFO) << "-----------------------------------------------" << endmsg;

  msg(MSG::INFO) << "Number of Fit PrepRawData Calls:          "<< m_FitPRD             << endmsg;
  msg(MSG::INFO) << "Number of Fit MeasurementBase Calls:      "<< m_FitMeasuremnetBase << endmsg;
  msg(MSG::INFO) << "Number of Forward Fit Failures:           "<< m_FowardFailure      << endmsg;
  msg(MSG::INFO) << "Number of Smoother Failures:              "<< m_SmootherFailure    << endmsg;
  msg(MSG::INFO) << "Number of MakePerigee Failures:           "<< m_PerigeeFailure     << endmsg;
  msg(MSG::INFO) << "Number of Trks that fail fitquality test: "<< m_fitQualityFailure  << endmsg;
  msg(MSG::INFO) << "-----------------------------------------------" << endmsg;

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

#if 0
StatusCode Trk::GaussianSumFitter::configureTools(const IMultiStateMeasurementUpdator* measurementUpdator, const IRIO_OnTrackCreator* rioOnTrackCreator)
{

  msg(MSG::INFO) << "Configuring the GaussianSumFilter!" << endmsg;

  StatusCode sc;

  m_updator = measurementUpdator;
  m_rioOnTrackCreator = rioOnTrackCreator;

  // Configure forward fitter
  sc = m_forwardGsfFitter->configureTools(m_extrapolator, m_updator, m_rioOnTrackCreator );

  if ( sc.isFailure() ) {
    msg(MSG::FATAL) << "Could not configure the forwards GSF fitter... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Configure smoother
  sc = m_gsfSmoother->configureTools(m_extrapolator, m_updator);

  if ( sc.isFailure() ) {
    msg(MSG::FATAL) << "Could not configure the GSF smoother... Exiting!" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "Configuration of Gaussian Sum Fitter successful" << endmsg;

  return StatusCode::SUCCESS;

}
#endif

/* =========================================================================================================================================
   =========================================================================================================================================

   Refitting of a track

   =========================================================================================================================================
   ========================================================================================================================================= */

Trk::Track* Trk::GaussianSumFitter::fit ( const Trk::Track&             inputTrack,
                                          const Trk::RunOutlierRemoval  outlierRemoval,
                                          const Trk::ParticleHypothesis particleHypothesis ) const
{

  if (msgLvl(MSG::VERBOSE))
    msg() << "Trk::GaussianSumFilter::fit() - Refitting a track" << endmsg;

  // Start the timer
  Chrono chrono( &(*m_chronoSvc), name() );

  // Check that the input track has well defined parameters
  if ( inputTrack.trackParameters()->empty() ) {
    msg(MSG::FATAL) << "No estimation of track parameters near origin... Exiting!" << endmsg;
    return 0;
  }

  // Check that the input track has associated MeasurementBase objects
  if ( inputTrack.trackStateOnSurfaces()->empty() ) {
    msg(MSG::FATAL) << "Attempting to fit track to empty MeasurementBase collection... Exiting!" << endmsg;
    return 0;
  }

  // Retrieve the set of track parameters closest to the reference point
  const Trk::TrackParameters* parametersNearestReference = *( std::min_element( inputTrack.trackParameters()->begin(),
                    inputTrack.trackParameters()->end(),
                    *m_trkParametersComparisonFunction ) );

  if (msgLvl(MSG::VERBOSE))
    msg() << "Estimation parameters near reference point: " << *parametersNearestReference << endmsg;

  // If refitting of track is at the MeasurementBase level extract the MeasurementBase from the input track and create a new vector
  if (m_refitOnMeasurementBase) {

    MeasurementSet measurementSet;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = inputTrack.trackStateOnSurfaces()->begin();
    for ( ; trackStateOnSurface != inputTrack.trackStateOnSurfaces()->end(); ++trackStateOnSurface ) {

      if ( !(*trackStateOnSurface) ){
        msg(MSG::WARNING) << "This track contains an empty MeasurementBase object that won't be included in the fit" << endmsg;
        continue;
      }

      if ( (*trackStateOnSurface)->measurementOnTrack() ){
        if ( (*trackStateOnSurface)->type(TrackStateOnSurface::Measurement) ){
          measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
        }
        else if ( m_reintegrateOutliers && (*trackStateOnSurface)->type(TrackStateOnSurface::Outlier) ){
          measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
        }
      }
    }

    // Apply GSF fit to MeasurementBase objects
    return fit( measurementSet, *parametersNearestReference, outlierRemoval, particleHypothesis );

 }

  // If refitting of the track is at the PrepRawData level then extract the PrepRawData objects from the input track
  else {

    PrepRawDataSet prepRawDataSet;

    DataVector<const Trk::MeasurementBase>::const_iterator measurementOnTrack = inputTrack.measurementsOnTrack()->begin();

    for ( ; measurementOnTrack != inputTrack.measurementsOnTrack()->end(); ++ measurementOnTrack ) {

      if ( !(*measurementOnTrack) ){
        msg(MSG::DEBUG) << "This track contains an empty MeasurementBase object... Ignoring object" << endmsg;
        continue;
      }

      // Dynamic cast to a RIO_OnTrack object
      const Trk::RIO_OnTrack* rioOnTrack = nullptr;
      if((*measurementOnTrack)->type(Trk::MeasurementBaseType::RIO_OnTrack)){
        rioOnTrack=static_cast< const Trk::RIO_OnTrack* >(*measurementOnTrack);
      }

      if ( !rioOnTrack){
        msg(MSG::DEBUG) << "Measurement could not be cast as a RIO_OnTrack object... continuing" << endmsg;
        continue;
      }

      const PrepRawData* prepRawData = rioOnTrack->prepRawData();

      if ( !prepRawData ){
        msg(MSG::DEBUG) << "Defined RIO_OnTrack object has no associated PrepRawData object... this object will be ignored in fit" << endmsg;
        continue;
      }

      prepRawDataSet.push_back( prepRawData );

    }

    // Apply GSF fit to PrepRawData objects
    return fit( prepRawDataSet, *parametersNearestReference, outlierRemoval, particleHypothesis );

  }

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Fitting of a set of PrepRawData objects

   =========================================================================================================================================
   ========================================================================================================================================= */

Trk::Track* Trk::GaussianSumFitter::fit ( const Trk::PrepRawDataSet&    prepRawDataSet,
                                          const Trk::TrackParameters&   estimatedParametersNearOrigin,
                                          const Trk::RunOutlierRemoval  outlierRemoval,
                                          const Trk::ParticleHypothesis particleHypothesis ) const
{

  if (msgLvl(MSG::VERBOSE)){
    msg() << "Trk::GaussianSumFilter::fit() - Fitting a set of PrepRawData objects" << endmsg;
    msg() << "Material effects switch: " << particleHypothesis << endmsg;
    msg() << "Outlier removal switch:  " << outlierRemoval << endmsg;
  }
  ++m_FitPRD;

  // Start the timer
  Chrono chrono( &(*m_chronoSvc), name() );


  // Protect against empty PrepRawDataSet object
  if ( prepRawDataSet.empty() ) {
    msg(MSG::FATAL) << "PrepRawData set for fit is empty... Exiting!" << endmsg;
    return 0;
  }

  // A const stl container cannot be sorted. This will re-cast it so that it can.
  Trk::PrepRawDataSet sortedPrepRawDataSet = PrepRawDataSet( prepRawDataSet );

  if ( m_doHitSorting ){

    Trk::PrepRawDataComparisonFunction* prdComparisonFunction = new Trk::PrepRawDataComparisonFunction( estimatedParametersNearOrigin.position(),
                          estimatedParametersNearOrigin.momentum() );
    sort ( sortedPrepRawDataSet.begin(), sortedPrepRawDataSet.end(), *prdComparisonFunction );

    delete prdComparisonFunction;

  }

  // Perform GSF forwards fit
  const ForwardTrajectory* forwardTrajectory = m_forwardGsfFitter->fitPRD( sortedPrepRawDataSet, estimatedParametersNearOrigin, particleHypothesis );

  if ( !forwardTrajectory ) {
    if (msgLvl(MSG::DEBUG)) msg() << "Forward GSF fit failed... Exiting!" << endmsg;
    ++m_FowardFailure;
    return 0;
  }

  if ( forwardTrajectory->empty() ){
    if (msgLvl(MSG::DEBUG)) msg() << "No states in forward trajectory... Exiting!" << endmsg;
    ++m_FowardFailure;
    delete forwardTrajectory;
    return 0;
  }

  if (msgLvl(MSG::VERBOSE)) msg() << "*** Forward GSF fit passed! ***" << endmsg;

  // Perform GSF smoother operation
  SmoothedTrajectory* smoothedTrajectory = m_gsfSmoother->fit( *forwardTrajectory, particleHypothesis );

  // Protect against failed smoother fit
  if ( !smoothedTrajectory ) {
    if (msgLvl(MSG::DEBUG)) msg() << "Smoother GSF fit failed... Exiting!" << endmsg;
    ++m_SmootherFailure;
    delete forwardTrajectory;
    return 0;
  }

  if (msgLvl(MSG::VERBOSE)) msg() << "*** GSF smoother fit passed! ***" << endmsg;

  // Outlier m_logic and track finalisation
  const FitQuality* fitQuality = buildFitQuality( *smoothedTrajectory );

  if ( !fitQuality ){
    ATH_MSG_DEBUG("Chi squared could not be calculated... Bailing" );
    ++m_fitQualityFailure;
    delete forwardTrajectory;
    delete smoothedTrajectory;
    return 0;
  }

  Track* fittedTrack = 0;

  if (outlierRemoval)
    ATH_MSG_DEBUG( "Outlier removal not yet implemented for the Gaussian Sum Filter" );


  if ( m_makePerigee ){
    const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface = this->makePerigee( smoothedTrajectory, particleHypothesis );
    ATH_MSG_DEBUG( "perigeeMultiStateOnSurface  :" << perigeeMultiStateOnSurface );
    if ( perigeeMultiStateOnSurface ) smoothedTrajectory->push_back( perigeeMultiStateOnSurface );
    else {
      ATH_MSG_DEBUG( "Perigee asked to be created but failed.....Exiting" );
      ++m_PerigeeFailure;
      delete smoothedTrajectory;
      delete forwardTrajectory;
      delete fitQuality;
      return 0;
    }
  }


  // Delete forward trajectory. New memory was assigned in ForwardGsfFitter.
  delete forwardTrajectory;

  //Reverse the order of the TSOS's to make be order flow from inside to out
  std::reverse(smoothedTrajectory->begin(), smoothedTrajectory->end());


  // Create new track
  Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
  info.setTrackProperties(TrackInfo::BremFit);
  info.setTrackProperties(TrackInfo::BremFitSuccessful);
  fittedTrack = new Track(info, smoothedTrajectory, fitQuality );

  if ( fittedTrack && msgLvl(MSG::VERBOSE)) {
    msg() << "Fitting of a set of PrepRawData objects is successful" << endmsg;
    msg() << "Track fit chi squared... " << fitQuality->chiSquared() << endmsg;
    msg() << "Track fit number of degrees of freedom... " << fitQuality->numberDoF() << endmsg;
  }
  else
    if (msgLvl(MSG::DEBUG)) msg() << "Trk::GaussianSumFilter::fit() failed!" << endmsg;

  return fittedTrack;

}

/* =========================================================================================================================================
   =========================================================================================================================================

   Fitting of a set of MeasurementBase objects

   =========================================================================================================================================
   ========================================================================================================================================= */

Trk::Track* Trk::GaussianSumFitter::fit ( const Trk::MeasurementSet&    measurementSet,
                                          const Trk::TrackParameters&   estimatedParametersNearOrigin,
                                          const Trk::RunOutlierRemoval  outlierRemoval,
                                          const Trk::ParticleHypothesis particleHypothesis ) const
{

  // Start the timer
  Chrono chrono( &(*m_chronoSvc), name() );

  if (msgLvl(MSG::VERBOSE)){
    msg() << "Trk::GaussianSumFilter::fit() - Fitting a set of MeasurementBase objects" << endmsg;
    msg() << "Material effects switch: " << particleHypothesis << endmsg;
    msg() << "Outlier removal switch:  " << outlierRemoval << endmsg;
  }

  ++m_FitMeasuremnetBase;

  // Protect against empty PrepRawDataSet object
  if ( measurementSet.empty() ) {
    msg(MSG::FATAL) << "MeasurementSet for fit is empty... Exiting!" << endmsg;
    return 0;
  }

  // Find the CCOT if it exsists
  const Trk::CaloCluster_OnTrack* ccot(0);
  Trk::MeasurementSet cleanedMeasurementSet;

  MeasurementSet::const_iterator itSet    = measurementSet.begin();
  MeasurementSet::const_iterator itSetEnd = measurementSet.end();
  for ( ; itSet!=itSetEnd; ++itSet) {
    if (!(*itSet)) {
      msg(MSG::WARNING) << "There is an empty MeasurementBase object in the track! Skip this object.." << endmsg;
    } 
    else {
      ccot = nullptr;
      if((*itSet)->type(Trk::MeasurementBaseType::CaloCluster_OnTrack)){
        ccot= static_cast <const Trk::CaloCluster_OnTrack*>(*itSet);
      }  
      if (!ccot){
        cleanedMeasurementSet.push_back(*itSet);
      } else {
       ATH_MSG_DEBUG( "The  MeasurementBase object is a Trk::CaloCluster_OnTrack" );
      }
    }
  }

  // A const stl container cannot be sorted. This will re-cast it so that it can.
  Trk::MeasurementSet sortedMeasurementSet = MeasurementSet( cleanedMeasurementSet );

  if ( m_doHitSorting ){

    Trk::MeasurementBaseComparisonFunction* measurementBaseComparisonFunction = new Trk::MeasurementBaseComparisonFunction( estimatedParametersNearOrigin.position(),
                                  estimatedParametersNearOrigin.momentum() );

    sort ( sortedMeasurementSet.begin(), sortedMeasurementSet.end(), *measurementBaseComparisonFunction );

    delete measurementBaseComparisonFunction;

  }
  // Perform GSF forwards fit - new memory allocated in forwards fitter
  const ForwardTrajectory* forwardTrajectory = m_forwardGsfFitter->fitMeasurements( sortedMeasurementSet, estimatedParametersNearOrigin, particleHypothesis );

  if ( !forwardTrajectory ) {
    if (msgLvl(MSG::DEBUG)) msg() << "Forward GSF fit failed... Exiting!" << endmsg;
    ++m_FowardFailure;
    return 0;
  }

  if ( forwardTrajectory->empty() ){
    if (msgLvl(MSG::DEBUG)) msg() << "No states in forward trajectory... Exiting!" << endmsg;
    delete forwardTrajectory;
    ++m_FowardFailure;
    return 0;
  }

  if (msgLvl(MSG::VERBOSE))
    msg() << "*** Forward GSF fit passed! ***" << endmsg;


  // Perform GSF smoother operation
  SmoothedTrajectory* smoothedTrajectory = m_gsfSmoother->fit( *forwardTrajectory, particleHypothesis, ccot );

  // Protect against failed smoother fit
  if ( !smoothedTrajectory ) {
    msg(MSG::DEBUG) << "Smoother GSF fit failed... Exiting!" << endmsg;
    ++m_SmootherFailure;
    delete forwardTrajectory;
    return 0;
  }


  if (msgLvl(MSG::VERBOSE))
    msg() << "*** GSF smoother fit passed! ***" << endmsg;


  // Outlier m_logic and track finalisation
  const FitQuality* fitQuality = buildFitQuality( *smoothedTrajectory );

  if ( !fitQuality ){
    if (msgLvl(MSG::DEBUG))
      msg() << "Chi squared could not be calculated... Bailing" << endmsg;
    ++m_fitQualityFailure;
    delete forwardTrajectory;
    delete smoothedTrajectory;
    return 0;
  }

  if (outlierRemoval && msgLvl(MSG::DEBUG))
    msg() << "Outlier removal not yet implemented for the Gaussian Sum Filter" << endmsg;

  if ( m_makePerigee ){
    const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface = this->makePerigee( smoothedTrajectory, particleHypothesis );
    if (msgLvl(MSG::DEBUG)) msg() << "perigeeMultiStateOnSurface  :" << perigeeMultiStateOnSurface << endmsg;

    if ( perigeeMultiStateOnSurface ) smoothedTrajectory->push_back( perigeeMultiStateOnSurface );
    else {
      if (msgLvl(MSG::DEBUG)) msg() << "Perigee asked to be created but failed.....Exiting" << endmsg;
      ++m_PerigeeFailure;
      delete fitQuality;
      delete forwardTrajectory;
      delete smoothedTrajectory;
      return 0;
    }
  }

  //Delete forward trajectory. New memory was assigned in ForwardGsfFitter.
  delete forwardTrajectory;
  
 
  //Reverse the order of the TSOS's to make be order flow from inside to out
  std::reverse(smoothedTrajectory->begin(), smoothedTrajectory->end());


  // Create new track
  Trk::TrackInfo info(Trk::TrackInfo::GaussianSumFilter, particleHypothesis);
  info.setTrackProperties(TrackInfo::BremFit);
  info.setTrackProperties(TrackInfo::BremFitSuccessful);
  Track* fittedTrack = new Track(info, smoothedTrajectory, fitQuality );

  if ( fittedTrack ) {
    msg(MSG::DEBUG) << "Fitting of a set of MeasurementBase objects is successful" << endmsg;
    msg(MSG::DEBUG) << "Track fit chi squared... " << fitQuality->chiSquared() << endmsg;
    msg(MSG::DEBUG) << "Track fit number of degrees of freedom... " << fitQuality->numberDoF() << endmsg;
  } else {
    msg(MSG::DEBUG) << "Trk::GaussianSumFilter::fit() failed!" << endmsg;
  }

  return fittedTrack;

}

Trk::Track* Trk::GaussianSumFitter::fit ( const Track&             intrk,
                                          const PrepRawDataSet&    addPrdColl,
                                          const RunOutlierRemoval  runOutlier,
                                          const ParticleHypothesis matEffects)   const
{

  if (msgLvl(MSG::VERBOSE)) {
    msg() << "--> enter GaussianSumFitter::fit(Track,PrdSet,,)" << endmsg;
    msg() << "    with Track from author = " << intrk.info().dumpInfo() << endmsg;
  }

  // protection, if empty PrepRawDataSet
  if (addPrdColl.empty()) {
    msg(MSG::WARNING) << "client tries to add an empty PrepRawDataSet to the track fit." << endmsg;
    return fit(intrk, runOutlier, matEffects);
  }

  /*  determine the Track Parameter which is the start of the trajectory,
      i.e. closest to the reference point */
  if (msgLvl(MSG::VERBOSE)) msg()<< "get track parameters near origin "
                             << (m_doHitSorting? "via STL sort" : "from 1st state")
                             << endmsg;

  const TrackParameters* estimatedStartParameters = m_doHitSorting
    ?  *(std::min_element(intrk.trackParameters()->begin(),
                          intrk.trackParameters()->end(),
                          *m_trkParametersComparisonFunction))
    :  *intrk.trackParameters()->begin() ;


  // use external preparator class to prepare PRD set for fitter interface

  Amg::Vector3D referencePosition( m_sortingReferencePoint[0],
           m_sortingReferencePoint[1],
           m_sortingReferencePoint[2] );

  TrackFitInputPreparator*  inputPreparator = new TrackFitInputPreparator(referencePosition);

  PrepRawDataSet orderedPRDColl =
    inputPreparator->stripPrepRawData(intrk,addPrdColl,m_doHitSorting,
                                        true /* do not lose outliers! */);

  delete inputPreparator;

  return fit(orderedPRDColl,*estimatedStartParameters,runOutlier,matEffects);
}


Trk::Track* Trk::GaussianSumFitter::fit ( const Track&             inputTrack,
                                          const MeasurementSet&    measurementSet,
                                          const RunOutlierRemoval  runOutlier,
                                          const ParticleHypothesis matEffects)   const
{

  if (msgLvl(MSG::VERBOSE))
    msg() << "Trk::GaussianSumFilter::fit() - Refitting a track with a additional information " << endmsg;

  // protection, if empty MeasurementSet
  if (measurementSet.empty()) {
    msg(MSG::WARNING) << "Client tries to add an empty MeasurementSet to the track fit." << endmsg;
    return fit(inputTrack, runOutlier, matEffects);
  }

  // Check that the input track has well defined parameters
  if ( inputTrack.trackParameters()->empty() ) {
    msg(MSG::FATAL) << "No estimation of track parameters near origin... Exiting!" << endmsg;
    return 0;
  }

  // Check that the input track has associated MeasurementBase objects
  if ( inputTrack.trackStateOnSurfaces()->empty() ) {
    msg(MSG::FATAL) << "Attempting to fit track to empty MeasurementBase collection... Exiting!" << endmsg;
    return 0;
  }

  // Retrieve the set of track parameters closest to the reference point
  const Trk::TrackParameters* parametersNearestReference = *( std::min_element( inputTrack.trackParameters()->begin(),
                    inputTrack.trackParameters()->end(),
                    *m_trkParametersComparisonFunction ) );



  MeasurementSet combinedMS  = m_inputPreparator->stripMeasurements (inputTrack,  measurementSet,
                                   true, false);

 // Apply GSF fit to MeasurementBase objects
 return fit( combinedMS, *parametersNearestReference, runOutlier, matEffects );
}

Trk::Track* Trk::GaussianSumFitter::fit ( const Track&             intrk1,
                                          const Track&             intrk2,
                                          const RunOutlierRemoval  runOutlier,
                                          const ParticleHypothesis matEffects)  const
{
  //Not a great implementation but simple...  Just add the hits on track
  // protection against not having measurements on the input tracks
  if (!intrk1.trackStateOnSurfaces() || !intrk2.trackStateOnSurfaces() || intrk1.trackStateOnSurfaces()->size() < 2) {
    msg(MSG::WARNING) << "called to refit empty track or track with too little information, reject fit" << endmsg;
    return 0;
  }

  if (!intrk1.trackParameters() || intrk1.trackParameters()->empty()) {
    msg(MSG::WARNING) << "input #1 fails to provide track parameters for seeding the GXF, reject fit" << endmsg;
    return 0;
  }

  const TrackParameters* minPar = *intrk1.trackParameters()->begin() ;
  DataVector<const TrackStateOnSurface>::const_iterator itStates = intrk1.trackStateOnSurfaces()->begin();
  DataVector<const TrackStateOnSurface>::const_iterator endState = intrk1.trackStateOnSurfaces()->end();
  DataVector<const TrackStateOnSurface>::const_iterator itStates2 = intrk2.trackStateOnSurfaces()->begin();
  DataVector<const TrackStateOnSurface>::const_iterator endState2 = intrk2.trackStateOnSurfaces()->end();

  Trk::MeasurementSet ms;

  for (;itStates!=endState;++itStates){
    if ( !((*itStates)->type(Trk::TrackStateOnSurface::Measurement) || 
           (*itStates)->type(Trk::TrackStateOnSurface::Outlier))) {continue;}

    if ((*itStates)->measurementOnTrack()->type(Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {continue;}
    
    ms.push_back((*itStates)->measurementOnTrack());
  }

  for (;itStates2!=endState2;++itStates2){

    if ( !((*itStates2)->type(Trk::TrackStateOnSurface::Measurement) || 
           (*itStates2)->type(Trk::TrackStateOnSurface::Outlier))) {continue;}

    if ((*itStates2)->measurementOnTrack()->type(Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {continue;}

    ms.push_back((*itStates2)->measurementOnTrack());
  }

  return fit(ms,*minPar,runOutlier,matEffects);

}


const Trk::MultiComponentStateOnSurface* Trk::GaussianSumFitter::makePerigee (
                                 const Trk::SmoothedTrajectory* smoothedTrajectory,
                                 const Trk::ParticleHypothesis particleHypothesis ) const
{

  if (msgLvl(MSG::VERBOSE))
    msg() << "Trk::GaussianSumFilter::makePerigee... starting" << endmsg;

  // Propagate track to perigee
  const Trk::PerigeeSurface perigeeSurface;

  const Trk::TrackStateOnSurface* stateOnSurfaceNearestOrigin = smoothedTrajectory->back();

  const Trk::MultiComponentStateOnSurface* multiComponentStateOnSurfaceNearestOrigin = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(stateOnSurfaceNearestOrigin);

  const Trk::MultiComponentState* multiComponentState = 0;

  if ( !multiComponentStateOnSurfaceNearestOrigin ){

    if (msgLvl(MSG::VERBOSE))
      msg() << "State nearest perigee is not a multi-component state... Converting" << endmsg;

    Trk::ComponentParameters componentParameters( stateOnSurfaceNearestOrigin->trackParameters(), 1. );
    multiComponentState = new Trk::MultiComponentState( componentParameters );

  }

  else
    multiComponentState = multiComponentStateOnSurfaceNearestOrigin->components();

  // Extrapolate to perigee, taking material effects considerations into account
  const Trk::MultiComponentState* stateExtrapolatedToPerigee = m_extrapolator->extrapolate( *multiComponentState,
                          perigeeSurface,
                          m_directionToPerigee,
                          false,
                          particleHypothesis );

  if (!stateExtrapolatedToPerigee){

    if (msgLvl(MSG::DEBUG))
      msg() << "Track could not be extrapolated to perigee... returning 0" << endmsg;

    return 0;
  }

  // Clean-up & pointer reset
  if ( !multiComponentStateOnSurfaceNearestOrigin && stateExtrapolatedToPerigee != multiComponentState )
    delete multiComponentState;

  multiComponentState = 0;

  // Calculate the mode of the q/p distribution
  double modeQoverP = 0;

  if ( modeQoverP  && msgLvl(MSG::VERBOSE) )
    msg() << "Calculated mode is stored by default in the Perigee Parameters and is NO Longer Stored as a seperate Parameter"  << endmsg;

  // Determine the combined state as well to be passed to the MultiComponentStateOnSurface object
  const Trk::TrackParameters* combinedPerigee = m_stateCombiner->combine( *stateExtrapolatedToPerigee, true );

  // Perigee is given as an additional MultiComponentStateOnSurface
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern(0);
  pattern.set(Trk::TrackStateOnSurface::Perigee);

  if (fabs(combinedPerigee->parameters()[Trk::qOverP])>1e8) { //GC: protection against 0-momentum track .. this check should NEVER be needed.
                                                              //    actual cutoff is 0.01eV track
    ATH_MSG_ERROR("makePerigee() about to return with 0 momentum!! Returning null instead");
    delete stateExtrapolatedToPerigee;
    delete combinedPerigee;
    return 0;
  }

  const Trk::MultiComponentStateOnSurface* perigeeMultiStateOnSurface
                                              = new MultiComponentStateOnSurface( 0,
                                                                                  combinedPerigee,
                                                                                  stateExtrapolatedToPerigee,
                                                                                  0,
                                                                                  0,
                                                                                  pattern,
                                                                                  modeQoverP );

  ATH_MSG_DEBUG( "makePerigee() returning sucessfully!" );

  return perigeeMultiStateOnSurface;
}


void Trk::GaussianSumFitter::validationAction() const
{
   
}

const Trk::FitQuality* Trk::GaussianSumFitter::buildFitQuality(const Trk::SmoothedTrajectory& smoothedTrajectory) const
{

  ATH_MSG_VERBOSE( "Gsf fitQuality" );

  double chiSquared = 0.;
  int    numberDoF  = -5;

  // Loop over all TrackStateOnSurface objects in trajectory
  SmoothedTrajectory::const_iterator stateOnSurface = smoothedTrajectory.begin();

  for ( ; stateOnSurface != smoothedTrajectory.end(); ++stateOnSurface ) {

    if ( !(*stateOnSurface)->type(TrackStateOnSurface::Measurement) ) continue;
    if ( (*stateOnSurface)->fitQualityOnSurface() == 0 ) continue;

    chiSquared += (*stateOnSurface)->fitQualityOnSurface()->chiSquared();
    numberDoF  += (*stateOnSurface)->fitQualityOnSurface()->numberDoF();

  }

  if ( std::isnan( chiSquared ) || chiSquared <= 0. ) return 0;

  const FitQuality* fitQuality = new FitQuality( chiSquared, numberDoF );

  return fitQuality;

}
