/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
//   Implementation file for class MdtDriftCircleOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for MdtDriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/07/2004
///////////////////////////////////////////////////////////////////

#include "StoreGate/StoreGateSvc.h"
#include "MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
#include "MdtCalibData/MdtRtRelation.h"
#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/TrRelation.h"

#include "MuonCalibEvent/MdtCalibHit.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonRecToolInterfaces/IMuonTofTool.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkDistortedSurfaces/DistortedSurface.h"
#include "Identifier/IdentifierHash.h"
#include <boost/assign/std/vector.hpp>

Muon::MdtDriftCircleOnTrackCreator::MdtDriftCircleOnTrackCreator
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_mdtCalibSvc("MdtCalibrationSvc", na),
  m_mdtCalibDbSvc("MdtCalibrationDbSvc", na),
  m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
  m_tofTool(""),
  m_invSpeed(1./299.792458),
  m_mdtCalibSvcSettings( 0 ),
  m_calibHit( 0 ),
  m_errorStrategy(Muon::MuonDriftCircleErrorStrategyInput())
{
  // create calibration service settings 
  m_mdtCalibSvcSettings = new MdtCalibrationSvcSettings();
  
  // algtool interface - necessary!
  declareInterface<IMdtDriftCircleOnTrackCreator>(this);
  declareInterface<IRIO_OnTrackCreator>(this);
  
  // --- if any of those false, then just copy, but no update.
  declareProperty("doMDT",  m_doMdt = true);
  declareProperty("TimingMode", m_timeCorrectionType = 0 );
  declareProperty("MuonTofTool", m_tofTool );
  declareProperty("ErrorScalingTool", m_errorScalingTool );
  declareProperty("FixedError",m_fixedError = 1.);
  declareProperty("DiscardMaskedHits",m_discardMaskedHits = true);
  declareProperty("GlobalToLocalTolerance",m_globalToLocalTolerance = 1000., "Tolerance used for the Surface::globalToLocal" );
  
  // configuration of the calibration service settings
  declareProperty("TimeWindowLowerBound",m_mdtCalibSvcSettings->windowLowerBound );
  declareProperty("TimeWindowUpperBound",m_mdtCalibSvcSettings->windowUpperBound );
  declareProperty("TimeWindowSetting",m_mdtCalibSvcSettings->windowSetting );
  
  // configuration of the default error strategy
  declareProperty("DefaultErrorStrategy", m_defaultStrategy = "Moore", "Default error strategy to be used in calculating errors" );
  declareProperty("CreateTubeHit",m_createTubeHits = false );
  declareProperty("DoErrorScaling",m_scaleMdtCov = true, "Switch on error scaling" );
  declareProperty("DoFixedError",m_doFixedError = true);
  declareProperty("UseParametrisedError", m_useErrorParametrisation = false );
  declareProperty("UseErrorAtPredictedPosition", m_errorAtPredictedPosition = false,
                  "The error will be adjusted to be that corresponding to the predicted position. This is useful to fix problems with tracks very close to the wire.");
  declareProperty("DoWireSag",m_doWireSag = false );
  declareProperty("DoStationError",m_stationError=false,"Add a term to the error to account for poorly aligned stations");
  declareProperty("T0RefitError",m_t0Refit=false,"Add an error to account for the T0 refit");
  declareProperty("DeweightIndividualChambers",m_doIndividualChamberReweights = true, "Apply chamber specific deweighting");
  declareProperty("DoTofCorrection",m_mdtCalibSvcSettings->doTof );
  declareProperty("DoPropagationCorrection",m_mdtCalibSvcSettings->doProp );
  declareProperty("DoTemperatureCorrection",m_mdtCalibSvcSettings->doTemp );
  declareProperty("DoMagneticFieldCorrection",m_mdtCalibSvcSettings->doField );
  declareProperty("DoWireSagCorrection",m_mdtCalibSvcSettings->doWireSag );
  declareProperty("DoSlewingCorrection",m_mdtCalibSvcSettings->doSlew );
  declareProperty("DoBackgroundCorrection",m_mdtCalibSvcSettings->doBkg );
  declareProperty("DoSegmentErrors",m_doSegments=true , "Use error strategy for segments");
  declareProperty("UseLooseErrors",m_looseErrors=false , "Use error strategy for MC");
  declareProperty("IsMC",m_isMC=false);
  
  fillMboyParametrisedErrors();
}


Muon::MdtDriftCircleOnTrackCreator::~MdtDriftCircleOnTrackCreator(){}


StatusCode Muon::MdtDriftCircleOnTrackCreator::initialize()
{

  m_mdtCalibSvcSettings->initialize();
  
  m_calibHit = new MdtCalibHit();
  
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::BroadError,               m_createTubeHits                 );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ScaledError,              m_scaleMdtCov                    );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::FixedError,               m_doFixedError                   );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors,      m_useErrorParametrisation        );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::StationError,             m_stationError                   );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition, m_errorAtPredictedPosition       );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::T0Refit,                  m_t0Refit                        );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection,    m_doWireSag                      );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::TofCorrection,            m_mdtCalibSvcSettings->doTof     );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::PropCorrection,           m_mdtCalibSvcSettings->doProp    );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::TempCorrection,           m_mdtCalibSvcSettings->doTemp    );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::MagFieldCorrection,       m_mdtCalibSvcSettings->doField   );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::WireSagTimeCorrection,    m_mdtCalibSvcSettings->doWireSag );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::SlewCorrection,           m_mdtCalibSvcSettings->doSlew    );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::BackgroundCorrection,     m_mdtCalibSvcSettings->doBkg     );
  m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::Segment,                  m_doSegments                     );
  m_errorStrategy.setCalibWindow(m_mdtCalibSvcSettings->timeWindowSetting());
  
  if ("Moore" == m_defaultStrategy){
    m_errorStrategy.setStrategy(MuonDriftCircleErrorStrategy::Moore);
  } else if ("Muonboy" == m_defaultStrategy){
    m_errorStrategy.setStrategy(MuonDriftCircleErrorStrategy::Muonboy);
  } else if ("Muon" == m_defaultStrategy){
    m_errorStrategy.setStrategy(MuonDriftCircleErrorStrategy::Muon);
  } else {
    // By default use one of the real strategies - don't default to unknown!
    m_errorStrategy.setStrategy(MuonDriftCircleErrorStrategy::Muon);
  }
  msg(MSG::INFO) << "Constructed default MuonDriftCircleErrorStrategy: ";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::BroadError ) ) msg(MSG::INFO) << " Broad";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::ScaledError ) ) msg(MSG::INFO) << " Scaled"; 
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::FixedError ) ) msg(MSG::INFO) << " Fixed";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors ) ) msg(MSG::INFO) << " Parm";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::StationError ) ) msg(MSG::INFO) << " Station";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition ) ) msg(MSG::INFO) << " ErrAtPos";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::T0Refit ) ) msg(MSG::INFO) << " T0";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection ) ) msg(MSG::INFO) << " WireG";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::TofCorrection ) ) msg(MSG::INFO) << " TOF";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::PropCorrection ) ) msg(MSG::INFO) << " Prop";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::TempCorrection ) ) msg(MSG::INFO) << " Temp";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::MagFieldCorrection ) ) msg(MSG::INFO) << " Mag";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::WireSagTimeCorrection ) ) msg(MSG::INFO) << " WireT";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::SlewCorrection ) ) msg(MSG::INFO) << " Slew";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::BackgroundCorrection ) ) msg(MSG::INFO) << " Back";
  if( m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::Segment ) ) msg(MSG::INFO) << " Seg";
  msg(MSG::INFO) << endmsg;
  
  if( !m_isMC && m_looseErrors )  ATH_MSG_INFO( "Using Data Loose error tuning");
  if( !m_isMC && !m_looseErrors ) ATH_MSG_INFO( "Using Data Tight error tuning");
  if( m_isMC )                    ATH_MSG_INFO( "Using MC error tuning");
  ATH_MSG_VERBOSE( "A correction is made if set to true: do_MDT = " << m_doMdt ); 
  
  if (m_idHelper.retrieve().isFailure()) {  
    msg(MSG::ERROR) << " Cannot retrieve " << m_idHelper << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (m_doMdt) {
    if( m_mdtCalibSvc.retrieve().isFailure() ) {
      msg(MSG::ERROR) << " Could not initialize MdtCalibration Service" << endmsg;
      return StatusCode::FAILURE;
    }
    // Get pointer to MdtCalibrationDbSvc and cache it :
    if ( m_mdtCalibDbSvc.retrieve().isFailure() ) {
      msg(MSG::ERROR) << MSG::ERROR << "Unable to retrieve pointer to MdtCalibrationDbSvc" << endmsg;
      return StatusCode::FAILURE;
    }
    
  } else {
    msg(MSG::WARNING) << " tool is configured such that MDT_DCs are only copied!"
    << endmsg;
  }
  
  // get error scaling tool
  if(m_errorScalingTool.retrieve().isFailure())   {
    msg(MSG::WARNING) << "Can not get error scaling tool " << m_errorScalingTool
    << ", will trigger failure." << endmsg;
    return StatusCode::SUCCESS;
  } 
  
  if( m_timeCorrectionType == COSMICS_TOF ){
    if( m_tofTool.empty() ) {
      msg(MSG::ERROR) << "The time of flight tool is not configured. Please check your configuration"  << endmsg;
      return StatusCode::FAILURE;
    }
    if( m_tofTool.retrieve().isSuccess() ){
      ATH_MSG_DEBUG("Retrieved " << m_tofTool << endmsg);
    }else{
      msg(MSG::ERROR)<<"Could not get " << m_tofTool <<endmsg; 
      return StatusCode::FAILURE;
    }
    if( !m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::TofCorrection) ){
      msg(MSG::WARNING) << "Detected bad default configuration, using Cosmic TOF witout time of flight corrections does not work" << endmsg;
    }
  }
  ATH_MSG_DEBUG("Timing mode set to  " << m_timeCorrectionType << endmsg);
  if (m_timeCorrectionType>=NumberOfTimingModes) {
    msg(MSG::ERROR)<<"Time Correction Type too large! Aborting."<<endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
} 

StatusCode Muon::MdtDriftCircleOnTrackCreator::finalize()
{
  delete m_calibHit;
  delete m_mdtCalibSvcSettings;
  return AthAlgTool::finalize(); 
}

