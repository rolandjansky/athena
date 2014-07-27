/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// IdScan: TrigIDSCAN.cxx
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

// general Atlas classes
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IssueSeverity.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaKernel/Timeout.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "eformat/DetectorMask.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetTrackUtils/TrigToTrkTrackTool.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"

#include "TrigInDetToolInterfaces/ITrigZFinder.h"
#include "TrigInDetToolInterfaces/ITrigHitFilter.h"
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"
#include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"

#ifdef IDSCANDUMP
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#endif

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"

// IdScan stuff
#include "TrigIDSCAN/TrigIDSCAN.h"
#include "TrigIDSCAN/RoI_Shape.h"

#include "TrigIDSCAN/IShifterTool.h"

// RegionSelector code to get disabled layers
#include "IRegionSelector/IRegSelSvc.h"
//#include "RegionSelector/RegSelSvc.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#ifdef IDSCANDUMP

#include "TTree.h"
#include "TFile.h"

#include "TrigIDSCAN/IDScanAthenaEventHeader.h"

#endif

#include<sstream>

using namespace std;

template <class SRC>
inline const DataVector<TrigInDetTrack>** dvec_cast(SRC** ptr) { 
  return (const DataVector<TrigInDetTrack>**)(ptr); 
} 

TrigIDSCAN::TrigIDSCAN(const std::string& name, ISvcLocator* pSvcLocator) : 
  HLT::FexAlgo(name, pSvcLocator), 
  m_SpacePointProvider("OnlineSpacePointProviderTool"),
  m_TrigInDetTrackFitter("TrigInDetTrackFitter"),
  m_TrigTRT_TrackExtensionTool("TrigTRT_TrackExtensionTool"),
  m_TrigL2ResidualCalculator("TrigL2ResidualCalculator"),
  m_zFinder("IDScanZFinder",this),
  m_convertorTool("TrigToTrkTrackTool",this),
  m_hitFilter("IDScanHitFilter",this),
  m_shifterTool("ShifterTool", this),
  m_findMultipleZ(false),
  m_ntracks(0),
  m_nPixSPsInRoI(0),
  m_nSCTSPsInRoI(0),
  m_currentStage(-1),
  m_nZvertices(0),
  m_zPosition(-9999.0),
  m_shift(0),
#ifdef IDSCANDUMP
  m_TrigL2SpacePointTruthTool("TrigL2SpacePointTruthTool"),
#endif
  m_generateRoI(false),
  m_useRoiDescriptorWidths(false),
  m_attachedFeatureName("")
{

  declareProperty( "TrueVertexLocation",      m_TrueVertexLocation = "TrueTrigVertexColl" );
  
  declareProperty( "PrintDiagnosticMessages", m_printDiagnosticMessages = false );
  
  declareProperty( "SpacePointProviderTool", m_SpacePointProvider  );
  declareProperty( "TrigZFinder",            m_zFinder);
  declareProperty( "TrigInDetTrackFitter",   m_TrigInDetTrackFitter );
  declareProperty( "TrigTRT_TrackExtensionTool", m_TrigTRT_TrackExtensionTool);
  declareProperty( "TrigL2ResidualCalculator", m_TrigL2ResidualCalculator);
#ifdef IDSCANDUMP
  declareProperty( "TrigL2SpacePointTruthTool", m_TrigL2SpacePointTruthTool);
#endif
  declareProperty( "TrigToTrkTrackConvertorTool",m_convertorTool);
  declareProperty( "TrigHitFilter",m_hitFilter);
  declareProperty( "ShifterTool",  m_shifterTool);
  
  declareProperty( "PixelSP_ContainerName", m_pixelSpName = "TrigPixelSpacePoints" );
  declareProperty( "SCT_SP_ContainerName",  m_sctSpName = "TrigSCT_SpacePoints" );
  
  declareProperty( "etaHalfWidth",        m_etaHalfWidth = 0.1);
  declareProperty( "phiHalfWidth",        m_phiHalfWidth = 0.1);
  declareProperty( "FullScanMode",        m_fullScanMode = false);
  declareProperty( "GenerateRoI",         m_generateRoI = false);
  
  declareProperty( "ZFinderMode",           m_zFinderMode = 0 );
  declareProperty( "MinHits",               m_minHits = 5 );
  declareProperty( "m_maxNumTracks",        m_maxNumTracks = 1000 );
  declareProperty( "TrkTrackCollName",      m_TrkTrackCollName = "IDSCAN_Tracks");
  declareProperty( "OutputCollectionSuffix",m_outputCollectionSuffix = "");
  declareProperty( "trackConvertorMode",    m_trackConvertorMode = 1 );
  declareProperty( "doTRTpropagation",      m_doTRTpropagation = true );
  
  declareProperty( "doShift",               m_doShift = false );
  declareProperty( "translateOnly",         m_translateOnly=false);
  declareProperty( "xShift",                m_xShift = 0);
  declareProperty( "yShift",                m_yShift = 0);
  declareProperty( "UseShifterZ",           m_useShifterZ = 2);

  declareProperty( "RegionSelectorTool",    m_regionSelectorName = "RegSelSvc" );
  declareProperty( "AdjustLayerThreshold",  m_adjustLayerThreshold = false );
  declareProperty( "DetMaskCheck",          m_check_detector_mask = false);
  declareProperty( "DetMaskCheckOnEvent",   m_detector_mask_on_event = false);

  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);

  declareProperty( "doResMonitoring",       m_doResMonitoring = false);

  declareProperty( "useRoiDescriptorWidths", m_useRoiDescriptorWidths = false);

  declareProperty( "FindMultipleZ", m_findMultipleZ = false );

  /** write spacepoints to ntpl ?
   ** To include this code, uncomment the three lines in the requirements file
   ** which define the IDSCANDUMP token and compile the root dictionary  
   **/
  // NB: these job options are always needed because of the python configurable
  declareProperty("WriteFlag",  mWriteFlag = true ); 
  declareProperty("DumpPrefix", mWriteName = "spacepoints" ); 
  declareProperty("WriteTruth", mWriteTruth = true ); 

  //duplication with OnlineSpacePointTool - needed for caching ROB requests
  declareProperty( "zMin", m_minZ = -168.0 );
  declareProperty( "zMax", m_maxZ =  168.0 );

#ifdef IDSCANDUMP
  mHeader=NULL;
#endif
  
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countRoIwithEnoughHits=0;
  m_countRoIwithGroups[0]=0;   m_countRoIwithGroups[1]=0; 
  m_countRoIwithTracks[0]=0;   m_countRoIwithTracks[1]=0; 
  m_countRoIzFinderFailed=0;
  m_l1Id=99999999;
  m_roiShape=0;
  m_recoTracks=NULL;

  // declare monitoring histograms


  declareMonitoredCollection("trk_pt",         *dvec_cast(&m_recoTracks), &monPt);
  declareMonitoredCollection("trk_a0",         *dvec_cast(&m_recoTracks), &monA0);
  declareMonitoredCollection("trk_z0",         *dvec_cast(&m_recoTracks), &monZ0);
  declareMonitoredCollection("trk_phi0",       *dvec_cast(&m_recoTracks), &monPhi0);
  declareMonitoredCollection("trk_eta",        *dvec_cast(&m_recoTracks), &monEta);
  declareMonitoredCollection("trk_chi2dof",    *dvec_cast(&m_recoTracks), &monChi2);
  declareMonitoredCollection("trk_nSiHits",    *dvec_cast(&m_recoTracks), &monNHit_Si);
  declareMonitoredCollection("trk_nPIXHits",   *dvec_cast(&m_recoTracks), &monNPixSP);
  declareMonitoredCollection("trk_nSCTHits",   *dvec_cast(&m_recoTracks), &monNSctSP);
  declareMonitoredCollection("trk_nTRTHits",   *dvec_cast(&m_recoTracks), &TrigInDetTrack::NTRHits);
  declareMonitoredCollection("trk_nTRTStraws", *dvec_cast(&m_recoTracks), &TrigInDetTrack::NStrawHits);
  declareMonitoredStdContainer("trk_a0beam",m_a0beam);
  declareMonitoredStdContainer("trk_dPhi0",m_trkdPhi0);
  declareMonitoredStdContainer("trk_dEta" ,m_trkdEta);

  declareMonitoredStdContainer("hit_PIXBarrelPhiResidual",m_pixResPhiBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelEtaResidual",m_pixResEtaBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelPhiPull",    m_pixPullPhiBarrel);
  declareMonitoredStdContainer("hit_PIXBarrelEtaPull",    m_pixPullEtaBarrel);
  declareMonitoredStdContainer("hit_SCTBarrelResidual",   m_sctResBarrel);
  declareMonitoredStdContainer("hit_SCTBarrelPull",       m_sctPullBarrel);
  declareMonitoredStdContainer("hit_PIXEndCapPhiResidual",m_pixResPhiEC);
  declareMonitoredStdContainer("hit_PIXEndCapEtaResidual",m_pixResEtaEC);
  declareMonitoredStdContainer("hit_PIXEndCapPhiPull",    m_pixPullPhiEC);
  declareMonitoredStdContainer("hit_PIXEndCapEtaPull",    m_pixPullEtaEC);
  declareMonitoredStdContainer("hit_SCTEndCapResidual",   m_sctResEC);
  declareMonitoredStdContainer("hit_SCTEndCapPull",       m_sctPullEC);

  declareMonitoredStdContainer("dataErrors_SCT",  m_sctDataErrors);
  declareMonitoredStdContainer("dataErrors_PIX",  m_pixelDataErrors);
  declareMonitoredStdContainer("dataErrors_TRT",  m_trtDataErrors);
  declareMonitoredStdContainer("dataErrors",      m_dataErrors);
  declareMonitoredVariable("roi_nTracks",m_ntracks);
  declareMonitoredVariable("roi_nSPsPIX",m_nPixSPsInRoI);
  declareMonitoredVariable("roi_nSPsSCT",m_nSCTSPsInRoI);
  declareMonitoredVariable("roi_lastStageExecuted",m_currentStage);
  declareMonitoredVariable("roi_eta", m_roiEta);
  declareMonitoredVariable("roi_phi", m_roiPhi);
  // IDScan specific monitoring:
  declareMonitoredVariable("roi_nZvertices",m_nZvertices);
  declareMonitoredVariable("roi_ZvrtxPos",m_zPosition);
  declareMonitoredVariable("roi_SPxShift",m_shift);
}

