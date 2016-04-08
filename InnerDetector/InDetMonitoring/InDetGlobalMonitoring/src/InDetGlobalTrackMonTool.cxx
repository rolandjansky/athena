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

//Local
#include "InDetGlobalTrackMonTool.h"
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
      m_loosePri_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
      m_tight_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
      m_doLumiblock(true),
      m_doHolePlots(false),
      m_DoHoles_Search(false),
      m_doHitMaps(false),
      m_doForwardTracks(m_dataType == AthenaMonManager::collisions),
      m_doIBL(true),
      m_nBinsEta( 50 ),
      m_nBinsPhi( 50 ),
      m_trackBin( 100 ),
      c_etaRange(2.5),
      c_etaTrackletsMin(2.4),
      c_etaTrackletsMax(2.7),
      c_etaRangeTRT(2.0),
      c_range_LB(2000),
      c_detector_labels{ "IBL", "PIX", "SCT", "TRT" },
      m_IBLParameterSvc("IBLParameterSvc",name),
      m_holes_search_tool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
      m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
      m_CombinedTracksName("Tracks"),
      m_ForwardTracksName("ResolvedForwardTracks"),
      sct_holes(nullptr),
      trt_holes(nullptr),
      pixel_holes(nullptr),
      comb_holes(nullptr),
      silicon_vs_trt(nullptr),
      sct_vs_pixels(nullptr),
      holes_quality(nullptr),
      holes_quality_profile(nullptr),
      m_Trk_nLoose(nullptr),
      m_Trk_eta_phi(nullptr),
      m_Trk_eta_phi_LoosePrimary_ratio(nullptr),
      m_Trk_eta_phi_Tight_ratio(nullptr),
      m_Trk_eta_phi_noIBLhit_ratio(nullptr),
      m_Trk_eta_phi_noBLhit_ratio(nullptr),
      m_Trk_eta_phi_noTRText_ratio(nullptr),
      m_Trk_nLoose_LB(nullptr),
      m_Trk_nLoosePrimary_LB(nullptr),
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
      m_holesComb2(nullptr),
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
      m_Trk_FORW_FA_eta_phi(nullptr),
      m_Trk_FORW_FC_eta_phi(nullptr),
      m_Trk_FORW_qoverp(nullptr),
      m_Trk_FORW_chi2(nullptr),
      m_Trk_FORW_FA_nPIXhits(nullptr),
      m_Trk_FORW_FC_nPIXhits(nullptr)
{
    declareProperty("DoHoleSearch",m_DoHoles_Search,"Write hole data?");
    declareProperty("HoleSearchTool", m_holes_search_tool,"Tool to search for holes on track");	
    declareProperty("TrackCollection",m_CombinedTracksName,"Name of the forward tracklet collection");
    declareProperty("ForwardTrackCollection",m_ForwardTracksName,"Name of the combined track collection");
    declareProperty("DoHitMaps", m_doHitMaps,"Produce hit maps?");	
    declareProperty("DoForwardTracks", m_doForwardTracks,"Run over forward tracks?");	
    declareProperty("DoIBL", m_doIBL,"IBL present?");	
    declareProperty("trackMax",m_trackMax,"Maximum number of tracks in track histograms");
    declareProperty("trackBin",m_trackBin,"Number of bins for the track histograms");

    declareProperty( "LoosePrimary_SelTool", m_loosePri_selTool );
    declareProperty( "Tight_SelTool", m_tight_selTool );
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

  if (m_IBLParameterSvc.retrieve().isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve IBLParameterSvc" << endreq;
      return StatusCode::FAILURE;
  } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_IBLParameterSvc << endreq;
  }
  
  m_doIBL = m_IBLParameterSvc->containsIBL();
  
  ATH_CHECK( m_loosePri_selTool.retrieve() );
  ATH_CHECK( m_tight_selTool.retrieve() );
  
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalTrackMonTool::bookHistograms()
{
    Interval_t detailsInterval = ( AthenaMonManager::environment() != AthenaMonManager::online ) ? lowStat : run;
    
    registerManHist( m_Trk_nLoose, "InDetGlobal/Track", detailsInterval,
		     "nCOMBtrks", "Track multiplicity (Loose tracks)",
		     m_trackBin, 0.5, m_trackMax+0.5,
		     "Track multiplicity" ).ignore();

    registerManHist( m_Trk_eta_phi, "InDetGlobal/Track", detailsInterval,
		     "COMB_eta_phi","Distribution of eta vs phi for combined tracks (both Si and TRT hits)",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta","#phi" ).ignore();
    
    // Eta-phi maps
    registerManHist( m_Trk_eta_phi_LoosePrimary_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_LoosePrimary_eta_phi_ratio","Distribution of eta vs phi for combined tracks passing Loose Primary selection",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi, -M_PI, M_PI,
                     "eta", "#phi_{0}" ).ignore();

    registerManHist( m_Trk_eta_phi_Tight_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_Tight_eta_phi_ratio","Distribution of eta vs phi for combined tracks passing Tight selection",
                     m_nBinsEta, -c_etaRange, c_etaRange,
                     m_nBinsPhi, -M_PI, M_PI,
                     "eta", "#phi_{0}" ).ignore();
    
    registerManHist( m_Trk_eta_phi_noTRText_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_eta_phi_ratio","Distribution of eta vs phi for combined tracks with no TRT extension",
		     20, -c_etaRangeTRT, c_etaRangeTRT, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "eta", "#phi_{0}" ).ignore();
    
    registerManHist( m_Trk_eta_phi_noBLhit_ratio, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhit_eta_phi_ratio","Eta-phi of tracks with no b-layer hit but a hit is expected, ratio to total tracks",
		     m_nBinsEta, -c_etaRange, c_etaRange, 
		     m_nBinsPhi, -M_PI, M_PI,
		     "#eta", "#phi_{0}").ignore();
    
    if ( m_doIBL )
    {
	registerManHist( m_Trk_eta_phi_noIBLhit_ratio, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhit_eta_phi_ratio","Eta-phi of tracks with no IBL hit but a hit is expected, ratio to total tracks",
			 m_nBinsEta, -c_etaRange, c_etaRange, 
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
			 "Trk_n"+c_detector_labels[i]+"hits_eta_phi","Number of "+c_detector_labels[i]+" hits per track, eta-phi profile",
			 m_nBinsEta, -c_etaRange, c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi" ).ignore();
    
	registerManHist( m_trk_hits_LB[i], "InDetGlobal/Track", detailsInterval,
			 "trk_n"+c_detector_labels[i]+"hits_LB","Average number of " + c_detector_labels[i] + " hits by LB",
			 c_range_LB,0,c_range_LB,
			 "LB #", "Average number of hits in LB").ignore();
    }
    
    for (unsigned int i = 1; i < m_trk_disabled_eta_phi.size(); i++ )
    {
	registerManHist( m_trk_disabled_eta_phi[i],  "InDetGlobal/Hits", detailsInterval,
			 "Trk_n"+c_detector_labels[i]+"disabled_eta_phi","Number of "+c_detector_labels[i]+" disabled detector elements per track, eta-phi profile",
			 m_nBinsEta, -c_etaRange, c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi" ).ignore();
    }
    
    registerManHist( m_trk_shared_pix_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nPixShared_eta_phi","Number of Pixel shared hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_split_pix_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nPixSplit_eta_phi","Number of Pixel split hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();

    registerManHist( m_trk_shared_sct_eta_phi,  "InDetGlobal/Hits", detailsInterval,
		     "Trk_nSCTShared_eta_phi","Number of SCT shared hits per track, eta-phi profile",
		     m_nBinsEta, -c_etaRange, c_etaRange,
		     m_nBinsPhi,-M_PI,M_PI,
		     "#eta", "#phi" ).ignore();
    
    // Forward Pixel tracks
    if ( m_doForwardTracks )
    {	
	registerManHist( m_Trk_FORW_FA_eta_phi, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_FA_eta_phi", "Eta-phi for pixel tracklets in the forward region (positive eta)",
			 5, c_etaTrackletsMin, c_etaTrackletsMax, 
			 m_nBinsPhi, -M_PI, M_PI, 
			 "#eta", "#phi_{0}" ).ignore();
	
	registerManHist( m_Trk_FORW_FC_eta_phi, "InDetGlobal/Track", detailsInterval,
			 "Trk_FORW_FC_eta_phi", "Eta-phi for pixel tracklets in the forward region (negative eta)",
			 5, -c_etaTrackletsMax, -c_etaTrackletsMin, 
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
    registerManHist( m_Trk_nLoose_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_nLoose_LB","Average number of loose tracks per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Average number of loose tracks in LB").ignore();

    registerManHist( m_Trk_nLoosePrimary_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_nLoosePrimary_LB","Average number of loose primary tracks per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Average number of loose primary tracks per event in LB").ignore();

    registerManHist( m_Trk_nTight_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_nTight_LB","Average number of tight tracks per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Average number of tight tracks per event in LB").ignore();
    
    if ( m_doIBL )
    {
	registerManHist( m_Trk_noIBLhits_LB, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhits_LB","Average number of tracks with missing IBL hit per event in LB",
			 c_range_LB,0,c_range_LB,
			 "LB #", "Average number of tracks with missing IBL hit per event in LB").ignore();
	registerManHist( m_Trk_noIBLhits_frac_LB, "InDetGlobal/Track", detailsInterval,
			 "Trk_noIBLhits_frac_LB","Fraction of tracks with missing IBL hit per event in LB",
			 c_range_LB,0,c_range_LB,
			 "LB #", "Fraction of tracks with missing IBL hit per event in LB").ignore();
    }
    
    registerManHist( m_Trk_noBLhits_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhits_LB","Average number of tracks with missing b-layer hit per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Average number of tracks with missing b-layer hit per event in LB").ignore();

    registerManHist( m_Trk_noBLhits_frac_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noBLhits_frac_LB","Fraction of tracks with missing b-layer hit per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Fraction of tracks with missing b-layer hit per event in LB").ignore();

    registerManHist( m_Trk_noTRText_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_LB","Average number of tracks without TRT extension per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Average number of tracks with without TRT extension per event in LB").ignore();
    registerManHist( m_Trk_noTRText_frac_LB, "InDetGlobal/Track", detailsInterval,
		     "Trk_noTRText_frac_LB","Fraction of tracks without TRT extension per event in LB",
		     c_range_LB,0,c_range_LB,
		     "LB #", "Fraction of tracks with without TRT extension per event in LB").ignore();

    // Holes
    if ( m_doHolePlots )
    {
	registerManHist( sct_holes, "InDetGlobal/Hits", detailsInterval,
			 "sct_holes", "Distribution of SCT Holes ",
			 104,-3.5,100.5,
			 "Number of SCT holes").ignore();	
	
	registerManHist( trt_holes, "InDetGlobal/Hits", detailsInterval,
			 "trt_holes", "Distribution of TRT Holes ",
			 104,-3.5,100.5,
			 "Number of TRT holes").ignore();
	
	registerManHist( pixel_holes, "InDetGlobal/Hits", detailsInterval,
			 "pixel_holes", "Distribution of Pixel Holes ",
			 104,-3.5,100.5,
			 "Number of Pixel holes").ignore();
	
	registerManHist( comb_holes, "InDetGlobal/Hits", detailsInterval,
			 "comb_holes", "Distribution of Combined Holes ",
			 104,-3.5,100.5,
			 "Total number of holes").ignore();
	
	registerManHist( silicon_vs_trt, "InDetGlobal/Hits", detailsInterval,
			 "silicon_vs_trt", "Silicon vs TRT holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "Silicon Combined holes", "TRT holes").ignore();
	
	registerManHist( sct_vs_pixels, "InDetGlobal/Hits", detailsInterval,
			 "sct_vs_pixels", "SCT vs Pixels holes ",
			 104,-3.5,100.5,
			 104,-3.5,100.5,
			 "SCT", "Pixels").ignore();
	
	registerManHist( holes_quality, "InDetGlobal/Hits", detailsInterval,
			 "holes_quality", "Number of holes/track vs #chi^{2}/ndf",
			 160,-0.5,15.5,
			 104,-3.5,100.5,
			 "#chi^{2}/ndf", "Combined Holes").ignore();	
	
	registerManHist( m_holes_eta_phi, "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_phi", "Holes Map #eta #phi",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();
	
	
	registerManHist( m_holes_eta_pt,  "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_pt", "Holes #eta vs p_{t}",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 30,-0.,30.,
			 "#eta", "#p_{t}").ignore();
	
	registerManHist( m_holes_phi_pt,  "InDetGlobal/Hits", detailsInterval,
			 "holes_phi_pt", "Holes #phi vs p_{t}",
			 m_nBinsPhi,-M_PI,M_PI,
			 30,-0.,30.,
			 "#phi", "#p_{t}").ignore();
	
	registerManHist( m_holes_eta_phi_n,  "InDetGlobal/Hits", detailsInterval,
			 "holes_eta_phi_n", "Holes Map (Norm) #eta #phi",
			 m_nBinsEta,-c_etaRange,c_etaRange,
			 m_nBinsPhi,-M_PI,M_PI,
			 "#eta", "#phi").ignore();
	
	registerManHist( holes_quality_profile,  "InDetGlobal/Hits", detailsInterval,
			 "holes_quality_profile", "#chi^{2}/ndf vs Number of holes/track profile",
			 104,-3.5,100.5,
			 "<#chi^{2}/ndf> / comb hole", "Combined Holes").ignore();
	holes_quality_profile->SetErrorOption("S");
	holes_quality_profile->SetMaximum( 1000 );
	holes_quality_profile->SetMinimum( 0 );
    
	
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

    int nLoose = 0;
    int nLP = 0;
    int nTight = 0;
    int nNoIBL = 0;
    int nNoBL = 0;
    int nNoTRText = 0;

    TrackCollection::const_iterator itrack = combined_tracks->begin();
    TrackCollection::const_iterator itrack_end = combined_tracks->end();
    for ( ; itrack!= itrack_end; ++itrack)
    {
	const Trk::Track * track = (*itrack);
	if ( !track || track->perigeeParameters() == 0 )
	{
	    ATH_MSG_DEBUG( "NULL track pointer in collection" );
	    continue;
	}
	
	// Skip tracks that are not inside out
	if ( m_dataType == AthenaMonManager::collisions
	     && ( ! (track->info().patternRecoInfo( Trk::TrackInfo::SiSPSeededFinder ) )
		  || track->perigeeParameters()->pT() < 5000. ) )
	    continue;
	
	// Create a new summary or get copy of the cached one
	const Trk::TrackSummary * summary = m_trkSummaryTool->createSummary( * track );
	
	if ( !summary )
	{
	    ATH_MSG_DEBUG( "NULL pointer to track summary" );
	    continue;
	}
	
	// Skip tracks without Pixel or SCT hits in cosmics
	if ( m_dataType == AthenaMonManager::cosmics  && 
	     summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfSCTHits) <= 0 )
	{
	    continue;
	}

	FillHits( track, summary );
	FillEtaPhi( track , summary );
	
	nLoose++;
	
	if ( m_loosePri_selTool->accept(*track) )
	{
	    nLP++;
	}
	if ( m_tight_selTool->accept(*track) )
	{
	    nTight++;
	    
	    if ( m_doIBL )
	    {
		if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) )
		{
		    nNoIBL++;
		    m_Trk_noIBLhits_frac_LB->Fill( m_manager->lumiBlockNumber(), 1 );
		}
		else
		{
		    m_Trk_noIBLhits_frac_LB->Fill( m_manager->lumiBlockNumber(), 0 );
		}
	    }
	    
	    if ( summary->get( ( m_doIBL ) ? Trk::expectNextToInnermostPixelLayerHit : Trk::expectInnermostPixelLayerHit ) && !summary->get( ( m_doIBL ) ? Trk::numberOfNextToInnermostPixelLayerHits : Trk::numberOfInnermostPixelLayerHits ) )
	    {
		nNoBL++;
		m_Trk_noBLhits_frac_LB->Fill( m_manager->lumiBlockNumber(), 1 );
	    }
	    else
	    {
		m_Trk_noBLhits_frac_LB->Fill( m_manager->lumiBlockNumber(), 0 );
	    }
	    if ( summary->get(Trk::numberOfTRTHits) + summary->get(Trk::numberOfTRTOutliers) == 0 )
	    {
		nNoTRText++;;
		m_Trk_noTRText_frac_LB->Fill(m_manager->lumiBlockNumber(), 1);
	    }
	    else
	    {
		m_Trk_noTRText_frac_LB->Fill(m_manager->lumiBlockNumber(), 0);
	    }
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
    
    m_Trk_nLoose->Fill( nLoose );
    m_Trk_nLoose_LB->Fill( m_manager->lumiBlockNumber(), nLoose );

    m_Trk_nLoosePrimary_LB->Fill( m_manager->lumiBlockNumber(), nLP );
    m_Trk_nTight_LB->Fill( m_manager->lumiBlockNumber(), nTight );
    if ( m_doIBL )
	m_Trk_noIBLhits_LB->Fill( m_manager->lumiBlockNumber(), nNoIBL );
    
    m_Trk_noBLhits_LB->Fill( m_manager->lumiBlockNumber(), nNoBL );
    m_Trk_noTRText_LB->Fill( m_manager->lumiBlockNumber(), nNoTRText );
    
    if ( m_doForwardTracks )
    {
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
		if ( !track )
		{
		    ATH_MSG_DEBUG( "NULL track pointer in collection" );
		    continue;
		}
		
		// Create a new summary or get copy of the cached one
		const Trk::TrackSummary * summary = m_trkSummaryTool->createSummary( * track );
		
		if ( !summary )
		{
		    ATH_MSG_DEBUG( "NULL pointer to track summary" );
		    continue;
		}
		
		FillForwardTracks( track , summary );
		
		delete summary;
	    }
	}
    }	
    return StatusCode::SUCCESS;
}


StatusCode InDetGlobalTrackMonTool::procHistograms()
{
    return StatusCode::SUCCESS;
}

void InDetGlobalTrackMonTool::FillHits( const Trk::Track *track, const Trk::TrackSummary* summary )
{
    int pixHits = ( summary->get(Trk::numberOfPixelHits) >= 0 ) ? summary->get(Trk::numberOfPixelHits) : 0 
	+ ( summary->get(Trk::numberOfPixelDeadSensors) >= 0 ) ? summary->get(Trk::numberOfPixelDeadSensors ) : 0;
    int sctHits = ( ( summary->get(Trk::numberOfSCTHits) >= 0 ) ? summary->get(Trk::numberOfSCTHits) : 0 )
	+ ( ( summary->get(Trk::numberOfSCTDeadSensors) >= 0 ) ? summary->get(Trk::numberOfSCTDeadSensors) : 0 );
    int trtHits = summary->get(Trk::numberOfTRTHits) + summary->get(Trk::numberOfTRTDeadStraws);
    
    const Trk::Perigee *perigee = track->perigeeParameters();
    if ( !perigee )
	return;

    if ( m_doIBL )
    {
	m_trk_hits_eta_phi[0]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], summary->get( Trk::numberOfInnermostPixelLayerHits ) );
	m_trk_hits_LB[0]->Fill( m_manager->lumiBlockNumber(), summary->get( Trk::numberOfInnermostPixelLayerHits ) );
    }
    
    m_trk_hits_eta_phi[1]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], pixHits );
    m_trk_disabled_eta_phi[1]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], 
				     ( summary->get(Trk::numberOfPixelDeadSensors) >= 0 ) ? summary->get(Trk::numberOfPixelDeadSensors) : 0 );
    m_trk_hits_LB[1]->Fill( m_manager->lumiBlockNumber(), pixHits );
    m_trk_shared_pix_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],
				    ( summary->get(Trk::numberOfPixelSharedHits) >= 0 ) ? summary->get(Trk::numberOfPixelSharedHits) : 0 );
    m_trk_split_pix_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],
				   ( summary->get(Trk::numberOfPixelSplitHits) >= 0 ) ? summary->get(Trk::numberOfPixelSplitHits) : 0 );
    
    m_trk_hits_eta_phi[2]->Fill(  perigee->eta(), perigee->parameters()[Trk::phi0],sctHits );
    m_trk_disabled_eta_phi[2]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], 
				     ( summary->get(Trk::numberOfSCTDeadSensors) >= 0 ) ? summary->get(Trk::numberOfSCTDeadSensors) : 0 ) ;
    m_trk_hits_LB[2]->Fill( m_manager->lumiBlockNumber(), sctHits );
    m_trk_shared_sct_eta_phi->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],
				    ( summary->get(Trk::numberOfSCTSharedHits) >= 0 ) ? summary->get(Trk::numberOfSCTSharedHits) : 0 );
    
    m_trk_hits_eta_phi[3]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0],trtHits );
    m_trk_disabled_eta_phi[3]->Fill( perigee->eta(), perigee->parameters()[Trk::phi0], 
				     ( summary->get(Trk::numberOfTRTDeadStraws) >= 0 ) ? summary->get(Trk::numberOfTRTDeadStraws) : 0 );
    m_trk_hits_LB[3]->Fill( m_manager->lumiBlockNumber(), trtHits );
} 