const Muon::MdtDriftCircleOnTrack* Muon::MdtDriftCircleOnTrackCreator::createRIO_OnTrack( 
                                                                                         const MdtPrepData& mdtPrd,
                                                                                         const Amg::Vector3D& GP, 
                                                                                         const Amg::Vector3D* GD,
                                                                                         const MuonDriftCircleErrorStrategy* strategy ) const
{
  const MuonDriftCircleErrorStrategy* myStrategy = 0==strategy?&m_errorStrategy:strategy;
  
  const Identifier& iD = mdtPrd.identify();
  
  // check whether the MdtPrepData is not a masked hit
  if( m_discardMaskedHits && mdtPrd.status() == Muon::MdtStatusMasked ){
    ATH_MSG_VERBOSE( "Unable to calibrate Masked hit, returning zero: channel "
                    << m_idHelper->toString(iD) );
    return 0;
  }
  
  // ************************
  // MdtDriftCircleOnTrack production
  
  // surface of nominal wire
  const Trk::SaggedLineSurface* nominalSurf=&mdtPrd.detectorElement()->surface(iD);



  // Local position for calculation of postion along the tube, used for wire sag treatment
 const Amg::Vector2D* tempLocOnWire = nominalSurf->Trk::Surface::globalToLocal(GP,m_globalToLocalTolerance);
  if( !tempLocOnWire ){
    msg(MSG::WARNING) << "globalToLocal failed! " << endmsg;
    return 0;
  }
  
  // if wire sag is taken into account, cast the surface to StraightLineSurface so it can be added to the ROT
  const Trk::StraightLineSurface* saggedSurf = 0;
  if (myStrategy->creationParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection)) {
    saggedSurf = nominalSurf->correctedSurface(*tempLocOnWire); // sagged surface
    if( !saggedSurf ){
      msg(MSG::WARNING) << "DistortedSurface failed to create sagged surface, using nominal surface " << endmsg;
      saggedSurf = nominalSurf;
    }
    
    //set large value for tolerance, to make sure that global position including drift radius is taken to the wire.
    const Amg::Vector2D* tempLocOnSaggedWire = saggedSurf->Trk::Surface::globalToLocal(GP,m_globalToLocalTolerance);
    delete tempLocOnWire; 
    if( !tempLocOnSaggedWire ){
      msg(MSG::WARNING) << "globalToLocal failed for sagged surface, not applying sagging! " << endmsg;
      return 0;
    }else{
      // replace tempLocOnWire with tempLocOnSaggedWire
      tempLocOnWire = tempLocOnSaggedWire;
    }
    // PK not needed delete tempLocOnSaggedWire;
  }
  
  double positionAlongWire = (*tempLocOnWire)[Trk::locZ];
  delete tempLocOnWire;
  // set driftcirclestatus, NODRIFTTIME if creating tube hits else UNDECIDED
  Trk::DriftCircleStatus dcstatus = m_doMdt ? Trk::UNDECIDED : Trk::NODRIFTTIME;
  
  // ************************
  // calculate local position and local error matrix
  MdtCalibrationSvcInput inputData;
  inputData.pointOfClosestApproach = &GP;
  inputData.trackDirection = GD;
  inputData.nominalWireSurface = nominalSurf;
  inputData.wireSurface = saggedSurf;
  CalibrationOutput calibOutput = getLocalMeasurement( mdtPrd, GP, GD,inputData, myStrategy ); 
  // This basically determines the error etc and is where the bulk of the work is done.
  
  // hack to determine whether we are before or after the spectrum, until we sort this out properly 
  if( !calibOutput.calibOk && calibOutput.driftTime > 500. ){
    ATH_MSG_DEBUG("Unable to perform calibration ");
    delete saggedSurf;
    return 0;
  }
  
  MdtDriftCircleOnTrack* rot =0;
  // we have to calculate sign, check whether direction is given
  if( m_doMdt && GD!=0 ) {
    // calculate sign using surface
    
    const Trk::StraightLineSurface& surf = saggedSurf ? *saggedSurf : *nominalSurf;
    const Amg::Vector2D* pos = surf.Trk::Surface::globalToLocal( GP, *GD);
    
    // check this might still fail....
    if( !pos ){
      msg(MSG::WARNING) << "Unexpected globalToLocal failure, cannot create MDT ROT " << endmsg;
      // clean up memory
      delete saggedSurf;
      return 0;
    }
    
    // calculate sign
    double sign = (*pos)[Trk::driftRadius] < 0 ? -1.0 : 1.0;
 calibOutput.locPars[Trk::driftRadius]*=sign;
    rot = new MdtDriftCircleOnTrack( &mdtPrd,
                                    calibOutput.locPars,
                                    calibOutput.locErr,
                                    calibOutput.driftTime,
                                    Trk::DECIDED,
                                    *GD,
                                    positionAlongWire,
                                    *myStrategy,
                                    saggedSurf );  
    delete pos;
    // @todo - improve simplistic test below. EJWM.
    // double tolerance=0.01;
    // if ( m_debug && (GP.x()-rot->globalPosition().x())>tolerance
    //     && (GP.y()-rot->globalPosition().y())>tolerance 
    //     && (GP.z()-rot->globalPosition().z())>tolerance  ) *m_log<<MSG::WARNING<<"Global position differs: before = "<<GP<<"\t after="<<rot->globalPosition()<<endmsg;    
  }else{
    // If the track direction is missing, the B-field correction was not applied 
    if (inputData.trackDirection) {
      // do not have access to direction, so have to use partial constructor:
      rot = new MdtDriftCircleOnTrack( &mdtPrd,
                                      calibOutput.locPars,
                                      calibOutput.locErr,
                                      calibOutput.driftTime,
                                      dcstatus,
                                      positionAlongWire,
                                      *myStrategy,
                                      saggedSurf );
    } else {
      MuonDriftCircleErrorStrategy tmpStrategy(myStrategy->getBits());
      tmpStrategy.setParameter(MuonDriftCircleErrorStrategy::MagFieldCorrection, false);
      rot = new MdtDriftCircleOnTrack( &mdtPrd,
                                      calibOutput.locPars,
                                      calibOutput.locErr,
                                      calibOutput.driftTime,
                                      dcstatus,
                                      positionAlongWire,
                                      tmpStrategy,
                                      saggedSurf );
    }
  }
  ATH_MSG_DEBUG( "MDT ROT: radius = " << rot->localParameters().get(Trk::driftRadius) 
                << " error = " << Amg::error(rot->localCovariance(),Trk::locR)
                << " error = " << calibOutput.locErr
			 << " ,channel " << m_idHelper->toString(iD) );
  
  return rot;
}

void Muon::MdtDriftCircleOnTrackCreator::updateSign( 
                                                    MdtDriftCircleOnTrack& caliDriftCircle, 
                                                    Trk::DriftCircleSide si) const
{
  // ************************
  // Apply additional corrections to local position 
  Trk::LocalParameters& lpos =  const_cast<Trk::LocalParameters&>(caliDriftCircle.localParameters()) ;
  
  // set sign LocalPosition
  if ( si == Trk::LEFT ){
    lpos[Trk::driftRadius] = -std::abs( lpos[Trk::driftRadius] );
  }else{
    lpos[Trk::driftRadius] = std::abs( lpos[Trk::driftRadius] );
  }
  
  caliDriftCircle.setLocalParameters( lpos );
  caliDriftCircle.m_status = Trk::DECIDED;
  ATH_MSG_VERBOSE( "MDT DriftCircleOnTrack made with radius = "<< lpos );
  return ;
} 

Muon::MdtDriftCircleOnTrackCreator::CalibrationOutput 
Muon::MdtDriftCircleOnTrackCreator::getLocalMeasurement(const MdtPrepData& DC, 
                                                        const Amg::Vector3D& gpos,
                                                        const Amg::Vector3D* /**gdir*/,
                                                        MdtCalibrationSvcInput& inputData,
                                                        const MuonDriftCircleErrorStrategy* strategy ) const
{
  const MuonDriftCircleErrorStrategy* myStrategy = 0==strategy?&m_errorStrategy:strategy;
  
  ATH_MSG_VERBOSE( "getLocalMeasurement with m_doMdt="<<m_doMdt<<" and "<<(*myStrategy));
  // ATH_MSG_INFO( "Dumping PRD: "<<DC);
  
  double radius(0.),errRadius(0.),sigmaR(1.),driftTime(0.);
  bool ok = true;
  
  if ( m_doMdt ) {
    const MuonGM::MdtReadoutElement* detEl = DC.detectorElement();
    if(!detEl){
      msg(MSG::WARNING) << "MdtPrepData without not a MdtReadoutElement" << endmsg;
  
Amg::MatrixX  localCov(1,1);
  localCov(0,0) = 0.0;

return CalibrationOutput(Amg::Vector2D(),localCov,0.,false);
    }
    
    // call calibration Service
    m_calibHit->setIdentifier( DC.identify() );
    m_calibHit->setTdc( DC.tdc() );
    m_calibHit->setAdc( DC.adc() );
    m_calibHit->setGlobalPointOfClosestApproach(gpos);
    m_calibHit->setGeometry(detEl);
    
    switch (m_timeCorrectionType){
      case ATLTIME:
        // normal time of flight corrections assuming IP + light speed
        inputData.tof = gpos.mag()*m_invSpeed;
        ATH_MSG_VERBOSE( " running in ATLTIME mode, tof: " << inputData.tof );
        break;
      case NO_CORRECTIONS:
        // special case for cosmics taken with scintilator trigger or cosmic simulation without TOF
        inputData.tof = 0.;
        ATH_MSG_VERBOSE( "running in NO_CORRECTIONS mode, tof: " << inputData.tof );
        break;
      case COSMICS_TRIGGERTIME:
        // special case for cosmics taken with scintilator trigger which is read out so the offset with 
        // respect to the clock is known
        // getTriggerTime() NOT IMPLEMENETED YET!!!!!!
        inputData.triggerOffset = getTriggerTime();
        ATH_MSG_VERBOSE( " running in COSMICS_TRIGGERTIME mode, triggerOffset: " 
                        << inputData.triggerOffset );
        break;
      case COSMICS_TOF:
        // case for normal cosmic data with rpc trigger or simulation including TOF
        inputData.tof = m_tofTool->timeOfFlight( DC.identify(), gpos );
        ATH_MSG_VERBOSE( " running in COSMICS_TOF mode, tof: " << inputData.tof );
        break;
      default:
        // default, no tof. Indicates wrong configuration
        msg(MSG::WARNING) << "No valid mode selected, cannot apply tof correction"  << endmsg;
        inputData.tof = 0.;
        break;
    }
    
    // call the calibration service providing the time when the particle passed the tube
    ok = m_mdtCalibSvc->driftRadiusFromTime( *m_calibHit, inputData, *m_mdtCalibSvcSettings );
    
    driftTime = m_calibHit->driftTime();
    radius    = m_calibHit->driftRadius();  // copy new values
    errRadius=radius; // Use same value      
    if ( myStrategy->creationParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition)){
      const Amg::Vector2D* myLocalPosition = DC.detectorElement()->surface(DC.identify()).Trk::Surface::globalToLocal(gpos);
	  if (myLocalPosition) {
        errRadius = (*myLocalPosition)[Trk::driftRadius];
         delete myLocalPosition;
  } else {
        ATH_MSG_WARNING("ErrorAtPredictedPosition failed because local position transformation didn't succeed. Using measured radius instead.");
        errRadius=radius; 
      }
    }
  }else{
    // use PRD values
    radius = DC.localPosition().x();
    errRadius=radius;
    
    // check consistency of error matrix
    if( DC.localCovariance().cols() > 1) {
      msg(MSG::WARNING) << "Error matrix of DC doesn't have dimension 1 " << DC.localCovariance() << endmsg;
      msg(MSG::WARNING) << "Reducing size to 1 dim" << endmsg;
    }
  } 
  
  // Handle the errors here.  Begin by getting the first part of the resolution term
  if (!m_doMdt){
    sigmaR = sqrt(DC.localCovariance()(0,0));
  } else if( myStrategy->creationParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors) ){
    if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ){
      sigmaR   = parametrisedSigma(errRadius);
      ATH_MSG_DEBUG("R= "<<radius<<"\tMoore sigmaR="<<sigmaR);
    } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muonboy ){
      sigmaR   = mBoyParametrisedSigma(errRadius);
      ATH_MSG_DEBUG("R= "<<radius<<"\tMboy sigmaR="<<mBoyParametrisedSigma(errRadius)<<".\tMoore param sigmaR = "<<parametrisedSigma(errRadius));
    } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muon ){
      sigmaR   = parametrisedSigma(errRadius);
      ATH_MSG_DEBUG("R= "<<radius<<"\tMuon sigmaR="<<sigmaR);
    }
  }else{
    // Use calib service errors.
    sigmaR   =sqrt(m_calibHit->sigma2DriftRadius());
  }
  ATH_MSG_DEBUG("Tube : " << m_idHelper->toString(DC.identify()) << " SigmaR = "<<sigmaR);
  double sigmaR2=0.0;
  // Handle the errors scaling / addition of fixed terms 
  if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ) {
    sigmaR2 = mooreErrorStrategy(myStrategy,sigmaR*sigmaR,DC.identify());
    ATH_MSG_DEBUG("After scaling etc:\t Moore sigmaR2 = "<<sigmaR2);
  } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muonboy ) {
    sigmaR2 = mboyErrorStrategy(myStrategy,sigmaR);
    ATH_MSG_DEBUG("After scaling etc:\t Mboy sigmaR2 = "<<sigmaR2<<". \t Moore sigmaR2 = "<<mooreErrorStrategy(myStrategy,sigmaR*sigmaR,DC.identify()));
  } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muon ) {
    sigmaR2 = muonErrorStrategy(myStrategy,sigmaR*sigmaR,DC.identify());
    ATH_MSG_DEBUG("After scaling etc:\t Muon ErrorStrategy sigmaR2 = "<<sigmaR2);
  }
  //////////////////////////////////////////////////////////////
  // update or copy drift radius and error
  
  // new radius
  Trk::DefinedParameter radiusPar(radius,Trk::driftRadius);
  
  // create new error matrix
  Amg::MatrixX  newLocalCov(1,1);
  newLocalCov(0,0) = sigmaR2;
  
  // return new values
  return CalibrationOutput( Trk::LocalParameters(radiusPar), newLocalCov, driftTime, ok );
}

