/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "GaudiKernel/ThreadLocalContext.h"

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

#include "TrigInDetEvent/TrigSiSpacePointBase.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 

#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"


#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"

#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigZFinder.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

//for GPU acceleration

#include "TrigInDetAccelerationTool/ITrigInDetAccelerationTool.h"
#include "TrigInDetAccelerationService/ITrigInDetAccelerationSvc.h"

#include "TrigFastTrackFinder.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CxxUtils/phihelper.h"

#include "TrigNavigation/NavigationCore.icc"

#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/ThreadLocalContext.h"

//for GPU acceleration

#include "TrigAccelEvent/TrigInDetAccelEDM.h"
#include "TrigAccelEvent/TrigInDetAccelCodes.h"

TrigFastTrackFinder::TrigFastTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) : 

  HLT::FexAlgo(name, pSvcLocator), 
  m_numberingTool("TrigL2LayerNumberTool"), 
  m_spacePointTool("TrigSpacePointConversionTool"),
  m_trackMaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_trigInDetTrackFitter("TrigInDetTrackFitter"),
  m_trigZFinder("TrigZFinder/TrigZFinder", this ),
  m_trackSummaryTool("Trk::ITrackSummaryTool/ITrackSummaryTool"),
  m_accelTool("TrigInDetAccelerationTool"), 
  m_accelSvc("TrigInDetAccelerationSvc", name),
  m_doCloneRemoval(true),
  m_useBeamSpot(true),
  m_doZFinder(false),
  m_doZFinderOnly(false),
  m_storeZFinderVertices(false),
  m_nfreeCut(5), 
  m_attachedFeatureName(""),
  m_outputCollectionSuffix(""),
  m_countTotalRoI(0),
  m_countRoIwithEnoughHits(0),
  m_countRoIwithTracks(0),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_particleHypothesis(Trk::pion),
  m_useNewLayerNumberScheme(false), 
  m_useGPU(false)
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
  declareProperty( "doZFinderOnly",       m_doZFinderOnly = false);

  declareProperty( "doFastZVertexSeeding",        m_doFastZVseeding = true);
  declareProperty( "zVertexResolution",           m_tcs.m_zvError = 10.0);
  declareProperty( "StoreZFinderVertices",        m_storeZFinderVertices = false );


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
  declareProperty( "doResMon",       m_doResMonitoring = true);

  declareProperty("doCloneRemoval", m_doCloneRemoval = true);


  declareProperty("useNewLayerNumberScheme", m_useNewLayerNumberScheme = false);

  declareProperty("useGPU", m_useGPU = false);

  // declare monitoring histograms

}

//--------------------------------------------------------------------------

TrigFastTrackFinder::~TrigFastTrackFinder() {}

//-----------------------------------------------------------------------

HLT::ErrorCode TrigFastTrackFinder::hltInitialize() {

  if (m_roiCollectionKey.initialize().isFailure() ) {
    return HLT::BAD_JOB_SETUP;
  }
  if (m_outputTracksKey.initialize().isFailure() ) {
    return HLT::BAD_JOB_SETUP;
  }

  // optional PRD to track association map
  if (m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ).isFailure()) {
    return HLT::BAD_JOB_SETUP;
  }
  
    
  auto scbs = m_beamSpotKey.initialize();
  if(scbs.isFailure()) {
    ATH_MSG_ERROR("Error initializing beamspot info");
    return HLT::BAD_JOB_SETUP;
  }



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

  sc= m_trackSummaryTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("unable to locate track summary tool");
    return HLT::BAD_JOB_SETUP;
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

  // Run3 monitoring
  if ( !m_monTool.empty() ) {
     if ( !m_monTool.retrieve() ) {
	ATH_MSG_ERROR("Cannot retrieve MonitoredTool");
	return HLT::BAD_JOB_SETUP;
     }
  }
  else {
     ATH_MSG_INFO("Monitoring tool is empty");
  }

  ATH_MSG_DEBUG(" Feature set recorded with Key " << m_attachedFeatureName);
  ATH_MSG_DEBUG(" doResMon " << m_doResMonitoring);

  if(m_useGPU) {//for GPU acceleration
    sc = m_accelSvc.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve "<<m_accelSvc); 
      m_useGPU = false;
    }
    if(!m_accelSvc->isReady()) {
      ATH_MSG_INFO("Acceleration service not ready - no GPU found"); 
      m_useGPU = false;
    }
    else {
      sc = m_accelTool.retrieve();
      if(sc.isFailure()) {
        ATH_MSG_ERROR("Could not retrieve "<<m_accelTool); 
        m_useGPU = false;
      }
    }
  }

  ATH_MSG_INFO("Use GPU acceleration : "<<std::boolalpha<<m_useGPU);
  
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

  m_tcs.m_tripletPtMin = m_tripletMinPtFrac*m_pTmin;

  return HLT::OK;
}