//--------------------------------------------------------------------------

TrigIDSCAN::~TrigIDSCAN()
{
  //    std::cout << "TrigIDSCAN::~TrigIDSCAN() m_roiShape=" << m_roiShape<< std::endl;
  if (m_roiShape != 0){
    delete m_roiShape;
    m_roiShape = 0;
  }
  
#ifdef IDSCANDUMP
  if ( mHeader ) { 
    delete mHeader;
    mHeader = NULL;
  }
#endif 
}

//-----------------------------------------------------------------------

HLT::ErrorCode TrigIDSCAN::hltInitialize()
{
  msg() << MSG::INFO << "TrigIDSCAN::initialize() "  << PACKAGE_VERSION << endreq;
  StatusCode sc = StatusCode::FAILURE;
  
  if (m_printDiagnosticMessages) msg() << MSG::WARNING 
				       << "TrigIDSCAN Diagnostic Messages enabled - this will give many messages per event to std::out !!" 
				       << endreq;
  
  //   Get SPTools
  sc = m_SpacePointProvider.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate SpacePointProviderTool " << m_SpacePointProvider << endreq;
    return HLT::BAD_JOB_SETUP;
  } 
  sc = m_TrigInDetTrackFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrackFitter tool " << m_TrigInDetTrackFitter << endreq;
    return HLT::BAD_JOB_SETUP;
  }      
  
  if(m_doTRTpropagation) {
    sc = m_TrigTRT_TrackExtensionTool.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL <<"Unable to locate TRT track extension tool " 
	    << m_TrigTRT_TrackExtensionTool << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  }

  sc = m_TrigL2ResidualCalculator.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL <<"Unable to locate Residual calculator tool " 
	  << m_TrigL2ResidualCalculator << endreq;
    return HLT::BAD_JOB_SETUP;
  }

#ifdef IDSCANDUMP
  sc = m_TrigL2SpacePointTruthTool.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL <<"Unable to locate SpacePoint-to-Truth associator tool " 
	  << m_TrigL2SpacePointTruthTool<< endreq;
    return HLT::BAD_JOB_SETUP;
  }
#endif

  if ( m_adjustLayerThreshold ) { 
    msg() << MSG::INFO << "will adjust layer threshold depending on disabled modules" << endreq;
  }
  //   Get the Region Selector Svc
  //
  sc = serviceLocator()->service( m_regionSelectorName, m_regionSelector);
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL 
	  << "Unable to retrieve RegionSelector Service  " << m_regionSelectorName << endreq;
    return HLT::BAD_JOB_SETUP;
  };


  m_detector_mask_not_checked = m_check_detector_mask;

  m_hasPixelBLayer = true;
  m_hasPixelBarrel = true; m_hasPixelDisk  = true;
  m_hasSCTBarrelA  = true; m_hasSCTEndcapA = true;
  m_hasSCTBarrelC  = true; m_hasSCTEndcapC = true;

  declareMonitoredObject("time_TRTDataPrep",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTTotalTime);
  declareMonitoredObject("time_TRTBStoDC",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTTrtCnvTime);
  declareMonitoredObject("time_TRTRobProv",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRTRobProvTime);
  declareMonitoredObject("time_TRTDataTotal",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_DataProviderTime);
  declareMonitoredObject("time_TRTRobPreload",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_RobPreloaderTime);
  declareMonitoredObject("time_TRTRoads",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_RoadBuilderTime);
  declareMonitoredObject("time_TRTPDAF",m_TrigTRT_TrackExtensionTool.operator*(),&ITrigTRT_TrackExtensionTool::getTRT_PDAF_TrackingTime);
  declareMonitoredObject("time_PIXBStoClusters",m_SpacePointProvider.operator*(),&ISpacePointProvider::getBStoClustersTimingPixel);
  declareMonitoredObject("time_SCTBStoClusters",m_SpacePointProvider.operator*(),&ISpacePointProvider::getBStoClustersTimingSCT);
  declareMonitoredObject("time_PIXSPFormation",m_SpacePointProvider.operator*(),&ISpacePointProvider::getSPformationTimingPixel);
  declareMonitoredObject("time_SCTSPFormation",m_SpacePointProvider.operator*(),&ISpacePointProvider::getSPformationTimingSCT);
  declareMonitoredObject("time_SiDataRegSel",m_SpacePointProvider.operator*(),&ISpacePointProvider::getRegSelTiming);
  declareMonitoredObject("time_SiDataTotal",m_SpacePointProvider.operator*(),&ISpacePointProvider::getTotalTiming);

  //   add some timers:
  
  if ( timerSvc() ) {
    m_SpacePointReaderTimer = addTimer("SpacePointReader");
    m_ZFinderTimer          = addTimer("ZFinder", "Zfinder_ZFnSP");
    m_IdScanHitsTimer       = addTimer("IdScanHits", "IdScanHits_intSP");
    m_HitFilterTimer        = addTimer("HitFilter", "HitFilter_HFnGr");
    m_SctKFitterTimer       = addTimer("SctKFitter", "SctKfitter_SKFnTracks");
    m_inZFTimer             = addTimer("inZF");
    m_inHFTimer             = addTimer("inHF");
    m_IdScanTRTTimer        = addTimer("IdScanTRT", "IdScanTRT_intTR");
  }

  msg() << MSG::INFO << " TrigIDSCAN : MinHits set to " << m_minHits << endreq;
  
  if ( m_doShift ) { 

    sc = m_shifterTool.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to retrieve ShifterTool " << m_shifterTool  << endreq;
      return HLT::BAD_JOB_SETUP;
    }

    msg() << MSG::INFO << " TrigIDSCAN :\t Shifting spacepoints " << m_translateOnly << endreq;
    if ( m_translateOnly ) {
      m_shifterTool->translateOnly(true);
      m_shifterTool->xShift(m_xShift);
      m_shifterTool->yShift(m_yShift);
      msg() << MSG::INFO << "\t\tTranslating by fixed (x,y)=" << m_xShift << "\t" << m_yShift << endreq;
    }

    if (m_useBeamSpot) 
      msg() << MSG::WARNING << "\t\tBeamspot displacement will be overwritten by the ShifterTool results!" << endreq;
  }

  if (m_useBeamSpot) {
    StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);
    if (scBS.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
    }
  }

  if( m_zFinderMode ==1 ){
    msg() << MSG::WARNING << " You have chosen to use the MC z0 position!" << endreq;
  }
  sc = m_zFinder.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to retrieve ZFinder " << m_zFinder  << endreq;
    return HLT::BAD_JOB_SETUP;
  }


  //   double zfPhiWidth;
  //   DoubleProperty zfPhiWidthProp( "ROIPhiWidth", zfPhiWidth);
  //   sc=m_zFinder->getProperty( &zfPhiWidthProp );
  //   if(sc.isFailure())
  //     {
  //       msg() << MSG::ERROR << "Cannot get ROIPhiWidth property of ZFinder"<<endreq;
  //       return HLT::BAD_JOB_SETUP;
  //     }
  
  //   if(fabs(zfPhiWidthProp.value()-2.0*m_phiHalfWidth)>0.0001)
  //     {
  //       msg() << MSG::WARNING << "ZFinder ROIPhiWidth does not match IdScan phiHalfWidth"<<endreq;
  //       msg() << MSG::WARNING << "ZFinder ROIPhiWidth = "<<zfPhiWidthProp.value()<<endreq;
  //       msg() << MSG::WARNING << "IdScan phiHalfWidth x 2 = "<<2.0*m_phiHalfWidth<<endreq;
  //       msg() << MSG::WARNING << "IdScan initialization failed"<<endreq;
  //       return StatusCode::FAILURE;
  //     }


  sc = m_hitFilter.retrieve();
  if ( sc.isFailure() ){
    msg() << MSG::FATAL << "Unable to retrieve HitFilter " << m_hitFilter <<endreq;
    return HLT::BAD_JOB_SETUP;
  }
  /*
    double hfPhiWidth,hfEtaWidth;
    DoubleProperty hfPhiWidthProp( "phiHalfWidth", hfPhiWidth);
    sc=m_hitFilter->getProperty( &hfPhiWidthProp );
    if(sc.isFailure()) {
    msg() << MSG::ERROR << "Cannot get phiHalfWidth property of HitFilter"<<endreq;
    return HLT::BAD_JOB_SETUP;
    }
    if(fabs(hfPhiWidthProp.value()-m_phiHalfWidth)>0.0001)
    {
    msg() << MSG::WARNING << "HitFilter phiHalfWidth does not match IdScan phiHalfWidth"<<endreq;
    msg() << MSG::WARNING << "HitFilter phiHalfWidth = "<<hfPhiWidthProp.value()<<endreq;
    msg() << MSG::WARNING << "IdScan phiHalfWidth  = "<<m_phiHalfWidth<<endreq;
    msg() << MSG::WARNING << "IdScan initialization failed"<<endreq;
    return HLT::BAD_JOB_SETUP;
    }
    DoubleProperty hfEtaWidthProp( "etaHalfWidth", hfEtaWidth);
    sc=m_hitFilter->getProperty( &hfEtaWidthProp );
    if(sc.isFailure())
    {
    msg() << MSG::ERROR << "Cannot get etaHalfWidth property of HitFilter"<<endreq;
    return HLT::BAD_JOB_SETUP;
    }
    if(fabs(hfEtaWidthProp.value()-m_etaHalfWidth)>0.0001)
    {
    msg() << MSG::WARNING << "HitFilter etaHalfWidth does not match IdScan etaHalfWidth"<<endreq;
    msg() << MSG::WARNING << "HitFilter etaHalfWidth = "<<hfEtaWidthProp.value()<<endreq;
    msg() << MSG::WARNING << "IdScan etaHalfWidth  = "<<m_etaHalfWidth<<endreq;
    msg() << MSG::WARNING << "IdScan initialization failed"<<endreq;
    return HLT::BAD_JOB_SETUP;
    }
  */
  if(m_trackConvertorMode == 1) {
    sc = m_convertorTool.retrieve();
    if ( sc.isFailure() ){
      msg() << MSG::FATAL << "Unable to retrieve TrigToTrkTrackTool " <<m_convertorTool << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  }

  if ( m_outputCollectionSuffix != "" )  {
    m_attachedFeatureName = string("TrigIDSCAN_") + m_outputCollectionSuffix; }
  else {
    string namestr = name();
    std::transform(namestr.begin(), namestr.end(),
		   namestr.begin(), (int(*)(int)) std::tolower);
    if ( namestr.find("cosmic") == string::npos )
      m_attachedFeatureName = name();
    else
      m_attachedFeatureName="TrigIDSCAN_Cosmics";
  }

  msg() << MSG::INFO << " Features recorded with Key " << m_attachedFeatureName << endreq;

#ifdef IDSCANDUMP

  // create ntpl for writing out spacepoints if required 
  
  if ( mWriteFlag ) { 
    
    mWriteCounter = 0; // counter 
    
    mHeader = new IDScanAthenaEventHeader();
    
    // maybe need some root directory navigation ?
    //TDirectory* cwd = gDirectory; 
    
    string filename = mWriteName + string("-") +  name() + string(".root");

    msg() << MSG::INFO << "Dumping space points to ntple file " << filename << endreq;

    mFile = new TFile(filename.c_str(), "recreate");
    mTree = new TTree("tree","tree");
    
    mTree->Branch("IDScanEventHeader", 
		  "IDScanEventHeader",
		  (IDScanEventHeader**)&mHeader, 6400, 1);
    
    
    // gDirectory->cd();
    
  }
#endif  
  
  //   sc = toolSvc()->retrieveTool( "JiveXML::EventData2XML", "EventData2XML", m_EventData2XML);
  //   if ( sc.isFailure() ) {
  //     msg() << MSG::FATAL << "Unable to locate tool  EventData2XML"  << endreq;
  //   }

  msg() << MSG::INFO << " Initialized successfully" << endreq; 
  return HLT::OK;  
}


