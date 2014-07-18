/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalTrackMonTool.h
 * Implementation of inner detector global track monitoring tool
 * based on Segment tool
 *
 *@author
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>
 * Gaetano Barone <Gaetano.Barone@cern.ch> @n 
 * 
 * $Id: InDetGlobalTrackMonTool.h,v 1.24 2009-04-06 17:18:19 kastanas Exp $
 *
 ****************************************************************************/

#ifndef InDetGlobalTrackMonTool_H
#define InDetGlobalTrackMonTool_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"

//Detector Managers
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

//Local
//Framework

#include "TrkTrack/TrackCollection.h"
//Standard c++
#include <string>
#include <map>
#include <vector>
#include "GaudiKernel/ToolHandle.h"
//Predeclarations
class IInterface;
class StatusCode;
class TH1I;
class TH1F;
class TH2F;
class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class TProfile_LW;
class TProfile;
class TProfile2D;

/// Monitoring tool derived from InDetGlobalMotherMonTool
/// Contains the global track information for the ID

class InDetGlobalTrackMonTool : public ManagedMonitorToolBase//public InDetGlobalMotherTrigMonTool
{
public:
    ///Default constructor
    InDetGlobalTrackMonTool( 
	const std::string & type,
	const std::string & name,
	const IInterface* parent);
    
    ///Virtual destructor
    virtual ~InDetGlobalTrackMonTool() {}
    
    virtual StatusCode initialize();

    ///@name histos Book, fill and proc histograms
    ///@{

    ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistograms();
    virtual StatusCode bookHistogramsRecurrent();

    ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
    virtual StatusCode fillHistograms();

    ///@copydoc InDetGlobalMotherMonTool::procHistograms()
    virtual StatusCode procHistograms();

    /// Functions to fill individual sets of histograms
    void FillForwardTracks( const Trk::Track *track, const Trk::TrackSummary* summary );
    void FillEtaPhi( const Trk::Track *track, const Trk::TrackSummary* summary );
    void FillHits( const Trk::Track *track, const Trk::TrackSummary* summary );
    void FillHoles( const Trk::Track *track, const Trk::TrackSummary* summary );
    void FillHitMaps( const Trk::Track *track );
    ///@} 
 
private:
    ///Switch if LB accounting should be done
    bool m_doLumiblock;
    /// Switch for hole searching
    bool m_doHolePlots;
    /// Switch for hitmaps
    bool m_doHitMaps;
    bool m_DoHoles_Search;

    unsigned int m_nBinsEta;
    unsigned int m_nBinsPhi;
    unsigned int m_trackBin;
    unsigned int m_trackMax;
    float m_d0Max;
    float m_z0Max;

    /// Contants for various histogram properties
    const float c_etaRange;
    const float c_etaTrackletsMin;
    const float c_etaTrackletsMax;
    const float c_etaRangeTRT;

    const float c_EndCapBoundary;
    const float c_BarrelBoundary;

    ToolHandle <Trk::ITrackHoleSearchTool> m_holes_search_tool;
    ToolHandle <Trk::ITrackSummaryTool> m_trkSummaryTool;

    std::string m_CombinedTracksName;
    std::string m_ForwardTracksName;

    //--- Shift histograms ----------------------------------------
    
    // Gaetano Holes : 
    // SCT Holes : 
    TH1F * sct_holes;
    // TRT Holes : 
    TH1F * trt_holes;
    // Pixel Holes : 
    TH1F * pixel_holes;
    // Combined Holes : 
    TH1F * comb_holes;
    // Silicon  vs TRT holes 
    TH2F  *silicon_vs_trt;
    TH2F  *sct_vs_pixels;
    
    // Combined number of holes vs track quality
    TH2F *holes_quality;
    TProfile *holes_quality_profile;
    
    ///Distribution of eta vs phi for combined tracks
    TH2F  * m_ID_COMB_eta_phi;

    TH2F  * m_ID_COMB_noTRText_eta_phi;
    TH2F  * m_ID_COMB_noSCThits_eta_phi;
    TH2F  * m_ID_COMB_noPIXhits_eta_phi;
    TH2F  * m_ID_COMB_noPIXhitsRatio_eta_phi;
    TH2F  * m_ID_COMB_btagqual_eta_phi;

    TH2F  * m_ID_COMB_noBLayerHitExpected_eta_phi;
    TH2F  * m_ID_COMB_noBLayerHitExpectedRatio_eta_phi;
    TH2F  * m_ID_COMB_noBLayerHit_eta_phi;
    TH2F  * m_ID_COMB_noBLayerHitRatio_eta_phi;

    ///d0 parameter for combined tracks
    TH1F  * m_ID_COMB_d0;

    ///z0 parameter for combined tracks
    TH1F  * m_ID_COMB_z0;

    TH1F  * m_ID_COMB_qoverp;
    TH1F  * m_ID_COMB_chi2;
    TH1F  * m_FORW_qoverp;
    TH1F  * m_FORW_chi2;
    TH1F  * m_ID_COMB_pt;

    /// Number of combined segments
    TH1I * m_ID_nCOMBtrks;

    //--- Combined tracks debug histograms-----------------------------------
    TH2F * m_COMB_goodPixTrk_eta_phi;
    TH2F * m_COMB_goodPixTrkRatio_eta_phi;

    TH2F * m_FORW_forwardTracklets_pos_eta_phi;
    TH2F * m_FORW_forwardTracklets_neg_eta_phi;

    TH2F * m_COMB_goodSctTrk_eta_phi;
    TH2F * m_COMB_goodSctTrkRatio_eta_phi;