namespace InDet {
  class ExtendedSiTrackMakerEventData_xk : public InDet::SiTrackMakerEventData_xk
  {
  public:
    ExtendedSiTrackMakerEventData_xk(const SG::ReadHandleKey<Trk::PRDtoTrackMap> &key, const EventContext& ctx) { 
      if (!key.key().empty()) {
        m_prdToTrackMap = SG::ReadHandle<Trk::PRDtoTrackMap>(key, ctx);
        if (!m_prdToTrackMap.isValid()) {
          throw std::runtime_error(std::string("Failed to get PRD to track map:") + key.key());
        }
        setPRDtoTrackMap(m_prdToTrackMap.cptr());
      }
    }
  private:
    void dummy() {}
    SG::ReadHandle<Trk::PRDtoTrackMap> m_prdToTrackMap;
  };

  class FeatureAccessor : public HLT::FexAlgo 
  {
  public:
    //make the getFeature method public
    template<class T> HLT::ErrorCode getFeature(const HLT::TriggerElement* te, const T*&  feature, 
                                                 const std::string& label = "") {
      return HLT::Algo::getFeature(te,feature,label);
    }
  };

  class FexSiTrackMakerEventData_xk : public InDet::SiTrackMakerEventData_xk
  {
  public:
    FexSiTrackMakerEventData_xk(HLT::FexAlgo &algo, const HLT::TriggerElement* outputTE, const std::string &key) {
      if (!key.empty()) {
        const Trk::PRDtoTrackMap *prd_to_track_map_cptr;
        HLT::ErrorCode stat = reinterpret_cast<FeatureAccessor &>(algo).getFeature(outputTE, prd_to_track_map_cptr, key);
        if(stat!= HLT::OK){
          throw std::runtime_error(std::string("Failed to get PRD to track map:") + key);
        }
        setPRDtoTrackMap(prd_to_track_map_cptr);
      }
    }
  private:
    void dummy() {}
  };

}

StatusCode TrigFastTrackFinder::execute() {

  auto ctx = getContext();
  //RoI preparation/update 

  SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey, ctx);

  ATH_CHECK(roiCollection.isValid());

  TrigRoiDescriptor internalRoI;

  if ( roiCollection->size()>1 ) ATH_MSG_WARNING( "More than one Roi in the collection: " << m_roiCollectionKey << ", this is not supported - use a composite Roi" );  
  if ( roiCollection->size()>0 ) internalRoI = **roiCollection->begin();

  //  internalRoI.manageConstituents(false);//Don't try to delete RoIs at the end
  m_countTotalRoI++;

  SG::WriteHandle<TrackCollection> outputTracks(m_outputTracksKey, ctx);
  outputTracks = std::make_unique<TrackCollection>();

  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(m_prdToTrackMap, ctx);
  ATH_CHECK(findTracks(trackEventData, internalRoI, *outputTracks, ctx));
  
  return StatusCode::SUCCESS;
}


