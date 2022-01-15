/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// TrigFastTrackFinder.cxx
// -------------------------------
// ATLAS Collaboration
//
// package created 16/04/2013 by Dmitry Emeliyanov (see ChangeLog for more details)
//
////////////////////////////////////////////////////////////////////////////////



#include "TrigFastTrackFinder.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

//
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkTrack/Track.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

#include "CxxUtils/phihelper.h"

#include "AthenaMonitoringKernel/Monitored.h"


#include "PathResolver/PathResolver.h"

//for UTT
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>

TrigFastTrackFinder::TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) :

  AthReentrantAlgorithm(name, pSvcLocator),
  m_numberingTool("TrigL2LayerNumberTool"),
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigZFinder("TrigZFinder/TrigZFinder", this ),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_disTrkFitter("Trk::KalmanFitter/InDetTrackFitter"),
  m_accelTool("TrigInDetAccelerationTool"),
  m_accelSvc("TrigInDetAccelerationSvc", name),
  m_doCloneRemoval(true),
  m_useBeamSpot(true),
  m_doZFinder(false),
  m_doZFinderOnly(false),
  m_storeZFinderVertices(false),
  m_useBeamSpotForRoiZwidth(false),
  m_nfreeCut(5),
  m_countTotalRoI(0),
  m_countRoIwithEnoughHits(0),
  m_countRoIwithTracks(0),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_particleHypothesis(Trk::pion),
  m_useNewLayerNumberScheme(false),
  m_useGPU(false),
  m_LRTmode(false),
  m_LRTD0Min(0.0),
  m_LRTHardMinPt(0.0),
  m_trigseedML_LUT(""),
  m_doHitDV(false),
  m_doHitDV_Seeding(false),
  m_dodEdxTrk(false),
  m_doDisappearingTrk(false)
{

  /** Initial cut */
  declareProperty( "MinHits",               m_minHits = 5,"Minimum number of hits needed to perform tracking"  );

  //** Zfinder mode
  declareProperty( "doZFinder",            m_doZFinder = true,"Use fast ZFinder to find z of primary vertices");
  declareProperty( "doZFinderOnly",        m_doZFinderOnly = false,"stop processing after ZFinder - no tracking performed");
  declareProperty( "VertexSeededMode",     m_vertexSeededMode = false); //** NOT USED Obsolete? ATR-24242
  declareProperty( "doFastZVertexSeeding", m_doFastZVseeding = true,"Use ZFinder vertex information to filter seeds");
  declareProperty( "zVertexResolution",    m_tcs.m_zvError = 10.0," Half-width (mm) in z of z region used to filter seeds when doFastZVertexSeeding enabled" );
  declareProperty( "StoreZFinderVertices", m_storeZFinderVertices = false ); //** NOT USED - to be implemented ATR-24242

  /** SeedMaker */
  declareProperty("useNewLayerNumberScheme", m_useNewLayerNumberScheme = false,"Use LayerNumberTool for layer numbers");

  /** Doublet finding properties. */
  declareProperty("Doublet_FilterRZ",          m_tcs.m_doubletFilterRZ = true,"Enable check that doublet is consistent with the RoI in the RZ plane");
  declareProperty("DoubletDR_Max",             m_tcs.m_doublet_dR_Max = 270.0,"Maximum Radial distance between spacepoints forming Doublets");
  declareProperty("SeedRadBinWidth",           m_tcs.m_seedRadBinWidth = 2.0);

  /** Triplet finding properties. */
  declareProperty("Triplet_D0Max",            m_tcs.m_tripletD0Max      = 4.0,"Maximum d0 for triplet");
  declareProperty("Triplet_D0_PPS_Max",       m_tcs.m_tripletD0_PPS_Max = 1.7,"Maximin d0 for PPS doublets");
  declareProperty("Triplet_nMaxPhiSlice",     m_tcs.m_nMaxPhiSlice = 53,"Number of phi-slices used for seeding");
  declareProperty("Triplet_MaxBufferLength",     m_tcs.m_maxTripletBufferLength = 3,"Maximum number of triplets sharing a common middle spacepoint");
  declareProperty("TripletDoPSS",            m_tcs.m_tripletDoPSS = false,"Allow PSS Triplet seeds");
  declareProperty("TripletDoPPS",            m_tcs.m_tripletDoPPS = true,"Allow PPS triplet seeds");
  declareProperty("TripletDoConfirm",        m_tcs.m_tripletDoConfirm = false,"Enable triplet confirmation");
  declareProperty("DoubletDR_Max_Confirm",     m_tcs.m_doublet_dR_Max_Confirm = 150.0,"doublet max DR when TripletDoConfirm enabled");
  declareProperty("TripletMaxCurvatureDiff",    m_tcs.m_curv_delta = 0.001,"Maximum curvature difference allowed in seed confirmation");//for the triplet confirmation
  declareProperty("Triplet_DtCut",            m_tcs.m_tripletDtCut      = 10.0);//i.e. 10*sigma_MS
  declareProperty("pTmin",                    m_pTmin = 1000.0,"Triplet pT threshold is pTmin*Triplet_MinPtFrac" );
  declareProperty("Triplet_MinPtFrac",        m_tripletMinPtFrac = 0.3,"Triplet pT threshold is pTmin*Triplet_MinPtFrac");
  declareProperty("doSeedRedundancyCheck",    m_checkSeedRedundancy = false,"skip Triplets already used in a track");

  /** settings for the ML-enhanced track seeding */
  declareProperty("UseTrigSeedML",              m_tcs.m_useTrigSeedML = 0,"set ML-based seed selection mode (0 disables)" );
  declareProperty("TrigSeedML_LUT",             m_trigseedML_LUT = "trigseed_ml_pixel_barrel_kde.lut","LUT used by ML-based seed selection");
  declareProperty("maxEC_Pixel_cluster_length", m_tcs.m_maxEC_len = 1.5,"Maximum Endcap Pixel cluster length for ML-based seed selection" );


  //* Clone removal (removal of tracks sharing too many hits */
  declareProperty("doCloneRemoval", m_doCloneRemoval = true,"Remove tracks sharing too many hits");
  declareProperty( "FreeClustersCut"   ,m_nfreeCut,"Minimum number of unshared clusters");

  //** Cuts applied to final tracks after Fit
  declareProperty("TrackInitialD0Max",            m_initialD0Max      = 10.0,"Maximum d0 of track");
  declareProperty("TrackZ0Max",                   m_Z0Max      = 300.0,"Maximum z0 of track");


  /* Monitoring */
  declareProperty( "doResMon",       m_doResMonitoring = true,"enable unbiased residual monitoring");
  declareProperty( "UseBeamSpot",           m_useBeamSpot = true,"Monitor d0 with respect to beamspot");

  //* Collection Names */
  declareProperty("TracksName",
                  m_outputTracksKey = std::string("TrigFastTrackFinder_Tracks"),
                  "TrackCollection name");

  declareProperty("inputTracksName",
                  m_inputTracksKey = std::string(""),
                  "TrackCollection name");

  declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");

  //* Tools */
  declareProperty( "SpacePointProviderTool", m_spacePointTool  );
  declareProperty( "LayerNumberTool", m_numberingTool  );
  declareProperty( "initialTrackMaker", m_trackMaker);
  declareProperty( "trigInDetTrackFitter",   m_trigInDetTrackFitter );
  declareProperty( "trigZFinder",   m_trigZFinder );
  declareProperty("TrackSummaryTool", m_trackSummaryTool);

  // Accleration
  declareProperty("useGPU", m_useGPU = false,"Use GPU acceleration");
  declareProperty("useBeamSpotForRoiZwidth", m_useBeamSpotForRoiZwidth = false);

  // Large Radius Tracking
  declareProperty("LRT_Mode", m_LRTmode,"Enable Large Radius Tracking mode" );
  declareProperty("LRT_D0Min", m_LRTD0Min=0.0,"Minimum d0 for tracks to be saved in LRT Mode" );
  declareProperty("LRT_HardMinPt", m_LRTHardMinPt=0.0,"Minimum pT for tracks to be saved in LRT Mode");

  // UTT
  declareProperty("doHitDV",           m_doHitDV           = false);
  declareProperty("doHitDV_Seeding",   m_doHitDV_Seeding   = false);
  declareProperty("dodEdxTrk",         m_dodEdxTrk         = false);
  declareProperty("doDisappearingTrk", m_doDisappearingTrk = false);
  declareProperty("DisTrackFitter",    m_disTrkFitter );
}

//--------------------------------------------------------------------------

TrigFastTrackFinder::~TrigFastTrackFinder() {}

//-----------------------------------------------------------------------

StatusCode TrigFastTrackFinder::initialize() {

  ATH_CHECK(m_roiCollectionKey.initialize() );
  ATH_CHECK(m_outputTracksKey.initialize() );

  // optional input tracks collection if present the clusters on previously found tracks are not used to form seeds
  if (m_LRTmode) {
    ATH_CHECK(m_inputTracksKey.initialize( !m_inputTracksKey.key().empty() ) );
  }
  // optional PRD to track association map
  ATH_CHECK(m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ));


  ATH_CHECK(m_beamSpotKey.initialize());



  ATH_MSG_DEBUG(" TrigFastTrackFinder : MinHits set to " << m_minHits);

  ATH_CHECK(m_numberingTool.retrieve());

  ATH_CHECK(m_spacePointTool.retrieve());

  ATH_CHECK(m_trackMaker.retrieve());

  ATH_CHECK(m_trigInDetTrackFitter.retrieve());

  if (m_doZFinder) {
    ATH_CHECK(m_trigZFinder.retrieve());
  }
  else {
    m_trigZFinder.disable();
  }

  ATH_CHECK(m_trackSummaryTool.retrieve());

  //Get ID helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));

  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));

  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));

  // monitoring
  if ( !m_monTool.empty() ) {
     ATH_CHECK(m_monTool.retrieve() );
  }
  else {
     ATH_MSG_INFO("Monitoring tool is empty");
  }

  ATH_MSG_DEBUG(" doResMon " << m_doResMonitoring);

  if(m_useGPU) {//for GPU acceleration
    ATH_CHECK(m_accelSvc.retrieve());
    ATH_CHECK(m_accelSvc->isReady());
    ATH_CHECK(m_accelTool.retrieve());
  }

  ATH_MSG_INFO("Use GPU acceleration : "<<std::boolalpha<<m_useGPU);

  if (m_LRTmode) {
    ATH_MSG_INFO(" FTF configures in Large Radius Tracking Mode");
    // set TrigTrackSeedGenerator to LRTmode
    m_tcs.m_LRTmode=m_LRTmode;

  }

  if(m_tcs.m_useTrigSeedML > 0) {
    //LUT params
    int lut_w = 30;
    int lut_h = 45;
    float lut_range[4] = {0.0,3.0,0.0,9.0};
    TrigSeedML_LUT L(1,lut_w,lut_h,lut_range);
    //read data from LUT file
    std::string lut_fileName = PathResolver::find_file(m_trigseedML_LUT, "DATAPATH");
    if (lut_fileName.empty()) {
      ATH_MSG_ERROR("Cannot find TrigSeedML LUT file " << lut_fileName);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO(lut_fileName);
      std::ifstream ifs(lut_fileName.c_str());
      int row, col0, col1;
      while(!ifs.eof()) {
        ifs >> row >> col0 >> col1;
        if(ifs.eof()) break;
        for(int c=col0;c<=col1;c++) L.setBin(row, c);
      }
      ifs.close();
      ATH_MSG_INFO("TrigSeedML LUT initialized from file " << m_trigseedML_LUT);
      m_tcs.m_vLUT.push_back(L);
    }
  }

  // UTT tools
  if( m_doDisappearingTrk ) {
     ATH_CHECK(m_extrapolator.retrieve());
     ATH_MSG_DEBUG("Retrieved tool " << m_extrapolator);

     ATH_CHECK(m_disTrkFitter.retrieve());
     ATH_MSG_DEBUG("Retrieved tool " << m_disTrkFitter);
  }

  // UTT read/write handles
  ATH_CHECK( m_recJetRoiCollectionKey.initialize(m_doHitDV) );
  ATH_CHECK( m_hitDVTrkKey.initialize(m_doHitDV) ) ;
  ATH_CHECK( m_hitDVSPKey.initialize(m_doHitDV) );
  ATH_CHECK( m_dEdxTrkKey.initialize(m_dodEdxTrk) );
  ATH_CHECK( m_dEdxHitKey.initialize(m_dodEdxTrk) );
  ATH_CHECK( m_disTrkCandKey.initialize(m_doDisappearingTrk) );

  //
  ATH_MSG_DEBUG("FTF : " << name()  );
  ATH_MSG_DEBUG("	m_tcs.m_doubletFilterRZ    : " <<  m_tcs.m_doubletFilterRZ     );
  ATH_MSG_DEBUG("	m_tcs.m_doublet_dR_Max     : " <<  m_tcs.m_doublet_dR_Max      );
  ATH_MSG_DEBUG("	m_tcs.m_doublet_dR_Max_Confirm  : " << m_tcs.m_doublet_dR_Max_Confirm   );
  ATH_MSG_DEBUG("	m_tcs.m_seedRadBinWidth    : " <<  m_tcs.m_seedRadBinWidth     );
  ATH_MSG_DEBUG("	m_tcs.m_tripletD0Max       : " <<  m_tcs.m_tripletD0Max        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletD0_PPS_Max  : " <<  m_tcs.m_tripletD0_PPS_Max   );
  ATH_MSG_DEBUG("	m_tcs.m_nMaxPhiSlice       : " <<  m_tcs.m_nMaxPhiSlice        );
  ATH_MSG_DEBUG("	m_tcs.m_maxTripletBufferLength  : " << m_tcs.m_maxTripletBufferLength   );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoPSS       : " <<  m_tcs.m_tripletDoPSS        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoPPS       : " <<  m_tcs.m_tripletDoPPS        );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDoConfirm   : " <<  m_tcs.m_tripletDoConfirm    );
  ATH_MSG_DEBUG("	m_tcs.m_curv_delta         : " <<  m_tcs.m_curv_delta          );
  ATH_MSG_DEBUG("	m_tcs.m_tripletDtCut       : " <<  m_tcs.m_tripletDtCut        );
  ATH_MSG_DEBUG("	m_tcs.m_useTrigSeedML      : " <<  m_tcs.m_useTrigSeedML       );
  ATH_MSG_DEBUG("	m_trigseedML_LUT           : " <<  m_trigseedML_LUT     );
  ATH_MSG_DEBUG("	m_tcs.m_maxEC_len          : " <<  m_tcs.m_maxEC_len    );
  ATH_MSG_DEBUG("	m_vertexSeededMode         : " <<  m_vertexSeededMode   );
  ATH_MSG_DEBUG("	m_doZFinder                : " <<  m_doZFinder          );
  ATH_MSG_DEBUG("	m_doZFinderOnly            : " <<  m_doZFinderOnly      );
  ATH_MSG_DEBUG("	m_doFastZVseeding          : " <<  m_doFastZVseeding    );
  ATH_MSG_DEBUG("	m_tcs.m_zvError            : " <<  m_tcs.m_zvError      );
  ATH_MSG_DEBUG("	m_storeZFinderVertices     : " <<  m_storeZFinderVertices   );
  ATH_MSG_DEBUG("	m_tripletMinPtFrac         : " <<  m_tripletMinPtFrac   );
  ATH_MSG_DEBUG("	m_pTmin                    : " <<  m_pTmin              );
  ATH_MSG_DEBUG("	m_initialD0Max             : " <<  m_initialD0Max       );
  ATH_MSG_DEBUG("	m_Z0Max                    : " <<  m_Z0Max              );
  ATH_MSG_DEBUG("	m_checkSeedRedundancy      : " <<  m_checkSeedRedundancy   );
  ATH_MSG_DEBUG("	m_minHits                  : " <<  m_minHits            );
  ATH_MSG_DEBUG("	"  );
  ATH_MSG_DEBUG("	m_useBeamSpot              : " <<  m_useBeamSpot       );
  ATH_MSG_DEBUG("	m_nfreeCut                 : " <<  m_nfreeCut          );
  ATH_MSG_DEBUG("	m_spacePointTool           : " <<  m_spacePointTool    );
  ATH_MSG_DEBUG("	m_numberingTool            : " <<  m_numberingTool     );
  ATH_MSG_DEBUG("	m_trackMaker               : " <<  m_trackMaker        );
  ATH_MSG_DEBUG("	m_trigInDetTrackFitter     : " <<  m_trigInDetTrackFitter   );
  ATH_MSG_DEBUG("	m_trigZFinder              : " <<  m_trigZFinder       );
  ATH_MSG_DEBUG("	m_trackSummaryTool         : " <<  m_trackSummaryTool  );
  ATH_MSG_DEBUG("	m_doResMonitoring          : " <<  m_doResMonitoring   );
  ATH_MSG_DEBUG("	m_doCloneRemoval           : " <<  m_doCloneRemoval    );
  ATH_MSG_DEBUG("	m_useNewLayerNumberScheme  : " <<  m_useNewLayerNumberScheme   );
  ATH_MSG_DEBUG("	m_useGPU                   : " <<  m_useGPU            );
  ATH_MSG_DEBUG("	m_LRTmode                  : " <<  m_LRTmode           );
  ATH_MSG_DEBUG("	m_doHitDV                  : " <<  m_doHitDV      );
  ATH_MSG_DEBUG("	m_dodEdxTrk                : " <<  m_dodEdxTrk         );

  ATH_MSG_DEBUG(" Initialized successfully");


  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------

StatusCode TrigFastTrackFinder::start()
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

  m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;

  return StatusCode::SUCCESS;
}


StatusCode TrigFastTrackFinder::execute(const EventContext& ctx) const {

  //RoI preparation/update

  SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey, ctx);

  ATH_CHECK(roiCollection.isValid());

  TrigRoiDescriptor internalRoI;

  if ( roiCollection->size()>1 ) ATH_MSG_WARNING( "More than one Roi in the collection: " << m_roiCollectionKey << ", this is not supported - use a composite Roi" );

  if ( roiCollection->size()>0) {
      if ( !m_useBeamSpotForRoiZwidth) {
          internalRoI = **roiCollection->begin();
      }else{
          SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };

          int beamSpotBitMap = beamSpotHandle->beamStatus();
          bool isOnlineBeamspot = ((beamSpotBitMap & 0x4) == 0x4);

          if ((isOnlineBeamspot && (beamSpotBitMap & 0x3) == 0x3) || !isOnlineBeamspot){ //converged or MC event, if the original RoI has a zed > 3 sig + 10 then set it to 3 sig + 10.
              TrigRoiDescriptor originRoI = **roiCollection->begin();
              double beamSpot_zsig = beamSpotHandle->beamSigma(2);
              Amg::Vector3D vertex = beamSpotHandle->beamPos();
              double zVTX = vertex.z();
              double origin_zedPlus  = originRoI.zedPlus() ;  //!< z at the most forward end of the RoI
              double origin_zedMinus = originRoI.zedMinus();  //!< z at the most backward end of the RoI

              double new_zedMargin = 10.;
              double new_zedRange  = 3.;

              double new_zedPlus  = zVTX + beamSpot_zsig * new_zedRange + new_zedMargin;
              double new_zedMinus = zVTX - beamSpot_zsig * new_zedRange - new_zedMargin;

              if (origin_zedPlus > new_zedPlus && origin_zedMinus < new_zedMinus){
                  ATH_MSG_DEBUG("Updated RoI with zed = "<<new_zedRange<<" * sig + "<<new_zedMargin);
                  double origin_etaPlus  = originRoI.etaPlus() ;    //!< gets eta at zedPlus
                  double origin_etaMinus = originRoI.etaMinus();    //!< gets eta at zMinus
                  double origin_phiPlus  = originRoI.phiPlus() ;     //!< gets phiPlus
                  double origin_phiMinus = originRoI.phiMinus();    //!< gets phiMinus
                  double origin_eta      = originRoI.eta();    //!< gets eta at zMinus
                  double origin_phi      = originRoI.phi() ;     //!< gets phiPlus
                  unsigned int origin_roiId   = originRoI.roiId() ;
                  unsigned int origin_l1Id    = originRoI.l1Id() ;
                  unsigned int origin_roiWord = originRoI.roiWord();
                  internalRoI = TrigRoiDescriptor(origin_roiWord, origin_l1Id, origin_roiId, origin_eta, origin_etaMinus, origin_etaPlus, origin_phi, origin_phiMinus, origin_phiPlus, zVTX, new_zedMinus, new_zedPlus);
                  if (originRoI.isFullscan()) internalRoI.setFullscan(true);
              }
              else internalRoI = **roiCollection->begin(); // we have a more narrow zed range in RoI, no need to update.
          }else{ //Not converged, set to the fullScan RoI
                internalRoI = **roiCollection->begin();
          }
      }
  }


  //  internalRoI.manageConstituents(false);//Don't try to delete RoIs at the end
  m_countTotalRoI++;

  SG::WriteHandle<TrackCollection> outputTracks(m_outputTracksKey, ctx);
  outputTracks = std::make_unique<TrackCollection>();

  const TrackCollection* inputTracks = nullptr;
  if (m_LRTmode) {
    if (!m_inputTracksKey.key().empty()){
      SG::ReadHandle<TrackCollection> inputTrackHandle(m_inputTracksKey, ctx);
      ATH_CHECK(inputTrackHandle.isValid());
      inputTracks = inputTrackHandle.ptr();
    }
  }
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap, ctx);
  ATH_CHECK(findTracks(trackEventData, internalRoI, inputTracks, *outputTracks, ctx));

  return StatusCode::SUCCESS;
}



