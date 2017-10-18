/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for tracks (combined ID and pixel only tracks)
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <sstream>
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StatusCode PixelMainMon::bookTrackMon(void) {
  ATH_MSG_DEBUG("Start booking Track histogtams..");

  std::string path = "Pixel/Track";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/TrackOnTrack");
  MonGroup trackHistos(this, path.c_str(), run, ATTRIB_MANAGED);  //declare a group of track histograms

  std::string modlabel[9];
  modlabel[0] = "ECA";
  modlabel[1] = "ECC";
  modlabel[2] = "B0";
  modlabel[3] = "B1";
  modlabel[4] = "B2";
  modlabel[5] = "IBL";
  modlabel[6] = "IBL2D";
  modlabel[7] = "IBL3D";
  std::string hname;
  std::string htitles;

  StatusCode sc;

  int nbins_LB = m_lbRange;
  double min_LB = -0.5;
  double max_LB = min_LB + (1.0 * nbins_LB);

  sc = trackHistos.regHist(m_track_res_phi = TH1F_LW::create("Track_res_phi", ("Pixel Residual LocX(phi)" + m_histTitleExt).c_str(), 100, -0.1, 0.1));
  sc = trackHistos.regHist(m_track_res_eta = TH1F_LW::create("Track_res_eta", ("Pixel Residual LocY(eta)" + m_histTitleExt).c_str(), 100, -0.3, 0.3));
  sc = trackHistos.regHist(m_track_pull_phi = TH1F_LW::create("Track_pull_phi", ("Pixel pull LocX(phi)" + m_histTitleExt).c_str(), 100, -1.2, 1.2));
  sc = trackHistos.regHist(m_track_pull_eta = TH1F_LW::create("Track_pull_eta", ("Pixel pull LocY(eta)" + m_histTitleExt).c_str(), 100, -2.0, 2.0));
  sc = trackHistos.regHist(m_track_chi2 = TH1F_LW::create("Track_chi2", ("chi2 of rec. track" + m_histTitleExt + ";#chi^{2}/DoF;#tracks").c_str(), 50, -0., 10.));
  sc = trackHistos.regHist(m_tracksPerEvt_per_lumi = TProfile_LW::create("tracksPerEvt_per_lumi", ("Number of tracks per event per LB" + m_histTitleExt + ";lumi block;tracks/event").c_str(), nbins_LB, min_LB, max_LB));

  hname = makeHistname("LorentzAngle_IBL", false);
  htitles = makeHisttitle("Lorentz angle IBL", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_IBL = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
  hname = makeHistname("LorentzAngle_IBL2D", false);
  htitles = makeHisttitle("Lorentz angle IBL2D", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_IBL2D = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
  hname = makeHistname("LorentzAngle_IBL3D", false);
  htitles = makeHisttitle("Lorentz angle IBL3D", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_IBL3D = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
  hname = makeHistname("LorentzAngle_B0", false);
  htitles = makeHisttitle("Lorentz angle B0", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_B0 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 22, -0.5, 21.5));
  hname = makeHistname("LorentzAngle_B1", false);
  htitles = makeHisttitle("Lorentz angle B1", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_B1 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 38, -0.5, 37.5));
  hname = makeHistname("LorentzAngle_B2", false);
  htitles = makeHisttitle("Lorentz angle B2", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
  sc = trackHistos.regHist(m_LorentzAngle_B2 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 52, -0.5, 51.5));

  if (m_LorentzAngle_IBL) m_LorentzAngle_IBL->SetOption("colz");
  if (m_LorentzAngle_IBL2D) m_LorentzAngle_IBL2D->SetOption("colz");
  if (m_LorentzAngle_IBL3D) m_LorentzAngle_IBL3D->SetOption("colz");
  if (m_LorentzAngle_B0) m_LorentzAngle_B0->SetOption("colz");
  if (m_LorentzAngle_B1) m_LorentzAngle_B1->SetOption("colz");
  if (m_LorentzAngle_B2) m_LorentzAngle_B2->SetOption("colz");

  if (m_do2DMaps && !m_doOnline) {
    m_tsos_hitmap = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("TSOS_Measurement", ("TSOS of type Measurement" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_tsos_hitmap->regHist(trackHistos);
    m_tsos_holemap = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("TSOS_Hole", ("TSOS of type Hole" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_tsos_holemap->regHist(trackHistos);
    m_tsos_outliermap = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("TSOS_Outlier", ("TSOS of type Outlier" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_tsos_outliermap->regHist(trackHistos);
  }
  if (m_doOnline) {
    m_tsos_holeratio_tmp = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("HoleRatio_tmp", ("TSOS of type Hole per track tmp" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true));
    sc = m_tsos_holeratio_tmp->regHist(trackHistos);
    m_tsos_holeratio_mon = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("HoleRatio_mon", ("TSOS of type Hole per track for monitoring" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true));
    sc = m_tsos_holeratio_mon->regHist(trackHistos);
    m_misshits_ratio_tmp = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("MissHitsRatioOnTrack_tmp", ("Hole+Outlier per track" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true));
    sc = m_misshits_ratio_tmp->regHist(trackHistos);
    m_misshits_ratio_mon = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("MissHitsRatioOnTrack_mon", ("Hole+Outlier per track for monitoring" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true));
    sc = m_misshits_ratio_mon->regHist(trackHistos);
  }

  if (m_doModules && !m_doOnTrack) {  // tbc essentially code weren't used since doModules was enabled only for AllHits; to keep it disabled also now !m_doOnTrack switch was added
    m_tsos_hiteff_vs_lumi = std::make_unique<PixelMonModulesProf>(PixelMonModulesProf("TSOS_HitEfficiency", ("TSOS-based hit efficiency in module" + m_histTitleExt).c_str(), 2500, -0.5, 2499.5));
    sc = m_tsos_hiteff_vs_lumi->regHist(this, (path + "/Modules_TSOSHitEff").c_str(), run);
    if (!m_doOnline) {
      sc = trackHistos.regHist(m_clustot_lowpt = TH1F_LW::create("m_clustot_lowpt", ("Cluster ToT vs Track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_clustot_highpt = TH1F_LW::create("m_clustot_highpt", ("Cluster ToT vs Track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_1hitclustot_lowpt = TH1F_LW::create("m_1hitclustot_lowpt", ("1-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_1hitclustot_highpt = TH1F_LW::create("m_1hitclustot_highpt", ("1-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_2hitclustot_lowpt = TH1F_LW::create("m_2hitclustot_lowpt", ("2-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_2hitclustot_highpt = TH1F_LW::create("m_2hitclustot_highpt", ("2-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + " Track pT; Cluster ToT (on track)").c_str(), 250, 0, 250));
      sc = trackHistos.regHist(m_clustot_vs_pt = TH2F_LW::create("m_clustot_vs_pt", ("Cluster ToT vs Track pT" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(), 10, 0, 50, 250, 0, 250));
      sc = trackHistos.regHist(m_track_chi2_bcl1 = TH1F_LW::create("m_Pixel_track_chi2_bcl1", ("track chi2 with 1 1-hit, low-ToT cluster" + m_histTitleExt + "; #chi^{ 2}/DoF;").c_str(), 50, -0., 10.));
      sc = trackHistos.regHist(m_track_chi2_bcl0 = TH1F_LW::create("m_Pixel_track_chi2_bcl0", ("track chi2 with 0 1-hit, low-ToT clusters" + m_histTitleExt + "; #chi^ {2}/DoF;").c_str(), 50, -0., 10.));
      sc = trackHistos.regHist(m_track_chi2_bclgt1 = TH1F_LW::create("m_Pixel_track_chi2_bclgt1", ("track chi2 with >1 1-hit, low-ToT cluster" + m_histTitleExt + "; # chi^{2}/DoF;").c_str(), 50, -0., 10.));
      sc = trackHistos.regHist(m_track_chi2_bcl1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl1_highpt", ("track chi2 with 1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50, -0., 10.));
      sc = trackHistos.regHist(m_track_chi2_bcl0_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl0_highpt", ("track chi2 with 0 1-hit, low-ToT clusters (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50, -0., 10.));
      sc = trackHistos.regHist(m_track_chi2_bclgt1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bclgt1_highpt", ("track chi2 with >1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50, -0., 10.));
    }
  }

  for (int i = 0; i < PixLayerDisk::COUNT; i++) {
    hname = makeHistname(("HitEff_all_" + m_modLabel_PixLayerDisk[i]), false);
    htitles = makeHisttitle(("hit efficiency, " + m_modLabel_PixLayerDisk[i]), ";lumi block;hit efficiency", false);
    sc = trackHistos.regHist(m_hiteff_incl_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));
  }

  if (sc.isFailure()) ATH_MSG_WARNING("Problems with booking Track histograms");
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillTrackMon(void) {
  ATH_MSG_DEBUG("Filling Track Monitoring Histograms");

  StatusCode sc;
  sc = evtStore()->retrieve(m_tracks, m_TracksName);
  if (sc.isFailure()) {
    ATH_MSG_INFO("No tracks in StoreGate found");
    if (m_storegate_errors) m_storegate_errors->Fill(4., 3.);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Tracks in StoreGate found");
  }

  m_ntracksPerEvent = 0;
  int nPixelHits = 0;

  if (m_doOnTrack) {
    m_RDOIDs.clear();
    m_ClusterIDs.clear();
  }

  for (int i = 0; i < (int)m_tracks->size(); i++) {
    const Trk::Track *track0 = (*m_tracks)[i];
    if (track0 == 0) {
      ATH_MSG_ERROR("no valid pointer to track!!!");
      break;
    }

    int nholes = -1;
    int nbadclus = 0;
    int ngoodclus = 0;
    bool passQualityCut = false;
    bool passTightCut = false;
    bool pass1hole2GeVTightCut = false;

    const Trk::TrackParameters *trkParameters = 0;
    const Trk::TrackSummary *summary = track0->trackSummary();
    const Trk::Perigee *measPerigee = dynamic_cast<const Trk::Perigee *>(track0->perigeeParameters());

    if (summary) {
      if (summary->get(Trk::numberOfPixelHits) == 0) continue;
    } else {
      ATH_MSG_INFO("No Track Summary Found");
      continue;
    }

    const Trk::Track *track = track0;

    nholes = summary->get(Trk::numberOfPixelHoles);
    if (m_doHoleSearch && !m_doOnline && nholes > 0) {
      track = m_holeSearchTool->getTrackWithHoles(*track0);
    }

    if (measPerigee->pT() / 1000.0 > 5.0 && fabs(measPerigee->eta()) < 2.5) passQualityCut = true;

    if (((fabs(measPerigee->eta()) <= 1.65 && summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits) >= 9) ||
         (fabs(measPerigee->eta()) > 1.65 && summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits) >= 11)) &&
        (summary->get(Trk::numberOfNextToInnermostPixelLayerHits) + summary->get(Trk::numberOfInnermostPixelLayerHits) > 0) &&
        (summary->get(Trk::numberOfPixelHoles) == 0) &&
        (fabs(measPerigee->parameters()[Trk::d0]) < 2.0) &&
        (fabs(measPerigee->parameters()[Trk::z0]) < 150.0)) {
      passTightCut = true;
    }

    if (measPerigee->pT() / 1000.0 > 5.0 &&
        ((fabs(measPerigee->eta()) <= 1.65 && summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits) >= 9) ||
         (fabs(measPerigee->eta()) > 1.65 && summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits) >= 11)) &&
        (summary->get(Trk::numberOfNextToInnermostPixelLayerHits) + summary->get(Trk::numberOfInnermostPixelLayerHits) > 0) &&
        (summary->get(Trk::numberOfPixelHoles) < 2) &&
        (fabs(measPerigee->parameters()[Trk::d0]) < 2.0) &&
        (fabs(measPerigee->parameters()[Trk::z0]) < 150.0)) {
      pass1hole2GeVTightCut = true;
    }

    const DataVector<const Trk::TrackStateOnSurface> *trackStates = track->trackStateOnSurfaces();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator = trackStates->begin(); trackStateOnSurfaceIterator != trackStates->end(); trackStateOnSurfaceIterator++) {
      // Change the track state on 1 surface into the cluster it represents
      Identifier clusID;
      Identifier surfaceID;
      IdentifierHash id_hash;
      const InDet::SiClusterOnTrack *clus = 0;
      const InDetDD::SiDetectorElement *side = 0;
      const Trk::MeasurementBase *mesb = (*trackStateOnSurfaceIterator)->measurementOnTrack();

      const Trk::RIO_OnTrack *hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack *>(mesb) : 0;

      float nOutlier = 0.;
      float nHole = 0.;
      float npixHitsInCluster = 0;
      float rowWidthOfCluster = 0;
      float totalToTOfCluster = 0;
      bool passClusterSelection = false;

      if (mesb && !hit) continue;  // skip pseudomeasurements etc.
      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
        surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
        side = dynamic_cast<const InDetDD::SiDetectorElement *>(mesb->associatedSurface().associatedDetectorElement());
      } else {  // holes, perigee
        if (not(*trackStateOnSurfaceIterator)->trackParameters()) {
          ATH_MSG_INFO("pointer of TSOS to track parameters or associated surface is null");
          continue;
        }
        surfaceID = (*trackStateOnSurfaceIterator)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();  //check ptr
      }

      if (!m_idHelper->is_pixel(surfaceID)) continue;

      int pixlayer = getPixLayerID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
      int pixlayerdisk = getPixLayerDiskID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
      int pixlayeribl2d3d = pixlayer;
      if (pixlayeribl2d3d == PixLayer::kIBL) {
        pixlayeribl2d3d = getPixLayerIDIBL2D3D(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_pixelid->eta_module(surfaceID), m_doIBL);
      }
      if (pixlayer == 99) continue;

      id_hash = m_pixelid->wafer_hash(surfaceID);

      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)) {
        clus = dynamic_cast<const InDet::SiClusterOnTrack *>(mesb);
        if (clus) clusID = clus->identify();

        if (m_tsos_hitmap) m_tsos_hitmap->fill(surfaceID, m_pixelid);
        if (m_tsos_hiteff_vs_lumi) m_tsos_hiteff_vs_lumi->fill(m_manager->lumiBlockNumber(), 1., surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayerdisk] && pass1hole2GeVTightCut) m_hiteff_incl_mod[pixlayerdisk]->Fill(m_manager->lumiBlockNumber(), 1.0);
      }

      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)) {
        clus = dynamic_cast<const InDet::SiClusterOnTrack *>((*trackStateOnSurfaceIterator)->measurementOnTrack());
        if (clus) clusID = clus->identify();
        nOutlier = 1.0;

        if (m_tsos_holemap) m_tsos_holemap->fill(surfaceID, m_pixelid);
        if (m_tsos_hiteff_vs_lumi) m_tsos_hiteff_vs_lumi->fill(m_manager->lumiBlockNumber(), 0., surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayerdisk] && pass1hole2GeVTightCut) m_hiteff_incl_mod[pixlayerdisk]->Fill(m_manager->lumiBlockNumber(), 0.0);
      }

      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)) {
        clus = dynamic_cast<const InDet::SiClusterOnTrack *>((*trackStateOnSurfaceIterator)->measurementOnTrack());
        if (clus) clusID = clus->identify();
        nHole = 1.0;

        if (m_tsos_outliermap) m_tsos_outliermap->fill(surfaceID, m_pixelid);
        if (m_tsos_hiteff_vs_lumi) m_tsos_hiteff_vs_lumi->fill(m_manager->lumiBlockNumber(), 0., surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayerdisk] && pass1hole2GeVTightCut) m_hiteff_incl_mod[pixlayerdisk]->Fill(m_manager->lumiBlockNumber(), 0.0);
      }

      if (m_doOnline && m_tsos_holeratio_tmp && passQualityCut) m_tsos_holeratio_tmp->fill(surfaceID, m_pixelid, nHole);
      if (passQualityCut) {
        if (nOutlier + nHole > 0.) {
          if (m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->fill(surfaceID, m_pixelid, 1.0);
        } else {
          if (m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->fill(surfaceID, m_pixelid, 0.0);
        }
      }

      if (!(*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)) continue;
      if (!clus) continue;
      const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster *>(clus->prepRawData());
      if (!RawDataClus) continue;
      if (!RawDataClus->detectorElement()->isPixel()) continue;

      nPixelHits++;  //add another pixel hit
      for (unsigned int loopSize = 0; loopSize < RawDataClus->rdoList().size(); loopSize++) {
        if (m_doOnTrack) m_RDOIDs.push_back(RawDataClus->rdoList().at(loopSize));
      }
      if (m_doOnTrack) m_ClusterIDs.push_back(clus->identify());

      const InDet::PixelCluster *pixelCluster = dynamic_cast<const InDet::PixelCluster *>(RawDataClus);
      if (pixelCluster) {
        if (!RawDataClus->gangedPixel() &&  // not include ganged-pixel
            !pixelCluster->isFake() &&      // not fake
            ((pixlayer == PixLayer::kIBL && fabs(clus->localParameters()[Trk::locX]) < 8.3) || (pixlayer != PixLayer::kIBL && fabs(clus->localParameters()[Trk::locX]) < 8.1)) &&
            ((pixlayeribl2d3d == PixLayerIBL2D3D::kIBL2D && fabs(clus->localParameters()[Trk::locY]) < 19.7) || (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL3D && fabs(clus->localParameters()[Trk::locY]) < 9.5) || (pixlayer != PixLayer::kIBL && fabs(clus->localParameters()[Trk::locY]) < 28.7))) {
          passClusterSelection = true;
        }

        npixHitsInCluster = pixelCluster->rdoList().size();
        rowWidthOfCluster = pixelCluster->width().colRow().x();
        totalToTOfCluster = pixelCluster->totalToT();

        if (npixHitsInCluster == 1 && totalToTOfCluster < 8) {
          nbadclus++;
        } else {
          ngoodclus++;
        }

        if (m_doModules && !m_doOnline) {  // indirectly disabled
          if (measPerigee) {
            float pt = measPerigee->pT() / 1000.0;
            if (m_clustot_vs_pt) m_clustot_vs_pt->Fill(pt, totalToTOfCluster);
            if (pt < 10) {
              if (m_clustot_lowpt) m_clustot_lowpt->Fill(totalToTOfCluster);
              if (npixHitsInCluster == 1) {
                if (m_1hitclustot_lowpt) m_1hitclustot_lowpt->Fill(totalToTOfCluster);
              }
              if (npixHitsInCluster == 2) {
                if (m_2hitclustot_lowpt) m_2hitclustot_lowpt->Fill(totalToTOfCluster);
              }
            } else {
              if (m_clustot_highpt) m_clustot_highpt->Fill(totalToTOfCluster);
              if (npixHitsInCluster == 1) {
                if (m_1hitclustot_highpt) m_1hitclustot_highpt->Fill(totalToTOfCluster);
              }
              if (npixHitsInCluster == 2) {
                if (m_2hitclustot_highpt) m_2hitclustot_highpt->Fill(totalToTOfCluster);
              }
            }
          }
        }
      }

      trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
      const Trk::AtaPlane *trackAtPlane = dynamic_cast<const Trk::AtaPlane *>(trkParameters);
      if (trackAtPlane) {
        const Amg::Vector2D localpos = trackAtPlane->localPosition();

        // Get local error matrix for hit and track and calc pull
        const AmgSymMatrix(5) trackErrMat = (*trackAtPlane->covariance());
        const Amg::MatrixX clusErrMat = clus->localCovariance();

        double error_sum = sqrt(pow(Amg::error(trackErrMat, Trk::locX), 2) + pow(Amg::error(clusErrMat, Trk::locX), 2));
        double res = clus->localParameters()[Trk::locX] - localpos[0];
        if (m_track_res_phi) m_track_res_phi->Fill(res);
        double pull = 0;
        if (error_sum != 0) pull = (res) / error_sum;
        if (m_track_pull_phi) m_track_pull_phi->Fill(pull);

        error_sum = sqrt(pow(Amg::error(trackErrMat, Trk::locY), 2) + pow(Amg::error(clusErrMat, Trk::locY), 2));
        res = clus->localParameters()[Trk::locY] - localpos[1];
        if (m_track_res_eta) m_track_res_eta->Fill(res);
        if (error_sum != 0) pull = (res) / error_sum;
        if (m_track_pull_eta) m_track_pull_eta->Fill(pull);

        Amg::Vector3D mynormal = side->normal();
        Amg::Vector3D myphiax = side->phiAxis();
        Amg::Vector3D mytrack = trackAtPlane->momentum();
        double trkphicomp = mytrack.dot(myphiax);
        double trknormcomp = mytrack.dot(mynormal);
        double phiIncident = atan2(trkphicomp, trknormcomp);
        if (npixHitsInCluster > 0 && passTightCut && passClusterSelection) {
          if (pixlayer == PixLayer::kIBL && m_LorentzAngle_IBL) m_LorentzAngle_IBL->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
          if (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL2D && m_LorentzAngle_IBL2D) m_LorentzAngle_IBL2D->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
          if (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL3D && m_LorentzAngle_IBL3D) m_LorentzAngle_IBL3D->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
          if (pixlayer == PixLayer::kB0 && m_LorentzAngle_B0) m_LorentzAngle_B0->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
          if (pixlayer == PixLayer::kB1 && m_LorentzAngle_B1) m_LorentzAngle_B1->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
          if (pixlayer == PixLayer::kB2 && m_LorentzAngle_B2) m_LorentzAngle_B2->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0 * rowWidthOfCluster);
        }
      }
    }  // end of TSOS loop

    if (m_doModules && !m_doOnline) {  // indirectly disabled
      float pt = measPerigee->pT() / 1000.0;
      if (nbadclus == 1) {
        if (m_track_chi2_bcl1 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl1->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
      }
      if (nbadclus == 0) {
        if (m_track_chi2_bcl0 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl0->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
      }
      if (nbadclus > 1) {
        if (m_track_chi2_bclgt1 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bclgt1->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
      }
      if (pt >= 10) {
        if (nbadclus == 1) {
          if (m_track_chi2_bcl1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl1_highpt->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
        }
        if (nbadclus == 0) {
          if (m_track_chi2_bcl1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl0_highpt->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
        }
        if (nbadclus > 1) {
          if (m_track_chi2_bclgt1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bclgt1_highpt->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
        }
      }
    }

    if (nPixelHits > 0) {
      m_ntracksPerEvent++;
    }

    if (m_doHoleSearch && !m_doOnline && nholes > 0) delete track;
  }  // end of track loop

  if (m_tracksPerEvt_per_lumi) m_tracksPerEvt_per_lumi->Fill(m_manager->lumiBlockNumber(), m_ntracksPerEvent);

  if (m_doOnTrack) {
    sort(m_RDOIDs.begin(), m_RDOIDs.end());
    sort(m_ClusterIDs.begin(), m_ClusterIDs.end());
  }

  if (m_doOnline) {
    if (m_doRefresh5min) {
      if (m_tsos_holeratio_mon && m_tsos_holeratio_tmp) m_tsos_holeratio_mon->fill2DMon(m_tsos_holeratio_tmp.get());
      if (m_misshits_ratio_mon && m_misshits_ratio_tmp) m_misshits_ratio_mon->fill2DMon(m_misshits_ratio_tmp.get());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procTrackMon(void) {
  double lengthLB = 0;
  lengthLB = m_LBendTime - m_LBstartTime;
  if (lengthLB <= 0) {
    ATH_MSG_INFO("Luminosity block has length <= 0 sec, cannot calculate track rate.");
    return StatusCode::SUCCESS;
  }

  for (int i = 0; i < PixLayerDisk::COUNT; i++) {
    if (m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMinimum(0.8);
    if (m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMaximum(1.01);
  }
  return StatusCode::SUCCESS;
}
