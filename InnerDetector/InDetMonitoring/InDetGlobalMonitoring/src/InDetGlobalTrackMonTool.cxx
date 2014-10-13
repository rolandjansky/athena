/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalTrackMonTool.cxx
 * Implementation of inner detector global track monitoring tool
 * based on Segment tool
 *
 *@author
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>   @n 
 * Gaetano Barone <Gaetano.Barone@cern.ch> @n 
 *
 * $Id: InDetGlobalTrackMonTool.cxx,v 1.33 2009-05-14 22:44:19 kastanas Exp $
 *
 ****************************************************************************/

///// ToDo:
// * Crosscheck all the histograms
// * Add the ratios!
// * Added the dead module maps
// * Ratio plots:
// ** No pix, pix hits
// ** SCT hits
// ** ADD TRT extension

/////

//Local
#include "InDetGlobalMonitoring/InDetGlobalTrackMonTool.h"
//Framework
#include "TrkTrack/TrackCollection.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GaudiKernel/StatusCode.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkSurfaces/Surface.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "TH1I.h"
#include "LWHists/TProfile_LW.h"
#include "TProfile2D.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

//Root
#include "TMath.h"

//Standard c++
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <functional>


InDetGlobalTrackMonTool::InDetGlobalTrackMonTool( const std::string & type,
						  const std::string & name,
						  const IInterface* parent)
    : ManagedMonitorToolBase(type, name, parent),
      m_doLumiblock(true),
      m_doHolePlots(false),
      m_doHitMaps(false),
      m_DoHoles_Search(false),
      m_nBinsEta( 50 ),
      m_nBinsPhi( 50 ),
      m_trackBin( 100 ),
      m_trackMax( 1000 ),
      m_d0Max( 15 ),
      m_z0Max( 400 ),
      c_etaRange(2.5),
      c_etaTrackletsMin(2.4),
      c_etaTrackletsMax(2.7),
      c_etaRangeTRT(2.0),
      c_EndCapBoundary( 1.6 ),
      c_BarrelBoundary( 0.8 ),
      m_holes_search_tool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
      m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
      m_CombinedTracksName("Tracks"),
      m_ForwardTracksName("ResolvedForwardTracks"),
      m_ID_COMB_eta_phi(0),
      m_ID_COMB_noTRText_eta_phi(0),
      m_ID_COMB_noSCThits_eta_phi(0),
      m_ID_COMB_noPIXhits_eta_phi(0),
      m_ID_COMB_noPIXhitsRatio_eta_phi(0),
      m_ID_COMB_btagqual_eta_phi(0),
      m_ID_COMB_noBLayerHitExpected_eta_phi(0),
      m_ID_COMB_noBLayerHit_eta_phi(0),
      m_ID_COMB_d0(0),
      m_ID_COMB_z0(0),
      m_ID_COMB_qoverp(0),
      m_ID_COMB_chi2(0),
      m_FORW_qoverp(0),
      m_FORW_chi2(0),
      m_ID_COMB_pt(0),
      m_ID_nCOMBtrks(0),
      m_COMB_goodPixTrk_eta_phi(0),
      m_COMB_goodPixTrkRatio_eta_phi(0),
      m_FORW_forwardTracklets_pos_eta_phi(0),
      m_FORW_forwardTracklets_neg_eta_phi(0),
      m_COMB_goodSctTrk_eta_phi(0),
      m_COMB_goodSctTrkRatio_eta_phi(0),
      m_COMB_goodTrtTrk_eta_phi(0),
      m_COMB_goodEleMTrk_eta_phi(0),
      m_COMB_nCOMBtrks_LB(0),
      m_PIX_ntrk_LB(0),
      m_PIX_ntrk_noPIXhits_LB(0),
      m_PIX_ntrk_noBLhits_LB(0),
      m_SCT_ntrk_LB(0),
      m_TRT_ntrk_LB(0),
      m_trk_rate_pix_LB(0),
      m_trk_rate_sct_LB(0),
      m_trk_rate_trt_LB(0),
      m_LB_start_time(0),
      m_LB_current_time(0),
      m_current_LB(0),
      m_range_LB(1000),
      m_holes_eta_phi(0),
      m_holes_eta_pt(0),
      m_holes_phi_pt(0), 
      m_holes_eta_phi_n(0),
      m_holes_hits(0),
      m_holesvshits(0),
      m_holesvshits_ECA(0),
      m_holesvshits_ECC(0),
      m_holesvshits_BA(0),
      m_ID_hitmap_x_y(0),
      m_ID_hitmap_x_y_eca(0),
      m_ID_hitmap_x_y_ecc(0),
      m_HolesMAP_XY(0),
      m_HolesMAP_ZX(0),
      m_HolesMAP_ZR(0),
      m_Trk_nPIXhits(0),
      m_Trk_nPIXhits_EA(0),
      m_Trk_nPIXhits_TA(0),
      m_Trk_nPIXhits_B(0),
      m_Trk_nPIXhits_TC(0),
      m_Trk_nPIXhits_EC(0),
      m_Trk_nPIXhits_FA(0),
      m_Trk_nPIXhits_FC(0),
      m_Trk_nPIXhits_eta_phi(0),
      m_Trk_nPIXDeadModules_eta_phi(0),
      m_Trk_nSCThits(0),
      m_Trk_nSCThits_EA(0),
      m_Trk_nSCThits_TA(0),
      m_Trk_nSCThits_B(0),
      m_Trk_nSCThits_TC(0),
      m_Trk_nSCThits_EC(0),
      m_Trk_nSCThits_eta_phi(0),
      m_Trk_nSCTDeadModules_eta_phi(0),
      m_Trk_nTRThits(0),
      m_Trk_nTRThits_EA(0),
      m_Trk_nTRThits_TA(0),
      m_Trk_nTRThits_B(0),
      m_Trk_nTRThits_TC(0),
      m_Trk_nTRThits_EC(0),
      m_Trk_nTRThits_eta_phi(0)
{
    declareProperty("DoHoleSearch",m_DoHoles_Search,"Write hole data?");
    declareProperty("HoleSearchTool", m_holes_search_tool,"Tool to search for holes on track");	
    declareProperty("TrackCollection",m_CombinedTracksName,"Name of the forward tracklet collection");
    declareProperty("ForwardTrackCollection",m_ForwardTracksName,"Name of the combined track collection");
    declareProperty("DoHitMaps", m_doHitMaps,"Produce hit maps?");	
    declareProperty("trackMax",m_trackMax,"Maximum number of tracks in track histograms");
    declareProperty("trackBin",m_trackBin,"Number of bins for the track histograms");
    declareProperty("d0Max", m_d0Max,"D0 range to use");
    declareProperty("z0Max", m_z0Max,"Z0 range to use");
}