StatusCode TrigFastTrackFinder::findTracks(InDet::SiTrackMakerEventData_xk &trackEventData,
                                           const TrigRoiDescriptor& roi,
                                           const TrackCollection* inputTracks,
                                           TrackCollection& outputTracks,
                                           const EventContext& ctx) const {

  ATH_MSG_DEBUG("Input RoI " << roi);

  auto mnt_roi_nTracks = Monitored::Scalar<int>("roi_nTracks", 0);
  auto mnt_roi_nSPs    = Monitored::Scalar<int>("roi_nSPs",    0);
  auto mnt_roi_nSPsPIX = Monitored::Scalar<int>("roi_nSPsPIX", 0);
  auto mnt_roi_nSPsSCT = Monitored::Scalar<int>("roi_nSPsSCT", 0);
  auto monSP = Monitored::Group(m_monTool, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT);

  auto mnt_timer_Total                 = Monitored::Timer<std::chrono::milliseconds>("TIME_Total");
  auto mnt_timer_SpacePointConversion  = Monitored::Timer<std::chrono::milliseconds>("TIME_SpacePointConversion");
  auto mnt_timer_PatternReco           = Monitored::Timer<std::chrono::milliseconds>("TIME_PattReco");
  auto mnt_timer_TripletMaking         = Monitored::Timer<std::chrono::milliseconds>("TIME_Triplets");
  auto mnt_timer_CombTracking          = Monitored::Timer<std::chrono::milliseconds>("TIME_CmbTrack");
  auto mnt_timer_TrackFitter           = Monitored::Timer<std::chrono::milliseconds>("TIME_TrackFitter");
  auto mnt_timer_HitDV                 = Monitored::Timer<std::chrono::milliseconds>("TIME_HitDV");
  auto mnt_timer_dEdxTrk               = Monitored::Timer<std::chrono::milliseconds>("TIME_dEdxTrk");
  auto mnt_timer_disTrkZVertex         = Monitored::Timer<std::chrono::milliseconds>("TIME_disTrkZVertex");
  auto mnt_timer_disTrk                = Monitored::Timer<std::chrono::milliseconds>("TIME_disappearingTrack");
  auto monTime = Monitored::Group(m_monTool, mnt_roi_nTracks, mnt_roi_nSPs, mnt_timer_Total, mnt_timer_SpacePointConversion,
				  mnt_timer_PatternReco, mnt_timer_TripletMaking, mnt_timer_CombTracking, mnt_timer_TrackFitter,
				  mnt_timer_HitDV, mnt_timer_dEdxTrk, mnt_timer_disTrkZVertex, mnt_timer_disTrk);

  auto mnt_roi_lastStageExecuted = Monitored::Scalar<int>("roi_lastStageExecuted", 0);
  auto monDataError              = Monitored::Group(m_monTool, mnt_roi_lastStageExecuted);

  mnt_timer_Total.start();
  mnt_timer_SpacePointConversion.start();


  mnt_roi_lastStageExecuted = 1;

  std::vector<TrigSiSpacePointBase> convertedSpacePoints;
  convertedSpacePoints.reserve(5000);

  std::map<Identifier, std::vector<long int> > siClusterMap;

  if (m_LRTmode) {
    // In LRT mode read the input track collection and enter the clusters on track into the cluster map so these are not used for seeding
    if (!m_inputTracksKey.key().empty()) {
      ATH_MSG_DEBUG("LRT Mode: Got input track collection with "<<inputTracks->size()<< "tracks");
      long int trackIndex=0;
      for (auto t:*inputTracks) {
	updateClusterMap(trackIndex++, t, siClusterMap);
      }
    }
    ATH_CHECK(m_spacePointTool->getSpacePoints(roi, convertedSpacePoints, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT, ctx, &siClusterMap));
  } else {
    ATH_CHECK(m_spacePointTool->getSpacePoints(roi, convertedSpacePoints, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT, ctx));
  }


  mnt_timer_SpacePointConversion.stop();
  mnt_roi_nSPs    = mnt_roi_nSPsPIX + mnt_roi_nSPsSCT;

  if( mnt_roi_nSPs >= m_minHits ) {
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPs << " space points.");
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPsPIX << " Pixel space points.");
    ATH_MSG_DEBUG("REGTEST / Found " << mnt_roi_nSPsSCT << " SCT space points.");
    m_countRoIwithEnoughHits++;
  }
  else {
    ATH_MSG_DEBUG("No tracks found - too few hits in ROI to run " << mnt_roi_nSPs);
    ATH_CHECK( createEmptyUTTEDMs(ctx) );
    return StatusCode::SUCCESS;
  }

  mnt_roi_lastStageExecuted = 2;


  /// this uses move semantics so doesn't do a deep copy, so ...
  std::unique_ptr<TrigRoiDescriptor> tmpRoi = std::make_unique<TrigRoiDescriptor>(roi);
  /// need to disable managment of the constituents
  tmpRoi->manageConstituents(false);
  auto vertices = std::make_unique<TrigVertexCollection>();
  std::vector<float> vZv;

  if (m_doZFinder) {
    auto mnt_timer_ZFinder = Monitored::Timer<std::chrono::milliseconds>("TIME_ZFinder");
    auto monTimeZFinder    = Monitored::Group(m_monTool, mnt_timer_ZFinder);
    mnt_timer_ZFinder.start();

    /// create a new internal superRoi - should really record this
    tmpRoi = std::make_unique<TrigRoiDescriptor>(true);
    tmpRoi->setComposite(true);

    vertices = std::make_unique<TrigVertexCollection>(*m_trigZFinder->findZ( convertedSpacePoints, roi));

    ATH_MSG_DEBUG("vertices->size(): " << vertices->size());


    if ( m_doFastZVseeding ) {
      vZv.reserve(vertices->size());
      for (const auto vertex : *vertices) {
        ATH_MSG_DEBUG("REGTEST / ZFinder vertex: " << *vertex);
        float z      = vertex->z();
        float zMinus = z - 7.0;
        float zPlus  = z + 7.0;
        TrigRoiDescriptor* newRoi =  new TrigRoiDescriptor(roi.eta(), roi.etaMinus(), roi.etaPlus(),
            roi.phi(), roi.phiMinus(), roi.phiPlus(), z, zMinus, zPlus);
        tmpRoi->push_back(newRoi);
        vZv.push_back(z);
      }

      ATH_MSG_DEBUG("REGTEST / tmpRoi: " << *tmpRoi);
    }

    mnt_timer_ZFinder.stop();

    if (  m_doZFinderOnly ) {
      /// write vertex collection ...
      /// TODO: add vertices collection handling here,
      /// should not be 0 at this point unless fastZVseeding
      /// is enabled
      ATH_CHECK( createEmptyUTTEDMs(ctx) );
      return StatusCode::SUCCESS;
    }
  }


  mnt_roi_lastStageExecuted = 3;

  mnt_timer_PatternReco.start();

  mnt_timer_TripletMaking.start();

  std::vector<TrigInDetTriplet> triplets;


  if(!m_useGPU) {
    TRIG_TRACK_SEED_GENERATOR seedGen(m_tcs);

    seedGen.loadSpacePoints(convertedSpacePoints);

    if (m_doZFinder && m_doFastZVseeding) {
      seedGen.createSeeds(tmpRoi.get(), vZv);
    }
    else {
      seedGen.createSeeds(tmpRoi.get());
    }

    seedGen.getSeeds(triplets);
  }
  else {
    //GPU offloading begins ...

    makeSeedsOnGPU(m_tcs, tmpRoi.get(), convertedSpacePoints, triplets);

    //GPU offloading ends ...
  }

  ATH_MSG_DEBUG("number of triplets: " << triplets.size());
  mnt_timer_TripletMaking.stop();
  mnt_roi_lastStageExecuted = 4;

  mnt_timer_CombTracking.start();

  // 8. Combinatorial tracking

  std::vector<std::tuple<bool, double,Trk::Track*>> qualityTracks; //bool used for later filtering
  qualityTracks.reserve(triplets.size());

  auto mnt_roi_nSeeds  = Monitored::Scalar<int>("roi_nSeeds",  0);
  auto monTrk_seed = Monitored::Group(m_monTool, mnt_roi_nSeeds);

  long int trackIndex=0;

  bool PIX = true;
  bool SCT = true;

  if( m_doDisappearingTrk ) trackEventData.combinatorialData().setFlagToReturnFailedTrack(true);
  m_trackMaker->newTrigEvent(ctx, trackEventData, PIX, SCT);

  std::vector<Trk::Track*> disFailTrks;
  std::vector<Trk::Track*> disCombTrks;
  std::vector<std::tuple<bool, double, Trk::Track*>> qualityDisFailTrks;
  std::vector<std::tuple<bool, double, Trk::Track*>> qualityDisCombTrks;
  int disTrk_n_disCombTrks=0;
  int disTrk_n_disCombTrks_cleaning=0;
  int disTrk_n_disFailTrks=0;
  int disTrk_n_disFailTrks_cleaning=0;

  for(unsigned int tripletIdx=0;tripletIdx!=triplets.size();tripletIdx++) {

    const TrigInDetTriplet &seed = triplets[tripletIdx];

    const Trk::SpacePoint* osp1 = seed.s1().offlineSpacePoint();
    const Trk::SpacePoint* osp2 = seed.s2().offlineSpacePoint();
    const Trk::SpacePoint* osp3 = seed.s3().offlineSpacePoint();

    if(m_checkSeedRedundancy) {
      //check if clusters do not belong to any track
      std::vector<Identifier> clusterIds;
      extractClusterIds(osp1, clusterIds);
      extractClusterIds(osp2, clusterIds);
      extractClusterIds(osp3, clusterIds);
      if(usedByAnyTrack(clusterIds, siClusterMap)) {
        continue;
      }
    }

    std::vector<const Trk::SpacePoint*> spVec = {osp1, osp2, osp3};

    ++mnt_roi_nSeeds;

    std::list<Trk::Track*> tracks;
    std::list<Trk::Track*> tracksFail;
    std::list<Trk::Track*> tracksAll = m_trackMaker->getTracks(ctx, trackEventData, spVec);
    auto resultCode = trackEventData.combinatorialData().resultCode();
    if( ! m_doDisappearingTrk || (m_doDisappearingTrk && resultCode==InDet::SiCombinatorialTrackFinderData_xk::ResultCode::Success) ) {
       tracks = tracksAll;
    }
    else {
       tracksFail = tracksAll;
    }
    if( m_doDisappearingTrk ) {
       ATH_MSG_VERBOSE("size of tracks=" << tracks.size() << ", tracksFail=" << tracksFail.size() << ": resultCode=" << resultCode);
       for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
	  if( ! (*t) ) continue;
	  m_trackSummaryTool->updateTrack(**t);
	  disTrk_n_disCombTrks++;
	  if( (*t)->perigeeParameters()!=0 && isCleaningPassDisTrack(seed, (*t), false) ) {
	     ATH_MSG_VERBOSE("... combTrk, cleaningPass");
	     disTrk_n_disCombTrks_cleaning++;
	     disCombTrks.push_back((*t));
	     qualityDisCombTrks.emplace_back(std::make_tuple(true, -disTrackQuality((*t)), (*t)));
	  }
       }
       for(std::list<Trk::Track*>::const_iterator t=tracksFail.begin(); t!=tracksFail.end(); ++t) {
	  if( ! (*t) ) continue;
	  m_trackSummaryTool->updateTrack(**t);
	  disTrk_n_disFailTrks++;
	  if( (*t)->perigeeParameters()!=0 && isCleaningPassDisTrack(seed, (*t), true) ) {
	     ATH_MSG_VERBOSE("... failTrk, cleaningPass");
	     disTrk_n_disFailTrks_cleaning++;
	     disFailTrks.push_back((*t));
	     qualityDisFailTrks.emplace_back(std::make_tuple(true, -disTrackQuality((*t)), (*t)));
	  }
	  else {
	     delete(*t); // delete failed trk but not disFailTrk candidate
	  }
       }
    }

    for(std::list<Trk::Track*>::const_iterator t=tracks.begin(); t!=tracks.end(); ++t) {
      if((*t)) {
        float d0 = (*t)->perigeeParameters()==0 ? 10000.0 : (*t)->perigeeParameters()->parameters()[Trk::d0];
        if (std::abs(d0) > m_initialD0Max) {
          ATH_MSG_DEBUG("REGTEST / Reject track with d0 = " << d0 << " > " << m_initialD0Max);
          qualityTracks.push_back(std::make_tuple(false,0,(*t)));//Flag track as bad, but keep in vector for later deletion
          continue;
        }
        if(m_checkSeedRedundancy) {
          //update clusterMap
          updateClusterMap(trackIndex++, (*t), siClusterMap);
        }
        if(m_doCloneRemoval) {
          qualityTracks.push_back(std::make_tuple(true, -trackQuality((*t)), (*t)));
        }
        else {
          qualityTracks.push_back(std::make_tuple(true, 0, (*t)));
        }
      }
    }
    ATH_MSG_VERBOSE("Found "<<tracks.size()<<" tracks using triplet");
  }

  if( m_doDisappearingTrk ) {
     ATH_MSG_DEBUG("===> nr of disFailTrks=" << disTrk_n_disFailTrks << " -> cleaning pass=" << disTrk_n_disFailTrks_cleaning);
     ATH_MSG_DEBUG("===> nr of disCombTrks=" << disTrk_n_disCombTrks << " -> cleaning pass=" << disTrk_n_disCombTrks_cleaning);
  }

  m_trackMaker->endEvent(trackEventData);

  //clone removal
  if(m_doCloneRemoval) {
    filterSharedTracks(qualityTracks);
  }

  // filter shared hits
  if( m_doDisappearingTrk ) {
    filterSharedDisTracks(qualityDisFailTrks);
    filterSharedDisTracks(qualityDisCombTrks);
  }

  TrackCollection initialTracks;
  initialTracks.reserve(qualityTracks.size());

  TrackCollection extraDisCombTracks;
  // if( m_doDisappearingTrk ) extraDisCombTracks.reserve(qualityTracks.size());

  unsigned int idx=0;
  std::vector<unsigned int> indexDisCombTrk;
  for(const auto& q : qualityTracks) {
    bool needed_for_disCombTrk = false;
    if( m_doDisappearingTrk ) {
       Trk::Track* trk_q = std::get<2>(q);
       for(const auto& qdis : qualityDisCombTrks ) {
	  if( std::get<2>(qdis) == trk_q ) {
	     needed_for_disCombTrk = std::get<0>(qdis);
	     break;
	  }
       }
       if( needed_for_disCombTrk) ATH_MSG_VERBOSE("idx=" << idx << " ===> neded for disCombTrk");
    }
    if (std::get<0>(q)==true) {
      initialTracks.push_back(std::get<2>(q));
      if( m_doDisappearingTrk && needed_for_disCombTrk ) indexDisCombTrk.push_back(idx);
    }
    else {
      if( ! m_doDisappearingTrk ) {
	 delete std::get<2>(q);
      }
      else {
	 if( needed_for_disCombTrk ) {
	    ATH_MSG_VERBOSE("... adding to extraDisCombTracks");
	    extraDisCombTracks.push_back(std::get<2>(q));
	 }
	 else {
	    delete std::get<2>(q);
	 }
      }
    }
    idx++;
  }
  qualityTracks.clear();

  ATH_MSG_DEBUG("After clone removal "<<initialTracks.size()<<" tracks left");


  mnt_timer_CombTracking.stop();
  mnt_timer_PatternReco.stop();

  mnt_roi_lastStageExecuted = 5;

  mnt_timer_TrackFitter.start();

  if( ! m_dodEdxTrk ) {
     m_trigInDetTrackFitter->fit(initialTracks, outputTracks, ctx, m_particleHypothesis);
  }
  else {
     TrackCollection outputTrackswTP;
     m_trigInDetTrackFitter->fit(initialTracks, outputTracks, outputTrackswTP, ctx, m_particleHypothesis, true); // add TP to TSoS for dEdx

     // large dEdx finding
     mnt_timer_dEdxTrk.start();
     for(auto t=outputTrackswTP.begin(); t!=outputTrackswTP.end();t++) { m_trackSummaryTool->updateTrack(**t); }
     ATH_CHECK( finddEdxTrk(ctx,outputTrackswTP) );
  }

  if( m_dodEdxTrk ) mnt_timer_dEdxTrk.stop(); // to include timing to destroy TrackCollection object

  if( outputTracks.empty() ) {
     ATH_MSG_DEBUG("REGTEST / No tracks fitted");
  }

  bool do_recoverDisCombTrk = true;
  if( m_doDisappearingTrk && (initialTracks.size()!=outputTracks.size()) ) {
     ATH_MSG_DEBUG("part of initialTracks fails in fitting. do not try to recover DisCombTracks");
     do_recoverDisCombTrk = false;
  }

  TrackCollection fittedExtraDisCombTracks;
  fittedExtraDisCombTracks.reserve(extraDisCombTracks.size());
  TrackCollection fittedDisCombTrks(SG::VIEW_ELEMENTS);
  if( m_doDisappearingTrk ) {
     ATH_MSG_VERBOSE("nr of extraDisCombTracks=" << extraDisCombTracks.size());
     if( extraDisCombTracks.size() > 0 ) {
	ATH_MSG_VERBOSE("fitting extraDisCombTracks ...");
	m_trigInDetTrackFitter->fit(extraDisCombTracks, fittedExtraDisCombTracks, ctx, m_particleHypothesis);
	for (auto fittedTrack = fittedExtraDisCombTracks.begin(); fittedTrack!=fittedExtraDisCombTracks.end(); ++fittedTrack) {
	   (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
	   m_trackSummaryTool->updateTrack(**fittedTrack);
	   fittedDisCombTrks.push_back(*fittedTrack);
	}
     }
  }

  size_t counter(1);
  idx = 0;
  for ( auto fittedTrack = outputTracks.begin(); fittedTrack!=outputTracks.end(); ) {
    if ((*fittedTrack)->perigeeParameters()){
      float d0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::d0];
      float z0 = (*fittedTrack)->perigeeParameters()->parameters()[Trk::z0];
      if (std::abs(d0) > m_initialD0Max || std::abs(z0) > m_Z0Max) {
        if(m_LRTmode){
          ATH_MSG_DEBUG("REGTEST / Reject track after fit with d0 = " << d0 << " z0= "  << z0
              << " larger than limits (" << m_initialD0Max << ", " << m_Z0Max << ")");
        }else{
          ATH_MSG_WARNING("REGTEST / Reject track after fit with d0 = " << d0 << " z0= "  << z0
              << " larger than limits (" << m_initialD0Max << ", " << m_Z0Max << ")");
        }
        ATH_MSG_DEBUG(**fittedTrack);
        fittedTrack = outputTracks.erase(fittedTrack);
        continue;
      }

      if(m_LRTmode){
        //reject tracks which have a d0 below a cut but only when an input track collection (from ftf) is also present
        if(m_LRTD0Min>0.0){
          if(std::abs(d0) < m_LRTD0Min && !m_inputTracksKey.key().empty()){
            ATH_MSG_DEBUG("REGTEST / Reject track after fit for min d0 (" << d0 << " < " << m_LRTD0Min <<")");
            fittedTrack = outputTracks.erase(fittedTrack);
            continue;
          }
        }

        //calculate pt
        float trkPt = 0.0;
        if(m_LRTHardMinPt > 0.0){
          //avoid a floating poitn error
          if(std::abs((*fittedTrack)->perigeeParameters()->parameters()[Trk::qOverP]) >= 1e-9){
            trkPt = std::sin((*fittedTrack)->perigeeParameters()->parameters()[Trk::theta])/std::abs((*fittedTrack)->perigeeParameters()->parameters()[Trk::qOverP]);

            if(trkPt < m_LRTHardMinPt){
              ATH_MSG_DEBUG("REGTEST / Reject track after fit for min pt (" << trkPt << " < " << m_LRTHardMinPt <<")");
              fittedTrack = outputTracks.erase(fittedTrack);
              continue;
            }
          }
        }
      }
    }

    (*fittedTrack)->info().setPatternRecognitionInfo(Trk::TrackInfo::FastTrackFinderSeed);
    ATH_MSG_VERBOSE("Updating fitted track: " << counter);
    ATH_MSG_VERBOSE(**fittedTrack);
    m_trackSummaryTool->updateTrack(**fittedTrack);
    ATH_MSG_VERBOSE("Updated track: " << counter);
    ATH_MSG_VERBOSE(**fittedTrack);

    if( m_doDisappearingTrk && do_recoverDisCombTrk ) {
       if( std::find(indexDisCombTrk.begin(),indexDisCombTrk.end(),idx)!=indexDisCombTrk.end() ) {
	  ATH_MSG_VERBOSE("fittedTrack idx=" << idx << ": recovers also for DisCombTrack");
	  fittedDisCombTrks.push_back(*fittedTrack);
       }
    }

    counter++; fittedTrack++;
    idx++;
  }

  mnt_timer_TrackFitter.stop();
  mnt_timer_Total.stop();

  if( outputTracks.empty() ) {
    ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
  }
  mnt_roi_lastStageExecuted = 6;

  // z-vertex for UTT
  std::vector<double> disTrk_v_xVtx;
  std::vector<double> disTrk_v_yVtx;
  std::vector<double> disTrk_v_zVtx;
  if( m_doDisappearingTrk ) {
     mnt_timer_disTrkZVertex.start();
     recoVertexForDisTrack(ctx, outputTracks, disTrk_v_xVtx, disTrk_v_yVtx, disTrk_v_zVtx);
     mnt_timer_disTrkZVertex.stop();
  }

  // disappearing track reco
  if( m_doDisappearingTrk ) {
     mnt_timer_disTrk.start();
     ATH_CHECK( findDisTracks(ctx,outputTracks,qualityDisFailTrks,qualityDisCombTrks,fittedDisCombTrks,disTrk_v_xVtx,disTrk_v_yVtx,disTrk_v_zVtx) );
     mnt_timer_disTrk.stop();
  }

  // find Hit-based displaced vertex
  if( m_doHitDV ) {
     mnt_timer_HitDV.start();
     ATH_CHECK( findHitDV(ctx,convertedSpacePoints,outputTracks) );
     mnt_timer_HitDV.stop();
  }

  //monitor Z-vertexing

  //monitor number of tracks
  ATH_MSG_DEBUG("REGTEST / Found " << outputTracks.size() << " tracks");
  if( !outputTracks.empty() )
    m_countRoIwithTracks++;

  ///////////// fill vectors of quantities to be monitored
  fillMon(outputTracks, *vertices, roi, ctx);

  mnt_roi_lastStageExecuted = 7;

  return StatusCode::SUCCESS;
}

