/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalTopBottomMonTool.h 
 * Implementation of inner detector global top bottom monitoring tool
 *
 * @author
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalTopBottomMonTool.h,v 1.16 2008-12-19 11:02:55 sandaker Exp $
 *
 ****************************************************************************/


#ifndef InDetGlobalTopBottomMonTool_H
#define InDetGlobalTopBottomMonTool_H

//Local
#include "InDetGlobalMotherMonTool.h"
//Standard c++
#include <string>
//Predeclarations
class TH1F;
class IInterface;
class StatusCode;

/// Monitoring tool derived from InDetGlobalMotherMonTool
/// This tool checks the discrepancy in track at the top and bottom of the detector

class InDetGlobalTopBottomMonTool : public InDetGlobalMotherMonTool
{
    
public:
    ///Constructor
    InDetGlobalTopBottomMonTool( 
	const std::string & type, 
	const std::string & name,
	const IInterface* parent); 
    
    ///Virtual desturctor
    virtual ~InDetGlobalTopBottomMonTool() {}

    ///@name histos Book, fill and proc histograms
    ///@{

    ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistogramsRecurrent() ;     

    ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
    virtual StatusCode fillHistograms();
    
    ///@} 
  
 
private:
    
    ///
    std::string m_detector;

    ///
    const TrackCollection* m_tracks_top;

    ///
    const TrackCollection* m_tracks_bottom;
    
    // Top/Bottom histograms
    TH1F*  m_nTopTrks;
    TH1F*  m_nBottomTrks;
 
    TH1F*  m_top_d0; 
    TH1F*  m_top_z0;
    TH1F*  m_top_phi; 
    TH1F*  m_top_dphi; 
    TH1F*  m_top_eta;
    TH1F*  m_top_chi2;
  
    TH1F*  m_bottom_d0; 
    TH1F*  m_bottom_z0;
    TH1F*  m_bottom_phi;
    TH1F*  m_bottom_dphi;
    TH1F*  m_bottom_eta;
    TH1F*  m_bottom_chi2;

    // Top/Bottom histograms per one track  
    TH1F*  m_top_d0_1trk; 
    TH1F*  m_top_z0_1trk;
    TH1F*  m_top_phi_1trk; 
    TH1F*  m_top_dphi_1trk; 
    TH1F*  m_top_eta_1trk;
    TH1F*  m_top_chi2_1trk;
  
    TH1F*  m_bottom_d0_1trk; 
    TH1F*  m_bottom_z0_1trk;
    TH1F*  m_bottom_phi_1trk;
    TH1F*  m_bottom_dphi_1trk;
    TH1F*  m_bottom_eta_1trk;
    TH1F*  m_bottom_chi2_1trk;
 
    TH1F*  m_top_bottom_dd0; 
    TH1F*  m_top_bottom_dz0;
    TH1F*  m_top_bottom_dphi; 
    TH1F*  m_top_bottom_deta;


    // Pull
    TH1F*  m_top_bottom_d0_pull; 
    TH1F*  m_top_bottom_z0_pull;
    TH1F*  m_top_bottom_phi_pull; 
    TH1F*  m_top_bottom_eta_pull;
 
    /// 
    static const unsigned int s_nMaxTracks = 100;

    float m_Top_eta[s_nMaxTracks];
    float m_Top_phi[s_nMaxTracks];
    float m_Top_d0[s_nMaxTracks];
    float m_Top_z0[s_nMaxTracks];
    float m_Top_chi2[s_nMaxTracks];

    float m_Bottom_eta[s_nMaxTracks];
    float m_Bottom_phi[s_nMaxTracks];
    float m_Bottom_d0[s_nMaxTracks];
    float m_Bottom_z0[s_nMaxTracks];
    float m_Bottom_chi2[s_nMaxTracks];

 
    float m_Top_deta[s_nMaxTracks];
    float m_Top_dphi[s_nMaxTracks];
    float m_Top_dd0[s_nMaxTracks];
    float m_Top_dz0[s_nMaxTracks];

    float m_Bottom_deta[s_nMaxTracks];
    float m_Bottom_dphi[s_nMaxTracks];
    float m_Bottom_dd0[s_nMaxTracks];
    float m_Bottom_dz0[s_nMaxTracks];
	
	const TrackCollection *m_sct_tracks_up;
    const TrackCollection *m_trt_tracks_up;
    const TrackCollection *m_pix_tracks_up; 
    const TrackCollection *m_combined_tracks_up;
    const TrackCollection *m_sct_tracks_low;
    const TrackCollection *m_trt_tracks_low;
    const TrackCollection *m_pix_tracks_low;  
    const TrackCollection *m_combined_tracks_low;
	/// Name of SCT tracks container
    std::string m_SCTTracksName;
	
    /// Name of TRT tracks container
    std::string m_TRTTracksName;
	
    /// Name of pixel tracks container
    std::string m_PIXTracksName;
	
    /// Name of combined inner detector tracks container
    std::string m_CombinedTracksName;
	/// Name of trt drift circle container
    std::string m_TRT_DriftCircleName;
	bool m_doTopBottom;
};

#endif