StatusCode InDetGlobalTrackMonTool::initialize() {
  StatusCode sc;
  
  // If any of the ID helpers are not found then we don't make the hit maps
  m_trtID = 0;
  if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
      ATH_MSG_DEBUG("Could not get TRT ID helper");
      m_doHitMaps = false;
  }
 
  m_sctID = 0;
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
      ATH_MSG_DEBUG("Could not get SCT ID helper");
      m_doHitMaps = false;
  }
  
  m_pixelID = 0;
  if (detStore()->retrieve(m_pixelID, "PixelID").isFailure()) {
      ATH_MSG_DEBUG("Could not get Pixel ID helper");
      m_doHitMaps = false;
  }
  
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalTrackMonTool::bookHistograms()
{
    registerManHist( m_ID_nCOMBtrks, "InDetGlobal/Track", lowStat,
		     "nCOMBtrks", "Track multiplicity",
		     m_trackBin, 0.5, m_trackMax+0.5,
		     "Track multiplicity" ).ignore();

    registerManHist( m_ID_COMB_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_eta_phi","Distribution of eta vs phi for combined tracks (both Si and TRT hits)",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta","#phi" ).ignore();
    
    registerManHist( m_ID_COMB_pt, "InDetGlobal/Track", lowStat,
		     "COMB_pt", "P_{T} of reconstructed combined tracks (both Si and TRT hits)",
		     50, -0., 10,
		     "P_{T} [GeV]" ).ignore();
    
    registerManHist( m_ID_COMB_qoverp, "InDetGlobal/Track", lowStat,
		     "COMB_qoverp", "Distribution of qoverp (both Si and TRT hits)",
		     400, -2.5, 2.5,
		     "Inverse momentum [GeV^{-1}]" ).ignore();
    
    registerManHist( m_ID_COMB_d0, "InDetGlobal/Track", lowStat,
		     "COMB_d0","Distribution of d_{0} for combined tracks (both Si and TRT hits)",
		     100, -m_d0Max, m_d0Max,
		     "Track d_{0} [mm]" ).ignore();
    
    registerManHist( m_ID_COMB_z0, "InDetGlobal/Track", lowStat,
		     "COMB_z0","Distribution of z0 for combined tracks (both Si and TRT hits)",
		     100, -m_z0Max, m_z0Max,
		     "Track z_{0} [mm]" ).ignore();
    
    registerManHist( m_ID_COMB_chi2, "InDetGlobal/Track", lowStat,
		     "COMB_chi2", "#chi^{2}/DoF of reconstructed combined tracks (both Si and TRT hits)",
		     80, -0, 20,
		     "#chi^{2}/DoF" ).ignore();

    // Eta-phi maps
    registerManHist( m_ID_COMB_noTRText_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_noTRText_eta_phi","Distribution of eta vs phi for combined tracks with no TRT extension",
		     20, -2, 2, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "eta", "#phi_{0}" ).ignore();
    
    registerManHist( m_ID_COMB_noSCThits_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_noSCThits_eta_phi","Distribution of eta vs phi for combined tracks without SCT hits",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_ID_COMB_noPIXhits_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_noPIXhits_eta_phi","Distribution of eta vs phi for combined tracks without Pixel hits",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_ID_COMB_noPIXhitsRatio_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_noPIXhitsRatio_eta_phi","Distribution of eta vs phi for combined tracks without Pixel hits, ratio to all tracks",
		     m_nBinsEta,-c_etaRange,c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi_{0}" ).ignore();
    m_ID_COMB_noPIXhitsRatio_eta_phi->SetBit(TH1::kIsAverage);

    registerManHist( m_ID_COMB_btagqual_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_btagqual_eta_phi","Distribution of eta vs phi for combined tracks passing b-tag quality cuts",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();
    
    registerManHist( m_ID_COMB_noBLayerHitExpected_eta_phi, "InDetGlobal/Track", lowStat,
		     "ID_COMB_noBLayerHitExpected_eta_phi","Eta-phi of tracks with no b-layer hit, but hit is expected",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_ID_COMB_noBLayerHitExpectedRatio_eta_phi, "InDetGlobal/Track", lowStat,
		     "ID_COMB_noBLayerHitExpectedRatio_eta_phi","Eta-phi of tracks with no b-layer hit, but hit is expected, ratio to all tracks",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();
    m_ID_COMB_noBLayerHitExpectedRatio_eta_phi->SetBit(TH1::kIsAverage);

    registerManHist( m_ID_COMB_noBLayerHit_eta_phi, "InDetGlobal/Track", lowStat,
		     "ID_COMB_noBLayerHit_eta_phi","Eta-phi of tracks with no b-layer hit",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}").ignore();

    registerManHist( m_ID_COMB_noBLayerHitRatio_eta_phi, "InDetGlobal/Track", lowStat,
		     "ID_COMB_noBLayerHitRatio_eta_phi","Eta-phi of tracks with no b-layer hit, ratio to total tracks",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}").ignore();
    m_ID_COMB_noBLayerHitRatio_eta_phi->SetBit(TH1::kIsAverage);

    registerManHist( m_COMB_goodPixTrk_eta_phi, "InDetGlobal/Track", lowStat,
                     "COMB_goodPixTrk_eta_phi,","Distribution of #eta vs #phi_{0} for combined tracks (>=1 Pixel hits)",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi,-M_PI,M_PI,
                     "#eta","#phi" ).ignore();
    
    registerManHist( m_COMB_goodPixTrkRatio_eta_phi, "InDetGlobal/Track", lowStat,
                     "COMB_goodPixTrkRatio_eta_phi","Distribution of #eta vs #phi_{0} for combined tracks (>=1 Pixel hits), ratio to total tracks",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi,-M_PI,M_PI,
                     "#eta","#phi" ).ignore();
    m_COMB_goodPixTrkRatio_eta_phi->SetBit(TH1::kIsAverage);

    registerManHist( m_COMB_goodSctTrk_eta_phi, "InDetGlobal/Track", lowStat,
                     "COMB_goodSctTrk_eta_phi,", "Distribution of #eta vs #phi_{0} for combined tracks (>=6 SCT hits)",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi,-M_PI,M_PI,
                     "#eta","#phi" ).ignore();
    
    registerManHist( m_COMB_goodSctTrkRatio_eta_phi, "InDetGlobal/Track", lowStat,
                     "COMB_goodSctTrkRatio_eta_phi","Distribution of #eta vs #phi_{0} for combined tracks (>=6 SCT hits), ratio to total tracks",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi, -M_PI, M_PI,
                     "#eta","#phi" ).ignore();
    m_COMB_goodSctTrkRatio_eta_phi->SetBit(TH1::kIsAverage);
    registerManHist( m_COMB_goodTrtTrk_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_goodTrtTrk_eta_phi", "Distribution of #eta vs #phi_{0} for combined tracks (>=20 TRT hits)",
		     20, -2, 2, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_COMB_goodEleMTrk_eta_phi, "InDetGlobal/Track", lowStat,
		     "COMB_goodEleMTrk_eta_phi", "Distribution of #eta vs #phi_{0} for combined tracks (Electron Track, Medium cut)",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}").ignore();

    
    registerManHist( m_Trk_nTRThits_eta_phi, "InDetGlobal/Track", lowStat,
		     "Trk_nTRThits_eta_phi","Number of TRT hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi_{0}" ).ignore(); 

    
    // Hit maps
    if ( m_doHitMaps )
    {
	registerManHist( m_ID_hitmap_x_y, "InDetGlobal/Hits", run,
			 "ID_hitmap_x_y","Map of ID hits (BARREL) in x vs y (mm)",
			 400,-1100,1100,
			 400,-1100,1100,
			 "x [mm]", "y [mm]" ).ignore();
	
	registerManHist( m_ID_hitmap_x_y_eca, "InDetGlobal/Hits", run,
			 "ID_hitmap_x_y_eca","Map of ID hits (ECA) in x vs y (mm)",
			 400,-1100,1100,
			 400,-1100,1100,
			 "x [mm]", "y [mm]" ).ignore();
	
	registerManHist( m_ID_hitmap_x_y_ecc, "InDetGlobal/Hits", run,
			 "ID_hitmap_x_y_ecc","Map of ID hits (ECC) in x vs y (mm)",
			 400,-1100,1100,
			 400,-1100,1100,
			 "x [mm]", "y [mm]" ).ignore();
    }

    // Pixel hit hists
    registerManHist( m_Trk_nPIXhits, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits","Number of Pixel hits per track",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();
    
    registerManHist( m_Trk_nPIXhits_FA, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_FA","Number of Pixel hits per track (Forward tracks ECA)",
		     5, 0.5, 5.5,
		     "# of hits per track" ).ignore();
    
    registerManHist( m_Trk_nPIXhits_EA, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_EA","Number of Pixel hits per track (ECA)",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nPIXhits_TA, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_TA","Number of Pixel hits per track (Transition region A)",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nPIXhits_B, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_B","Number of Pixel hits per track (B)",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nPIXhits_TC, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_TC","Number of Pixel hits per track (Transition region C)",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nPIXhits_EC, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_EC","Number of Pixel hits per track (ECC)",
		     13, -0.5, 12.5,
		     "# of hits per track" ).ignore();
	
    registerManHist( m_Trk_nPIXhits_FC, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_FC","Number of Pixel hits per track (Forward tracks FC)",
		     5, 0.5, 5.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nPIXhits_eta_phi,  "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXhits_eta_phi","Number of Pixel hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_Trk_nPIXDeadModules_eta_phi, "InDetGlobal/Hits", lowStat,
		     "Trk_nPIXDeadModules_eta_phi","Number of Pixel dead modules per track, eta-phi profile",
		     m_nBinsEta,-c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    // SCT hit hists
    registerManHist( m_Trk_nSCThits, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits","Number of SCT hits per track",
		     31, -0.5, 30.5,
		     "# of hits per track" ).ignore();
    
    registerManHist( m_Trk_nSCThits_EA, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_EA","Number of SCT hits per track (ECA)",
		     31,-0.5,30.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nSCThits_TA, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_TA","Number of SCT hits per track (Transition region A)",
		     31,-0.5,30.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nSCThits_B, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_B","Number of SCT hits per track (B)",
		     31,-0.5,30.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nSCThits_TC, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_TC","Number of SCT hits per track (Transition region C)",
		     31,-0.5,30.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nSCThits_EC, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_EC","Number of SCT hits per track (ECC)",
		     31,-0.5,30.5,
		     "# of hits per track" ).ignore();

    registerManHist( m_Trk_nSCThits_eta_phi, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCThits_eta_phi","Number of SCT hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta","#phi" ).ignore(); 

    registerManHist( m_Trk_nSCTDeadModules_eta_phi, "InDetGlobal/Hits", lowStat,
		     "Trk_nSCTDeadModules_eta_phi","Number of SCT dead modules per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta","#phi" ).ignore(); 

    // TRT hit hists
    registerManHist( m_Trk_nTRThits, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits","Number of TRT hits per track",
		     100,0.5,100.5,
		     "# of hits per track").ignore();

    registerManHist( m_Trk_nTRThits_EA, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits_EA","Number of TRT hits per track (ECA)",
		     100,0.5,100.5,
		     "# of hits per track").ignore(); 

    registerManHist( m_Trk_nTRThits_TA, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits_TA","Number of TRT hits per track (Transition region A)",
		     100,0.5,100.5,
		     "# of hits per track").ignore(); 

    registerManHist( m_Trk_nTRThits_B, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits_B","Number of TRT hits per track (Barrel)",
		     100,0.5,100.5,
		     "# of hits per track").ignore(); 

    registerManHist( m_Trk_nTRThits_TC, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits_TC","Number of TRT hits per track (Transition region C)",
		     100,0.5,100.5,
		     "# of hits per track").ignore(); 

    registerManHist( m_Trk_nTRThits_EC, "InDetGlobal/Hits", lowStat,
		     "Trk_nTRThits_EC","Number of TRT hits per track (ECC)",
		     100,0.5,100.5,
		     "# of hits per track").ignore(); 
    

    // Forward Pixel tracks
    registerManHist( m_FORW_forwardTracklets_pos_eta_phi, "InDetGlobal/Track", lowStat,
		     "FORW_pos_eta_phi", "Eta-phi for pixel tracklets in the forward region (positive eta)",
		     5, c_etaTrackletsMin, c_etaTrackletsMax, 
		     m_nBinsPhi, -M_PI, M_PI, 
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_FORW_forwardTracklets_neg_eta_phi, "InDetGlobal/Track", lowStat,
		     "FORW_neg_eta_phi", "Eta-phi for pixel tracklets in the forward region (negative eta)",
		     5, -c_etaTrackletsMax, -c_etaTrackletsMin, 
		     m_nBinsPhi, -M_PI, M_PI, 
		     "#eta", "#phi_{0}" ).ignore();

    registerManHist( m_FORW_chi2, "InDetGlobal/Track", lowStat,
		     "FORW_chi2", "#chi^{2}/DoF of reconstructed forward tracklets",
		     80, -0., 20.,
		     "#chi^{2}/DoF").ignore();
    
    registerManHist( m_FORW_qoverp, "InDetGlobal/Track", lowStat,
		     "FORW_qoverp", "Distribution of qoverp (forward Pixel tracklets)", 
		     400, -2.5, 2.5,
		     "Inverse momentum [GeV^{-1}]" ).ignore();


    // LB histograms
    registerManHist( m_COMB_nCOMBtrks_LB, "InDetGlobal/Track", lowStat,
		     "COMB_nCOMBtrks_LB","Average number of tracks by good LB",
		     m_range_LB,0,m_range_LB,
		     "LB #", "Average number of tracks in LB").ignore();
    
    registerManHist( m_PIX_ntrk_LB, "InDetGlobal/Track", lowStat,
		     "PIX_ntrk_LB", "Number of good pixel tracks per LB",
		     m_range_LB,0., m_range_LB,
		     "LB #", "Total number of tracks in LB" ).ignore();
    
    registerManHist( m_PIX_ntrk_noPIXhits_LB, "InDetGlobal/Track", lowStat,
		     "PIX_ntrk_noPIXhits_LB", "Number of tracks with no Pixel hits per LB",
		     m_range_LB,0., m_range_LB,
		     "LB #", "Total number of tracks with no Pixel hits by LB" ).ignore();

    registerManHist( m_PIX_ntrk_noBLhits_LB, "InDetGlobal/Track", lowStat,
		     "PIX_ntrk_noBLhits_LB", "Number of tracks with no b-layer hits per LB",
		     m_range_LB,0., m_range_LB,
		     "LB #", "Total number of tracks with no b-layer hits by LB" ).ignore();
    
    registerManHist( m_SCT_ntrk_LB, "InDetGlobal/Track", lowStat,
		     "SCT_ntrk_LB", "Number of good SCT tracks per LB",
		     m_range_LB, 0., m_range_LB ,
		     "LB #", "Total number of tracks in LB" ).ignore();
    
    registerManHist( m_TRT_ntrk_LB, "InDetGlobal/Track", lowStat,
		     "TRT_ntrk_LB","Number of good TRT tracks per LB",
		     m_range_LB,0.,m_range_LB,
		     "LB #", "Total number of tracks in LB" ).ignore();

    m_COMB_nCOMBtrks_LB->SetBit(TH1::kCanRebin, true);
    m_PIX_ntrk_LB->SetBit(TH1::kCanRebin, true);
    m_PIX_ntrk_noPIXhits_LB->SetBit(TH1::kCanRebin, true);
    m_PIX_ntrk_noBLhits_LB->SetBit(TH1::kCanRebin, true);
    m_SCT_ntrk_LB->SetBit(TH1::kCanRebin, true);
    m_TRT_ntrk_LB->SetBit(TH1::kCanRebin, true);

    // Holes
    if ( m_doHolePlots )
    {
	registerManHist( sct_holes, "InDetGlobal/Hits", lowStat,
			 "sct_holes", "Distribution of SCT Holes ",
			 104,-3.5,100.5,
			 "Number of SCT holes").ignore();	
	
	registerManHist( trt_holes, "InDetGlobal/Hits", lowStat,
			 "trt_holes", "Distribution of TRT Holes ",
			 104,-3.5,100.5,
			 "Number of TRT holes").ignore();
	
	registerManHist( pixel_holes, "InDetGlobal/Hits", lowStat,
			 "pixel_holes", "Distribution of Pixel Holes ",
			 104,-3.5,100.5,
			 "Number of Pixel holes").ignore();
	
	registerManHist( comb_holes, "InDetGlobal/Hits", lowStat,
			 "comb_holes", "Distribution of Combined Holes ",
			 104,-3.5,100.5,
			 "Total number of holes").ignore();
	
	registerManHist( silicon_vs_trt, "InDetGlobal/Hits", lowStat,
			 "silicon_vs_trt", "Silicon vs TRT holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "Silicon Combined holes", "TRT holes").ignore();
	
	registerManHist( sct_vs_pixels, "InDetGlobal/Hits", lowStat,
			 "sct_vs_pixels", "SCT vs Pixels holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "SCT", "Pixels").ignore();
	
	registerManHist( holes_quality, "InDetGlobal/Hits", lowStat,
			 "holes_quality", "Number of holes/track vs #chi^{2}/ndf",
			 160,-0.5,15.5,
			 104,-3.5,100.5,
			 "#chi^{2}/ndf", "Combined Holes").ignore();	
	
	registerManHist( m_holes_eta_phi, "InDetGlobal/Hits", lowStat,
			 "holes_eta_phi", "Holes Map #eta #phi",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();
	
	
	registerManHist( m_holes_eta_pt,  "InDetGlobal/Hits", lowStat,
			 "holes_eta_pt", "Holes #eta vs p_{t}",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 30,-0.,30.,
			 "#eta", "#p_{t}").ignore();
	
	registerManHist( m_holes_phi_pt,  "InDetGlobal/Hits", lowStat,
			 "holes_phi_pt", "Holes #phi vs p_{t}",
			 m_nBinsPhi,-M_PI,M_PI,
			 30,-0.,30.,
			 "#phi", "#p_{t}").ignore();
	
	registerManHist( m_holes_eta_phi_n,  "InDetGlobal/Hits", lowStat,
			 "holes_eta_phi_n", "Holes Map (Norm) #eta #phi",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();
	
	registerManHist( holes_quality_profile,  "InDetGlobal/Hits", lowStat,
			 "holes_quality_profile", "#chi^{2}/ndf vs Number of holes/track profile",
			 104,-3.5,100.5,
			 "<#chi^{2}/ndf> / comb hole", "Combined Holes").ignore();
	holes_quality_profile->SetErrorOption("S");
	holes_quality_profile->SetMaximum( 1000 );
	holes_quality_profile->SetMinimum( 0 );
    
	
	registerManHist( m_holes_hits,  "InDetGlobal/Hits", lowStat,
			 "holes_hits", "Number of holes vs number of hits per track Combined",
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb. holes/track").ignore();
	m_holes_hits->SetErrorOption("S");
	m_holes_hits->SetMaximum( 1000 );
	m_holes_hits->SetMinimum( -10 );
	
	registerManHist( m_holesvshits,  "InDetGlobal/Hits", lowStat,
			 "holesvshits","Number of Holes/track vs Number of Hits/track if holes >0 ",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();
	
	registerManHist( m_holesvshits_ECA,  "InDetGlobal/Hits", lowStat,
			 "holesvshits_ECA", "Number of Holes/track vs Number of Hits/track if holes >0 EndCap A",
			 101,-0.5,100.5,
			 101,-0.5,100.5, 
			 "nb. of hits/track", "nb holes/track").ignore();	
	
	registerManHist( m_holesvshits_ECC,  "InDetGlobal/Hits", lowStat,
			 "holesvshits_ECC", "Number of Holes/track vs Number of Hits/track if holes >0 EndCap C",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();
	
	registerManHist( m_holesvshits_BA,  "InDetGlobal/Hits", lowStat,
			 "holesvshits_BA", "Number of Holes/track vs Number of Hits/track if holes >0 BARREL",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();
	
	registerManHist( m_HolesMAP_XY,  "InDetGlobal/Hits", lowStat,
			 "HolesMAP_XY", "Map of ID holes x vs y (mm)",
			 400,-1100,1100,
			 400,-1100,1100,
			 "X [mm]", "Y [mm]").ignore();
	
	registerManHist( m_HolesMAP_ZR,  "InDetGlobal/Hits", lowStat,
			 "HolesMAP_ZR", "Map of ID holes  z vs r (mm)",
			 3000,-3100,3100,
			 1100,0,1100,
			 "Z [mm]", "R [mm]").ignore(); 
	
	registerManHist( m_HolesMAP_ZX,  "InDetGlobal/Hits", lowStat,
			 "HolesMAP_ZX", "Map of ID holes  z vs x (mm)",
			 1000,-3100,3100,
			 300,-1100,1100,
			 "Z [mm]", "X [mm]").ignore();
    }

    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode InDetGlobalTrackMonTool::bookHistogramsRecurrent()
{
    if ( newLumiBlock )
    {
	m_COMB_nCOMBtrks_LB->RebinAxis( m_manager->lumiBlockNumber(),  m_COMB_nCOMBtrks_LB->GetXaxis() );
	m_PIX_ntrk_LB->RebinAxis( m_manager->lumiBlockNumber(), m_PIX_ntrk_LB->GetXaxis() );
	m_PIX_ntrk_noPIXhits_LB->RebinAxis( m_manager->lumiBlockNumber(), m_PIX_ntrk_noPIXhits_LB->GetXaxis() );
	m_PIX_ntrk_noBLhits_LB->RebinAxis( m_manager->lumiBlockNumber(), m_PIX_ntrk_noBLhits_LB->GetXaxis() );
	m_SCT_ntrk_LB->RebinAxis( m_manager->lumiBlockNumber(), m_SCT_ntrk_LB->GetXaxis() );
	m_TRT_ntrk_LB->RebinAxis( m_manager->lumiBlockNumber(), m_TRT_ntrk_LB->GetXaxis() );
    }
    
    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode InDetGlobalTrackMonTool::fillHistograms() 
{   
    const TrackCollection * combined_tracks = 0;
    if ( evtStore()->contains<TrackCollection>( m_CombinedTracksName ) ) 
    {
	if ( evtStore()->retrieve(combined_tracks,m_CombinedTracksName).isFailure() )
	{
	    ATH_MSG_DEBUG( "Failed to retrieve combined tracks in StoreGate " + m_CombinedTracksName );
	    return StatusCode::SUCCESS;
	}
    }
    else
    {
	ATH_MSG_DEBUG( "No combined tracks in StoreGate " + m_CombinedTracksName );
	return StatusCode::SUCCESS;
    }

    m_ID_nCOMBtrks->Fill( combined_tracks->size() );
    m_COMB_nCOMBtrks_LB->Fill( m_manager->lumiBlockNumber(), combined_tracks->size() );

    TrackCollection::const_iterator itrack = combined_tracks->begin();
    TrackCollection::const_iterator itrack_end = combined_tracks->end();
    for ( ; itrack!= itrack_end; ++itrack)
    {
	const Trk::Track * track = (*itrack);
	if ( !track )
	{
	    ATH_MSG_DEBUG( "NULL track pointer in collection" );
	    return StatusCode::SUCCESS;
	}
	
	// Skip tracks that are not inside out
	
	if ( m_dataType == AthenaMonManager::collisions && ! (track->info().patternRecoInfo( Trk::TrackInfo::SiSPSeededFinder ) ) )
	    continue;

	// Create a new summary or get copy of the cached one
	const Trk::TrackSummary * summary = m_trkSummaryTool->createSummary( * track );
	
	if ( !summary )
	{
	    ATH_MSG_DEBUG( "NULL pointer to track summary" );
	    return StatusCode::SUCCESS;
	}

	FillHits( track, summary );

	if ( track->perigeeParameters() != 0 ) 
	{
	    FillEtaPhi( track , summary );
	}
	
	if ( m_doHitMaps ) 
	{
	    FillHitMaps( track );
	}
	
	if ( m_doHolePlots )  
	{
	    FillHoles( track, summary );
	}

	// Delete our copy
	delete summary;
    }

    const TrackCollection * forward_tracks = 0;
    if ( evtStore()->contains<TrackCollection>( m_ForwardTracksName ) ) 
    {
	if ( evtStore()->retrieve(forward_tracks,m_ForwardTracksName).isFailure() )
	{
	    ATH_MSG_DEBUG( "No combined tracks in StoreGate " + m_ForwardTracksName );
	    return StatusCode::SUCCESS;
	}
	
	TrackCollection::const_iterator iftrack = forward_tracks->begin();
	TrackCollection::const_iterator iftrack_end = forward_tracks->end();
	for ( ; iftrack!= iftrack_end; ++iftrack)
	{
	    const Trk::Track * track = (*iftrack);
	    msg(MSG::INFO) << "GOT TRACKS!" << std::endl;
	    if ( !track )
	    {
		ATH_MSG_DEBUG( "NULL track pointer in collection" );
		return StatusCode::SUCCESS;
	    }
	    
	    // Create a new summary or get copy of the cached one
	    const Trk::TrackSummary * summary = m_trkSummaryTool->createSummary( * track );
	    
	    if ( !summary )
	    {
		ATH_MSG_DEBUG( "NULL pointer to track summary" );
		return StatusCode::SUCCESS;
	    }
	    
	    FillForwardTracks( track , summary );
	    
	    delete summary;
	}
    }
    
    return StatusCode::SUCCESS;
}


StatusCode InDetGlobalTrackMonTool::procHistograms()
{
    if ( endOfRun ) 
    {
	m_ID_COMB_noPIXhitsRatio_eta_phi->Divide( m_ID_COMB_eta_phi );
	m_COMB_goodPixTrkRatio_eta_phi->Divide( m_ID_COMB_eta_phi );
	m_COMB_goodSctTrkRatio_eta_phi->Divide( m_ID_COMB_eta_phi );
	m_ID_COMB_noBLayerHitExpectedRatio_eta_phi->Divide( m_ID_COMB_eta_phi );
	m_ID_COMB_noBLayerHitRatio_eta_phi->Divide( m_ID_COMB_eta_phi );
    }
    
    if ( endOfLumiBlock ) {}

    return StatusCode::SUCCESS;
}

void InDetGlobalTrackMonTool::FillHits( const Trk::Track *track, const Trk::TrackSummary* summary )
{
    unsigned int pixHits = summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors);
    unsigned int sctHits = summary->get(Trk::numberOfSCTHits)   + summary->get(Trk::numberOfSCTDeadSensors);
    unsigned int trtHits = summary->get(Trk::numberOfTRTHits);

    m_Trk_nPIXhits->Fill( pixHits );
    m_Trk_nSCThits->Fill( sctHits );
    m_Trk_nTRThits->Fill( trtHits );

    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( !perigee )
	return;

    m_Trk_nPIXhits_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], pixHits );
    m_Trk_nPIXDeadModules_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], summary->get(Trk::numberOfPixelDeadSensors) );
    m_Trk_nSCThits_eta_phi->Fill(  perigee->eta(), perigee->parameters()[Trk::phi0],sctHits );
    m_Trk_nSCTDeadModules_eta_phi->Fill(  perigee->eta(), perigee->parameters()[Trk::phi0], summary->get(Trk::numberOfSCTDeadSensors) );
    m_Trk_nTRThits_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],trtHits );

    if ( perigee->eta() < - c_EndCapBoundary )
    {
	m_Trk_nTRThits_EA->Fill(trtHits);
	m_Trk_nSCThits_EA->Fill(sctHits);
	m_Trk_nPIXhits_EA->Fill(pixHits);
    }	
    else if ( perigee->eta() < - c_BarrelBoundary )
    {
	m_Trk_nTRThits_TA->Fill(trtHits);
	m_Trk_nSCThits_TA->Fill(sctHits);
	m_Trk_nPIXhits_TA->Fill(pixHits);
    }
    else if ( perigee->eta() < c_BarrelBoundary )
    {
	m_Trk_nTRThits_B->Fill(trtHits);
	m_Trk_nSCThits_B->Fill(sctHits);
	m_Trk_nPIXhits_B->Fill(pixHits);
    }
    else if ( perigee->eta() < c_EndCapBoundary )
    {
	m_Trk_nTRThits_TC->Fill(trtHits);
	m_Trk_nSCThits_TC->Fill(sctHits);
	m_Trk_nPIXhits_TC->Fill(pixHits);
    }
    else
    {
	m_Trk_nTRThits_EC->Fill(trtHits);
	m_Trk_nSCThits_EC->Fill(sctHits);
	m_Trk_nPIXhits_EC->Fill(pixHits);
    }
} 


void InDetGlobalTrackMonTool::FillEtaPhi( const Trk::Track *track, const Trk::TrackSummary* summary )
{
    const Trk::Perigee *perigee = track->perigeeParameters();
    float eta = perigee->eta();
    float phi = perigee->parameters()[Trk::phi0];

    /// Map of all extended tracks
    m_ID_COMB_eta_phi->Fill( eta, phi );

    m_ID_COMB_d0->Fill(perigee->parameters()[Trk::d0]);
    m_ID_COMB_z0->Fill(perigee->parameters()[Trk::z0]);
    m_ID_COMB_qoverp->Fill( perigee->parameters()[Trk::qOverP] *1000.0 );
    m_ID_COMB_pt->Fill(perigee->pT()/1000.0);
    
    if ( (summary->get(Trk::expectBLayerHit) == 1) && !(summary->get(Trk::numberOfBLayerHits)) )
    {
	m_ID_COMB_noBLayerHitExpected_eta_phi->Fill( eta, phi );
	m_ID_COMB_noBLayerHitExpectedRatio_eta_phi->Fill( eta, phi );
	m_PIX_ntrk_noBLhits_LB->Fill( m_manager->lumiBlockNumber() );
    }
    
    if ( !(summary->get(Trk::numberOfBLayerHits)) )
	m_ID_COMB_noBLayerHit_eta_phi->Fill( eta, phi );
	m_ID_COMB_noBLayerHitRatio_eta_phi->Fill( eta, phi );
    
    // Eta-phi of tracks with at least one pixel hit
    if ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) >= 1 ){
	m_PIX_ntrk_LB->Fill( m_manager->lumiBlockNumber() );
	m_COMB_goodPixTrk_eta_phi->Fill( eta, phi );
	m_COMB_goodPixTrkRatio_eta_phi->Fill( eta, phi );
    }
    
    // Eta-phi of tracks with at least six SCT hits
    if ( summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors) >= 6  ){	
	m_SCT_ntrk_LB->Fill( m_manager->lumiBlockNumber() );
	m_COMB_goodSctTrk_eta_phi->Fill( eta, phi );
	m_COMB_goodSctTrkRatio_eta_phi->Fill( eta, phi );
    }
    
    // Eta-phi of tracks with at least twenty trt hits
    if ( summary->get(Trk::numberOfTRTHits) >= 20 ){
	m_TRT_ntrk_LB->Fill( m_manager->lumiBlockNumber() );
	m_COMB_goodTrtTrk_eta_phi->Fill( eta, phi );
    }
    
    // Eta-phi of tracks with at least one pixel hit and at least nine silicon hits
    if (summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) >= 1 && 
	summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) + 
	summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors) >= 9 )
    {
	m_COMB_goodEleMTrk_eta_phi->Fill( eta, phi );
    }
    
    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 ){
	m_ID_COMB_chi2->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
    }

    // Tracks with SCT hits and no Pixel hits
    if ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) == 0 )
    {
	m_ID_COMB_noPIXhits_eta_phi->Fill(eta, phi);
	m_ID_COMB_noPIXhitsRatio_eta_phi->Fill(eta, phi);
	m_PIX_ntrk_noPIXhits_LB->Fill( m_manager->lumiBlockNumber() );
    }
    
    // Tracks with Pixel hits and no SCT hits
    if ( summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors) == 0 )
	m_ID_COMB_noSCThits_eta_phi->Fill(eta, phi);
    
    // Tracks with SCT and Pixel and no TRT extension
    if ( summary->get(Trk::numberOfTRTHits) == 0 )
	m_ID_COMB_noTRText_eta_phi->Fill(eta, phi );
    
    // Tracks passing b-tagging quality cuts
    if ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) >= 2 &&
	 summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors) + summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) >= 7 &&
	 summary->get(Trk::numberOfBLayerHits) >= 1 ) 
    {
	m_ID_COMB_btagqual_eta_phi->Fill(eta, phi );
    }
    
    return;    
}

