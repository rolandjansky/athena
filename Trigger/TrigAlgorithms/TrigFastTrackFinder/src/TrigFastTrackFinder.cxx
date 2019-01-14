/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// TrigFastTrackFinder.cxx
// -------------------------------
// ATLAS Collaboration
//
// package created 16/04/2013 by Dmitry Emeliyanov (see ChangeLog for more details)
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h" 

#include "TrkParameters/TrackParameters.h" 
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackInfo.h" 

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "TrigInDetEvent/TrigSiSpacePointBase.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrigInDetPattRecoEvent/TrigL2TimeoutException.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"


#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigZFinder.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "src/TrigFastTrackFinder.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigFastTrackFinder::TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) : 

  HLT::FexAlgo(name, pSvcLocator), 
  m_numberingTool("TrigL2LayerNumberTool"), 
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_TrigL2SpacePointTruthTool("TrigL2SpacePointTruthTool"),
  m_trigL2ResidualCalculator("TrigL2ResidualCalculator"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigZFinder("TrigZFinder"),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_ftkDataProviderSvc("TrigFTK_DataProviderSvc",name),
  m_shift_x(0.0),
  m_shift_y(0.0),
  m_doCloneRemoval(true),
  m_ftkMode(false),
  m_ftkRefit(false),
  m_useBeamSpot(true),
  m_nfreeCut(5), 
  m_nTracks(0),
  m_nPixSPsInRoI(0),
  m_nSCTSPsInRoI(0),
  m_currentStage(-1),
  m_SpacePointConversionTimer(nullptr),
  m_ZFinderTimer(nullptr),
  m_PatternRecoTimer(nullptr), 
  m_TripletMakingTimer(nullptr), 
  m_CombTrackingTimer(nullptr), 
  m_TrackFitterTimer(nullptr), 
  m_attachedFeatureName(""),
  m_outputCollectionSuffix(""),
  m_countTotalRoI(0),
  m_countRoIwithEnoughHits(0),
  m_countRoIwithTracks(0),
  m_nSignalPresent(0),
  m_nSignalDetected(0),
  m_nSignalTracked(0),
  m_nSignalClones(0),
  m_minSignalSPs(0),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_particleHypothesis(Trk::pion),
  m_useNewLayerNumberScheme(false)
{

  /** Doublet finding properties. */
  declareProperty("Doublet_FilterRZ",            m_tcs.m_doubletFilterRZ = true);
  declareProperty("DoubletDR_Max",            m_tcs.m_doublet_dR_Max = 270.0);
  declareProperty("SeedRadBinWidth",            m_tcs.m_seedRadBinWidth = 2.0);

  /** Triplet finding properties. */

  declareProperty("Triplet_D0Max",            m_tcs.m_tripletD0Max      = 4.0);
  declareProperty("Triplet_D0_PPS_Max",       m_tcs.m_tripletD0_PPS_Max = 1.7);
  declareProperty("Triplet_nMaxPhiSlice",     m_tcs.m_nMaxPhiSlice = 53);
  declareProperty("Triplet_MaxBufferLength",     m_tcs.m_maxTripletBufferLength = 3);
  declareProperty("TripletDoPSS",            m_tcs.m_tripletDoPSS = false);

  declareProperty("Triplet_DtCut",            m_tcs.m_tripletDtCut      = 10.0);//i.e. 10*sigma_MS

  declareProperty( "VertexSeededMode",    m_vertexSeededMode = false);
  declareProperty( "doZFinder",           m_doZFinder = true);
   declareProperty( "doFTKZFinder",           m_doFTKZFinder = false);
   declareProperty( "doFTKFastVtxFinder",           m_doFTKFastVtxFinder = false);

  declareProperty( "doFastZVertexSeeding",           m_doFastZVseeding = true);
  declareProperty( "zVertexResolution",           m_tcs.m_zvError = 10.0);

  declareProperty("Triplet_MinPtFrac",        m_tripletMinPtFrac = 0.3);
  declareProperty("pTmin",                    m_pTmin = 1000.0);
  declareProperty("TrackInitialD0Max",            m_initialD0Max      = 10.0);
  declareProperty("TrackZ0Max",                   m_Z0Max      = 300.0);

  declareProperty("doSeedRedundancyCheck",            m_checkSeedRedundancy = false);

  declareProperty( "MinHits",               m_minHits = 5 );

  declareProperty( "OutputCollectionSuffix",m_outputCollectionSuffix = "");

  declareProperty("TracksName", 
                  m_outputTracksKey = std::string("TrigFastTrackFinder_Tracks"),
                  "TrackCollection name");

  declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
 
  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);
  declareProperty( "FreeClustersCut"   ,m_nfreeCut      );
  declareProperty( "SpacePointProviderTool", m_spacePointTool  );
  declareProperty( "LayerNumberTool", m_numberingTool  );

  declareProperty( "initialTrackMaker", m_trackMaker);
  declareProperty( "trigInDetTrackFitter",   m_trigInDetTrackFitter );
  declareProperty( "trigZFinder",   m_trigZFinder );

  declareProperty("TrackSummaryTool", m_trackSummaryTool);
  declareProperty( "TrigL2SpacePointTruthTool", m_TrigL2SpacePointTruthTool);
  declareProperty( "retrieveBarCodes", m_retrieveBarCodes = false);
  declareProperty( "SignalBarCodes", m_vSignalBarCodes);
  declareProperty( "MinSignalSPs", m_minSignalSPs = 3);
  declareProperty( "doResMon",       m_doResMonitoring = true);

  declareProperty("doCloneRemoval", m_doCloneRemoval = true);

  declareProperty("FTK_Mode",            m_ftkMode = false);
  declareProperty("FTK_DataProviderService",             m_ftkDataProviderSvc);
  declareProperty("FTK_Refit",           m_ftkRefit = false);

  declareProperty("useNewLayerNumberScheme", m_useNewLayerNumberScheme = false);

  // declare monitoring histograms

  declareMonitoredStdContainer("trk_pt",         m_trk_pt);
  declareMonitoredStdContainer("trk_a0",         m_trk_a0);
  declareMonitoredStdContainer("trk_z0",         m_trk_z0);
  declareMonitoredStdContainer("trk_phi0",       m_trk_phi0);
  declareMonitoredStdContainer("trk_eta",        m_trk_eta);
  declareMonitoredStdContainer("trk_chi2dof",    m_trk_chi2dof);
  declareMonitoredStdContainer("trk_nSiHits",    m_trk_nSiHits);
  declareMonitoredStdContainer("trk_nPIXHits",   m_trk_nPIXHits);
  declareMonitoredStdContainer("trk_nSCTHits",   m_trk_nSCTHits);
  declareMonitoredStdContainer("trk_a0beam",     m_trk_a0beam);
  declareMonitoredStdContainer("trk_dPhi0",      m_trk_dPhi0);
  declareMonitoredStdContainer("trk_dEta" ,      m_trk_dEta);

  declareMonitoredVariable("roi_nSeeds",m_nSeeds);
  declareMonitoredVariable("roi_nTracks",m_nTracks);
  declareMonitoredVariable("roi_nSPsPIX",m_nPixSPsInRoI);
  declareMonitoredVariable("roi_nSPsSCT",m_nSCTSPsInRoI);
  declareMonitoredVariable("roi_lastStageExecuted",m_currentStage);
  declareMonitoredVariable("roi_eta", m_roiEta);
  declareMonitoredVariable("roi_etaWidth", m_roiEtaWidth);
  declareMonitoredVariable("roi_phi", m_roiPhi);
  declareMonitoredVariable("roi_phiWidth", m_roiPhiWidth);
  declareMonitoredVariable("roi_z", m_roiZ);
  declareMonitoredVariable("roi_zWidth", m_roiZ_Width);
  declareMonitoredVariable("roi_nSPs", m_roi_nSPs);
  declareMonitoredVariable("time_PattRecoOnly",m_timePattReco);

  // Z-vertexing 
  declareMonitoredVariable("roi_nZvertices",m_nZvertices);
  declareMonitoredStdContainer("roi_zVertices",m_zVertices);
  declareMonitoredStdContainer("roi_nTrk_zVtx",m_nTrk_zVtx);
  ////Spacepoints
  //declareMonitoredStdContainer("sp_x" ,m_sp_x);
  //declareMonitoredStdContainer("sp_y" ,m_sp_y);
  //declareMonitoredStdContainer("sp_z" ,m_sp_z);
  //declareMonitoredStdContainer("sp_r" ,m_sp_r);

  //Unbiased residuals
  declareMonitoredStdContainer("layer_IBL",m_IBL_layer);
  declareMonitoredStdContainer("layer_PixB",m_PixB_layer);
  declareMonitoredStdContainer("layer_PixE",m_PixE_layer);
  declareMonitoredStdContainer("layer_SCTB",m_SCTB_layer);
  declareMonitoredStdContainer("layer_SCTE",m_SCTE_layer);

  declareMonitoredStdContainer("hit_IBLPhiResidual",m_iblResPhi);
  declareMonitoredStdContainer("hit_IBLEtaResidual",m_iblResEta);
  declareMonitoredStdContainer("hit_IBLPhiPull",    m_iblPullPhi);
  declareMonitoredStdContainer("hit_IBLEtaPull",    m_iblPullEta);
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

  declareMonitoredStdContainer("hit_PIXBarrelL1PhiResidual",m_pixResPhiBarrelL1);
  declareMonitoredStdContainer("hit_PIXBarrelL1EtaResidual",m_pixResEtaBarrelL1);
  declareMonitoredStdContainer("hit_PIXBarrelL2PhiResidual",m_pixResPhiBarrelL2);
  declareMonitoredStdContainer("hit_PIXBarrelL2EtaResidual",m_pixResEtaBarrelL2);
  declareMonitoredStdContainer("hit_PIXBarrelL3PhiResidual",m_pixResPhiBarrelL3);
  declareMonitoredStdContainer("hit_PIXBarrelL3EtaResidual",m_pixResEtaBarrelL3);

  declareMonitoredStdContainer("hit_PIXEndCapL1PhiResidual",m_pixResPhiECL1);
  declareMonitoredStdContainer("hit_PIXEndCapL1EtaResidual",m_pixResEtaECL1);
  declareMonitoredStdContainer("hit_PIXEndCapL2PhiResidual",m_pixResPhiECL2);
  declareMonitoredStdContainer("hit_PIXEndCapL2EtaResidual",m_pixResEtaECL2);
  declareMonitoredStdContainer("hit_PIXEndCapL3PhiResidual",m_pixResPhiECL3);
  declareMonitoredStdContainer("hit_PIXEndCapL3EtaResidual",m_pixResEtaECL3);

  declareMonitoredStdContainer("hit_SCTBarrelL1PhiResidual",m_sctResPhiBarrelL1);
  declareMonitoredStdContainer("hit_SCTBarrelL2PhiResidual",m_sctResPhiBarrelL2);
  declareMonitoredStdContainer("hit_SCTBarrelL3PhiResidual",m_sctResPhiBarrelL3);
  declareMonitoredStdContainer("hit_SCTBarrelL4PhiResidual",m_sctResPhiBarrelL4);

  declareMonitoredStdContainer("hit_SCTEndcapL1PhiResidual",m_sctResPhiEndcapL1);
  declareMonitoredStdContainer("hit_SCTEndcapL2PhiResidual",m_sctResPhiEndcapL2);
  declareMonitoredStdContainer("hit_SCTEndcapL3PhiResidual",m_sctResPhiEndcapL3);
  declareMonitoredStdContainer("hit_SCTEndcapL4PhiResidual",m_sctResPhiEndcapL4);
  declareMonitoredStdContainer("hit_SCTEndcapL5PhiResidual",m_sctResPhiEndcapL5);
  declareMonitoredStdContainer("hit_SCTEndcapL6PhiResidual",m_sctResPhiEndcapL6);
  declareMonitoredStdContainer("hit_SCTEndcapL7PhiResidual",m_sctResPhiEndcapL7);
  declareMonitoredStdContainer("hit_SCTEndcapL8PhiResidual",m_sctResPhiEndcapL8);
  declareMonitoredStdContainer("hit_SCTEndcapL9PhiResidual",m_sctResPhiEndcapL9);
}