void InDetGlobalTrackMonTool::FillEtaPhi( const Trk::Track *track, const Trk::TrackSummary* summary )
{
    if ( ! track || ! summary ) return;
	
    const Trk::Perigee *perigee = track->perigeeParameters();
    float eta = perigee->eta();
    float phi = perigee->parameters()[Trk::phi0];

    /// Map of all extended tracks
    m_Trk_eta_phi->Fill( eta, phi );
    bool isTight = false;

    // Loose primary tracks
    if ( m_loosePri_selTool->accept(*track) )
    {
	m_Trk_eta_phi_LoosePrimary_ratio->Fill( eta, phi, 1 );
    }
    else
    {
	m_Trk_eta_phi_LoosePrimary_ratio->Fill( eta, phi, 0 );
    }
    
    /// TRACKSEL: Tight
    if ( m_tight_selTool->accept(*track) )
    {
	isTight = true;
	m_Trk_eta_phi_Tight_ratio->Fill( eta, phi, 1 );
    }
    else
    {
	m_Trk_eta_phi_Tight_ratio->Fill( eta, phi, 0 );
    }

    /// Innermost layer
    if ( isTight )
    {
	if ( m_doIBL )
	{
	    if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) )
	    {
		m_Trk_eta_phi_noIBLhit_ratio->Fill( eta, phi, 1 );
	    }
	    else
	    {
		m_Trk_eta_phi_noIBLhit_ratio->Fill( eta, phi, 0 );
	    }
	    
	    /// Next-to-innermost
	    if ( summary->get( Trk::expectNextToInnermostPixelLayerHit ) && !summary->get( Trk::numberOfNextToInnermostPixelLayerHits ) )
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
	    if ( summary->get( Trk::expectInnermostPixelLayerHit ) && !summary->get( Trk::numberOfInnermostPixelLayerHits ) )
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 1 );
	    }
	    else
	    {
		m_Trk_eta_phi_noBLhit_ratio->Fill( eta, phi, 0 );
	    }
	}
	
	// No TRT extension
	if ( summary->get(Trk::numberOfTRTHits) == 0 )
	    m_Trk_eta_phi_noTRText_ratio->Fill( eta, phi, 1 );
	else
	    m_Trk_eta_phi_noTRText_ratio->Fill( eta, phi, 0 );
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
	    m_Trk_FORW_FA_eta_phi->Fill( eta, phi );
	    m_Trk_FORW_FA_nPIXhits->Fill ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) );
	}    
	else
	{
	    m_Trk_FORW_FC_eta_phi->Fill( eta, phi );
	    m_Trk_FORW_FC_nPIXhits->Fill ( summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors) );
	}

	m_Trk_FORW_qoverp->Fill( perigee->parameters()[Trk::qOverP] *1000.0 );
    }
    
    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 )
	m_Trk_FORW_chi2->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
    
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


