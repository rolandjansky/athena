/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibSvc/MdtCalibrationTool.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
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
#include "MdtCalibInterfaces/IShiftMapTools.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "MagFieldElements/AtlasFieldCache.h"
#include "MuonCalibEvent/MdtCalibHit.h"

namespace {
  static double const twoBySqrt12 = 2/std::sqrt(12);
}

//
// private helper functions
//

class MdtCalibrationTool::Imp {
public:

  Imp( std::string name );

  const MuonGM::MdtReadoutElement* getGeometry( const Identifier &id ) {
    assert( m_muonGeoManager );
    return m_muonGeoManager->getMdtReadoutElement( id );
  }

  double applyCorrections( MagField::AtlasFieldCache& fieldCache,
                           MdtCalibHit &hit,
			   const MdtCalibrationSvcInput &input,
			   const MdtCalibrationSvcSettings &settings,
			   double adcCal,
			   const MuonCalib::MdtCorFuncSet *corrections,
			   const MuonCalib::IRtRelation *rt,
         MsgStream* msgStr, const MdtIdHelper *idHelp) const;

  const MuonGM::MuonDetectorManager *m_muonGeoManager;
  MdtCalibrationSvcSettings          settings;

  double m_inverseSpeedOfLight;      // in ns/mm
  double m_inversePropagationSpeed;  // in ns/mm

  /* T0 Shift Service -- Per-tube offsets of t0 value */
  ServiceHandle<MuonCalib::IShiftMapTools> m_t0ShiftSvc;
  /* TMax Shift Service -- Per-tube offsets of Tmax */
  ServiceHandle<MuonCalib::IShiftMapTools> m_tMaxShiftSvc;

  // tools should only be retrieved if they are used
  bool m_doT0Shift;
  bool m_doTMaxShift;

  double m_unphysicalHitRadiusUpperBound;
  double m_unphysicalHitRadiusLowerBound;
  double m_resTwin;
  bool m_BMGpresent;
  int m_BMGid;

};

MdtCalibrationTool::Imp::Imp(std::string name) :
  m_muonGeoManager(nullptr),
  m_inverseSpeedOfLight(1./Gaudi::Units::c_light),
  m_inversePropagationSpeed(m_inverseSpeedOfLight/0.85),
  m_t0ShiftSvc("MdtCalibrationT0ShiftSvc", name),
  m_tMaxShiftSvc("MdtCalibrationTMaxShiftSvc", name),
  m_doT0Shift(false),
  m_doTMaxShift(false),
  m_unphysicalHitRadiusUpperBound(-1.),
  m_unphysicalHitRadiusLowerBound(-1.),
  m_resTwin(-1.),
  m_BMGpresent(false),
  m_BMGid(-1)
{}


MdtCalibrationTool::MdtCalibrationTool(const std::string& type, const std::string &name, const IInterface* parent)
  : base_class(type, name, parent)
{
  m_imp.reset(new MdtCalibrationTool::Imp(name));
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
  declareProperty("UpperBoundHitRadius", m_imp->m_unphysicalHitRadiusUpperBound = 20. );
  declareProperty("LowerBoundHitRadius", m_imp->m_unphysicalHitRadiusLowerBound = 0. );
  declareProperty("DoT0Shift", m_imp->m_doT0Shift = false );
  declareProperty("DoTMaxShift", m_imp->m_doTMaxShift = false );
}

MdtCalibrationTool::~MdtCalibrationTool() {
  m_imp.reset();
}

StatusCode MdtCalibrationTool::initialize() {
  ATH_MSG_DEBUG( "Initializing" );

  m_imp->settings.initialize();

  // Read handle for AtlasFieldCacheCondObj
  ATH_CHECK(m_fieldCacheCondObjInputKey.initialize());

  ATH_CHECK(m_idHelperSvc.retrieve());

  // assign BMG identifier
  m_imp->m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
  if(m_imp->m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_imp->m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
  }
  // initialise MuonGeoModel access
  ATH_CHECK(detStore()->retrieve( m_imp->m_muonGeoManager ));

  if (m_imp->m_doT0Shift) ATH_CHECK( m_imp->m_t0ShiftSvc.retrieve() );

  if (m_imp->m_doTMaxShift) ATH_CHECK ( m_imp->m_tMaxShiftSvc.retrieve() );

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
}  //end MdtCalibrationTool::initialize