void InDetGlobalTrackMonTool::FillForwardTracks( const Trk::Track *track, const Trk::TrackSummary* summary )
{
    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( perigee )
    {
	float eta = perigee->eta();
	float phi = perigee->parameters()[Trk::phi0];
	
	if ( eta > 0. )
	{
	    m_FORW_forwardTracklets_pos_eta_phi->Fill( eta, phi );
	    m_Trk_nPIXhits_FA->Fill ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) );
	}    
	else
	{
	    m_FORW_forwardTracklets_neg_eta_phi->Fill( eta, phi );
	    m_Trk_nPIXhits_FC->Fill ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) );
	}

	m_FORW_qoverp->Fill( perigee->parameters()[Trk::qOverP] *1000.0 );
    }
    
    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 )
	m_FORW_chi2->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
    
    return;    
}

void InDetGlobalTrackMonTool::FillHitMaps( const Trk::Track *track )
{
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
    if ( trackStates == 0 ) return;
    
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = trackStates->end();
    for (;it!=it_end; ++it) {
	const Trk::TrackStateOnSurface* trackState=(*it);
	
	if (trackState == 0) continue;
	
	//TRT
	const InDet::TRT_DriftCircleOnTrack *trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trackState->measurementOnTrack());
	if ( !trtcircle ) continue;

	switch ( m_trtID->barrel_ec( trtcircle->identify() ) ) {
	case -2:
	    m_ID_hitmap_x_y_ecc->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	    break;
	case -1:
	    m_ID_hitmap_x_y->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	    break;
	case 1:
	    m_ID_hitmap_x_y->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	    break;
	case 2:
	    m_ID_hitmap_x_y_eca->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	    break;
	}
	
	//SILICON (SCT + Pixel)
	const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>( trackState->measurementOnTrack() );
	if ( !clus ) continue;
	
	const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
	if (RawDataClus==0) continue;
	
	if ( RawDataClus->detectorElement()->isSCT() ) {
	    switch ( m_sctID->barrel_ec( RawDataClus->identify() ) ) {
	    case -2:
		m_ID_hitmap_x_y_ecc->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    case 0:
		m_ID_hitmap_x_y->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    case 2:
		m_ID_hitmap_x_y_eca->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    }
	    
	}else{
	    switch ( m_pixelID->barrel_ec( RawDataClus->identify() ) ) {
	    case -2:
		m_ID_hitmap_x_y_ecc->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    case 0:
		m_ID_hitmap_x_y->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    case 2:
		m_ID_hitmap_x_y_eca->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		break;
	    }
	}
    }
    return;
}