//-------------------------------------------------------------------------

HLT::ErrorCode TrigIDSCAN::hltBeginRun()
{
  msg() << MSG::INFO << "At BeginRun of " << name() << endreq;

  if(m_detector_mask_not_checked) {

    const EventInfo* pEventInfo(0);
    if ( store()->retrieve(pEventInfo).isFailure() ) {
      msg() << MSG::FATAL << "Failed to get EventInfo for detector mask info in BeginRun()" << endreq;
      return HLT::SG_ERROR;
    }
    setup_detector_mask(pEventInfo->event_ID());
  }
  return HLT::OK;
}

void TrigIDSCAN::setup_detector_mask(const EventID* eventId) { 

  uint64_t dmask = eventId->detector_mask();

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieved the detector mask = 0x"
	  << MSG::hex << dmask << MSG::dec << endreq;
  }

  if( dmask==0 ) {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "detector mask == 0: do nothing!" << endreq;
    }
    return;
  }

  eformat::helper::DetectorMask dm(dmask);
  m_hasPixelBLayer  = dm.is_set(eformat::PIXEL_B_LAYER);
  m_hasPixelBarrel  = dm.is_set(eformat::PIXEL_BARREL);
  m_hasPixelDisk    = dm.is_set(eformat::PIXEL_DISK);
  m_hasSCTBarrelA   = dm.is_set(eformat::SCT_BARREL_A_SIDE);
  m_hasSCTBarrelC   = dm.is_set(eformat::SCT_BARREL_C_SIDE);
  m_hasSCTEndcapA   = dm.is_set(eformat::SCT_ENDCAP_A_SIDE);
  m_hasSCTEndcapC   = dm.is_set(eformat::SCT_ENDCAP_C_SIDE);

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " ==== Detector Mask settings ==== " << endreq;           
    msg() << MSG::DEBUG << " ---> Pixel B-Layer: " << ((m_hasPixelBLayer)? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> Pixel Barrel: "  << ((m_hasPixelBarrel)? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> Pixel Disk: "    << ((m_hasPixelDisk)  ? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> SCT Barrel side A: " << ((m_hasSCTBarrelA)? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> SCT Barrel side C: " << ((m_hasSCTBarrelC)? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> SCT Endcap side A: " << ((m_hasSCTEndcapA)? "ON" : "OFF") << endreq;
    msg() << MSG::DEBUG << " ---> SCT Endcap side C: " << ((m_hasSCTEndcapC)? "ON" : "OFF") << endreq;
  }

  if (!m_detector_mask_on_event) m_detector_mask_not_checked = false;
}


//-------------------------------------------------------------------------
HLT::ErrorCode TrigIDSCAN::prepareRobRequests(const HLT::TriggerElement* inputTE ) 
{
  // Calculate ROBs needed  - this code should be shared with hltExecute to avoid slightly different requests
  const TrigRoiDescriptor* roi = 0; 
  bool useforID(false);

  if (getFeature(inputTE, roi, "forID") != HLT::OK || roi == 0){
    getFeature(inputTE, roi);
  } else {
    useforID = true;
  }
 
  if ( roi==NULL ) { 
    msg() <<  MSG::WARNING << "REGTEST / Failed to find RoiDescriptor " << endreq;
    return HLT::NAV_ERROR;
  }
 
    

  TrigRoiDescriptor tmp_roi(*roi);
  m_roiEta = roi->eta();
  m_roiPhi = roi->phi();

  if ( m_fullScanMode && roi->phi()!=0 ) { 
    m_roiPhi = 0;
    m_roiEta = 0;
    msg() << MSG::DEBUG << " FullScan input RoI not properly defined roi->phi()=" << roi->phi() 
	  << " will use roiPhi=" << m_roiPhi << endreq;
  }

  if(m_roiShape!=NULL) delete m_roiShape;
  if ( m_fullScanMode ) {
    m_roiShape = new TrigIdScan::RoI_Shape( 3, M_PI );
  }
  else { 
    
    if ( m_useRoiDescriptorWidths && useforID ) { 

      double etaHalfWitt = 0.5*( tmp_roi.etaPlus() - tmp_roi.etaMinus() ); 
      double phiHalfWitt = 0.5*HLT::wrapPhi( tmp_roi.phiPlus() - tmp_roi.phiMinus() ); 

      m_roiShape = new TrigIdScan::RoI_Shape( etaHalfWitt, phiHalfWitt ); 
    }
    else {
      /// use values from configurable
      m_roiShape = new TrigIdScan::RoI_Shape( m_etaHalfWidth, m_phiHalfWidth ); 
    }
 
  }


  // check if the roi is properly defined
  if ( !( m_roiEta>-10 && m_roiEta<10 ) ) { 
    msg() << MSG::WARNING << " Input RoI not properly defined eta=" << m_roiEta << endreq;
    return HLT::NUM_ERROR;
  }
    
  if ( !( m_roiPhi>=-2*M_PI && m_roiPhi<=2*M_PI ) ) { 
    msg() << MSG::WARNING << " Input RoI not properly defined phi=" << m_roiPhi << endreq;
    return HLT::NUM_ERROR;
  }

  if ( m_roiPhi > M_PI ) { // ...it means that the convention is 0->2pi; change it to be -pi->+pi.
    m_roiPhi -= 2*M_PI;
  }

  if ( m_roiPhi < -M_PI ) { // some outher out of range error!!; change it to be -pi->+pi.
    m_roiPhi += 2*M_PI;
  }


  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST prepareROBs / event"
				// << IdEvent
				    << " RoI " << *roi  << endreq;
  if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC prepareROBs"
				   //<< " Event " << IdEvent
					   <<  " RoI " << *roi << std::endl;
   


  std::vector<unsigned int> uIntListOfRobs;
  m_regionSelector->DetROBIDListUint(PIXEL, uIntListOfRobs);

  config()->robRequestInfo()->addRequestScheduledRobIDs(uIntListOfRobs); 	
  uIntListOfRobs.clear();

  m_regionSelector->DetROBIDListUint(SCT, uIntListOfRobs);
  
  config()->robRequestInfo()->addRequestScheduledRobIDs(uIntListOfRobs); 	

  //  
  
  

  return HLT::OK; 
  
}

//-------------------------------------------------------------------------

HLT::ErrorCode TrigIDSCAN::hltExecute(const HLT::TriggerElement* inputTE,
				      HLT::TriggerElement* outputTE)
{
  m_ntracks = 0;
  m_recoTracks=NULL;
  m_dataErrors.clear();
  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();
  m_trtDataErrors.clear();
  m_a0beam.clear();
  m_trkdPhi0.clear();
  m_trkdEta.clear();

  m_pixResPhiBarrel.clear();
  m_pixResEtaBarrel.clear();
  m_pixPullPhiBarrel.clear();
  m_pixPullEtaBarrel.clear();
  m_sctResBarrel.clear();
  m_sctPullBarrel.clear();
  m_pixResPhiEC.clear();
  m_pixResEtaEC.clear();
  m_pixPullPhiEC.clear();
  m_pixPullEtaEC.clear();
  m_sctResEC.clear();
  m_sctPullEC.clear();

  m_nPixSPsInRoI=0;
  m_nSCTSPsInRoI=0;
  m_currentStage=0;
  m_nZvertices=0;
  m_zPosition=-9999.0;
  m_shift = 0;

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigIDSCAN::execHLTAlgorithm()" << endreq;

  //   Order of things:
  //   1. Get the external space points: new SpacePointReader
  //   2. Run the z-finder: zFinder->findZ
  //   3. Make internal space points: spacePoints->makeIdScanSPs
  //   4. Run hitFilter and groupCleaner: hitFilter->execute
  //   5. Run Kalman track Fitter: sctKFitter->execute or 
  //                               IdScanFitter->execute
  //   6. Create Trk::Tracks (optional)
  //   7. Propagate tracks through TRT (optional)
  //   8. store tracks in the TDS 
  //

  // get the run and event no.

  const EventInfo* pEventInfo;
  unsigned int IdRun=0;
  unsigned int IdEvent=0;
  if ( store()->retrieve(pEventInfo).isFailure() ) {

    if(m_detector_mask_not_checked && m_detector_mask_on_event) {
      msg() << MSG::ERROR << "Could not find EventInfo object for detector mask info" << endreq;
      return HLT::SG_ERROR;
    }
    // if we are not interested in the detector mask, this is a minor problem.
    if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;

  } else {

    IdRun   = pEventInfo->event_ID()->run_number();
    IdEvent = pEventInfo->event_ID()->event_number();
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;

    if(m_detector_mask_not_checked && m_detector_mask_on_event)
      setup_detector_mask(pEventInfo->event_ID());

  }

  std::vector< TrigSiSpacePoint* > commonSPs;
  commonSPs.clear();

  // Put all roi definition stuff in one place, rather than a bit here, and a bit there 
  // also consolodate wit hthe roi shape information, so that, getting it all from the 
  // roi descriptor would be prerefable

  bool useforID = false;

  const TrigRoiDescriptor* roi = NULL;
  //    if ( m_fullScanMode ) 
  if ( m_generateRoI ) 
    {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<  "Generate RoI Mode. Creating TrigRoiDescriptor." << endreq;
      //double eta = 0;
      //double phi = 0;
      // should set widths in roi as well - we can start to do this ourselves...
      // create the RoI with the actual parameters were required, crteating with TrigRoiDescriptor(false) 
      // just creates an RoI with no size, if we are creating an RoI asnd don;t know the size, it will have 
      // to be a full scan RoI 
      roi = new TrigRoiDescriptor(true);    
    }
  else 
    {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<  "inputTE->getId():  " <<  inputTE->getId() << endreq;
      if( getFeature(inputTE, roi, "forID") != HLT::OK || roi == 0 ) 
	{
	  //	    bool status1 = getFeature(inputTE, roi);
	  getFeature(inputTE, roi);
	}
      else useforID = true;
   
      if ( roi==NULL ) { 
	msg() <<  MSG::WARNING << "REGTEST / Failed to find RoiDescriptor " << endreq;
	return HLT::NAV_ERROR;
      }
    }

  /// why are we copying an RoiDescriptor? Why not just use the original ????
  TrigRoiDescriptor tmp_roi(*roi);

  m_roiEta = roi->eta();
  m_roiPhi = roi->phi();
  
  // a fullScan (in phi) roi should not be specified as phi = phi0 +- deltaphi with deltaphi = pi unless phi0 = 0
  if ( m_fullScanMode && roi->phi()!=0 ) { 
    m_roiPhi = 0;
    m_roiEta = 0;
    //msg() << MSG::WARNING << " FullScan input RoI not properly defined roi->phi()=" << roi->phi() 
    msg() << MSG::DEBUG << " FullScan input RoI not properly defined roi->phi()=" << roi->phi() 
	  << " will use roiPhi=" << m_roiPhi << endreq;
    //   return HLT::NUM_ERROR;
  }

  /// NB: all this is nonsense code now. The TrigRoiDescriptor properly 
  ///     contains all the shape information, so we should remove this 
  ///     uneccessary RoI_Shape class

  if(m_roiShape!=NULL) delete m_roiShape;

  if ( m_fullScanMode ) {
    m_roiShape = new TrigIdScan::RoI_Shape( 3, M_PI );
  }
  else { 

    if ( m_useRoiDescriptorWidths && useforID ) { 
      /// use widths from TrigRoiDescriptor
      // tmp_roi.etaHalfWidth(0.1);
      // tmp_roi.phiHalfWidth(0.1);
      // Note: assumption that RoI is symmetric

      double etaHalfWitt = 0.5*(tmp_roi.etaPlus()-tmp_roi.etaMinus());
      double phiHalfWitt = 0.5*HLT::wrapPhi(tmp_roi.phiPlus()-tmp_roi.phiMinus());
    
      m_roiShape = new TrigIdScan::RoI_Shape( etaHalfWitt, phiHalfWitt );
    }
    else {
      /// use values from configurable
      m_roiShape = new TrigIdScan::RoI_Shape( m_etaHalfWidth, m_phiHalfWidth ); 
    }
 
  }
  
  // check if the roi is properly defined

  if ( !( m_roiEta>-10 && m_roiEta<10 ) ) { 
    msg() << MSG::WARNING << " Input RoI not properly defined eta=" << m_roiEta << endreq;
    return HLT::NUM_ERROR;
  }
    
  if ( !( m_roiPhi>=-2*M_PI && m_roiPhi<=2*M_PI ) ) { 
    msg() << MSG::WARNING << " Input RoI not properly defined phi=" << m_roiPhi << endreq;
    return HLT::NUM_ERROR;
  }

  if ( m_roiPhi > M_PI ) { // ...it means that the convention is 0->2pi; change it to be -pi->+pi.
    m_roiPhi -= 2*M_PI;
  }

  if ( m_roiPhi < -M_PI ) { // some outher out of range error!!; change it to be -pi->+pi.
    m_roiPhi += 2*M_PI;
  }

  // so the SpacePointReader always takes -pi->+pi
    
  double roiPhiMin = m_roiPhi - m_roiShape->phiHalfWidth();
  double roiPhiMax = m_roiPhi + m_roiShape->phiHalfWidth();

  double roiEtaMin = roi->eta() - m_roiShape->etaHalfWidth();
  double roiEtaMax = roi->eta() + m_roiShape->etaHalfWidth();

  if ( m_fullScanMode ) { 
    roiPhiMin = -m_roiShape->phiHalfWidth();
    roiPhiMax =  m_roiShape->phiHalfWidth();

    roiEtaMin = -m_roiShape->etaHalfWidth();
    roiEtaMax =  m_roiShape->etaHalfWidth();
  }
  else { 
    if(roiPhiMin < -M_PI) roiPhiMin += 2*M_PI;
    if(roiPhiMax >  M_PI) roiPhiMax -= 2*M_PI;
  }      

  bool isBoundary=(roiPhiMin>roiPhiMax);
    
  // finished roi specification - this all seems a little complicated, but there you go 

  m_countTotalRoI++;
  
  //    std::cout << "TrigIDSCAN DBG " << name() << " " << IdEvent << " RoI id " << roi->roiId()  
  //	      << " fullScanMode=" << m_fullScanMode 
  //	      << " located at   roi->phi() = " <<  roi->phi()
  //	      << " (using phi=" << m_roiPhi << " phiMin=" << std::setprecision(10) << roiPhiMin << " phiMax=" << std::setprecision(10) << roiPhiMax << ")"
  //	      << " eta = " << roi->eta() << endl;
    
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / event " << IdEvent << " RoI " << *roi << endreq;
  if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " << " Event " << IdEvent << 
				   " RoI " << *roi << std::endl;
   

  //      if (m_fullScanMode || (roi->l1Id() != m_l1Id)) {
  if (m_fullScanMode || (IdEvent != m_l1Id)) {
    m_countTotalEvents++;
    //        m_l1Id = roi->l1Id();
      
    m_l1Id = IdEvent;
      
#ifdef IDSCANDUMP
      
    // write out the ntpl event header for the spacepoints
      
    if ( mWriteFlag ) { 
	
      // write out the last event before clearing the header
	
      //	  cout << *mHeader << endl;
	
      if ( mWriteCounter ) mTree->Fill();
      mHeader->FillHeader(IdRun, IdEvent);
      if ( mWriteTruth ) { 
	  
	// get the true vertex
	  
	TrigVertexCollection* zVertexColl=NULL;
	if ( store()->retrieve(zVertexColl, m_TrueVertexLocation).isFailure() ) {
	  msg() << MSG::DEBUG <<  "Failed to get true vertex for writing" << endreq;
	  mHeader->AddUserData(-999); // use a default value
	}
	else {
	  TrigVertex* trueVertex = zVertexColl->front();
	  double true_zvertex    = trueVertex->position().z();
	  mHeader->AddUserData( true_zvertex );
	}   	          
	  
      }
      ++mWriteCounter;
    }
#endif
      
  }

  m_currentStage = 1;
    
  if ( timerSvc() ) m_SpacePointReaderTimer->start();
    
  long nHits;
    
  StatusCode scOSP=StatusCode::SUCCESS;

  if(m_doShift) {

    scOSP=m_shifterTool->execute(m_SpacePointProvider, roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax, m_shift);

    if ( !( scOSP.isRecoverable() || scOSP.isSuccess() ) )
      { 
        msg() << MSG::WARNING << "ShifterTool failed " << endreq;
        if ( m_generateRoI ) { delete roi; roi=NULL; }
        return HLT::TOOL_FAILURE;
    }

    if (msgLvl() <= MSG::DEBUG) msg() <<  MSG::DEBUG << " translating spacepoints by (x,y)=" 
				      << m_shifterTool->get_xshift() << ",\t" <<  m_shifterTool->get_yshift() 
				      << endreq;
      
    nHits  = m_shifterTool->HitCount();
    std::vector<TrigSiSpacePoint*> spVec = m_shifterTool->spVec();
    std::vector<TrigSiSpacePoint*>::iterator spItr(spVec.begin());
  
    for ( int i=0 ; spItr!=spVec.end() ; ++i, ++spItr ) {
      if ( isPixel( (*spItr)->layer() ) )
	m_nPixSPsInRoI++;
    }
    m_nSCTSPsInRoI = nHits - m_nPixSPsInRoI;
  }
  else {
    std::vector<int> listOfPixIds;
    std::vector<int> listOfSctIds;

    scOSP=m_SpacePointProvider->fillCollections(true,true, listOfPixIds,listOfSctIds);
    if(scOSP.isRecoverable()||scOSP.isSuccess())
      {
	std::vector< TrigSiSpacePoint* > spVec;
	const TrigSiSpacePointContainer* pixCont;
	const TrigSiSpacePointContainer* sctCont;
	    
	StatusCode sc=store()->retrieve(pixCont,m_pixelSpName);
	if(sc.isFailure())
	  {
	    msg() << MSG::WARNING << "Trig SP Pixel container " <<m_pixelSpName
		  <<" not found"<<endreq; 
	    // if ( m_fullScanMode ) delete roi;
	    if ( m_generateRoI ) { delete roi; roi=NULL; }
	    return HLT::TOOL_FAILURE;
	  }
	sc=store()->retrieve(sctCont,m_sctSpName);
	if(sc.isFailure())
	  {
	    msg() << MSG::WARNING << "Trig SP SCT container " <<m_sctSpName
		  <<" not found"<<endreq; 
	    //  if ( m_fullScanMode ) delete roi;
	    if ( m_generateRoI ) { delete roi; roi=NULL; }
	    return HLT::TOOL_FAILURE;
	  }
	    
	double phi;
	long i=0;

	for(std::vector<int>::iterator idIt=listOfPixIds.begin();idIt!=listOfPixIds.end();++idIt)
	  {
	    TrigSiSpacePointContainer::const_iterator spCollIt=pixCont->indexFind((*idIt));
	    if(spCollIt==pixCont->end()) continue;
	    if((*spCollIt)==NULL) continue;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
		spIt!=(*spCollIt)->end();++spIt)
	      {
		phi = (*spIt)->original_phi();i++;
		if(msgLvl() <= MSG::VERBOSE) 
		  msg() << MSG::VERBOSE << " SP " << i << " phi " << phi << " roiphimin/max " 
			<< roiPhiMin << " / " << roiPhiMax << endreq; 
		if( 
		   //(m_useBeamSpot && m_iBeamCondSvc)||
		    (!isBoundary&&(phi > roiPhiMin && phi < roiPhiMax))
		    ||
		    (isBoundary&&(phi > roiPhiMin || phi < roiPhiMax)))
		  {
		    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " SP " << i << " phi " << phi << " roiphimin/max " 
							<< roiPhiMin << " / " << roiPhiMax << " in roi" << endreq; 
		    //msg() << MSG::VERBOSE << " SP accepted " << endreq;
		    commonSPs.push_back((*spIt));
		    m_nPixSPsInRoI++;
		  }
		//                else if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " SP rejected " << endreq;
	      }
	  }
	for(std::vector<int>::iterator idIt=listOfSctIds.begin();idIt!=listOfSctIds.end();++idIt)
	  {
	    TrigSiSpacePointContainer::const_iterator spCollIt=sctCont->indexFind((*idIt));
	    if(spCollIt==sctCont->end()) continue;
	    if((*spCollIt)==NULL) continue;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
		spIt!=(*spCollIt)->end();++spIt)
	      {
		phi = (*spIt)->original_phi();i++;
		if(msgLvl() <= MSG::VERBOSE) 
		  msg() << MSG::VERBOSE << " SP " << i << " phi " << phi << " roiphimin/max " 
			<< roiPhiMin << " / " << roiPhiMax << endreq; 
		if(
		   //(m_useBeamSpot && m_iBeamCondSvc)||
		   (!isBoundary&&(phi > roiPhiMin && phi < roiPhiMax))
		   ||
		   (isBoundary&&(phi > roiPhiMin || phi < roiPhiMax)))
		  {
		    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " SP " << i << " phi " << phi << " roiphimin/max " 
							<< roiPhiMin << " / " << roiPhiMax << " in roi" << endreq; 
		    //                    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " SP accepted " << endreq;
		    commonSPs.push_back((*spIt));
		  }
		else if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " SP rejected " << endreq;
	      }
	  }
      }
    nHits=commonSPs.size();
    m_nSCTSPsInRoI = nHits - m_nPixSPsInRoI;
  }

  if (Athena::Timeout::instance().reached()) {
    if(msgLvl()<=MSG::DEBUG)
      msg() << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);	        
  }

  if(scOSP.isRecoverable())
    {
      m_dataErrors.push_back(1);
      const std::vector<int>* errVect = m_SpacePointProvider->fillPixelDataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_pixelDataErrors));
      errVect = m_SpacePointProvider->fillSCT_DataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_sctDataErrors));
    }
  else if(scOSP.isFailure())
    {
      msg() << MSG::WARNING << "SpacePoint formation failed " << endreq; 
      m_dataErrors.push_back(0);
      if ( m_generateRoI ) { delete roi; roi=NULL; } 
      //  if ( m_fullScanMode ) delete roi;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD); 
    }

  if ( timerSvc() ) { 
    m_SpacePointReaderTimer->propVal(nHits);
    m_SpacePointReaderTimer->stop();
  }
    
  m_currentStage = 2;

  //    m_shifterTool->xShift(10);
  //    m_shifterTool->translateOnly(true);
  //    double d = m_shifterTool->execute( m_SpacePointProvider, roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax );
  //    std::cout << "shifter d = " << d << std::endl;
  //    std::vector<TrigSiSpacePoint*> spv = m_shifterTool->spVec();
  //    m_shifterTool->clear();
    


  //  disabled modules
  //   std::vector<IdentifierHash> hashlist;
  //   m_regionSelector->DetDisabledHashIDList(PIXEL, 0, -168, 168, roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax, &hashlist);
    
  int missing_layers = 0;

  // This needs optimization: how about Blayer? how about partially covered RoIs?
  //   But remember, we can still rely on the regionselector code just below...
  bool absentPixel = !( m_hasPixelDisk || m_hasPixelBarrel );
  bool absentSctA = !( m_hasSCTBarrelA || m_hasSCTEndcapA );
  bool absentSctC = !( m_hasSCTBarrelC || m_hasSCTEndcapC );

  if ( absentPixel  ||  ( absentSctA && absentSctC )  ||
       ( roiEtaMin>0 && absentSctC )  ||   // RoI falls fully onto absent detector
       ( roiEtaMax<0 && absentSctA ) )  {

    msg() << MSG::DEBUG << " reducing layer threshold due to detector mask" << endreq;
    missing_layers = 1;
  }

  else if ( m_adjustLayerThreshold ) {

    // get layers with disabled modules from region selector
    // sct and pixel subdetectors, are 
    //  0: barrel
    // -2: negative endcap
    //  2: positive endcap
    // layers start from 0 in each subdetector, 
    //    ie pixel barrel: 0, 1, 2; sct barrel: 0, 1, 2, 3; sct endcap 0, 1, 2, 3, 4, ...
    // alternatively, use the method that doesn't require subdetector
    // but takes three vectors, these return the layer list for the barrel,
    // endcap A and endcap C respectively    

    // RegSelSvc needs a zmin, zmax value, these should be defined by the roi, 
    // or default values used. The proper default values are stored in the 
    // OnlineSpacepointProvider, so they should be obtained by that, but we 
    // am in a rush, so this will have to be done later 

    // pixel layers with disabled modules   
    std::vector<int>  pixellayers[3];
    std::vector<int>  sctlayers[3];

    //RegSelSvc* regionSelector = dynamic_cast<RegSelSvc*>(m_regionSelector);
    //IRegSelSvc* regionSelector = m_regionSelector;

    //    m_regionSelector->getDisabledLayers(PIXEL, -168, 168, roiEtaMin, roiEtaMax,
    //regionSelector->getDisabledLayers(PIXEL, m_minZ, m_maxZ, roiEtaMin, roiEtaMax,
		//			roiPhiMin, roiPhiMax, 
		//			pixellayers[0], pixellayers[1], pixellayers[2]);

    // sct layers with disabled modules   
    // m_regionSelector->getDisabledLayers(SCT, -168, 168, roiEtaMin, roiEtaMax,
    //regionSelector->getDisabledLayers(SCT, m_minZ, m_maxZ, roiEtaMin, roiEtaMax,
		//			roiPhiMin, roiPhiMax, 
		//			sctlayers[0], sctlayers[1], sctlayers[2]);

    if (msgLvl() <= MSG::VERBOSE) { 

      msg() << MSG::DEBUG << " layers with disabled modules from the RegSelSvc:" << endreq;           
		
      static std::string      subdet[3] = { "barrel  ",  "endcap A", "endcap C" }; 
      static std::string      det[2]    = { "pixel",     "sct  " };
      std::vector<int>*       layers[2] = {  pixellayers, sctlayers };
	
      for ( int idet=0 ; idet<2 ; idet++ ) { 
	msg() << MSG::VERBOSE << det[idet] << " layer information" << endreq;           
	for ( int isub=0 ; isub<3 ; isub++ ) {
	  msg() << MSG::VERBOSE << subdet[isub] << " " << layers[idet][isub].size() << " layers with disabled modules" << endreq;
	  for ( int ilayer=layers[idet][isub].size() ; ilayer-- ; ) { 
	    msg() << MSG::VERBOSE << "\tdisabled modules in layer " << layers[idet][isub][ilayer] << endreq;
	  }
	}
      }
    }


    // need a proper, genuine set of possible combinations here, 
    // including some from the sct
    if ( pixellayers[0].size()>1 || 
	 pixellayers[1].size()>1 || 
	 pixellayers[2].size()>1 || 
	 (pixellayers[0].size()+pixellayers[1].size())>1 || 
	 (pixellayers[0].size()+pixellayers[2].size())>1 ) { 
	
      msg() << MSG::DEBUG << " reducing layer threshold " << endreq;           
	
      //      double threshold = dynamic_cast<IDScanHitFilter*>(m_hitFilter)->getLayerThreshold();
      //   dynamic_cast<IDScanHitFilter*>(m_hitFilter)->setLayerThreshold(threshold-1);
      missing_layers = 1;
    }
  }


