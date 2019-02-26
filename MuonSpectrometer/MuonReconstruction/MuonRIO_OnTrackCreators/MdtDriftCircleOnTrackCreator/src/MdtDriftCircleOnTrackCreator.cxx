/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  m_tofTool(""),
  m_invSpeed(1./299.792458),
  m_mdtCalibSvcSettings( 0 ),
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
}


Muon::MdtDriftCircleOnTrackCreator::~MdtDriftCircleOnTrackCreator(){}


StatusCode Muon::MdtDriftCircleOnTrackCreator::initialize()
{

  m_mdtCalibSvcSettings->initialize();
  
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
  
  ATH_CHECK( m_idHelper.retrieve() );
  
  if (m_doMdt) {
    ATH_CHECK( m_mdtCalibSvc.retrieve() );
    // Get pointer to MdtCalibrationDbSvc and cache it :
    ATH_CHECK( m_mdtCalibDbSvc.retrieve() );
  } else {
    ATH_MSG_WARNING( " tool is configured such that MDT_DCs are only copied!" );
  }
  
  if( m_timeCorrectionType == COSMICS_TOF ){
    if( m_tofTool.empty() ) {
      ATH_MSG_ERROR( "The time of flight tool is not configured. Please check your configuration" );
      return StatusCode::FAILURE;
    }
    if( m_tofTool.retrieve().isSuccess() ){
      ATH_MSG_DEBUG("Retrieved " << m_tofTool );
    }else{
      ATH_MSG_ERROR( "Could not get " << m_tofTool );
      return StatusCode::FAILURE;
    }
    if( !m_errorStrategy.creationParameter(MuonDriftCircleErrorStrategy::TofCorrection) ){
      ATH_MSG_WARNING( "Detected bad default configuration, using Cosmic TOF witout time of flight corrections does not work" );
    }
  }
  ATH_MSG_DEBUG("Timing mode set to  " << m_timeCorrectionType );
  if (m_timeCorrectionType>=NumberOfTimingModes) {
    ATH_MSG_ERROR( "Time Correction Type too large! Aborting." );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
} 

StatusCode Muon::MdtDriftCircleOnTrackCreator::finalize()
{
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
    ATH_MSG_WARNING( "globalToLocal failed! " );
    return 0;
  }
  
  // if wire sag is taken into account, cast the surface to StraightLineSurface so it can be added to the ROT
  const Trk::StraightLineSurface* saggedSurf = 0;
  if (myStrategy->creationParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection)) {
    saggedSurf = nominalSurf->correctedSurface(*tempLocOnWire); // sagged surface
    if( !saggedSurf ){
      ATH_MSG_WARNING( "DistortedSurface failed to create sagged surface, using nominal surface " );
      saggedSurf = nominalSurf;
    }
    
    //set large value for tolerance, to make sure that global position including drift radius is taken to the wire.
    const Amg::Vector2D* tempLocOnSaggedWire = saggedSurf->Trk::Surface::globalToLocal(GP,m_globalToLocalTolerance);
    delete tempLocOnWire; 
    if( !tempLocOnSaggedWire ){
      ATH_MSG_WARNING( "globalToLocal failed for sagged surface, not applying sagging! " );
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
      ATH_MSG_WARNING( "Unexpected globalToLocal failure, cannot create MDT ROT " );
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
    //     && (GP.z()-rot->globalPosition().z())>tolerance  ) ATH_MSG_WARNING("Global position differs: before = "<<GP<<"\t after="<<rot->globalPosition() );
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
  
  std::unique_ptr<MdtCalibHit> calibHit;
  if ( m_doMdt ) {
    const MuonGM::MdtReadoutElement* detEl = DC.detectorElement();
    if(!detEl){
      ATH_MSG_WARNING( "MdtPrepData without not a MdtReadoutElement" );
  
      Amg::MatrixX  localCov(1,1);
      localCov(0,0) = 0.0;
 
      return CalibrationOutput(Amg::Vector2D(),localCov,0.,false);
    }
    
    // call calibration Service
    calibHit=std::make_unique<MdtCalibHit>();
    calibHit->setIdentifier( DC.identify() );
    calibHit->setTdc( DC.tdc() );
    calibHit->setAdc( DC.adc() );
    calibHit->setGlobalPointOfClosestApproach(gpos);
    calibHit->setGeometry(detEl);
    
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
        ATH_MSG_WARNING( "No valid mode selected, cannot apply tof correction" );
        inputData.tof = 0.;
        break;
    }
    
    // call the calibration service providing the time when the particle passed the tube
    ok = m_mdtCalibSvc->driftRadiusFromTime( *calibHit, inputData, *m_mdtCalibSvcSettings );
    
    driftTime = calibHit->driftTime();
    radius    = calibHit->driftRadius();  // copy new values
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
      ATH_MSG_WARNING( "Error matrix of DC doesn't have dimension 1 " << DC.localCovariance() );
      ATH_MSG_WARNING( "Reducing size to 1 dim" );
    }
  } 
  
  // Handle the errors here.  Begin by getting the first part of the resolution term
  if (!m_doMdt){
    sigmaR = sqrt(DC.localCovariance()(0,0));
  } else if( myStrategy->creationParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors) ){
    if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ){
      sigmaR   = parametrisedSigma(errRadius);
      ATH_MSG_DEBUG("R= "<<radius<<"\tMoore sigmaR="<<sigmaR);
    } else if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Muon ){
      sigmaR   = parametrisedSigma(errRadius);
      ATH_MSG_DEBUG("R= "<<radius<<"\tMuon sigmaR="<<sigmaR);
    }
  }else{
    // Use calib service errors.
    if(calibHit) sigmaR   =sqrt(calibHit->sigma2DriftRadius());
  }
  ATH_MSG_DEBUG("Tube : " << m_idHelper->toString(DC.identify()) << " SigmaR = "<<sigmaR);
  double sigmaR2=0.0;
  // Handle the errors scaling / addition of fixed terms 
  if ( myStrategy->strategy()==MuonDriftCircleErrorStrategy::Moore ) {
    sigmaR2 = mooreErrorStrategy(myStrategy,sigmaR*sigmaR,DC.identify());
    ATH_MSG_DEBUG("After scaling etc:\t Moore sigmaR2 = "<<sigmaR2);
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
    ATH_MSG_WARNING( " Incorrect hit type:  Trk::PrepRawData not a Muon::MdtPrepData!! No rot created " );
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
    ATH_MSG_WARNING( " Incorrect hit type:  Trk::PrepRawData not a Muon::MdtPrepData!! No rot created " );
    return 0;
  }
  
  static bool firstTime = true;
  if (firstTime){
   ATH_MSG_DEBUG( "Called correct using the base class implementation.  Will use the default error strategy" );
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
  //   ATH_MSG_WARNING( "Unknown error strategy combination - check your configuration please! "<<(*myStrategy) );
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
  //   ATH_MSG_WARNING( "Unknown error strategy combination - check your configuration please! "<<(*myStrategy) );
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
  //   ATH_MSG_WARNING( "Unknown error strategy combination - check your configuration please! "<<(*myStrategy) );
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