//-------------------------------------------------------------------------
HLT::ErrorCode TrigFastTrackFinder::hltExecute(const HLT::TriggerElement*,
    HLT::TriggerElement* outputTE) {
  const IRoiDescriptor* internalRoI;
  HLT::ErrorCode ec = getRoI(outputTE, internalRoI);
  if(ec != HLT::OK) {
    return ec;
  }
  TrackCollection* outputTracks = new TrackCollection(SG::OWN_ELEMENTS);
  InDet::FexSiTrackMakerEventData_xk trackEventData(*this, outputTE, m_prdToTrackMap.key());

  StatusCode sc = findTracks(trackEventData, *internalRoI, *outputTracks, getContext());
  HLT::ErrorCode code = HLT::OK;
  if (sc != StatusCode::SUCCESS) {
    delete outputTracks;
    code = attachFeature(outputTE, new TrackCollection(SG::VIEW_ELEMENTS), m_attachedFeatureName);
    if (code != HLT::OK) {
      return code;
    }
    return HLT::ERROR;
  }

  code = attachFeature(outputTE, outputTracks, m_attachedFeatureName);
  
  return code;
}


StatusCode TrigFastTrackFinder::findTracks(InDet::SiTrackMakerEventData_xk &trackEventData,
                                           const TrigRoiDescriptor& roi,
                                           TrackCollection& outputTracks,
                                           const EventContext& ctx) const {
  // Run3 monitoring ---------->
  auto mnt_roi_nTracks = Monitored::Scalar<int>("roi_nTracks", 0);
  auto mnt_roi_nSPs    = Monitored::Scalar<int>("roi_nSPs",    0);
  auto mnt_roi_nSPsPIX = Monitored::Scalar<int>("roi_nSPsPIX", 0);
  auto mnt_roi_nSPsSCT = Monitored::Scalar<int>("roi_nSPsSCT", 0);
  auto monSP = Monitored::Group(m_monTool, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT);

  auto mnt_timer_SpacePointConversion  = Monitored::Timer<std::chrono::milliseconds>("TIME_SpacePointConversion");
  auto mnt_timer_PatternReco           = Monitored::Timer<std::chrono::milliseconds>("TIME_PattReco");
  auto mnt_timer_TripletMaking         = Monitored::Timer<std::chrono::milliseconds>("TIME_Triplets");
  auto mnt_timer_CombTracking          = Monitored::Timer<std::chrono::milliseconds>("TIME_CmbTrack");
  auto mnt_timer_TrackFitter           = Monitored::Timer<std::chrono::milliseconds>("TIME_TrackFitter");
  auto monTime = Monitored::Group(m_monTool, mnt_roi_nTracks, mnt_roi_nSPs, mnt_timer_SpacePointConversion,
				  mnt_timer_PatternReco, mnt_timer_TripletMaking, mnt_timer_CombTracking, mnt_timer_TrackFitter);

  auto mnt_roi_lastStageExecuted = Monitored::Scalar<int>("roi_lastStageExecuted", 0);
  auto monDataError              = Monitored::Group(m_monTool, mnt_roi_lastStageExecuted);

  mnt_timer_SpacePointConversion.start(); // Run3 monitoring

  
  mnt_roi_lastStageExecuted = 1; // Run3 monitoring

  std::vector<TrigSiSpacePointBase> convertedSpacePoints;
  convertedSpacePoints.reserve(5000);
  ATH_CHECK(m_spacePointTool->getSpacePoints(roi, convertedSpacePoints, mnt_roi_nSPsPIX, mnt_roi_nSPsSCT, ctx));

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
    return StatusCode::SUCCESS;
  }

  mnt_roi_lastStageExecuted = 2; // Run3 monitoring


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
    
    mnt_timer_ZFinder.stop(); // Run3 monitoring
    
    if (  m_doZFinderOnly ) { 
      /// write vertex collection ... 
      /// TODO: add vertices collection handling here,
      /// should not be 0 at this point unless fastZVseeding 
      /// is enabled
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

  std::map<Identifier, std::vector<long int> > siClusterMap;

  bool PIX = true;
  bool SCT = true;
  m_trackMaker->newTrigEvent(ctx, trackEventData, PIX, SCT);

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

    std::list<Trk::Track*> tracks = m_trackMaker->getTracks(ctx, trackEventData, spVec);

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

  m_trackMaker->endEvent(trackEventData);

  //clone removal
  if(m_doCloneRemoval) {
    filterSharedTracks(qualityTracks);
  }

  TrackCollection initialTracks;
  initialTracks.reserve(qualityTracks.size());
  for(const auto& q : qualityTracks) {
    if (std::get<0>(q)==true) {
      initialTracks.push_back(std::get<2>(q));
    }
    else {
      delete std::get<2>(q);
    }
  }
  qualityTracks.clear();

  ATH_MSG_DEBUG("After clone removal "<<initialTracks.size()<<" tracks left");


  mnt_timer_CombTracking.stop(); // Run3 monitoring
  mnt_timer_PatternReco.stop();  // Run3 monitoring

  mnt_roi_lastStageExecuted = 5; // Run3 monitoring

  mnt_timer_TrackFitter.start(); // Run3 monitoring

  m_trigInDetTrackFitter->fit(initialTracks, outputTracks, ctx, m_particleHypothesis);

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

  mnt_timer_TrackFitter.stop(); // Run3 monitoring

  if( outputTracks.empty() ) {
    ATH_MSG_DEBUG("REGTEST / No tracks reconstructed");
  }
  mnt_roi_lastStageExecuted = 6; // Run3 monitoring

  //monitor Z-vertexing

  //monitor number of tracks
  ATH_MSG_DEBUG("REGTEST / Found " << outputTracks.size() << " tracks");
  if( !outputTracks.empty() )
    m_countRoIwithTracks++;

  ///////////// fill vectors of quantities to be monitored
  fillMon(outputTracks, *vertices, roi, ctx);

  mnt_roi_lastStageExecuted = 7; // Run3 monitoring

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

HLT::ErrorCode TrigFastTrackFinder::hltFinalize()
{

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("TrigFastTrackFinder::finalize() - TrigFastTrackFinder Statistics: ");
  ATH_MSG_INFO("RoI processed: " <<  m_countTotalRoI);
  ATH_MSG_INFO("RoI with enough SPs : " <<  m_countRoIwithEnoughHits);
  ATH_MSG_INFO("RoI with Track(s)  Total/goodZvertex/badZvertex: " << m_countRoIwithTracks);
  ATH_MSG_INFO("=========================================================");

  return HLT::OK;
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
    //no sorting is needed as the vectors are sorted by the algorithm design
    //due to monotonically increasing trackIdx
    // std::map<Identifier, std::vector<long int> >::iterator itm = clusterMap.find(id);
    //std::sort((*itm).second.begin(),(*itm).second.end());
    //std::copy((*itm).second.begin(),(*itm).second.end(),std::ostream_iterator<long int>(std::cout," "));
    //std::cout<<std::endl;
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
    // Run3 monitoring ---------->
    mnt_trk_a0.push_back(a0);
    mnt_trk_z0.push_back(z0);
    mnt_trk_phi0.push_back(phi0);
    mnt_trk_a0beam.push_back(a0+shift_x*sin(phi0)-shift_y*cos(phi0));
    mnt_trk_eta.push_back(eta);
    for(unsigned int i=0; i<roi.size(); i++) {
       mnt_trk_dPhi0.push_back(CxxUtils::wrapToPi(phi0 - (roi.at(i))->phi()));
       mnt_trk_dEta.push_back(eta - (roi.at(i))->eta());
    }
    // <---------- Run3 monitoring

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
    // <---------- Run3 monitoring

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

  // Run3 monitoring ---------->
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
  // <---------- Run3 monitoring

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
        // Run3 monitoring ---------->
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
        // <---------- Run3 monitoring
        break;
      case Region::PixEndcap :
        ATH_MSG_DEBUG("Pixel Endcap "  );
        // Run3 monitoring ---------->
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
        // Run3 monitoring ---------->
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
        // <---------- Run3 monitoring
        break;
      case Region::SctEndcap :
        ATH_MSG_DEBUG("SCT Endcap"  );
        // Run3 monitoring ---------->
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
        // <---------- Run3 monitoring
        break;
      case Region::IBL :
        // Run3 monitoring ---------->
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