#if 0

  for ( unsigned i=0 ; i<10 && i<spv.size() ; i++ ) {
      
    std::cout << i << " \tx=" << spv[i]->x() << ", " << spv[i]->y();
      
    TrigSiSpacePoint* sp = m_shifterTool->mapPoint(spv[i]);
      
    if ( sp ) std::cout << "\t x(unshifted)=" << sp->x() << ", " << sp->y() << std::endl;
    else      std::cout << "\t NOT IN MAP!" << std::endl;
      
  }
#endif    



#ifdef IDSCANDUMP
  // write out the spacepoints if required

  if ( mWriteFlag ) { // new

    vector<double> roi_userdata;
    //      if (m_doShift) roi_userdata.push_back(spacePoints->get_xshift());
    if (m_doShift) roi_userdata.push_back(m_shifterTool->get_xshift());

    if (m_doShift)   mHeader->FillRoI( roi->eta(), roi->phi(), 
				       m_roiShape->etaHalfWidth(), m_roiShape->phiHalfWidth(),  
				       m_shifterTool->spVec(), roi_userdata);
    //					 spacePoints->spVec(), roi_userdata);
    else    {
      std::vector<int> vBar;
      m_TrigL2SpacePointTruthTool->getBarCodes(commonSPs,vBar);
      mHeader->FillRoI( roi->eta(), roi->phi(), 
			m_roiShape->etaHalfWidth(), m_roiShape->phiHalfWidth(),  
			commonSPs,vBar);
    }
  }