bool MdtCalibrationTool::driftRadiusFromTime( MdtCalibHit &hit,
                                             double signedTrackLength,
                                             double triggerTime,
                                             bool resolFromRtrack ) const {
  MdtCalibrationSvcInput inputData;
  inputData.tof = signedTrackLength*m_imp->m_inverseSpeedOfLight;
  inputData.triggerOffset = triggerTime;
  return driftRadiusFromTime( hit, inputData, m_imp->settings, resolFromRtrack );
}

bool MdtCalibrationTool::driftRadiusFromTime( MdtCalibHit &hit,
                                             const MdtCalibrationSvcInput &inputData,
                                             bool resolFromRtrack ) const {
  return driftRadiusFromTime( hit, inputData, m_imp->settings, resolFromRtrack );
}

bool MdtCalibrationTool::driftRadiusFromTime( MdtCalibHit &hit,
                                             const MdtCalibrationSvcInput &inputData,
                                             const MdtCalibrationSvcSettings &settings,
                                             bool resolFromRtrack ) const {

  if( settings.timeWindowUpperBound() < 0. || settings.timeWindowLowerBound() < 0. ) {
    // Should be an ERROR, but can't return StatusCode::FAILURE
    ATH_MSG_WARNING( "Uninitialized settings object at " << &settings );
    return false;
  }

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Input: tof " << inputData.tof << " trigger offset " << inputData.triggerOffset);
    if( inputData.pointOfClosestApproach ) ATH_MSG_VERBOSE("  PointOfClosestApproach");
    if( inputData.trackDirection )         ATH_MSG_VERBOSE("  TrackDirection");
    if( inputData.nominalWireSurface )     ATH_MSG_VERBOSE("  Nom. WireSurface");
    if( inputData.wireSurface )            ATH_MSG_VERBOSE("  Sagged Wiresurface");
    ATH_MSG_VERBOSE("Settings: window " << settings.timeWindowLowerBound() << "  " << settings.timeWindowUpperBound());
    if( settings.doTof )     ATH_MSG_VERBOSE(" Tof");
    if( settings.doProp )    ATH_MSG_VERBOSE(" Prop");
    if( settings.doTemp )    ATH_MSG_VERBOSE(" Temp");
    if( settings.doField )   ATH_MSG_VERBOSE(" Field");
    if( settings.doWireSag ) ATH_MSG_VERBOSE(" WireSag");
    if( settings.doSlew )    ATH_MSG_VERBOSE(" Slew");
    if( settings.doBkg )     ATH_MSG_VERBOSE(" Bkg");
  }

  const Identifier &id = hit.identify();
  const MuonGM::MdtReadoutElement *geo = hit.geometry();
  // set geometry pointer if not yet set
  if ( !geo ) {
    geo = m_imp->getGeometry( id );
    if ( !geo ) return false;
    hit.setGeometry( geo );
  }

  // get calibration constants from DbTool
  MuonCalib::MdtFullCalibData data = m_dbTool->getCalibration( geo->collectionHash(),
								     geo->detectorElementHash() );

  // require at least the MdtRtRelation to be available
  const MuonCalib::MdtRtRelation *rtRelation = data.rtRelation;
  // Hardcoded MDT tube radius 14.6mm here - not correct for sMDT
  // on the other hand it should be rare that a tube does not have an RT
  if( !rtRelation ) {
    ATH_MSG_WARNING( "no rtRelation found, cannot calibrate tube" );
    hit.setDriftRadius(0., twoBySqrt12*geo->innerTubeRadius());
    return false;
  }
  double t0(0.);
  double inversePropSpeed = m_imp->m_inversePropagationSpeed;
  double adcCal(1.);

  // access t0 for the given tube
  if ( data.tubeCalib ) {

    const int ml    = m_idHelperSvc->mdtIdHelper().multilayer(id)-1;
    const int layer = m_idHelperSvc->mdtIdHelper().tubeLayer(id)-1;
    const int tube  = m_idHelperSvc->mdtIdHelper().tube(id)-1;

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
      ATH_MSG_WARNING("failed to access tubedata for "
			<< ml << " " << layer << " " << tube
			<< " using defaults.. ");
      if ( geo )
	ATH_MSG_WARNING("detel " << geo->getMultilayer()
			  << " lay " << geo->getNLayers()
			  << " tubes " << geo->getNtubesperlayer());
      t0 =  800.;
    }

    // get t0 shift from tool (default: no shift, value is zero)
    if (m_imp->m_doT0Shift) t0 += m_imp->m_t0ShiftSvc->getValue(id);
  } else {
    ATH_MSG_WARNING("MdtTubeCalibContainer not found for "
		      << m_idHelperSvc->mdtIdHelper().print_to_string( id ));
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

    MagField::AtlasFieldCache fieldCache;
    if (settings.doField) {
      // 
      // For the moment, use Gaudi Hive for the event context - would need to be passed in from clients
      SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
      const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
      if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("driftRadiusFromTime: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
        return false;
      }
      fieldCondObj->getInitializedCache (fieldCache);
    }
    corTime = m_imp->applyCorrections( fieldCache, hit, inputData, settings,
				       adcCal, data.corrections, rtRelation->rt(), &msg(), &m_idHelperSvc->mdtIdHelper());
  }

  // calculate drift radius + error
  double r(0.);
  double reso(1.);
  double t = hit.driftTime();
  double t_inrange = hit.driftTime();
  bool calibOk = true;
  Muon::MdtDriftCircleStatus timeStatus = driftTimeStatus(t, rtRelation, settings);
  if( rtRelation->rt() ){

    r = rtRelation->rt()->radius(t);

    // apply tUpper gshift
    if (m_imp->m_doTMaxShift) {
      float tShift = m_imp->m_tMaxShiftSvc->getValue(id);
      r = rtRelation->rt()->radius( t * (1 + tShift) );
    }

    // check whether drift times are within range, if not fix them to the min/max range
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
      reso = rtRelation->rtRes()->resolution(rtRelation->tr()->tFromR(std::abs(hit.signedDistanceToTrack()),
								      out_of_bound_flag) );
    }
  }else{
    ATH_MSG_WARNING( "no rtRes found" );
  }
  hit.setDriftRadius( r, reso );

  // summary
  ATH_MSG_VERBOSE( "driftRadiusFromTime for tube " << m_idHelperSvc->mdtIdHelper().print_to_string(id)
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
}  //end MdtCalibrationTool::driftRadiusFromTime