//--------------------------------------------------------------------------

TrigFastTrackFinder::~TrigFastTrackFinder() {}

//-----------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltInitialize() {

  ATH_MSG_DEBUG("TrigFastTrackFinder::initialize() "  << PACKAGE_VERSION);

  if (m_roiCollectionKey.initialize().isFailure() ) {
    return HLT::BAD_JOB_SETUP;
  }
  if (m_outputTracksKey.initialize().isFailure() ) {
    return HLT::BAD_JOB_SETUP;
  }

  if ( timerSvc() ) {
    m_SpacePointConversionTimer = addTimer("SpacePointConversion"); 
    m_ZFinderTimer              = addTimer("ZFinder"); 
    m_PatternRecoTimer          = addTimer("PattReco","PattReco_nSP");
    m_TripletMakingTimer        = addTimer("Triplets","Triplets_nSP");
    m_CombTrackingTimer         = addTimer("CmbTrack","CmbTrack_nTr");
    m_TrackFitterTimer          = addTimer("TrackFitter","TrackFitter_nTracks");
  }
  
    
  auto scbs = m_beamSpotKey.initialize();
  if(scbs.isFailure()) {
    ATH_MSG_ERROR("Error initializing beamspot info");
    return HLT::BAD_JOB_SETUP;
  }

  if(m_ftkMode) {
    StatusCode sc= m_ftkDataProviderSvc.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_ERROR("unable to locate FTK_DataProviderSvc" << m_ftkDataProviderSvcName);
      return HLT::BAD_JOB_SETUP;
    } else {
      ATH_MSG_INFO("Configured to retrieve FTK tracks from " << m_ftkDataProviderSvcName);
    }
    m_numberingTool.disable();
    m_spacePointTool.disable();
    m_TrigL2SpacePointTruthTool.disable();
    m_trackMaker.disable();   
    m_trigInDetTrackFitter.disable();
    m_trigZFinder.disable();

  } else {

    
    ATH_MSG_DEBUG(" TrigFastTrackFinder : MinHits set to " << m_minHits);
    
    StatusCode sc=m_numberingTool.retrieve(); 
    if(sc.isFailure()) { 
      ATH_MSG_ERROR("Could not retrieve "<<m_numberingTool); 
      return HLT::BAD_JOB_SETUP;
    } 
    
    sc = m_spacePointTool.retrieve();
    if(sc.isFailure()) { 
      ATH_MSG_ERROR("Could not retrieve "<<m_spacePointTool); 
      return HLT::BAD_JOB_SETUP;
    }
    
    sc = m_trackMaker.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve "<<m_trackMaker); 
      return HLT::BAD_JOB_SETUP;
    }
    sc = m_trigInDetTrackFitter.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve "<<m_trigInDetTrackFitter); 
      return HLT::BAD_JOB_SETUP;
    }
    
    ATH_MSG_DEBUG("In Initialize m_doZFinder, m_doFTKZFinder " << m_doZFinder << " " << m_doFTKZFinder);

    if (m_doZFinder) {
      sc = m_trigZFinder.retrieve();
      if(sc.isFailure()) {
        ATH_MSG_ERROR("Could not retrieve "<<m_trigZFinder); 
        return HLT::BAD_JOB_SETUP;
      }
    }
    else {
      m_trigZFinder.disable();
    }

    if(m_doFTKZFinder ) {
      StatusCode sc= m_ftkDataProviderSvc.retrieve();
      if(sc.isFailure()) {
        ATH_MSG_ERROR("unable to locate FTK_DataProviderSvc" << m_ftkDataProviderSvcName);
        return HLT::BAD_JOB_SETUP;
      } else {
        ATH_MSG_INFO("Configured to retrieve FTK tracks from " << m_ftkDataProviderSvcName);
      }
    }
    if(m_doFTKZFinder ) {
      StatusCode sc= m_ftkDataProviderSvc.retrieve();
      if(sc.isFailure()) {
        ATH_MSG_ERROR("unable to locate FTK_DataProviderSvc" << m_ftkDataProviderSvcName);
        return HLT::BAD_JOB_SETUP;
      } else {
        ATH_MSG_INFO("Configured to retrieve FTK tracks from " << m_ftkDataProviderSvcName);
      }
    }
  }

  StatusCode sc= m_trackSummaryTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("unable to locate track summary tool");
    return HLT::BAD_JOB_SETUP;
  }

  if (m_doResMonitoring) {
    sc = m_trigL2ResidualCalculator.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL <<"Unable to locate Residual calculator tool " << m_trigL2ResidualCalculator << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  } else {
    m_trigL2ResidualCalculator.disable();
  }

  //Get ID helper
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR("Could not get AtlasDetectorID helper AtlasID");
    return HLT::BAD_JOB_SETUP;
  }
  
  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
    ATH_MSG_ERROR("Could not get Pixel ID helper");
    return HLT::BAD_JOB_SETUP;
  }
  
  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) { 
    ATH_MSG_ERROR("Could not get Pixel ID helper");
    return HLT::BAD_JOB_SETUP;
  }

  
  if ( m_outputCollectionSuffix != "" ) {
    m_attachedFeatureName = std::string("TrigFastTrackFinder_") + m_outputCollectionSuffix;
  }
  else {
    m_attachedFeatureName      = std::string("TrigFastTrackFinder_");
  }

  if (m_retrieveBarCodes) {
    m_nSignalPresent=0;
    m_nSignalDetected=0;
    m_nSignalTracked=0;
    m_nSignalClones=0;
    StatusCode sc = m_TrigL2SpacePointTruthTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("Unable to locate SpacePoint-to-Truth associator tool " << m_TrigL2SpacePointTruthTool);
      return HLT::BAD_JOB_SETUP;
    }
  } else {
    m_TrigL2SpacePointTruthTool.disable();
  }
  
  ATH_MSG_DEBUG(" Feature set recorded with Key " << m_attachedFeatureName);
  ATH_MSG_DEBUG(" doResMon " << m_doResMonitoring);
  ATH_MSG_DEBUG(" Initialized successfully"); 
  return HLT::OK;
}


