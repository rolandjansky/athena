/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for tracks (combined ID and pixel only tracks)
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <sstream>
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
#include "TProfile.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
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

  if (m_do2DMaps && !m_doOnline) {
    m_tsos_hitmap = std::make_unique<PixelMon2DMapsLW>("TSOS_Measurement", ("TSOS of type Measurement" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true);
    sc = m_tsos_hitmap->regHist(trackHistos);
    m_tsos_holemap = std::make_unique<PixelMon2DMapsLW>("TSOS_Hole", ("TSOS of type Hole" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true);
    sc = m_tsos_holemap->regHist(trackHistos);
    m_tsos_outliermap = std::make_unique<PixelMon2DMapsLW>("TSOS_Outlier", ("TSOS of type Outlier" + m_histTitleExt), PixMon::HistConf::kPixDBMIBL2D3D, true);
    sc = m_tsos_outliermap->regHist(trackHistos);
  }

  m_tsos_holeratio_tmp = std::make_unique<PixelMon2DProfilesLW>("HolesRatio", ("Holes per track" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true);
  sc = m_tsos_holeratio_tmp->regHist(trackHistos);
  m_misshits_ratio_tmp = std::make_unique<PixelMon2DProfilesLW>("MissHitsRatio", ("Hole+Outlier per track" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true);
  sc = m_misshits_ratio_tmp->regHist(trackHistos);

  if (m_doOnline) {
    m_tsos_holeratio_mon = std::make_unique<PixelMon2DProfilesLW>("HolesRatio_mon", ("Holes per track reset every 5 min" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true);
    sc = m_tsos_holeratio_mon->regHist(trackHistos);
    m_misshits_ratio_mon = std::make_unique<PixelMon2DProfilesLW>("MissHitsRatio_mon", ("Hole+Outlier per track reset every 5 min" + m_histTitleExt), PixMon::HistConf::kPixIBL2D3D, true);
    sc = m_misshits_ratio_mon->regHist(trackHistos);
  }

  for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
    hname = makeHistname(("HitEff_all_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("hit efficiency, " + m_modLabel_PixLayerIBL2D3D[i]), ";lumi block;hit efficiency", false);
    sc = trackHistos.regHist(m_hiteff_incl_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));
    if (m_doOnline) {
      hname = makeHistname(("HitEff_last100lb_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("hit efficiency last 100 LB, " + m_modLabel_PixLayerIBL2D3D[i]), ";last 100 lumi blocks;hit efficiency", false);
      sc = trackHistos.regHist(m_hiteff_lastXlb_mod[i] = new TProfile(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5));
    }
  }

  hname = makeHistname("NPixhits_per_track_lumi", false);
  htitles = makeHisttitle("Number of pixhits per track", ";lumi block;number of hits", false);
  sc = trackHistos.regHist(m_npixhits_per_track_lumi = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 10, -0.5, 9.5));
  if (m_doOnline) {
    hname = makeHistname("NPixhits_per_track_last100lb", false);
    htitles = makeHisttitle("Number of pixhits per track last 100 LB", ";last 100 lumi blocks;number of hits", false);
    sc = trackHistos.regHist(m_npixhits_per_track_lastXlb = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 10, -0.5, 9.5));
    if (m_npixhits_per_track_lastXlb) m_npixhits_per_track_lastXlb->SetOption("colz");
  }

  if (sc.isFailure()) ATH_MSG_WARNING("Problems with booking Track histograms");
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillTrackMon(void) {
  ATH_MSG_DEBUG("Filling Track Monitoring Histograms");

  auto tracks = SG::makeHandle(m_TracksName);
  if (!(tracks.isValid())) {
    ATH_MSG_INFO("No tracks in StoreGate found");
    if (m_storegate_errors) m_storegate_errors->Fill(4., 3.);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Tracks in StoreGate found");
  }

  m_ntracksPerEvent = 0;

  if (m_doOnTrack) {
    m_RDOIDs.clear();
    m_ClusterIDs.clear();
  }

  TrackCollection::const_iterator itrack = tracks->begin();
  TrackCollection::const_iterator itrack_end = tracks->end();
  for (; itrack != itrack_end; ++itrack) {
    const Trk::Track *track0 = (*itrack);
    if (track0 == 0 || track0->perigeeParameters() == 0 || track0->trackSummary() == 0 || track0->trackSummary()->get(Trk::numberOfPixelHits) == 0) {
      ATH_MSG_DEBUG("Track either invalid or it does not contain pixel hits, continuing...");
      continue;
    }

    const Trk::TrackSummary *summary = track0->trackSummary();
    const Trk::Perigee *measPerigee = dynamic_cast<const Trk::Perigee *>(track0->perigeeParameters());
    const Trk::Track *track = track0;

    int npixholes = summary->get(Trk::numberOfPixelHoles);
    if (m_doHoleSearch && npixholes > 0) {
      track = m_holeSearchTool->getTrackWithHoles(*track0);
    }
    int nPixelHits = 0;
    bool passJOTrkTightCut = static_cast<bool>(m_trackSelTool->accept(*track0));
    bool pass1hole1GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 1.0));  // misshit ratios
    bool pass1hole5GeVptTightCut = (passJOTrkTightCut && (measPerigee->pT() / 1000.0 > 5.0));  // eff vs lumi

    const DataVector<const Trk::TrackStateOnSurface> *trackStates = track->trackStateOnSurfaces();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator = trackStates->begin(); trackStateOnSurfaceIterator != trackStates->end(); trackStateOnSurfaceIterator++) {

      const Trk::MeasurementBase *mesb = (*trackStateOnSurfaceIterator)->measurementOnTrack();
      const Trk::RIO_OnTrack *hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack *>(mesb) : 0;
      if (mesb && !hit) continue;  // skip pseudomeasurements but not holes, outliers

      const Trk::TrackParameters *trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
      Identifier surfaceID;
      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
        surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
      } else {  // holes, perigee
        if (trkParameters) {
          surfaceID = trkParameters->associatedSurface().associatedDetectorElementIdentifier();
        } else {
          ATH_MSG_INFO("pointer of TSOS to track parameters or associated surface is null");
          continue;
        }
      }

      if (!m_idHelper->is_pixel(surfaceID)) continue;
      int pixlayer = getPixLayerID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
      if (pixlayer == 99) continue;

      float nOutlier = 0.;
      float nHole = 0.;

      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)) {
        nOutlier = 1.0;
        if (m_tsos_holemap) m_tsos_holemap->fill(surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayer] && pass1hole5GeVptTightCut) m_hiteff_incl_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), 0.0);
      }
      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)) {
        nHole = 1.0;
        if (m_tsos_outliermap) m_tsos_outliermap->fill(surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayer] && pass1hole5GeVptTightCut) m_hiteff_incl_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), 0.0);
      }
      if (pass1hole1GeVptTightCut) {
        if (m_tsos_holeratio_tmp) m_tsos_holeratio_tmp->fill(surfaceID, m_pixelid, nHole);
        if (m_misshits_ratio_tmp) m_misshits_ratio_tmp->fill(surfaceID, m_pixelid, nOutlier + nHole);
        if (m_doOnline) {
          if (m_tsos_holeratio_mon) m_tsos_holeratio_mon->fill(surfaceID, m_pixelid, nHole);
          if (m_misshits_ratio_mon) m_misshits_ratio_mon->fill(surfaceID, m_pixelid, nOutlier + nHole);
        }
      }
      if ((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)) {
        if (m_tsos_hitmap) m_tsos_hitmap->fill(surfaceID, m_pixelid);
        if (m_hiteff_incl_mod[pixlayer] && pass1hole5GeVptTightCut) m_hiteff_incl_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), 1.0);
      } else {
        continue;
        // working only with real hits (not outliers or holes) from now on
      }
      //need the mesb to be sensible before dereferencing it
      if (not mesb) continue;
      const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(mesb->associatedSurface().associatedDetectorElement());
      const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack *>(mesb);
      if (!side || !clus) continue;
      const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster *>(clus->prepRawData());
      if (!RawDataClus || !RawDataClus->detectorElement()->isPixel()) continue;

      nPixelHits++;  //add another pixel hit

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
        Amg::Vector3D mytrack = trackAtPlane->momentum();
        double trknormcomp = mytrack.dot(mynormal);

        // Fill containers, which hold id's of hits and clusters on track _and_ incident angle information for later normalization
        double mytrack_mag = mytrack.mag();
        double cosalpha = 0.;
        if (mytrack_mag != 0) cosalpha = fabs(trknormcomp / mytrack_mag);
        if (m_doOnTrack) {
          for (unsigned int loopSize = 0; loopSize < RawDataClus->rdoList().size(); loopSize++) {
            m_RDOIDs.push_back(RawDataClus->rdoList().at(loopSize));
          }
          m_ClusterIDs.push_back(std::make_pair(clus->identify(), cosalpha));
        }
      }
    }  // end of TSOS loop

    if (m_track_chi2 && track0->fitQuality()->numberDoF() != 0) m_track_chi2->Fill(track0->fitQuality()->chiSquared() / track0->fitQuality()->numberDoF());
    if (m_npixhits_per_track_lumi) m_npixhits_per_track_lumi->Fill(m_manager->lumiBlockNumber(), nPixelHits);
    m_ntracksPerEvent++;

    if (m_doHoleSearch && npixholes > 0) delete track;
  }  // end of track loop

  // Fill histograms per event
  if (m_tracksPerEvt_per_lumi) m_tracksPerEvt_per_lumi->Fill(m_manager->lumiBlockNumber(), m_ntracksPerEvent);

  if (m_doOnTrack) {
    sort(m_RDOIDs.begin(), m_RDOIDs.end());
    sort(m_ClusterIDs.begin(), m_ClusterIDs.end(), [](const std::pair<Identifier, double> &left, const std::pair<Identifier, double> &right) {
      return left.first < right.first;
    });
  }

  if (m_doOnline) {
    if (m_doRefresh5min) {
      if (m_tsos_holeratio_mon) m_tsos_holeratio_mon->reset();
      if (m_misshits_ratio_mon) m_misshits_ratio_mon->reset();
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procTrackMon(void) {
  if (m_doOnline) {
    int lastlb = m_manager->lumiBlockNumber()-1; //remove -1 for testing
    double cont(0.0);
    int entr(0), entries(0);
    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      if (m_hiteff_incl_mod[i] && m_hiteff_lastXlb_mod[i]) {
	int bing = m_hiteff_incl_mod[i]->GetXaxis()->FindBin(lastlb);

	int nXbins = m_hiteff_lastXlb_mod[i]->GetNbinsX();
	m_hiteff_lastXlb_mod[i]->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
	m_hiteff_lastXlb_mod[i]->Reset();

	for (int binf=m_hiteff_lastXlb_mod[i]->GetNbinsX(); binf>0; binf--) {
	  if (bing>0) {
	    cont = m_hiteff_incl_mod[i]->GetBinContent(bing);
	    entr = m_hiteff_incl_mod[i]->GetBinEntries(bing);
	    entries += entr;	   
	    if (entr>0) {
	      m_hiteff_lastXlb_mod[i]->SetBinEntries(binf, entr);
	      m_hiteff_lastXlb_mod[i]->SetBinContent(binf, cont * entr);
	      (*m_hiteff_lastXlb_mod[i]->GetSumw2())[binf] = cont * entr; // works only for this type of histogram
	    }
	    bing--;
	  } 
	}
	m_hiteff_lastXlb_mod[i]->SetMinimum(0.8);
        m_hiteff_lastXlb_mod[i]->SetMaximum(1.01);
	m_hiteff_lastXlb_mod[i]->SetEntries(entries);
	//m_hiteff_lastXlb_mod[i]->SetEntries(lastlb);      // for testing
      }
    }
    if (m_npixhits_per_track_lumi && m_npixhits_per_track_lastXlb) {
      int bingx = m_npixhits_per_track_lumi->GetXaxis()->FindBin(lastlb);
      int nbingy = m_npixhits_per_track_lumi->GetNbinsY();

      int nXbins = m_npixhits_per_track_lastXlb->GetNbinsX();
      m_npixhits_per_track_lastXlb->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_npixhits_per_track_lastXlb->Reset();

      for (int binfx=m_npixhits_per_track_lastXlb->GetNbinsX(); binfx>0; binfx--) {
	if (bingx>0) {
	  for (int bingy = 1; bingy <= nbingy; bingy++) {
	    cont = m_npixhits_per_track_lumi->GetBinContent(bingx, bingy);
	    if (cont!=0) {
	      m_npixhits_per_track_lastXlb->SetBinContent(binfx, bingy, cont);
	    }
	  }
	  bingx--;
	} 
      }
      //m_npixhits_per_track_lastXlb->SetEntries(lastlb);      // for testing 
    }
  }
  if (m_doOffline) {
    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      if (m_hiteff_incl_mod[i]) {
	m_hiteff_incl_mod[i]->SetMinimum(0.8);
	m_hiteff_incl_mod[i]->SetMaximum(1.01);
      }
    }
  }
  return StatusCode::SUCCESS;
}