double TrigFastTrackFinder::trackQuality(const Trk::Track* Tr) const {

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

void TrigFastTrackFinder::filterSharedTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) const {

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

StatusCode TrigFastTrackFinder::finalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinder::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)   : " << m_countRoIwithTracks);
  ATH_MSG_INFO("=========================================================");

  return StatusCode::SUCCESS;
}

void TrigFastTrackFinder::updateClusterMap(long int trackIdx, const Trk::Track* pTrack, std::map<Identifier, std::vector<long int> >& clusterMap) const {
  //loop over clusters

  for(auto tMOT = pTrack->measurementsOnTrack()->begin(); tMOT != pTrack->measurementsOnTrack()->end(); ++tMOT) {

    const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT);
    if (siCLOT==nullptr) continue;
    const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData());
    if (siCL==nullptr) continue;
    Identifier id = siCL->identify();
    clusterMap[id].push_back(trackIdx);
  }
}

void TrigFastTrackFinder::extractClusterIds(const Trk::SpacePoint* pSP, std::vector<Identifier>& vIds) const {
  const InDet::SiCluster* pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().first);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
  //check second cluster : SCT uv clusters only !
  pCL = dynamic_cast<const InDet::SiCluster*>(pSP->clusterList().second);
  if(pCL!=nullptr) vIds.push_back(pCL->identify());
}

bool TrigFastTrackFinder::usedByAnyTrack(const std::vector<Identifier>& vIds, std::map<Identifier, std::vector<long int> >& clusterMap) const {

  std::vector<long int> xSection;
  //initializing
  std::map<Identifier, std::vector<long int> >::iterator itm0 = clusterMap.find(*vIds.begin());
  if(itm0 == clusterMap.end()) return false;
  xSection.reserve((*itm0).second.size());
  std::copy((*itm0).second.begin(), (*itm0).second.end(), std::back_inserter(xSection));
  std::vector<Identifier>::const_iterator it = vIds.begin();++it;
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

void TrigFastTrackFinder::getBeamSpot(float& shift_x, float& shift_y, const EventContext& ctx) const {
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  Amg::Vector3D vertex = beamSpotHandle->beamPos();
  ATH_MSG_VERBOSE("Beam spot position " << vertex);
  double xVTX = vertex.x();
  double yVTX = vertex.y();
  double zVTX = vertex.z();
  double tiltXZ = beamSpotHandle->beamTilt(0);
  double tiltYZ = beamSpotHandle->beamTilt(1);
  shift_x = xVTX - tiltXZ*zVTX;//correction for tilt
  shift_y = yVTX - tiltYZ*zVTX;//correction for tilt
  ATH_MSG_VERBOSE("Beam center position:  " << shift_x <<"  "<< shift_y);
}


void TrigFastTrackFinder::fillMon(const TrackCollection& tracks, const TrigVertexCollection& vertices,
                                  const TrigRoiDescriptor& roi, const EventContext& ctx) const {
  float shift_x = 0;
  float shift_y = 0;
  if(m_useBeamSpot) {
    getBeamSpot(shift_x, shift_y, ctx);
  }
  auto mnt_roi_eta      = Monitored::Scalar<float>("roi_eta",      0.0);
  auto mnt_roi_phi      = Monitored::Scalar<float>("roi_phi",      0.0);
  auto mnt_roi_etaWidth = Monitored::Scalar<float>("roi_etaWidth", 0.0);
  auto mnt_roi_phiWidth = Monitored::Scalar<float>("roi_phiWidth", 0.0);
  auto mnt_roi_z        = Monitored::Scalar<float>("roi_z",        0.0);
  auto mnt_roi_zWidth   = Monitored::Scalar<float>("roi_zWidith",  0.0);
  auto mnt_roi_nTracks  = Monitored::Scalar<int>("roi_nTracks", 0);
  auto monRoI           = Monitored::Group(m_monTool, mnt_roi_eta, mnt_roi_phi, mnt_roi_etaWidth, mnt_roi_phiWidth, mnt_roi_z, mnt_roi_zWidth, mnt_roi_nTracks);

  for(unsigned int i=0; i<roi.size(); i++) {
     mnt_roi_eta      = (roi.at(i))->eta();
     mnt_roi_phi      = (roi.at(i))->phi();
     mnt_roi_etaWidth = (roi.at(i))->etaPlus() - (roi.at(i))->etaMinus();
     mnt_roi_phiWidth = CxxUtils::wrapToPi((roi.at(i))->phiPlus() - (roi.at(i))->phiMinus());
     mnt_roi_z        = (roi.at(i))->zed();
     mnt_roi_zWidth   = (roi.at(i))->zedPlus() - (roi.at(i))->zedMinus();
  }

  std::vector<float> mnt_trk_pt;
  std::vector<float> mnt_trk_a0;
  std::vector<float> mnt_trk_z0;
  std::vector<float> mnt_trk_phi0;
  std::vector<float> mnt_trk_eta;
  std::vector<float> mnt_trk_chi2dof;
  std::vector<float> mnt_trk_nSiHits;
  std::vector<float> mnt_trk_nPIXHits;
  std::vector<float> mnt_trk_nSCTHits;
  std::vector<float> mnt_trk_a0beam;
  std::vector<float> mnt_trk_dPhi0;
  std::vector<float> mnt_trk_dEta;

  auto mon_pt       = Monitored::Collection("trk_pt",       mnt_trk_pt);
  auto mon_a0       = Monitored::Collection("trk_a0",       mnt_trk_a0);
  auto mon_z0       = Monitored::Collection("trk_z0",       mnt_trk_z0);
  auto mon_phi0     = Monitored::Collection("trk_phi0",     mnt_trk_phi0);
  auto mon_eta      = Monitored::Collection("trk_eta",      mnt_trk_eta);
  auto mon_chi2dof  = Monitored::Collection("trk_chi2dof",  mnt_trk_chi2dof);
  auto mon_nSiHits  = Monitored::Collection("trk_nSiHits",  mnt_trk_nSiHits);
  auto mon_nPIXHits = Monitored::Collection("trk_nPIXHits", mnt_trk_nPIXHits);
  auto mon_nSCTHits = Monitored::Collection("trk_nSCTHits", mnt_trk_nSCTHits);
  auto mon_a0beam   = Monitored::Collection("trk_a0beam",   mnt_trk_a0beam);
  auto mon_dPhi0    = Monitored::Collection("trk_dPhi0",    mnt_trk_dPhi0);
  auto mon_dEta     = Monitored::Collection("trk_dEta",     mnt_trk_dEta);
  auto monTrk       = Monitored::Group(m_monTool, mon_pt, mon_a0, mon_z0, mon_phi0, mon_eta, mon_chi2dof,
				       mon_nSiHits, mon_nPIXHits, mon_nSCTHits, mon_a0beam, mon_dPhi0, mon_dEta);

  std::vector<float> mnt_roi_zVertices;
  auto mon_roi_nZvertices = Monitored::Scalar<int>("roi_nZvertices", 0);
  auto mon_roi_zVertices  = Monitored::Collection("roi_zVertices", mnt_roi_zVertices);
  auto monVtx = Monitored::Group(m_monTool, mon_roi_nZvertices, mon_roi_zVertices);
  mon_roi_nZvertices = vertices.size();
  for (const auto vertex : vertices) {
    mnt_roi_zVertices.push_back(vertex->z());
  }
  mnt_roi_nTracks = tracks.size();


  for (auto track : tracks) {
    const Trk::TrackParameters* trackPars = track->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
    }

    float a0 = trackPars->parameters()[Trk::d0];
    float z0 = trackPars->parameters()[Trk::z0];
    float phi0 = trackPars->parameters()[Trk::phi0];
    float theta = trackPars->parameters()[Trk::theta];
    float eta = -log(tan(0.5*theta));
    mnt_trk_a0.push_back(a0);
    mnt_trk_z0.push_back(z0);
    mnt_trk_phi0.push_back(phi0);
    mnt_trk_a0beam.push_back(a0+shift_x*sin(phi0)-shift_y*cos(phi0));
    mnt_trk_eta.push_back(eta);
    for(unsigned int i=0; i<roi.size(); i++) {
       mnt_trk_dPhi0.push_back(CxxUtils::wrapToPi(phi0 - (roi.at(i))->phi()));
       mnt_trk_dEta.push_back(eta - (roi.at(i))->eta());
    }

    float qOverP = trackPars->parameters()[Trk::qOverP];
    if (qOverP==0) {
      ATH_MSG_DEBUG("REGTEST / q/p == 0, adjusting to 1e-12");
      qOverP = 1e-12;
    }
    float pT=sin(theta)/qOverP;

    const Trk::FitQuality* fq = track->fitQuality();
    float chi2 = 1e8;
    if (fq) {
      ATH_MSG_VERBOSE("Fitted chi2: " << fq->chiSquared());
      ATH_MSG_VERBOSE("Fitted ndof: " << fq->numberDoF());
      if(fq->numberDoF()!=0) {
        chi2 = fq->chiSquared()/fq->numberDoF();
      }
    }
    mnt_trk_pt.push_back(pT);
    mnt_trk_chi2dof.push_back(chi2);

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
    mnt_trk_nPIXHits.push_back(nPix);
    mnt_trk_nSCTHits.push_back(nSct/2);
    mnt_trk_nSiHits.push_back(nPix + nSct/2);

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
      runResidualMonitoring(*track, ctx);
    }
  }
}

