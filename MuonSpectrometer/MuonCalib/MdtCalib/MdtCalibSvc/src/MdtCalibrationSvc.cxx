/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// std
#include <fstream>
#include <string>
// other packages
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// this package
#include "MuonCalibEvent/MdtCalibHit.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

// this package
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"

#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/MdtRtRelation.h"
#include "MdtCalibData/MdtTubeCalibContainer.h"
#include "MdtCalibData/MdtCorFuncSet.h"
#include "MdtCalibData/IMdtBFieldCorFunc.h"
#include "MdtCalibData/IMdtSlewCorFunc.h"
#include "MdtCalibData/IMdtTempCorFunc.h"
#include "MdtCalibData/IMdtBackgroundCorFunc.h"
#include "MdtCalibData/IMdtWireSagCorFunc.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/TrRelation.h"
#include "MdtCalibData/RtScaleFunction.h"

#include "GaudiKernel/ServiceHandle.h"

//
// private helper functions
//

class MdtCalibrationSvc::Imp {
public:
  
  Imp( std::string name );

  const MuonGM::MdtReadoutElement* getGeometry( const Identifier &id ) {
    assert( m_muonGeoManager );
    return m_muonGeoManager->getMdtReadoutElement( id );
  }

  double applyCorrections( MdtCalibHit &hit,
			   const MdtCalibrationSvcInput &input,
			   const MdtCalibrationSvcSettings &settings,
			   double adcCal,
			   const MuonCalib::MdtCorFuncSet *corrections,
			   const MuonCalib::IRtRelation *rt ) const;

  const MuonGM::MuonDetectorManager *m_muonGeoManager;
  const MdtIdHelper                 *m_mdtIdHelper;
  MdtCalibrationSvcSettings          settings;
    
  double m_inverseSpeedOfLight;      // in ns/mm
  double m_inversePropagationSpeed;  // in ns/mm
  
  MsgStream *m_log; //!< Pointer to MsgStream - MdtCalibrationSvc now owns this
  bool m_verbose;   //!< True if we should print MSG::VERBOSE messages
  bool m_debug;     //!< True if we should print MSG::DEBUG messages

  //handle of b-field service - job option
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;
  ServiceHandle<MdtCalibrationDbSvc> m_dbSvc;
  
  double m_unphysicalHitRadiusUpperBound;
  double m_unphysicalHitRadiusLowerBound;
  double m_resTwin;
};

MdtCalibrationSvc::Imp::Imp(std::string name) :
  m_muonGeoManager(0),
  m_mdtIdHelper(0),
  m_inverseSpeedOfLight(1./299.792458),
  m_inversePropagationSpeed(m_inverseSpeedOfLight/0.85),
  m_log(0),
  m_verbose(false),
  m_debug(false),
  m_magFieldSvc("AtlasFieldSvc", name),
  m_dbSvc("MdtCalibrationDbSvc", name),
  m_unphysicalHitRadiusUpperBound(-1.),
  m_unphysicalHitRadiusLowerBound(-1.),
  m_resTwin(-1.)
{}

MdtCalibrationSvc::MdtCalibrationSvc(const std::string &name,ISvcLocator *sl)
  : AthService(name,sl) {
  m_imp = new MdtCalibrationSvc::Imp(name);
  // settable properties
  declareProperty("TimeWindowLowerBound",m_imp->settings.windowLowerBound );
  declareProperty("TimeWindowUpperBound",m_imp->settings.windowUpperBound );
  declareProperty("TimeWindowSetting",m_imp->settings.windowSetting );
  declareProperty("DoTofCorrection",m_imp->settings.doTof );
  declareProperty("DoPropagationCorrection",m_imp->settings.doProp );
  declareProperty("DoTemperatureCorrection",m_imp->settings.doTemp );
  declareProperty("DoMagneticFieldCorrection",m_imp->settings.doField );
  declareProperty("DoWireSagCorrection",m_imp->settings.doWireSag );
  declareProperty("DoSlewingCorrection",m_imp->settings.doSlew );
  declareProperty("DoBackgroundCorrection",m_imp->settings.doBkg );
  declareProperty("ResolutionTwinTube",   m_imp->m_resTwin = 1.05, "Twin Tube resolution");
  declareProperty("MagFieldSvc", m_imp->m_magFieldSvc);
  declareProperty("UpperBoundHitRadius", m_imp->m_unphysicalHitRadiusUpperBound = 20. );
  declareProperty("LowerBoundHitRadius", m_imp->m_unphysicalHitRadiusLowerBound = 0. );
}

MdtCalibrationSvc::~MdtCalibrationSvc() {
  delete m_imp;
}

