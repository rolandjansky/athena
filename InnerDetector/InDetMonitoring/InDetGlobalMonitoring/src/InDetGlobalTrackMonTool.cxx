/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

//Local
#include "InDetGlobalTrackMonTool.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandle.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "TH1I.h"
#include "LWHists/TProfile_LW.h"
#include "TProfile2D.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


//Standard c++
#include <vector>
#include <string>
#include <cmath>


InDetGlobalTrackMonTool::InDetGlobalTrackMonTool( const std::string & type,
						  const std::string & name,
						  const IInterface* parent)
    : ManagedMonitorToolBase(type, name, parent),
      m_baseline_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
      m_tight_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
      m_doHolePlots(false),
      m_DoHoles_Search(false),
      m_doHitMaps(false),
      m_doTide(true),
      m_doTideResiduals(false),
      m_doForwardTracks(m_dataType == AthenaMonManager::collisions),
      m_doIBL(true),
      m_nBinsEta( 50 ),
      m_nBinsPhi( 50 ),
      m_trackBin( 100 ),
      m_c_etaRange(2.5),
      m_c_etaTrackletsMin(2.4),
      m_c_etaTrackletsMax(2.7),
      m_c_etaRangeTRT(2.0),
      m_c_range_LB(3000),
      m_c_detector_labels{ "IBL", "PIX", "SCT", "TRT" },
      m_IBLParameterSvc("IBLParameterSvc",name),
      m_holes_search_tool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
      m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
      m_iUpdator("Trk::KalmanUpdator"),
      m_sct_holes(nullptr),
      m_trt_holes(nullptr),
      m_pixel_holes(nullptr),
      m_comb_holes(nullptr),
      m_silicon_vs_trt(nullptr),
      m_sct_vs_pixels(nullptr),
      m_holes_quality(nullptr),
      m_holes_quality_profile(nullptr),
      m_Trk_eta_phi_Base(nullptr),
      m_Trk_eta_phi_Tight(nullptr),
      m_Trk_eta_phi_Tight_ratio(nullptr),
      m_Trk_eta_phi_noIBLhit_ratio(nullptr),
      m_Trk_eta_phi_noBLhit_ratio(nullptr),
      m_Trk_eta_phi_noTRText_ratio(nullptr),
      m_Trk_nBase_LB(nullptr),
      m_Trk_nTight_LB(nullptr),
      m_Trk_noIBLhits_LB(nullptr),
      m_Trk_noBLhits_LB(nullptr),
      m_Trk_noTRText_LB(nullptr),
      m_Trk_noIBLhits_frac_LB(nullptr),
      m_Trk_noBLhits_frac_LB(nullptr),
      m_Trk_noTRText_frac_LB(nullptr),
      m_trtID(nullptr),
      m_sctID(nullptr),
      m_pixelID(nullptr),
      m_holes_eta_phi(nullptr),
      m_holes_eta_pt(nullptr),
      m_holes_phi_pt(nullptr),
      m_holes_eta_phi_n(nullptr),
      m_holes_hits(nullptr),
      m_holesvshits(nullptr),
      m_holesvshits_ECA(nullptr),
      m_holesvshits_ECC(nullptr),
      m_holesvshits_BA(nullptr),
      m_ID_hitmap_x_y(nullptr),
      m_ID_hitmap_x_y_eca(nullptr),
      m_ID_hitmap_x_y_ecc(nullptr),
      m_HolesMAP_XY(nullptr),
      m_HolesMAP_ZX(nullptr),
      m_HolesMAP_ZR(nullptr),
      m_trk_hits_eta_phi{nullptr},
      m_trk_disabled_eta_phi{nullptr},
      m_trk_hits_LB{nullptr},
      m_trk_shared_pix_eta_phi(nullptr),
      m_trk_split_pix_eta_phi(nullptr),
      m_trk_shared_sct_eta_phi(nullptr),
      m_trk_holes_pix_eta_phi(nullptr),
      m_trk_holes_sct_eta_phi(nullptr),
      m_Trk_FORW_FA_eta_phi(nullptr),
      m_Trk_FORW_FC_eta_phi(nullptr),
      m_Trk_FORW_qoverp(nullptr),
      m_Trk_FORW_chi2(nullptr),
      m_Trk_FORW_FA_nPIXhits(nullptr),
      m_Trk_FORW_FC_nPIXhits(nullptr)
{
    declareProperty("DoHoleSearch",m_DoHoles_Search,"Write hole data?");
    declareProperty("DoTide",m_doTide,"Make TIDE plots?");
    declareProperty("DoTideResiduals",m_doTideResiduals,"Make TIDE residual plots?");
    declareProperty("HoleSearchTool", m_holes_search_tool,"Tool to search for holes on track");
    declareProperty("UpdatorTool"                  , m_iUpdator);
    declareProperty("DoHitMaps", m_doHitMaps,"Produce hit maps?");
    declareProperty("DoForwardTracks", m_doForwardTracks,"Run over forward tracks?");
    declareProperty("DoIBL", m_doIBL,"IBL present?");
    declareProperty("trackMax",m_trackMax,"Maximum number of tracks in track histograms");
    declareProperty("trackBin",m_trackBin,"Number of bins for the track histograms");
    declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator);

    declareProperty( "Baseline_SelTool", m_baseline_selTool );
    declareProperty( "Tight_SelTool", m_tight_selTool );
}