//-------------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltStart()
{
  //getting magic numbers from the layer numbering tool

  m_tcs.m_maxBarrelPix    = m_numberingTool->offsetBarrelSCT();
  m_tcs.m_minEndcapPix    = m_numberingTool->offsetEndcapPixels(); 
  m_tcs.m_maxEndcapPix    = m_numberingTool->offsetEndcapSCT();
  m_tcs.m_maxSiliconLayer = m_numberingTool->maxSiliconLayerNum();
  m_tcs.m_layerGeometry.clear();

  if(m_useNewLayerNumberScheme) {
    const std::vector<TRIG_INDET_SI_LAYER>* pVL = m_numberingTool->layerGeometry();
    std::copy(pVL->begin(),pVL->end(),std::back_inserter(m_tcs.m_layerGeometry));
  }

  return HLT::OK;
}

StatusCode TrigFastTrackFinder::execute() {
  //RoI preparation/update 
  SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
  ATH_CHECK(roiCollection.isValid());
  TrigRoiDescriptorCollection::const_iterator roi = roiCollection->begin();
  TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();
  TrigRoiDescriptor internalRoI;
  for (; roi != roiE; ++roi) {
    internalRoI.push_back(*roi);
  }
  internalRoI.manageConstituents(false);//Don't try to delete RoIs at the end
  m_currentStage = 1;
  m_countTotalRoI++;
  m_tcs.roiDescriptor = &internalRoI;

  SG::WriteHandle<TrackCollection> outputTracks(m_outputTracksKey);
  outputTracks = std::make_unique<TrackCollection>();

  ATH_CHECK(findTracks(internalRoI, *outputTracks));
  
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
HLT::ErrorCode TrigFastTrackFinder::hltExecute(const HLT::TriggerElement* /*inputTE*/,
    HLT::TriggerElement* outputTE) {
  const IRoiDescriptor* internalRoI;
  HLT::ErrorCode ec = getRoI(outputTE, internalRoI);
  if(ec != HLT::OK) {
    return ec;
  }
  TrackCollection* outputTracks = new TrackCollection(SG::OWN_ELEMENTS);
  StatusCode sc = findTracks(*internalRoI, *outputTracks);
  HLT::ErrorCode code = HLT::OK;
  if (sc != StatusCode::SUCCESS) {
    delete outputTracks;
    code = attachFeature(outputTE, new TrackCollection(SG::VIEW_ELEMENTS), m_attachedFeatureName);
    if (code != HLT::OK) {
      return code;
    }
    return HLT::ERROR;
  }
  if (outputTracks->empty()) {
    delete outputTracks;
    code = attachFeature(outputTE, new TrackCollection(SG::VIEW_ELEMENTS), m_attachedFeatureName);
  }
  else {
    code = attachFeature(outputTE, outputTracks, m_attachedFeatureName);
  }
  
  return code;
}


StatusCode TrigFastTrackFinder::findTracks(const TrigRoiDescriptor& roi,
                                      TrackCollection& outputTracks) {
  clearMembers();

  m_shift_x=0.0;
  m_shift_y=0.0;
  if(m_useBeamSpot) {
    getBeamSpot();

  }
  else {
    m_vertex = Amg::Vector3D(0.0,0.0,0.0);
  }
  

  ATH_MSG_INFO("Debug: " << roi);
  
  if ( timerSvc() ) m_SpacePointConversionTimer->start();
  
  
  m_currentStage = 1;

  if(m_ftkMode) {
    if ( timerSvc() ) m_TrackFitterTimer->start();
    outputTracks = *(m_ftkDataProviderSvc->getTracksInRoi(roi, m_ftkRefit));//This is a view container
    if ( timerSvc() ) { 
      m_TrackFitterTimer->propVal(outputTracks.size() );
      m_TrackFitterTimer->stop();
    }
    if ( m_ftkRefit ) { 
      ATH_MSG_DEBUG("FTK_DataProviderSvc returned " << outputTracks.size() << " refitted tracks");
    } else {
      ATH_MSG_DEBUG("FTK_DataProviderSvc returned " << outputTracks.size() << " raw tracks");
    }
    if (outputTracks.size()> 0) {
      m_countRoIwithEnoughHits++;
      m_countRoIwithTracks++;
      // fill vectors of quantities to be monitored
      fillMon(outputTracks, roi);
    }
    m_nTracks=outputTracks.size();

    return StatusCode::SUCCESS;
  }
  else {
    m_tcs.roiDescriptor = &roi;
  
    std::vector<TrigSiSpacePointBase> convertedSpacePoints;
    convertedSpacePoints.reserve(5000);
    ATH_CHECK(m_spacePointTool->getSpacePoints( roi, convertedSpacePoints, m_nPixSPsInRoI, m_nSCTSPsInRoI));

    m_roi_nSPs = convertedSpacePoints.size();    
    if ( timerSvc() ) m_SpacePointConversionTimer->stop();
    
    if( m_roi_nSPs >= m_minHits ) {
      ATH_MSG_DEBUG("REGTEST / Found " << m_roi_nSPs << " space points.");
      m_countRoIwithEnoughHits++;
    }
    else {
      ATH_MSG_DEBUG("No tracks found - too few hits in ROI to run " << m_roi_nSPs);
      return StatusCode::SUCCESS;
    }

    m_currentStage = 2;
    
    std::unique_ptr<TrigRoiDescriptor> superRoi = std::make_unique<TrigRoiDescriptor>();

    if (m_doZFinder) {
      if (m_doFTKZFinder ) {
        if ( timerSvc() ) m_ZFinderTimer->start();
        m_tcs.m_vZv.clear();
        superRoi->setComposite(true);

        xAOD::VertexContainer* vertexCollection = new xAOD::VertexContainer();
        xAOD::VertexAuxContainer    theVertexAux;
        vertexCollection->setStore(&theVertexAux);
        bool useRefittedTracks=false;
        if (m_doFTKFastVtxFinder) {
          vertexCollection = m_ftkDataProviderSvc->getFastVertices(ftk::RawTrack);
        } else {
          StatusCode sc = m_ftkDataProviderSvc->getVertexContainer( vertexCollection, useRefittedTracks);
          if (sc != StatusCode::SUCCESS) {
            ATH_MSG_DEBUG (" Error getting VertexContainer StatusCode is " << sc );
          }
        }

      ATH_MSG_DEBUG("vertexCollection->size(): " << vertexCollection->size());

      //      std::sort(vertexCollection->begin(), vertexCollection->end(), vertexSort);
      //      std::sort(vertexCollection->begin(), vertexCollection->end(), 
      //          [](xAOD::Vertex* a, xAOD::Vertex* b){return a->vxTrackAtVertex().size() > b->vxTrackAtVertex().size();});


      unsigned int MaxNumVertex=100;

      for (auto vertex : *vertexCollection) {
        if (m_zVertices.size() == MaxNumVertex) continue;
        ATH_MSG_DEBUG("REGTEST / FTK ZFinder vertex: x,y,z, nTrack  type"   << vertex->x()  << " " <<  vertex->y() << " "  << vertex->z() << "  " << vertex->vxTrackAtVertex().size() << "  " << vertex->vertexType() );
	// test to compare with Tyler's numbers
	if (vertex->vxTrackAtVertex().size() < 3 ) continue;

        float z      = vertex->z();
        float zMinus = z - 7.0;
        float zPlus  = z + 7.0;
        ATH_MSG_DEBUG("REGTEST / FTK ZFinder vertex: z,zplus,zminus  " << z << " "  << zPlus  << " "  << zMinus );
        TrigRoiDescriptor* newRoi =  new TrigRoiDescriptor(roi.eta(), roi.etaMinus(), roi.etaPlus(), 
            roi.phi(), roi.phiMinus(), roi.phiPlus(), z, zMinus, zPlus);
        superRoi->push_back(newRoi);
        m_zVertices.push_back(z);
        m_nTrk_zVtx.push_back( vertex->vxTrackAtVertex().size());
	m_tcs.m_vZv.push_back(z);

      } // end loop over vertices
      m_tcs.roiDescriptor = superRoi.get();
      ATH_MSG_DEBUG("REGTEST / superRoi: " << *superRoi);
      delete vertexCollection;
      if ( timerSvc() ) m_ZFinderTimer->stop();
      } 
      else {
        if ( timerSvc() ) m_ZFinderTimer->start();
        m_tcs.m_vZv.clear();
        superRoi->setComposite(true);

        TrigVertexCollection* vertexCollection = m_trigZFinder->findZ( convertedSpacePoints, roi);

        ATH_MSG_DEBUG("vertexCollection->size(): " << vertexCollection->size());

        for (auto vertex : *vertexCollection) {
          ATH_MSG_DEBUG("REGTEST / ZFinder vertex: " << *vertex);
          float z      = vertex->z();
          float zMinus = z - 7.0;
          float zPlus  = z + 7.0;
          TrigRoiDescriptor* newRoi =  new TrigRoiDescriptor(roi.eta(), roi.etaMinus(), roi.etaPlus(), 
              roi.phi(), roi.phiMinus(), roi.phiPlus(), z, zMinus, zPlus);
          superRoi->push_back(newRoi);
          m_zVertices.push_back(z);
          m_tcs.m_vZv.push_back(z);
        }
        m_tcs.roiDescriptor = superRoi.get();
        ATH_MSG_DEBUG("REGTEST / superRoi: " << *superRoi);
        delete vertexCollection;
        if ( timerSvc() ) m_ZFinderTimer->stop();
      }
    }

    m_currentStage = 3;

    if (m_retrieveBarCodes) {
      std::vector<int> vBar;
      m_TrigL2SpacePointTruthTool->getBarCodes(convertedSpacePoints,vBar);

      //for(auto barCode : vBar) std::cout<<"SP bar code = "<<barCode<<std::endl;
    } 

    m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;
    ATH_MSG_VERBOSE("m_tcs.m_tripletPtMin: " << m_tcs.m_tripletPtMin);
    ATH_MSG_VERBOSE("m_pTmin: " << m_pTmin);

    if ( timerSvc() ) m_PatternRecoTimer->start();

    std::map<int, int> nGoodRejected;
    std::map<int, int> nGoodAccepted;
    std::map<int, int> nGoodTotal;
    if(m_retrieveBarCodes) {
      for(auto barCode : m_vSignalBarCodes) {
        nGoodRejected.insert(std::pair<int,int>(barCode,0));
        nGoodAccepted.insert(std::pair<int,int>(barCode,0));
        nGoodTotal.insert(std::pair<int,int>(barCode,0));
      }
    }

    int iSeed=0;


    if ( timerSvc() ) m_TripletMakingTimer->start();

    TRIG_TRACK_SEED_GENERATOR seedGen(m_tcs);

    seedGen.loadSpacePoints(convertedSpacePoints);

    if (m_doZFinder && m_doFastZVseeding) seedGen.createSeedsZv();
    else seedGen.createSeeds();
    std::vector<TrigInDetTriplet*> triplets;
    seedGen.getSeeds(triplets);

    ATH_MSG_DEBUG("number of triplets: " << triplets.size());

    if ( timerSvc() ) {
      m_TripletMakingTimer->stop();
      m_TripletMakingTimer->propVal(m_roi_nSPs);
    }
    m_currentStage = 4;

    if ( timerSvc() ) m_CombTrackingTimer->start();

    // 8. Combinatorial tracking

    std::vector<int> vTBarCodes(triplets.size(),-1);

    if(m_retrieveBarCodes) {
      assignTripletBarCodes(triplets, vTBarCodes);
    }

    std::vector<std::tuple<bool, double,Trk::Track*>> qualityTracks; //bool used for later filtering
    qualityTracks.reserve(triplets.size());

    m_nSeeds  = 0;
    iSeed=0;

    long int trackIndex=0;

    if(m_checkSeedRedundancy) m_siClusterMap.clear();

    bool PIX = true;
    bool SCT = true;

    m_trackMaker->newTrigEvent(PIX,SCT);

    for(unsigned int tripletIdx=0;tripletIdx!=triplets.size();tripletIdx++) {

      TrigInDetTriplet* seed = triplets[tripletIdx];

      const Trk::SpacePoint* osp1 = seed->s1().offlineSpacePoint();
      const Trk::SpacePoint* osp2 = seed->s2().offlineSpacePoint();
      const Trk::SpacePoint* osp3 = seed->s3().offlineSpacePoint();

      if(m_checkSeedRedundancy) {
        //check if clusters do not belong to any track
        std::vector<Identifier> clusterIds;
        extractClusterIds(osp1, clusterIds);
        extractClusterIds(osp2, clusterIds);
        extractClusterIds(osp3, clusterIds);
        if(usedByAnyTrack(clusterIds, m_siClusterMap)) {
          continue;
        }
      }

      std::list<const Trk::SpacePoint*> spList = {osp1, osp2, osp3};

      bool trackFound=false;

      ++m_nSeeds;

      const std::list<Trk::Track*>& tracks = m_trackMaker->getTracks(spList);

      for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
        if((*t)) {
          float d0 = (*t)->perigeeParameters()==0 ? 10000.0 : (*t)->perigeeParameters()->parameters()[Trk::d0]; 
          if (fabs(d0) > m_initialD0Max) {
	     ATH_MSG_DEBUG("REGTEST / Reject track with d0 = " << d0 << " > " << m_initialD0Max);
	     qualityTracks.push_back(std::make_tuple(false,0,(*t)));//Flag track as bad, but keep in vector for later deletion
	     continue;
          }
          if(m_checkSeedRedundancy) {
            //update clusterMap 
            updateClusterMap(trackIndex++, (*t), m_siClusterMap);
          }
          if(m_doCloneRemoval) {
            qualityTracks.push_back(std::make_tuple(true, -trackQuality((*t)), (*t)));
          }
          else {
            qualityTracks.push_back(std::make_tuple(true, 0, (*t)));
          }
        }
      }  
      iSeed++;
      ATH_MSG_VERBOSE("Found "<<tracks.size()<<" tracks using triplet");
      if(!tracks.empty()) {
        trackFound = true;
      }

      if(m_retrieveBarCodes) {
        bool goodTriplet=false;
        int foundBarCode=-1;

        for(auto barCode : m_vSignalBarCodes) {
          if (vTBarCodes[tripletIdx] == barCode) {
            foundBarCode=barCode;
            goodTriplet=true;break;
          }
        }

        if(goodTriplet) {
          (*nGoodTotal.find(foundBarCode)).second++;
          if(trackFound) (*nGoodAccepted.find(foundBarCode)).second++;
          else (*nGoodRejected.find(foundBarCode)).second++;
        }
      }
    }

    m_trackMaker->endEvent();
    for(auto& seed : triplets) delete seed;

    //clone removal
    if(m_doCloneRemoval) {
      filterSharedTracks(qualityTracks);
    }

    TrackCollection* initialTracks = new TrackCollection;
    initialTracks->reserve(qualityTracks.size());
    for(const auto& q : qualityTracks) {
      if (std::get<0>(q)==true) {
        initialTracks->push_back(std::get<2>(q));
      }
      else {
        delete std::get<2>(q);
      }
    }
    qualityTracks.clear();

    ATH_MSG_DEBUG("After clone removal "<<initialTracks->size()<<" tracks left");


    if ( timerSvc() ) {
      m_CombTrackingTimer->stop();
      m_CombTrackingTimer->propVal(iSeed);
      m_PatternRecoTimer->propVal( initialTracks->size() );
      m_PatternRecoTimer->stop();
      m_timePattReco = m_PatternRecoTimer->elapsed();
    }
    m_currentStage = 5;



    if (m_retrieveBarCodes) {
      //reco. efficiency analysis
      calculateRecoEfficiency(convertedSpacePoints, nGoodTotal, nGoodAccepted);
    }

    if ( timerSvc() ) m_TrackFitterTimer->start();

    outputTracks = *(m_trigInDetTrackFitter->fit(*initialTracks, m_particleHypothesis));
    delete initialTracks;

    if( outputTracks.empty() ) {
      ATH_MSG_DEBUG("REGTEST / No tracks fitted");
    }

    size_t counter(1);
    for (auto fittedTrack = outputTracks.begin(); fittedTrack!=outputTracks.end(); ) {
      if ((*fittedTrack)->perigeeParameters()){
	float d0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::d0]; 
	float z0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::z0]; 
	if (fabs(d0) > m_initialD0Max || fabs(z0) > m_Z0Max) {
	  ATH_MSG_WARNING("REGTEST / Reject track after fit with d0 = " << d0 << " z0= "  << z0
			  << " larger than limits (" << m_initialD0Max << ", " << m_Z0Max << ")");
	  ATH_MSG_DEBUG(**fittedTrack);
	  fittedTrack = outputTracks.erase(fittedTrack);
	  continue;
	}

      } 

      (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
      ATH_MSG_VERBOSE("Updating fitted track: " << counter);
      ATH_MSG_VERBOSE(**fittedTrack);
      m_trackSummaryTool->updateTrack(**fittedTrack);
      ATH_MSG_VERBOSE("Updated track: " << counter);
      ATH_MSG_VERBOSE(**fittedTrack);
      counter++; fittedTrack++;
    }

    if ( timerSvc() ) { 
      m_TrackFitterTimer->propVal(outputTracks.size() );
      m_TrackFitterTimer->stop();
    }

    if( outputTracks.empty() ) {
      ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
    }
    m_currentStage = 6;

    //monitor Z-vertexing

    m_nZvertices=m_zVertices.size();

    //monitor number of tracks
    m_nTracks=outputTracks.size();
    ATH_MSG_DEBUG("REGTEST / Found " << m_nTracks << " tracks");
    if( !outputTracks.empty() )
      m_countRoIwithTracks++;

    ///////////// fill vectors of quantities to be monitored
    fillMon(outputTracks, roi);

    m_currentStage = 7;

    return StatusCode::SUCCESS;
  }
}