const Muon::MdtDriftCircleOnTrack* Muon::MdtDriftCircleOnTrackCreator::correct(
                                                                               const Trk::PrepRawData& prd, 
                                                                               const Trk::TrackParameters& tp,
                                                                               const MuonDriftCircleErrorStrategy* strategy ) const
{
  const MdtPrepData* mdtPrd = dynamic_cast<const MdtPrepData*>(&prd);
  if( !mdtPrd ){
    msg(MSG::WARNING) << " Incorrect hit type:  Trk::PrepRawData not a Muon::MdtPrepData!! No rot created " << endmsg;
    return 0;
  }
  
  return createRIO_OnTrack(*mdtPrd,tp.position(),&tp.momentum(),strategy);
}


const Muon::MdtDriftCircleOnTrack* Muon::MdtDriftCircleOnTrackCreator::correct(
                                                                               const Trk::PrepRawData& prd,
                                                                               const Trk::TrackParameters& tp ) const
{
  const MdtPrepData* mdtPrd = dynamic_cast<const MdtPrepData*>(&prd);
  if( !mdtPrd ){
    msg(MSG::WARNING) << " Incorrect hit type:  Trk::PrepRawData not a Muon::MdtPrepData!! No rot created " << endmsg;
    return 0;
  }
  
  static bool firstTime = true;
  if (firstTime){
   ATH_MSG_DEBUG( "Called correct using the base class implementation.  Will use the default error strategy" << endmsg);
    firstTime = false;
  }
  
  return createRIO_OnTrack(*mdtPrd,tp.position(),&tp.momentum());
}


double Muon::MdtDriftCircleOnTrackCreator::getErrorFromRt(const Muon::MdtDriftCircleOnTrack& DCT) const {
  double t = DCT.driftTime();
  const MuonGM::MdtReadoutElement* detEl = DCT.detectorElement();

  MuonCalib::MdtFullCalibData data = m_mdtCalibDbSvc->getCalibration( detEl->collectionHash(), detEl->detectorElementHash() ); 
  const MuonCalib::MdtRtRelation* rtRelation = data.rtRelation;
  if( !rtRelation ){
    ATH_MSG_WARNING("no calibration found for tube " << m_idHelper->toString(DCT.identify()));
    return 0; //FIXME!!!
  }
  
  // check whether drift time is within range, if not fix them to the min/max range
  if( rtRelation->rt() ){
    if( t < rtRelation->rt()->tLower() )      t = rtRelation->rt()->tLower();
    else if( t > rtRelation->rt()->tUpper() ) t = rtRelation->rt()->tUpper();
  }else{
    ATH_MSG_WARNING("no rt found for tube " << m_idHelper->toString(DCT.identify()));
    return 0;//FIXME!!!
  }
  
  if( !rtRelation->rtRes() ){
    ATH_MSG_WARNING("no rtRes found for tube " << m_idHelper->toString(DCT.identify()) );
    return 0;//FIXME!!!
  }
  
  return rtRelation->rtRes()->resolution( t );
}

const Muon::MdtDriftCircleOnTrack* Muon::MdtDriftCircleOnTrackCreator::updateError( 
                                                                                   const Muon::MdtDriftCircleOnTrack& DCT,
                                                                                   const Trk::TrackParameters* /**pars*/,
                                                                                   const MuonDriftCircleErrorStrategy* strategy ) const 
{
  const MuonDriftCircleErrorStrategy* myStrategy = (0==strategy)?&m_errorStrategy:strategy;
  
  // calculate error
  double sigmaR(1.);
  double t = DCT.driftTime();
  
  const MuonGM::MdtReadoutElement* detEl = DCT.detectorElement();
  if( !detEl ){
    ATH_MSG_WARNING("MdtDriftCircleOnTrack without MdtReadoutElement " << m_idHelper->toString(DCT.identify()));
    return 0;
  }
  
  double radius=  DCT.driftRadius();

  if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) && 
     !myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) &&
     !myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError )){
    sigmaR = detEl->innerTubeRadius()/sqrt(3.0); // Tube hit
  } else {  
    if( myStrategy->creationParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition) )
      ATH_MSG_WARNING("updateError: ErrorAtPredictedPosition is not yet supported!");
    // get error 
    if( myStrategy->creationParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors) ){
      if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ){
        
        sigmaR   = parametrisedSigma(radius);
      } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muonboy ){
        sigmaR   = mBoyParametrisedSigma(radius);
        ATH_MSG_DEBUG("Mboy drift errors:"<<sigmaR<<" for radius="<<radius);
      } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muon ){
        sigmaR   = parametrisedSigma(radius);
        ATH_MSG_DEBUG("Muon drift errors:"<<sigmaR<<" for radius="<<radius);
      }
    }else{
      sigmaR = getErrorFromRt(DCT);
      if( sigmaR < 0.0001 || sigmaR*sigmaR < 0.0001 ){
        ATH_MSG_WARNING( "Bad obtained from calibration service: error " << m_idHelper->toString(DCT.identify()) << " reso " << sigmaR << " sigma2 " << sigmaR*sigmaR
                        << " drift time " << t << " original " << DCT.driftTime() );
        return 0;
      } 
    }
    
  } // end of tube hit check
  
  double sigmaR2=0.0;
  // Handle the errors scaling / addition of fixed terms 
  if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ) {
    sigmaR2 = mooreErrorStrategy(myStrategy,sigmaR*sigmaR,DCT.identify());
  } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muonboy ) {
    sigmaR2 = mboyErrorStrategy(myStrategy,sigmaR);
  } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muon ) {
    sigmaR2 = muonErrorStrategy(myStrategy,sigmaR*sigmaR,DCT.identify());
    ATH_MSG_DEBUG("After scaling etc:\t Muon ErrorStrategy sigmaR2 = "<<sigmaR2);
  }
  Muon::MdtDriftCircleOnTrack* rot = DCT.clone();
 rot->m_localCovariance(0,0) = sigmaR2; 
 rot->setErrorStrategy(myStrategy);
  
  
  ATH_MSG_VERBOSE("updated error for " << m_idHelper->toString(DCT.identify()) << " new error " << Amg::error(rot->localCovariance(),Trk::locR)
                  << " old error " << Amg::error(DCT.localCovariance(),Trk::locR) );
  
  // return result
  return rot; 
}

const Muon::MdtDriftCircleOnTrack* Muon::MdtDriftCircleOnTrackCreator::updateErrorExternal( const MdtDriftCircleOnTrack& DCT,
                                                                                            const Trk::TrackParameters* /**pars*/ ,
                                                                                            const std::map<Identifier,double>* errorlist ) const
{
  // calculate error
  double sigmaRT(1.), sigmaEXT(1.);
  double t = DCT.driftTime();

  const MuonGM::MdtReadoutElement* detEl = DCT.detectorElement();
  if( !detEl ){
    ATH_MSG_WARNING("MdtDriftCircleOnTrack without MdtReadoutElement!");
    return 0;
  }
  Identifier detElId = detEl->identify();

  sigmaRT = getErrorFromRt(DCT);

  if( sigmaRT < 0.0001 || sigmaRT*sigmaRT < 0.0001 ){
    ATH_MSG_WARNING( "Bad obtained from calibration service: error " << m_idHelper->toString(detElId) << " reso " << sigmaRT << " sigma2 " << sigmaRT*sigmaRT
                    << " drift time " << t << " original " << DCT.driftTime() << " => Only external error is applied (if any)!" );
    sigmaRT=0.;
    if(!errorlist) {
      ATH_MSG_WARNING("List of external errors also empty: Returning NULL pointer.");
      return 0;
    }
  }

  if(!errorlist) {
    ATH_MSG_WARNING("List of external errors empty: Applying only error from RT.");
    sigmaEXT = 0.;
  } else {
    if( errorlist->find(DCT.identify()) != errorlist->end() )
      sigmaEXT = errorlist->find(DCT.identify())->second;
    else {
      ATH_MSG_DEBUG("There is no external error stored in the map for " << m_idHelper->toString(detElId) << ". Applying only error from RT.");
      sigmaEXT = 0.;
    }
  }

  Muon::MdtDriftCircleOnTrack* rot = DCT.clone();
  rot->m_localCovariance(0,0) = sigmaRT*sigmaRT + sigmaEXT*sigmaEXT;

  ATH_MSG_DEBUG("updated error for " << m_idHelper->toString(DCT.identify()) << " new error " << Amg::error(rot->localCovariance(),Trk::locR)
                  << " old error " << Amg::error(DCT.localCovariance(),Trk::locR) << " with RT error " << sigmaRT << " and external error " << sigmaEXT );

  return rot;
}