void TrigFastTrackFinder::runResidualMonitoring(const Trk::Track& track, const EventContext& ctx) const {

  std::vector<float> mnt_layer_IBL;
  std::vector<float> mnt_layer_PixB;
  std::vector<float> mnt_layer_PixE;
  std::vector<float> mnt_layer_SCTB;
  std::vector<float> mnt_layer_SCTE;
  std::vector<float> mnt_hit_IBLPhiResidual;
  std::vector<float> mnt_hit_IBLEtaResidual;
  std::vector<float> mnt_hit_IBLPhiPull;
  std::vector<float> mnt_hit_IBLEtaPull;
  std::vector<float> mnt_hit_PIXBarrelPhiResidual;
  std::vector<float> mnt_hit_PIXBarrelEtaResidual;
  std::vector<float> mnt_hit_PIXBarrelPhiPull;
  std::vector<float> mnt_hit_PIXBarrelEtaPull;
  std::vector<float> mnt_hit_SCTBarrelResidual;
  std::vector<float> mnt_hit_SCTBarrelPull;
  std::vector<float> mnt_hit_PIXEndcapPhiResidual;
  std::vector<float> mnt_hit_PIXEndcapEtaResidual;
  std::vector<float> mnt_hit_PIXEndcapPhiPull;
  std::vector<float> mnt_hit_PIXEndcapEtaPull;
  std::vector<float> mnt_hit_SCTEndcapResidual;
  std::vector<float> mnt_hit_SCTEndcapPull;
  std::vector<float> mnt_hit_PIXBarrelL1PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL1EtaResidual;
  std::vector<float> mnt_hit_PIXBarrelL2PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL2EtaResidual;
  std::vector<float> mnt_hit_PIXBarrelL3PhiResidual;
  std::vector<float> mnt_hit_PIXBarrelL3EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL1PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL1EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL2PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL2EtaResidual;
  std::vector<float> mnt_hit_PIXEndcapL3PhiResidual;
  std::vector<float> mnt_hit_PIXEndcapL3EtaResidual;
  std::vector<float> mnt_hit_SCTBarrelL1PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL2PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL3PhiResidual;
  std::vector<float> mnt_hit_SCTBarrelL4PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL1PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL2PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL3PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL4PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL5PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL6PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL7PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL8PhiResidual;
  std::vector<float> mnt_hit_SCTEndcapL9PhiResidual;
  auto mon_layer_IBL  = Monitored::Collection("layer_IBL", mnt_layer_IBL);
  auto mon_layer_PixB = Monitored::Collection("layer_PixB",mnt_layer_PixB);
  auto mon_layer_PixE = Monitored::Collection("layer_PixE",mnt_layer_PixE);
  auto mon_layer_SCTB = Monitored::Collection("layer_SCTB",mnt_layer_SCTB);
  auto mon_layer_SCTE = Monitored::Collection("layer_SCTE",mnt_layer_SCTE);
  auto mon_hit_IBLPhiResidual = Monitored::Collection("hit_IBLPhiResidual",mnt_hit_IBLPhiResidual);
  auto mon_hit_IBLEtaResidual = Monitored::Collection("hit_IBLEtaResidual",mnt_hit_IBLEtaResidual);
  auto mon_hit_IBLPhiPull = Monitored::Collection("hit_IBLPhiPull",mnt_hit_IBLPhiPull);
  auto mon_hit_IBLEtaPull = Monitored::Collection("hit_IBLEtaPull",mnt_hit_IBLEtaPull);
  auto mon_hit_PIXBarrelPhiResidual = Monitored::Collection("hit_PIXBarrelPhiResidual",mnt_hit_PIXBarrelPhiResidual);
  auto mon_hit_PIXBarrelEtaResidual = Monitored::Collection("hit_PIXBarrelEtaResidual",mnt_hit_PIXBarrelEtaResidual);
  auto mon_hit_PIXBarrelPhiPull = Monitored::Collection("hit_PIXBarrelPhiPull",mnt_hit_PIXBarrelPhiPull);
  auto mon_hit_PIXBarrelEtaPull = Monitored::Collection("hit_PIXBarrelEtaPull",mnt_hit_PIXBarrelEtaPull);
  auto mon_hit_SCTBarrelResidual = Monitored::Collection("hit_SCTBarrelResidual",mnt_hit_SCTBarrelResidual);
  auto mon_hit_SCTBarrelPull = Monitored::Collection("hit_SCTBarrelPull",mnt_hit_SCTBarrelPull);
  auto mon_hit_PIXEndcapPhiResidual = Monitored::Collection("hit_PIXEndcapPhiResidual",mnt_hit_PIXEndcapPhiResidual);
  auto mon_hit_PIXEndcapEtaResidual = Monitored::Collection("hit_PIXEndcapEtaResidual",mnt_hit_PIXEndcapEtaResidual);
  auto mon_hit_PIXEndcapPhiPull = Monitored::Collection("hit_PIXEndcapPhiPull",mnt_hit_PIXEndcapPhiPull);
  auto mon_hit_PIXEndcapEtaPull = Monitored::Collection("hit_PIXEndcapEtaPull",mnt_hit_PIXEndcapEtaPull);
  auto mon_hit_SCTEndcapResidual = Monitored::Collection("hit_SCTEndcapResidual",mnt_hit_SCTEndcapResidual);
  auto mon_hit_SCTEndcapPull = Monitored::Collection("hit_SCTEndcapPull",mnt_hit_SCTEndcapPull);
  auto mon_hit_PIXBarrelL1PhiResidual = Monitored::Collection("hit_PIXBarrelL1PhiResidual",mnt_hit_PIXBarrelL1PhiResidual);
  auto mon_hit_PIXBarrelL1EtaResidual = Monitored::Collection("hit_PIXBarrelL1EtaResidual",mnt_hit_PIXBarrelL1EtaResidual);
  auto mon_hit_PIXBarrelL2PhiResidual = Monitored::Collection("hit_PIXBarrelL2PhiResidual",mnt_hit_PIXBarrelL2PhiResidual);
  auto mon_hit_PIXBarrelL2EtaResidual = Monitored::Collection("hit_PIXBarrelL2EtaResidual",mnt_hit_PIXBarrelL2EtaResidual);
  auto mon_hit_PIXBarrelL3PhiResidual = Monitored::Collection("hit_PIXBarrelL3PhiResidual",mnt_hit_PIXBarrelL3PhiResidual);
  auto mon_hit_PIXBarrelL3EtaResidual = Monitored::Collection("hit_PIXBarrelL3EtaResidual",mnt_hit_PIXBarrelL3EtaResidual);
  auto mon_hit_PIXEndcapL1PhiResidual = Monitored::Collection("hit_PIXEndcapL1PhiResidual",mnt_hit_PIXEndcapL1PhiResidual);
  auto mon_hit_PIXEndcapL1EtaResidual = Monitored::Collection("hit_PIXEndcapL1EtaResidual",mnt_hit_PIXEndcapL1EtaResidual);
  auto mon_hit_PIXEndcapL2PhiResidual = Monitored::Collection("hit_PIXEndcapL2PhiResidual",mnt_hit_PIXEndcapL2PhiResidual);
  auto mon_hit_PIXEndcapL2EtaResidual = Monitored::Collection("hit_PIXEndcapL2EtaResidual",mnt_hit_PIXEndcapL2EtaResidual);
  auto mon_hit_PIXEndcapL3PhiResidual = Monitored::Collection("hit_PIXEndcapL3PhiResidual",mnt_hit_PIXEndcapL3PhiResidual);
  auto mon_hit_PIXEndcapL3EtaResidual = Monitored::Collection("hit_PIXEndcapL3EtaResidual",mnt_hit_PIXEndcapL3EtaResidual);
  auto mon_hit_SCTBarrelL1PhiResidual = Monitored::Collection("hit_SCTBarrelL1PhiResidual",mnt_hit_SCTBarrelL1PhiResidual);
  auto mon_hit_SCTBarrelL2PhiResidual = Monitored::Collection("hit_SCTBarrelL2PhiResidual",mnt_hit_SCTBarrelL2PhiResidual);
  auto mon_hit_SCTBarrelL3PhiResidual = Monitored::Collection("hit_SCTBarrelL3PhiResidual",mnt_hit_SCTBarrelL3PhiResidual);
  auto mon_hit_SCTBarrelL4PhiResidual = Monitored::Collection("hit_SCTBarrelL4PhiResidual",mnt_hit_SCTBarrelL4PhiResidual);
  auto mon_hit_SCTEndcapL1PhiResidual = Monitored::Collection("hit_SCTEndcapL1PhiResidual",mnt_hit_SCTEndcapL1PhiResidual);
  auto mon_hit_SCTEndcapL2PhiResidual = Monitored::Collection("hit_SCTEndcapL2PhiResidual",mnt_hit_SCTEndcapL2PhiResidual);
  auto mon_hit_SCTEndcapL3PhiResidual = Monitored::Collection("hit_SCTEndcapL3PhiResidual",mnt_hit_SCTEndcapL3PhiResidual);
  auto mon_hit_SCTEndcapL4PhiResidual = Monitored::Collection("hit_SCTEndcapL4PhiResidual",mnt_hit_SCTEndcapL4PhiResidual);
  auto mon_hit_SCTEndcapL5PhiResidual = Monitored::Collection("hit_SCTEndcapL5PhiResidual",mnt_hit_SCTEndcapL5PhiResidual);
  auto mon_hit_SCTEndcapL6PhiResidual = Monitored::Collection("hit_SCTEndcapL6PhiResidual",mnt_hit_SCTEndcapL6PhiResidual);
  auto mon_hit_SCTEndcapL7PhiResidual = Monitored::Collection("hit_SCTEndcapL7PhiResidual",mnt_hit_SCTEndcapL7PhiResidual);
  auto mon_hit_SCTEndcapL8PhiResidual = Monitored::Collection("hit_SCTEndcapL8PhiResidual",mnt_hit_SCTEndcapL8PhiResidual);
  auto mon_hit_SCTEndcapL9PhiResidual = Monitored::Collection("hit_SCTEndcapL9PhiResidual",mnt_hit_SCTEndcapL9PhiResidual);

  auto monRes = Monitored::Group(m_monTool, mon_layer_IBL, mon_layer_PixB, mon_layer_PixE, mon_layer_SCTB, mon_layer_SCTE, mon_hit_IBLPhiResidual, mon_hit_IBLEtaResidual, mon_hit_IBLPhiPull, mon_hit_IBLEtaPull, mon_hit_PIXBarrelPhiResidual, mon_hit_PIXBarrelEtaResidual, mon_hit_PIXBarrelPhiPull, mon_hit_PIXBarrelEtaPull, mon_hit_SCTBarrelResidual, mon_hit_SCTBarrelPull, mon_hit_PIXEndcapPhiResidual, mon_hit_PIXEndcapEtaResidual, mon_hit_PIXEndcapPhiPull, mon_hit_PIXEndcapEtaPull, mon_hit_SCTEndcapResidual, mon_hit_SCTEndcapPull, mon_hit_PIXBarrelL1PhiResidual, mon_hit_PIXBarrelL1EtaResidual, mon_hit_PIXBarrelL2PhiResidual, mon_hit_PIXBarrelL2EtaResidual, mon_hit_PIXBarrelL3PhiResidual, mon_hit_PIXBarrelL3EtaResidual, mon_hit_PIXEndcapL1PhiResidual, mon_hit_PIXEndcapL1EtaResidual, mon_hit_PIXEndcapL2PhiResidual, mon_hit_PIXEndcapL2EtaResidual, mon_hit_PIXEndcapL3PhiResidual, mon_hit_PIXEndcapL3EtaResidual, mon_hit_SCTBarrelL1PhiResidual, mon_hit_SCTBarrelL2PhiResidual, mon_hit_SCTBarrelL3PhiResidual, mon_hit_SCTBarrelL4PhiResidual, mon_hit_SCTEndcapL1PhiResidual, mon_hit_SCTEndcapL2PhiResidual, mon_hit_SCTEndcapL3PhiResidual, mon_hit_SCTEndcapL4PhiResidual, mon_hit_SCTEndcapL5PhiResidual, mon_hit_SCTEndcapL6PhiResidual, mon_hit_SCTEndcapL7PhiResidual, mon_hit_SCTEndcapL8PhiResidual, mon_hit_SCTEndcapL9PhiResidual);

  std::vector<TrigL2HitResidual> vResid;
  vResid.clear();
  StatusCode scRes = m_trigInDetTrackFitter->getUnbiasedResiduals(track,vResid, ctx);
  if(!scRes.isSuccess()) return;
  for(std::vector<TrigL2HitResidual>::iterator it=vResid.begin();it!=vResid.end();++it) {
    Identifier id = it->identify();
    int pixlayer= (m_pixelId->layer_disk(id) );
    int sctlayer= (m_sctId->layer_disk(id) );

    switch(it->regionId()) {
      case Region::PixBarrel :
        mnt_layer_PixB.push_back(pixlayer);
        mnt_hit_PIXBarrelPhiResidual.push_back(it->phiResidual());
        mnt_hit_PIXBarrelPhiPull.push_back(it->phiPull());
        mnt_hit_PIXBarrelEtaResidual.push_back(it->etaResidual());
        mnt_hit_PIXBarrelEtaPull.push_back(it->etaPull());
        if (pixlayer == 1) {
          mnt_hit_PIXBarrelL1PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL1EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 2) {
          mnt_hit_PIXBarrelL2PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL2EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 3) {
          mnt_hit_PIXBarrelL3PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelL3EtaResidual.push_back(it->etaResidual());
        }
        break;
      case Region::PixEndcap :
        ATH_MSG_DEBUG("Pixel Endcap "  );
        mnt_layer_PixE.push_back(pixlayer);
        mnt_hit_PIXEndcapPhiResidual.push_back(it->phiResidual());
        mnt_hit_PIXEndcapPhiPull.push_back(it->phiPull());
        mnt_hit_PIXEndcapEtaResidual.push_back(it->etaResidual());
        mnt_hit_PIXEndcapEtaPull.push_back(it->etaPull());
        if (pixlayer == 0) {
          mnt_hit_PIXEndcapL1PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL1EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 1) {
          mnt_hit_PIXEndcapL2PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL2EtaResidual.push_back(it->etaResidual());
        }
        if (pixlayer == 2) {
          mnt_hit_PIXEndcapL3PhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXEndcapL3EtaResidual.push_back(it->etaResidual());
        }
        break;
      case Region::SctBarrel :
        mnt_layer_SCTB.push_back(sctlayer);
        mnt_hit_SCTBarrelResidual.push_back(it->phiResidual());
        mnt_hit_SCTBarrelPull.push_back(it->phiPull());
        if (sctlayer == 0) {
          mnt_hit_SCTBarrelL1PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 1) {
          mnt_hit_SCTBarrelL2PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 2) {
          mnt_hit_SCTBarrelL3PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 3) {
          mnt_hit_SCTBarrelL4PhiResidual.push_back(it->phiResidual());
        }
        break;
      case Region::SctEndcap :
        ATH_MSG_DEBUG("SCT Endcap"  );
        mnt_layer_SCTE.push_back(sctlayer);
        mnt_hit_SCTEndcapResidual.push_back(it->phiResidual());
        mnt_hit_SCTEndcapPull.push_back(it->phiPull());
        if (sctlayer == 0) {
          mnt_hit_SCTEndcapL1PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 1) {
          mnt_hit_SCTEndcapL2PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 2) {
          mnt_hit_SCTEndcapL3PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 3) {
          mnt_hit_SCTEndcapL4PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 4) {
          mnt_hit_SCTEndcapL5PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 5) {
          mnt_hit_SCTEndcapL6PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 6) {
          mnt_hit_SCTEndcapL7PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 7) {
          mnt_hit_SCTEndcapL8PhiResidual.push_back(it->phiResidual());
        }
        if (sctlayer == 8) {
          mnt_hit_SCTEndcapL9PhiResidual.push_back(it->phiResidual());
        }
        break;
      case Region::IBL :
        mnt_layer_IBL.push_back(pixlayer);
        if (m_tcs.m_maxSiliconLayer==32) {
          mnt_hit_IBLPhiResidual.push_back(it->phiResidual());
          mnt_hit_IBLPhiPull.push_back(it->phiPull());
          mnt_hit_IBLEtaResidual.push_back(it->etaResidual());
          mnt_hit_IBLEtaPull.push_back(it->etaPull());
        }
        else {//No IBL, fill pixel histograms instead
          ATH_MSG_DEBUG("IBL wrong region"  );
          mnt_hit_PIXBarrelPhiResidual.push_back(it->phiResidual());
          mnt_hit_PIXBarrelPhiPull.push_back(it->phiPull());
          mnt_hit_PIXBarrelEtaResidual.push_back(it->etaResidual());
          mnt_hit_PIXBarrelEtaPull.push_back(it->etaPull());
        }
        break;
      case Region::Undefined :
        ATH_MSG_DEBUG("Undefined ID region");
        break;
    }
  }
}

void TrigFastTrackFinder::makeSeedsOnGPU(const TrigCombinatorialSettings& tcs, const IRoiDescriptor* roi, const std
::vector<TrigSiSpacePointBase>& vsp, std::vector<TrigInDetTriplet>& output) const {

  output.clear();

  TrigAccel::DATA_EXPORT_BUFFER* dataBuffer = new TrigAccel::DATA_EXPORT_BUFFER(5000);//i.e. 5KB

  size_t actualSize = m_accelTool->exportSeedMakingJob(tcs, roi, vsp, *dataBuffer);

  ATH_MSG_DEBUG("SeedMakingJob is ready, data size for transfer = " <<actualSize);

  std::shared_ptr<TrigAccel::OffloadBuffer> pBuff = std::make_shared<TrigAccel::OffloadBuffer>(dataBuffer);

  TrigAccel::Work* pJob = m_accelSvc->createWork(TrigAccel::InDetJobControlCode::MAKE_SEEDS, pBuff);

  if(pJob) {
    ATH_MSG_DEBUG("Work item created for task "<<TrigAccel::InDetJobControlCode::MAKE_SEEDS);

    pJob->run();


    std::shared_ptr<TrigAccel::OffloadBuffer> pOB = pJob->getOutput();

    TrigAccel::OUTPUT_SEED_STORAGE* pOutput = reinterpret_cast<TrigAccel::OUTPUT_SEED_STORAGE *>(pOB->m_rawBuffer);

    ATH_MSG_DEBUG("Found "<<pOutput->m_nSeeds<<" triplets on GPU");

    int nTriplets = pOutput->m_nSeeds;

    //copy seeds into the output buffer

    output.clear();

    for(int k=0;k<nTriplets;k++) {
      const TrigSiSpacePointBase& SPi = vsp[pOutput->m_innerIndex[k]];
      const TrigSiSpacePointBase& SPm = vsp[pOutput->m_middleIndex[k]];
      const TrigSiSpacePointBase& SPo = vsp[pOutput->m_outerIndex[k]];
      output.emplace_back(SPi, SPm, SPo, pOutput->m_Q[k]);
    }
  }

  delete pJob;
  delete dataBuffer;
}