double TrigFastTrackFinder::trackQuality(const Trk::Track* Tr) {

  DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
       me = Tr->trackStateOnSurfaces()->end  ();

  double quality = 0. ;
  const double W       = 17.;

  for(; m!=me; ++m) {
    const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
    if(!fq) continue;

    double x2 = fq->chiSquared();
    double q;
    if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
    else                     q =      (W-x2    );
    if(q < 0.) q = 0.;
    quality+=q;
  }
  return quality;
}

void TrigFastTrackFinder::filterSharedTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) {

  std::set<const Trk::PrepRawData*> clusters;

  const Trk::PrepRawData* prd[100];

  std::sort(QT.begin(), QT.end(),
      [](const std::tuple<bool, double, Trk::Track*>& lhs, const std::tuple<bool, double, Trk::Track*>& rhs) {
      return std::get<1>(lhs) < std::get<1>(rhs); } );

  for (auto& q : QT) {
    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = std::get<2>(q)->measurementsOnTrack()->begin(), 
         me = std::get<2>(q)->measurementsOnTrack()->end  ();

    int nf = 0, nc = 0; 
    for(; m!=me; ++m ) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
        ++nc;
        if(clusters.find(pr)==clusters.end()) {prd[nf++]=pr; if(nf==100) break;}
      }
    }
    if((nf >= m_nfreeCut) || (nf == nc) ) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
    }
    else  {
      std::get<0>(q) = false;
    }
  }
}