#endif
   
  if( nHits >= m_minHits ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / Found " << nHits 
				      << " hits. Proceeding to zFinder." << endreq;
    m_countRoIwithEnoughHits++;
  }
  else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
				      << "Too few hits in ROI for IdScan to run. Skipping event. " << nHits
				      << endreq;
    if ( m_generateRoI ) { delete roi; roi=NULL; }
    // if ( m_fullScanMode ) delete roi;
    return HLT::OK;
  }
  
  unsigned int zVertexFailOrSucceed=0;
  TrigVertexCollection* zVertexColl=NULL;

  double trueZ0(-9999.0);
  std::vector<double> zReco;

  if ( timerSvc() ) m_ZFinderTimer->start();

  if ( m_zFinderMode == 1 ){
    if ( store()->retrieve(zVertexColl, m_TrueVertexLocation).isFailure() ) {
      if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Failed to get trueZvCollection" << endreq;
    } else {
      TrigVertex* trueVertex = zVertexColl->front();
      trueZ0 = (trueVertex->position()).z();
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " True z position " << trueZ0 << endreq;
    }
  } else {

    if(m_doShift)
      //        zVertexColl = m_zFinder->findZ( spacePoints->spVec(), (TrigRoiDescriptor*)roi );
      zVertexColl = m_zFinder->findZ( m_shifterTool->spVec(), *roi );
    else
      zVertexColl = m_zFinder->findZ( commonSPs, *roi );
    
    if (zVertexColl!=NULL) {
      if (attachFeature(outputTE, zVertexColl, m_attachedFeatureName) != HLT::OK) {
	msg()  << MSG::WARNING << "Failed to store ZvCollection" << endreq;
      }
    }
  }

  if ( zVertexColl )
    for(TrigVertexCollection::iterator ivx=zVertexColl->begin(); ivx != zVertexColl->end(); ivx++)
      zReco.push_back( (*ivx)->position().z() );

  if ( timerSvc() ) { 
    m_ZFinderTimer->propVal( nHits );
    m_ZFinderTimer->stop();
  }

  if ( m_doShift && m_useShifterZ>0 ) {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "z-position from the shifter = " << m_shifterTool->get_shifterz() << endreq;

    if ( m_useShifterZ==1 ) zReco.clear(); // Overwrite the zfinder results

    if ( m_shifterTool->get_shifterz() != 0 )
      zReco.push_back( m_shifterTool->get_shifterz() );
  }

  m_nZvertices = zReco.size();
  if (m_nZvertices == 0) zReco.push_back(-9999.0);

  m_zPosition = zReco[0]; 
  if (m_zPosition == -9999.0) { 
    m_countRoIzFinderFailed++;
    zVertexFailOrSucceed=1;
  }

  if (Athena::Timeout::instance().reached()) {
    if(msgLvl()<=MSG::DEBUG)
      msg() << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);	        
  }

  m_currentStage = 3;

  // Extract the beamspot shift
  double shiftx(0), shifty(0);
  if (!m_doShift && m_useBeamSpot && m_iBeamCondSvc) {
    Amg::Vector3D vertex = m_iBeamCondSvc->beamPos();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Beam spot position " << vertex << endreq;
    double xVTX = vertex.x();
    double yVTX = vertex.y();
    double zVTX = vertex.z();
    double tiltXZ = m_iBeamCondSvc->beamTilt(0);
    double tiltYZ = m_iBeamCondSvc->beamTilt(1);
    shiftx = xVTX - tiltXZ*zVTX;//correction for tilt
    shifty = yVTX - tiltYZ*zVTX;//correction for tilt

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Center position:  " << shiftx <<"  "<< shifty << endreq;
  }

  m_recoTracks=NULL;
  m_recoTracks = new TrigInDetTrackCollection;
  m_placeHolder.clear();

  for ( int iz=0; iz<m_nZvertices; ++iz )  {
    if ( m_zFinderMode == 0 ) m_zPosition = zReco[iz]; 
    if ( m_zPosition != -9999.0 ) 
      {
	m_currentStage = 4;

	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / z found = " 
					  << m_zPosition << endreq;
	if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " 
						 <<  " z found = " << m_zPosition << std::endl;

	if ( timerSvc() ) m_HitFilterTimer->resume();
	if(m_doShift) {
	  //m_hitFilter->findTracks( spacePoints->spVec(), *m_recoTracks, roi, m_zPosition, spacePoints->get_xshift());
	  m_hitFilter->findTracks( m_shifterTool->spVec(), *m_recoTracks, roi, m_zPosition, 
			                               m_shifterTool->get_xshift(), 0, missing_layers );
	}
	else{ 
	  m_hitFilter->findTracks(commonSPs, *m_recoTracks, &tmp_roi, m_zPosition, shiftx, shifty, missing_layers );
	}
	if ( timerSvc() ) {
	  m_HitFilterTimer->pause();
	}
      }
    m_placeHolder.push_back(m_recoTracks->size());
    if ((!m_recoTracks->empty() && m_findMultipleZ == false) || m_recoTracks->size() > m_maxNumTracks)  {
      HLT::ErrorCode storeCode1 = this->storeInfo( m_zPosition, outputTE );
      if (storeCode1 != HLT::OK){
        msg() << MSG::ERROR << "zinfo Write into outputTE failed" << endreq;
        return storeCode1;
      }
      m_currentStage = 5;
      break;
    }
  }
  if (m_findMultipleZ == true && !m_recoTracks->empty())  {
    HLT::ErrorCode storeCode2 = this->storeInfo( m_zPosition, outputTE );
    if (storeCode2 != HLT::OK){
      msg() << MSG::ERROR << "zinfo Write into outputTE failed" << endreq;
      return storeCode2;
    }
    m_currentStage = 5;
  }
      
  unsigned int nZverticesHF = m_placeHolder.size();

  if ( timerSvc() ) {
    if (!m_recoTracks->empty()) 
      m_HitFilterTimer->propVal( m_recoTracks->size() );
    else m_HitFilterTimer->propVal(0);
    m_HitFilterTimer->stop();
  }

  if (Athena::Timeout::instance().reached()) {
    if(msgLvl()<=MSG::DEBUG)
      msg() << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);	        
  }
  
  if ( m_recoTracks->empty() ) 
    {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / No groups found." << endreq;
      if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " <<  " No groups found." << std::endl;
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / No tracks reconstructed" << endreq;
      if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " << " No tracks reconstructed" << std::endl;      
      //     StatusCode sc = m_EventData2XML->execute();
      delete m_recoTracks;
      m_recoTracks=NULL;
    }
  else 
    {


      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
					<< "REGTEST / Made " << m_recoTracks->size() 
					<< " IdScanGroups. Proceeding to track fitter." << endreq;
      if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " <<  " Made " << m_recoTracks->size() 
					       << " IdScanGroups. Proceeding to track fitter." << std::endl;
      m_countRoIwithGroups[zVertexFailOrSucceed]++;
      if ( timerSvc() ) m_SctKFitterTimer->start();
      m_TrigInDetTrackFitter->fit( m_recoTracks);

      //deleting tracks with very high values of chi2 (indicating failed Kalman filter extrapolation) 
      TrigInDetTrack * deleteMe;
      for(unsigned int trackNum=0; trackNum<m_recoTracks->size();){
        if(m_recoTracks->at(trackNum)->chi2() >= 1e8){
          m_recoTracks->swapElement(trackNum, 0, deleteMe);
	  m_recoTracks->erase(m_recoTracks->begin() + trackNum);
	  delete deleteMe;
	  deleteMe = NULL;
	  for(unsigned int l = 0;l<m_placeHolder.size();l++){
	    if(trackNum < m_placeHolder[l]) m_placeHolder[l] -= 1;
	  }
	}
	else trackNum +=1;
      }


      //Deleting duplicate tracks from multiple z vertices 
      if(m_findMultipleZ == true && nZverticesHF > 1){
        unsigned int trackNum2(0), numPointsShared(0);
	std::vector<const TrigSiSpacePoint*>* trackPoints1;
	std::vector<const TrigSiSpacePoint*>* trackPoints2;
	for(int trackNum1=0; trackNum1 < int (m_placeHolder[nZverticesHF - 1]); trackNum1++){
          //only loop through tracks from other vertices 
          for(unsigned int i=0;i<m_placeHolder.size();i++){
	    if(trackNum1 < int (m_placeHolder[i])){
	      trackNum2 = m_placeHolder[i];
              break;
	    } 
	  }
	  trackPoints1 = m_recoTracks->at(trackNum1)->siSpacePoints();
	  for(; trackNum2 < m_recoTracks->size(); trackNum2++){
	    trackPoints2 = m_recoTracks->at(trackNum2)->siSpacePoints();
	    numPointsShared = 0;
	    //check if the two tracks share any spacpoints
	    for(unsigned int k=0;k<trackPoints1->size(); k++){
	      for(unsigned int j=0;j < trackPoints2->size(); j++){
	        if(trackPoints1->at(k) == trackPoints2->at(j)){
	          numPointsShared +=1; 
	        }
	      }
              if(numPointsShared > 2) break;
	    }
	    if(numPointsShared > 2){
	      //delete one of the duplicate tracks, keeping the one with more spacepoints or a lower chi2
	      if(m_recoTracks->at(trackNum1)->NPixelSpacePoints() > m_recoTracks->at(trackNum2)->NPixelSpacePoints()){
	        m_recoTracks->swapElement(trackNum2, 0, deleteMe);
	        m_recoTracks->erase(m_recoTracks->begin() + trackNum2);
		delete deleteMe;
		deleteMe = NULL;
              }else if(m_recoTracks->at(trackNum2)->NPixelSpacePoints() > m_recoTracks->at(trackNum1)->NPixelSpacePoints()){
	        m_recoTracks->swapElement(trackNum1, 0, deleteMe);
		m_recoTracks->erase(m_recoTracks->begin() + trackNum1);
		delete deleteMe;
		deleteMe = NULL;
		for(unsigned int l = 0;l<m_placeHolder.size();l++){
		  if(trackNum1 < int (m_placeHolder[l])) m_placeHolder[l] -= 1;
		}
		trackNum1 -=1;
		break;
	      }else if(m_recoTracks->at(trackNum1)->NSCT_SpacePoints() > m_recoTracks->at(trackNum2)->NSCT_SpacePoints()){
	        m_recoTracks->swapElement(trackNum2, 0, deleteMe);
		m_recoTracks->erase(m_recoTracks->begin() + trackNum2);
		delete deleteMe;
		deleteMe = NULL;
	      }else if(m_recoTracks->at(trackNum2)->NSCT_SpacePoints() > m_recoTracks->at(trackNum1)->NSCT_SpacePoints()){
		m_recoTracks->swapElement(trackNum1, 0, deleteMe);
		m_recoTracks->erase(m_recoTracks->begin() + trackNum1);
		delete deleteMe;
		deleteMe = NULL;
		for(unsigned int l = 0;l<m_placeHolder.size();l++){
		  if(trackNum1 < int (m_placeHolder[l])) m_placeHolder[l] -= 1;
		}
		trackNum1 -=1;
		break;
	      }else if(m_recoTracks->at(trackNum1)->chi2() < m_recoTracks->at(trackNum2)->chi2()){
		m_recoTracks->swapElement(trackNum2, 0, deleteMe);
		m_recoTracks->erase(m_recoTracks->begin() + trackNum2);
		delete deleteMe;
		deleteMe = NULL;
	      }else{
		m_recoTracks->swapElement(trackNum1, 0, deleteMe);
		m_recoTracks->erase(m_recoTracks->begin() + trackNum1);
		delete deleteMe;
		deleteMe = NULL;
		for(unsigned int l = 0;l<m_placeHolder.size();l++){
		  if(trackNum1 < int (m_placeHolder[l])) m_placeHolder[l] -= 1;
		}
		trackNum1 -=1;
		break;
	      }
	      for(unsigned int l = 0;l<m_placeHolder.size();l++){
	        if(trackNum2 < m_placeHolder[l]) m_placeHolder[l] -= 1;
              }
	      trackNum2 -=1;
	    }
	  }
	}
      }
      
      //monitor number of tracks
      m_ntracks=m_recoTracks->size();

      ///////////// fill vectors of quantities to be monitored
      // a0 wrt beam spot
      for(TrigInDetTrackCollection::iterator trackIt = m_recoTracks->begin();trackIt != m_recoTracks->end();++trackIt)
	{
	  double a0 = (*trackIt)->param()->a0();
	  double phi0 = (*trackIt)->param()->phi0();
	  m_a0beam.push_back(a0+shiftx*sin(phi0)-shifty*cos(phi0));
	  double dPhi0 = phi0 - m_roiPhi;
	  if ( dPhi0> 2*M_PI ) dPhi0 -= 2*M_PI;
	  if ( dPhi0<-2*M_PI ) dPhi0 += 2*M_PI;
	  m_trkdPhi0.push_back(dPhi0);
	  m_trkdEta.push_back((*trackIt)->param()->eta() - m_roiEta);
	  //zVTX = m_zPosition; // precise calculation using vertex z, not the beamspot cond service - not needed
	}

      // hit residuals
      if (m_doResMonitoring)
	{
	  std::vector<TrigL2HitResidual> vResid;
	  vResid.clear();
	  for(TrigInDetTrackCollection::iterator trackIt = m_recoTracks->begin();trackIt != m_recoTracks->end();++trackIt)
	    {
	      // <---- add track preselection here : pT, nHits, etc.
	      //bool goodTrack = (*trackIt)->param()->pT()>1000. && ((*trackIt)->NPixelSpacePoints()+(*trackIt)->NSCT_SpacePoints())>2;
	      // tighter selection for unbiased residuals
	      bool goodTrack = std::fabs((*trackIt)->param()->pT())>1000. && ((*trackIt)->NPixelSpacePoints()+(*trackIt)->NSCT_SpacePoints())>3 && (*trackIt)->NSCT_SpacePoints()>0 ;
	      if(!goodTrack) continue;
	      //StatusCode scRes = m_TrigL2ResidualCalculator->getResiduals((*trackIt),vResid);
	      StatusCode scRes = m_TrigL2ResidualCalculator->getUnbiassedResiduals((*trackIt),vResid);
	      if(scRes.isSuccess())
		{
		  for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it)
		    {
		      //msg() << MSG::INFO << (*it) << endreq;
		      if (it->regionId()==1) 
			{
			  m_pixResPhiBarrel.push_back(it->phiResidual());
			  m_pixPullPhiBarrel.push_back(it->phiPull());
			  m_pixResEtaBarrel.push_back(it->etaResidual());
			  m_pixPullEtaBarrel.push_back(it->etaPull());
			}
		      if (it->regionId()==2)
			{
			  m_pixResPhiEC.push_back(it->phiResidual());
			  m_pixPullPhiEC.push_back(it->phiPull());
			  m_pixResEtaEC.push_back(it->etaResidual());
			  m_pixPullEtaEC.push_back(it->etaPull());
			}
		      if (it->regionId()==3) 
			{
			  m_sctResBarrel.push_back(it->phiResidual());
			  m_sctPullBarrel.push_back(it->phiPull());
			}
		      if (it->regionId()==4) 
			{
			  m_sctResEC.push_back(it->phiResidual());
			  m_sctPullEC.push_back(it->phiPull());
			}
		    } // loop over residuals
		}
	    } // loop over tracks
	} // if doResMonitoring
      
      if ( timerSvc() ) 
	{ 
	  m_SctKFitterTimer->propVal( m_recoTracks->size() );
	  m_SctKFitterTimer->stop();
	}
      m_countRoIwithTracks[zVertexFailOrSucceed]++;
      m_currentStage = 6;
      if(m_recoTracks->size() > m_maxNumTracks) m_doTRTpropagation = false;
      if(m_doTRTpropagation)
	{
	  if ( timerSvc() ) m_IdScanTRTTimer->start();
	  StatusCode sc=m_TrigTRT_TrackExtensionTool->propagate(m_recoTracks);
	  if ( timerSvc() ) 
	    { 
	      m_IdScanTRTTimer->propVal(m_recoTracks->size() );
	      m_IdScanTRTTimer->stop();
	    }
	  if(sc.isRecoverable())
	    {
	      m_dataErrors.push_back(3);
	      const std::vector<int>* errVect = m_TrigTRT_TrackExtensionTool->fillTRT_DataErrors();
	      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
	    }
	  else if(sc.isFailure())
	    {
	      msg() << MSG::WARNING <<"Bad TRT data"<<endreq;
	      m_dataErrors.push_back(2);
	      if ( m_generateRoI ) { delete roi; roi=NULL; }
	      //  if ( m_fullScanMode ) delete roi;
	      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
	    }
	}
      m_currentStage = 7; 

      if ( msgLvl() <= MSG::DEBUG || m_printDiagnosticMessages) 
	{ 
	  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST / Reconstructed " << m_recoTracks->size() << " tracks " << endreq;
	  if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " << " Reconstructed " << m_recoTracks->size() << " tracks " << std::endl;
	  TrigInDetTrackCollection::iterator track = m_recoTracks->begin();
	  TrigInDetTrackCollection::iterator lastTrack = m_recoTracks->end();
	    
	  for(; track !=lastTrack; track++)
	      
	    {
		
	      msg() << MSG::DEBUG << "REGTEST / track nPixelSPs/nSCT_SPs/nstraw/ntr/phi0/pt/eta/d0/z0/chi2: " <<
		(*track)->NPixelSpacePoints() << " / "  << 
		(*track)->NSCT_SpacePoints() << " / "  << 
		(*track)->NStrawHits() << " / "  << 
		(*track)->NTRHits() << " / "  << 
		(*track)->param()->phi0() << " / "  << 
		(*track)->param()->pT() << " / " << 
		(*track)->param()->eta() << " / " << 
		(*track)->param()->a0() << " / " <<
		(*track)->param()->z0() << " / " <<
		(*track)->chi2() << endreq;
	    
	      if (m_printDiagnosticMessages) std::cout << "TrigIDSCAN DIAGNOSTIC " << " track nPixelSPs/nSCT_SPs/nstraw/ntr/phi0/pt/eta/d0/z0/chi2: " <<
		      			       (*track)->NPixelSpacePoints() << " / "  << 
				       	       (*track)->NSCT_SpacePoints() << " / "  << 
					       (*track)->NStrawHits() << " / "  << 
					       (*track)->NTRHits() << " / "  << 
					       (*track)->param()->phi0() << " / "  << 
					       (*track)->param()->pT() << " / " << 
					       (*track)->param()->eta() << " / " << 
					       (*track)->param()->a0() << " / " <<
					       (*track)->param()->z0() << " / " <<
					       (*track)->chi2() << std::endl;
	    }
	}

      // Store RoI ID
      //        if (m_fullScanMode) {
      if (m_generateRoI) {
	m_recoTracks->RoI_ID (-2);
      } else {
	m_recoTracks->RoI_ID (roi->roiId());
      }

      HLT::ErrorCode code = attachFeature(outputTE, m_recoTracks, m_attachedFeatureName);

      if ( code != HLT::OK ) {
	msg() << MSG::ERROR << "REGTEST/ Write into outputTE failed" << endreq;
	// if ( m_fullScanMode ) delete roi;
	if ( m_generateRoI ) { delete roi; roi=NULL; }
	return code;
      }
      
      if ( m_trackConvertorMode==1 ) {
	DataVector<Trk::Track>* newTracks = new DataVector<Trk::Track>;
	m_convertorTool->execute(m_recoTracks,newTracks);
      
	StatusCode sc=store()->record(newTracks,"IDSCAN_Tracks",false);
	if(sc.isFailure())
	  {
	    msg() << MSG::ERROR << "REGTEST/ Could not save converted IDSCAN tracks!!! " <<endreq;
	  return HLT::SG_ERROR;
	  }
      }
    
    }
  // We don't need to clear.
  // m_shifterTool->execute() cleans itself everytime it is called.
  // commonSPs goes away when we are out of scope.
  //if(m_doShift) m_shifterTool->clear(); // delete spacePoints;
  //else commonSPs.clear();

  //    if ( m_fullScanMode ) delete roi;
  if ( m_generateRoI ) { delete roi; roi=NULL; }
  return HLT::OK;
}