StatusCode InDetGlobalTrackMonTool::initialize() {
  StatusCode sc;

  // If any of the ID helpers are not found then we don't make the hit maps
  m_trtID = nullptr;
  if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) {
      ATH_MSG_DEBUG("Could not get TRT ID helper");
      m_doHitMaps = false;
  }

  m_sctID = nullptr;
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
      ATH_MSG_DEBUG("Could not get SCT ID helper");
      m_doHitMaps = false;
  }

  m_pixelID = nullptr;
  if (detStore()->retrieve(m_pixelID, "PixelID").isFailure()) {
      ATH_MSG_DEBUG("Could not get Pixel ID helper");
      m_doHitMaps = false;
  }

  if (m_IBLParameterSvc.retrieve().isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve " << m_IBLParameterSvc << endmsg;
      return StatusCode::FAILURE;
  } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_IBLParameterSvc << endmsg;
  }

  if ( m_residualPullCalculator.retrieve().isFailure() )
  {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve " << m_residualPullCalculator << endmsg;
      return StatusCode::FAILURE;
  } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_residualPullCalculator << endmsg;
  }

  ATH_CHECK( m_iUpdator.retrieve() );


  ATH_CHECK ( m_trackToVertexIPEstimator.retrieve());


  m_doIBL = m_IBLParameterSvc->containsIBL();

  ATH_CHECK( m_baseline_selTool.retrieve() );
  ATH_CHECK( m_tight_selTool.retrieve() );
  if (m_holes_search_tool.empty() && m_DoHoles_Search) {
     ATH_MSG_FATAL("Hole search enabled but HoleSearchTool not configured.");
  }
  ATH_CHECK( m_holes_search_tool.retrieve( DisableTool{!m_DoHoles_Search} ));

  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  ATH_CHECK( m_TrackParticleName.initialize() );
  ATH_CHECK( m_ForwardTrackParticleName.initialize() );
  ATH_CHECK( m_JetsName.initialize() );
  ATH_CHECK( m_vertexKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalTrackMonTool::bookHistograms()
{
    Interval_t detailsInterval = ( AthenaMonManager::environment() != AthenaMonManager::online ) ? lowStat : run;

    registerManHist( m_Trk_Base, "InDetGlobal/Track", detailsInterval,
		     "nCOMBtrks", "Track multiplicity (baseline tracks)",
		     m_trackBin, 0.5, m_trackMax+0.5,
		     "Track multiplicity" ).ignore();

    // Eta-phi maps
    registerManHist( m_Trk_eta_phi_Base, "InDetGlobal/Track", detailsInterval,
		     "Trk_Base_eta_phi","Distribution of eta vs phi for combined tracks passing Loose Primary selection",
                     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
                     m_nBinsPhi, -M_PI, M_PI,
                     "eta", "#phi_{0}" ).ignore();

    registerManHist( m_Trk_eta_phi_Tight, "InDetGlobal/Track", detailsInterval,
		     "Trk_Tight_eta_phi","Distribution of eta vs phi for combined tracks passing monitoring selection",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi, -M_PI, M_PI,
		     "eta", "#phi_{0}" ).ignore();

    registerManHist( m_Trk_eta_phi_Tight_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_Tight_eta_phi_ratio","Distribution of eta vs phi for combined tracks passing Tight selection",
                     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
                     m_nBinsPhi, -M_PI, M_PI,
                     "eta", "#phi_{0}" ).ignore();

    registerManHist( m_Trk_eta_phi_noTRText_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_eta_phi_ratio","Distribution of eta vs phi for combined tracks with no TRT extension",
		     20, -m_c_etaRangeTRT, m_c_etaRangeTRT,
		     m_nBinsPhi, -M_PI, M_PI,
		     "eta", "#phi_{0}" ).ignore();

    registerManHist( m_Trk_eta_phi_noBLhit_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhit_eta_phi_ratio","Eta-phi of tracks with no b-layer hit but a hit is expected, ratio to total tracks",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}").ignore();

    if ( m_doIBL )
    {
	registerManHist( m_Trk_eta_phi_noIBLhit_ratio, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhit_eta_phi_ratio","Eta-phi of tracks with no IBL hit but a hit is expected, ratio to total tracks",
			 m_nBinsEta, -m_c_etaRange, m_c_etaRange,
			 m_nBinsPhi, -M_PI, M_PI,
			 "#eta", "#phi_{0}").ignore();
    }

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

    for (unsigned int i = ( (m_doIBL) ? 0 : 1) ; i < m_trk_hits_eta_phi.size(); i++ )
    {
	registerManHist( m_trk_hits_eta_phi[i],  "InDetGlobal/Hits", detailsInterval,
			 "Trk_n"+m_c_detector_labels[i]+"hits_eta_phi","Number of "+m_c_detector_labels[i]+" hits per track, eta-phi profile",
			 m_nBinsEta, -m_c_etaRange, m_c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi" ).ignore();

	registerManHist( m_trk_hits_LB[i], "InDetGlobal/Track", detailsInterval,
			 "trk_n"+m_c_detector_labels[i]+"hits_LB","Average number of " + m_c_detector_labels[i] + " hits by LB",
			 m_c_range_LB,0,m_c_range_LB,
			 "LB #", "Average number of hits in LB").ignore();
    }

    for (unsigned int i = 1; i < m_trk_disabled_eta_phi.size(); i++ )
    {
	registerManHist( m_trk_disabled_eta_phi[i],  "InDetGlobal/Hits", detailsInterval,
			 "Trk_n"+m_c_detector_labels[i]+"disabled_eta_phi","Number of "+m_c_detector_labels[i]+" disabled detector elements per track, eta-phi profile",
			 m_nBinsEta, -m_c_etaRange, m_c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi" ).ignore();
    }

    registerManHist( m_trk_shared_pix_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nPixShared_eta_phi","Number of Pixel shared hits per track, eta-phi profile",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_split_pix_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nPixSplit_eta_phi","Number of Pixel split hits per track, eta-phi profile",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_jetassoc_d0_reso_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_d0_dr", "IP resolution  per ghost associated track vs #DeltaR of track and jet",
		     20, 0, 0.4, "#Delta R", "Fraction" ).ignore();
    registerManHist( m_trk_jetassoc_z0_reso_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_z0_dr", "IP resolution  per ghost associated track vs #DeltaR of track and jet",
		     20, 0, 0.4, "#Delta R", "Fraction" ).ignore();

    registerManHist( m_trk_jetassoc_split_pix_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_split_pix_dr", "Fraction of split Pixel hits per ghost associated track vs #DeltaR of track and jet",
		     20, 0, 0.4, "#Delta R", "Fraction" ).ignore();

    registerManHist( m_trk_jetassoc_shared_pix_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_shared_pix_dr", "Fraction of shared Pixel hits  per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Fraction" ).ignore();

    registerManHist( m_trk_jetassoc_res_pix_l0_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_l0_dr", "Average Pixel L0 residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l1_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_l1_dr", "Average Pixel L1 residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l2_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_l2_dr", "Average Pixel L2 residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l3_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_l3_dr", "Average Pixel L3 residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();

    registerManHist( m_trk_jetassoc_res_pix_eca_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_eca_dr", "Average Pixel ECA residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_ecc_x_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_x_ecc_dr", "Average Pixel ECC residual (X) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();

    registerManHist( m_trk_jetassoc_res_pix_l0_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_l0_dr", "Average Pixel L0 residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l1_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_l1_dr", "Average Pixel L1 residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l2_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_l2_dr", "Average Pixel L2 residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_l3_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_l3_dr", "Average Pixel L3 residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();

    registerManHist( m_trk_jetassoc_res_pix_eca_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_eca_dr", "Average Pixel ECA residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();
    registerManHist( m_trk_jetassoc_res_pix_ecc_y_dr, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_res_pix_y_ecc_dr", "Average Pixel ECC residual (Y) per ghost associated track vs #DeltaR of jet",
		     20, 0, 0.4, "#DeltaR", "Biased Residual" ).ignore();


    registerManHist( m_trk_jetassoc_ip_reso_lb, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_ip_reso_lb", "IP resolution  per ghost associated track vs LB",
		     m_c_range_LB, 0, m_c_range_LB, "#Delta R", "Fraction" ).ignore();

    registerManHist( m_trk_jetassoc_split_pix_lb, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_split_pix_lb", "Fraction of split Pixel hits vs LB",
		     m_c_range_LB, 0, m_c_range_LB, "#Delta R", "Fraction" ).ignore();

    registerManHist( m_trk_jetassoc_shared_pix_lb, "InDetGlobal/Hits", detailsInterval,
		     "Trk_jetassoc_shared_pix_lb", "Fraction of shared Pixel hits vs LB",
		     m_c_range_LB, 0, m_c_range_LB, "#DeltaR", "Fraction" ).ignore();

    registerManHist( m_trk_shared_sct_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nSCTShared_eta_phi","Number of SCT shared hits per track, eta-phi profile",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_holes_pix_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nPixHoles_eta_phi","Number of Pixel holes per track, eta-phi profile",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_holes_sct_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nSCTHoles_eta_phi","Number of SCT holes per track, eta-phi profile",
		     m_nBinsEta, -m_c_etaRange, m_c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    // Forward Pixel tracks
    if ( m_doForwardTracks )
    {
	registerManHist( m_Trk_FORW_FA_eta_phi, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_FA_eta_phi", "Eta-phi for pixel tracklets in the forward region (positive eta)",
			 5, m_c_etaTrackletsMin, m_c_etaTrackletsMax,
			 m_nBinsPhi, -M_PI, M_PI,
			 "#eta", "#phi_{0}" ).ignore();

	registerManHist( m_Trk_FORW_FC_eta_phi, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_FC_eta_phi", "Eta-phi for pixel tracklets in the forward region (negative eta)",
			 5, -m_c_etaTrackletsMax, -m_c_etaTrackletsMin,
			 m_nBinsPhi, -M_PI, M_PI,
			 "#eta", "#phi_{0}" ).ignore();

	registerManHist( m_Trk_FORW_chi2, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_chi2", "#chi^{2}/DoF of reconstructed forward tracklets",
			 80, -0., 20.,
			 "#chi^{2}/DoF").ignore();

	registerManHist( m_Trk_FORW_qoverp, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_qoverp", "Distribution of qoverp (forward Pixel tracklets)",
			 400, -2.5, 2.5,
			 "Inverse momentum [GeV^{-1}]" ).ignore();

	registerManHist( m_Trk_FORW_FA_nPIXhits, "InDetGlobal/Hits", detailsInterval,
			 "Trk_FORW_nPIXhits_FA","Number of Pixel hits per track (Forward tracks FA)",
			 5, 0.5, 5.5,
			 "# of hits per track" ).ignore();

	registerManHist( m_Trk_FORW_FC_nPIXhits, "InDetGlobal/Hits", detailsInterval,
			 "Trk_FORW_nPIXhits_FC","Number of Pixel hits per track (Forward tracks FC)",
			 5, 0.5, 5.5,
			 "# of hits per track" ).ignore();
    }

    // LB histograms
    registerManHist( m_Trk_nBase_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_nBase_LB","Average number of baseline tracks per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Average number of loose primary tracks per event in LB").ignore();

    registerManHist( m_Trk_nTight_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_nTight_LB","Average number of tight tracks per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Average number of tight tracks per event in LB").ignore();

    if ( m_doIBL )
    {
	registerManHist( m_Trk_noIBLhits_LB, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhits_LB","Average number of tracks with missing IBL hit per event in LB",
			 m_c_range_LB,0,m_c_range_LB,
			 "LB #", "Average number of tracks with missing IBL hit per event in LB").ignore();
	registerManHist( m_Trk_noIBLhits_frac_LB, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhits_frac_LB","Fraction of tracks with missing IBL hit per event in LB",
			 m_c_range_LB,0,m_c_range_LB,
			 "LB #", "Fraction of tracks with missing IBL hit per event in LB").ignore();
    }

    registerManHist( m_Trk_noBLhits_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhits_LB","Average number of tracks with missing b-layer hit per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Average number of tracks with missing b-layer hit per event in LB").ignore();

    registerManHist( m_Trk_noBLhits_frac_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhits_frac_LB","Fraction of tracks with missing b-layer hit per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Fraction of tracks with missing b-layer hit per event in LB").ignore();

    registerManHist( m_Trk_noTRText_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_LB","Average number of tracks without TRT extension per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Average number of tracks with without TRT extension per event in LB").ignore();
    registerManHist( m_Trk_noTRText_frac_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_frac_LB","Fraction of tracks without TRT extension per event in LB",
		     m_c_range_LB,0,m_c_range_LB,
		     "LB #", "Fraction of tracks with without TRT extension per event in LB").ignore();

    // Holes
    if ( m_doHolePlots )
    {
	registerManHist( m_sct_holes, "InDetGlobal/Hits", detailsInterval,
			 "sct_holes", "Distribution of SCT Holes ",
			 104,-3.5,100.5,
			 "Number of SCT holes").ignore();

	registerManHist( m_trt_holes, "InDetGlobal/Hits", detailsInterval,
			 "trt_holes", "Distribution of TRT Holes ",
			 104,-3.5,100.5,
			 "Number of TRT holes").ignore();

	registerManHist( m_pixel_holes, "InDetGlobal/Hits", detailsInterval,
			 "pixel_holes", "Distribution of Pixel Holes ",
			 104,-3.5,100.5,
			 "Number of Pixel holes").ignore();

	registerManHist( m_comb_holes, "InDetGlobal/Hits", detailsInterval,
			 "comb_holes", "Distribution of Combined Holes ",
			 104,-3.5,100.5,
			 "Total number of holes").ignore();

	registerManHist( m_silicon_vs_trt, "InDetGlobal/Hits", detailsInterval,
			 "silicon_vs_trt", "Silicon vs TRT holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "Silicon Combined holes", "TRT holes").ignore();

	registerManHist( m_sct_vs_pixels, "InDetGlobal/Hits", detailsInterval,
			 "sct_vs_pixels", "SCT vs Pixels holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "SCT", "Pixels").ignore();

	registerManHist( m_holes_quality, "InDetGlobal/Hits", detailsInterval,
			 "holes_quality", "Number of holes/track vs #chi^{2}/ndf",
			 160,-0.5,15.5,
			 104,-3.5,100.5,
			 "#chi^{2}/ndf", "Combined Holes").ignore();

	registerManHist( m_holes_eta_phi, "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_phi", "Holes Map #eta #phi",
			 m_nBinsEta,-m_c_etaRange,m_c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();


	registerManHist( m_holes_eta_pt,  "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_pt", "Holes #eta vs p_{t}",
			 m_nBinsEta,-m_c_etaRange,m_c_etaRange,
			 30,-0.,30.,
			 "#eta", "#p_{t}").ignore();

	registerManHist( m_holes_phi_pt,  "InDetGlobal/Hits", detailsInterval,
			 "holes_phi_pt", "Holes #phi vs p_{t}",
			 m_nBinsPhi,-M_PI,M_PI,
			 30,-0.,30.,
			 "#phi", "#p_{t}").ignore();

	registerManHist( m_holes_eta_phi_n,  "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_phi_n", "Holes Map (Norm) #eta #phi",
			 m_nBinsEta,-m_c_etaRange,m_c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();

	registerManHist( m_holes_quality_profile,  "InDetGlobal/Hits", detailsInterval,
			 "holes_quality_profile", "#chi^{2}/ndf vs Number of holes/track profile",
			 104,-3.5,100.5,
			 "<#chi^{2}/ndf> / comb hole", "Combined Holes").ignore();
	m_holes_quality_profile->SetErrorOption("S");
	m_holes_quality_profile->SetMaximum( 1000 );
	m_holes_quality_profile->SetMinimum( 0 );


	registerManHist( m_holes_hits,  "InDetGlobal/Hits", detailsInterval,
			 "holes_hits", "Number of holes vs number of hits per track Combined",
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb. holes/track").ignore();
	m_holes_hits->SetErrorOption("S");
	m_holes_hits->SetMaximum( 1000 );
	m_holes_hits->SetMinimum( -10 );

	registerManHist( m_holesvshits,  "InDetGlobal/Hits", detailsInterval,
			 "holesvshits","Number of Holes/track vs Number of Hits/track if holes >0 ",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();

	registerManHist( m_holesvshits_ECA,  "InDetGlobal/Hits", detailsInterval,
			 "holesvshits_ECA", "Number of Holes/track vs Number of Hits/track if holes >0 EndCap A",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();

	registerManHist( m_holesvshits_ECC,  "InDetGlobal/Hits", detailsInterval,
			 "holesvshits_ECC", "Number of Holes/track vs Number of Hits/track if holes >0 EndCap C",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();

	registerManHist( m_holesvshits_BA,  "InDetGlobal/Hits", detailsInterval,
			 "holesvshits_BA", "Number of Holes/track vs Number of Hits/track if holes >0 BARREL",
			 101,-0.5,100.5,
			 101,-0.5,100.5,
			 "nb. of hits/track", "nb holes/track").ignore();

	registerManHist( m_HolesMAP_XY,  "InDetGlobal/Hits", detailsInterval,
			 "HolesMAP_XY", "Map of ID holes x vs y (mm)",
			 400,-1100,1100,
			 400,-1100,1100,
			 "X [mm]", "Y [mm]").ignore();

	registerManHist( m_HolesMAP_ZR,  "InDetGlobal/Hits", detailsInterval,
			 "HolesMAP_ZR", "Map of ID holes  z vs r (mm)",
			 3000,-3100,3100,
			 1100,0,1100,
			 "Z [mm]", "R [mm]").ignore();

	registerManHist( m_HolesMAP_ZX,  "InDetGlobal/Hits", detailsInterval,
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
    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode InDetGlobalTrackMonTool::fillHistograms()
{
    SG::ReadHandle<xAOD::TrackParticleContainer> trackParticles(m_TrackParticleName);
    if ( !trackParticles.isValid() )
    {
	ATH_MSG_WARNING( "Failed to retrieve TrackParticles in StoreGate " + m_TrackParticleName.key() );
	return StatusCode::SUCCESS;
    }

    int nBase = 0;
    int nTight = 0;
    int nNoIBL = 0;
    int nNoBL = 0;
    int nNoTRText = 0;

    uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

    for (const auto& trackPart: *trackParticles)
    {
	const Trk::Track * track = trackPart->track();
	if ( !track || track->perigeeParameters() == nullptr )
	{
	    ATH_MSG_DEBUG( "NULL track pointer in collection" );
	    continue;
	}

	// Skip tracks that are not inside out
        if ( ( m_dataType == AthenaMonManager::collisions || m_dataType == AthenaMonManager::userDefined )
             && ! ( track->info().patternRecoInfo( Trk::TrackInfo::SiSPSeededFinder ) ||
		    track->info().patternRecoInfo( Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon ) ) )
	    continue;

	if ( ! m_baseline_selTool->accept(*track) )
	    continue;

	nBase++;

	FillHits( trackPart );
	FillEtaPhi( trackPart );

	if ( m_doIBL )
	{
	    int expInHit = trackPart->summaryValue(iSummaryValue, xAOD::expectInnermostPixelLayerHit) ? iSummaryValue : 0;
	    int nInHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;

	    if ( expInHit==1 && nInHits==0 )
	    {
		nNoIBL++;
		m_Trk_noIBLhits_frac_LB->Fill( AthenaMonManager::lumiBlockNumber(), 1 );
	    }
	    else
	    {
		m_Trk_noIBLhits_frac_LB->Fill( AthenaMonManager::lumiBlockNumber(), 0 );
	    }
	}

	xAOD::SummaryType expNInHitField = ( m_doIBL ) ? xAOD::expectNextToInnermostPixelLayerHit : xAOD::expectInnermostPixelLayerHit;
	xAOD::SummaryType nNInHitField = ( m_doIBL ) ? xAOD::numberOfNextToInnermostPixelLayerHits : xAOD::numberOfInnermostPixelLayerHits;
	int expNInHit = trackPart->summaryValue(iSummaryValue, expNInHitField) ? iSummaryValue : 0;
	int nNInHits = trackPart->summaryValue(iSummaryValue, nNInHitField) ? iSummaryValue : 0;

	if ( expNInHit==1 && nNInHits==0 )
	{
	    nNoBL++;
	    m_Trk_noBLhits_frac_LB->Fill( AthenaMonManager::lumiBlockNumber(), 1 );
	}
	else
	{
	    m_Trk_noBLhits_frac_LB->Fill( AthenaMonManager::lumiBlockNumber(), 0 );
	}

	int nTRTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
	int nTRTOutliers = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTOutliers ) ? iSummaryValue : 0;

	if ( (nTRTHits + nTRTOutliers) == 0 )
	{
	    nNoTRText++;
	    m_Trk_noTRText_frac_LB->Fill(AthenaMonManager::lumiBlockNumber(), 1);
	}
	else
	{
	    m_Trk_noTRText_frac_LB->Fill(AthenaMonManager::lumiBlockNumber(), 0);
	}

	if ( m_tight_selTool->accept(*track) )
	{
	    nTight++;
	}

	if ( m_doHitMaps )
	{
	    FillHitMaps( trackPart );
	}

	if ( m_doHolePlots )
	{
	    FillHoles( trackPart );
	}

	m_Trk_Base->Fill( nBase );

	m_Trk_nBase_LB->Fill( AthenaMonManager::lumiBlockNumber(), nBase );
	m_Trk_nTight_LB->Fill( AthenaMonManager::lumiBlockNumber(), nTight );
	if ( m_doIBL ) m_Trk_noIBLhits_LB->Fill( AthenaMonManager::lumiBlockNumber(), nNoIBL );

	m_Trk_noBLhits_LB->Fill( AthenaMonManager::lumiBlockNumber(), nNoBL );
	m_Trk_noTRText_LB->Fill( AthenaMonManager::lumiBlockNumber(), nNoTRText );
    }

    if ( m_doTide ) FillTIDE();

    if ( m_doForwardTracks )
    {
	SG::ReadHandle<xAOD::TrackParticleContainer> forwardTrackParticles(m_ForwardTrackParticleName);
	if ( forwardTrackParticles.isPresent() )
	{
	    if ( !forwardTrackParticles.isValid() )
	    {
	        ATH_MSG_DEBUG( "No forward TrackParticles in StoreGate " + m_ForwardTrackParticleName.key() );
		return StatusCode::SUCCESS;
	    }

	    for (const auto& trackPart: *forwardTrackParticles) {
		FillForwardTracks( trackPart );
	    }
	}
    }

    return StatusCode::SUCCESS;
}


StatusCode InDetGlobalTrackMonTool::procHistograms()
{
    return StatusCode::SUCCESS;
}

void InDetGlobalTrackMonTool::FillHits( const xAOD::TrackParticle *trackPart )
{
    uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

    const Trk::Track * track = trackPart->track();
    if( !track) return;

    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( !perigee )
	return;

    if ( m_doIBL )
    {
	int nInPixHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;
	m_trk_hits_eta_phi[0]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nInPixHits );
	m_trk_hits_LB[0]->Fill( AthenaMonManager::lumiBlockNumber(), nInPixHits );
    }

    int nPixHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;
    int nPixDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;
    int pixHits = nPixHits + nPixDead;
    int nPixSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSharedHits) ? iSummaryValue : 0;
    int nPixHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHoles) ? iSummaryValue : 0;
    int nPixSplit = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelSplitHits) ? iSummaryValue : 0;

    m_trk_hits_eta_phi[1]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], pixHits );
    m_trk_disabled_eta_phi[1]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nPixDead );
    m_trk_hits_LB[1]->Fill( AthenaMonManager::lumiBlockNumber(), pixHits );

    m_trk_shared_pix_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nPixSharedHits );
    m_trk_holes_pix_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nPixHoles );
    m_trk_split_pix_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nPixSplit );

    int nSCTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHits) ? iSummaryValue : 0;
    int nSCTDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTDeadSensors) ? iSummaryValue : 0;
    int sctHits = nSCTHits + nSCTDead;
    int nSCTSharedHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTSharedHits) ? iSummaryValue : 0;
    int nSCTHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHoles) ? iSummaryValue : 0;

    m_trk_hits_eta_phi[2]->Fill(  perigee->eta(), perigee->parameters()[Trk::phi0], sctHits );
    m_trk_disabled_eta_phi[2]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nSCTDead ) ;
    m_trk_hits_LB[2]->Fill( AthenaMonManager::lumiBlockNumber(), sctHits );

    m_trk_shared_sct_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nSCTSharedHits );
    m_trk_holes_sct_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nSCTHoles );

    int nTRTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
    int nTRTDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTDeadStraws) ? iSummaryValue : 0;
    int trtHits = nTRTHits + nTRTDead;

    m_trk_hits_eta_phi[3]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],trtHits );
    m_trk_disabled_eta_phi[3]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], nTRTDead);
    m_trk_hits_LB[3]->Fill( AthenaMonManager::lumiBlockNumber(), trtHits );
}


void InDetGlobalTrackMonTool::FillEtaPhi( const xAOD::TrackParticle *trackPart)
{
    uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

    const Trk::Track * track = trackPart->track();
    if ( ! track ) return;

    const Trk::Perigee *perigee = track->perigeeParameters();
    float eta = perigee->eta();
    float phi = perigee->parameters()[Trk::phi0];

    // Loose primary tracks
    if ( m_baseline_selTool->accept(*track) )
    {
	m_Trk_eta_phi_Base->Fill( eta, phi );
	if ( m_doIBL )
	{
	    int expInHit = trackPart->summaryValue(iSummaryValue, xAOD::expectInnermostPixelLayerHit) ? iSummaryValue : 0;
	    int nInHit = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;
	    if ( expInHit==1 && nInHit==0 )
	    {
		m_Trk_eta_phi_noIBLhit_ratio->Fill( eta, phi, 1 );
	    }
	    else
	    {
		m_Trk_eta_phi_noIBLhit_ratio->Fill( eta, phi, 0 );
	    }

	    /// Next-to-innermost
	    int expNInHit = trackPart->summaryValue(iSummaryValue, xAOD::expectNextToInnermostPixelLayerHit) ? iSummaryValue : 0;
	    int nNInHit = trackPart->summaryValue(iSummaryValue, xAOD::numberOfNextToInnermostPixelLayerHits) ? iSummaryValue : 0;
	    if ( expNInHit==1 && nNInHit==0 )
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 1 );
	    }
	    else
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 0 );
	    }
	}
	else
	{
	    int expInHit = trackPart->summaryValue(iSummaryValue, xAOD::expectInnermostPixelLayerHit) ? iSummaryValue : 0;
	    int nInHit = trackPart->summaryValue(iSummaryValue, xAOD::numberOfInnermostPixelLayerHits) ? iSummaryValue : 0;
	    if ( expInHit==1 && nInHit==0 )
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 1 );
	    }
	    else
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 0 );
	    }
	}

	// No TRT extension
	int nTRTHit = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
	if ( nTRTHit == 0 )
	    m_Trk_eta_phi_noTRText_ratio->Fill( eta, phi, 1 );
	else
	    m_Trk_eta_phi_noTRText_ratio->Fill( eta, phi, 0 );
    }

    m_Trk_eta_phi_Tight->Fill( eta, phi);

    /// TRACKSEL: Tight
    if ( m_tight_selTool->accept(*track) )
    {
	m_Trk_eta_phi_Tight_ratio->Fill( eta, phi, 1 );
    }
    else
    {
	m_Trk_eta_phi_Tight_ratio->Fill( eta, phi, 0 );
    }

    return;
}

