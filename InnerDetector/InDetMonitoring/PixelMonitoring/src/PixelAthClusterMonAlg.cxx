/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "PixelAthClusterMonAlg.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
//for Amg::error helper function:
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"





PixelAthClusterMonAlg::PixelAthClusterMonAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthMonitorAlgorithm(name, pSvcLocator),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool", this),
  m_trackSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_atlasid(nullptr),
  m_pixelid(nullptr) {
  //jo flags go here, keys and some tools -> in class
  declareProperty("HoleSearchTool", m_holeSearchTool);
  declareProperty("TrackSelectionTool", m_trackSelTool); //needed for cfg in python jo

  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = true);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
  declareProperty("doFEPlots", m_doFEPlots = false);
}

PixelAthClusterMonAlg::~PixelAthClusterMonAlg() {}


StatusCode PixelAthClusterMonAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_atlasid, "AtlasID"));
  ATH_CHECK(detStore()->retrieve(m_pixelid, "PixelID"));
  ATH_CHECK(m_pixelCondSummaryTool.retrieve());
  ATH_CHECK(m_pixelReadout.retrieve());
  if (!m_holeSearchTool.empty()) ATH_CHECK(m_holeSearchTool.retrieve());
  if (!m_trackSelTool.empty()) ATH_CHECK(m_trackSelTool.retrieve());

  ATH_CHECK(m_tracksKey.initialize());
  ATH_CHECK(m_clustersKey.initialize());
  return AthMonitorAlgorithm::initialize();
}

