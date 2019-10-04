/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelMonitoring/PixelAthClusterMonTool.h"

PixelAthClusterMonTool::PixelAthClusterMonTool( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool", this),
  m_trackSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_atlasid(nullptr),
  m_pixelid(nullptr)
{
  //jo flags go here, keys and some tools -> in class
  declareProperty("HoleSearchTool", m_holeSearchTool);
  declareProperty("TrackSelectionTool", m_trackSelTool); //needed for cfg in python jo

  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doModules", m_doModules = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = true);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
}


PixelAthClusterMonTool::~PixelAthClusterMonTool() {}


StatusCode PixelAthClusterMonTool::initialize() {

  ATH_CHECK( detStore()->retrieve(m_atlasid, "AtlasID") );
  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  if ( !m_holeSearchTool.empty() ) ATH_CHECK( m_holeSearchTool.retrieve() );
  if ( !m_trackSelTool.empty() ) ATH_CHECK( m_trackSelTool.retrieve() );

  ATH_CHECK( m_tracksKey.initialize() );
  ATH_CHECK( m_clustersKey.initialize() );
  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthClusterMonTool::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();

  //*******************************************************************************
  //************************** Begin of filling Status Histograms ******************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Status Monitoring Histograms");
  float index = 1.0;
  float nBadMod[PixLayers::COUNT] = {0.};
  float nDisabledMod[PixLayers::COUNT] = {0.};
  int phiMod(-99);
  int etaMod(-99);
  bool copyFEval(false);
  int clusPerEventArray_disksA[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk] = { {0} };
  int clusPerEventArray_disksC[PixMon::kNumModulesDisk][PixMon::kNumLayersDisk] = { {0} };
  int clusPerEventArray_l0[PixMon::kNumStavesL0][PixMon::kNumModulesBarrel] = { {0} };
  int clusPerEventArray_l1[PixMon::kNumStavesL1][PixMon::kNumModulesBarrel] = { {0} };
  int clusPerEventArray_l2[PixMon::kNumStavesL2][PixMon::kNumModulesBarrel] = { {0} };
  int clusPerEventArray_lI[PixMon::kNumStavesIBL][PixMon::kNumFEsIBL] = { {0} };  

  for (auto idIt = m_pixelid->wafer_begin(); idIt!=m_pixelid->wafer_end(); ++idIt) {
    Identifier     waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);
    
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    if (pixlayer == 99) continue;
       
    // read https://twiki.cern.ch/twiki/bin/view/AtlasComputing/MultiThreadingConditionsAccess
    if (m_pixelCondSummaryTool->isActive(id_hash) == true && m_pixelCondSummaryTool->isGood(id_hash) == true) {
      index = 0;
    } else if (m_pixelCondSummaryTool->isActive(id_hash) == false) {
      index = 2;  // inactive or bad modules
      nDisabledMod[pixlayer]++;
      getPhiEtaMod(m_pixelid, waferID, phiMod, etaMod, copyFEval);
      switch (pixlayer) {
      case PixLayers::kECA : 
	clusPerEventArray_disksA[phiMod][etaMod] = -1;
	break;
      case PixLayers::kECC : 
	clusPerEventArray_disksC[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB0 : 
	clusPerEventArray_l0[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB1 : 
	clusPerEventArray_l1[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB2 : 
	clusPerEventArray_l2[phiMod][etaMod] = -1;
	break;
      case PixLayers::kIBL : 
	clusPerEventArray_lI[phiMod][etaMod] = -1;
	if (copyFEval) clusPerEventArray_lI[phiMod][++etaMod] = -1; 
	break;
      }
    } else {
      index = 1;  // bad but active modules
      nBadMod[pixlayer]+= inv_nmod_per_layer[pixlayer];
    }
    
    CHECK( fill2DProfLayer("Map_Of_Modules_Status_"+pixLayersLabel[pixlayer], waferID, m_pixelid, index, true) );
    CHECK( fill2DProfLayer("Map_Of_Modules_Status_Mon_"+pixLayersLabel[pixlayer], waferID, m_pixelid, index, true) );
    if (m_doLumiBlock) CHECK( fill2DProfLayer("Map_Of_Modules_Status_LB_"+pixLayersLabel[pixlayer], waferID, m_pixelid, index, true) );
  }  // end of pixelid wafer loop

  CHECK( fill1DProfLumiLayers("BadModules_per_lumi", lb, nBadMod) );
  CHECK( fill1DProfLumiLayers("DisabledModules_per_lumi", lb, nDisabledMod) );

  //*******************************************************************************
  //*************************** End of filling Status Histograms ******************
  //*******************************************************************************


  //*******************************************************************************
  //************************** Begin of filling Track Histograms ******************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Track Monitoring Histograms");

  auto tracks = SG::makeHandle(m_tracksKey, ctx);

  if ( !(tracks.isValid()) ) {
    ATH_MSG_ERROR("PixelMonitoring: Track container "<< m_tracksKey.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("PixelMonitoring: Track container "<< tracks.name() <<" is found.");
  }

  int ntracksPerEvent = 0;
  std::vector<std::pair<Identifier, double> > ClusterIDs;

  auto lbval    = Monitored::Scalar<int>( "pixclusmontool_lb", lb );

  TrackCollection::const_iterator itrack = tracks->begin();
  TrackCollection::const_iterator itrack_end = tracks->end();
  for (; itrack != itrack_end; ++itrack) {
    if ((*itrack) == nullptr || (*itrack)->perigeeParameters() == nullptr || (*itrack)->trackSummary() == nullptr || (*itrack)->trackSummary()->get(Trk::numberOfPixelHits) == 0) {
      ATH_MSG_DEBUG("PixelMonitoring: Track either invalid or it does not contain pixel hits, continuing...");
      continue;
    }

    const Trk::Perigee* measPerigee = static_cast<const Trk::Perigee *>((*itrack)->perigeeParameters());
    std::unique_ptr<const Trk::Track> trackWithHoles( m_holeSearchTool->getTrackWithHoles(**itrack) );

    int nPixelHits = 0;
    bool passJOTrkTightCut = static_cast<bool>( m_trackSelTool->accept(**itrack) );
    bool pass1hole1GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 1.0));  // misshit ratios
    bool pass1hole5GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 5.0));  // eff vs lumi

    const DataVector<const Trk::TrackStateOnSurface> *trackStates = trackWithHoles->trackStateOnSurfaces();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator = trackStates->begin(); trackStateOnSurfaceIterator != trackStates->end(); trackStateOnSurfaceIterator++) {

      const Trk::MeasurementBase* mesBase = (*trackStateOnSurfaceIterator)->measurementOnTrack();

      const Trk::RIO_OnTrack* RIOOnTrack = nullptr;
      if ( mesBase && mesBase->type(Trk::MeasurementBaseType::RIO_OnTrack) ) {
        RIOOnTrack = static_cast<const Trk::RIO_OnTrack*>(mesBase);
      }

      if (mesBase && !RIOOnTrack) continue;  // skip pseudomeasurements but not hits, holes, outliers

      const Trk::TrackParameters* trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
      Identifier surfaceID;
      if (mesBase && mesBase->associatedSurface().associatedDetectorElement()) {
        surfaceID = mesBase->associatedSurface().associatedDetectorElement()->identify();
      } else {  // holes, perigee
        if (trkParameters) {
          surfaceID = trkParameters->associatedSurface().associatedDetectorElementIdentifier();
        } else {
          ATH_MSG_INFO("PixelMonitoring: pointer of TSOS to track parameters or associated surface is null");
          continue;
        }
      }
      if (!m_atlasid->is_pixel(surfaceID)) continue;
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID) );
      if (pixlayer == 99) continue;
     
      float nOutlier = 0.;
      float nHole    = 0.;
      auto effval  = Monitored::Scalar<float>( "HitEff_all_val", 0. );
      auto efflb   = Monitored::Scalar<float>( "HitEff_all_lb", lb );
      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)) {
        nOutlier = 1.0;
	if (!m_doOnline) CHECK( fill2DProfLayer("TSOS_Outlier_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, 1.0, true) );
        if (pass1hole5GeVptTightCut) fill("HitEff_all_" + pixLayersLabel[pixlayer], efflb, effval);

      } else if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)) {
        nHole = 1.0;
	if (!m_doOnline) CHECK( fill2DProfLayer("TSOS_Hole_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, 1.0, true) );
        if (pass1hole5GeVptTightCut) fill("HitEff_all_" + pixLayersLabel[pixlayer], efflb, effval);

      } else if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)) {

	if (!m_doOnline) CHECK( fill2DProfLayer("TSOS_Measurement_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, 1.0, true) );
	effval = 1.;
	if (pass1hole5GeVptTightCut) fill("HitEff_all_" + pixLayersLabel[pixlayer], efflb, effval);

	if (not mesBase) continue;
	const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(mesBase->associatedSurface().associatedDetectorElement());
	const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack *>(mesBase);
	if (!side || !clus) continue;
	const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster *>(clus->prepRawData());
	if (!RawDataClus || !RawDataClus->detectorElement()->isPixel()) continue;
	
	nPixelHits++;
	
	const Trk::AtaPlane *trackAtPlane = dynamic_cast<const Trk::AtaPlane *>(trkParameters);
	if (trackAtPlane) {
	  const Amg::Vector2D localpos = trackAtPlane->localPosition();
	  
	  // Get local error matrix for hit and track and calc pull
	  const AmgSymMatrix(5) trackErrMat = (*trackAtPlane->covariance());
	  const Amg::MatrixX clusErrMat = clus->localCovariance();
	  
	  double error_sum = sqrt(pow(Amg::error(trackErrMat, Trk::locX), 2) + pow(Amg::error(clusErrMat, Trk::locX), 2));
	  auto  resPhi = Monitored::Scalar<float>( "res_phi", clus->localParameters()[Trk::locX] - localpos[0]);	  
	  fill("Track_res_phi", resPhi);
	  if (error_sum != 0) {
	    auto pullPhi = Monitored::Scalar<float>( "pull_phi", resPhi / error_sum);
	    fill("Track_pulls_phi", pullPhi);
	  }

	  error_sum = sqrt(pow(Amg::error(trackErrMat, Trk::locY), 2) + pow(Amg::error(clusErrMat, Trk::locY), 2));
	  auto  resEta = Monitored::Scalar<float>( "res_eta", clus->localParameters()[Trk::locY] - localpos[1]);
	  fill("Track_res_eta", resEta);
	  if (error_sum != 0) {
	    auto pullEta = Monitored::Scalar<float>( "pull_eta", resEta / error_sum);
	    fill("Track_pulls_eta", pullEta);
	  }
	  // Filling containers, which hold id's of hits and clusters on track 
	  // _and_ incident angle information for later normalization

	  Amg::Vector3D mynormal = side->normal();
	  Amg::Vector3D mytrack = trackAtPlane->momentum();
	  double trknormcomp = mytrack.dot(mynormal);

	  double mytrack_mag = mytrack.mag();
	  double cosalpha = 0.;
	  if (mytrack_mag != 0) cosalpha = fabs(trknormcomp / mytrack_mag);
	  ClusterIDs.push_back(std::make_pair(clus->identify(), cosalpha));
	}


      } // end of measurement case
    
      if (pass1hole1GeVptTightCut) {
	CHECK( fill2DProfLayer("HolesRatio_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, nHole, true) );
	CHECK( fill2DProfLayer("MissHitsRatio_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, nOutlier + nHole, true) ); 
        if (m_doOnline) {
	  CHECK( fill2DProfLayer("HolesRatio_5min_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, nHole, true) );
	  CHECK( fill2DProfLayer("MissHitsRatio_5min_"+pixLayersLabel[pixlayer], surfaceID, m_pixelid, nOutlier + nHole, true) );
        }
      }
 
    } // end of TSOS loop

    ntracksPerEvent++;
    auto nph    = Monitored::Scalar<int>( "npixhits_per_track", nPixelHits);
    auto nphwgt = Monitored::Scalar<float>( "npixhits_per_track_wgt", 1.0 );
    fill("NPixhits_per_track_lumi", lbval, nph, nphwgt);

    int     trkfitndf = (*itrack)->fitQuality()->numberDoF();
    double trkfitchi2 = (*itrack)->fitQuality()->chiSquared();
    if (trkfitndf != 0) {
      auto trkChiN = Monitored::Scalar<float>( "fit_chi2byndf", trkfitchi2 / trkfitndf);
      fill("Track_chi2byndf", trkChiN);
    }
  } // end of track loop

  sort(ClusterIDs.begin(), ClusterIDs.end(), [](const std::pair<Identifier, double> &left, const std::pair<Identifier, double> &right) {
      return left.first < right.first;
    });
  

  // Filling per-event histograms
  //
  auto nTrks   = Monitored::Scalar<int>( "ntrks_per_event", ntracksPerEvent);
  fill("tracksPerEvt_per_lumi", lbval, nTrks);

  //m_npixhits_per_track_lastXlb-> // m_npixhits_per_track_lumi TH2F vs lumi
  //*******************************************************************************
  //**************************** End of filling Track Histograms ******************
  //*******************************************************************************

  //*******************************************************************************
  //************************ Begin of filling Cluster Histograms ******************
  //*******************************************************************************

  ATH_MSG_DEBUG("Filling Cluster Monitoring Histograms");

  auto pixel_clcontainer = SG::makeHandle(m_clustersKey, ctx);

  if ( !(pixel_clcontainer.isValid()) ) {
    ATH_MSG_ERROR("Pixel Monitoring: Pixel Cluster container "<< m_clustersKey.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel Monitoring: Pixel Cluster container "<< pixel_clcontainer.name() <<" is found.");
  }

  int nclusters = 0;
  int nclusters_ontrack = 0;
  float nclusters_mod[PixLayers::COUNT] = {0.};
  float nclusters_ontrack_mod[PixLayers::COUNT] = {0.};

  Identifier clusID;
  InDet::PixelClusterContainer::const_iterator colNext = pixel_clcontainer->begin();
  InDet::PixelClusterContainer::const_iterator lastCol = pixel_clcontainer->end();
  DataVector<InDet::PixelCluster>::const_iterator p_clus;

  for (; colNext != lastCol; ++colNext) {

    const InDet::PixelClusterCollection* ClusterCollection(*colNext);
    if (!ClusterCollection) {
      ATH_MSG_DEBUG("Pixel Monitoring: Pixel Cluster container is empty.");
      continue;
    }

    for (p_clus = ClusterCollection->begin(); p_clus != ClusterCollection->end(); ++p_clus) {
      clusID = (*p_clus)->identify();
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID) );
      if (pixlayer == 99) continue;

      const InDet::PixelCluster& cluster = **p_clus;

      nclusters++;
      nclusters_mod[pixlayer]++;

      // begin timing histos
      //
      auto clLVL1A   = Monitored::Scalar<float>( "Cluster_LVL1A_lvl1a", cluster.LVL1A() );
      fill( addTxt("Cluster_LVL1A", false), clLVL1A);
      CHECK( fill2DProfLayer( addTxt("Cluster_LVL1A_Mod", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid, cluster.LVL1A() + 0.00001) );
      if (cluster.rdoList().size() > 1) CHECK( fill2DProfLayer( addTxt("Cluster_LVL1A_SizeCut", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid, cluster.LVL1A() + 0.00001) );
      if (cluster.totalToT() > clusterToTMinCut[pixlayer]) fill( addTxt("Cluster_LVL1A_ToTCut", false) + "_" + pixLayersLabel[pixlayer], clLVL1A);


      // 
      // end timing histos
      // begin cluster rate
      //
      if (m_doOnline) CHECK( fill2DProfLayer( addTxt("ClusterMap_Mon", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
      // 
      // end cluster rate
      // begin cluster occupancy
      //
      CHECK( fill2DProfLayer( addTxt("Cluster_Occupancy", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
      if (cluster.rdoList().size() > 1) CHECK( fill2DProfLayer( addTxt("Clus_Occ_SizeCut", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
      if (m_doLumiBlock) CHECK( fill2DProfLayer( addTxt("Cluster_Occupancy_LB", false)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
      // 
      // end cluster occupancy
      double cosalpha(0.); 
      if ( isClusterOnTrack(clusID, ClusterIDs, cosalpha) ) {
	nclusters_ontrack++;
	nclusters_ontrack_mod[pixlayer]++;
	getPhiEtaMod(m_pixelid, clusID, phiMod, etaMod, copyFEval);
	switch (pixlayer) {
	case PixLayers::kECA : 
	  clusPerEventArray_disksA[phiMod][etaMod]++;
	  break;
	case PixLayers::kECC : 
	  clusPerEventArray_disksC[phiMod][etaMod]++;
	  break;
	case PixLayers::kB0 : 
	  clusPerEventArray_l0[phiMod][etaMod]++;
	  break;
	case PixLayers::kB1 : 
	  clusPerEventArray_l1[phiMod][etaMod]++;
	  break;
	case PixLayers::kB2 : 
	  clusPerEventArray_l2[phiMod][etaMod]++;
	  break;
	case PixLayers::kIBL : 
	  clusPerEventArray_lI[phiMod][etaMod]++;
	  break;
	}
	// begin timing histos
	//
	clLVL1A = cluster.LVL1A(); 
	fill( addTxt("Cluster_LVL1A", true), clLVL1A);
	CHECK( fill2DProfLayer( addTxt("Cluster_LVL1A_Mod", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid, cluster.LVL1A() + 0.00001) );
	if (cluster.rdoList().size() > 1) CHECK( fill2DProfLayer( addTxt("Cluster_LVL1A_SizeCut", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid, cluster.LVL1A() + 0.00001) );
	if (cluster.totalToT() > clusterToTMinCut[pixlayer]) fill( addTxt("Cluster_LVL1A_ToTCut", true) + "_" + pixLayersLabel[pixlayer], clLVL1A);
	// 
	// end timing histos
	// begin cluster sizes
	//
	auto clSize    = Monitored::Scalar<float>( Form("%s_%s", addTxt("Cluster_Size", true).c_str(), "clsize"), cluster.rdoList().size());
	auto etaModule = Monitored::Scalar<float>( Form("%s_%s", addTxt("Cluster_Size", true).c_str(), "em"), m_pixelid->eta_module(clusID) );
	if ( abs(m_pixelid->barrel_ec(clusID))!=0 ) etaModule = m_pixelid->layer_disk(clusID) + 1;
	fill( addTxt("Cluster_groupsize_vs_eta", true) + "_" + pixLayersLabel[pixlayer], etaModule, clSize );
	CHECK( fill2DProfLayer( addTxt("Cluster_Size_Map", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid, cluster.rdoList().size()) );
	// 
	// end cluster sizes
	// begin cluster rate
	//
	if (m_doOnline) CHECK( fill2DProfLayer( addTxt("ClusterMap_Mon", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
	// 
	// end cluster rate
	// begin cluster occupancy
	//
	CHECK( fill2DProfLayer( addTxt("Cluster_Occupancy", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
	if (cluster.rdoList().size() > 1) CHECK( fill2DProfLayer( addTxt("Clus_Occ_SizeCut", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
	if (m_doLumiBlock) CHECK( fill2DProfLayer( addTxt("Cluster_Occupancy_LB", true)+"_"+pixLayersLabel[pixlayer], clusID, m_pixelid) );
	// 
	// end cluster occupancy
	// begin cluster ToT and charge
	//
	auto clToTcosAlpha = Monitored::Scalar<float>( Form("%s_%s", addTxt("Cluster_ToTxCosAlpha", true).c_str(), "val"), cluster.totalToT() * cosalpha );
	fill( addTxt("Cluster_ToTxCosAlpha", true) + "_" + pixLayersLabel[pixlayer], clToTcosAlpha);
	if (!m_doOnline) {
	  auto clQcosAlpha   = Monitored::Scalar<float>( Form("%s_%s", addTxt("Cluster_QxCosAlpha", true).c_str(), "val"), cluster.totalCharge() * cosalpha);
	  fill( addTxt("Cluster_QxCosAlpha", true) + "_" + pixLayersLabel[pixlayer], clQcosAlpha);
	}
	// 
	// end cluster ToT and charge   
      }
    }

  }
  // begin cluster rates
  //
  auto nCls   = Monitored::Scalar<int>( "ncls_per_event", nclusters );
  fill( addTxt("Clusters_per_lumi", false), lbval, nCls);
  nCls = nclusters_ontrack;
  fill( addTxt("Clusters_per_lumi", true), lbval, nCls);

  if (nclusters>0) {
    auto clsFracOnTrack = Monitored::Scalar<float>( "cls_frac_ontrack", (float)nclusters_ontrack / nclusters );
    fill( "ClustersOnOffTrack_per_lumi", lbval, clsFracOnTrack);
  }

  CHECK( fill1DProfLumiLayers( addTxt("Clusters_per_lumi", false), lb, nclusters_mod) );
  CHECK( fill1DProfLumiLayers( addTxt("Clusters_per_lumi", true), lb, nclusters_ontrack_mod) );

  CHECK( fillPP0Histos( addTxt("Cluster_Occupancy_PP0", true), 
			clusPerEventArray_disksA, clusPerEventArray_disksC, 
			clusPerEventArray_l0, clusPerEventArray_l1,
			clusPerEventArray_l2, clusPerEventArray_lI ) );

  if (ntracksPerEvent>0) {
    for (unsigned int ii=0; ii<PixLayers::COUNT; ii++) nclusters_ontrack_mod[ii]/= ntracksPerEvent; // keep as in Run 1,2
    CHECK( fill1DProfLumiLayers( "num_clusters_per_track_per_lumi", lb, nclusters_ontrack_mod) );
  }
  // 
  // end cluster rate
  ClusterIDs.clear();
  
  //*******************************************************************************
  //************************** End of filling Cluster Histograms ******************
  //*******************************************************************************

  return StatusCode::SUCCESS;
}