void InDetGlobalTrackMonTool::FillForwardTracks( const xAOD::TrackParticle *trackPart )
{
    uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

    int nPixHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;
    int nPixDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;

    const Trk::Track * track = trackPart->track();
    if ( ! track ) return;

    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( perigee )
    {
	float eta = perigee->eta();
	float phi = perigee->parameters()[Trk::phi0];

	if ( eta > 0. )
	{
	    m_Trk_FORW_FA_eta_phi->Fill( eta, phi );
	    m_Trk_FORW_FA_nPIXhits->Fill ( nPixHits + nPixDead );
	}
	else
	{
	    m_Trk_FORW_FC_eta_phi->Fill( eta, phi );
	    m_Trk_FORW_FC_nPIXhits->Fill ( nPixHits + nPixDead );
	}

	m_Trk_FORW_qoverp->Fill( perigee->parameters()[Trk::qOverP] *1000.0 );
    }

    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 )
	m_Trk_FORW_chi2->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());

    return;
}

void InDetGlobalTrackMonTool::FillTIDE()
{
    SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey };
    SG::ReadHandle<xAOD::JetContainer> jets(m_JetsName);
    if ( jets.isValid() ) {
        for ( const auto& jetItr : *jets )
	{
	    if ( jetItr->pt() < 20000. )
		continue;

	    std::vector<const xAOD::IParticle*> trackVector;
	    if ( !jetItr->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack, trackVector) )
		continue;

	    for ( const auto trkItr : trackVector )
	    {
		const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(trkItr);

		if ( !trackPart )
		    continue;

		uint8_t split;
		uint8_t shared;
		uint8_t pix;

		if ( trackPart->summaryValue(pix, xAOD::numberOfPixelHits) && pix )
		{
		    const Trk::Perigee perigee = trackPart->perigeeParameters();
		    const xAOD::Vertex* foundVertex { nullptr };
		    if ( vertices.isValid() )
		    {
		      for ( const auto *const vx : *vertices )
		      {
			for ( const auto& tpLink : vx->trackParticleLinks() )
			{
			  if ( *tpLink == trackPart )
			  {
			    foundVertex = vx;
			    break;
			  }
			}
			if (foundVertex) break;
		      }
		    }
		    if ( foundVertex )
		    {
			std::unique_ptr<const Trk::ImpactParametersAndSigma> myIPandSigma(m_trackToVertexIPEstimator->estimate(trackPart, foundVertex ));
			if ( myIPandSigma )
			{
			    float dr = trackPart->p4().DeltaR( jetItr->p4() );
			    float sigmad0 = myIPandSigma->sigmad0;
			    float PVsigmad0 = myIPandSigma->PVsigmad0;
			    float d0sig = std::abs( myIPandSigma->IPd0 ) / std::sqrt( sigmad0*sigmad0 + PVsigmad0*PVsigmad0 );
			    float sigmaz0 = myIPandSigma->sigmaz0;
			    float PVsigmaz0 = myIPandSigma->PVsigmaz0;
			    float z0sig = std::abs( myIPandSigma->IPz0 ) / std::sqrt( sigmaz0*sigmaz0 + PVsigmaz0*PVsigmaz0 );

			    m_trk_jetassoc_d0_reso_dr->Fill( dr, d0sig );
			    m_trk_jetassoc_z0_reso_dr->Fill( dr, z0sig );
			    m_trk_jetassoc_ip_reso_lb->Fill( AthenaMonManager::lumiBlockNumber(), d0sig );
			}
		    }
		    if ( trackPart->summaryValue( split, xAOD::numberOfPixelSplitHits) )
		    {
			float frac = (double)split / pix;
			m_trk_jetassoc_split_pix_dr->Fill( trackPart->p4().DeltaR( jetItr->p4() ), frac );
			m_trk_jetassoc_split_pix_lb->Fill( AthenaMonManager::lumiBlockNumber(), frac );
		    }

		    if ( trackPart->summaryValue( shared, xAOD::numberOfPixelSharedHits) )
		    {
			float frac = (float)shared / pix;
			m_trk_jetassoc_shared_pix_dr->Fill( trackPart->p4().DeltaR( jetItr->p4() ), frac );
			m_trk_jetassoc_shared_pix_lb->Fill( AthenaMonManager::lumiBlockNumber(), frac );
		    }

		    if ( m_doTideResiduals )
		    {
			const auto *track = trackPart->track();
			if ( ! track )
			    continue;

			const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
			if ( trackStates == nullptr ) return;

			for (const auto& tsos : *trackStates) {

			    if (tsos == nullptr) continue;

			    //SILICON (SCT + Pixel)
			    const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>( tsos->measurementOnTrack() );
			    if ( !clus ) continue;

			    const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
			    if (!RawDataClus) continue;

			    if ( RawDataClus->detectorElement()->isPixel() ) {
				const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>( tsos->measurementOnTrack() );

				if (hit && tsos->trackParameters()) {
				    const Trk::TrackParameters* PropagatedTrackParams = tsos->trackParameters()->clone();
				    const Trk::TrackParameters* UnbiasedTrackParams = m_iUpdator->removeFromState(*PropagatedTrackParams,
                                                                                          tsos->measurementOnTrack()->localParameters(),
                                                                                          tsos->measurementOnTrack()->localCovariance()).release();
				    delete PropagatedTrackParams;
				    if ( !UnbiasedTrackParams )
					if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "RemoveFromState did not work, using original TrackParameters" << endmsg;

				    const Trk::ResidualPull * residualPull = m_residualPullCalculator->residualPull(tsos->measurementOnTrack(), ( UnbiasedTrackParams ) ? UnbiasedTrackParams:tsos->trackParameters(), Trk::ResidualPull::Unbiased);
				    if (residualPull) {
					{
					    float dr = trackPart->p4().DeltaR( jetItr->p4() );
					    float pull_x = residualPull->pull()[Trk::loc1];
					    float pull_y = residualPull->pull()[Trk::loc2];

					    switch ( m_pixelID->barrel_ec( RawDataClus->identify() ) )
					    {
					    case 0:
						switch ( m_pixelID->layer_disk( RawDataClus->identify() ) )
						{
						case 0:
						    m_trk_jetassoc_res_pix_l0_x_dr->Fill( dr, pull_x );
						    m_trk_jetassoc_res_pix_l0_y_dr->Fill( dr, pull_y );
						    break;
						case 1:
						    m_trk_jetassoc_res_pix_l1_x_dr->Fill( dr, pull_x );
						    m_trk_jetassoc_res_pix_l1_y_dr->Fill( dr, pull_y );
						    break;
						case 2:
						    m_trk_jetassoc_res_pix_l2_x_dr->Fill( dr, pull_x );
						    m_trk_jetassoc_res_pix_l2_y_dr->Fill( dr, pull_y );
						    break;
						case 3:
						    m_trk_jetassoc_res_pix_l3_x_dr->Fill( dr, pull_x );
						    m_trk_jetassoc_res_pix_l3_y_dr->Fill( dr, pull_y );
						    break;
						}
						break;
					    case -2:
						m_trk_jetassoc_res_pix_ecc_x_dr->Fill( dr, pull_x );
						m_trk_jetassoc_res_pix_ecc_y_dr->Fill( dr, pull_y );
						break;
					    case 2:
						m_trk_jetassoc_res_pix_eca_x_dr->Fill( dr, pull_x );
						m_trk_jetassoc_res_pix_eca_y_dr->Fill( dr, pull_y );
						break;
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    else
    {
	ATH_MSG_WARNING( "Unable to get jets, turning TIDE plots off!" );
	m_doTide = false;
    }

    return;
}

void InDetGlobalTrackMonTool::FillHitMaps( const xAOD::TrackParticle *trackPart )
{
    const Trk::Track * track = trackPart->track();
    if ( ! track ) return;

    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
    if ( trackStates == nullptr ) return;

    for (const auto& trackState : *trackStates) {

	if (trackState == nullptr) continue;

	//TRT
	const InDet::TRT_DriftCircleOnTrack *trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trackState->measurementOnTrack());
	if ( trtcircle )
	{
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
	}

	//SILICON (SCT + Pixel)
	const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>( trackState->measurementOnTrack() );
	if ( !clus ) continue;

	const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
	if (!RawDataClus) continue;

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


void InDetGlobalTrackMonTool::FillHoles( const xAOD::TrackParticle *trackPart )
{
    uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

    int nPixHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHoles) ? iSummaryValue : -1;
    int nSCTHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHoles) ? iSummaryValue : -1;
    int nTRTHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHoles) ? iSummaryValue : -1;

    const Trk::Track * track = trackPart->track();
    if ( ! track ) return;

    m_pixel_holes->Fill(nPixHoles);
    m_sct_holes->Fill(nSCTHoles);
    m_trt_holes->Fill(nTRTHoles);

    // Filling Combined Holes and Excluding case Tracks on surface does not exist
    if (nPixHoles>=0)
	m_comb_holes->Fill(nPixHoles);
    if (nSCTHoles>=0)
        m_comb_holes->Fill(nSCTHoles);
    if (nTRTHoles>=0)
	m_comb_holes->Fill(nTRTHoles);

    nPixHoles = std::max(nPixHoles, 0);
    nSCTHoles = std::max(nSCTHoles, 0);
    nTRTHoles = std::max(nTRTHoles, 0);
    int nSiHoles = nPixHoles + nSCTHoles;
    int nTotHoles = nPixHoles + nSCTHoles + nTRTHoles;

    m_silicon_vs_trt->Fill(nSiHoles, nTRTHoles);
    m_sct_vs_pixels->Fill(nSCTHoles, nPixHoles);

    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 ){
	float chi2_ndf = track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF();
	m_holes_quality->Fill(chi2_ndf, nTotHoles);
	m_holes_quality_profile->Fill(nTotHoles, chi2_ndf);
    }

    // Filling Number of holes vs number of hits for tracks with at least a hole.

    int nPixHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;
    int nPixDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;
    int nSCTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTHits) ? iSummaryValue : 0;
    int nSCTDead = trackPart->summaryValue(iSummaryValue, xAOD::numberOfSCTDeadSensors) ? iSummaryValue : 0;
    int nTRTHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfTRTHits) ? iSummaryValue : 0;
    int nTotHits = nPixHits + nPixDead + nSCTHits + nSCTDead + nTRTHits;

    if (nPixHoles>0 || nSCTHoles>0 || nTRTHoles>0) {
	m_holes_hits->Fill(nTotHits, nTotHoles);
	m_holesvshits->Fill(nTotHits, nTotHoles);
    }

    // Here The Perigee Parameters of Holes
    const Trk::Perigee *perigee = dynamic_cast<const Trk::Perigee *>(track->perigeeParameters());

    m_holes_eta_phi_n->Fill(perigee->eta(), perigee->parameters()[Trk::phi], float(nSiHoles)/nTotHits);
    m_holes_eta_phi->Fill(perigee->eta(), perigee->parameters()[Trk::phi]);
    m_holes_eta_pt->Fill(perigee->eta(), perigee->pT()/1000.0, nSiHoles);
    m_holes_phi_pt->Fill(perigee->parameters()[Trk::phi], perigee->pT()/1000.0, nSiHoles);

    // Filling holes vs hits in eta bins
    if (nPixHoles >0 || nSCTHoles >0 || nTRTHoles >0){
	// Filling ECA
	if  (perigee->eta()>=-2.5 && perigee->eta()<=-1.5) {
	    m_holesvshits_ECA->Fill(nTotHits, nTotHoles);
	}

	else if  (perigee->eta()>=-1.5 && perigee->eta()<=1.5) {
	    m_holesvshits_BA->Fill(nTotHits, nTotHoles);
	}

	else if  (perigee->eta()>=1.5 && perigee->eta()<=2.5) {
	    m_holesvshits_ECC->Fill(nTotHits, nTotHoles);
	}
    }

    if (m_DoHoles_Search) FillHoleMaps(trackPart);

    return;
}

void InDetGlobalTrackMonTool::FillHoleMaps( const xAOD::TrackParticle *trackPart )
{
    const Trk::Track * track = trackPart->track();
    if ( ! track ) return;

    std::unique_ptr<const DataVector<const Trk::TrackStateOnSurface> > holesOnTrack( m_holes_search_tool->getHolesOnTrack(*track, track->info().particleHypothesis()) );

    // loop over holes
    if (!holesOnTrack) {
	msg(MSG::WARNING) << "Got no holes on track" << endmsg;
    } else {
        for( const auto it : *holesOnTrack ) {
	    if (!it) {
		msg(MSG::WARNING) << "TrackStateOnSurface from hole search tool == Null" << endmsg;
		continue;
	    }
	    // Here The X Y Z of Holes
	    const Trk::TrackParameters *clus = it->trackParameters() ;
	    if (clus){
		m_HolesMAP_XY->Fill(clus->position()[0], clus->position()[1]);
		m_HolesMAP_ZX->Fill(clus->position()[2], clus->position()[0]);
		m_HolesMAP_ZR->Fill(clus->position()[2], std::sqrt( std::pow( clus->position()[0], 2) + std::pow( clus->position()[1], 2) ));

	    }
	}
    }

    return;
}