void InDetGlobalTrackMonTool::FillHoles( const Trk::Track * track, const Trk::TrackSummary* summary )
{
    unsigned int pixelh = summary->get(Trk::numberOfPixelHoles) >= 0 ? summary->get(Trk::numberOfPixelHoles) : 0;
    unsigned int scth   = summary->get(Trk::numberOfSCTHoles) >= 0 ? summary->get(Trk::numberOfSCTHoles) : 0;
    unsigned int trth   = summary->get(Trk::numberOfTRTHoles) >= 0 ? summary->get(Trk::numberOfTRTHoles) : 0;
    
    // Gaetano Added : 
    pixel_holes->Fill(summary->get(Trk::numberOfPixelHoles));
    sct_holes->Fill(summary->get(Trk::numberOfSCTHoles));
    trt_holes->Fill(summary->get(Trk::numberOfTRTHoles));
    
    // Filling Combined Holes and Excluding # holes = -1 case Tracks on surface does not exist		
    if (summary->get(Trk::numberOfSCTHoles) >=0 ){
	comb_holes->Fill(summary->get(Trk::numberOfSCTHoles));
	scth=summary->get(Trk::numberOfSCTHoles);
    }
    if (summary->get(Trk::numberOfTRTHoles)>=0)	{
	comb_holes->Fill(summary->get(Trk::numberOfTRTHoles));
	trth=summary->get(Trk::numberOfTRTHoles);
    }
    if (summary->get(Trk::numberOfPixelHoles)>=0) {
	comb_holes->Fill(summary->get(Trk::numberOfPixelHoles));
	pixelh=summary->get(Trk::numberOfPixelHoles);
    }
    silicon_vs_trt->Fill(pixelh+scth,trth);
    sct_vs_pixels->Fill(scth,pixelh);	
	    
    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 ){
	holes_quality->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF(),summary->get(Trk::numberOfSCTHoles)+summary->get(Trk::numberOfTRTHoles)+summary->get(Trk::numberOfPixelHoles));
	
	holes_quality_profile->Fill(scth+trth+pixelh,track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
    }
    // Filling Number of holes vs number of hits for tracks with at least a hole. 			
    if (scth >0 || trth >0 || pixelh >0) {
	m_holes_hits->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+trth+pixelh);
	m_holesvshits->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+trth+pixelh);
    }
    
    if (m_DoHoles_Search) {		
	const DataVector<const Trk::TrackStateOnSurface>* holesOnTrack = m_holes_search_tool->getHolesOnTrack(*track,track->info().particleHypothesis());
	
	// loop over holes
	if (!holesOnTrack) {
	    msg(MSG::WARNING) << "Got no holes on track" << endreq;
	} else {	
	    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=holesOnTrack->begin();
		 it!=holesOnTrack->end();
		 it++) {
		if (!(*it)) {
		    msg(MSG::WARNING) << "TrackStateOnSurface from hole search tool == Null" << endreq;
		    continue;
		}
		// Here The X Y Z of Holes	
		const Trk::TrackParameters *clus =(*it)->trackParameters() ;
		if (clus){						
		    m_HolesMAP_XY->Fill(clus->position()[0],clus->position()[1]);
		    m_HolesMAP_ZX->Fill( clus->position()[2], clus->position()[0]);
		    m_HolesMAP_ZR->Fill(clus->position()[2], sqrt( pow( clus->position()[0], 2) + pow( clus->position()[1], 2) ));	
		    
		}	
		
		// Here The Perigee Parameters of Holes
		const Trk::Perigee *perigee2 = dynamic_cast<const Trk::Perigee *>(track->perigeeParameters());	
		
		m_holes_eta_phi_n->Fill( perigee2->eta(),perigee2->parameters()[Trk::phi],scth+pixelh/(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors)));
		
		m_holes_eta_phi->Fill( perigee2->eta(),perigee2->parameters()[Trk::phi]);
		m_holes_eta_pt->Fill(perigee2->eta(),perigee2->pT()/1000.0,scth+pixelh);
		m_holes_phi_pt->Fill(perigee2->parameters()[Trk::phi],perigee2->pT()/1000.0,scth+pixelh);
	
		// Filling holes vs hits in eta bins	
		if (scth >0 || trth >0 || pixelh >0){
		    // Filling ECA
		    if  (perigee2->eta()>=-2.5 && perigee2->eta()<=-1.5) {
			m_holesvshits_ECA->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
		    }
			    
		    if  (perigee2->eta()>=-1.5 && perigee2->eta()<=1.5) {
			m_holesvshits_BA->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
		    }
			    
			    
		    if  (perigee2->eta()>=1.5 && perigee2->eta()<=2.5) {
			m_holesvshits_ECC->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
		    }
		}
	    }
	}
	delete holesOnTrack;
    }
    return;
}