// + TWIN TUBES
bool MdtCalibrationTool::twinPositionFromTwinHits( MdtCalibHit &hit,
                                                  MdtCalibHit &secondHit,
                                                  double signedTrackLength,
                                                  double secondSignedTrackLength,
                                                  bool &secondDigitIsPrompt,
                                                  double triggerTime ) const {
  MdtCalibrationSvcInput inputData;
  inputData.tof = signedTrackLength*m_imp->m_inverseSpeedOfLight;
  inputData.triggerOffset = triggerTime;

  MdtCalibrationSvcInput secondInputData;
  secondInputData.tof = secondSignedTrackLength*m_imp->m_inverseSpeedOfLight;
  secondInputData.triggerOffset = triggerTime;
  return twinPositionFromTwinHits( hit, secondHit, inputData,
				   secondInputData, m_imp->settings, secondDigitIsPrompt );
}

bool MdtCalibrationTool::twinPositionFromTwinHits( MdtCalibHit &hit,
                                                  MdtCalibHit &secondHit,
                                                  const MdtCalibrationSvcInput &inputData,
                                                  const MdtCalibrationSvcInput &secondInputData,
                                                  const MdtCalibrationSvcSettings &settings,
                                                  bool &secondDigitIsPrompt ) const {

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
  // get calibration constants from DbTool
  MuonCalib::MdtFullCalibData data = m_dbTool->getCalibration( geo->collectionHash(), geo->detectorElementHash() );
  MuonCalib::MdtFullCalibData dataSecond = m_dbTool->getCalibration( geoSecond->collectionHash(), geoSecond->detectorElementHash() );

  double inversePropSpeed = m_imp->m_inversePropagationSpeed;
  double inversePropSpeedSecond = m_imp->m_inversePropagationSpeed;

  // access t0 for the give tube
  if( data.tubeCalib ){

    const int ml    = m_idHelperSvc->mdtIdHelper().multilayer(id)-1;
    const int layer = m_idHelperSvc->mdtIdHelper().tubeLayer(id)-1;
    const int tube  = m_idHelperSvc->mdtIdHelper().tube(id)-1;

    if( ml<0 || layer<0 || tube<0 ){
      ATH_MSG_WARNING( "Oops negative index....." );
      return false;
    }

    // extract calibration constants for single tube
    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib *singleTubeData =
      data.tubeCalib->getCalib( ml, layer, tube );
    if ( singleTubeData ){
      inversePropSpeed = singleTubeData->inversePropSpeed;
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
		     << m_idHelperSvc->mdtIdHelper().print_to_string( id ) );
    ATH_MSG_WARNING( "Tube cannot be calibrated!!!" );
    return false;
  }

  // access t0 for the given second tube
  if ( dataSecond.tubeCalib ){
    const int mlSecond    = m_idHelperSvc->mdtIdHelper().multilayer(idSecond)-1;
    const int layerSecond = m_idHelperSvc->mdtIdHelper().tubeLayer(idSecond)-1;
    const int tubeSecond  = m_idHelperSvc->mdtIdHelper().tube(idSecond)-1;

    if( mlSecond<0 || layerSecond<0 || tubeSecond<0 ){
      ATH_MSG_WARNING( "Oops negative index for second tube....." );
      return false;
    }

    // extract calibration constants for single tube
    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib *singleTubeDataSecond =
      dataSecond.tubeCalib->getCalib( mlSecond, layerSecond, tubeSecond );
    if ( singleTubeDataSecond ){
      inversePropSpeedSecond = singleTubeDataSecond->inversePropSpeed;
    } else {
      ATH_MSG_WARNING( "failed to access (second) tubedata for " << mlSecond
		       << " " << layerSecond << " " << tubeSecond
		       << " using defaults.. " );
      if ( geoSecond )
	ATH_MSG_WARNING( "detel " << geoSecond->getMultilayer()
			 << " lay " << geoSecond->getNLayers()
			 << " tubes " << geoSecond->getNtubesperlayer() );
    }
  } else {
    ATH_MSG_WARNING( "MdtTubeCalibContainer not found for "
		     << m_idHelperSvc->mdtIdHelper().print_to_string( idSecond ) );
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
  int prompthit_tdc = 0;
  int twinhit_tdc = 0;
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
    if (msgLvl(MSG::DEBUG)) {
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
      if (msgLvl(MSG::DEBUG)) {
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

    if (msgLvl(MSG::VERBOSE)) {
      ATH_MSG_VERBOSE( " TWIN TUBE "
		     << " tube: " << m_idHelperSvc->mdtIdHelper().print_to_string(id)
		     << " twintube: " << m_idHelperSvc->mdtIdHelper().print_to_string(idSecond)
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

    if (msgLvl(MSG::VERBOSE)) {
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
}  //end MdtCalibrationTool::twinPositionFromTwinHits

double MdtCalibrationTool::tdcBinSize(const Identifier &id) const {
//BMG which uses HPTDC instead of AMT, and has 0.2ns TDC ticksize
  if( m_idHelperSvc->mdtIdHelper().stationName(id) == m_imp->m_BMGid && m_imp->m_BMGpresent)
    return 0.1953125; // 25/128
  return 0.78125;  //25/32; exact number: (1000.0/40.079)/32.0
}

double MdtCalibrationTool::Imp::applyCorrections(MagField::AtlasFieldCache& fieldCache,
                                                 MdtCalibHit &hit,
                                                 const MdtCalibrationSvcInput &inputData,
                                                 const MdtCalibrationSvcSettings &settings,
                                                 double /*adcCal*/,
                                                 const MuonCalib::MdtCorFuncSet *corrections, const MuonCalib::IRtRelation *rt, MsgStream *msgStr, const MdtIdHelper *idHelp) const {

  double corTime(0.);
  // apply corrections
  if ( corrections ){

    if (msgStr->level()<=MSG::VERBOSE) *msgStr << MSG::VERBOSE << "There are correction functions." << endmsg;

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
        fieldCache.getField(xyz, BGXYZ);
        
	for (int i=0; i<3; i++) BGXYZ[i] *= 1000.; // convert kT to Tesla
        Amg::Vector3D B_global(BGXYZ[0], BGXYZ[1], BGXYZ[2]);
        Amg::Vector3D B_loc(gToStation.linear()*B_global);
        double Bpar = B_loc.x();
        Amg::Vector3D loc_dir(gToStation.linear()*(*(inputData.trackDirection)));
        Amg::Vector3D dir(0.0, loc_dir.y(), loc_dir.z());
        double Bper = B_loc.dot(dir.unit());
        hit.setBFieldPerp(Bper);
        hit.setBFieldPara(Bpar);
        if(msgStr->level()<=MSG::VERBOSE) *msgStr << MSG::VERBOSE << "doing b-field correction" << endmsg;
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
      float scle_time=MuonCalib::RtScaleFunction(hit.driftTime(), idHelp->multilayer(hit.identify())==2, *rt);
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
    if (msgStr->level()<=MSG::VERBOSE) {
      if ( settings.doWireSag ){
        *msgStr << MSG::VERBOSE << "settings.doWireSag == TRUE" << endmsg;
      } else {
        *msgStr << MSG::VERBOSE << "settings.doWireSag == FALSE" << endmsg;
      }
      if ( corrections->wireSag() ){
        *msgStr << MSG::VERBOSE << "corrections->wireSag() == TRUE" << endmsg;
      } else {
        *msgStr << MSG::VERBOSE << "corrections->wireSag() == FALSE" << endmsg;
      }
    }
    // Wire sag corrections
    if ( settings.doWireSag && corrections->wireSag() ){

      if (msgStr->level()<=MSG::VERBOSE) {
        *msgStr << MSG::VERBOSE << "Performing Rt Corrections due to Wire Sag." << endmsg;

	if (inputData.pointOfClosestApproach) {
        *msgStr << MSG::VERBOSE << "Have a point of closest approach." << endmsg;
	} else {
        *msgStr << MSG::VERBOSE << "No point of closest approach!" << endmsg;
	}
	if (inputData.trackDirection) {
        *msgStr << MSG::VERBOSE << "Have a track direction." << endmsg;
	} else {
        *msgStr << MSG::VERBOSE << "No track direction!" << endmsg;
	}
	if (inputData.nominalWireSurface) {
        *msgStr << MSG::VERBOSE << "Have a nominal wire surface." << endmsg;
	} else {
        *msgStr << MSG::VERBOSE << "No nominal wire surface!" << endmsg;
	}
	if (inputData.wireSurface) {
        *msgStr << MSG::VERBOSE << "Have a sagged wire surface." << endmsg;
	} else {
        *msgStr << MSG::VERBOSE << "No sagged wire surface!" << endmsg;
	}
      }

      const Amg::Vector3D *pointOfClosestApproach = inputData.pointOfClosestApproach;
      const Amg::Vector3D *trackDirection         = inputData.trackDirection;
      const Trk::StraightLineSurface *nominalWireSurface = inputData.nominalWireSurface;
      const Trk::StraightLineSurface *wireSurface = inputData.wireSurface;

      // check whether all input data is there
      if ( !pointOfClosestApproach || !trackDirection || !nominalWireSurface ){
        *msgStr << MSG::WARNING << " cannot perform wire sag correction: " << endmsg;
        if( !pointOfClosestApproach ) *msgStr << MSG::WARNING << " no pointOfClosestApproach " << endmsg;
        if( !trackDirection )         *msgStr << MSG::WARNING << " no trackDirection " << endmsg;
        if( !nominalWireSurface )     *msgStr << MSG::WARNING << " no nominalWireSurface " << endmsg;
      } else {

	if (msgStr->level()<=MSG::VERBOSE) {
        *msgStr << MSG::VERBOSE << "All Necessary Wire Sag data available: " << endmsg;
        *msgStr << MSG::VERBOSE << "  pCA = ("
		 << pointOfClosestApproach->x() << ", "
		 << pointOfClosestApproach->y() << ", "
		 << pointOfClosestApproach->z() << ")" << endmsg;
	}

        // store pointer to sagged surface as we get ownership if we recalculate it
        const Trk::StraightLineSurface *tempSaggedSurface = nullptr;

        // if wire surface is missing, calculate sagged wire position
        if ( !wireSurface ){
          const Trk::SaggedLineSurface *nominalSurf = dynamic_cast<const Trk::SaggedLineSurface*>(nominalWireSurface);
          if ( nominalSurf ){
            // Local position for calculation of position along the tube,
            // used for wire sag treatment
            const Amg::Vector2D *tempLocOnWire = nominalSurf->Trk::Surface::globalToLocal(*pointOfClosestApproach,1000.);
            if ( !tempLocOnWire ){
              *msgStr << MSG::WARNING << "globalToLocal failed! " << endmsg;
            } else {
              // sagged surface
              wireSurface = nominalSurf->correctedSurface(*tempLocOnWire);
              tempSaggedSurface = wireSurface;
              delete tempLocOnWire;
            }
          } else {
            *msgStr << MSG::WARNING << "Nominal wire surface not a SaggedLineSurface,"
		   << " cannot perform wire sag correction" << endmsg;
          }
        }

        if ( !wireSurface ){
          *msgStr << MSG::WARNING << " cannot perform wire sag correction: no sagged wire surface " << endmsg;
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

          double signedDriftRadius = deltaY*(std::abs(hit.driftRadius()/deltaY));

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
}  //end MdtCalibrationTool::Imp::applyCorrections

Muon::MdtDriftCircleStatus MdtCalibrationTool::driftTimeStatus( double driftTime, const MuonCalib::MdtRtRelation *rtRelation, const MdtCalibrationSvcSettings &settings ) const {
  if( settings.timeWindowUpperBound() < 0. || settings.timeWindowLowerBound() < 0. ) {
      ATH_MSG_WARNING( " Unphysical time window detected, both ranges should be positive: "
		       << " lower bound " << settings.timeWindowLowerBound()
		       << " upper bound " << settings.timeWindowUpperBound() );
  }

  if( rtRelation && rtRelation->rt() ) {
    if( driftTime < rtRelation->rt()->tLower() - settings.timeWindowLowerBound() ) {
      // Will this produce too much output from ROT creators?
      if (msgLvl(MSG::VERBOSE))
	ATH_MSG_VERBOSE( " drift time outside time window "
			 << driftTime << ". Mininum time = "
			 << rtRelation->rt()->tLower() - settings.timeWindowLowerBound() );
      return Muon::MdtStatusBeforeSpectrum;
    } else if( driftTime > rtRelation->rt()->tUpper() + settings.timeWindowUpperBound()  ) {
      if (msgLvl(MSG::VERBOSE))
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
}  //end MdtCalibrationTool::driftTimeStatus