Muon::MdtDriftCircleStatus Muon::MdtDriftCircleOnTrackCreator::driftCircleStatus( const Muon::MdtDriftCircleOnTrack& DCT ) const 
{
  const MuonGM::MdtReadoutElement* detEl = DCT.detectorElement();
  if( !detEl ){
    ATH_MSG_WARNING("MdtDriftCircleOnTrack without MdtReadoutElement " << m_idHelper->toString(DCT.identify()));
    return Muon::MdtStatusUnDefined;
  }
  
  // access rt relation
  MuonCalib::MdtFullCalibData data = m_mdtCalibDbSvc->getCalibration( detEl->collectionHash(), detEl->detectorElementHash() ); 
  const MuonCalib::MdtRtRelation* rtRelation = data.rtRelation;
  if( !rtRelation ){
    ATH_MSG_WARNING("no calibration found for tube " << m_idHelper->toString(DCT.identify()));
    return Muon::MdtStatusUnDefined;
  }
  
  // check whether drift time is within range, if not fix them to the min/max range
  double t = DCT.driftTime();
  return m_mdtCalibSvc->driftTimeStatus(t, rtRelation, *m_mdtCalibSvcSettings); 
}

double Muon::MdtDriftCircleOnTrackCreator::parametrisedSigma( double r ) const {
  return 0.23*exp(-fabs(r)/6.06)+0.0362;
}