//---------------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltFinalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinder::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks);
  if (m_retrieveBarCodes) {
    ATH_MSG_INFO("Number of signal tracks present " << m_nSignalPresent);
    ATH_MSG_INFO("Number of signal seeds  found   " << m_nSignalDetected);
    ATH_MSG_INFO("Number of signal tracks found   " << m_nSignalTracked);
    if(m_nSignalPresent!=0) {
      ATH_MSG_INFO("Track seeding  efficiency      " << (100.0*m_nSignalDetected/m_nSignalPresent) <<" % ");
      ATH_MSG_INFO("Track seeding  redundancy      " << (100.0*m_nSignalClones/m_nSignalPresent) << " %");
      ATH_MSG_INFO("Track finding efficiency       " << (100.0*m_nSignalTracked/m_nSignalPresent) << " %");
    }
  }
  ATH_MSG_INFO("=========================================================");

  return HLT::OK;
}

void TrigFastTrackFinder::updateClusterMap(long int trackIdx, const Trk::Track* pTrack, std::map<Identifier, std::vector<long int> >& clusterMap) {
  //loop over clusters

  for(auto tMOT = pTrack->measurementsOnTrack()->begin(); tMOT != pTrack->measurementsOnTrack()->end(); ++tMOT) { 

    const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT); 
    if (siCLOT==nullptr) continue;
    const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData()); 
    if (siCL==nullptr) continue;
    Identifier id = siCL->identify(); 
    clusterMap[id].push_back(trackIdx);
    //no sorting is needed as the vectors are sorted by the algorithm design
    //due to monotonically increasing trackIdx
    // std::map<Identifier, std::vector<long int> >::iterator itm = clusterMap.find(id);
    //std::sort((*itm).second.begin(),(*itm).second.end());
    //std::copy((*itm).second.begin(),(*itm).second.end(),std::ostream_iterator<long int>(std::cout," "));
    //std::cout<<std::endl;
  }
}