StatusCode TrigFastTrackFinder::createEmptyUTTEDMs(const EventContext& ctx) const
{
   if( m_doHitDV ) {
      SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVTrkHandle(m_hitDVTrkKey, ctx);
      ATH_CHECK( hitDVTrkHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   }
   if( m_dodEdxTrk ) {
      SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVSPHandle(m_hitDVSPKey, ctx);
      ATH_CHECK( hitDVSPHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
      SG::WriteHandle<xAOD::TrigCompositeContainer> dEdxTrkHandle(m_dEdxTrkKey, ctx);
      ATH_CHECK( dEdxTrkHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   }
   if( m_doDisappearingTrk ) {
      SG::WriteHandle<xAOD::TrigCompositeContainer> disTrkCandHandle(m_disTrkCandKey, ctx);
      ATH_CHECK( disTrkCandHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   }
   return StatusCode::SUCCESS;
}

StatusCode TrigFastTrackFinder::findHitDV(const EventContext& ctx, const std::vector<TrigSiSpacePointBase>& convertedSpacePoints,
					  const TrackCollection& outputTracks) const
{
   SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVTrkHandle(m_hitDVTrkKey, ctx);
   ATH_CHECK( hitDVTrkHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   auto hitDVTrkContainer = hitDVTrkHandle.ptr();

   SG::WriteHandle<xAOD::TrigCompositeContainer> hitDVSPHandle(m_hitDVSPKey, ctx);
   ATH_CHECK( hitDVSPHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   auto hitDVSPContainer = hitDVSPHandle.ptr();

   // select good tracks
   const float  TRKCUT_PT               = 0.5;
   const float  TRKCUT_A0BEAM           = 2.5;
   const int    TRKCUT_N_HITS_INNERMOST = 1;
   const int    TRKCUT_N_HITS_PIX       = 2;
   const int    TRKCUT_N_HITS           = 4;
   std::unordered_map<const Trk::PrepRawData*, int> umap_fittedTrack_prd;
   int fittedTrack_id = -1;
   std::vector<int>   v_dvtrk_id;
   std::vector<float> v_dvtrk_pt;
   std::vector<float> v_dvtrk_eta;
   std::vector<float> v_dvtrk_phi;
   std::vector<int>   v_dvtrk_n_hits_innermost;
   std::vector<int>   v_dvtrk_n_hits_pix;
   std::vector<int>   v_dvtrk_n_hits_sct;
   std::vector<float> v_dvtrk_a0beam;
   for (auto track : outputTracks) {
      if ( ! track->perigeeParameters() ) continue;
      if ( ! track->trackSummary() )      continue;
      int n_hits_innermost = track->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
      int n_hits_pix       = track->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
      int n_hits_sct       = track->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
      if( n_hits_innermost < TRKCUT_N_HITS_INNERMOST )  continue;
      if( n_hits_pix < TRKCUT_N_HITS_PIX )              continue;
      if( (n_hits_pix+n_hits_sct) < TRKCUT_N_HITS )     continue;
      float theta  = track->perigeeParameters()->parameters()[Trk::theta];
      float qOverP = track->perigeeParameters()->parameters()[Trk::qOverP];
      if( std::abs(qOverP)<1e-12 ) qOverP = 1e-12;
      float pt = std::sin(theta)/qOverP;
      pt /= 1000.0;
      if( pt < TRKCUT_PT ) continue;
      float a0   = track->perigeeParameters()->parameters()[Trk::d0];
      float phi0 = track->perigeeParameters()->parameters()[Trk::phi0];
      float shift_x = 0; float shift_y = 0;
      if( m_useBeamSpot ) getBeamSpot(shift_x, shift_y, ctx);
      float a0beam = a0 + shift_x*sin(phi0)-shift_y*cos(phi0);
      if( std::abs(a0beam) > TRKCUT_A0BEAM ) continue;

      // track is selected
      fittedTrack_id++;
      ATH_MSG_DEBUG("Selected track pT = " << pt);
      DataVector<const Trk::MeasurementBase>::const_iterator
	 m  = track->measurementsOnTrack()->begin(),
	 me = track->measurementsOnTrack()->end  ();
      for(; m!=me; ++m ) {
	 const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
	 if( prd == nullptr ) continue;
	 if( umap_fittedTrack_prd.find(prd) == umap_fittedTrack_prd.end() ) {
	    umap_fittedTrack_prd.insert(std::make_pair(prd,fittedTrack_id));
	 }
      }

      float eta = -std::log(std::tan(0.5*theta));
      float phi = track->perigeeParameters()->parameters()[Trk::phi];
      v_dvtrk_id.push_back(fittedTrack_id);
      v_dvtrk_pt.push_back(pt*1000.0);
      v_dvtrk_eta.push_back(eta);
      v_dvtrk_phi.push_back(phi);
      v_dvtrk_n_hits_innermost.push_back(n_hits_innermost);
      v_dvtrk_n_hits_pix.push_back(n_hits_pix);
      v_dvtrk_n_hits_sct.push_back(n_hits_sct);
      v_dvtrk_a0beam.push_back(a0beam);
   }
   ATH_MSG_DEBUG("Nr of selected tracks = " << fittedTrack_id);
   ATH_MSG_DEBUG("Nr of PRDs used by selected tracks = " << umap_fittedTrack_prd.size());

   // space points
   int n_sp           = 0;
   int n_sp_usedByTrk = 0;
   std::vector<float> v_sp_eta;
   std::vector<float> v_sp_r;
   std::vector<float> v_sp_phi;
   std::vector<int>   v_sp_layer;
   std::vector<bool>  v_sp_isPix;
   std::vector<bool>  v_sp_isSct;
   std::vector<int>   v_sp_usedTrkId;

   for(unsigned int iSp=0; iSp<convertedSpacePoints.size(); ++iSp) {

      bool isPix = convertedSpacePoints[iSp].isPixel();
      bool isSct = convertedSpacePoints[iSp].isSCT();
      if( ! isPix && ! isSct ) continue;

      const Trk::SpacePoint* sp = convertedSpacePoints[iSp].offlineSpacePoint();
      const Amg::Vector3D& pos_sp = sp->globalPosition();
      float sp_x = pos_sp[Amg::x];
      float sp_y = pos_sp[Amg::y];
      float sp_z = pos_sp[Amg::z];
      TVector3 p3Sp(sp_x,sp_y,sp_z);
      float sp_eta = p3Sp.Eta();
      float sp_phi = p3Sp.Phi();

      // whether used by selected tracks
      const Trk::PrepRawData* prd = sp->clusterList().first;
      int usedTrack_id = -1;
      if( prd != nullptr && umap_fittedTrack_prd.find(prd) != umap_fittedTrack_prd.end() ) {
	 ATH_MSG_DEBUG("prd first is there in umap");
	 usedTrack_id = umap_fittedTrack_prd[prd];
      }
      else {
	 const Trk::PrepRawData* prd = sp->clusterList().second;
	 if( prd != nullptr && umap_fittedTrack_prd.find(prd) != umap_fittedTrack_prd.end() ) {
	    ATH_MSG_DEBUG("prd second is there in umap");
	    usedTrack_id = umap_fittedTrack_prd[prd];
	 }
      }

      //
      n_sp++;
      if( usedTrack_id != -1 ) n_sp_usedByTrk++;
      int  layer = convertedSpacePoints[iSp].layer();
      float sp_r = convertedSpacePoints[iSp].r();

      v_sp_eta.push_back(sp_eta);
      v_sp_r.push_back(sp_r);
      v_sp_phi.push_back(sp_phi);
      v_sp_layer.push_back(layer);
      v_sp_isPix.push_back(isPix);
      v_sp_isSct.push_back(isSct);
      v_sp_usedTrkId.push_back(usedTrack_id);

      ATH_MSG_VERBOSE("+++ SP eta / phi / layer / ixPix / usedTrack_id = " << sp_eta << " / " << sp_phi << " / " << layer << " / " << isPix << " / " << usedTrack_id);

   }
   ATH_MSG_VERBOSE("Nr of SPs = " << n_sp);
   ATH_MSG_VERBOSE("Nr of SPs used by selected tracks = " << n_sp_usedByTrk);

   // Seed
   std::vector<float> v_seeds_eta;
   std::vector<float> v_seeds_phi;

   if( m_doHitDV_Seeding ) {

      // space-point based (unseeded mode)
      ATH_CHECK( findSPSeeds(ctx, v_sp_eta, v_sp_phi, v_sp_layer, v_sp_usedTrkId, v_seeds_eta, v_seeds_phi) );
      ATH_MSG_VERBOSE("Nr of SP seeds = " << v_seeds_eta.size());

      // add J1 J30 seeds
      const unsigned int L1JET_ET_CUT = 30;

      auto recJetRoiCollectionHandle = SG::makeHandle( m_recJetRoiCollectionKey, ctx );
      const DataVector<LVL1::RecJetRoI> *recJetRoiCollection = recJetRoiCollectionHandle.cptr();
      if (!recJetRoiCollectionHandle.isValid()){
	 ATH_MSG_ERROR("ReadHandle for DataVector<LVL1::RecJetRoI> key:" << m_recJetRoiCollectionKey.key() << " isn't Valid");
	 return StatusCode::FAILURE;
      }
      for (size_t size=0; size<recJetRoiCollection->size(); ++size){
	 const LVL1::RecJetRoI* recRoI = recJetRoiCollection->at(size);
	 if( recRoI == nullptr ) continue;
	 bool isSeed = false;
	 for( const unsigned int thrMapping : recRoI->thresholdsPassed()) {
	    double thrValue = recRoI->triggerThreshold(thrMapping) * Gaudi::Units::GeV;
	    if( thrValue >= L1JET_ET_CUT ) {
	       isSeed = true;
	       break;
	    }
	 }
	 if( ! isSeed ) continue;
	 // Convert to ATLAS phi convention: see RoIResultToAOD.cxx
	 float roiPhi = recRoI->phi();
	 if( roiPhi > TMath::Pi() ) roiPhi -= 2 * TMath::Pi();
	 v_seeds_eta.push_back(recRoI->eta());
	 v_seeds_phi.push_back(roiPhi);
      }
      ATH_MSG_VERBOSE("Nr of SP + L1_J30 seeds = " << v_seeds_eta.size());
   }

   // fill objects

   // track
   const float TRKCUT_DELTA_R_TO_SEED = 1.0;
   hitDVTrkContainer->reserve(v_dvtrk_pt.size());
   for(unsigned int iTrk=0; iTrk<v_dvtrk_pt.size(); ++iTrk) {
      float trk_eta = v_dvtrk_eta[iTrk];
      float trk_phi = v_dvtrk_phi[iTrk];
      if( m_doHitDV_Seeding ) {
	 bool isNearSeed = false;
	 for (unsigned int iSeed=0; iSeed<v_seeds_eta.size(); ++iSeed) {
	    float seed_eta = v_seeds_eta[iSeed];
	    float seed_phi = v_seeds_phi[iSeed];
	    float dR = deltaR(trk_eta,trk_phi,seed_eta,seed_phi);
	    if( dR <= TRKCUT_DELTA_R_TO_SEED ) { isNearSeed = true; break; }
	 }
	 if( ! isNearSeed ) continue;
      }
      xAOD::TrigComposite *hitDVTrk = new xAOD::TrigComposite();
      hitDVTrkContainer->push_back(hitDVTrk);
      hitDVTrk->setDetail<int>  ("hitDVTrk_id",  v_dvtrk_id[iTrk]);
      hitDVTrk->setDetail<float>("hitDVTrk_pt",  v_dvtrk_pt[iTrk]);
      hitDVTrk->setDetail<float>("hitDVTrk_eta", v_dvtrk_eta[iTrk]);
      hitDVTrk->setDetail<float>("hitDVTrk_phi", v_dvtrk_phi[iTrk]);
      hitDVTrk->setDetail<int>  ("hitDVTrk_n_hits_innermost", v_dvtrk_n_hits_innermost[iTrk]);
      hitDVTrk->setDetail<int>  ("hitDVTrk_n_hits_pix", v_dvtrk_n_hits_pix[iTrk]);
      hitDVTrk->setDetail<int>  ("hitDVTrk_n_hits_sct", v_dvtrk_n_hits_sct[iTrk]);
      hitDVTrk->setDetail<float>("hitDVTrk_a0beam",     v_dvtrk_a0beam[iTrk]);
   }

   // space points
   const float SPCUT_DELTA_R_TO_SEED = 0.6;
   unsigned int n_sp_stored = 0;
   const unsigned int N_MAX_SP_STORED = 100000;

   hitDVSPContainer->reserve(v_sp_eta.size());
   for(unsigned int iSp=0; iSp<v_sp_eta.size(); ++iSp) {
      float sp_eta = v_sp_eta[iSp];
      float sp_phi = v_sp_phi[iSp];
      if( m_doHitDV_Seeding ) {
	 bool isNearSeed = false;
	 for (unsigned int iSeed=0; iSeed<v_seeds_eta.size(); ++iSeed) {
	    float seed_eta = v_seeds_eta[iSeed];
	    float seed_phi = v_seeds_phi[iSeed];
	    float dR = deltaR(sp_eta,sp_phi,seed_eta,seed_phi);
	    if( dR <= SPCUT_DELTA_R_TO_SEED ) { isNearSeed = true; break; }
	 }
	 if( ! isNearSeed ) continue;
      }
      ++n_sp_stored;
      if( n_sp_stored > N_MAX_SP_STORED ) break;
      xAOD::TrigComposite *hitDVSP = new xAOD::TrigComposite();
      hitDVSPContainer->push_back(hitDVSP);
      hitDVSP->setDetail<float>  ("hitDVSP_eta",       v_sp_eta[iSp]);
      hitDVSP->setDetail<float>  ("hitDVSP_r",         v_sp_r[iSp]);
      hitDVSP->setDetail<float>  ("hitDVSP_phi",       v_sp_phi[iSp]);
      hitDVSP->setDetail<int16_t>("hitDVSP_layer",     (int16_t)v_sp_layer[iSp]);
      hitDVSP->setDetail<bool>   ("hitDVSP_isPix",     v_sp_isPix[iSp]);
      hitDVSP->setDetail<bool>   ("hitDVSP_isSct",     v_sp_isSct[iSp]);
      hitDVSP->setDetail<int16_t>("hitDVSP_usedTrkId", (int16_t)v_sp_usedTrkId[iSp]);
   }

   return StatusCode::SUCCESS;
}

StatusCode TrigFastTrackFinder::findSPSeeds( const EventContext& ctx,
					     const std::vector<float>& v_sp_eta, const std::vector<float>& v_sp_phi,
					     const std::vector<int>& v_sp_layer, const std::vector<int>& v_sp_usedTrkId,
					     std::vector<float>& seeds_eta, std::vector<float>& seeds_phi ) const
{
   seeds_eta.clear();
   seeds_phi.clear();

   const int   NBINS_ETA = 50;
   const float ETA_MIN   = -2.5;
   const float ETA_MAX   =  2.5;

   const int   NBINS_PHI = 80;
   const float PHI_MIN   = -4.0;
   const float PHI_MAX   =  4.0;

   char hname[64];

   unsigned int slotnr    = ctx.slot();
   unsigned int subSlotnr = ctx.subSlot();

   sprintf(hname,"ftf_s%i_ss%i_ly6_h2_nsp",slotnr,subSlotnr);
   std::unique_ptr<TH2F> ly6_h2_nsp = std::make_unique<TH2F>(hname,hname,NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   sprintf(hname,"ftf_s%i_ss%i_ly7_h2_nsp",slotnr,subSlotnr);
   std::unique_ptr<TH2F> ly7_h2_nsp = std::make_unique<TH2F>(hname,hname,NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   sprintf(hname,"ftf_s%i_ss%i_ly6_h2_nsp_notrk",slotnr,subSlotnr);
   std::unique_ptr<TH2F> ly6_h2_nsp_notrk = std::make_unique<TH2F>(hname,hname,NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);
   sprintf(hname,"ftf_s%i_ss%i_ly7_h2_nsp_notrk",slotnr,subSlotnr);
   std::unique_ptr<TH2F> ly7_h2_nsp_notrk = std::make_unique<TH2F>(hname,hname,NBINS_ETA,ETA_MIN,ETA_MAX,NBINS_PHI,PHI_MIN,PHI_MAX);

   for(unsigned int iSeed=0; iSeed<v_sp_eta.size(); ++iSeed) {

      int sp_layer = v_sp_layer[iSeed];
      float sp_eta = v_sp_eta[iSeed];
      int ilayer = getSPLayer(sp_layer,sp_eta);
      if( ilayer<6 ) continue;

      int sp_trkid = v_sp_usedTrkId[iSeed];
      bool isUsedByTrk = (sp_trkid != -1);
      float sp_phi = v_sp_phi[iSeed];

      bool fill_out_of_pi = false;
      float sp_phi2 = 0;
      if( sp_phi < 0 ) {
	 sp_phi2 = 2*TMath::Pi() + sp_phi;
	 if( sp_phi2 < PHI_MAX ) fill_out_of_pi = true;
      }
      else {
	 sp_phi2 = -2*TMath::Pi() + sp_phi;
	 if( PHI_MIN < sp_phi2 ) fill_out_of_pi = true;
      }
      if( ilayer==6 ) {
	                      ly6_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly6_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly6_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
      if( ilayer==7 ) {
	                      ly7_h2_nsp->Fill(sp_eta,sp_phi);
	 if( fill_out_of_pi ) ly7_h2_nsp->Fill(sp_eta,sp_phi2);
	 if( ! isUsedByTrk )                  ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi);
	 if( ! isUsedByTrk && fill_out_of_pi) ly7_h2_nsp_notrk->Fill(sp_eta,sp_phi2);
      }
   }

   ATH_MSG_VERBOSE("looking for ly6/ly6 doublet seeds");

   // (idx, sort/weight, eta, phi)
   std::vector<std::tuple<int,float,float,float>> QT;

   for(int ly6_ieta=1; ly6_ieta<=NBINS_ETA; ly6_ieta++) {
      float ly6_eta = (ly6_h2_nsp->GetXaxis()->GetBinLowEdge(ly6_ieta) + ly6_h2_nsp->GetXaxis()->GetBinUpEdge(ly6_ieta))/2.0;
      for(int ly6_iphi=1; ly6_iphi<=NBINS_PHI; ly6_iphi++) {
	 float ly6_phi = (ly6_h2_nsp->GetYaxis()->GetBinLowEdge(ly6_iphi) + ly6_h2_nsp->GetYaxis()->GetBinUpEdge(ly6_iphi))/2.0;

	 float ly6_nsp       = ly6_h2_nsp      ->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_nsp_notrk = ly6_h2_nsp_notrk->GetBinContent(ly6_ieta,ly6_iphi);
	 float ly6_frac      = ( ly6_nsp > 0 ) ? ly6_nsp_notrk / ly6_nsp : 0;
	 if( ly6_nsp < 10 || ly6_frac < 0.85 ) continue;

	 float ly7_frac_max = 0;
	 float ly7_eta_max  = 0;
	 float ly7_phi_max  = 0;
	 for(int ly7_ieta=std::max(1,ly6_ieta-1); ly7_ieta<std::min(NBINS_ETA,ly6_ieta+1); ly7_ieta++) {
	    for(int ly7_iphi=std::max(1,ly6_iphi-1); ly7_iphi<=std::min(NBINS_PHI,ly6_iphi+1); ly7_iphi++) {
	       float ly7_nsp       = ly7_h2_nsp      ->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_nsp_notrk = ly7_h2_nsp_notrk->GetBinContent(ly7_ieta,ly7_iphi);
	       float ly7_frac      = ( ly7_nsp > 0 ) ? ly7_nsp_notrk / ly7_nsp : 0;
	       if( ly7_nsp < 10 ) continue;
	       if( ly7_frac > ly7_frac_max ) {
		  ly7_frac_max = ly7_frac;
		  ly7_eta_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_ieta) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_ieta))/2.0;
		  ly7_phi_max  = (ly7_h2_nsp->GetXaxis()->GetBinLowEdge(ly7_iphi) + ly7_h2_nsp->GetXaxis()->GetBinUpEdge(ly7_iphi))/2.0;
	       }
	    }
	 }
	 if( ly7_frac_max < 0.85 ) continue;
	 //
	 float wsum = ly6_frac + ly7_frac_max;
	 float weta = (ly6_eta*ly6_frac + ly7_eta_max*ly7_frac_max) / wsum;
	 float wphi = (ly6_phi*ly6_frac + ly7_phi_max*ly7_frac_max) / wsum;
	 float w = wsum / 2.0;
	 QT.push_back(std::make_tuple(-1,w,weta,wphi));
      }
   }
   ATH_MSG_VERBOSE("nr of ly6/ly7 doublet candidate seeds=" << QT.size() << ", doing clustering...");

   // sort
   std::sort(QT.begin(), QT.end(),
	     [](const std::tuple<int,float,float,float>& lhs, const std::tuple<int,float,float,float>& rhs) {
		return std::get<1>(lhs) > std::get<1>(rhs); } );

   // clustering
   const double CLUSTCUT_DIST      = 0.2;
   const double CLUSTCUT_SEED_FRAC = 0.9;

   std::vector<float> seeds_wsum;

   for(unsigned int i=0; i<QT.size(); i++) {
      float phi  = std::get<3>(QT[i]);
      float eta  = std::get<2>(QT[i]);
      float w    = std::get<1>(QT[i]);
      if(i==0) {
	 seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	 seeds_wsum.push_back(w);
	 continue;
      }
      const int IDX_INITIAL = 100;
      float dist_min = 100.0;
      int idx_min     = IDX_INITIAL;
      for(unsigned j=0; j<seeds_eta.size(); j++) {
	 float ceta = seeds_eta[j]/seeds_wsum[j];
	 float cphi = seeds_phi[j]/seeds_wsum[j];
	 // intentionally calculate in this way as phi is defined beyond -Pi/Pi (no boundary)
	 float deta = std::fabs(ceta-eta);
	 float dphi = std::fabs(cphi-phi);
	 float dist = std::sqrt(dphi*dphi+deta*deta);
	 if( dist < dist_min ) {
	    dist_min = dist;
	    idx_min  = j;
	 }
      }
      int match_idx = IDX_INITIAL;
      if( idx_min != IDX_INITIAL ) {
	 if( dist_min < CLUSTCUT_DIST ) { match_idx = idx_min; }
      }
      if( match_idx == IDX_INITIAL ) {
	 if( w > CLUSTCUT_SEED_FRAC && dist_min > CLUSTCUT_DIST ) {
	    seeds_eta.push_back(w*eta); seeds_phi.push_back(w*phi);
	    seeds_wsum.push_back(w);
	 }
	 continue;
      }
      float new_eta   = seeds_eta[match_idx]  + w*eta;
      float new_phi   = seeds_phi[match_idx]  + w*phi;
      float new_wsum  = seeds_wsum[match_idx] + w;
      seeds_eta[match_idx]   = new_eta;
      seeds_phi[match_idx]   = new_phi;
      seeds_wsum[match_idx]  = new_wsum;
   }
   QT.clear();
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      float eta = seeds_eta[i] / seeds_wsum[i];
      float phi = seeds_phi[i] / seeds_wsum[i];
      seeds_eta[i] = eta;
      seeds_phi[i] = phi;
      if( phi < -TMath::Pi() ) phi =  2*TMath::Pi() + phi;
      if( phi >  TMath::Pi() ) phi = -2*TMath::Pi() + phi;
      seeds_phi[i] = phi;
   }
   ATH_MSG_VERBOSE("after clustering, nr of seeds = " << seeds_eta.size());

   // delete overlap (can happen at phi=-Pi/Pi bounadry)
   std::vector<unsigned int> idx_to_delete;
   for(unsigned int i=0; i<seeds_eta.size(); i++) {
      if( std::find(idx_to_delete.begin(),idx_to_delete.end(),i) != idx_to_delete.end() ) continue;
      float eta_i = seeds_eta[i];
      float phi_i = seeds_phi[i];
      for(unsigned int j=i+1; j<seeds_eta.size(); j++) {
	 if( std::find(idx_to_delete.begin(),idx_to_delete.end(),j) != idx_to_delete.end() ) continue;
	 float eta_j = seeds_eta[j];
	 float phi_j = seeds_phi[j];
	 float dr = deltaR(eta_i,phi_i,eta_j,phi_j);
	 if( dr < CLUSTCUT_DIST ) idx_to_delete.push_back(j);
      }
   }
   ATH_MSG_VERBOSE("nr of duplicated seeds to be removed = " << idx_to_delete.size());
   if( idx_to_delete.size() > 0 ) {
      std::sort(idx_to_delete.begin(),idx_to_delete.end());
      for(unsigned int j=idx_to_delete.size(); j>0; j--) {
	 unsigned int idx = idx_to_delete[j-1];
	 seeds_eta.erase(seeds_eta.begin()+idx);
	 seeds_phi.erase(seeds_phi.begin()+idx);
      }
   }

   ATH_MSG_VERBOSE("nr of ly6/ly7 seeds=" << seeds_eta.size());

   // return
   return StatusCode::SUCCESS;
}

float TrigFastTrackFinder::deltaR(float eta_1, float phi_1, float eta_2, float phi_2) const {
   float dPhi = phi_1 - phi_2;
   if (dPhi < -TMath::Pi()) dPhi += 2*TMath::Pi();
   if (dPhi >  TMath::Pi()) dPhi -= 2*TMath::Pi();
   float dEta = eta_1 - eta_2;
   return sqrt(dPhi*dPhi+dEta*dEta);
}

int TrigFastTrackFinder::getSPLayer(int layer, float eta) const
{
   float abseta = std::fabs(eta);

   // Pixel barrel or SCT barrel
   if( 0<=layer && layer <=7 ) {
      ATH_MSG_VERBOSE("layer=" << layer << ", eta=" << abseta);
      return layer;
   }

   int base = 0;

   //
   const float PixBR6limit = 1.29612;
   const float PixBR5limit = 1.45204;
   const float PixBR4limit = 1.64909;
   const float PixBR3limit = 1.90036;
   const float PixBR2limit = 2.2146;

   // Pixel Endcap #1
   base = 8;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Pix EC1, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #2
   base = 9;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Pix EC2, eta=" << abseta);
      if( abseta > PixBR2limit ) return 2;
      return 3;
   }

   // Pixel Endcap #3
   base = 10;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Pix EC3, eta=" << abseta);
      return 3;
   }

   // SCT Endcap #1
   base = 11;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC1, eta=" << abseta);
      if( abseta < PixBR6limit )      return 7;
      else if( abseta < PixBR5limit ) return 6;
      return 5;
   }

   // SCT Endcap #2
   base = 12;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC2, eta=" << abseta);
      if( abseta < PixBR5limit )      return 7;
      else if( abseta < PixBR4limit ) return 6;
      return 4;
   }

   // SCT Endcap #3
   base = 13;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC3, eta=" << abseta);
      if( abseta < PixBR4limit ) return 7;
      return 5;
   }

   // SCT Endcap #4
   base = 14;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC4, eta=" << abseta);
      if( abseta < PixBR4limit ) return 6;
      else if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #5
   base = 15;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC5, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #6
   base = 16;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC6, eta=" << abseta);
      if( abseta < PixBR3limit ) return 6;
      return 4;
   }

   // SCT Endcap #7
   base = 17;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC7, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 5;
   }

   // SCT Endcap #8
   base = 18;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC8, eta=" << abseta);
      if( abseta < PixBR3limit ) return 7;
      return 6;
   }

   // SCT Endcap #9
   base = 19;
   if( layer==base || layer==(base+12) ) {
      ATH_MSG_VERBOSE("Sct EC9, eta=" << abseta);
      return 7;
   }

   return 0;
}