void Muon::MdtDriftCircleOnTrackCreator::fillMboyParametrisedErrors(){
  using namespace boost::assign;
  m_mboyParametrisedErrors+=0.66685e-01,0.66685e-01,0.66685e-01,0.62843e-01,0.59443e-01,0.56402e-01,0.53662e-01,0.51176e-01,0.48908e-01,0.46829e-01, 
  0.44916e-01,0.43147e-01,0.41508e-01,0.39983e-01,0.38560e-01,0.37229e-01,0.35982e-01,0.34810e-01,0.33707e-01,0.32666e-01, 
  0.31683e-01,0.30752e-01,0.29870e-01,0.29032e-01,0.28236e-01,0.27477e-01,0.26755e-01,0.26065e-01,0.25406e-01,0.24776e-01, 
  0.24173e-01,0.23595e-01,0.23040e-01,0.22508e-01,0.21997e-01,0.21505e-01,0.21032e-01,0.20576e-01,0.20137e-01,0.19713e-01, 
  0.19305e-01,0.18910e-01,0.18529e-01,0.18160e-01,0.17803e-01,0.17458e-01,0.17124e-01,0.16800e-01,0.16486e-01,0.16182e-01, 
  0.15887e-01,0.15600e-01,0.15322e-01,0.15051e-01,0.14788e-01,0.14533e-01,0.14284e-01,0.14042e-01,0.13807e-01,0.13578e-01, 
  0.13354e-01,0.13137e-01,0.12925e-01,0.12718e-01,0.12516e-01,0.12320e-01,0.12128e-01,0.11940e-01,0.11757e-01,0.11579e-01, 
  0.11404e-01,0.11233e-01,0.11067e-01,0.10904e-01,0.10744e-01,0.10588e-01,0.10436e-01,0.10286e-01,0.10140e-01,0.99973e-02, 
  0.98572e-02,0.97200e-02,0.95856e-02,0.94540e-02,0.93249e-02,0.91984e-02,0.90744e-02,0.89528e-02,0.88336e-02,0.87166e-02, 
  0.86019e-02,0.84893e-02,0.83788e-02,0.82704e-02,0.81639e-02,0.80594e-02,0.79567e-02,0.78559e-02,0.77569e-02,0.76597e-02, 
  0.75641e-02,0.74702e-02,0.73779e-02,0.72871e-02,0.71979e-02,0.71102e-02,0.70240e-02,0.69392e-02,0.68558e-02,0.67737e-02, 
  0.66930e-02,0.66136e-02,0.65354e-02,0.64585e-02,0.63828e-02,0.63083e-02,0.62349e-02,0.61627e-02,0.60916e-02,0.60216e-02, 
  0.59526e-02,0.58846e-02,0.58177e-02,0.57518e-02,0.56868e-02,0.56228e-02,0.55597e-02,0.54976e-02,0.54363e-02,0.53759e-02, 
  0.53164e-02,0.52577e-02,0.51999e-02,0.51428e-02,0.50865e-02,0.50311e-02,0.49763e-02,0.49224e-02,0.48691e-02,0.48166e-02, 
  0.47648e-02,0.47136e-02,0.46632e-02,0.46134e-02,0.45643e-02,0.45158e-02,0.44680e-02,0.44207e-02,0.43741e-02,0.43281e-02, 
  0.42826e-02,0.42378e-02,0.41935e-02,0.41497e-02,0.41065e-02,0.40639e-02,0.40218e-02,0.39801e-02,0.39390e-02,0.38984e-02, 
  0.38583e-02,0.38187e-02,0.37796e-02,0.37409e-02,0.37027e-02,0.36649e-02,0.36276e-02,0.35907e-02,0.35543e-02,0.35183e-02, 
  0.34827e-02,0.34475e-02,0.34127e-02,0.33783e-02,0.33443e-02,0.33107e-02,0.32775e-02,0.32446e-02,0.32121e-02,0.31800e-02, 
  0.31482e-02,0.31168e-02,0.30858e-02,0.30550e-02,0.30247e-02,0.29946e-02,0.29649e-02,0.29355e-02,0.29064e-02,0.28776e-02, 
  0.28491e-02,0.28210e-02,0.27931e-02,0.27655e-02,0.27383e-02,0.27113e-02,0.26846e-02,0.26581e-02,0.26320e-02,0.26061e-02, 
  0.25805e-02,0.25551e-02,0.25300e-02,0.25052e-02,0.24806e-02,0.24563e-02,0.24322e-02,0.24084e-02,0.23848e-02,0.23614e-02, 
  0.23383e-02,0.23154e-02,0.22927e-02,0.22702e-02,0.22480e-02,0.22260e-02,0.22042e-02,0.21826e-02,0.21613e-02,0.21401e-02, 
  0.21192e-02,0.20984e-02,0.20778e-02,0.20575e-02,0.20373e-02,0.20174e-02,0.19976e-02,0.19780e-02,0.19586e-02,0.19394e-02, 
  0.19203e-02,0.19015e-02,0.18828e-02,0.18643e-02,0.18459e-02,0.18278e-02,0.18098e-02,0.17919e-02,0.17743e-02,0.17568e-02, 
  0.17394e-02,0.17222e-02,0.17052e-02,0.16883e-02,0.16716e-02,0.16550e-02,0.16386e-02,0.16223e-02,0.16062e-02,0.15902e-02, 
  0.15744e-02,0.15587e-02,0.15432e-02,0.15277e-02,0.15125e-02,0.14973e-02,0.14823e-02,0.14674e-02,0.14527e-02,0.14380e-02, 
  0.14236e-02,0.14092e-02,0.13950e-02,0.13808e-02,0.13668e-02,0.13530e-02,0.13392e-02,0.13256e-02,0.13121e-02,0.12987e-02, 
  0.12854e-02,0.12722e-02,0.12592e-02,0.12462e-02,0.12334e-02,0.12206e-02,0.12080e-02,0.11955e-02,0.11831e-02,0.11708e-02, 
  0.11586e-02,0.11465e-02,0.11345e-02,0.11226e-02,0.11108e-02,0.10991e-02,0.10875e-02,0.10760e-02,0.10646e-02,0.10533e-02, 
  0.10421e-02,0.10309e-02,0.10199e-02,0.10090e-02,0.99810e-03,0.98733e-03,0.97666e-03,0.96606e-03,0.95556e-03,0.94514e-03, 
  0.93481e-03,0.92456e-03,0.91439e-03,0.90431e-03,0.89431e-03,0.88439e-03,0.87454e-03,0.86478e-03,0.85510e-03,0.84549e-03, 
  0.83597e-03,0.82651e-03,0.81714e-03,0.80784e-03,0.79861e-03,0.78946e-03,0.78038e-03,0.77137e-03,0.76243e-03,0.75357e-03, 
  0.74477e-03,0.73605e-03,0.72739e-03,0.71880e-03,0.71028e-03,0.70183e-03,0.69345e-03,0.68513e-03,0.67687e-03,0.66868e-03, 
  0.66056e-03,0.65249e-03,0.64450e-03,0.63656e-03,0.62869e-03,0.62087e-03,0.61312e-03,0.60543e-03,0.59780e-03,0.59023e-03, 
  0.58272e-03,0.57526e-03,0.56787e-03,0.56053e-03,0.55325e-03,0.54602e-03,0.53885e-03,0.53174e-03,0.52468e-03,0.51767e-03, 
  0.51072e-03,0.50383e-03,0.49698e-03,0.49019e-03,0.48345e-03,0.47677e-03,0.47013e-03,0.46355e-03,0.45702e-03,0.45053e-03, 
  0.44410e-03,0.43771e-03,0.43138e-03,0.42509e-03,0.41885e-03,0.41266e-03,0.40652e-03,0.40042e-03,0.39437e-03,0.38837e-03, 
  0.38241e-03,0.37650e-03,0.37063e-03,0.36481e-03,0.35903e-03,0.35330e-03,0.34761e-03,0.34196e-03,0.33636e-03,0.33080e-03, 
  0.32528e-03,0.31981e-03,0.31437e-03,0.30898e-03,0.30363e-03,0.29832e-03,0.29305e-03,0.28782e-03,0.28262e-03,0.27747e-03, 
  0.27236e-03,0.26729e-03,0.26225e-03,0.25726e-03,0.25230e-03,0.24738e-03,0.24250e-03,0.23765e-03,0.23284e-03,0.22807e-03, 
  0.22334e-03,0.21864e-03,0.21397e-03,0.20935e-03,0.20475e-03,0.20019e-03,0.19567e-03,0.19118e-03,0.18673e-03,0.18231e-03, 
  0.17792e-03,0.17357e-03,0.16925e-03,0.16496e-03,0.16071e-03,0.15649e-03,0.15230e-03,0.14814e-03,0.14402e-03,0.13992e-03, 
  0.13586e-03,0.13183e-03,0.12783e-03,0.12386e-03,0.11992e-03,0.11602e-03,0.11214e-03,0.10829e-03,0.10447e-03,0.10068e-03, 
  0.96922e-04,0.93191e-04,0.89489e-04,0.85815e-04,0.82170e-04,0.78553e-04,0.74965e-04,0.71404e-04,0.67870e-04,0.64365e-04, 
  0.60886e-04,0.57435e-04,0.54010e-04,0.50612e-04,0.47241e-04,0.43896e-04,0.40577e-04,0.37284e-04,0.34017e-04,0.30776e-04, 
  0.27560e-04,0.24370e-04,0.21204e-04,0.18064e-04,0.14948e-04,0.11857e-04,0.87902e-05,0.57479e-05,0.27297e-05,-.26457e-06, 
  -.32350e-05,-.61818e-05,-.91051e-05,-.12005e-04,-.14882e-04,-.17735e-04,-.20566e-04,-.23374e-04,-.26160e-04,-.28923e-04, 
  -.31664e-04,-.34382e-04,-.37079e-04,-.39754e-04,-.42407e-04,-.45038e-04,-.47649e-04,-.50237e-04,-.52805e-04,-.55351e-04, 
  -.57877e-04,-.60382e-04,-.62866e-04,-.65330e-04,-.67774e-04,-.70197e-04,-.72600e-04,-.74983e-04,-.77346e-04,-.79690e-04, 
  -.82014e-04,-.84318e-04,-.86603e-04,-.88869e-04,-.91116e-04,-.93344e-04,-.95552e-04,-.97743e-04,-.99914e-04,-.10207e-03, 
  -.10420e-03,-.10632e-03,-.10842e-03,-.11050e-03,-.11256e-03,-.11460e-03,-.11663e-03,-.11864e-03,-.12063e-03,-.12260e-03, 
  -.12456e-03,-.12650e-03,-.12842e-03,-.13033e-03,-.13221e-03,-.13409e-03,-.13594e-03,-.13778e-03,-.13960e-03,-.14141e-03, 
  -.14319e-03,-.14497e-03,-.14672e-03,-.14847e-03,-.15019e-03,-.15190e-03,-.15359e-03,-.15527e-03,-.15693e-03,-.15858e-03, 
  -.16021e-03,-.16183e-03,-.16343e-03,-.16501e-03,-.16658e-03,-.16814e-03,-.16968e-03,-.17121e-03,-.17272e-03,-.17422e-03, 
  -.17570e-03,-.17717e-03,-.17862e-03,-.18006e-03,-.18149e-03,-.18290e-03,-.18430e-03,-.18568e-03,-.18705e-03,-.18841e-03, 
  -.18975e-03,-.19108e-03,-.19239e-03,-.19370e-03,-.19499e-03,-.19626e-03,-.19752e-03,-.19877e-03,-.20001e-03,-.20123e-03, 
  -.20244e-03,-.20364e-03,-.20482e-03,-.20600e-03,-.20715e-03,-.20830e-03,-.20944e-03,-.21056e-03,-.21167e-03,-.21276e-03, 
  -.21385e-03,-.21492e-03,-.21598e-03,-.21703e-03,-.21807e-03,-.21910e-03,-.22011e-03,-.22111e-03,-.22210e-03,-.22308e-03, 
  -.22405e-03,-.22500e-03,-.22594e-03,-.22688e-03,-.22780e-03,-.22871e-03,-.22961e-03,-.23050e-03,-.23137e-03,-.23224e-03, 
  -.23309e-03,-.23394e-03,-.23477e-03,-.23559e-03,-.23641e-03,-.23721e-03,-.23800e-03,-.23878e-03,-.23955e-03,-.24031e-03, 
  -.24106e-03,-.24179e-03,-.24252e-03,-.24324e-03,-.24395e-03,-.24465e-03,-.24533e-03,-.24601e-03,-.24668e-03,-.24734e-03, 
  -.24799e-03,-.24863e-03,-.24926e-03,-.24987e-03,-.25048e-03,-.25108e-03,-.25168e-03,-.25226e-03,-.25283e-03,-.25339e-03, 
  -.25394e-03,-.25449e-03,-.25502e-03,-.25555e-03,-.25606e-03,-.25657e-03,-.25707e-03,-.25756e-03,-.25804e-03,-.25851e-03, 
  -.25897e-03,-.25943e-03,-.25987e-03,-.26031e-03,-.26074e-03,-.26116e-03,-.26157e-03,-.26197e-03,-.26236e-03,-.26275e-03, 
  -.26313e-03,-.26349e-03,-.26386e-03,-.26421e-03,-.26455e-03,-.26489e-03,-.26521e-03,-.26553e-03,-.26584e-03,-.26615e-03, 
  -.26644e-03,-.26673e-03,-.26701e-03,-.26728e-03,-.26755e-03,-.26780e-03,-.26805e-03,-.26829e-03,-.26852e-03,-.26875e-03, 
  -.26897e-03,-.26918e-03,-.26938e-03,-.26958e-03,-.26976e-03,-.26994e-03,-.27012e-03,-.27028e-03,-.27044e-03,-.27059e-03, 
  -.27074e-03,-.27087e-03,-.27100e-03,-.27113e-03,-.27124e-03,-.27135e-03,-.27145e-03,-.27155e-03,-.27163e-03,-.27171e-03, 
  -.27179e-03,-.27185e-03,-.27191e-03,-.27197e-03,-.27201e-03,-.27205e-03,-.27209e-03,-.27211e-03,-.27213e-03,-.27215e-03, 
  -.27215e-03,-.27216e-03,-.27215e-03,-.27214e-03,-.27212e-03,-.27209e-03,-.27206e-03,-.27202e-03,-.27198e-03,-.27193e-03, 
  -.27187e-03,-.27181e-03,-.27174e-03,-.27166e-03,-.27158e-03,-.27150e-03,-.27140e-03,-.27130e-03,-.27120e-03,-.27109e-03, 
  -.27097e-03,-.27084e-03,-.27072e-03,-.27058e-03,-.27044e-03,-.27029e-03,-.27014e-03,-.26998e-03,-.26982e-03,-.26965e-03, 
  -.26947e-03,-.26929e-03,-.26911e-03,-.26891e-03,-.26872e-03,-.26851e-03,-.26830e-03,-.26809e-03,-.26787e-03,-.26764e-03, 
  -.26741e-03,-.26718e-03,-.26694e-03,-.26669e-03,-.26644e-03,-.26618e-03,-.26592e-03,-.26565e-03,-.26538e-03,-.26510e-03, 
  -.26482e-03,-.26453e-03,-.26423e-03,-.26394e-03,-.26363e-03,-.26332e-03,-.26301e-03,-.26269e-03,-.26237e-03,-.26204e-03, 
  -.26170e-03,-.26137e-03,-.26102e-03,-.26067e-03,-.26032e-03,-.25996e-03,-.25960e-03,-.25923e-03,-.25886e-03,-.25848e-03, 
  -.25810e-03,-.25772e-03,-.25732e-03,-.25693e-03,-.25653e-03,-.25612e-03,-.25571e-03,-.25530e-03,-.25488e-03,-.25446e-03, 
  -.25403e-03,-.25360e-03,-.25316e-03,-.25272e-03,-.25228e-03,-.25183e-03,-.25137e-03,-.25091e-03,-.25045e-03,-.24998e-03, 
  -.24951e-03,-.24904e-03,-.24855e-03,-.24807e-03,-.24758e-03,-.24709e-03,-.24659e-03,-.24609e-03,-.24559e-03,-.24508e-03, 
  -.24456e-03,-.24404e-03,-.24352e-03,-.24300e-03,-.24247e-03,-.24193e-03,-.24140e-03,-.24085e-03,-.24031e-03,-.23976e-03, 
  -.23920e-03,-.23865e-03,-.23809e-03,-.23752e-03,-.23695e-03,-.23638e-03,-.23580e-03,-.23522e-03,-.23463e-03,-.23405e-03, 
  -.23345e-03,-.23286e-03,-.23226e-03,-.23165e-03,-.23105e-03,-.23044e-03,-.22982e-03,-.22920e-03,-.22858e-03,-.22796e-03, 
  -.22733e-03,-.22670e-03,-.22606e-03,-.22542e-03,-.22478e-03,-.22413e-03,-.22348e-03,-.22282e-03,-.22217e-03,-.22151e-03, 
  -.22084e-03,-.22017e-03,-.21950e-03,-.21883e-03,-.21815e-03,-.21747e-03,-.21678e-03,-.21610e-03,-.21541e-03,-.21471e-03, 
  -.21401e-03,-.21331e-03,-.21261e-03,-.21190e-03,-.21119e-03,-.21047e-03,-.20976e-03,-.20903e-03,-.20831e-03,-.20758e-03, 
  -.20685e-03,-.20612e-03,-.20538e-03,-.20464e-03,-.20390e-03,-.20315e-03,-.20240e-03,-.20165e-03,-.20090e-03,-.20014e-03, 
  -.19938e-03,-.19861e-03,-.19785e-03,-.19708e-03,-.19630e-03,-.19553e-03,-.19475e-03,-.19396e-03,-.19318e-03,-.19239e-03, 
  -.19160e-03,-.19080e-03,-.19001e-03,-.18921e-03,-.18841e-03,-.18760e-03,-.18679e-03,-.18598e-03,-.18517e-03,-.18435e-03, 
  -.18353e-03,-.18271e-03,-.18188e-03,-.18105e-03,-.18022e-03,-.17939e-03,-.17855e-03,-.17771e-03,-.17687e-03,-.17603e-03, 
  -.17518e-03,-.17433e-03,-.17348e-03,-.17263e-03,-.17177e-03,-.17091e-03,-.17004e-03,-.16918e-03,-.16831e-03,-.16744e-03, 
  -.16657e-03,-.16569e-03,-.16481e-03,-.16393e-03,-.16305e-03,-.16216e-03,-.16127e-03,-.16038e-03,-.15949e-03,-.15859e-03, 
  -.15770e-03,-.15680e-03,-.15589e-03,-.15499e-03,-.15408e-03,-.15317e-03,-.15225e-03,-.15134e-03,-.15042e-03,-.14950e-03, 
  -.14858e-03,-.14765e-03,-.14673e-03,-.14580e-03,-.14487e-03,-.14393e-03,-.14299e-03,-.14206e-03,-.14111e-03,-.14017e-03, 
  -.13923e-03,-.13828e-03,-.13733e-03,-.13637e-03,-.13542e-03,-.13446e-03,-.13350e-03,-.13254e-03,-.13158e-03,-.13061e-03, 
  -.12965e-03,-.12867e-03,-.12770e-03,-.12673e-03,-.12575e-03,-.12477e-03,-.12379e-03,-.12281e-03,-.12182e-03,-.12084e-03, 
  -.11985e-03,-.11886e-03,-.11786e-03,-.11687e-03,-.11587e-03,-.11487e-03,-.11387e-03,-.11286e-03,-.11186e-03,-.11085e-03, 
  -.10984e-03,-.10883e-03,-.10781e-03,-.10680e-03,-.10578e-03,-.10476e-03,-.10374e-03,-.10272e-03,-.10169e-03,-.10066e-03, 
  -.99632e-04,-.98601e-04,-.97567e-04,-.96532e-04,-.95495e-04,-.94456e-04,-.93414e-04,-.92372e-04,-.91327e-04,-.90280e-04, 
  -.89231e-04,-.88181e-04,-.87129e-04,-.86075e-04,-.85019e-04,-.83961e-04,-.82902e-04,-.81840e-04,-.80777e-04,-.79712e-04, 
  -.78645e-04,-.77577e-04,-.76506e-04,-.75434e-04,-.74360e-04,-.73285e-04,-.72208e-04,-.71128e-04,-.70048e-04,-.68965e-04, 
  -.67881e-04,-.66795e-04,-.65707e-04,-.64618e-04,-.63527e-04,-.62434e-04,-.61340e-04,-.60243e-04,-.59146e-04,-.58046e-04, 
  -.56945e-04,-.55842e-04,-.54738e-04,-.53632e-04,-.52524e-04,-.51415e-04,-.50304e-04,-.49192e-04,-.48078e-04,-.46962e-04, 
  -.45845e-04,-.44726e-04,-.43606e-04,-.42484e-04,-.41360e-04,-.40235e-04,-.39108e-04,-.37980e-04,-.36850e-04,-.35719e-04, 
  -.34586e-04,-.33452e-04,-.32316e-04,-.31179e-04,-.30040e-04,-.28900e-04,-.27758e-04,-.26614e-04,-.25470e-04,-.24323e-04, 
  -.23176e-04,-.22026e-04,-.20876e-04,-.19724e-04,-.18570e-04,-.17415e-04,-.16259e-04,-.15101e-04,-.13942e-04,-.12781e-04, 
  -.11619e-04,-.10455e-04,-.92902e-05,-.81239e-05,-.69562e-05,-.57871e-05,-.46166e-05,-.34448e-05,-.22716e-05,-.10970e-05, 
  0.78959e-07,0.12563e-05,0.24349e-05,0.36149e-05,0.47962e-05,0.59788e-05,0.71628e-05,0.83481e-05,0.95347e-05,0.10723e-04, 
  0.11912e-04,0.13102e-04,0.14294e-04,0.15487e-04,0.16682e-04,0.17877e-04,0.19074e-04,0.20273e-04,0.21472e-04,0.22673e-04, 
  0.23875e-04,0.25078e-04,0.26283e-04,0.27488e-04,0.28695e-04,0.29904e-04,0.31113e-04,0.32324e-04,0.33536e-04,0.34749e-04, 
  0.35963e-04,0.37179e-04,0.38395e-04,0.39613e-04,0.40832e-04,0.42053e-04,0.43274e-04,0.44497e-04,0.45721e-04,0.46945e-04, 
  0.48172e-04,0.49399e-04,0.50627e-04,0.51857e-04,0.53088e-04,0.54320e-04,0.55553e-04,0.56787e-04,0.58022e-04,0.59258e-04, 
  0.60496e-04,0.61735e-04,0.62974e-04,0.64215e-04,0.65457e-04,0.66700e-04,0.67944e-04,0.69189e-04,0.70436e-04,0.71683e-04, 
  0.72932e-04,0.74181e-04,0.75432e-04,0.76683e-04,0.77936e-04,0.79190e-04,0.80445e-04,0.81701e-04,0.82957e-04,0.84215e-04, 
  0.85474e-04,0.86734e-04,0.87995e-04,0.89257e-04,0.90521e-04,0.91785e-04,0.93050e-04,0.94316e-04,0.95583e-04,0.96851e-04, 
  0.98120e-04,0.99390e-04,0.10066e-03,0.10193e-03,0.10321e-03,0.10448e-03,0.10576e-03,0.10703e-03,0.10831e-03,0.10959e-03, 
  0.11087e-03,0.11214e-03,0.11343e-03,0.11471e-03,0.11599e-03,0.11727e-03,0.11856e-03,0.11984e-03,0.12113e-03,0.12242e-03, 
  0.12371e-03,0.12499e-03,0.12628e-03,0.12758e-03,0.12887e-03,0.13016e-03,0.13145e-03,0.13275e-03,0.13404e-03,0.13534e-03, 
  0.13664e-03,0.13794e-03,0.13923e-03,0.14053e-03,0.14183e-03,0.14314e-03,0.14444e-03,0.14574e-03,0.14705e-03,0.14835e-03, 
  0.14966e-03,0.15096e-03,0.15227e-03,0.15358e-03,0.15489e-03,0.15620e-03,0.15751e-03,0.15882e-03,0.16013e-03,0.16145e-03, 
  0.16276e-03,0.16408e-03,0.16539e-03,0.16671e-03,0.16803e-03,0.16935e-03,0.17066e-03,0.17198e-03,0.17331e-03,0.17463e-03, 
  0.17595e-03,0.17727e-03,0.17860e-03,0.17992e-03,0.18125e-03,0.18257e-03,0.18390e-03,0.18523e-03,0.18655e-03,0.18788e-03, 
  0.18921e-03,0.19054e-03,0.19188e-03,0.19321e-03,0.19454e-03,0.19587e-03,0.19721e-03,0.19854e-03,0.19988e-03,0.20122e-03, 
  0.20255e-03,0.20389e-03,0.20523e-03,0.20657e-03,0.20791e-03,0.20925e-03,0.21059e-03,0.21194e-03,0.21328e-03,0.21462e-03, 
  0.21597e-03,0.21731e-03,0.21866e-03,0.22000e-03,0.22135e-03,0.22270e-03,0.22405e-03,0.22540e-03,0.22675e-03,0.22810e-03, 
  0.22945e-03,0.23080e-03,0.23215e-03,0.23351e-03,0.23486e-03,0.23622e-03,0.23757e-03,0.23893e-03,0.24028e-03,0.24164e-03, 
  0.24300e-03,0.24436e-03,0.24572e-03,0.24708e-03,0.24844e-03,0.24980e-03,0.25116e-03,0.25252e-03,0.25389e-03,0.25525e-03, 
  0.25661e-03,0.25798e-03,0.25934e-03,0.26071e-03,0.26208e-03,0.26344e-03,0.26481e-03,0.26618e-03,0.26755e-03,0.26892e-03, 
  0.27029e-03,0.27166e-03,0.27303e-03,0.27440e-03,0.27578e-03,0.27715e-03,0.27852e-03,0.27990e-03,0.28127e-03,0.28265e-03, 
  0.28403e-03,0.28540e-03,0.28678e-03,0.28816e-03,0.28954e-03,0.29091e-03,0.29229e-03,0.29367e-03,0.29505e-03,0.29644e-03, 
  0.29782e-03,0.29920e-03,0.30058e-03,0.30197e-03,0.30335e-03,0.30473e-03,0.30612e-03,0.30751e-03,0.30889e-03,0.31028e-03, 
  0.31167e-03,0.31305e-03,0.31444e-03,0.31583e-03,0.31722e-03,0.31861e-03,0.32000e-03,0.32139e-03,0.32278e-03,0.32417e-03, 
  0.32557e-03,0.32696e-03,0.32835e-03,0.32975e-03,0.33114e-03,0.33253e-03,0.33393e-03,0.33533e-03,0.33672e-03,0.33812e-03, 
  0.33952e-03,0.34091e-03,0.34231e-03,0.34371e-03,0.34511e-03,0.34651e-03,0.34791e-03,0.34931e-03,0.35071e-03,0.35211e-03, 
  0.35352e-03,0.35492e-03,0.35632e-03,0.35772e-03,0.35913e-03,0.36053e-03,0.36194e-03,0.36334e-03,0.36475e-03,0.36616e-03, 
  0.36756e-03,0.36897e-03,0.37038e-03,0.37178e-03,0.37319e-03,0.37460e-03,0.37601e-03,0.37742e-03,0.37883e-03,0.38024e-03, 
  0.38165e-03,0.38306e-03,0.38448e-03,0.38589e-03,0.38730e-03,0.38871e-03,0.39013e-03,0.39154e-03,0.39296e-03,0.39437e-03, 
  0.39579e-03,0.39720e-03,0.39862e-03,0.40003e-03,0.40145e-03,0.40287e-03,0.40429e-03,0.40570e-03,0.40712e-03,0.40854e-03, 
  0.40996e-03,0.41138e-03,0.41280e-03,0.41422e-03,0.41564e-03,0.41706e-03,0.41848e-03,0.41991e-03,0.42133e-03,0.42275e-03, 
  0.42417e-03,0.42560e-03,0.42702e-03,0.42845e-03,0.42987e-03,0.43130e-03,0.43272e-03,0.43415e-03,0.43557e-03,0.43700e-03, 
  0.43843e-03,0.43985e-03,0.44128e-03,0.44271e-03,0.44414e-03,0.44557e-03,0.44699e-03,0.44842e-03,0.44985e-03,0.45128e-03, 
  0.45271e-03,0.45415e-03,0.45558e-03,0.45701e-03,0.45844e-03,0.45987e-03,0.46130e-03,0.46274e-03,0.46417e-03,0.46560e-03, 
  0.46704e-03,0.46847e-03,0.46991e-03,0.47134e-03,0.47278e-03,0.47421e-03,0.47565e-03,0.47708e-03,0.47852e-03,0.47996e-03, 
  0.48139e-03,0.48283e-03,0.48427e-03,0.48571e-03,0.48714e-03,0.48858e-03,0.49002e-03,0.49146e-03,0.49290e-03,0.49434e-03, 
  0.49578e-03,0.49722e-03,0.49866e-03,0.50010e-03,0.50154e-03,0.50298e-03,0.50443e-03,0.50587e-03,0.50731e-03,0.50875e-03, 
  0.51020e-03,0.51164e-03,0.51308e-03,0.51453e-03,0.51597e-03,0.51742e-03,0.51886e-03,0.52031e-03,0.52175e-03,0.52320e-03, 
  0.52464e-03,0.52609e-03,0.52754e-03,0.52898e-03,0.53043e-03,0.53188e-03,0.53332e-03,0.53477e-03,0.53622e-03,0.53767e-03, 
  0.53912e-03,0.54057e-03,0.54201e-03,0.54346e-03,0.54491e-03,0.54636e-03,0.54781e-03,0.54926e-03,0.55071e-03,0.55217e-03, 
  0.55362e-03,0.55507e-03,0.55652e-03,0.55797e-03,0.55942e-03,0.56088e-03,0.56233e-03,0.56378e-03,0.56523e-03,0.56669e-03, 
  0.56814e-03,0.56960e-03,0.57105e-03,0.57250e-03,0.57396e-03,0.57541e-03,0.57687e-03,0.57832e-03,0.57978e-03,0.58123e-03, 
  0.58269e-03,0.58415e-03,0.58560e-03,0.58706e-03,0.58852e-03,0.58997e-03,0.59143e-03,0.59289e-03,0.59435e-03,0.59580e-03, 
  0.59726e-03,0.59872e-03,0.60018e-03,0.60164e-03,0.60310e-03,0.60456e-03,0.60602e-03,0.60748e-03,0.60894e-03,0.61040e-03, 
  0.61186e-03,0.61332e-03,0.61478e-03,0.61624e-03,0.61770e-03,0.61916e-03,0.62062e-03,0.62208e-03,0.62354e-03,0.62501e-03, 
  0.62647e-03,0.62793e-03,0.62939e-03,0.63086e-03,0.63232e-03,0.63378e-03,0.63525e-03,0.63671e-03,0.63817e-03,0.63964e-03, 
  0.64110e-03,0.64257e-03,0.64403e-03,0.64550e-03,0.64696e-03,0.64843e-03,0.64989e-03,0.65136e-03,0.65282e-03,0.65429e-03, 
  0.65575e-03,0.65722e-03,0.65869e-03,0.66015e-03,0.66162e-03,0.66309e-03,0.66455e-03,0.66602e-03,0.66749e-03,0.66896e-03, 
  0.67042e-03,0.67189e-03,0.67336e-03,0.67483e-03,0.67630e-03,0.67776e-03,0.67923e-03,0.68070e-03,0.68217e-03,0.68364e-03, 
  0.68511e-03,0.68658e-03,0.68805e-03,0.68952e-03,0.69099e-03,0.69246e-03,0.69393e-03,0.69540e-03,0.69687e-03,0.69834e-03, 
  0.69981e-03,0.70128e-03,0.70275e-03,0.70422e-03,0.70569e-03,0.70717e-03,0.70864e-03,0.71011e-03,0.71158e-03,0.71305e-03, 
  0.71453e-03,0.71600e-03,0.71747e-03,0.71894e-03,0.72042e-03,0.72189e-03,0.72336e-03,0.72483e-03,0.72631e-03,0.72778e-03, 
  0.72925e-03,0.73073e-03,0.73220e-03,0.73368e-03,0.73515e-03,0.73662e-03,0.73810e-03,0.73957e-03,0.74105e-03,0.74252e-03, 
  0.74400e-03,0.74547e-03,0.74695e-03,0.74842e-03,0.74990e-03,0.75137e-03,0.75285e-03,0.75432e-03,0.75580e-03,0.75728e-03, 
  0.75875e-03,0.76023e-03,0.76170e-03,0.76318e-03,0.76466e-03,0.76613e-03,0.76761e-03,0.76909e-03,0.77056e-03,0.77204e-03, 
  0.77352e-03,0.77500e-03,0.77647e-03,0.77795e-03,0.77943e-03,0.78091e-03,0.78238e-03,0.78386e-03,0.78534e-03,0.78682e-03, 
  0.78829e-03,0.78977e-03,0.79125e-03,0.79273e-03,0.79421e-03,0.79569e-03,0.79717e-03,0.79864e-03,0.80012e-03,0.80160e-03, 
  0.80308e-03,0.80456e-03,0.80604e-03,0.80752e-03,0.80900e-03,0.81048e-03,0.81196e-03,0.81344e-03,0.81492e-03,0.81640e-03, 
  0.81788e-03,0.81936e-03,0.82084e-03,0.82232e-03,0.82380e-03,0.82528e-03,0.82676e-03,0.82824e-03,0.82972e-03,0.83120e-03, 
  0.83268e-03,0.83416e-03,0.83564e-03,0.83712e-03,0.83861e-03,0.84009e-03,0.84157e-03,0.84305e-03,0.84453e-03,0.84601e-03, 
  0.84749e-03,0.84898e-03,0.85046e-03,0.85194e-03,0.85342e-03,0.85490e-03,0.85638e-03,0.85787e-03,0.85935e-03,0.86083e-03, 
  0.86231e-03,0.86379e-03,0.86528e-03,0.86676e-03,0.86824e-03,0.86972e-03,0.87121e-03,0.87269e-03,0.87417e-03,0.87566e-03, 
  0.87714e-03,0.87862e-03,0.88010e-03,0.88159e-03,0.88307e-03,0.88455e-03,0.88604e-03,0.88752e-03,0.88900e-03,0.89049e-03;
}