// queryInterface 
StatusCode MdtCalibrationSvc::queryInterface(const InterfaceID &riid, void **ppvIF) { 
  if ( interfaceID().versionMatch(riid) ) { 
    *ppvIF = dynamic_cast<MdtCalibrationSvc*>(this); 
  } else { 
    return AthService::queryInterface(riid, ppvIF); 
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationSvc::initialize() { 
  ATH_MSG_DEBUG( "Initializing" ); 
  if( AthService::initialize().isFailure() ) return StatusCode::FAILURE;

  m_imp->settings.initialize();
  // initialize the MdtIdHelper
  ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", name());
  if ( detStore.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Can't locate the DetectorStore" ); 
    return StatusCode::FAILURE;
  }
 
  if ( detStore->retrieve(m_imp->m_mdtIdHelper, "MDTIDHELPER" ).isFailure() ) {
    ATH_MSG_FATAL( "Can't retrieve MdtIdHelper" );
    return StatusCode::FAILURE;
  }
  // initialise MuonGeoModel access
  if ( detStore->retrieve( m_imp->m_muonGeoManager ).isFailure() ) {
    ATH_MSG_FATAL( "Can't retrieve MuonDetectorManager" );
    return StatusCode::FAILURE;
  }
    
  if( m_imp->m_dbSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Could not get MdtCalibrationDbSvc" );
    return StatusCode::FAILURE;
  }

  if ( m_imp->m_magFieldSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Could not find MagFieldSvc" );
    return StatusCode::FAILURE;
  }                                                         

  m_imp->m_log = &msg();
  m_imp->m_verbose = msgLvl(MSG::VERBOSE);
  m_imp->m_debug = msgLvl(MSG::DEBUG);

  ATH_MSG_DEBUG("Settings:");
  ATH_MSG_DEBUG("  Time window: lower bound " << m_imp->settings.timeWindowLowerBound() 
	       << "   upper bound " << m_imp->settings.timeWindowUpperBound() );
  if( m_imp->settings.doTofCorrection() )           ATH_MSG_DEBUG(" Using TOF ");
  if( m_imp->settings.doPropCorrection() )          ATH_MSG_DEBUG(" Using Prop ");
  if( m_imp->settings.doCorrections() )             ATH_MSG_DEBUG(" Using Cor ");
  if( m_imp->settings.doTemperatureCorrection() )   ATH_MSG_DEBUG(" Using TempCor ");
  if( m_imp->settings.doMagneticFieldCorrection() ) ATH_MSG_DEBUG(" Using MagCor ");
  if( m_imp->settings.doWireSagCorrection() )       ATH_MSG_DEBUG(" Using WireSag ");
  if( m_imp->settings.doSlewingCorrection() )       ATH_MSG_DEBUG(" Using Slew ");
  if( m_imp->settings.doBackgroundCorrection() )    ATH_MSG_DEBUG(" Using BkgCor");

  return StatusCode::SUCCESS;
}  //end MdtCalibrationSvc::initialize

StatusCode MdtCalibrationSvc::finalize() { 
  ATH_MSG_DEBUG( "Finalizing " );
  return AthService::finalize();
}

bool MdtCalibrationSvc::driftRadiusFromTime( MdtCalibHit &hit,
                                             double signedTrackLength,
                                             double triggerTime,
                                             bool resolFromRtrack ) {
  MdtCalibrationSvcInput inputData;
  inputData.tof = signedTrackLength*m_imp->m_inverseSpeedOfLight;
  inputData.triggerOffset = triggerTime;
  return driftRadiusFromTime( hit, inputData, m_imp->settings, resolFromRtrack );
}
 
bool MdtCalibrationSvc::driftRadiusFromTime( MdtCalibHit &hit,
                                             const MdtCalibrationSvcInput &inputData,
                                             bool resolFromRtrack ) {
  return driftRadiusFromTime( hit, inputData, m_imp->settings, resolFromRtrack );
}

bool MdtCalibrationSvc::driftRadiusFromTime( MdtCalibHit &hit,
                                             const MdtCalibrationSvcInput &inputData,
                                             const MdtCalibrationSvcSettings &settings,
                                             bool resolFromRtrack ) {
  
  if( settings.timeWindowUpperBound() < 0. || settings.timeWindowLowerBound() < 0. ) {
    // Should be an ERROR, but can't return StatusCode::FAILURE
    ATH_MSG_WARNING( "Uninitialized settings object at " << &settings );
    return false;
  }
    
  // Easiest MessageSvc implementation, not necessarily the best
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Input: tof " << inputData.tof << " trigger offset " << inputData.triggerOffset;
    if( inputData.pointOfClosestApproach ) msg(MSG::VERBOSE) << "  PointOfClosestApproach";
    if( inputData.trackDirection )         msg(MSG::VERBOSE) << "  TrackDirection";
    if( inputData.nominalWireSurface )     msg(MSG::VERBOSE) << "  Nom. WireSurface";
    if( inputData.wireSurface )            msg(MSG::VERBOSE) << "  Sagged Wiresurface";
    msg(MSG::VERBOSE) << endmsg;
    msg(MSG::VERBOSE) << "Settings: window " << settings.timeWindowLowerBound() << "  " << settings.timeWindowUpperBound();
    if( settings.doTof )     msg(MSG::VERBOSE) << " Tof";
    if( settings.doProp )    msg(MSG::VERBOSE) << " Prop";
    if( settings.doTemp )    msg(MSG::VERBOSE) << " Temp";
    if( settings.doField )   msg(MSG::VERBOSE) << " Field";
    if( settings.doWireSag ) msg(MSG::VERBOSE) << " WireSag";
    if( settings.doSlew )    msg(MSG::VERBOSE) << " Slew";
    if( settings.doBkg )     msg(MSG::VERBOSE) << " Bkg";
    msg(MSG::VERBOSE) << endmsg;
  }

  const Identifier &id = hit.identify();

  const MuonGM::MdtReadoutElement *geo = hit.geometry();

  // set geometry pointer if not yet set
  if ( !geo ) {
    geo = m_imp->getGeometry( id );
    if ( !geo ) return false;
    hit.setGeometry( geo );
  }

  // get calibration constants from DbSvc
  MuonCalib::MdtFullCalibData data = m_imp->m_dbSvc->getCalibration( geo->collectionHash(),
								     geo->detectorElementHash() ); 
    
  // require at least the MdtRtRelation to be available
  const MuonCalib::MdtRtRelation *rtRelation = data.rtRelation;
  // Hardcoded MDT tube radius 14.6mm here - not correct for sMDT
  // on the other hand it should be rare that a tube does not have an RT
  if( !rtRelation ) {
    ATH_MSG_WARNING( "no rtRelation found, cannot calibrate tube" );
    hit.setDriftRadius( 0., 2*14.6/sqrt(12) ); // treat the tube as a 'strip' measurement
    return false;
  }
  double t0(0.);
  double inversePropSpeed = m_imp->m_inversePropagationSpeed;
  double adcCal(1.);

  // access t0 for the given tube
  if ( data.tubeCalib ) {

    const int ml    = m_imp->m_mdtIdHelper->multilayer(id)-1;
    const int layer = m_imp->m_mdtIdHelper->tubeLayer(id)-1;
    const int tube  = m_imp->m_mdtIdHelper->tube(id)-1;
     
    if ( ml<0 || layer<0 || tube<0 ){
      ATH_MSG_WARNING( "Oops negative index....." );
      return false;
    }
     
    // extract calibration constants for single tube
    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib *singleTubeData = 
      data.tubeCalib->getCalib( ml, layer, tube );
    if( singleTubeData ){
      t0 = singleTubeData->t0;
      inversePropSpeed = singleTubeData->inversePropSpeed;
      adcCal = singleTubeData->adcCal;
    } else {
      msg(MSG::WARNING) << "failed to access tubedata for "
			<< ml << " " << layer << " " << tube
			<< " using defaults.. " << endmsg;
      if ( geo ) 
	msg(MSG::WARNING) << "detel " << geo->getMultilayer()
			  << " lay " << geo->getNLayers() 
			  << " tubes " << geo->getNtubesperlayer() << endmsg;
      t0 =  800.;
    }
  } else {
    msg(MSG::WARNING) << "MdtTubeCalibContainer not found for "
		      << m_imp->m_mdtIdHelper->print_to_string( id ) << endmsg;
    ATH_MSG_WARNING( "Tube cannot be calibrated!!!" );
    return false;
  }

  // correct for global t0 of rt-region
  t0 += rtRelation->t0Global();
  hit.setTubeT0(t0);
  hit.setTubeAdcCal(adcCal);

  // in order to have clean info in the ntuple set to 0 the 
  // corrections which are not used
  /*    hit.setSlewingTime( 0 );
	hit.setLorentzTime( 0 );
	hit.setTemperatureTime( 0 );
	hit.setWiresagTime( 0 );*/
  // set propagation delay
  double propTime(0.);
  if ( settings.doProp ){
    // set propagation delay w.r.t. centre of tube.
    double distToRO = geo->distanceFromRO( hit.globalPointOfClosestApproach(), id );
    hit.setDistanceToReadout( distToRO );
    double propagationDistance = distToRO - geo->RODistanceFromTubeCentre( id );
    propTime = propagationDistance * inversePropSpeed;
  }
  hit.setPropagationTime(propTime);

  // set time-of-flight
  double triggerTime = inputData.tof + inputData.triggerOffset;
  hit.setTimeOfFlight( settings.doTof ? triggerTime : 0. ); 

  // calculate drift time
  double driftTime = hit.tdcCount() * tdcBinSize(id) - hit.timeOfFlight()
                     - hit.tubeT0() - hit.propagationTime();
  hit.setDriftTime( driftTime );
      
  // apply corrections
  double corTime(0.);
  if ( settings.doCorrections() ) {
    corTime = m_imp->applyCorrections( hit, inputData, settings,
				       adcCal, data.corrections, rtRelation->rt());
  }

  // calculate drift radius + error
  double r(0.);
  double reso(1.);
  double t = hit.driftTime();
  double t_inrange = hit.driftTime();
  bool calibOk = true;
  Muon::MdtDriftCircleStatus timeStatus = driftTimeStatus(t, rtRelation, settings);
  if( rtRelation->rt() ){
      
    // check whether drift times are within range, if not fix them to the min/max range
    r = rtRelation->rt()->radius( t );
    if ( t < rtRelation->rt()->tLower() ) {
      t_inrange = rtRelation->rt()->tLower();
      double rmin = rtRelation->rt()->radius( t_inrange );
      double drdt = (rtRelation->rt()->radius( t_inrange + 30. ) - rmin)/30.;
 
      // now check whether we are outside the time window
      if ( timeStatus == Muon::MdtStatusBeforeSpectrum ) {
	t = rtRelation->rt()->tLower() - settings.timeWindowLowerBound();
	calibOk = false;
      }

      // if we get here we are outside the rt range but inside the window. 
      r = rmin + drdt*(t-t_inrange);
      if( r < m_imp->m_unphysicalHitRadiusLowerBound ) r = m_imp->m_unphysicalHitRadiusLowerBound;
    } else if( t > rtRelation->rt()->tUpper() ) {
      t_inrange = rtRelation->rt()->tUpper();
      double rmax = rtRelation->rt()->radius( t_inrange );
      double drdt = (rmax - rtRelation->rt()->radius( t_inrange - 30. ))/30.;
 
      // now check whether we are outside the time window
      if ( timeStatus == Muon::MdtStatusAfterSpectrum ) {
	t = rtRelation->rt()->tUpper() + settings.timeWindowUpperBound();
	calibOk = false;
      }

      // if we get here we are outside the rt range but inside the window. 
      r = rmax + drdt*(t-t_inrange);
    }
  } else {
    ATH_MSG_WARNING( "no rt found" );
  }

  if ( rtRelation->rtRes() ){
    if (!resolFromRtrack) {
      reso = rtRelation->rtRes()->resolution( t_inrange );
    } else {
      bool out_of_bound_flag;
      reso = rtRelation->rtRes()->resolution(rtRelation->tr()->tFromR(fabs(hit.signedDistanceToTrack()),
								      out_of_bound_flag) );
    }
  }else{
    ATH_MSG_WARNING( "no rtRes found" );
  }
  hit.setDriftRadius( r, reso );

  // summary
  ATH_MSG_VERBOSE( "driftRadiusFromTime for tube " << m_imp->m_mdtIdHelper->print_to_string(id) 
		 << (calibOk ? " OK" : "FAILED") );
  ATH_MSG_VERBOSE( " raw drift time " << hit.tdcCount() * tdcBinSize(id)
		 << " TriggerOffset " << inputData.triggerOffset << endmsg
		 << "Tof " << inputData.tof << " Propagation Delay "
		 << hit.propagationTime() << " T0 " << hit.tubeT0() 
		 << " invProp " << inversePropSpeed << endmsg
		 << "Drift time after corrections " << driftTime
		 << " time cor " << corTime 
		 << " Drift radius " << hit.driftRadius()
		 << " sigma " <<  hit.sigmaDriftRadius() );

  
  return calibOk;
}  //end MdtCalibrationSvc::driftRadiusFromTime

// + TWIN TUBES
bool MdtCalibrationSvc::twinPositionFromTwinHits( MdtCalibHit &hit,
                                                  MdtCalibHit &secondHit,
                                                  double signedTrackLength,
                                                  double secondSignedTrackLength,
                                                  bool &secondDigitIsPrompt,
                                                  double triggerTime ){
  MdtCalibrationSvcInput inputData;
  inputData.tof = signedTrackLength*m_imp->m_inverseSpeedOfLight;
  inputData.triggerOffset = triggerTime;
  
  MdtCalibrationSvcInput secondInputData;
  secondInputData.tof = secondSignedTrackLength*m_imp->m_inverseSpeedOfLight;
  secondInputData.triggerOffset = triggerTime;
  return twinPositionFromTwinHits( hit, secondHit, inputData,
				   secondInputData, m_imp->settings, secondDigitIsPrompt );
}

bool MdtCalibrationSvc::twinPositionFromTwinHits( MdtCalibHit &hit,
                                                  MdtCalibHit &secondHit,
                                                  const MdtCalibrationSvcInput &inputData,
                                                  const MdtCalibrationSvcInput &secondInputData,
                                                  const MdtCalibrationSvcSettings &settings,
                                                  bool &secondDigitIsPrompt ) {

  // 13/02/2009 A.Koutsman: after discussion with P.Kluit rebuilt this function to use the standard way 
  // of calibrating a MdtCalibHit with driftRadiusFromTime(...) 
  
  driftRadiusFromTime( hit, inputData, settings );
  driftRadiusFromTime( secondHit, secondInputData, settings );
  
  // get Identifier and MdtReadOutElement for twin tubes
  const Identifier &id = hit.identify();
  const Identifier &idSecond = secondHit.identify();
  const MuonGM::MdtReadoutElement *geo = hit.geometry();
  const MuonGM::MdtReadoutElement *geoSecond = secondHit.geometry();

  // get 'raw' drifttimes of twin pair; we don't use timeofFlight or propagationTime cause they are irrelevant for twin coordinate
  double driftTime = hit.tdcCount()*tdcBinSize(id) - hit.tubeT0();
  double driftTimeSecond = secondHit.tdcCount()*tdcBinSize(idSecond) - secondHit.tubeT0();
    
  if(!geo) {
    ATH_MSG_WARNING( "Geometry not set for first hit" );
    return false;
  }
  if(!geoSecond) {
    ATH_MSG_WARNING( "Geometry not set for second hit" );
    return false;
  }
  // get calibration constants from DbSvc
  MuonCalib::MdtFullCalibData data = m_imp->m_dbSvc->getCalibration( geo->collectionHash(), geo->detectorElementHash() ); 
  MuonCalib::MdtFullCalibData dataSecond = m_imp->m_dbSvc->getCalibration( geoSecond->collectionHash(), geoSecond->detectorElementHash() ); 
  
  //double t0(0.);
  double inversePropSpeed = m_imp->m_inversePropagationSpeed;
  //double adcCal(1.);
  
  //double t0Second(0.);
  double inversePropSpeedSecond = m_imp->m_inversePropagationSpeed;
  //double adcCalSecond(1.);
 
  // access t0 for the give tube
  if( data.tubeCalib ){

    const int ml    = m_imp->m_mdtIdHelper->multilayer(id)-1;
    const int layer = m_imp->m_mdtIdHelper->tubeLayer(id)-1;
    const int tube  = m_imp->m_mdtIdHelper->tube(id)-1;
 
    if( ml<0 || layer<0 || tube<0 ){
      ATH_MSG_WARNING( "Oops negative index....." );
      return false;
    }
 
    // extract calibration constants for single tube
    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib *singleTubeData = 
      data.tubeCalib->getCalib( ml, layer, tube );
    if ( singleTubeData ){
      //t0 = singleTubeData->t0;
      inversePropSpeed = singleTubeData->inversePropSpeed;
      //adcCal = singleTubeData->adcCal;
    } else {
      ATH_MSG_WARNING( "failed to access tubedata for "
		       << ml << " " << layer << " " << tube 
		       << " using defaults.. " );
      if ( geo ) 
	ATH_MSG_WARNING( "detel "
			 << geo->getMultilayer() << " lay " << geo->getNLayers() 
			 << " tubes " << geo->getNtubesperlayer() );
    }
  } else {
    ATH_MSG_WARNING( "MdtTubeCalibContainer not found for "
		     << m_imp->m_mdtIdHelper->print_to_string( id ) );
    ATH_MSG_WARNING( "Tube cannot be calibrated!!!" );
    return false;
  }
 
  // access t0 for the given second tube
  if ( dataSecond.tubeCalib ){
    const int mlSecond    = m_imp->m_mdtIdHelper->multilayer(idSecond)-1;
    const int layerSecond = m_imp->m_mdtIdHelper->tubeLayer(idSecond)-1;
    const int tubeSecond  = m_imp->m_mdtIdHelper->tube(idSecond)-1;
 
    if( mlSecond<0 || layerSecond<0 || tubeSecond<0 ){
      ATH_MSG_WARNING( "Oops negative index for second tube....." );
      return false;
    }
 
    // extract calibration constants for single tube
    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib *singleTubeDataSecond = 
      dataSecond.tubeCalib->getCalib( mlSecond, layerSecond, tubeSecond );
    if ( singleTubeDataSecond ){
      //t0Second = singleTubeDataSecond->t0;
      inversePropSpeedSecond = singleTubeDataSecond->inversePropSpeed;
      //adcCalSecond = singleTubeDataSecond->adcCal;
    } else {
      ATH_MSG_WARNING( "failed to access (second) tubedata for " << mlSecond
		       << " " << layerSecond << " " << tubeSecond 
		       << " using defaults.. " );
      if ( geoSecond ) 
	ATH_MSG_WARNING( "detel " << geoSecond->getMultilayer()
			 << " lay " << geoSecond->getNLayers() 
			 << " tubes " << geoSecond->getNtubesperlayer() );
      //t0Second =  800.;
    }
  } else {
    ATH_MSG_WARNING( "MdtTubeCalibContainer not found for "
		     << m_imp->m_mdtIdHelper->print_to_string( idSecond ) );
    ATH_MSG_WARNING( "Second tube cannot be calibrated!!!" );
    return false;
  }

  // define twin position and error
  double zTwin(0.);
  double errZTwin(0.);
  double twin_timedif(0.);

  // find out which tube was the prompt
  // (= actually hit by the muon; not hit by the muon = twinhit_)
  // in the formula for z_hit_from_twin we take as convention that
  // twindif = twin_time - prompt_time

  Identifier prompthit_id = id;
  const MuonGM::MdtReadoutElement *prompthit_geo = geo;
  int prompthit_tdc = 0; //double prompthit_adc = 0;
  int twinhit_tdc = 0;   //double twinhit_adc = 0;
  if ( driftTime < driftTimeSecond) { 
    twin_timedif = driftTimeSecond - driftTime;  
    //prompthit_tdc = hit.tdcCount(); prompthit_adc = adcCal; 
    //twinhit_tdc = secondHit.tdcCount(); twinhit_adc = adcCalSecond; 
    secondDigitIsPrompt = false;
  } else if (driftTimeSecond <= driftTime){ 
    prompthit_id = idSecond; 
    prompthit_geo = geoSecond;  
    twin_timedif = driftTime - driftTimeSecond; 
    //prompthit_tdc = secondHit.tdcCount(); prompthit_adc = adcCalSecond; 
    //twinhit_tdc = hit.tdcCount(); twinhit_adc = adcCal; 
    secondDigitIsPrompt = true;
  }
     
  // get tubelength and set HV-delay (~6ns)

  double tubelength = prompthit_geo->tubeLength(prompthit_id);
  double HVdelay = 6.;

  // twin_timedif must be between min and max of possible time-difference
  // between prompt and twin signals
  // accounting for 3 std.dev. of twin time resolution
  if ( twin_timedif < (HVdelay - 5.*m_imp->m_resTwin) 
       || twin_timedif > (tubelength*inversePropSpeed
			  + tubelength*inversePropSpeedSecond
			  + HVdelay + 5.*m_imp->m_resTwin)){
    if ( m_imp->m_debug ) {
      ATH_MSG_DEBUG( " TIME DIFFERENCE OF TWIN PAIR OUT OF RANGE(" 
		     << (HVdelay - 5.*m_imp->m_resTwin) 
		     << "-"
		     << (tubelength*inversePropSpeed
			 + tubelength*inversePropSpeedSecond
			 + HVdelay + 5.*m_imp->m_resTwin) 
		     << ")   time difference = "
		     << twin_timedif );
    }  
  }
  
  // Make ONLY a twin PrepData if twin time difference is physical (within tubelength)
  if (twin_timedif < (tubelength*inversePropSpeed
		      + tubelength*inversePropSpeedSecond
		      + HVdelay + 10.*m_imp->m_resTwin)){

    //calculate local(!) z of the hit from twin tubes information

    double z_hit_sign_from_twin = ( 1 / (inversePropSpeed *2.))
      * (tubelength*inversePropSpeedSecond - twin_timedif + HVdelay) ;

    // Put twin hit always inside acceptance
    if (z_hit_sign_from_twin < -tubelength/2.) {
      if ( m_imp->m_debug ) {
	ATH_MSG_DEBUG( " TWIN HIT outside acceptance with time difference "
		       <<  twin_timedif
		       << " Z local hit " <<  z_hit_sign_from_twin
		       << " Z local minimum " <<  -tubelength/2 );  
      }
      z_hit_sign_from_twin = - tubelength/2.; 
    }
    // do sign management just like in MdtDigitizationTool.cxx
    double distRO = prompthit_geo->tubeFrame_localROPos(prompthit_id).z(); 
    double sign(-1.);  
    if (distRO < 0.) sign = 1.;

    double z_hit_geo_from_twin = sign*z_hit_sign_from_twin;
  
    zTwin = z_hit_geo_from_twin;
    errZTwin = m_imp->m_resTwin*inversePropSpeed;
  
    if ( m_imp->m_verbose ){
      ATH_MSG_VERBOSE( " TWIN TUBE " 
		     << " tube: " << m_imp->m_mdtIdHelper->print_to_string(id)
		     << " twintube: " << m_imp->m_mdtIdHelper->print_to_string(idSecond)
		     );
      ATH_MSG_VERBOSE( " prompthit tdc = " << prompthit_tdc//*TDCbinsize
		     << "  twinhit tdc = " << twinhit_tdc// *TDCbinsize
		     << "  tube driftTime = " << driftTime
		     << "  second tube driftTime = " << driftTimeSecond
		     << " TWIN PAIR time difference = " << twin_timedif
		     );
      ATH_MSG_VERBOSE( " z_hit_sign_from_twin = " << z_hit_sign_from_twin
		     << " z_hit_geo_from_twin = " << z_hit_geo_from_twin
		     << " with sign = " << sign
		     << "  distRO = " << distRO
		     );
    }
  
  } // end  if(twin_timedif < (tubelength*inversePropSpeed + tubelength*inversePropSpeedSecond + HVdelay + 10.*m_imp->m_resTwin)){
  else {
   
    if ( m_imp->m_verbose ){
      ATH_MSG_VERBOSE( " TIME DIFFERENCE OF TWIN PAIR UNPHYSICAL OUT OF RANGE(" 
		     << (HVdelay - 5*m_imp->m_resTwin) << "-"
		     << (2*tubelength*inversePropSpeed + HVdelay + 5*m_imp->m_resTwin) 
		     << ")   time difference = "
		     << twin_timedif ); 
    }
    zTwin = 0.;
    errZTwin = tubelength/2.;
  }

  hit.setLocXtwin( zTwin );
  hit.setSigma2LocXtwin( errZTwin*errZTwin );
  secondHit.setLocXtwin( zTwin );
  secondHit.setSigma2LocXtwin( errZTwin*errZTwin );

  return true;
}  //end MdtCalibrationSvc::twinPositionFromTwinHits

double MdtCalibrationSvc::tdcBinSize(const Identifier &id) {
//BMG which uses HPTDC instead of AMT, and has 0.2ns TDC ticksize
  //if( m_imp->m_mdtIdHelper->stationName(id) == 54 )  //BMG
    //return 0.2;    
// Alternative method if you don't like hardcoding BMG stationName (54)
  if( m_imp->m_mdtIdHelper->stationNameString( m_imp->m_mdtIdHelper->stationName(id) ) == "BMG" ) 
    return 0.2;
  return 0.78125;  //25/32; exact number: (1000.0/40.079)/32.0 
}

double MdtCalibrationSvc::Imp::applyCorrections(MdtCalibHit &hit,
                                                const MdtCalibrationSvcInput &inputData, 
						const MdtCalibrationSvcSettings &settings,
						double /*adcCal*/,
                                                const MuonCalib::MdtCorFuncSet *corrections, const MuonCalib::IRtRelation *rt) const {

  double corTime(0.);
  // apply corrections
  if ( corrections ){
    
    if (m_verbose) *m_log << MSG::VERBOSE << "There are correction functions." << endmsg;
    
    // slewing corrections
    if ( settings.doSlew && corrections->slewing() ){
      double slew_time=corrections->slewing()->correction( hit.driftTime(), hit.adcCount());
      corTime -=slew_time;
      hit.setSlewingTime(slew_time);
    }
    
    if( settings.doField && corrections->bField()){
      if( inputData.trackDirection && inputData.pointOfClosestApproach ) {
        Amg::Transform3D gToStation = hit.geometry()->GlobalToAmdbLRSTransform();
        double BGXYZ[3];
	double xyz[3] = { (*inputData.pointOfClosestApproach)(0), 
			  (*inputData.pointOfClosestApproach)(1), 
			  (*inputData.pointOfClosestApproach)(2) };
        m_magFieldSvc->getField(xyz, BGXYZ);
	for (int i=0; i<3; i++) BGXYZ[i] *= 1000.; // convert kT to Tesla 
        Amg::Vector3D B_global(BGXYZ[0], BGXYZ[1], BGXYZ[2]);
        Amg::Vector3D B_loc(gToStation.linear()*B_global);
        double Bpar = B_loc.x();
        Amg::Vector3D loc_dir(gToStation.linear()*(*(inputData.trackDirection)));
        Amg::Vector3D dir(0.0, loc_dir.y(), loc_dir.z());
        double Bper = B_loc.dot(dir.unit());
        hit.setBFieldPerp(Bper);
        hit.setBFieldPara(Bpar); 
        if( m_verbose ) *m_log << MSG::VERBOSE << "doing b-field correction" << endmsg;
        if(hit.bFieldPara() != MdtCalibHit::kNoValue && hit.bFieldPerp() != MdtCalibHit::kNoValue) {
	  hit.setLorentzTime(corrections->bField()->correction( hit.driftTime(), hit.bFieldPara(),  hit.bFieldPerp() ));
	} else {
	  hit.setLorentzTime(0);
	}
        corTime -= hit.lorentzTime();
      }   
    }   
    
    // temperature corrections
    // NOTE: Use this temporarily for ML dependent scaling. 
 /*   if ( settings.doTemp && corrections->temperature() ){
      if(hit.temperature() != MdtCalibHit::kNoValue)
        {
          hit.setTemperatureTime(corrections->temperature()->correction( hit.driftTime(), hit.temperature()));
        }
      else
        {
          hit.setTemperatureTime(0);
        }
      corTime += hit.TemperatureTime();
    }*/
    
    // Scale RT function from Tmax difference
    if( settings.doTemp && rt && rt->HasTmaxDiff()) {
      float scle_time=MuonCalib::RtScaleFunction(hit.driftTime(), m_mdtIdHelper->multilayer(hit.identify())==2, *rt);
      hit.setTemperatureTime(scle_time);
      corTime-=scle_time;
    } else {
      hit.setTemperatureTime(0);
    }
    
    // background corrections
    if ( settings.doBkg && corrections->background() ){
      double bgLevel = 0.;
      corTime += corrections->background()->correction( hit.driftTime(), bgLevel );
    }
    
    // wire sag corrections
    // First some debug output
    if (m_verbose) {
      if ( settings.doWireSag ){
	*m_log << MSG::VERBOSE << "settings.doWireSag == TRUE" << endmsg;
      } else {
	*m_log << MSG::VERBOSE << "settings.doWireSag == FALSE" << endmsg;
      }
      if ( corrections->wireSag() ){
	*m_log << MSG::VERBOSE << "corrections->wireSag() == TRUE" << endmsg;
      } else {
	*m_log << MSG::VERBOSE << "corrections->wireSag() == FALSE" << endmsg;
      }
    }
    // Wire sag corrections
    if ( settings.doWireSag && corrections->wireSag() ){
      
      if (m_verbose) {
	*m_log << MSG::VERBOSE << "Performing Rt Corrections due to Wire Sag." << endmsg;
	
	if (inputData.pointOfClosestApproach) {
	  *m_log << MSG::VERBOSE << "Have a point of closest approach." << endmsg;
	} else {
	  *m_log << MSG::VERBOSE << "No point of closest approach!" << endmsg;
	}
	if (inputData.trackDirection) {
	  *m_log << MSG::VERBOSE << "Have a track direction." << endmsg;
	} else {
	  *m_log << MSG::VERBOSE << "No track direction!" << endmsg;
	}
	if (inputData.nominalWireSurface) {
	  *m_log << MSG::VERBOSE << "Have a nominal wire surface." << endmsg;
	} else {
	  *m_log << MSG::VERBOSE << "No nominal wire surface!" << endmsg;
	}
	if (inputData.wireSurface) {
	  *m_log << MSG::VERBOSE << "Have a sagged wire surface." << endmsg;
	} else {
	  *m_log << MSG::VERBOSE << "No sagged wire surface!" << endmsg;
	}
      }
      
      const Amg::Vector3D *pointOfClosestApproach = inputData.pointOfClosestApproach;
      const Amg::Vector3D *trackDirection         = inputData.trackDirection;
      const Trk::StraightLineSurface *nominalWireSurface = inputData.nominalWireSurface;
      const Trk::StraightLineSurface *wireSurface = inputData.wireSurface;
      
      // check whether all input data is there
      if ( !pointOfClosestApproach || !trackDirection || !nominalWireSurface ){
	*m_log << MSG::WARNING << " cannot perform wire sag correction: ";
        if( !pointOfClosestApproach ) *m_log << " no pointOfClosestApproach ";
        if( !trackDirection )         *m_log << " no trackDirection ";
        if( !nominalWireSurface )     *m_log << " no nominalWireSurface ";
        *m_log << endmsg;
      } else {

	if (m_verbose) {
	  *m_log << MSG::VERBOSE << "All Necessary Wire Sag data available: " << endmsg;
	  *m_log << MSG::VERBOSE << "  pCA = ("
		 << pointOfClosestApproach->x() << ", "
		 << pointOfClosestApproach->y() << ", "
		 << pointOfClosestApproach->z() << ")" << endmsg;
	}

        // store pointer to sagged surface as we get ownership if we recalculate it
        const Trk::StraightLineSurface *tempSaggedSurface = 0;

        // if wire surface is missing, calculate sagged wire position 
        if ( !wireSurface ){
          const Trk::SaggedLineSurface *nominalSurf = dynamic_cast<const Trk::SaggedLineSurface*>(nominalWireSurface);
          if ( nominalSurf ){
            // Local position for calculation of position along the tube,
            // used for wire sag treatment
            const Amg::Vector2D *tempLocOnWire = nominalSurf->Trk::Surface::globalToLocal(*pointOfClosestApproach,1000.);
            if ( !tempLocOnWire ){
              *m_log << MSG::WARNING << "globalToLocal failed! " << endmsg;
            } else {
              // sagged surface
              wireSurface = nominalSurf->correctedSurface(*tempLocOnWire);
              tempSaggedSurface = wireSurface;
              delete tempLocOnWire;
            }
          } else {
            *m_log << MSG::WARNING
		   << "Nominal wire surface not a SaggedLineSurface,"
		   << " cannot perform wire sag correction" << endmsg;
          }
        }

        if ( !wireSurface ){
          *m_log << MSG::WARNING
		 << " cannot perform wire sag correction: no sagged wire surface "
		 << endmsg;
        } else {

          // get to transformation matrix from global into the tube frame
          Amg::Transform3D globalToTube = nominalWireSurface->transform().inverse();

          // calculate the local position of the nominal and sagged wire 
          Amg::Vector3D locNominalPos(0.,0.,0.);
          Amg::Vector3D locSaggedPos = globalToTube*wireSurface->center();
          locSaggedPos[2] = 0.;

          // calculate the local track direction
          Amg::Vector3D locTrackDir = globalToTube.linear()*(*trackDirection);
          locTrackDir[2] = 0.;
          locTrackDir = locTrackDir.unit();

          // check if point of closest approach has been calculated
          // relative to nominal or sagged wire

          Amg::Vector3D locApproachDir(pointOfClosestApproach->x(),
				       pointOfClosestApproach->y(),
				       pointOfClosestApproach->z());
          double dotProd = locApproachDir.dot(locTrackDir);
          double deltaY = 0.;
          if (dotProd == 0){
            // this means the closest approach was calculated
            // from the nominal position
            deltaY = pointOfClosestApproach->y() - nominalWireSurface->center().y();
          } else {
            // this means the closest approach was calculated
            // from the sagged position
            deltaY = pointOfClosestApproach->y() - wireSurface->center().y();
          }

          // sign of drift radius (for sag calculation) is +/- of track passes
          // above/below wire

          double signedDriftRadius = fabs(hit.driftRadius())*deltaY/fabs(deltaY);
 
          // calculate the magnitude of the wire sag

          double effectiveSag = nominalWireSurface->center().y()
            - wireSurface->center().y();

          // apply the correction

          corTime += corrections->wireSag()->correction(signedDriftRadius, effectiveSag);

        }
        delete tempSaggedSurface;
      }
    }

  }

  hit.setDriftTime(hit.driftTime() + corTime);

  return corTime;
}  //end MdtCalibrationSvc::Imp::applyCorrections

Muon::MdtDriftCircleStatus MdtCalibrationSvc::driftTimeStatus( double driftTime, const MuonCalib::MdtRtRelation *rtRelation, const MdtCalibrationSvcSettings &settings ) {
  if( settings.timeWindowUpperBound() < 0. || settings.timeWindowLowerBound() < 0. ) {
      ATH_MSG_WARNING( " Unphysical time window detected, both ranges should be positive: " 
		       << " lower bound " << settings.timeWindowLowerBound()
		       << " upper bound " << settings.timeWindowUpperBound() );
  }
  
  if( rtRelation && rtRelation->rt() ) {
    if( driftTime < rtRelation->rt()->tLower() - settings.timeWindowLowerBound() ) {
      // Will this produce too much output from ROT creators?
      if ( m_imp->m_verbose )
	ATH_MSG_VERBOSE( " drift time outside time window "
			 << driftTime << ". Mininum time = "
			 << rtRelation->rt()->tLower() - settings.timeWindowLowerBound() );
      return Muon::MdtStatusBeforeSpectrum;
    } else if( driftTime > rtRelation->rt()->tUpper() + settings.timeWindowUpperBound()  ) {
      if ( m_imp->m_verbose )
	ATH_MSG_VERBOSE( " drift time outside time window "
			 << driftTime << ". Maximum time  = "
			 << rtRelation->rt()->tUpper() + settings.timeWindowUpperBound() );
      return Muon::MdtStatusAfterSpectrum;
    }
  } else {
    ATH_MSG_WARNING( "No valid rt relation supplied for driftTimeStatus method" );
    return Muon::MdtStatusUnDefined; 
  }
  return Muon::MdtStatusDriftTime;
}  //end MdtCalibrationSvc::driftTimeStatus