void TrigFastTrackFinder::extractClusterIds(const Trk::SpacePoint* pSP, std::vector<Identifier>& vIds) {
  const InDet::SiCluster* pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().first);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
  //check second cluster : SCT uv clusters only !
  pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().second);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
}

bool TrigFastTrackFinder::usedByAnyTrack(const std::vector<Identifier>& vIds, std::map<Identifier, std::vector<long int> >& clusterMap) {

  std::vector<long int> xSection;
  //initializing
  std::map<Identifier, std::vector<long int> >::iterator itm0 = clusterMap.find(*vIds.begin());
  if(itm0 == clusterMap.end()) return false;
  xSection.reserve((*itm0).second.size());
  std::copy((*itm0).second.begin(), (*itm0).second.end(), std::back_inserter(xSection));
  std::vector<Identifier>::const_iterator it = vIds.begin();it++;
  for(;it!=vIds.end();++it) {
    std::map<Identifier, std::vector<long int> >::iterator itm1 = clusterMap.find(*it);
    if(itm1 == clusterMap.end()) return false;
    std::vector<long int> tmp;
    std::set_intersection(xSection.begin(), xSection.end(), (*itm1).second.begin(),(*itm1).second.end(), std::back_inserter(tmp));
    if(tmp.empty()) return false;
    //update xSection
    xSection.clear();
    xSection.reserve(tmp.size());
    std::copy(tmp.begin(), tmp.end(), std::back_inserter(xSection));
  }
  return !xSection.empty();
}

int TrigFastTrackFinder::findBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  int nFound=0;
  std::set<int> layerSet;
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      nFound++;
      layerSet.insert(sp.layer());
    }
  }
  if(int(layerSet.size())<m_minSignalSPs) {//less than N unique layers
    nFound=0;
  }
  return nFound;
}

void TrigFastTrackFinder::showBarCodeInData(int barCode, const std::vector<TrigSiSpacePointBase>& vSP) {
  for(auto sp : vSP) {
    if(barCode==sp.barCode()) {
      ATH_MSG_DEBUG("L="<<sp.layer()<<" r="<<sp.r()<<" z="<<sp.z());
    }
  }
}

int TrigFastTrackFinder::findBarCodeInTriplets(int barCode, const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR) {
  int nFound=0;
  for(auto tr : vTR) {
    bool found = (barCode == tr->s1().barCode()) && (barCode == tr->s2().barCode()) && (barCode == tr->s3().barCode());
    if(found) {
      nFound++;
    }
  }
  return nFound;
}