float Muon::MdtDriftCircleOnTrackCreator::mBoyParametrisedSigma(double r) const{
  static float invDeltaR = 100.0;// was 1000.0, but we also need to convert from mm (ATLAS units) to cm (Mboy internal units)
  float diffrr = r*invDeltaR; // Skipping rmin, since in the original code it seems to be set to zero.
  int index = static_cast<unsigned int>(diffrr)-1;// account for difference in offsets between fortran and C++ by subtracting 1
  float error = 0.0;
  if (index>=static_cast<int>(m_mboyParametrisedErrors.size())){
    error= m_mboyParametrisedErrors.back();
  } else if (index<0) {
    error= m_mboyParametrisedErrors[0];
  } else {
    error = m_mboyParametrisedErrors[index] 
    + (m_mboyParametrisedErrors[index+1] - m_mboyParametrisedErrors[index])*(diffrr-static_cast<float>(index+1));
  }
  ATH_MSG_DEBUG(std::setw(20)<<std::setprecision(6)<<std::scientific <<"mBoyParametrisedSigma with radius="<<r<<"index="<<index<<", and error="<<error);
  return (error*10);// convert back into mm
}

double Muon::MdtDriftCircleOnTrackCreator::mooreErrorStrategy(const MuonDriftCircleErrorStrategy* myStrategy, 
                                                              double sigmaR2, const Identifier& id) const
{
  if(m_isMC) 
    return mooreErrorStrategyMC(myStrategy, sigmaR2, id);
  else{
    if(m_looseErrors)
      return mooreErrorStrategyLoose(myStrategy, sigmaR2, id);
    else
      return mooreErrorStrategyTight(myStrategy, sigmaR2, id);
  }
}