void InDetGlobalTrackMonTool::FillHoles( const Trk::Track * track, const Trk::TrackSummary* summary )
{
    int pixelh = summary->get(Trk::numberOfPixelHoles) >= 0 ? summary->get(Trk::numberOfPixelHoles) : 0;
    int scth   = summary->get(Trk::numberOfSCTHoles) >= 0 ? summary->get(Trk::numberOfSCTHoles) : 0;
    int trth   = summary->get(Trk::numberOfTRTHoles) >= 0 ? summary->get(Trk::numberOfTRTHoles) : 0;
    
    // Gaetano Added : 
    pixel_holes->Fill(summary->get(Trk::numberOfPixelHoles));
    sct_holes->Fill(summary->get(Trk::numberOfSCTHoles));
    trt_holes->Fill(summary->get(Trk::numberOfTRTHoles));
    
    // Filling Combined Holes and Excluding # holes = -1 case Tracks on surface does not exist		
    if (summary->get(Trk::numberOfPixelHoles)>=0)
	comb_holes->Fill(pixelh);

    if (summary->get(Trk::numberOfSCTHoles) >=0 )
	comb_holes->Fill(scth);

    if (summary->get(Trk::numberOfTRTHoles)>=0)
	comb_holes->Fill(trth);

    silicon_vs_trt->Fill(pixelh+scth,trth);
    sct_vs_pixels->Fill(scth,pixelh);	
	    
    if ( track->fitQuality() && track->fitQuality()->numberDoF() > 0 ){
	holes_quality->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF(),pixelh+scth+trth);
	holes_quality_profile->Fill(scth+trth+pixelh,track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
    }
    
    // Filling Number of holes vs number of hits for tracks with at least a hole. 			
    if (scth >0 || trth >0 || pixelh >0) {
	m_holes_hits->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+trth+pixelh);
	m_holesvshits->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+trth+pixelh);
    }
    // Here The Perigee Parameters of Holes
    const Trk::Perigee *perigee = dynamic_cast<const Trk::Perigee *>(track->perigeeParameters());	
    
    m_holes_eta_phi_n->Fill( perigee->eta(),perigee->parameters()[Trk::phi],scth+pixelh/(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors)));
    
    m_holes_eta_phi->Fill( perigee->eta(),perigee->parameters()[Trk::phi]);
    m_holes_eta_pt->Fill(perigee->eta(),perigee->pT()/1000.0,scth+pixelh);
    m_holes_phi_pt->Fill(perigee->parameters()[Trk::phi],perigee->pT()/1000.0,scth+pixelh);
    
    // Filling holes vs hits in eta bins	
    if (scth >0 || trth >0 || pixelh >0){
	// Filling ECA
	if  (perigee->eta()>=-2.5 && perigee->eta()<=-1.5) {
	    m_holesvshits_ECA->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
	}
	
	if  (perigee->eta()>=-1.5 && perigee->eta()<=1.5) {
	    m_holesvshits_BA->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
	}
	
	
	if  (perigee->eta()>=1.5 && perigee->eta()<=2.5) {
	    m_holesvshits_ECC->Fill(summary->get(Trk::numberOfTRTHits)+summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)+summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors),scth+pixelh+trth);
	}
    }
    
    if (m_DoHoles_Search)
	FillHoleMaps(track);
    
    return;
}

void InDetGlobalTrackMonTool::FillHoleMaps( const Trk::Track *track )
{
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
	}	
    }
    delete holesOnTrack;
    
    return;
}