void TrigFastTrackFinder::assignTripletBarCodes(const std::vector<std::shared_ptr<TrigInDetTriplet>>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinder::assignTripletBarCodes(const std::vector<TrigInDetTriplet*>& vTR, std::vector<int>& vBar) {
  int iTR=0;
  for(auto tr : vTR) {
    bool good = (tr->s1().barCode() == tr->s2().barCode()) && (tr->s3().barCode() == tr->s2().barCode());
    good = good && (tr->s1().barCode() > 0);
    if(good) {
      vBar[iTR] = tr->s1().barCode();
    }
    iTR++;
  }
}

void TrigFastTrackFinder::getBeamSpot() {
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  m_vertex = beamSpotHandle->beamPos();
  ATH_MSG_VERBOSE("Beam spot position " << m_vertex);
  double xVTX = m_vertex.x();
  double yVTX = m_vertex.y();
  double zVTX = m_vertex.z();
  double tiltXZ = beamSpotHandle->beamTilt(0);
  double tiltYZ = beamSpotHandle->beamTilt(1);
  m_shift_x = xVTX - tiltXZ*zVTX;//correction for tilt
  m_shift_y = yVTX - tiltYZ*zVTX;//correction for tilt
  ATH_MSG_VERBOSE("Beam center position:  " << m_shift_x <<"  "<< m_shift_y);
}

HLT::ErrorCode TrigFastTrackFinder::getRoI(const HLT::TriggerElement* outputTE, const IRoiDescriptor*& roi)
{

  const TrigRoiDescriptor* externalRoI = nullptr;
  HLT::ErrorCode ec = getFeature(outputTE, externalRoI);
  if(ec != HLT::OK) {
    ATH_MSG_ERROR("REGTEST / Failed to find RoiDescriptor");
    return HLT::NAV_ERROR;
  }

  if(externalRoI==nullptr) {
    ATH_MSG_ERROR("REGTEST / null RoiDescriptor");
    return HLT::NAV_ERROR;
  }

  roi = externalRoI;
  ATH_MSG_DEBUG("REGTEST / RoI" << *roi);

  return HLT::OK;
}

void TrigFastTrackFinder::clearMembers() {
  m_nTracks = 0;
  m_zVertices.clear();
  m_nTrk_zVtx.clear();

  m_trk_pt.clear();
  m_trk_a0.clear();
  m_trk_z0.clear();
  m_trk_phi0.clear();
  m_trk_eta.clear();
  m_trk_chi2dof.clear();
  m_trk_nSiHits.clear();
  m_trk_nPIXHits.clear();
  m_trk_nSCTHits.clear();
  m_trk_a0beam.clear();
  m_trk_dPhi0.clear();
  m_trk_dEta.clear();

  //m_sp_x.clear();
  //m_sp_y.clear();
  //m_sp_z.clear();
  //m_sp_r.clear();

  m_IBL_layer.clear();
  m_PixB_layer.clear();
  m_PixE_layer.clear();
  m_SCTB_layer.clear();
  m_SCTE_layer.clear();

  m_iblResPhi.clear();
  m_iblResEta.clear();
  m_iblPullPhi.clear();
  m_iblPullEta.clear();
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

  m_pixResPhiBarrelL1.clear();
  m_pixResEtaBarrelL1.clear();
  m_pixResPhiBarrelL2.clear();
  m_pixResEtaBarrelL2.clear();
  m_pixResPhiBarrelL3.clear();
  m_pixResEtaBarrelL3.clear();

  m_pixResPhiECL1.clear();
  m_pixResEtaECL1.clear();
  m_pixResPhiECL2.clear();
  m_pixResEtaECL2.clear();
  m_pixResPhiECL3.clear();
  m_pixResEtaECL3.clear();

  m_sctResPhiBarrelL1.clear();
  m_sctResPhiBarrelL2.clear();
  m_sctResPhiBarrelL3.clear();
  m_sctResPhiBarrelL4.clear();

  m_sctResPhiEndcapL1.clear();
  m_sctResPhiEndcapL2.clear();
  m_sctResPhiEndcapL3.clear();
  m_sctResPhiEndcapL4.clear();
  m_sctResPhiEndcapL5.clear();
  m_sctResPhiEndcapL6.clear();
  m_sctResPhiEndcapL7.clear();
  m_sctResPhiEndcapL8.clear();
  m_sctResPhiEndcapL9.clear();

  m_nPixSPsInRoI=0;
  m_nSCTSPsInRoI=0;
  m_currentStage=0;
  m_roi_nSPs=0;
  m_nZvertices=0;
}

void TrigFastTrackFinder::calculateRecoEfficiency(const std::vector<TrigSiSpacePointBase>& convertedSpacePoints,
    const std::map<int,int>& nGoodTotal, 
    const std::map<int,int>& nGoodAccepted) {

  //reco. efficiency analysis
  for(auto barCode : m_vSignalBarCodes) {
    int nSignalSPs = findBarCodeInData(barCode, convertedSpacePoints);
    if(nSignalSPs<m_minSignalSPs) continue;
    m_nSignalPresent+=1;
    int nSignalTracks = (*nGoodTotal.find(barCode)).second;
    if(nSignalTracks==0) {
      continue;
    }
    m_nSignalDetected+=1;
    m_nSignalClones+=nSignalTracks;

    int nGoodTripletsAccepted = (*nGoodAccepted.find(barCode)).second;
    if(nGoodTripletsAccepted==0) continue;
    m_nSignalTracked+=1;
  } 
}

void TrigFastTrackFinder::fillMon(const TrackCollection& tracks, const TrigRoiDescriptor& roi) {
  size_t size = tracks.size();
  m_trk_pt.reserve(size);
  m_trk_a0.reserve(size);
  m_trk_z0.reserve(size);
  m_trk_phi0.reserve(size);
  m_trk_eta.reserve(size);
  m_trk_chi2dof.reserve(size);
  m_trk_nSiHits.reserve(size);
  m_trk_nPIXHits.reserve(size);
  m_trk_nSCTHits.reserve(size);
  m_trk_a0beam.reserve(size);
  m_trk_dPhi0.reserve(size);
  m_trk_dEta.reserve(size);

  m_roiEta = roi.eta();
  m_roiEtaWidth = roi.etaPlus() - roi.etaMinus();
  m_roiPhi = roi.phi();
  m_roiPhiWidth = HLT::wrapPhi(roi.phiPlus() - roi.phiMinus());
  m_roiZ = roi.zed();
  m_roiZ_Width = roi.zedPlus() - roi.zedMinus();


  for (auto track : tracks) {
    const Trk::TrackParameters* trackPars = track->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }

    float a0 = trackPars->parameters()[Trk::d0]; 
    m_trk_a0.push_back(a0);
    float z0 = trackPars->parameters()[Trk::z0]; 
    m_trk_z0.push_back(z0);
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    m_trk_phi0.push_back(phi0);
    m_trk_a0beam.push_back(a0+m_shift_x*sin(phi0)-m_shift_y*cos(phi0));
    float dPhi0 = HLT::wrapPhi(phi0 - m_roiPhi);
    m_trk_dPhi0.push_back(dPhi0);
    float theta = trackPars->parameters()[Trk::theta]; 
    float eta = -log(tan(0.5*theta)); 
    m_trk_eta.push_back(eta);
    m_trk_dEta.push_back(eta - m_roiEta);

    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    if (qOverP==0) {
      ATH_MSG_DEBUG("REGTEST / q/p == 0, adjusting to 1e-12");
      qOverP = 1e-12;
    }
    float pT=sin(theta)/qOverP;
    m_trk_pt.push_back(pT);

    const Trk::FitQuality* fq = track->fitQuality();
    float chi2 = 1e8;
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
        chi2 = fq->chiSquared()/fq->numberDoF();
      }
    }
    m_trk_chi2dof.push_back(chi2);
    int nPix=0, nSct=0;

    for(auto tSOS = track->trackStateOnSurfaces()->begin();  
        tSOS!=track->trackStateOnSurfaces()->end(); ++tSOS) { 
      if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
        const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface(); 
        if(!fq) continue; 
        int nd = fq->numberDoF(); 
        if(nd==2) nPix++;
        if(nd==1) nSct++;
      }
    }
    if (m_ftkMode) {
      const Trk::TrackSummary* summary = track->trackSummary();
      if( summary != nullptr){
        nPix = summary->get(Trk::numberOfPixelHits);
        nSct = summary->get(Trk::numberOfSCTHits);
      }
    }
    m_trk_nPIXHits.push_back(nPix); 
    m_trk_nSCTHits.push_back(nSct/2); 
    m_trk_nSiHits.push_back(nPix + nSct/2); 

    ATH_MSG_DEBUG("REGTEST / track npix/nsct/phi0/pt/eta/d0/z0/chi2: " <<
        nPix   << " / "  << 
        nSct/2 << " / "  << 
        phi0   << " / "  << 
        pT     << " / " << 
        eta    << " / " << 
        a0     << " / " <<
        z0     << " / " <<
        chi2);
    // tighter selection for unbiased residuals
    bool goodTrack = std::fabs(pT)>1000. && (nPix + nSct/2) > 3 && nSct > 0;
    if (goodTrack && m_doResMonitoring) {
      runResidualMonitoring(*track);
    }
  }
}