double Muon::MdtDriftCircleOnTrackCreator::mooreErrorStrategyMC(const MuonDriftCircleErrorStrategy* myStrategy,
								double sigmaR2, const Identifier& id) const {
  ATH_MSG_DEBUG("mooreErrorStrategy sigmaR2="<<sigmaR2);                                 
  
  // Moore error strategy.  Hard coding numbers for the time being - hope to make them configurable some day
  if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::Segment ) ){
    if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) &&
	 myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){
      if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::T0Refit ) ){
	ATH_MSG_VERBOSE(" segment error, t0fit ");
	return sigmaR2 + 0.005; // Collisions with T0 refit (input)
      } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ){
	ATH_MSG_VERBOSE(" segment error, broad ");
	return 4*sigmaR2 + 0.16; // Output segments - broad errors
      } else {
	ATH_MSG_VERBOSE(" segment error, precise ");
	return sigmaR2 + 0.005; // Input segments , no T0 refit
      }
      // dead code never reached - should be kept nevertheless to remember default
      // ATH_MSG_VERBOSE(" segment error default ");
      // return sigmaR2;
    }
    // Don't know how to handle other cases - error?
  } else { // Track
    Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
    if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::StationError ) ){
      if( stIndex == MuonStationIndex::BE ){
	ATH_MSG_VERBOSE(" track error BEE ");
	return 1.44*sigmaR2 + 1.44; // 1.2* + 1.2 mm
      }else if( stIndex == MuonStationIndex::EE ){
	ATH_MSG_VERBOSE(" track error EE ");
	if( !m_isMC && m_idHelper->stationEta(id) < 0 ) return 1.44*sigmaR2 + 0.16; // 1.2* + 0.4 mm
	return 1.44*sigmaR2 + 1.; // 1.2* + 1. mm
      }else if( stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIS &&
 	        abs(m_idHelper->stationEta(id)) > 6 ){
	ATH_MSG_VERBOSE(" track error BIS78 ");
	if( abs(m_idHelper->stationEta(id)) == 7 ) return 1.44*sigmaR2 + 1.; // 1.2* + 1. mm
	else                                       return 4*sigmaR2 + 25;    // 2* + 5. mm
      }
      ATH_MSG_VERBOSE(" track station error  ");
      return 1.44*sigmaR2 + 1.; // 1.2* + 1. mm
      
    }else if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) ){
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError) ) {
	ATH_MSG_VERBOSE(" track error Fixed/Broad ");
	return 4*sigmaR2 + 49.;  // 2* + 7 mm -> barrel/endcap overlaps
      }else{
	ATH_MSG_VERBOSE(" track error Fixed ");
	return 4*sigmaR2 + 4.; // 2* + 2mm S/L overlaps   
      }
      
    } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ) {
	ATH_MSG_VERBOSE(" track error Scaled/Broad ");
	return 2.25*sigmaR2 + 0.09;
      }else{
	// use slightly smaller errors for the barrel
	double fixedTerm = (stIndex == MuonStationIndex::BI||stIndex == MuonStationIndex::BM||stIndex == MuonStationIndex::BO) ? 0.014 : 0.04;
	if( m_doIndividualChamberReweights && stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIL &&
	    m_idHelper->stationEta(id) == 1 && m_idHelper->sector(id) == 13 && m_idHelper->mdtIdHelper().multilayer(id) == 1 ){
	  fixedTerm = 1;
	  ATH_MSG_VERBOSE(" track error Scaled: BIL1A13, first multi layer ");
	}else{
	  ATH_MSG_VERBOSE(" track error Scaled ");
	}
 	
	return 1.44*sigmaR2 + fixedTerm;
      }
    }
  } // End of segment or track
  // static bool first = true;
  // if (first){
  //   msg(MSG::WARNING) << "Unknown error strategy combination - check your configuration please! "<<(*myStrategy)<<endmsg;
  //   first = false;
  // }
  return sigmaR2;
}

double Muon::MdtDriftCircleOnTrackCreator::mooreErrorStrategyLoose(const MuonDriftCircleErrorStrategy* myStrategy, 
                                                                       double sigmaR2, const Identifier& id) const {
  ATH_MSG_DEBUG("mooreErrorStrategy sigmaR2="<<sigmaR2);                                  

  // Moore error strategy.  Hard coding numbers for the time being - hope to make them configurable some day
  if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::Segment ) ){
    if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) && 
        myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){
      if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::T0Refit ) ){
        ATH_MSG_VERBOSE(" segment error, t0fit ");
        return sigmaR2 + 0.005; // Collisions with T0 refit (input)
      } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ){
        ATH_MSG_VERBOSE(" segment error, broad ");
        return 4*sigmaR2 + 0.16; // Output segments - broad errors
      } else {
        ATH_MSG_VERBOSE(" segment error, precise ");
        return sigmaR2 + 0.005; // Input segments , no T0 refit
      }
      // dead code never reached - should be kept nevertheless to remember default
      // ATH_MSG_VERBOSE(" segment error default ");
      // return sigmaR2;
    } 
    // Don't know how to handle other cases - error?
  } else { // Track
    Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
    if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::StationError ) ){
      if( stIndex == MuonStationIndex::BE ){
        ATH_MSG_VERBOSE(" track error BEE ");
        return 1.44*sigmaR2 + 4; // 1.2* + 2 mm
      }else if( stIndex == MuonStationIndex::EE ){
        ATH_MSG_VERBOSE(" track error EE ");
        return 1.44*sigmaR2 + 0.04; // 1.2* + 0.2 mm
      }else if( stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIS &&
                abs(m_idHelper->stationEta(id)) > 6 ){
        ATH_MSG_VERBOSE(" track error BIS78 ");
        if( abs(m_idHelper->stationEta(id)) == 7 ) return 1.44*sigmaR2 + 1.; // 1.2* + 1. mm
        else                                       return 4*sigmaR2 + 25;    // 2* + 5. mm
      }else if( stIndex == MuonStationIndex::BM && m_idHelper->stationPhi(id) == 7 && 
                (m_idHelper->mdtIdHelper()).stationName(id) == 53 ){
        ATH_MSG_VERBOSE(" track error BME ");
        return 1.44*sigmaR2 + 0.25; // 1.2* + 0.5 mm
      }else if( stIndex == MuonStationIndex::BO && m_idHelper->chamberIndex(id) == MuonStationIndex::BOL &&
                abs(m_idHelper->stationEta(id)) == 7 && m_idHelper->stationPhi(id) == 7 ){
        ATH_MSG_VERBOSE(" track error BOE ");
        return 1.44*sigmaR2 + 0.25; // 1.2* + 0.5 mm
      }
      ATH_MSG_VERBOSE(" track station error  ");
      return 1.44*sigmaR2 + 0.04; // 1.2* + 0.2 mm

    }else if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) ){ 
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError) ) {
        ATH_MSG_VERBOSE(" track error Fixed/Broad ");
        return 4*sigmaR2 + 4.;  // 2* + 2 mm -> barrel/endcap overlaps
      }else{
        ATH_MSG_VERBOSE(" track error Fixed ");
        return 4*sigmaR2 + 4.; // 2* + 2mm S/L overlaps   
      }
      
    } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){ 
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ) {
        ATH_MSG_VERBOSE(" track error Scaled/Broad ");
        return 2.25*sigmaR2 + 0.09;
      }else{
        // use slightly smaller errors for the barrel
        double fixedTerm = (stIndex == MuonStationIndex::BI||stIndex == MuonStationIndex::BM||stIndex == MuonStationIndex::BO) ? 0.015 : 0.015;
        if( m_doIndividualChamberReweights && stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIL && 
        m_idHelper->stationEta(id) == 1 && m_idHelper->sector(id) == 13 && m_idHelper->mdtIdHelper().multilayer(id) == 1 ){
          fixedTerm = 1;
          ATH_MSG_VERBOSE(" track error Scaled: BIL1A13, first multi layer ");
        }else{
          ATH_MSG_VERBOSE(" track error Scaled ");
        }

        return 1.44*sigmaR2 + fixedTerm;
      }
    }
  } // End of segment or track
  // static bool first = true;
  // if (first){
  //   msg(MSG::WARNING) << "Unknown error strategy combination - check your configuration please! "<<(*myStrategy)<<endmsg;
  //   first = false;
  // }
  return sigmaR2;
}