//---------------------------------------------------------------------------

HLT::ErrorCode TrigIDSCAN::hltFinalize()
{
#ifdef IDSCANDUMP
  // finish up ntpl  
  if ( mWriteFlag ) {
    // write out last event
    if ( mWriteCounter ) mTree->Fill(); 
    mFile->Write();
    mFile->Close();
  }
#endif
  
  msg() << MSG::INFO << "=========================================================" << endreq;
  msg() << MSG::INFO << "TrigIDSCAN::finalize() - TrigIDSCAN Statistics: " << endreq;
  msg() << MSG::INFO << "Events processed: " <<  m_countTotalEvents << endreq;
  msg() << MSG::INFO << "RoI processed: " <<  m_countTotalRoI << endreq;
  msg() << MSG::INFO << "RoI with enough SP Total/goodZvertex/badZvertex: " <<  m_countRoIwithEnoughHits << "/" <<
    m_countRoIwithEnoughHits - m_countRoIzFinderFailed << "/" << m_countRoIzFinderFailed << endreq;
  msg() << MSG::INFO << "RoI with Group(s)  Total/goodZvertex/badZvertex: " <<  
    m_countRoIwithGroups[0]+m_countRoIwithGroups[1] << "/" <<
    m_countRoIwithGroups[0] << "/" << m_countRoIwithGroups[1] << endreq;
  msg() << MSG::INFO << "RoI with Track(s)  Total/goodZvertex/badZvertex: " <<  
    m_countRoIwithTracks[0]+m_countRoIwithTracks[1] << "/" <<
    m_countRoIwithTracks[0] << "/" << m_countRoIwithTracks[1] << endreq;
  msg() << MSG::INFO << "=========================================================" << endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------

HLT::ErrorCode TrigIDSCAN::storeInfo(double zPosition, HLT::TriggerElement* outputTE)
{
  TrigVertexCollection* zinfo = new TrigVertexCollection();
  TrigVertex* zvert = new TrigVertex( zPosition );
  zinfo->push_back(zvert);

  HLT::ErrorCode code = attachFeature(outputTE, zinfo, m_attachedFeatureName);
  return code;
}