void TrigFastTrackFinder::runResidualMonitoring(const Trk::Track& track) {
  std::vector<TrigL2HitResidual> vResid;
  vResid.clear();
  StatusCode scRes = m_trigL2ResidualCalculator->getUnbiassedResiduals(track,vResid);
  if(!scRes.isSuccess()) return;
  for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it) {
    Identifier id = it->identify();
    int pixlayer= (m_pixelId->layer_disk(id) );
    int sctlayer= (m_sctId->layer_disk(id) );

    switch(it->regionId()) {
      case Region::PixBarrel :
	m_PixB_layer.push_back(pixlayer);
        ATH_MSG_DEBUG("Pixel Barrel "  );
        m_pixResPhiBarrel.push_back(it->phiResidual());
        m_pixPullPhiBarrel.push_back(it->phiPull());
	if (pixlayer == 1) {
	  m_pixResPhiBarrelL1.push_back(it->phiResidual());
	  m_pixResEtaBarrelL1.push_back(it->etaResidual());
	}
	if (pixlayer == 2) {
	  m_pixResPhiBarrelL2.push_back(it->phiResidual());
	  m_pixResEtaBarrelL2.push_back(it->etaResidual());
	}
	if (pixlayer == 3) {
	  m_pixResPhiBarrelL3.push_back(it->phiResidual());
	  m_pixResEtaBarrelL3.push_back(it->etaResidual());
	}
        m_pixResEtaBarrel.push_back(it->etaResidual());
        m_pixPullEtaBarrel.push_back(it->etaPull());
        break;
      case Region::PixEndcap :
        ATH_MSG_DEBUG("Pixel Endcap "  );
	m_PixE_layer.push_back(pixlayer);
        m_pixResPhiEC.push_back(it->phiResidual());
        m_pixPullPhiEC.push_back(it->phiPull());
	if (pixlayer == 0) {
	  m_pixResPhiECL1.push_back(it->phiResidual());
	  m_pixResEtaECL1.push_back(it->etaResidual());
	}
	if (pixlayer == 1) {
	  m_pixResPhiECL2.push_back(it->phiResidual());
	  m_pixResEtaECL2.push_back(it->etaResidual());
	}
	if (pixlayer == 2) {
	  m_pixResPhiECL3.push_back(it->phiResidual());
	  m_pixResEtaECL3.push_back(it->etaResidual());
	}
        m_pixResEtaEC.push_back(it->etaResidual());
        m_pixPullEtaEC.push_back(it->etaPull());
        break;
      case Region::SctBarrel :
	m_SCTB_layer.push_back(sctlayer);
        ATH_MSG_DEBUG("SCT Barrel"  );
        m_sctResBarrel.push_back(it->phiResidual());
        m_sctPullBarrel.push_back(it->phiPull());
	if (sctlayer == 0) {
	  m_sctResPhiBarrelL1.push_back(it->phiResidual());
	}
	if (sctlayer == 1) {
	  m_sctResPhiBarrelL2.push_back(it->phiResidual());
	}
	if (sctlayer == 2) {
	  m_sctResPhiBarrelL3.push_back(it->phiResidual());
	}
	if (sctlayer == 3) {
	  m_sctResPhiBarrelL4.push_back(it->phiResidual());
	}
        break;
      case Region::SctEndcap :
	m_SCTE_layer.push_back(sctlayer);
        ATH_MSG_DEBUG("SCT Endcap"  );
        m_sctResEC.push_back(it->phiResidual());
        m_sctPullEC.push_back(it->phiPull());
	if (sctlayer == 0) {
	  m_sctResPhiEndcapL1.push_back(it->phiResidual());
	}
	if (sctlayer == 1) {
	  m_sctResPhiEndcapL2.push_back(it->phiResidual());
	}
	if (sctlayer == 2) {
	  m_sctResPhiEndcapL3.push_back(it->phiResidual());
	}
	if (sctlayer == 3) {
	  m_sctResPhiEndcapL4.push_back(it->phiResidual());
	}
	if (sctlayer == 4) {
	  m_sctResPhiEndcapL5.push_back(it->phiResidual());
	}
	if (sctlayer == 5) {
	  m_sctResPhiEndcapL6.push_back(it->phiResidual());
	}
	if (sctlayer == 6) {
	  m_sctResPhiEndcapL7.push_back(it->phiResidual());
	}
	if (sctlayer == 7) {
	  m_sctResPhiEndcapL8.push_back(it->phiResidual());
	}
	if (sctlayer == 8) {
	  m_sctResPhiEndcapL9.push_back(it->phiResidual());
	}
        break;
      case Region::IBL :
	m_IBL_layer.push_back(pixlayer);
        if (m_tcs.m_maxSiliconLayer==32) {
          m_iblResPhi.push_back(it->phiResidual());
          m_iblPullPhi.push_back(it->phiPull());
          m_iblResEta.push_back(it->etaResidual());
          m_iblPullEta.push_back(it->etaPull());
        }
        else {//No IBL, fill pixel histograms instead
	  ATH_MSG_DEBUG("IBL wrong region"  );
          m_pixResPhiBarrel.push_back(it->phiResidual());
          m_pixPullPhiBarrel.push_back(it->phiPull());
          m_pixResEtaBarrel.push_back(it->etaResidual());
          m_pixPullEtaBarrel.push_back(it->etaPull());
        }
        break;
      case Region::Undefined :
        ATH_MSG_DEBUG("Undefined ID region");
        break;
    }
  }
}