StatusCode PixelAthClusterMonAlg::fillHistograms(const EventContext& ctx) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();

  //*******************************************************************************
  //************************** Begin of filling Status Histograms ******************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Status Monitoring Histograms");
  float index = 1.0;
  float nBadMod[PixLayers::COUNT] = {
    0.
  };
  float nDisabledMod[PixLayers::COUNT] = {
    0.
  };
  float nBadAndDisabledMod[PixLayers::COUNT] = {
    0.
  };
  int phiMod(-99);
  int etaMod(-99);
  bool copyFEval(false);
  AccumulatorArrays clusPerEventArray = {{{0}}, {{0}}, {{0}}, {{0}}, {{0}}, {{0}}};
  VecAccumulator2DMap Map_Of_Modules_Status("MapOfModulesStatus", true);

  VecAccumulator2DMap Map_Of_FEs_Status("MapOfFEsStatus");

  for (auto idIt = m_pixelid->wafer_begin(); idIt != m_pixelid->wafer_end(); ++idIt) {
    Identifier waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);

    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID));
    if (pixlayer == 99) continue;
    getPhiEtaMod(m_pixelid, waferID, phiMod, etaMod, copyFEval);

    if (m_pixelCondSummaryTool->isActive(id_hash) == true && m_pixelCondSummaryTool->isGood(id_hash) == true) {
      index = 0;
    } else if (m_pixelCondSummaryTool->isActive(id_hash) == false) {
      index = 2;  // inactive or bad modules
      if (pixlayer == PixLayers::kIBL) {
	int iblsublayer = (m_pixelid->eta_module(waferID) > -7 && m_pixelid->eta_module(waferID) < 6) ? PixLayers::kIBL2D : PixLayers::kIBL3D;
	nDisabledMod[iblsublayer] += inv_nmod_per_layer[iblsublayer];
      } else nDisabledMod[pixlayer] += inv_nmod_per_layer[pixlayer];
      switch (pixlayer) {
      case PixLayers::kECA:
        clusPerEventArray.DA[phiMod][etaMod] = -1;
        break;

      case PixLayers::kECC:
        clusPerEventArray.DC[phiMod][etaMod] = -1;
        break;

      case PixLayers::kB0:
        clusPerEventArray.B0[phiMod][etaMod] = -1;
        break;

      case PixLayers::kB1:
        clusPerEventArray.B1[phiMod][etaMod] = -1;
        break;

      case PixLayers::kB2:
        clusPerEventArray.B2[phiMod][etaMod] = -1;
        break;

      case PixLayers::kIBL:
        clusPerEventArray.IBL[phiMod][etaMod] = -1;
        if (copyFEval) clusPerEventArray.IBL[phiMod][++etaMod] = -1;
        break;
      }
    } else {
      index = 1;  // bad but active modules
      if (pixlayer == PixLayers::kIBL) {
	int iblsublayer = (m_pixelid->eta_module(waferID) > -7 && m_pixelid->eta_module(waferID) < 6) ? PixLayers::kIBL2D : PixLayers::kIBL3D;
	nBadMod[iblsublayer] += inv_nmod_per_layer[iblsublayer];
      } else nBadMod[pixlayer] += inv_nmod_per_layer[pixlayer];
    }

    Map_Of_Modules_Status.add(pixlayer, waferID, m_pixelid, index);

    // Per FE Status
    //

    // code requires testing w/ different pixel conditions - could cause segfault!
    if (m_doFEPlots) {
      int nFE = getNumberOfFEs(pixlayer, m_pixelid->eta_module(waferID));
      for (int iFE = 0; iFE < nFE; iFE++) {
        Identifier pixelID = m_pixelReadout->getPixelIdfromHash(id_hash, iFE, 1, 1);
        if (pixelID.is_valid()) {
          if (m_pixelCondSummaryTool->isActive(id_hash,
                                               pixelID) == true &&
              m_pixelCondSummaryTool->isGood(id_hash, pixelID) == true) {
            index = 0;  // active and good FE
          } else if (m_pixelCondSummaryTool->isActive(id_hash, pixelID) == false) {
            index = 2;  // inactive or bad FE
          } else {
            index = 1;  // active and bad FE
          }
          Map_Of_FEs_Status.add(pixlayer, waferID, m_pixelid, iFE, index);
        } else {
          ATH_MSG_ERROR(
            "PixelMonitoring: got invalid pixelID " << pixelID << " from id_hash " << id_hash << " with FE#" << iFE <<
              ".");
        }
      }
    }
  }  // end of pixelid wafer loop

  fill2DProfLayerAccum(Map_Of_Modules_Status);
  fill1DProfLumiLayers("BadModulesPerLumi", lb, nBadMod);
  fill1DProfLumiLayers("DisabledModulesPerLumi", lb, nDisabledMod);

  for (unsigned int ii = 0; ii < PixLayers::COUNT; ii++) nBadAndDisabledMod[ii] = nBadMod[ii]+nDisabledMod[ii];
  fill1DProfLumiLayers("BadAndDisabledModulesPerLumi", lb, nBadAndDisabledMod);

  if (m_doFEPlots) fill2DProfLayerAccum(Map_Of_FEs_Status);

  //*******************************************************************************
  //*************************** End of filling Status Histograms ******************
  //*******************************************************************************


  //*******************************************************************************
  //************************** Begin of filling Track Histograms ******************
  //*******************************************************************************


  ATH_MSG_DEBUG("Filling Track Monitoring Histograms");

  VecAccumulator2DMap TSOS_Outlier("TSOSOutlier");
  VecAccumulator2DMap TSOS_Outlier_FE("TSOSOutlierFE");
  VecAccumulator2DMap TSOS_Hole("TSOSHole");
  VecAccumulator2DMap TSOS_Hole_FE("TSOSHoleFE");
  VecAccumulator2DMap TSOS_Measurement("TSOSMeasurement");
  VecAccumulator2DMap TSOS_Measurement_FE("TSOSMeasurementFE");
  VecAccumulator2DMap HolesRatio("HolesRatio");
  VecAccumulator2DMap MissHitsRatio("MissHitsRatio");
  auto trackGroup = getGroup("Track");

  auto tracks = SG::makeHandle(m_tracksKey, ctx);

  if (!(tracks.isValid())) {
    ATH_MSG_ERROR("PixelMonitoring: Track container " << m_tracksKey.key() << " could not be found.");
    auto dataread_err = Monitored::Scalar<int>("trkdataread_err", DataReadErrors::ContainerInvalid);
    fill(trackGroup, dataread_err);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("PixelMonitoring: Track container " << tracks.name() << " is found.");
  }

  int ntracksPerEvent = 0;
  bool havePixelHits(false);
  std::vector<std::pair<Identifier, double> > ClusterIDs;

  auto lbval = Monitored::Scalar<int>("pixclusmontool_lb", lb);

  TrackCollection::const_iterator itrack = tracks->begin();
  TrackCollection::const_iterator itrack_end = tracks->end();
  for (; itrack != itrack_end; ++itrack) {
    if ((*itrack) == nullptr || (*itrack)->perigeeParameters() == nullptr || (*itrack)->trackSummary() == nullptr ||
        (*itrack)->trackSummary()->get(Trk::numberOfPixelHits) == 0) {
      ATH_MSG_DEBUG("PixelMonitoring: Track either invalid or it does not contain pixel hits, continuing...");
      continue;
    }

    int nPixelHits = 0;
    const Trk::Perigee* measPerigee = static_cast<const Trk::Perigee*>((*itrack)->perigeeParameters());
    bool passJOTrkTightCut = static_cast<bool>(m_trackSelTool->accept(**itrack));
    bool pass1hole1GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 1.0));  // misshit ratios
    bool pass1hole5GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 5.0));  // eff vs lumi

    const Trk::Track* trackWithHoles((*itrack));
    std::unique_ptr<const Trk::Track> trackWithHolesUnique = nullptr;
    if ((*itrack)->trackSummary()->get(Trk::numberOfPixelHoles) > 0) {
      trackWithHolesUnique.reset(m_holeSearchTool->getTrackWithHoles(**itrack));
      trackWithHoles = trackWithHolesUnique.get();
    }
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = trackWithHoles->trackStateOnSurfaces();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator = trackStates->begin();
         trackStateOnSurfaceIterator != trackStates->end(); ++trackStateOnSurfaceIterator) {
      const Trk::MeasurementBase* mesBase = (*trackStateOnSurfaceIterator)->measurementOnTrack();

      const Trk::RIO_OnTrack* RIOOnTrack = nullptr;
      if (mesBase && mesBase->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
        RIOOnTrack = static_cast<const Trk::RIO_OnTrack*>(mesBase);
      }

      if (mesBase && !RIOOnTrack) continue;                                                // skip pseudomeasurements
                                                                                           // but not hits, holes,
                                                                                           // outliers

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
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID));
      if (pixlayer == 99) continue;

      float nOutlier = 0.;
      float nHole = 0.;
      auto effval = Monitored::Scalar<float>("HitEffAll_val", 0.);
      auto efflb = Monitored::Scalar<float>("HitEffAll_lb", lb);
      const InDetDD::SiDetectorElement *sde = dynamic_cast<const InDetDD::SiDetectorElement *>(trkParameters->associatedSurface().associatedDetectorElement());
      const InDetDD::SiLocalPosition trkLocalPos = trkParameters->localPosition();
      Identifier locPosID;

      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)) 
	{
	  nOutlier = 1.0;
	  const InDet::SiClusterOnTrack *siclus = dynamic_cast<const InDet::SiClusterOnTrack *>(mesBase);
	  if ( mesBase && siclus) { 
	    locPosID = siclus->identify();
	    if ( !(locPosID.is_valid()) ) {
	      ATH_MSG_INFO("Pixel Monitoring: got invalid track local position on surface for an outlier.");
	      continue;
	    }
	    TSOS_Outlier.add(pixlayer, locPosID, m_pixelid, 1.0);
	    if (!m_doOnline) {
	      TSOS_Outlier_FE.add(pixlayer, locPosID, m_pixelid, m_pixelReadout->getFE(locPosID, locPosID), 1.0);
	    }
	  }
	} 
      else if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)) 
	{
	  nHole = 1.0;
	  locPosID = sde->identifierOfPosition(trkLocalPos);
	  if ( !(locPosID.is_valid()) ) {
	    ATH_MSG_INFO("Pixel Monitoring: got invalid track local position on surface for a hole.");
	    continue;
	  }
	  TSOS_Hole.add(pixlayer, locPosID, m_pixelid, 1.0);
	  if (!m_doOnline) {
	    TSOS_Hole_FE.add(pixlayer, locPosID, m_pixelid, m_pixelReadout->getFE(locPosID, locPosID), 1.0);
	  }
	} 
      else if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)) 
	{
	  if (not mesBase) continue;
	  const InDetDD::SiDetectorElement* side =
	    dynamic_cast<const InDetDD::SiDetectorElement*>(mesBase->associatedSurface().associatedDetectorElement());
	  const InDet::SiClusterOnTrack* clus = dynamic_cast<const InDet::SiClusterOnTrack*>(mesBase);
	  if (!side || !clus) continue;
	  const InDet::SiCluster* RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
	  if (!RawDataClus || !RawDataClus->detectorElement()->isPixel()) continue;

	  nPixelHits++;

	  locPosID = clus->identify();
	  if ( !(locPosID.is_valid()) ) {
	    ATH_MSG_INFO("Pixel Monitoring: got invalid cluster on track ID.");
	    continue;
	  }
	  TSOS_Measurement.add(pixlayer, locPosID, m_pixelid, 1.0);
	  if (!m_doOnline) {
	    TSOS_Measurement_FE.add(pixlayer, locPosID, m_pixelid, m_pixelReadout->getFE(locPosID, locPosID), 1.0);
	  }
	  effval = 1.;

	  const Trk::AtaPlane* trackAtPlane = dynamic_cast<const Trk::AtaPlane*>(trkParameters);
	  if (trackAtPlane) {
	    const Amg::Vector2D localpos = trackAtPlane->localPosition();

	    // Get local error matrix for hit and track and calc pull
	    const AmgSymMatrix(5) trackErrMat = (*trackAtPlane->covariance());
	    const Amg::MatrixX clusErrMat = clus->localCovariance();
	    
	    double error_sum =
	      sqrt(pow(Amg::error(trackErrMat, Trk::locX), 2) + pow(Amg::error(clusErrMat, Trk::locX), 2));
	    auto resPhi = Monitored::Scalar<float>("res_phi", clus->localParameters()[Trk::locX] - localpos[0]);
	    fill(trackGroup, resPhi);
	    if (error_sum != 0) {
	      auto pullPhi = Monitored::Scalar<float>("pull_phi", resPhi / error_sum);
	      fill(trackGroup, pullPhi);
	    }
	    
	    error_sum = sqrt(pow(Amg::error(trackErrMat, Trk::locY), 2) + pow(Amg::error(clusErrMat, Trk::locY), 2));
	    auto resEta = Monitored::Scalar<float>("res_eta", clus->localParameters()[Trk::locY] - localpos[1]);
	    fill(trackGroup, resEta);
	    if (error_sum != 0) {
	      auto pullEta = Monitored::Scalar<float>("pull_eta", resEta / error_sum);
	      fill(trackGroup, pullEta);
	    }
	    // Filling containers, which hold id's of hits and clusters on track
	    // _and_ incident angle information for later normalization

	    Amg::Vector3D mynormal = side->normal();
	    Amg::Vector3D mytrack = trackAtPlane->momentum();
	    double trknormcomp = mytrack.dot(mynormal);
	    
	    double mytrack_mag = mytrack.mag();
	    double cosalpha = 0.;
	    if (mytrack_mag != 0) cosalpha = std::abs(trknormcomp / mytrack_mag);
	    ClusterIDs.emplace_back(clus->identify(), cosalpha);
	  }
	} // end of measurement case
      else continue;

      if (pass1hole5GeVptTightCut) {
	if (pixlayer == PixLayers::kIBL) {
	  int iblsublayer = (m_pixelid->eta_module(surfaceID) > -7 && m_pixelid->eta_module(surfaceID) < 6) ? PixLayers::kIBL2D : PixLayers::kIBL3D;
	  fill(pixLayersLabel[iblsublayer], efflb, effval);
	} else fill(pixLayersLabel[pixlayer], efflb, effval);
      }
      
      if (pass1hole1GeVptTightCut && locPosID.is_valid()) {
        HolesRatio.add(pixlayer, locPosID, m_pixelid, nHole);
        MissHitsRatio.add(pixlayer, locPosID, m_pixelid, nOutlier + nHole);
      }
    } // end of TSOS loop

    ntracksPerEvent++;
    auto nph = Monitored::Scalar<int>("npixhits_per_track", nPixelHits);
    auto nphwgt = Monitored::Scalar<float>("npixhits_per_track_wgt", 1.0);
    fill(trackGroup, lbval, nph, nphwgt);

    int trkfitndf = (*itrack)->fitQuality()->numberDoF();
    double trkfitchi2 = (*itrack)->fitQuality()->chiSquared();
    if (trkfitndf != 0) {
      auto trkChiN = Monitored::Scalar<float>("fit_chi2byndf", trkfitchi2 / trkfitndf);
      fill(trackGroup, trkChiN);
    }
    havePixelHits = havePixelHits || (nPixelHits > 0);
  } // end of track loop

  if (!havePixelHits) {
    auto dataread_err = Monitored::Scalar<int>("trkdataread_err", DataReadErrors::EmptyContainer);
    fill(trackGroup, dataread_err);
  }

  fill2DProfLayerAccum(HolesRatio);
  fill2DProfLayerAccum(MissHitsRatio);
  fill2DProfLayerAccum(TSOS_Outlier);
  fill2DProfLayerAccum(TSOS_Hole);
  fill2DProfLayerAccum(TSOS_Measurement);
  if (!m_doOnline) { 
    fill2DProfLayerAccum(TSOS_Outlier_FE);
    fill2DProfLayerAccum(TSOS_Hole_FE);
    fill2DProfLayerAccum(TSOS_Measurement_FE);
  }

  sort(ClusterIDs.begin(), ClusterIDs.end(),
       [](const std::pair<Identifier, double>& left, const std::pair<Identifier, double>& right) {
    return left.first < right.first;
  });


  // Filling per-event histograms
  //
  auto nTrks = Monitored::Scalar<int>("ntrks_per_event", ntracksPerEvent);
  fill(trackGroup, lbval, nTrks);

  //m_npixhits_per_track_lastXlb-> // m_npixhits_per_track_lumi TH2F vs lumi
  //*******************************************************************************
  //**************************** End of filling Track Histograms ******************
  //*******************************************************************************

  //*******************************************************************************
  //************************ Begin of filling Cluster Histograms ******************
  //*******************************************************************************

  ATH_MSG_DEBUG("Filling Cluster Monitoring Histograms");


  auto clToTcosAlphaLB = Monitored::Scalar<float>("ClusterToTxCosAlphaOnTrack_lb", lb);

  VecAccumulator2DMap Cluster_LVL1A_Mod("ClusterLVL1AMod");
  VecAccumulator2DMap Cluster_LVL1A_SizeCut("ClusterLVL1ASizeCut");
  VecAccumulator2DMap Cluster_LVL1A_Mod_OnTrack("ClusterLVL1AModOnTrack");
  VecAccumulator2DMap Cluster_LVL1A_SizeCut_OnTrack("ClusterLVL1ASizeCutOnTrack");
  VecAccumulator2DMap ClusterMap_Mon("ClusterMapMon");
  VecAccumulator2DMap ClusterMap_Mon_OnTrack("ClusterMapMonOnTrack");
  VecAccumulator2DMap Cluster_Size_Map_OnTrack("ClusterSizeMapOnTrack");
  VecAccumulator2DMap Cluster_Occupancy("ClusterOccupancy");
  VecAccumulator2DMap Cluster_Occupancy_OnTrack("ClusterOccupancyOnTrack");
  VecAccumulator2DMap Clus_Occ_SizeCut("ClusOccSizeCut");
  VecAccumulator2DMap Clus_Occ_SizeCut_OnTrack("ClusOccSizeCutOnTrack");
  VecAccumulator2DMap Cluster_FE_Occupancy("ClusterFEOccupancy");
  VecAccumulator2DMap Cluster_FE_Occupancy_OnTrack("ClusterFEOccupancyOnTrack");

  auto clusterGroup = getGroup("Cluster");
  auto clusterGroup_OnTrack = getGroup("Cluster_OnTrack");

  auto pixel_clcontainer = SG::makeHandle(m_clustersKey, ctx);

  if (!(pixel_clcontainer.isValid())) {
    ATH_MSG_ERROR("Pixel Monitoring: Pixel Cluster container " << m_clustersKey.key() << " could not be found.");
    auto dataread_err = Monitored::Scalar<int>("clsdataread_err", DataReadErrors::ContainerInvalid);
    fill(clusterGroup, dataread_err);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel Monitoring: Pixel Cluster container " << pixel_clcontainer.name() << " is found.");
  }

  int nclusters = 0;
  int nclusters_ontrack = 0;
  float nclusters_mod[PixLayers::COUNT] = {
    0.
  };
  float nclusters_ontrack_mod[PixLayers::COUNT] = {
    0.
  };

  Identifier clusID;
  for (auto colNext: *pixel_clcontainer) {
    const InDet::PixelClusterCollection* ClusterCollection(colNext);
    if (!ClusterCollection) {
      ATH_MSG_DEBUG("Pixel Monitoring: Pixel Cluster container is empty.");
      auto dataread_err = Monitored::Scalar<int>("clsdataread_err", DataReadErrors::CollectionInvalid);
      fill(clusterGroup, dataread_err);
      continue;
    }

    for (auto p_clus: *ClusterCollection) {
      clusID = p_clus->identify();
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID));
      if (pixlayer == 99) continue;
      getPhiEtaMod(m_pixelid, clusID, phiMod, etaMod, copyFEval);

      const InDet::PixelCluster& cluster = *p_clus;
      nclusters++;

      // begin timing histos
      //
      auto clLVL1A = Monitored::Scalar<float>("Cluster_LVL1A_lvl1a", cluster.LVL1A());
      fill(clusterGroup, clLVL1A);
      Cluster_LVL1A_Mod.add(pixlayer, clusID, m_pixelid, cluster.LVL1A() + 0.00001);
      if (cluster.rdoList().size() > 1) Cluster_LVL1A_SizeCut.add(pixlayer, clusID, m_pixelid, cluster.LVL1A() + 0.00001);
      if (pixlayer == PixLayers::kIBL) {
	int iblsublayer = (m_pixelid->eta_module(clusID) > -7 && m_pixelid->eta_module(clusID) < 6) ? PixLayers::kIBL2D : PixLayers::kIBL3D;
	if (cluster.totalToT() > clusterToTMinCut[iblsublayer]) fill("ClusterLVL1AToTCut_" + pixLayersLabel[iblsublayer], clLVL1A);
	nclusters_mod[iblsublayer]++;
      } else {
	if (cluster.totalToT() > clusterToTMinCut[pixlayer]) fill("ClusterLVL1AToTCut_" + pixLayersLabel[pixlayer], clLVL1A);
	nclusters_mod[pixlayer]++;
      }
      //
      // end timing histos
      // begin cluster rate
      //
      if (m_doOnline) ClusterMap_Mon.add(pixlayer, clusID, m_pixelid);
      //
      // end cluster rate
      // begin cluster occupancy
      //
      Cluster_Occupancy.add(pixlayer, clusID, m_pixelid);
      if (m_doFEPlots) {
        Cluster_FE_Occupancy.add(pixlayer, clusID, m_pixelid, m_pixelReadout->getFE(clusID, clusID), 1.0);
      }
      if (cluster.rdoList().size() > 1) Clus_Occ_SizeCut.add(pixlayer, clusID, m_pixelid);
      // end cluster occupancy

      double cosalpha(0.);
      if (isClusterOnTrack(clusID, ClusterIDs, cosalpha)) {
        nclusters_ontrack++;
        switch (pixlayer) {
        case PixLayers::kECA:
          clusPerEventArray.DA[phiMod][etaMod]++;
          break;

        case PixLayers::kECC:
          clusPerEventArray.DC[phiMod][etaMod]++;
          break;

        case PixLayers::kB0:
          clusPerEventArray.B0[phiMod][etaMod]++;
          break;

        case PixLayers::kB1:
          clusPerEventArray.B1[phiMod][etaMod]++;
          break;

        case PixLayers::kB2:
          clusPerEventArray.B2[phiMod][etaMod]++;
          break;

        case PixLayers::kIBL:
          clusPerEventArray.IBL[phiMod][etaMod]++;
          break;
        }
        // begin timing histos
        //
        clLVL1A = cluster.LVL1A();
        fill(clusterGroup_OnTrack, clLVL1A);
        Cluster_LVL1A_Mod_OnTrack.add(pixlayer, clusID, m_pixelid, cluster.LVL1A() + 0.00001);
        if (cluster.rdoList().size() > 1) Cluster_LVL1A_SizeCut_OnTrack.add(pixlayer, clusID, m_pixelid,
            cluster.LVL1A() + 0.00001);
        //
        // end timing histos
        // begin cluster sizes
        //
        auto clSize = Monitored::Scalar<float>("ClusterSizeOnTrack_clsize", cluster.rdoList().size());
        auto clSizeEtaModule = Monitored::Scalar<float>("ClusterSizeOnTrack_em", m_pixelid->eta_module(clusID));
        if (abs(m_pixelid->barrel_ec(clusID)) != 0) clSizeEtaModule = m_pixelid->layer_disk(clusID) + 1;
        fill("ClusterGroupsizeVsEtaOnTrack_" + pixBaseLayersLabel[pixlayer], clSizeEtaModule, clSize);

        Cluster_Size_Map_OnTrack.add(pixlayer, clusID, m_pixelid, cluster.rdoList().size());
        //
        // end cluster sizes
        // begin cluster rate
        //
        if (m_doOnline) ClusterMap_Mon_OnTrack.add(pixlayer, clusID, m_pixelid);
        //
        // end cluster rate
        // begin cluster occupancy
        //
        Cluster_Occupancy_OnTrack.add(pixlayer, clusID, m_pixelid);
        if (m_doFEPlots) {
          Cluster_FE_Occupancy_OnTrack.add(pixlayer, clusID, m_pixelid, m_pixelReadout->getFE(clusID, clusID), 1.0);
        }
        if (cluster.rdoList().size() > 1) Clus_Occ_SizeCut_OnTrack.add(pixlayer, clusID, m_pixelid);
        //
        // end cluster occupancy
        // begin cluster ToT, 1D timing, charge
        //
	if (pixlayer == PixLayers::kIBL)
	  {
	    pixlayer = (m_pixelid->eta_module(clusID) > -7 && m_pixelid->eta_module(clusID) < 6) ? PixLayers::kIBL2D : PixLayers::kIBL3D;
	  }
	if (cluster.totalToT() > clusterToTMinCut[pixlayer]) fill("ClusterLVL1AToTCutOnTrack_" + pixLayersLabel[pixlayer], clLVL1A);

	auto clToTcosAlpha = Monitored::Scalar<float>("ClusterToTxCosAlphaOnTrack_val", cluster.totalToT() * cosalpha);
        fill(pixLayersLabel[pixlayer], clToTcosAlphaLB, clToTcosAlpha);

        if (!m_doOnline) {
          auto clQcosAlpha = Monitored::Scalar<float>("ClusterQxCosAlphaOnTrack_val", cluster.totalCharge() * cosalpha);
          fill(pixLayersLabel[pixlayer], clQcosAlpha);
        }
        nclusters_ontrack_mod[pixlayer]++;
        //
        // end cluster ToT, 1D timing, charge
      } // end on track
    }   // end cluster collection 
  }
  fill2DProfLayerAccum(Cluster_LVL1A_Mod);
  fill2DProfLayerAccum(Cluster_LVL1A_SizeCut);
  fill2DProfLayerAccum(Cluster_LVL1A_Mod_OnTrack);
  fill2DProfLayerAccum(Cluster_LVL1A_SizeCut_OnTrack);
  if (m_doOnline) {
    fill2DProfLayerAccum(ClusterMap_Mon);
    fill2DProfLayerAccum(ClusterMap_Mon_OnTrack);
  }
  fill2DProfLayerAccum(Cluster_Size_Map_OnTrack);
  fill2DProfLayerAccum(Cluster_Occupancy);
  fill2DProfLayerAccum(Cluster_Occupancy_OnTrack);
  fill2DProfLayerAccum(Clus_Occ_SizeCut);
  fill2DProfLayerAccum(Clus_Occ_SizeCut_OnTrack);
  if (m_doFEPlots) {
    fill2DProfLayerAccum(Cluster_FE_Occupancy);
    fill2DProfLayerAccum(Cluster_FE_Occupancy_OnTrack);
  }
  // begin cluster rates
  //
  auto nCls = Monitored::Scalar<int>("ClustersPerEvent_val", nclusters);
  fill(clusterGroup, lbval, nCls);
  auto nClsOnTrk = Monitored::Scalar<int>("ClustersPerEventOnTrack_val", nclusters_ontrack);
  fill(clusterGroup_OnTrack, lbval, nClsOnTrk);
  for (unsigned int ii = 0; ii < PixLayers::COUNT; ii++) {
    auto vals = Monitored::Scalar<float>("ClustersPerEvent_val", nclusters_mod[ii]);
    auto vals_ontrack = Monitored::Scalar<float>("ClustersPerEventOnTrack_val", nclusters_ontrack_mod[ii]);
    fill(pixLayersLabel[ii], vals, vals_ontrack);
  }

  if (nclusters > 0) {
    auto clsFracOnTrack = Monitored::Scalar<float>("cls_frac_ontrack", (float) nclusters_ontrack / nclusters);
    fill(clusterGroup_OnTrack, lbval, clsFracOnTrack);
  } else {
    auto dataread_err = Monitored::Scalar<int>("clsdataread_err", DataReadErrors::EmptyContainer);
    fill(clusterGroup, dataread_err);
  }

  fill1DProfLumiLayers("ClustersPerLumi", lb, nclusters_mod);
  fill1DProfLumiLayers("ClustersPerLumiOnTrack", lb, nclusters_ontrack_mod);

  fillFromArrays("ClusterOccupancyPP0OnTrack", clusPerEventArray);

  if (ntracksPerEvent > 0) {
    for (unsigned int ii = 0; ii < PixLayers::COUNT; ii++) nclusters_ontrack_mod[ii] /= ntracksPerEvent; // keep as in
                                                                                                         // Run 1,2
    fill1DProfLumiLayers("NumClustersPerTrackPerLumi", lb, nclusters_ontrack_mod);
  }
  //
  // end cluster rate
  ClusterIDs.clear();

  //*******************************************************************************
  //************************** End of filling Cluster Histograms ******************
  //*******************************************************************************

  return StatusCode::SUCCESS;
}