StatusCode TrigFastTrackFinder::finddEdxTrk(const EventContext& ctx, const TrackCollection& outputTracks) const
{
   SG::WriteHandle<xAOD::TrigCompositeContainer> dEdxTrkHandle(m_dEdxTrkKey, ctx);
   ATH_CHECK( dEdxTrkHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   auto dEdxTrkContainer = dEdxTrkHandle.ptr();

   SG::WriteHandle<xAOD::TrigCompositeContainer> dEdxHitHandle(m_dEdxHitKey, ctx);
   ATH_CHECK( dEdxHitHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   auto dEdxHitContainer = dEdxHitHandle.ptr();

   std::vector<float> mnt_dedx;
   std::vector<int>   mnt_dedx_nusedhits;
   auto mon_dedx           = Monitored::Collection("trk_dedx",           mnt_dedx);
   auto mon_dedx_nusedhits = Monitored::Collection("trk_dedx_nusedhits", mnt_dedx_nusedhits);
   auto mondEdx            = Monitored::Group(m_monTool, mon_dedx, mon_dedx_nusedhits);

   int i_track=0;

   ATH_MSG_VERBOSE("========== in finddEdxTrk ==========");

   const float  TRKCUT_A0BEAM       = 2.5;
   const int    TRKCUT_N_HITS_INNER = 1;
   const int    TRKCUT_N_HITS_PIX   = 2;
   const int    TRKCUT_N_HITS       = 4;

   const float  TRKCUT_PTGEV_LOOSE  =  3.0;
   const float  TRKCUT_PTGEV_TIGHT  = 10.0;
   const float  TRKCUT_DEDX_LOOSE   =  1.25;
   const float  TRKCUT_DEDX_TIGHT   =  1.55;

   for (auto track : outputTracks) {

      ATH_MSG_VERBOSE("+++++++ i_track: " << i_track << " +++++++");

      i_track++;
      if ( ! track->perigeeParameters() ) continue;
      if ( ! track->trackSummary() )      continue;
      float n_hits_innermost = 0;  float n_hits_next_to_innermost = 0; float n_hits_inner = 0; float n_hits_pix = 0; float n_hits_sct = 0;
      n_hits_innermost = (float)track->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
      n_hits_next_to_innermost = (float)track->trackSummary()->get(Trk::SummaryType::numberOfNextToInnermostPixelLayerHits);
      n_hits_inner = n_hits_innermost + n_hits_next_to_innermost;
      n_hits_pix = (float)track->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
      n_hits_sct = (float)track->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
      if( n_hits_inner < TRKCUT_N_HITS_INNER )      continue;
      if( n_hits_pix < TRKCUT_N_HITS_PIX )          continue;
      if( (n_hits_pix+n_hits_sct) < TRKCUT_N_HITS ) continue;
      float theta = track->perigeeParameters()->parameters()[Trk::theta];
      float pt    = std::abs(1./track->perigeeParameters()->parameters()[Trk::qOverP]) * sin(theta);
      float ptgev = pt / 1000.0;
      if( ptgev < TRKCUT_PTGEV_LOOSE ) continue;
      float a0   = track->perigeeParameters()->parameters()[Trk::d0];
      float phi0 = track->perigeeParameters()->parameters()[Trk::phi0];
      float shift_x = 0; float shift_y = 0;
      if( m_useBeamSpot ) getBeamSpot(shift_x, shift_y, ctx);
      float a0beam = a0 + shift_x*sin(phi0)-shift_y*cos(phi0);
      if( std::abs(a0beam) > TRKCUT_A0BEAM ) continue;

      ATH_MSG_VERBOSE("calculate dEdx -->");
      int pixelhits=0; int n_usedhits=0;
      std::vector<float> v_pixhit_dedx; std::vector<float> v_pixhit_tot; std::vector<float> v_pixhit_trkchi2; std::vector<float> v_pixhit_trkndof;
      std::vector<int> v_pixhit_iblovfl; std::vector<int> v_pixhit_loc; std::vector<int> v_pixhit_layer;
      float dedx = dEdx(track,pixelhits,n_usedhits,v_pixhit_dedx,v_pixhit_tot,v_pixhit_trkchi2,v_pixhit_trkndof,
			v_pixhit_iblovfl,v_pixhit_loc,v_pixhit_layer);
      ATH_MSG_VERBOSE("--> dedx = " << dedx);

      mnt_dedx.push_back(dedx);
      mnt_dedx_nusedhits.push_back(n_usedhits);

      bool hpt = (ptgev >= TRKCUT_PTGEV_TIGHT && dedx >= TRKCUT_DEDX_LOOSE);
      bool lpt = (ptgev >= TRKCUT_PTGEV_LOOSE && dedx >= TRKCUT_DEDX_TIGHT);
      if( ! hpt && ! lpt ) continue;

      xAOD::TrigComposite *dEdxTrk = new xAOD::TrigComposite();
      dEdxTrk->makePrivateStore();
      dEdxTrkContainer->push_back(dEdxTrk);
      dEdxTrk->setDetail<int>  ("dEdxTrk_id",     i_track);
      dEdxTrk->setDetail<float>("dEdxTrk_pt",     pt);
      float eta = -log(tan(0.5*theta));
      dEdxTrk->setDetail<float>("dEdxTrk_eta",    eta);
      dEdxTrk->setDetail<float>("dEdxTrk_phi",    phi0);
      dEdxTrk->setDetail<float>("dEdxTrk_a0beam", a0beam);
      dEdxTrk->setDetail<float>("dEdxTrk_dedx",   dedx);
      dEdxTrk->setDetail<int>  ("dEdxTrk_dedx_n_usedhits",  n_usedhits);
      dEdxTrk->setDetail<int>  ("dEdxTrk_n_hits_innermost", n_hits_innermost);
      dEdxTrk->setDetail<int>  ("dEdxTrk_n_hits_inner",     n_hits_inner);
      dEdxTrk->setDetail<int>  ("dEdxTrk_n_hits_pix",       n_hits_pix);
      dEdxTrk->setDetail<int>  ("dEdxTrk_n_hits_sct",       n_hits_sct);

      for(unsigned int i=0; i<v_pixhit_dedx.size(); i++) {
	 xAOD::TrigComposite *dEdxHit = new xAOD::TrigComposite();
	 dEdxHit->makePrivateStore();
	 dEdxHitContainer->push_back(dEdxHit);
	 dEdxHit->setDetail<int>  ("dEdxHit_trkid",   i_track);
	 dEdxHit->setDetail<float>("dEdxHit_dedx",    v_pixhit_dedx[i]);
	 dEdxHit->setDetail<float>("dEdxHit_tot",     v_pixhit_tot[i]);
	 dEdxHit->setDetail<float>("dEdxHit_trkchi2", v_pixhit_trkchi2[i]);
	 dEdxHit->setDetail<float>("dEdxHit_trkndof", v_pixhit_trkndof[i]);
	 dEdxHit->setDetail<int>  ("dEdxHit_iblovfl", v_pixhit_iblovfl[i]);
	 dEdxHit->setDetail<int>  ("dEdxHit_loc",     v_pixhit_loc[i]);
	 dEdxHit->setDetail<int>  ("dEdxHit_layer",   v_pixhit_layer[i]);
      }
   }

   return StatusCode::SUCCESS;
}

float TrigFastTrackFinder::dEdx(const Trk::Track* track, int& pixelhits, int& n_usedhits,
				std::vector<float>& v_pixhit_dedx,    std::vector<float>& v_pixhit_tot,
				std::vector<float>& v_pixhit_trkchi2, std::vector<float>& v_pixhit_trkndof,
				std::vector<int>&   v_pixhit_iblovfl, std::vector<int>&   v_pixhit_loc,     std::vector<int>& v_pixhit_layer) const
{
   const float Pixel_sensorthickness=.025;      // 250 microns Pixel Planars
   const float IBL_3D_sensorthickness=.023;     // 230 microns IBL 3D
   const float IBL_PLANAR_sensorthickness=.020; // 200 microns IBL Planars

   const float energyPair = 3.68e-6; // Energy in MeV to create an electron-hole pair in silicon
   const float sidensity = 2.329;    // silicon density in g cm^-3

   float conversion_factor=energyPair/sidensity;

   // Loop over pixel hits on track, and calculate dE/dx:

   pixelhits  = 0;
   n_usedhits = 0;

   v_pixhit_dedx.clear();
   v_pixhit_tot.clear();
   v_pixhit_trkchi2.clear();
   v_pixhit_trkndof.clear();
   v_pixhit_iblovfl.clear();
   v_pixhit_loc.clear();
   v_pixhit_layer.clear();

   const int PIXLOC_IBL_PL = 0;
   const int PIXLOC_IBL_3D = 1;
   const int PIXLOC_PIX_LY = 2;
   const int PIXLOC_PIX_EC = 3;
   const int PIXLOC_IBL_UNKNOWN = 4;
   const int PIXLOC_PIX_UNKNOWN = 5;

   std::multimap<float,int> dEdxMap;
   int   n_usedIBLOverflowHits=0;
   float dEdxValue = 0;

   // Check for track states:
   const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track->trackStateOnSurfaces();
   if (recoTrackStates) {

      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

      int i_tsos=0;

      // Loop over track states on surfaces (i.e. generalized hits):
      for ( ; tsosIter != tsosIterEnd; ++tsosIter) {

	 ATH_MSG_VERBOSE("-------- TSoS: " << i_tsos++  << " --------");

	 const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
	 if ( measurement == nullptr ) {
	    ATH_MSG_VERBOSE("no measurement on this TSoS, skip it");
	    continue;
	 }
	 const Trk::TrackParameters* tp = (*tsosIter)->trackParameters();
	 if( tp == nullptr ) {
	    ATH_MSG_VERBOSE("no trackParameters() to this TSoS, skip it");
	    continue;
	 }
	 const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	 if ( pixclus == nullptr ) {
	    ATH_MSG_VERBOSE("this TSoS is not Pixel, skip it");
	    continue;
	 }
	 const InDet::PixelCluster* prd = pixclus->prepRawData();
	 if( prd == nullptr ) {
	    ATH_MSG_VERBOSE("no PrepRawData(), skip it");
	    continue;
	 }

	 float dotProd  = tp->momentum().dot(tp->associatedSurface().normal());
	 float cosalpha = std::abs(dotProd/tp->momentum().mag());
	 ATH_MSG_VERBOSE("dotProd / cosalpha = " << dotProd  << " / " << cosalpha);
	 if (std::abs(cosalpha)<.16) continue;

	 const std::vector<int>& v_tots = prd->totList();
	 float charge = prd->totalCharge();
	 float tot    = prd->totalToT();
	 ATH_MSG_VERBOSE("charge / ToT = " << charge << " / " << tot);
	 charge *= cosalpha; // path length correction

	 double locx = pixclus->localParameters()[Trk::locX];
	 double locy = pixclus->localParameters()[Trk::locY];
	 int    bec        = m_pixelId->barrel_ec(pixclus->identify());
	 int    layer      = m_pixelId->layer_disk(pixclus->identify());
	 int    eta_module = m_pixelId->eta_module(pixclus->identify()); //check eta module to select thickness

	 float chi2  = 0.;
	 float ndof  = 0.;
	 const Trk::FitQualityOnSurface* fq = (*tsosIter)->fitQualityOnSurface();
	 if( fq != nullptr ) {
	    chi2  = fq->chiSquared();
	    ndof  = fq->doubleNumberDoF();
	 }

	 int iblOverflow=0; // keep track if this is IBL overflow
	 float thickness=0;
	 int loc=-1;

	 if ( (bec==0) and (layer==0) ){ // IBL
	    const float overflowIBLToT = 16; // m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();
	    for (int pixToT : v_tots) {
	       if (pixToT >= overflowIBLToT) {
		  iblOverflow = 1; // overflow pixel hit -- flag cluster
		  break; //no need to check other hits of this cluster
	       }
	    }
	    if(iblOverflow==1) ATH_MSG_VERBOSE("IBL overflow");

	    if(((eta_module>=-10 && eta_module<=-7)||(eta_module>=6 && eta_module<=9)) && (std::abs(locy)<10. && (locx>-8.33 && locx <8.3)) ){ // IBL 3D
	       thickness = IBL_3D_sensorthickness;
	       loc = PIXLOC_IBL_3D;
	    }
	    else if((eta_module>=-6 && eta_module<=5) && (std::abs(locy)<20. &&( locx >-8.33 && locx <8.3 )) ){ // IBL planer
	       thickness = IBL_PLANAR_sensorthickness;
	       loc = PIXLOC_IBL_PL;
	    }
	    else {
	       ATH_MSG_VERBOSE("unknown IBL module");
	       loc = PIXLOC_IBL_UNKNOWN;
	    }
	 }
	 else if(bec==0 && std::abs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ){ //PIXEL layer
	    thickness = Pixel_sensorthickness;
	    loc = PIXLOC_PIX_LY;
	 }
	 else if(std::abs(bec) == 2 && std::abs(locy)<30. && ( ( locx > -8.15 && locx < -0.55 ) || ( locx > 0.55 && locx < 8.15 ) ) ) { //PIXEL endcap
	    thickness = Pixel_sensorthickness;
	    loc = PIXLOC_PIX_EC;
	 }
	 else {
	    ATH_MSG_VERBOSE("unknown Pixel module");
	    loc = PIXLOC_IBL_UNKNOWN;
	 }

	 dEdxValue = 0;
	 if( loc != PIXLOC_IBL_UNKNOWN && loc != PIXLOC_PIX_UNKNOWN ) {
	    dEdxValue = charge*conversion_factor/thickness;
	    dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
	    pixelhits++;
	    if(iblOverflow==1)n_usedIBLOverflowHits++;
	 }
	 ATH_MSG_VERBOSE("dEdx=" << dEdxValue);
	 v_pixhit_dedx.push_back(dEdxValue); v_pixhit_tot.push_back(tot);
	 v_pixhit_trkchi2.push_back(chi2); v_pixhit_trkndof.push_back(ndof);
	 v_pixhit_iblovfl.push_back(iblOverflow); v_pixhit_loc.push_back(loc); v_pixhit_layer.push_back(layer);
      }
   }

   // Now calculate dEdx, multimap is already sorted in ascending order
   // float averageCharge=-1;

   float averagedEdx=0.;
   int IBLOverflow=0;

   int i_map=0;

   for (std::pair<float,int> itdEdx : dEdxMap) {
      ATH_MSG_VERBOSE("++++++++ i_map: " << i_map++  << " ++++++++");
      if(itdEdx.second==0){
	 ATH_MSG_VERBOSE("usedhits, dEdx=" << itdEdx.first);
         averagedEdx += itdEdx.first;
         n_usedhits++;
      }
      if(itdEdx.second > 0){
	 ATH_MSG_VERBOSE("IBLOverflow");
         IBLOverflow++;
      }
      // break, skipping last or the two last elements depending on total measurements
      if (((int)pixelhits >= 5) and ((int)n_usedhits >= (int)pixelhits-2)) {
	 ATH_MSG_VERBOSE("break, skipping last or two last elements");
	 break;
      }

      // break, IBL Overflow case pixelhits==3 and 4
      if((int)IBLOverflow>0 and ((int)pixelhits==3) and (int)n_usedhits==1) {
	 ATH_MSG_VERBOSE("break, IBL overflow case, pixel hits=3");
	 break;
      }
      if((int)IBLOverflow>0 and ((int)pixelhits==4) and (int)n_usedhits==2) {
	 ATH_MSG_VERBOSE("break, IBL overflow case, pixel hits=4");
	 break;
      }

      if (((int)pixelhits > 1) and ((int)n_usedhits >=(int)pixelhits-1)) {
	 ATH_MSG_VERBOSE("break, skipping last??");
	 break;
      }

      if((int)IBLOverflow>0 and (int)pixelhits==1){ // only IBL in overflow
	 ATH_MSG_VERBOSE("break, only IBL in overflow");
         averagedEdx=itdEdx.first;
         break;
      }
   }

   if (n_usedhits > 0 or (n_usedhits==0 and(int)IBLOverflow>0 and (int)pixelhits==1)) {
      if(n_usedhits > 0) averagedEdx = averagedEdx / n_usedhits;
      //if(n_usedhits == 0 and (int)IBLOverflow > 0 and (int)pixelhits == 1) averagedEdx = averagedEdx;  //no-op
      ATH_MSG_DEBUG("=====> averaged dEdx = " << averagedEdx << " =====>");;
      ATH_MSG_DEBUG("   +++ Used hits: " << n_usedhits << ", IBL overflows: " << IBLOverflow );;
      ATH_MSG_DEBUG("   +++ Original number of measurements = " << pixelhits << " (map size = " << dEdxMap.size() << ") ");
      return averagedEdx;
   }

   // -- false return
   ATH_MSG_DEBUG("dEdx not calculated, return 0");
   return 0.;
}

bool TrigFastTrackFinder::isCleaningPassDisTrack(const TrigInDetTriplet& seed, Trk::Track* trk, bool isFail) const
{
   const float  PT_CUT                            = 3.0;
   //
   const double FAIL_CHI2_OV_NDOF_CUT             = 20.0;
   //
   const int    COMB_N_HITS_IBL_OR_BL_CUT         = 1;
   const int    COMB_N_HITS_PIX_BR_CUT            = 3;
   const double COMB_CHI2_OV_NDOF_PIX_BR_CUT      = 3.0;
   const int    COMB_N_GOOD_HITS_SCT_BR_CUT       = 2;
   const int    COMB_N_GOOD_DOUBLEHITS_SCT_BR_CUT = 0;

   // sanity check
   if( trk==nullptr )                      return false;
   if( trk->perigeeParameters()==nullptr ) return false;
   if( trk->fitQuality()==nullptr )        return false;

   // if allpix/barrel
   if( !seed.s1().isPixel() || !seed.s2().isPixel() || !seed.s3().isPixel() ) return false;
   float s1_z = seed.s1().z();
   float s2_z = seed.s2().z();
   float s3_z = seed.s3().z();
   const float PIXEL_BARREL_Z = 410.0;
   if( std::abs(s1_z) > PIXEL_BARREL_Z || std::abs(s2_z) > PIXEL_BARREL_Z || std::abs(s3_z) > PIXEL_BARREL_Z ) return false;
   ATH_MSG_VERBOSE("FTF::isCleaningPassDisTrack> ... barrel cut passed");

   // pt cut
   double theta  = trk->perigeeParameters()->parameters()[Trk::theta];
   double qOverP = std::abs(trk->perigeeParameters()->parameters()[Trk::qOverP]);
   if ( qOverP < 1e-12 ) qOverP = 1e-12;
   double pt = sin(theta)/qOverP;
   if( pt/1000.0 < PT_CUT ) return false;
   ATH_MSG_VERBOSE("FTF::isCleaningPassDisTrack> ... pT cut passed");

   // fail/comb dependent cuts
   if( isFail ) {
      double chi2 = trk->fitQuality()->chiSquared();
      double ndof = trk->fitQuality()->doubleNumberDoF();
      if( std::abs(ndof) < 1e-12 ) return false;
      if( chi2/ndof > FAIL_CHI2_OV_NDOF_CUT ) return false;
      ATH_MSG_VERBOSE("FTF::isCleaningPassDisTrack> ... (failTrk) Chi2 cut passed");
   }
   else {

      const auto & barrelInfo = getTrkBarrelLayerInfo(trk);

      int n_hits_iblbl = barrelInfo[0].nHits + barrelInfo[1].nHits;
      if( n_hits_iblbl < COMB_N_HITS_IBL_OR_BL_CUT ) return false;

      // PIX cuts
      int  n_hits_pixbr = 0;
      double chi2_pixbr = 0;
      int    ndof_pixbr = 0;
      for(unsigned int ily=0; ily<=3; ily++) {
	      n_hits_pixbr += barrelInfo[ily].nHits;
	      chi2_pixbr   += barrelInfo[ily].chiSq;
	      ndof_pixbr   += barrelInfo[ily].nDof;
      }
      if( n_hits_pixbr < COMB_N_HITS_PIX_BR_CUT ) return false;
      if( ndof_pixbr < 1 )  return false;
      double chi2_ov_ndof_pixbr = chi2_pixbr / ndof_pixbr;
      if( chi2_ov_ndof_pixbr > COMB_CHI2_OV_NDOF_PIX_BR_CUT ) return false;
      ATH_MSG_VERBOSE("FTF::isCleaningPassDisTrack> ... (combTrk) Pix cut passed");

      // SCT cuts
      int n_hits_sctbr_good = 0;
      int n_doublehits_sctbr_good = 0;
      for(unsigned int ily=4; ily<=7; ily++) {
	      n_hits_sctbr_good += barrelInfo[ily].nGood;
	      if( barrelInfo[ily].nGood >= 2 ) n_doublehits_sctbr_good++;
      }
      if( n_hits_sctbr_good > COMB_N_GOOD_HITS_SCT_BR_CUT ) return false;
      if( n_doublehits_sctbr_good > COMB_N_GOOD_DOUBLEHITS_SCT_BR_CUT ) return false;
      ATH_MSG_VERBOSE("FTF::isCleaningPassDisTrack> ... (combTrk) SCT cut passed");
   }

   // cut passed
   return true;
}

std::array<TrigFastTrackFinder::OneLayerInfo_t, TrigFastTrackFinder::N_BARREL_LAYERS>
TrigFastTrackFinder::getTrkBarrelLayerInfo(Trk::Track* t) const
{
   static constexpr double CHI2_GOOD_CUT = 3.0;
   //nHits, chiSq, nDof, nGood in array
   std::array<TrigFastTrackFinder::OneLayerInfo_t, TrigFastTrackFinder::N_BARREL_LAYERS> result{};
   if (not t) return result;

   const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = t->trackStateOnSurfaces();
   if (recoTrackStates) {
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();
      for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
	      const Trk::FitQualityOnSurface* fq = (*tsosIter)->fitQualityOnSurface();
	      double x2 = 0;
	      int  ndof = 0;
	      if(fq!=nullptr) {
	        x2 = fq->chiSquared();
	        ndof = fq->numberDoF();
	      }
	      bool chi2_good = (x2 <= CHI2_GOOD_CUT);
	      const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
	      const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	      if (pixclus!=nullptr) {
	        int bec   = m_pixelId->barrel_ec(pixclus->identify());
	        int layer = m_pixelId->layer_disk(pixclus->identify());
	        if ( bec==0 ) {
	          if( layer < 0 || 3 < layer ) {
		          ATH_MSG_WARNING("Pixel layer out of range, layer=" << layer);
		          continue;
	          }
	          result[layer].nHits++;
	          result[layer].chiSq += x2;
	          result[layer].nDof += ndof;
	          if(chi2_good) result[layer].nGood++;
	        }
	      }
	      const InDet::SCT_ClusterOnTrack *sctclus = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(measurement);
	      if (sctclus!=nullptr) {
	      int bec   = m_sctId->barrel_ec(sctclus->identify());
	      int layer = m_sctId->layer_disk(sctclus->identify());
	      if ( bec==0 ) {
	        if( layer < 0 || 3 < layer ) {
		        ATH_MSG_WARNING("SCT layer out of range, layer=" << layer);
		        continue;
	        }
          layer += 4;
          result[layer].nHits++;
          result[layer].chiSq += x2;
          result[layer].nDof += ndof;
          if(chi2_good) result[layer].nGood++;
	        }
	      }
      }//end loop on TSoS
   }
   return result;
}

double TrigFastTrackFinder::disTrackQuality(const Trk::Track* Tr) const
{
   DataVector<const Trk::TrackStateOnSurface>::const_iterator
      m  = Tr->trackStateOnSurfaces()->begin(),
      me = Tr->trackStateOnSurfaces()->end  ();

   double quality_pixel = 0. ;
   double quality_sct   = 0. ;

   const double W = 17.;

   for(; m!=me; ++m) {
      const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
      if(!fq) continue;

      double x2 = fq->chiSquared();
      double q;
      if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5));
      else                     q =      (W-x2    );
      if(q < 0.) q = 0.;

      const Trk::MeasurementBase *measurement = (*m)->measurementOnTrack();
      if (measurement) {
	 const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	 if( pixclus !=0 ) quality_pixel += q;
	 else quality_sct += q;
      }
   }

   //
   double quality = quality_pixel;
   quality -= quality_sct;
   if( quality < 0. ) quality = 0.;

   return quality;
}