double Muon::MdtDriftCircleOnTrackCreator::mooreErrorStrategyTight(const MuonDriftCircleErrorStrategy* myStrategy, 
                                                              double sigmaR2, const Identifier& id) const {
  ATH_MSG_DEBUG("mooreErrorStrategy sigmaR2="<<sigmaR2);                                  

  // Moore error strategy.  Hard coding numbers for the time being - hope to make them configurable some day
  if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::Segment ) ){
    if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) && 
        myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){
      if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::T0Refit ) ){
        ATH_MSG_VERBOSE(" segment error, t0fit ");
        return sigmaR2 + 0.005; // Collisions with T0 refit (input)
      } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ){
        ATH_MSG_VERBOSE(" segment error, broad ");
        return 4*sigmaR2 + 0.16; // Output segments - broad errors
      } else {
        ATH_MSG_VERBOSE(" segment error, precise ");
        return sigmaR2 + 0.005; // Input segments , no T0 refit
      }
      // dead code never reached - should be kept nevertheless to remember default
      // ATH_MSG_VERBOSE(" segment error default ");
      // return sigmaR2;
    } 
    // Don't know how to handle other cases - error?
  } else { // Track
    Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
    if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::StationError ) ){
      if( stIndex == MuonStationIndex::BE ){
        ATH_MSG_VERBOSE(" track error BEE ");
        return 1.44*sigmaR2 + 0.04; // 1.2* + 0.2 mm
      }else if( stIndex == MuonStationIndex::EE ){
        ATH_MSG_VERBOSE(" track error EE ");
        if(  m_idHelper->isSmallChamber(id) ) return 1.21*sigmaR2 + 0.01; // 1.1* + 0.1 mm
        else                                  return 1.21*sigmaR2 + 0.01; // 1.1* + 0.1 mm
      }else if( stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIS &&
        abs(m_idHelper->stationEta(id)) > 6 ){
          ATH_MSG_VERBOSE(" track error BIS78 ");
          if( abs(m_idHelper->stationEta(id)) == 7 ) return 1.44*sigmaR2 + 1.; // 1.2* + 1. mm
          else                                       return 4*sigmaR2 + 1.;    // 2* + 1. mm
      }else if( stIndex == MuonStationIndex::BM && m_idHelper->stationPhi(id) == 7 && 
		(m_idHelper->mdtIdHelper()).stationName(id) == 53 ){
          ATH_MSG_VERBOSE(" track error BME ");
          return 1.21*sigmaR2 + 0.25; // 1.1* + 0.5 mm
      }else if( stIndex == MuonStationIndex::BO && m_idHelper->chamberIndex(id) == MuonStationIndex::BOL &&
        abs(m_idHelper->stationEta(id)) == 7 && m_idHelper->stationPhi(id) == 7 ){
          ATH_MSG_VERBOSE(" track error BOE ");
          return 1.21*sigmaR2 + 0.25; // 1.1* + 0.5 mm
      }else if( stIndex == MuonStationIndex::EE && m_idHelper->chamberIndex(id) == MuonStationIndex::EEL &&
        m_idHelper->stationEta(id) < 0 && m_idHelper->stationPhi(id) == 3 ){
          ATH_MSG_VERBOSE(" track error EEL1C05 ");
          return 1.21*sigmaR2 + 25.; // 1.1* + 5 mm
      }
      ATH_MSG_VERBOSE(" track station error  ");
      return 1.21*sigmaR2 + 0.04; // 1.1* + 0.2 mm

    }else if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) ){ 
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError) ) {
        ATH_MSG_VERBOSE(" track error Fixed/Broad ");
        return 4*sigmaR2 + 4.;  // 2* + 2 mm -> barrel/endcap overlaps
      }else{
        ATH_MSG_VERBOSE(" track error Fixed ");
        return 4*sigmaR2 + 4.; // 2* + 2mm S/L overlaps   
      }
      
    } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){ 
      
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ) {
        ATH_MSG_VERBOSE(" track error Scaled/Broad ");
        return 2.25*sigmaR2 + 0.09;
      }else{
	
        // use slightly smaller errors for the barrel
	// 
        double fixedTerm = 0.01;
	bool isSmall = m_idHelper->isSmallChamber(id);
	if( stIndex == MuonStationIndex::BI||stIndex == MuonStationIndex::BM||stIndex == MuonStationIndex::BO) {
	  if( isSmall ) fixedTerm = 0.01;
	  else          fixedTerm = 0.01;
	}else{
	  if( isSmall ) fixedTerm = 0.01;
	  else          fixedTerm = 0.01;
	}
        if( m_doIndividualChamberReweights ){
	  if( stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIL && 
	      m_idHelper->stationEta(id) == 1 && m_idHelper->sector(id) == 13 && m_idHelper->mdtIdHelper().multilayer(id) == 1 ){
	    fixedTerm = 1;
	    ATH_MSG_VERBOSE(" track error Scaled: BIL1A13, first multi layer ");
	  }
        }else{
          ATH_MSG_VERBOSE(" track error Scaled ");
        }

        return 1.21*sigmaR2 + fixedTerm;
      }
    }
  } // End of segment or track
  // static bool first = true;
  // if (first){
  //   msg(MSG::WARNING) << "Unknown error strategy combination - check your configuration please! "<<(*myStrategy)<<endmsg;
  //   first = false;
  // }
  return sigmaR2;
}

double Muon::MdtDriftCircleOnTrackCreator::muonErrorStrategy(const MuonDriftCircleErrorStrategy* myStrategy, 
                                                             double sigmaR2, const Identifier& /*id*/) const {

//
//   the new muonErrorStrategy is identical for Data and MC
//   it assumes that for tracks the alignment uncertainties are added later 
//   this is done by the AignmentErrorTool where AlignmentEffectsOnTrack are used.
//   (for segment errors the mooreStrategy is coded)
//
//   it is inspired by the mooreErrorStrategyTight but doesnot need a constant term
//

  ATH_MSG_DEBUG("muonErrorStrategy sigmaR2="<<sigmaR2);                                  

  // Muon error strategy.  Hard coding numbers for the time being - hope to make them configurable some day
  if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::Segment ) ){
    if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) && 
        myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){
      if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::T0Refit ) ){
        ATH_MSG_VERBOSE(" segment error, t0fit ");
        return sigmaR2 + 0.005; // Collisions with T0 refit (input)
      } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ){
        ATH_MSG_VERBOSE(" segment error, broad ");
        return 4*sigmaR2 + 0.16; // Output segments - broad errors
      } else {
        ATH_MSG_VERBOSE(" segment error, precise ");
        return sigmaR2 + 0.005; // Input segments , no T0 refit
      }
    } 
  } else { // Track
    if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::StationError ) ){
      ATH_MSG_VERBOSE(" track station error  ");
      return 1.21*sigmaR2; // 1.1* mm
    }else if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::FixedError ) ){ 
        ATH_MSG_VERBOSE(" track error Fixed ");
        return 4*sigmaR2; // 2*    
    } else if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::ScaledError ) ){ 
      if( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::BroadError ) ) {
        ATH_MSG_VERBOSE(" track error Scaled/Broad ");
        return 2.25*sigmaR2;
      }else{
        ATH_MSG_VERBOSE(" Track scaled error ");
        return 1.21*sigmaR2;
      }
    }
  } // End of segment or track
  return sigmaR2;
}

double Muon::MdtDriftCircleOnTrackCreator::mboyErrorStrategy(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR) const 
{ 
  if ( myStrategy->creationParameter( MuonDriftCircleErrorStrategy::Segment ) ){
    //FRESEG = RESEGTUB(IMDT)**2 + DIFREC**2
    ATH_MSG_DEBUG("mboyErrorStrategy segments with input sigmaR2="<<sigmaR*sigmaR<<" and returning "<<sigmaR*sigmaR + 0.015625);
    return sigmaR*sigmaR + 0.015625; // hardcoded for now. 0.125^2
  } else {
    // Track for iTypfit!=6
    // FREREC = SQRT( RESTUB(IMDT)**2 + DIFREC**2 )
    // RESTUB(IMDT) = 1.60000000000000003E-002 cm for all values of IMDT
    // std::cout<<"Track error is "<<sigmaR2 + 0.0256<<std::endl;
    // return sigmaR2 + 0.0256 ; // 0.0256=0.16^2
    
    //iTypFit==6
    //DIFREC = FREDIGINT(RRR) 
    //FREREC = 1.1D0 * ( FREDIGINT(RRR) + RESOTU(IMDT) )
    // RESOTU(IMDT) always equals 8.50000000000000061E-003 cm, so 8.5E-002 mm 0.085
    //std::cout << "sqrt(sigmaR2)=" <<std::setprecision(6)<< sqrt(sigmaR2) << ", mboyErrorStrategy=" <<std::setprecision(6)<< (1.1*(sqrt(sigmaR2)+0.085))*(1.1*(sqrt(sigmaR2)+0.085)) << ", RESOTU(IMDT)=0.085=hardcoded \n";
    ATH_MSG_DEBUG("mboyErrorStrategy tracks with input sigmaR2="<<sigmaR*sigmaR<<" and returning "<<(1.1*(sigmaR+0.085))*(1.1*(sigmaR+0.085)));
    return (1.1*(sigmaR+0.085))*(1.1*(sigmaR+0.085));
  }
  // dead code never reached - should be kept nevertheless to remember default
  // static bool first = true;
  // if (first){
  //   msg(MSG::WARNING) << "Unknown error strategy combination - check your configuration please! "<<(*myStrategy)<<endmsg;
  //   first = false;
  // }
  // return sigmaR*sigmaR;
}