    TH2F * m_COMB_goodTrtTrk_eta_phi;
    TH2F * m_COMB_goodEleMTrk_eta_phi;

    // Total number of tracks per LB for each subdetector 
    TProfile * m_COMB_nCOMBtrks_LB;

    TH1I * m_PIX_ntrk_LB;
    TH1I * m_PIX_ntrk_noPIXhits_LB;
    TH1I * m_PIX_ntrk_noBLhits_LB;
    TH1I * m_SCT_ntrk_LB;
    TH1I * m_TRT_ntrk_LB;

    // Profile plots of the track rate vs LB number
    TH1F* m_trk_rate_pix_LB;
    TH1F* m_trk_rate_sct_LB;
    TH1F* m_trk_rate_trt_LB;

    // LB parameters
    unsigned int m_LB_start_time;
    unsigned int m_LB_current_time;  //Timestamp of current event in the LB
    unsigned int m_current_LB;       //LB number
    unsigned int m_range_LB;

    ///@name Detector managesr
    ///{@

    /// the TRT ID helper
    const TRT_ID *m_trtID;
    
    /// the SCT ID helper 
    const SCT_ID *m_sctID;  

    /// the Pixel ID helper 
    const PixelID *m_pixelID;
    
    ///@}

    // changes Holes mapping 
    TH2F     *m_holes_eta_phi;
    TProfile2D  *m_holes_eta_pt;
    TProfile2D  *m_holes_phi_pt;
    TProfile2D  *m_holes_eta_phi_n;
    TH1F     *m_holesComb2;
    TProfile *m_holes_hits;
    TH2F     *m_holesvshits;
    TH2F     *m_holesvshits_ECA;
    TH2F     *m_holesvshits_ECC;
    TH2F    *m_holesvshits_BA;

    TH2F     *m_ID_hitmap_x_y;
    TH2F     *m_ID_hitmap_x_y_eca;
    TH2F     *m_ID_hitmap_x_y_ecc;
    TH2F     *m_HolesMAP_XY;
    TH2F     *m_HolesMAP_ZX;
    TH2F     *m_HolesMAP_ZR;

    ///Number of PIX hits per track
    TH1I *  m_Trk_nPIXhits;
    TH1I *  m_Trk_nPIXhits_EA;
    TH1I *  m_Trk_nPIXhits_TA;
    TH1I *  m_Trk_nPIXhits_B;
    TH1I *  m_Trk_nPIXhits_TC;
    TH1I *  m_Trk_nPIXhits_EC;
    TH1I *  m_Trk_nPIXhits_FA;
    TH1I *  m_Trk_nPIXhits_FC;
    TProfile2D * m_Trk_nPIXhits_eta_phi;
    TProfile2D * m_Trk_nPIXDeadModules_eta_phi;
    
    ///@}

    ///@name SCT histograms
    ///@{

    ///Number of SCT hits per track
    TH1I *  m_Trk_nSCThits;
    TH1I *  m_Trk_nSCThits_EA;
    TH1I *  m_Trk_nSCThits_TA;
    TH1I *  m_Trk_nSCThits_B;
    TH1I *  m_Trk_nSCThits_TC;
    TH1I *  m_Trk_nSCThits_EC;
    TProfile2D *  m_Trk_nSCThits_eta_phi;
    TProfile2D *  m_Trk_nSCTDeadModules_eta_phi;
    ///@}

    ///@name TRT histograms
    ///@{

    ///Number of TRT hits per track
    TH1I *  m_Trk_nTRThits;
    TH1I *  m_Trk_nTRThits_EA;
    TH1I *  m_Trk_nTRThits_TA;
    TH1I *  m_Trk_nTRThits_B;
    TH1I *  m_Trk_nTRThits_TC;
    TH1I *  m_Trk_nTRThits_EC;
    TProfile2D *  m_Trk_nTRThits_eta_phi;

    template <class histClass>
    StatusCode registerManHist ( 
	histClass * & target, std::string path, Interval_t interval,
	std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel, std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi );
	    target->GetXaxis()->SetTitle( xlabel.c_str() );
	    target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return regHist( target, path, interval );
	}

    template <class histClass>
    StatusCode registerManHist ( 
	histClass * & target, std::string path, Interval_t interval,
	std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	int nbinsy, double ylow, double yhi,
	std::string xlabel, std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi,
				    nbinsy, ylow, yhi );
	    target->GetXaxis()->SetTitle( xlabel.c_str() );
	    target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return regHist( target, path, interval );
	}
    
    template <class histClass>
    StatusCode registerHistI ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = histClass::create( name.c_str(), title.c_str(), nbinsx, xlow, xhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );

	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}

    template <class histClass>
    StatusCode registerHistI ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	int nbinsy, double ylow, double yhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = histClass::create( name.c_str(), title.c_str(), 
					nbinsx, xlow, xhi,
					nbinsy, ylow, yhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );
	    
	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    
    template <class histClass>
    StatusCode registerHistIR ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );

	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    
    template <class histClass>
    StatusCode registerHistIR ( 
	MonGroup & theGroup, histClass * & target, std::string name, std::string title,
	int nbinsx, double xlow, double xhi,
	int nbinsy, double ylow, double yhi,
	std::string xlabel = "", std::string ylabel = "" )
	{
	    target = new histClass( name.c_str(), title.c_str(), 
				    nbinsx, xlow, xhi,
				    nbinsy, ylow, yhi );
	    
	    if ( xlabel != "" )
		target->GetXaxis()->SetTitle( xlabel.c_str() );
	    
	    if ( ylabel != "" )
		target->GetYaxis()->SetTitle( ylabel.c_str() );
	    
	    return theGroup.regHist( target );
	}
    


};

#endif