void TrigFastTrackFinder::recoVertexForDisTrack(const EventContext& ctx, TrackCollection& tracks, std::vector<double>& v_xvtx, std::vector<double>& v_yvtx, std::vector<double>& v_zvtx) const
{
   v_xvtx.clear();
   v_yvtx.clear();
   v_zvtx.clear();

   // beamspot and tilt
   SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
   Amg::Vector3D vertex = beamSpotHandle->beamPos();
   double xVTX = vertex.x();
   double yVTX = vertex.y();
   double tiltXZ = beamSpotHandle->beamTilt(0);
   double tiltYZ = beamSpotHandle->beamTilt(1);

   // zvertex
   const double CLUSTCUT_DIST_SIGMA = 5.0;
   const double CLUSTCUT_DIST       = 2.5;
   const double CLUSTCUT_SEED_PT    = 3.0;

   const int VTXCUT_N_TRACKS = 3;
   const int VTXCUT_ALGO     = 1; // 0: highest pT track, 1: sumPt

   std::vector<std::tuple<int,double,double,Trk::Track*>> QT;
   QT.reserve(tracks.size());

   for (auto t=tracks.begin(); t!=tracks.end(); ++t) {
      if( ! isGoodForDisTrackVertex(*t) ) continue;
      // consider for vertex fitting (idx, sort, weight, Trk)
      double theta = (*t)->perigeeParameters()->parameters()[Trk::theta];
      double qOverP = std::abs((*t)->perigeeParameters()->parameters()[Trk::qOverP]);
      if ( qOverP < 1e-12 ) qOverP = 1e-12;
      double pt = sin(theta)/qOverP;
      pt /= 1000.0;
      QT.emplace_back(std::make_tuple(-1,pt,pt,(*t)));
   }

   // sort
   std::sort(QT.begin(), QT.end(),
	     [](const std::tuple<int,double,double,Trk::Track*>& lhs, const std::tuple<int,double,double,Trk::Track*>& rhs) {
		return std::get<1>(lhs) > std::get<1>(rhs); } );

   // clustering
   std::vector<int>    cluster_ntrk;
   std::vector<double> cluster_ptsum;
   std::vector<double> cluster_z;
   std::vector<double> cluster_wsum;
   std::vector<double> cluster_zerr;
   std::vector<double> cluster_w2sum;

   for(unsigned int i=0; i<QT.size(); ++i) {
      Trk::Track* t = std::get<3>(QT[i]);
      double z = t->perigeeParameters()->parameters()[Trk::z0];
      double zerr = sqrt((*(t->perigeeParameters()->covariance()))(Trk::z0,Trk::z0));
      double w  = std::get<2>(QT[i]);
      double pt = std::get<1>(QT[i]);
      if(i==0) {
	 cluster_ntrk.push_back(1);             cluster_ptsum.push_back(pt);
	 cluster_z.push_back(w*z);              cluster_wsum.push_back(w);
	 cluster_zerr.push_back(w*zerr*w*zerr); cluster_w2sum.push_back(w*w);
	 continue;
      }
      //
      const int IDX_INITIAL = 100;
      double dist_min = 100.0;
      int idx_min     = IDX_INITIAL;
      for(unsigned j=0; j<cluster_z.size(); ++j) {
	 double dist = std::abs(z - cluster_z[j]/cluster_wsum[j]);
	 if( dist < dist_min ) {
	    dist_min = dist;
	    idx_min  = j;
	 }
      }
      int match_idx = IDX_INITIAL;
      if( idx_min != IDX_INITIAL ) {
	 double c_zerr_min = std::sqrt(cluster_zerr[idx_min]/cluster_w2sum[idx_min]);
	 double err = std::sqrt(zerr*zerr+c_zerr_min*c_zerr_min);
	 if( std::abs(err) < 1e-12 ) err = 1e-12;
	 double dist = dist_min / err;
	 if( dist < CLUSTCUT_DIST_SIGMA && dist_min < CLUSTCUT_DIST ) { match_idx = idx_min; }
      }
      //
      if( match_idx == IDX_INITIAL ) {
	 if( pt > CLUSTCUT_SEED_PT && dist_min > CLUSTCUT_DIST ) {
	    cluster_ntrk.push_back(1);             cluster_ptsum.push_back(pt);
	    cluster_z.push_back(w*z);              cluster_wsum.push_back(w);
	    cluster_zerr.push_back(w*zerr*w*zerr); cluster_w2sum.push_back(w*w);
	 }
	 continue;
      }
      int    new_n     = cluster_ntrk[match_idx]  + 1;
      double new_ptsum = cluster_ptsum[match_idx] + pt;
      double new_z     = cluster_z[match_idx]     + w*z;
      double new_wsum  = cluster_wsum[match_idx]  + w;
      double new_zerr  = cluster_zerr[match_idx]  + w*zerr*w*zerr;
      double new_w2sum = cluster_w2sum[match_idx] + w*w;
      cluster_ntrk[match_idx]  = new_n;
      cluster_ptsum[match_idx] = new_ptsum;
      cluster_z[match_idx]     = new_z;
      cluster_wsum[match_idx]  = new_wsum;
      cluster_zerr[match_idx]  = new_zerr;
      cluster_w2sum[match_idx] = new_w2sum;
   }
   QT.clear();

   // determine zvtx (pt sort)
   std::vector<std::tuple<double,double,double,int>> zVtx;
   zVtx.reserve(tracks.size());
   for(unsigned int i=0; i<cluster_z.size(); i++) {
      if( cluster_ntrk[i] < VTXCUT_N_TRACKS ) continue;
      double z = cluster_z[i] / cluster_wsum[i];
      double zerr = std::sqrt(cluster_zerr[i] / cluster_w2sum[i]);
      zVtx.push_back(std::make_tuple(cluster_ptsum[i],z,zerr,cluster_ntrk[i]));
   }
   // ptsum sort
   if( VTXCUT_ALGO == 1 ) {
      std::sort(zVtx.begin(), zVtx.end(),
		[](const std::tuple<double,double,double,int>& lhs, const std::tuple<double,double,double,int>& rhs) {
		   return std::get<0>(lhs) > std::get<0>(rhs); } );
   }
   ATH_MSG_VERBOSE("disTrkZVtertex> =====  looping zVtx size: " << zVtx.size());
   for(unsigned int i=0; i<zVtx.size(); i++) {
      double z    = std::get<1>(zVtx[i]);
      double zerr = std::get<2>(zVtx[i]);
      double pt   = std::get<0>(zVtx[i]);
      int    n    = std::get<3>(zVtx[i]);
      v_zvtx.push_back(z);
      v_xvtx.push_back(xVTX - tiltXZ*z); //correction for tilt
      v_yvtx.push_back(yVTX - tiltYZ*z); //correction for tilt
      ATH_MSG_VERBOSE("disTrkZVtertex> Vertex cand i=" << i << ":  z = " << z << " +- " << zerr << ", sum n / pt = " << n << " / " << pt);
   }

   // monitoring
   auto mnt_disTrk_nVtx  = Monitored::Scalar<int>  ("disTrk_nVtx",  0);
   auto mnt_disTrk_xVtx  = Monitored::Scalar<float>("disTrk_xVtx",  0);
   auto mnt_disTrk_yVtx  = Monitored::Scalar<float>("disTrk_yVtx",  0);
   auto mnt_disTrk_zVtx  = Monitored::Scalar<float>("disTrk_zVtx",  0);
   auto monDisTrk = Monitored::Group(m_monTool, mnt_disTrk_nVtx, mnt_disTrk_xVtx, mnt_disTrk_yVtx, mnt_disTrk_zVtx);
   mnt_disTrk_nVtx = v_zvtx.size();
   if(v_zvtx.size()>0) {
      mnt_disTrk_xVtx = v_xvtx[0];
      mnt_disTrk_yVtx = v_yvtx[0];
      mnt_disTrk_zVtx = v_zvtx[0];
   }
}

bool TrigFastTrackFinder::isGoodForDisTrackVertex(Trk::Track* t) const
{
   const double TRKCUT_CHI2_OV_NDOF        = 3.0;
   const double TRKCUT_PT                  = 1.0;
   const double TRKCUT_D0                  = 2.0;
   const int    TRKCUT_N_HITS_INNER        = 1;
   const int    TRKCUT_N_HITS_PIX          = 3;
   const int    TRKCUT_N_HITS              = 7;

   // sanity check
   if ( ! t->perigeeParameters() ) return false;
   if ( ! t->fitQuality() )        return false;
   if ( t->trackSummary()==0 ) {
      m_trackSummaryTool->updateTrack(*t);
      if ( t->trackSummary()==0 )  return false;
   }

   // chi2
   double chi2  = t->fitQuality()->chiSquared();
   double ndof  = t->fitQuality()->doubleNumberDoF();
   if( std::abs(ndof) < 1e-2 ) return false;
   double chi2_ov_ndof = chi2/ndof;
   if( chi2_ov_ndof > TRKCUT_CHI2_OV_NDOF )  return false;

   // pt
   double theta = t->perigeeParameters()->parameters()[Trk::theta];
   double qOverP = std::abs(t->perigeeParameters()->parameters()[Trk::qOverP]);
   if ( qOverP < 1e-12 ) qOverP = 1e-12;
   double pt    = std::sin(theta)/qOverP;
   pt /= 1000.0;
   if( pt < TRKCUT_PT ) return false;

   // d0
   double d0 = t->perigeeParameters()->parameters()[Trk::d0];
   if( std::abs(d0) > TRKCUT_D0 ) return false;

   // nr hits
   int n_hits_innermost = t->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
   int n_hits_next_to_innermost = t->trackSummary()->get(Trk::SummaryType::numberOfNextToInnermostPixelLayerHits);
   int n_hits_inner = n_hits_innermost + n_hits_next_to_innermost;
   int n_hits_pix = t->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
   int n_hits_sct = t->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
   if( n_hits_inner  < TRKCUT_N_HITS_INNER )     return false;
   if( n_hits_pix    < TRKCUT_N_HITS_PIX )       return false;
   if( (n_hits_pix+n_hits_sct) < TRKCUT_N_HITS ) return false;

   // ok
   return true;
}

void TrigFastTrackFinder::filterSharedDisTracks(std::vector<std::tuple<bool, double,Trk::Track*>>& QT) const
{
   const int N_FREE_PIX_HITS_CUT = 2;

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
	    const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>((*m));
	    if (pixclus) {
	       ++nc;
	       if(clusters.find(pr)==clusters.end()) {prd[nf++]=pr; if(nf==100) break;}
	    }
	 }
      }
      if((nf >= N_FREE_PIX_HITS_CUT) || (nf == nc) ) {
	 for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
      }
      else  {
	 std::get<0>(q) = false;
      }
   }
}

StatusCode TrigFastTrackFinder::findDisTracks(const EventContext& ctx,
					      TrackCollection& tracks,
					      std::vector<std::tuple<bool, double, Trk::Track*>>& qualityDisFailTrks,
					      std::vector<std::tuple<bool, double, Trk::Track*>>& qualityDisCombTrks,
					      TrackCollection& fittedDisCombTrks,
					      const std::vector<double>& v_xvtx,
					      const std::vector<double>& v_yvtx,
					      const std::vector<double>& v_zvtx) const
{
   SG::WriteHandle<xAOD::TrigCompositeContainer> disTrkCandHandle(m_disTrkCandKey, ctx);
   ATH_CHECK( disTrkCandHandle.record(std::make_unique<xAOD::TrigCompositeContainer>(), std::make_unique<xAOD::TrigCompositeAuxContainer>()) );
   auto disTrkCandContainer = disTrkCandHandle.ptr();

   // monitoring
   auto mnt_disFailTrk_n      = Monitored::Scalar<int>("disFailTrk_n",      0);
   auto mnt_disFailTrk_nclone = Monitored::Scalar<int>("disFailTrk_nclone", 0);
   auto mnt_disFailTrk_ncand  = Monitored::Scalar<int>("disFailTrk_ncand",  0);
   auto mnt_disCombTrk_n      = Monitored::Scalar<int>("disCombTrk_n",      0);
   auto mnt_disCombTrk_nclone = Monitored::Scalar<int>("disCombTrk_nclone", 0);
   auto mnt_disCombTrk_ncand  = Monitored::Scalar<int>("disCombTrk_ncand",  0);
   auto monDisTrk             = Monitored::Group(m_monTool, mnt_disFailTrk_n, mnt_disFailTrk_nclone, mnt_disFailTrk_ncand, mnt_disCombTrk_n, mnt_disCombTrk_nclone, mnt_disCombTrk_ncand);

   // select tracks to be used for isolation calculation
   std::vector<Trk::Track*> tracksForIso;
   for (auto t=tracks.begin(); t!=tracks.end(); ++t) {
      if( isGoodForDisTrackVertex(*t) ) tracksForIso.push_back(*t);
   }

   //
   const std::string prefix = "disTrkCand";

   // disFailTrk
   TrackCollection initialDisFailTrks;
   initialDisFailTrks.reserve(qualityDisFailTrks.size());
   std::vector<int> resultCodes;
   for(const auto& q : qualityDisFailTrks) {
      if (std::get<0>(q)==true) {
	 initialDisFailTrks.emplace_back(std::get<2>(q));
      }
      else {
	 delete std::get<2>(q);
      }
   }
   ATH_MSG_VERBOSE("===> nr of disFailTrk=" << qualityDisFailTrks.size() << " -> clone removal=" << initialDisFailTrks.size());

   TrackCollection fittedDisFailTrks;
   m_trigInDetTrackFitter->fit(initialDisFailTrks, fittedDisFailTrks, ctx, m_particleHypothesis);
   int n_disFailTrkCands = recoAndFillDisTrkCand(prefix, &fittedDisFailTrks, tracksForIso, disTrkCandContainer, v_xvtx, v_yvtx, v_zvtx, true);
   ATH_MSG_VERBOSE("disFailTrk: nr of cands = " << n_disFailTrkCands);

   mnt_disFailTrk_n      = qualityDisFailTrks.size();
   mnt_disFailTrk_nclone = initialDisFailTrks.size();
   mnt_disFailTrk_ncand  = n_disFailTrkCands;

   // disCombTrk
   ATH_MSG_VERBOSE("===> nr of disCombTrk=" << qualityDisCombTrks.size() << " -> clone removal=" << fittedDisCombTrks.size());
   int n_disCombTrkCands = recoAndFillDisTrkCand(prefix, &fittedDisCombTrks, tracksForIso, disTrkCandContainer, v_xvtx, v_yvtx, v_zvtx, false);
   ATH_MSG_VERBOSE("disCombTrk: nr of cands = " << n_disCombTrkCands);

   mnt_disCombTrk_n      = qualityDisCombTrks.size();
   mnt_disCombTrk_nclone = fittedDisCombTrks.size();
   mnt_disCombTrk_ncand  = n_disCombTrkCands;

   return StatusCode::SUCCESS;
}

bool TrigFastTrackFinder::isPreselPassDisTrack(Trk::Track* trk, double d0_wrtVtx, double z0_wrtVtx) const
{
   const double PRESEL_D0_WRTVTX =  5.0;
   const double PRESEL_Z0_WRTVTX = 50.0;
   const int    PRESEL_N_GOOD_BR_LAYERS_PIX    = 3;
   const double PRESEL_CHI2_OV_NDOF_PIX_BR_CUT = 5.0;

   // sanity check
   if( trk == nullptr ) return false;
   if( trk->perigeeParameters() == nullptr ) return false;

   // barrel hits
   const auto & barrelLayerInfo = getTrkBarrelLayerInfo(trk);

   // PIX cuts
   double chi2_pixbr = 0.0;
   int    ndof_pixbr = 0;
   int    n_good_brlayers_pix = 0;
   for(unsigned int ily=0; ily<=3; ily++) {
      if( barrelLayerInfo[ily].nGood >= 1 ) n_good_brlayers_pix++;
      chi2_pixbr += barrelLayerInfo[ily].chiSq;
      ndof_pixbr += barrelLayerInfo[ily].nDof;
   }
   if( n_good_brlayers_pix < PRESEL_N_GOOD_BR_LAYERS_PIX ) return false;

   if( ndof_pixbr < 1 )  return false;
   double chi2_ov_ndof_pixbr = chi2_pixbr / ndof_pixbr;
   if( chi2_ov_ndof_pixbr > PRESEL_CHI2_OV_NDOF_PIX_BR_CUT ) return false;

   // d0
   if( std::abs(d0_wrtVtx) > PRESEL_D0_WRTVTX ) return false;

   // z0
   if( std::abs(z0_wrtVtx) > PRESEL_Z0_WRTVTX ) return false;

   // cut passed
   return true;
}

const Trk::Perigee* TrigFastTrackFinder::extrapolateDisTrackToBS(Trk::Track* t,
								 const std::vector<double>& v_xvtx,
								 const std::vector<double>& v_yvtx,
								 const std::vector<double>& v_zvtx) const
{
   float vtx_x  = 0;
   float vtx_y  = 0;
   float vtx_z  = 9999;
   float trk_z0 = t->perigeeParameters()->parameters()[Trk::z0];
   float z0_min = 9999;
   for(unsigned int i_vtx=0; i_vtx<v_zvtx.size(); i_vtx++) {
      float z = v_zvtx[i_vtx];
      if( std::abs(trk_z0-z) < z0_min ) {
	 z0_min = std::abs(trk_z0-z);
	 vtx_z  = z;
	 vtx_x  = v_xvtx[i_vtx];
	 vtx_y  = v_yvtx[i_vtx];
      }
   }

   Amg::Vector3D gp(vtx_x, vtx_y, vtx_z);
   Trk::PerigeeSurface persf(gp);
   const Trk::Perigee* vertexPerigee   = 0;
   const Trk::Perigee* trackparPerigee = t->perigeeParameters();
   vertexPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(
       Gaudi::Hive::currentContext(),
       (*trackparPerigee),
       persf));

   return vertexPerigee;
}

TrigFastTrackFinder::DisTrkCategory TrigFastTrackFinder::getDisTrkCategory(Trk::Track* trk) const
{
   const auto & result = getTrkBarrelLayerInfo(trk);

   int n_good_brlayers_pix = 0;
   int n_hits_sct = 0;
   for(unsigned int ily=0; ily<8; ily++) {
      if( ily<=3 && result[ily].nGood >= 1 ) n_good_brlayers_pix++;
      if( 4<=ily ) {
	      n_hits_sct += result[ily].nHits;
      }
   }
   if( trk->trackSummary()!=0 ) { n_hits_sct = trk->trackSummary()->get(Trk::SummaryType::numberOfSCTHits); }

   // category
   DisTrkCategory cat = DisTrkCategory::Other;
   if( n_good_brlayers_pix == 4 ) {
      if( n_hits_sct==0 ) { cat=DisTrkCategory::Pix4l_Sct0; }
      else                { cat=DisTrkCategory::Pix4l_Sct1p; }
   }
   else if( n_good_brlayers_pix == 3 ) {
      if( n_hits_sct==0 ) { cat=DisTrkCategory::Pix3l_Sct0; }
      else                { cat=DisTrkCategory::Pix3l_Sct1p; }
   }
   return cat;
}

void TrigFastTrackFinder::fillDisTrkCand(xAOD::TrigComposite* comp, const std::string& prefix, Trk::Track* trk, const Trk::Perigee* vertexPerigee) const
{
   std::vector<Trk::Track*> vtmp;
   fillDisTrkCand(comp,prefix,trk,vertexPerigee,false,vtmp);
}

void TrigFastTrackFinder::fillDisTrkCand(xAOD::TrigComposite* comp, const std::string& prefix, Trk::Track* trk, const Trk::Perigee* vertexPerigee,
					 bool fillIso, std::vector<Trk::Track*>& tracksForIso) const
{
   // category
   int category = (trk != nullptr) ? (int)getDisTrkCategory(trk) : -1;
   if( prefix.find("refit") == std::string::npos ) comp->setDetail<int>(prefix+"_category",category);

   // track
   float theta=0; float eta=0; float pt=0; float d0=0; float z0=0; float phi=0; float chi2=0; float ndof=0;
   int n_hits_innermost=-1; int n_hits_next_to_innermost=-1; int n_hits_inner=-1; int n_hits_pix=-1; int n_hits_sct=-1;
   if( trk != nullptr ) {
      theta = trk->perigeeParameters()->parameters()[Trk::theta];
      eta   = -std::log(std::tan(0.5*theta));
      float qOverP = std::abs(trk->perigeeParameters()->parameters()[Trk::qOverP]);
      if ( qOverP < 1e-12 ) qOverP = 1e-12;
      pt    = sin(theta)/qOverP;
      d0    = trk->perigeeParameters()->parameters()[Trk::d0];
      z0    = trk->perigeeParameters()->parameters()[Trk::z0];
      phi   = trk->perigeeParameters()->parameters()[Trk::phi];
      chi2  = trk->fitQuality()->chiSquared();
      ndof  = trk->fitQuality()->doubleNumberDoF();
      if( trk->trackSummary()!=0 ) {
	 n_hits_pix = trk->trackSummary()->get(Trk::SummaryType::numberOfPixelHits);
	 n_hits_sct = trk->trackSummary()->get(Trk::SummaryType::numberOfSCTHits);
	 n_hits_innermost = trk->trackSummary()->get(Trk::SummaryType::numberOfInnermostPixelLayerHits);
	 n_hits_next_to_innermost = trk->trackSummary()->get(Trk::SummaryType::numberOfNextToInnermostPixelLayerHits);
	 n_hits_inner = n_hits_innermost + n_hits_next_to_innermost;
      }
   }
   comp->setDetail<float>(prefix+"_pt",   pt);
   comp->setDetail<float>(prefix+"_eta",  eta);
   comp->setDetail<float>(prefix+"_phi",  phi);
   comp->setDetail<float>(prefix+"_d0",   d0);
   comp->setDetail<float>(prefix+"_z0",   z0);
   comp->setDetail<float>(prefix+"_chi2", chi2);
   comp->setDetail<float>(prefix+"_ndof", ndof);
   comp->setDetail<int>  (prefix+"_n_hits_innermost", n_hits_innermost);
   comp->setDetail<int>  (prefix+"_n_hits_inner",     n_hits_inner);
   comp->setDetail<int>  (prefix+"_n_hits_pix",       n_hits_pix);
   comp->setDetail<int>  (prefix+"_n_hits_sct",       n_hits_sct);

   // extrapolate
   float theta_wrtVtx=0; float eta_wrtVtx=0; float pt_wrtVtx=0; float d0_wrtVtx=0; float z0_wrtVtx=0; float phi_wrtVtx=0;
   if( vertexPerigee != nullptr ) {
      theta_wrtVtx = vertexPerigee->parameters()[Trk::theta];
      eta_wrtVtx   = -std::log(std::tan(0.5*theta_wrtVtx));
      float qOverP_wrtVtx = std::abs(vertexPerigee->parameters()[Trk::qOverP]);
      if ( qOverP_wrtVtx < 1e-12 ) qOverP_wrtVtx = 1e-12;
      pt_wrtVtx    = std::sin(theta_wrtVtx)/qOverP_wrtVtx;
      d0_wrtVtx    = vertexPerigee->parameters()[Trk::d0];
      z0_wrtVtx    = vertexPerigee->parameters()[Trk::z0];
      phi_wrtVtx   = vertexPerigee->parameters()[Trk::phi];
   }
   comp->setDetail<float>(prefix+"_pt_wrtVtx",  pt_wrtVtx);
   comp->setDetail<float>(prefix+"_eta_wrtVtx", eta_wrtVtx);
   comp->setDetail<float>(prefix+"_phi_wrtVtx", phi_wrtVtx);
   comp->setDetail<float>(prefix+"_d0_wrtVtx",  d0_wrtVtx);
   comp->setDetail<float>(prefix+"_z0_wrtVtx",  z0_wrtVtx);

   // barrel hits
   std::array<OneLayerInfo_t, N_BARREL_LAYERS> barrelInfo{};
   int n_ibl  =-1; int n_pix1  =-1; int n_pix2  =-1; int n_pix3  =-1; int n_sct1  =-1; int n_sct2  =-1; int n_sct3  =-1; int n_sct4  =-1;
   int n_ibl_g=-1; int n_pix1_g=-1; int n_pix2_g=-1; int n_pix3_g=-1; int n_sct1_g=-1; int n_sct2_g=-1; int n_sct3_g=-1; int n_sct4_g=-1;

   barrelInfo = getTrkBarrelLayerInfo(trk);
   n_ibl = barrelInfo[0].nHits; n_pix1=barrelInfo[1].nHits; n_pix2=barrelInfo[2].nHits; n_pix3=barrelInfo[3].nHits;
   n_sct1=barrelInfo[4].nHits; n_sct2=barrelInfo[5].nHits; n_sct3=barrelInfo[6].nHits; n_sct4=barrelInfo[7].nHits;
   //
   n_ibl_g =barrelInfo[0].nGood; n_pix1_g=barrelInfo[1].nGood; n_pix2_g=barrelInfo[2].nGood; n_pix3_g=barrelInfo[3].nGood;
   n_sct1_g=barrelInfo[4].nGood; n_sct2_g=barrelInfo[5].nGood; n_sct3_g=barrelInfo[6].nGood; n_sct4_g=barrelInfo[7].nGood;

   comp->setDetail<int>  (prefix+"_n_brhits_ibl",       n_ibl);
   comp->setDetail<int>  (prefix+"_n_brhits_pix1",      n_pix1);
   comp->setDetail<int>  (prefix+"_n_brhits_pix2",      n_pix2);
   comp->setDetail<int>  (prefix+"_n_brhits_pix3",      n_pix3);
   comp->setDetail<int>  (prefix+"_n_brhits_sct1",      n_sct1);
   comp->setDetail<int>  (prefix+"_n_brhits_sct2",      n_sct2);
   comp->setDetail<int>  (prefix+"_n_brhits_sct3",      n_sct3);
   comp->setDetail<int>  (prefix+"_n_brhits_sct4",      n_sct4);
   comp->setDetail<float>(prefix+"_chi2sum_br_ibl",     barrelInfo[0].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_pix1",    barrelInfo[1].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_pix2",    barrelInfo[2].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_pix3",    barrelInfo[3].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_sct1",    barrelInfo[4].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_sct2",    barrelInfo[5].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_sct3",    barrelInfo[6].chiSq);
   comp->setDetail<float>(prefix+"_chi2sum_br_sct4",    barrelInfo[7].chiSq);
   comp->setDetail<float>(prefix+"_ndofsum_br_ibl",     barrelInfo[0].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_pix1",    barrelInfo[1].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_pix2",    barrelInfo[2].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_pix3",    barrelInfo[3].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_sct1",    barrelInfo[4].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_sct2",    barrelInfo[5].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_sct3",    barrelInfo[6].nDof);
   comp->setDetail<float>(prefix+"_ndofsum_br_sct4",    barrelInfo[7].nDof);
   comp->setDetail<int>  (prefix+"_n_brhits_good_ibl",  n_ibl_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_pix1", n_pix1_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_pix2", n_pix2_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_pix3", n_pix3_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_sct1", n_sct1_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_sct2", n_sct2_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_sct3", n_sct3_g);
   comp->setDetail<int>  (prefix+"_n_brhits_good_sct4", n_sct4_g);

   // isolation
   if( fillIso ) {
      const float ISOL_CALC_Z0_DIFF_CUT          = 2.5;
      const float ISOL_CALC_DR_CUT_TO_AVOID_ZERO = 0.015;
      float iso1_dr01=0; float iso1_dr02=0; float iso1_dr04=0;
      float iso2_dr01=0; float iso2_dr02=0; float iso2_dr04=0;
      float iso3_dr01=0; float iso3_dr02=0; float iso3_dr04=0;
      for(auto t=tracksForIso.begin(); t!=tracksForIso.end(); t++) {
	 float z0_t   = (*t)->perigeeParameters()->parameters()[Trk::z0];
	 if( std::abs(z0_t - z0) <= ISOL_CALC_Z0_DIFF_CUT ) {
	    float theta_t = (*t)->perigeeParameters()->parameters()[Trk::theta];
	    float qOverP_t= std::abs((*t)->perigeeParameters()->parameters()[Trk::qOverP]);
	    if ( qOverP_t < 1e-12 ) qOverP_t = 1e-12;
	    float pt_t    = std::sin(theta_t)/qOverP_t;
	    float phi_t   = (*t)->perigeeParameters()->parameters()[Trk::phi];
	    float eta_t   = -std::log(std::tan(theta_t/2.0));
	    float deta    = eta_t - eta;
	    float dphi    = std::abs(phi_t - phi);
	    if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
	    float dr   = std::sqrt(deta*deta + dphi*dphi);
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.1 && pt_t > 1000.0 ) iso1_dr01 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.2 && pt_t > 1000.0 ) iso1_dr02 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.4 && pt_t > 1000.0 ) iso1_dr04 += pt_t;
	    //
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.1 && pt_t > 2000.0 ) iso2_dr01 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.2 && pt_t > 2000.0 ) iso2_dr02 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.4 && pt_t > 2000.0 ) iso2_dr04 += pt_t;
	    //
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.1 && pt_t > 3000.0 ) iso3_dr01 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.2 && pt_t > 3000.0 ) iso3_dr02 += pt_t;
	    if( dr > ISOL_CALC_DR_CUT_TO_AVOID_ZERO && dr<0.4 && pt_t > 3000.0 ) iso3_dr04 += pt_t;
	 }
      }
      comp->setDetail<float>(prefix+"_iso1_dr01", iso1_dr01);
      comp->setDetail<float>(prefix+"_iso1_dr02", iso1_dr02);
      comp->setDetail<float>(prefix+"_iso1_dr04", iso1_dr04);
      comp->setDetail<float>(prefix+"_iso2_dr01", iso2_dr01);
      comp->setDetail<float>(prefix+"_iso2_dr02", iso2_dr02);
      comp->setDetail<float>(prefix+"_iso2_dr04", iso2_dr04);
      comp->setDetail<float>(prefix+"_iso3_dr01", iso3_dr01);
      comp->setDetail<float>(prefix+"_iso3_dr02", iso3_dr02);
      comp->setDetail<float>(prefix+"_iso3_dr04", iso3_dr04);
   }
}

int TrigFastTrackFinder::recoAndFillDisTrkCand(const std::string& base_prefix,
					       TrackCollection* tracks, std::vector<Trk::Track*>& tracksForIso,
					       xAOD::TrigCompositeContainer* trigCompositeContainer,
					       const std::vector<double>& v_xvtx,
					       const std::vector<double>& v_yvtx,
					       const std::vector<double>& v_zvtx,
					       bool isFail) const
{
   std::string prefix;

   int n_stored_tracks = 0;
   int idx = -1;

   for (auto trk = tracks->begin(); trk!=tracks->end(); ++trk) {

      idx++;

      Trk::Track* ptrk = *trk;

      if( ptrk == nullptr ) continue;
      if( ptrk->perigeeParameters()==nullptr ) continue;

      // extrapolate to vertex
      const Trk::Perigee* vertexPerigee = extrapolateDisTrackToBS(ptrk,v_xvtx,v_yvtx,v_zvtx);
      double d0 = ptrk->perigeeParameters()->parameters()[Trk::d0];
      double z0 = ptrk->perigeeParameters()->parameters()[Trk::z0];
      double d0_wrtVtx = 0;
      double z0_wrtVtx = 0;
      if( vertexPerigee != nullptr ) {
	 d0_wrtVtx = vertexPerigee->parameters()[Trk::d0];
	 z0_wrtVtx = vertexPerigee->parameters()[Trk::z0];
	 ATH_MSG_VERBOSE("d0 : " << d0 << " -> extrapolate -> " << d0_wrtVtx);
	 ATH_MSG_VERBOSE("z0 : " << z0 << " -> extrapolate -> " << z0_wrtVtx);
      }

      // pre-selection
      if( ! isPreselPassDisTrack(ptrk,d0_wrtVtx,z0_wrtVtx) ) continue;

      // store it!
      n_stored_tracks++;

      xAOD::TrigComposite *comp = new xAOD::TrigComposite();
      comp->makePrivateStore();
      trigCompositeContainer->push_back(comp);

      m_trackSummaryTool->updateTrack(*ptrk);

      //
      int is_fail = isFail ? 1 : 0;
      comp->setDetail<int>(base_prefix+"_is_fail",is_fail);

      // store trk info
      prefix = base_prefix;
      fillDisTrkCand(comp,prefix,ptrk,vertexPerigee,true,tracksForIso);

      // refit
      std::unique_ptr<Trk::Track> refit_trk = disTrk_refit(ptrk);
      if( refit_trk != nullptr ) m_trackSummaryTool->updateTrack(*refit_trk);

      // extrapolate refitted track to vertex
      const Trk::Perigee* refitVertexPerigee = nullptr;
      if( refit_trk != nullptr ) {
	 refitVertexPerigee = extrapolateDisTrackToBS(refit_trk.get(),v_xvtx,v_yvtx,v_zvtx);
	 if( refitVertexPerigee == nullptr ) {
	    ATH_MSG_VERBOSE("extrapote to BS fails for refit track");
	 }
	 else {
	    float d0 = refit_trk.get()->perigeeParameters()->parameters()[Trk::d0];
	    float z0 = refit_trk.get()->perigeeParameters()->parameters()[Trk::z0];
	    float d0_wrtVtx    = refitVertexPerigee->parameters()[Trk::d0];
	    float z0_wrtVtx    = refitVertexPerigee->parameters()[Trk::z0];
	    ATH_MSG_VERBOSE("refit trk d0 : " << d0 << " -> extrapolate -> " << d0_wrtVtx);
	    ATH_MSG_VERBOSE("refit trk z0 : " << z0 << " -> extrapolate -> " << z0_wrtVtx);
	 }
      }

      // store refit trk info
      prefix = base_prefix + "_refit";
      if( refit_trk != nullptr ) {
	 fillDisTrkCand(comp,prefix,refit_trk.get(),refitVertexPerigee);
      }
      else {
	 fillDisTrkCand(comp,prefix,nullptr,refitVertexPerigee);
      }
   }

   //
   ATH_MSG_VERBOSE("========> filling trigcomposite for " << prefix << " end");
   ATH_MSG_VERBOSE("nr of " << prefix << " tracks / stored = " << tracks->size() << " / " << n_stored_tracks);

   //
   return n_stored_tracks;
}

std::unique_ptr<Trk::Track> TrigFastTrackFinder::disTrk_refit(Trk::Track* t) const
{
   std::unique_ptr<Trk::Track> newtrack = nullptr;

   if( t == nullptr ) return newtrack;
   if( t->trackSummary() == nullptr ) m_trackSummaryTool->updateTrack(*t);

   ATH_MSG_VERBOSE("refitting - input track:");
   print_disTrk(t);

   const Trk::Perigee* origPerigee = t->perigeeParameters();
   if( origPerigee == nullptr ) return newtrack;
   ATH_MSG_VERBOSE("... origPerigee is there");

   // remove SCT hits
   std::vector<const Trk::MeasurementBase*> vec;
   int n_measurements       = 0;
   int n_measurements_refit = 0;
   const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = t->trackStateOnSurfaces();
   if (recoTrackStates) {
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();
      for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
	 const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
	 if (measurement) {
	    n_measurements++;
	    const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	    const InDet::SCT_ClusterOnTrack  *sctclus = dynamic_cast<const InDet::SCT_ClusterOnTrack*>(measurement);
	    bool to_add = true;
	    if ( !pixclus && sctclus ) to_add = false;
	    if( to_add ) {
	       vec.push_back(measurement);
	       n_measurements_refit++;
	    }
	 }
      }
   }
   ATH_MSG_VERBOSE("... Nr of measurments / refit = " << n_measurements << " / " << n_measurements_refit);

   // perform refit
   newtrack = m_disTrkFitter->fit(Gaudi::Hive::currentContext(),vec, *origPerigee, false, m_particleHypothesis); // false to run outlier switch
   ATH_MSG_VERBOSE("... ---> refit track:");
   if( newtrack!=0 && newtrack.get() ) {
      print_disTrk(dynamic_cast<const Trk::Track*>(newtrack.get()));
   }
   else {
      ATH_MSG_VERBOSE("... refit failed");
   }

   //
   return newtrack;
}

void TrigFastTrackFinder::print_disTrk(const Trk::Track* t) const
{
   float chi2=0; float ndof=0; float d0=0; float z0=0; float phi=0; float theta=0; float pt=0;
   if( t!=nullptr ) {
      chi2  = t->fitQuality()->chiSquared();
      ndof  = t->fitQuality()->doubleNumberDoF();
      d0    = t->perigeeParameters()->parameters()[Trk::d0];
      z0    = t->perigeeParameters()->parameters()[Trk::z0];
      phi   = t->perigeeParameters()->parameters()[Trk::phi];
      theta = t->perigeeParameters()->parameters()[Trk::theta];
      float qOverP = std::abs(t->perigeeParameters()->parameters()[Trk::qOverP]);
      if ( qOverP < 1e-12 ) qOverP = 1e-12;
      pt = sin(theta)/qOverP;
      pt /= 1000.0;
   }
   ATH_MSG_DEBUG("... pt / theta / phi / d0 / z0 = " << pt << " / " << theta << " / " << phi << " / " << d0 << " / " << z0);
   ATH_MSG_DEBUG("... chi2 / ndof = " << chi2 << " / " << ndof);
}
