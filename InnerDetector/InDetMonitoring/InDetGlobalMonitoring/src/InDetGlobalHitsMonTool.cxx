/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalHitsMonTool.cxx
 * Implementation of inner detector global hits monitoring tool
 *
 * @author
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>     
 *
 * $Id: InDetGlobalHitsMonTool.cxx,v 1.73 2009-05-14 22:43:22 kastanas Exp $
 *
 ****************************************************************************/


//Local
#include "InDetGlobalHitsMonTool.h"
//Framework
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkTrackSummary/TrackSummary.h"


#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TProfile2D.h"


//Root
#include <string>

InDetGlobalHitsMonTool::InDetGlobalHitsMonTool(const std::string & type, 
				               const std::string & name,
				               const IInterface* parent)
    : InDetGlobalMotherMonTool(type, name, parent),
      m_updator(nullptr),
      m_iUpdator("Trk::KalmanUpdator"),
      m_nTracks(0),
      m_tracks(nullptr),
      m_ID_hitmap_x_y(nullptr),
      m_ID_hitmap_x_y_eca(nullptr),
      m_ID_hitmap_x_y_ecc(nullptr),
      m_ID_hitmap_z_x(nullptr),
      m_ID_hitmap_z_r(nullptr),
      m_ID_holes(nullptr),
      m_Trk_nSihits_nTRThits(nullptr),m_Trk_nPIXhits_nSCThits(nullptr),m_Trk_nSCThits_nTRThits(nullptr),
      m_Trk_nPIXhits(0),
      m_Trk_nPIXhits_EA(0),
      m_Trk_nPIXhits_TA(0),
      m_Trk_nPIXhits_B(0),
      m_Trk_nPIXhits_TC(0),
      m_Trk_nPIXhits_EC(0),
      m_Trk_nPIXhits_eta(0),
      m_Trk_nPIXhits_phi(0),
      m_Trk_nPIXhits_eta_phi(0),
      m_Trk_nPixhits_SCTTRTPixFid(nullptr), 
      m_Trk_nPIXhits_fidusial(0),
      m_Trk_nPIXhits_1trk(0),
      m_Trk_nSCThits_1trk(0),
      m_Trk_nSCThits(0),
      m_Trk_nSCThits_EA(0),
      m_Trk_nSCThits_TA(0),
      m_Trk_nSCThits_B(0),
      m_Trk_nSCThits_TC(0),
      m_Trk_nSCThits_EC(0),
      m_Trk_nSCThits_eta(0),
      m_Trk_nSCThits_phi(0),
      m_Trk_nSCThits_eta_phi(0),
      m_Trk_nSCThits_pt(0),
      m_Trk_nSCThits_PixTRT(0),
      m_Trk_nSCThits_withPix(0),
      m_Trk_nTRThits(0),
      m_Trk_nTRThits_EA(0),
      m_Trk_nTRThits_TA(0),
      m_Trk_nTRThits_B(0),
      m_Trk_nTRThits_TC(0),
      m_Trk_nTRThits_EC(0),
      m_Trk_nTRThits_eta(0),
      m_Trk_nTRThits_phi(0),
      m_Trk_nTRThits_eta_phi(0),
      m_Trk_nTRThits_PixSCT(nullptr),
      m_Trk_nTRTHLhits(0),
      m_Trk_nTRTLLhits(0),
      m_Trk_nTRThits_withSi(0),
      m_Trk_nTRTHLhits_1trk(0),
      m_Trk_nTRTLLhits_1trk(0),
      m_TRT_LL(0),
      m_TRT_HL(0),
      m_SCThits(0),
      m_PIXhits(0),
      m_PIXhits_fid(0),
      m_pix_barrel(0),
      m_pix_eca(0),
      m_pix_ecc(0),
      m_sct_barrel(0),
      m_sct_eca(0),
      m_sct_ecc(0),
      m_trt_barrel(0),
      m_trt_eca(0),
      m_trt_ecc(0)
      
{
   
}

StatusCode InDetGlobalHitsMonTool::initialize(){
  if ( ManagedMonitorToolBase::initialize().isFailure() )
    {
      if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Could not initialize Monitor tool base!" << endmsg;
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------
StatusCode InDetGlobalHitsMonTool::bookHistogramsRecurrent() 	
{
  bool status = true;
  
  if ( newRunFlag() ) {
    
    MonGroup monGr_shift(this, "InDetGlobal/Hits",run );
    MonGroup monGr_exp(this, "InDetGlobal/Hits", run );
    MonGroup monGr_bug(this, "InDetGlobal/Hits", run );

  

    //--- ID histograms
    status &= registerHist( monGr_shift,  m_ID_hitmap_x_y = TH2F_LW::create("m_ID_hitmap_x_y","Map of ID hits (BARREL) in x vs y (mm)",400,-1100,1100,400,-1100,1100)).isSuccess();
    status &= registerHist( monGr_shift,  m_ID_hitmap_x_y_eca = TH2F_LW::create("m_ID_hitmap_x_y_eca","Map of ID hits (ECA) in x vs y (mm)",400,-1100,1100,400,-1100,1100)).isSuccess();
    status &= registerHist( monGr_shift,  m_ID_hitmap_x_y_ecc = TH2F_LW::create("m_ID_hitmap_x_y_ecc","Map of ID hits (ECC) in x vs y (mm)",400,-1100,1100,400,-1100,1100)).isSuccess();
    status &= registerHist( monGr_shift,  m_ID_hitmap_z_x = TH2F_LW::create("m_ID_hitmap_z_x","Map of ID hits in z vs x (mm)",1000,-3100,3100,300,-1100,1100)).isSuccess();
    status &= registerHist( monGr_shift,  m_ID_hitmap_z_r = TH2F_LW::create("m_ID_hitmap_z_r","Map of ID hits in z vs r (mm)",3000,-3100,3100,1100,0,1100)).isSuccess();
    status &= registerHist( monGr_shift,  m_ID_holes = TH2I_LW::create("m_ID_holes","Number of ID holes on tracks in the subdetectors",2,0,2,3,0,3)).isSuccess();
    
    //--- Pixel histograms
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_nSCThits = TH2I_LW::create("m_Trk_nPIXhits_nSCThits","Number of SCT hits vs Pixel hits",13,-0.5,12.5,31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPixhits_SCTTRTPixFid = TH1I_LW::create("m_Trk_nPixhits_SCTTRTPixFid","Number of Pixel hits per track with a least one hit in the other sub-detectors and going through the fiducial layer",11,-0.5,10.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nSihits_nTRThits = TH2I_LW::create("m_Trk_nSihits_nTRThits","Number of Silicon hits vs TRT hits",100,0.5,100.5,31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_nTRThits = TH2I_LW::create("m_Trk_nSCThits_nTRThits","Number of SCT hits vs TRT hits",100,0.5,100.5,31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits = TH1I_LW::create("m_Trk_nPIXhits","Number of Pixel hits per track",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_EA = TH1I_LW::create("m_Trk_nPIXhits_EA","Number of Pixel hits per track (ECA)",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_TA = TH1I_LW::create("m_Trk_nPIXhits_TA","Number of Pixel hits per track (Transition region A)",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_B = TH1I_LW::create("m_Trk_nPIXhits_B","Number of Pixel hits per track (Barrel)",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_TC = TH1I_LW::create("m_Trk_nPIXhits_TC","Number of Pixel hits per track (Transition region C)",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_EC = TH1I_LW::create("m_Trk_nPIXhits_EC","Number of Pixel hits per track (ECC)",13,-0.5,12.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_eta = TProfile_LW::create("m_Trk_nPIXhits_eta","Number of Pixel hits per track, as a function of eta",25,-2.5,2.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_phi = TProfile_LW::create("m_Trk_nPIXhits_phi","Number of Pixel hits per track, as a function of phi",30,-3.2,3.2)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_eta_phi = new TProfile2D("m_Trk_nPIXhits_eta_phi","Number of Pixel hits per track, eta-phi profile",25,-2.5,2.5,30,-3.2,3.2)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nPIXhits_fidusial = TH1I_LW::create("m_Trk_nPIXhits_fiducial","Number of Pixel hits per track for tracks with |d0|<5cm and |z0|<1.4m",12,0.5,12.5)).isSuccess();
    status &= registerHist( monGr_exp , m_Trk_nPIXhits_1trk = TH1I_LW::create("m_Trk_nPIXhits_1trk","Number of Pixel hits per track - only 1 track per event",12,0.5,12.5)).isSuccess();
      
    //--- SCT histograms
    status &= registerHist( monGr_shift,  m_Trk_nSCThits = TH1I_LW::create("m_Trk_nSCThits","Number of SCT hits per track",31,-0.5,30.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_EA = TH1I_LW::create("m_Trk_nSCThits_EA","Number of SCT hits per track (ECA)",31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_TA = TH1I_LW::create("m_Trk_nSCThits_TA","Number of SCT hits per track (Transition region A)",31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_B = TH1I_LW::create("m_Trk_nSCThits_B","Number of SCT hits per track (Barrel)",31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_TC = TH1I_LW::create("m_Trk_nSCThits_TC","Number of SCT hits per track (Transition region C)",31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_EC = TH1I_LW::create("m_Trk_nSCThits_EC","Number of SCT hits per track (ECC)",31,-0.5,30.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_eta = TProfile_LW::create("m_Trk_nSCThits_eta","Number of SCT hits per track, as a function of eta",25,-2.5,2.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_phi = TProfile_LW::create("m_Trk_nSCThits_phi","Number of SCT hits per track, as a function of phi",30,-3.2,3.2)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_eta_phi = new TProfile2D("m_Trk_nSCThits_eta_phi","Number of SCT hits per track, eta-phi profile",25,-2.5,2.5,30,-3.2,3.2)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_pt = TH2I_LW::create("m_Trk_nSCThits_pt", "Number of SCT hits per track by vs track P_{T}",50,-0.,10,30,0.5,30.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_PixTRT = TH1I_LW::create("m_Trk_nSCThits_PixTRT","Number of SCT hits per track, with at least one hit in the other subdetectors",31,-0.5,30.5)).isSuccess();
    status &= registerHist( monGr_exp , m_Trk_nSCThits_1trk = TH1I_LW::create("m_Trk_nSCThits_1trk","Number of SCT hits per track - only 1 track per event",30,0.5,30.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nSCThits_withPix = TH1I_LW::create("m_Trk_nSCThits_withPix","number of SCT hits for tracks which have Pixel hit",31,-0.5,30.5)).isSuccess();    
    //--- TRT histograms
    status &= registerHist( monGr_shift,  m_Trk_nTRThits = TH1I_LW::create("m_Trk_nTRThits","Number of TRT hits per track",100,0.5,100.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_EA = TH1I_LW::create("m_Trk_nTRThits_EA","Number of TRT hits per track (ECA)",100,0.5,100.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_TA = TH1I_LW::create("m_Trk_nTRThits_TA","Number of TRT hits per track (Transition region A)",100,0.5,100.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_B = TH1I_LW::create("m_Trk_nTRThits_B","Number of TRT hits per track (Barrel)",100,0.5,100.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_TC = TH1I_LW::create("m_Trk_nTRThits_TC","Number of TRT hits per track (Transition region C)",100,0.5,100.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_EC = TH1I_LW::create("m_Trk_nTRThits_EC","Number of TRT hits per track (ECC)",100,0.5,100.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_eta = TProfile_LW::create("m_Trk_nTRThits_eta","Number of TRT hits per track, as a function of eta",30,-3.5,3.5)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_phi = TProfile_LW::create("m_Trk_nTRThits_phi","Number of TRT hits per track, as a function of phi",30,-3.2,3.2)).isSuccess(); 
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_eta_phi = new TProfile2D("m_Trk_nTRThits_eta_phi","Number of TRT hits per track, eta-phi profile",25,-2.5,2.5,30,-3.2,3.2)).isSuccess(); 

    status &= registerHist( monGr_shift,  m_Trk_nTRThits_PixSCT = TH1I_LW::create("m_Trk_nTRThits_PixSCT","Number of TRT hits per track, with at least one hit in other subdetectors",100,-0.5,100.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nTRTHLhits = TH1I_LW::create("m_Trk_nTRTHLhits","Number of high level TRT hits per track",30,0.5,30.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nTRTLLhits = TH1I_LW::create("m_Trk_nTRTLLhits","Number of low level TRT hits per track",80,0.5,80.5)).isSuccess();
    status &= registerHist( monGr_shift,  m_Trk_nTRThits_withSi = TH1I_LW::create("m_Trk_nTRThits_withSi","Number of TRT hits for tracks with both Pixel and SCT hits",100,0.5,100.5)).isSuccess();
    status &= registerHist( monGr_exp , m_Trk_nTRTHLhits_1trk = TH1I_LW::create("m_Trk_nHL_1trk","Number of high level TRT hits per track - only 1 track per event",30,0.5,30.5)).isSuccess();
    status &= registerHist( monGr_exp , m_Trk_nTRTLLhits_1trk = TH1I_LW::create("m_Trk_nLL_1trk","Number of low level TRT hits per track - only 1 track per event",80,0.5,80.5)).isSuccess();
    
    // Fix the labels for the holes histogram
    LWHist::LWHistAxis *xaxis = m_ID_holes->GetXaxis();
    LWHist::LWHistAxis *yaxis = m_ID_holes->GetYaxis();
    xaxis->SetBinLabel(1,"Pixel");
    xaxis->SetBinLabel(2,"SCT");
    yaxis->SetBinLabel(1,"ECA");
    yaxis->SetBinLabel(2,"Barrel");
    yaxis->SetBinLabel(3,"ECC");

    LWHist::LWHistAxis *xaxis_hitmap = m_ID_hitmap_x_y->GetXaxis();
    LWHist::LWHistAxis *yaxis_hitmap = m_ID_hitmap_x_y->GetYaxis();
    xaxis_hitmap->SetTitle("x [mm]");
    yaxis_hitmap->SetTitle("y [mm]");
    
    LWHist::LWHistAxis *xaxis_hitmap_z = m_ID_hitmap_z_x->GetXaxis();
    LWHist::LWHistAxis *yaxis_hitmap_z = m_ID_hitmap_z_x->GetYaxis();
    xaxis_hitmap_z->SetTitle("z [mm]");
    yaxis_hitmap_z->SetTitle("x [mm]");

    LWHist::LWHistAxis *xaxis_hitmap_r = m_ID_hitmap_z_r->GetXaxis();
    LWHist::LWHistAxis *yaxis_hitmap_r = m_ID_hitmap_z_r->GetYaxis();
    xaxis_hitmap_r->SetTitle("z [mm]");
    yaxis_hitmap_r->SetTitle("r [mm]");
    
    LWHist::LWHistAxis *xaxis_PS = m_Trk_nPIXhits_nSCThits->GetXaxis();
    LWHist::LWHistAxis *yaxis_PS = m_Trk_nPIXhits_nSCThits->GetYaxis();        
    xaxis_PS->SetTitle("Pixel hits");
    yaxis_PS->SetTitle("SCT hits");
    
    LWHist::LWHistAxis *xaxis_SiT = m_Trk_nSihits_nTRThits->GetXaxis();
    LWHist::LWHistAxis *yaxis_SiT = m_Trk_nSihits_nTRThits->GetYaxis();        
    xaxis_SiT->SetTitle("TRT hits");
    yaxis_SiT->SetTitle("Silicon hits");
    
    LWHist::LWHistAxis *xaxis_ST = m_Trk_nSCThits_nTRThits->GetXaxis();
    LWHist::LWHistAxis *yaxis_ST = m_Trk_nSCThits_nTRThits->GetYaxis();        
    xaxis_ST->SetTitle("TRT hits");
    yaxis_ST->SetTitle("SCT hits");

  }
  
  if (status) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
}

//---------------------------------------------------------
StatusCode InDetGlobalHitsMonTool::fillHistograms()
{  
  m_tracks = m_combined_tracks;
  
  if (m_evt%1000==0 && m_evt>0 )  m_ID_hitmap_x_y->Reset();
  
  if (m_tracks) { 
    TrackCollection::const_iterator itrack = m_tracks->begin();
    TrackCollection::const_iterator itrack_end = m_tracks->end();
    for ( ; itrack!= itrack_end; ++itrack) {
      m_TRT_LL = 0;
      m_TRT_HL = 0;
      m_SCThits = 0;
      m_PIXhits = 0;
      m_PIXhits_fid = 0;
      m_pix_eca = 0;
      m_pix_barrel = 0;
      m_pix_ecc = 0;
      m_sct_eca = 0;
      m_sct_barrel = 0;
      m_sct_ecc = 0;
      m_trt_eca = 0;
      m_trt_barrel = 0;
      m_trt_ecc = 0;
      
      // Get track
      const Trk::Track *track=(*itrack);
      if (track == 0){
	if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) 
	  << "no pointer to track!!!" << endmsg;  break;
      }
      
      const Trk::Perigee *measPerigee = dynamic_cast<const Trk::Perigee *>(track->perigeeParameters());

      const DataVector<const Trk::TrackStateOnSurface>* 
	trackStates=track->trackStateOnSurfaces(); 
      if (trackStates == 0) {
	if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR) 
	  << "for current track is TrackStateOnSurfaces == Null " 
	  << endmsg;
      } else {     
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=trackStates->end();
	for (;it!=it_end; ++it) {
	  const Trk::TrackStateOnSurface* trackState=(*it);
	  if ( processHit( track, trackState ) == StatusCode::FAILURE ) {}
	}//end of loop on track surfaces
	
      }//trackStates ok
      
      m_Trk_nTRThits->Fill( m_TRT_LL + m_TRT_HL );
      if ( m_TRT_LL + m_TRT_HL ){
	  m_Trk_nTRThits_eta->Fill(  measPerigee->eta(),m_TRT_LL + m_TRT_HL );
	  m_Trk_nTRThits_phi->Fill(  measPerigee->parameters()[Trk::phi0],m_TRT_LL + m_TRT_HL );
	  m_Trk_nTRThits_eta_phi->Fill( measPerigee->eta(), measPerigee->parameters()[Trk::phi0],m_TRT_LL + m_TRT_HL );

	  if ( measPerigee->eta() < -1.6 )
	      m_Trk_nTRThits_EA->Fill(m_TRT_LL + m_TRT_HL);
	  else if ( measPerigee->eta() < -0.8 )
	      m_Trk_nTRThits_TA->Fill(m_TRT_LL + m_TRT_HL);
	  else if ( measPerigee->eta() < 0.8 )
	      m_Trk_nTRThits_B->Fill(m_TRT_LL + m_TRT_HL);
	  else if ( measPerigee->eta() < 1.6 )
	      m_Trk_nTRThits_TC->Fill(m_TRT_LL + m_TRT_HL);
	  else 
	      m_Trk_nTRThits_EC->Fill(m_TRT_LL + m_TRT_HL);

	  m_Trk_nTRTLLhits->Fill(m_TRT_LL);
	  m_Trk_nTRTHLhits->Fill(m_TRT_HL);
      }

      if ( m_SCThits > 0 && m_PIXhits > 0 ){
        m_Trk_nTRThits_PixSCT->Fill(m_TRT_LL + m_TRT_HL);
      }
      if ( m_SCThits ){ 
	  m_Trk_nSCThits_eta->Fill(  measPerigee->eta(),m_SCThits );
	  m_Trk_nSCThits_phi->Fill(  measPerigee->parameters()[Trk::phi0],m_SCThits );
	  m_Trk_nSCThits_eta_phi->Fill(  measPerigee->eta(), measPerigee->parameters()[Trk::phi0],m_SCThits );

	  if ( measPerigee->eta() < -1.6 )
	      m_Trk_nSCThits_EA->Fill(m_SCThits);
	  else if ( measPerigee->eta() < -0.8 )
	      m_Trk_nSCThits_TA->Fill(m_SCThits);
	  else if ( measPerigee->eta() < 0.8 )
	      m_Trk_nSCThits_B->Fill(m_SCThits);
	  else if ( measPerigee->eta() < 1.6 )
	      m_Trk_nSCThits_TC->Fill(m_SCThits);
	  else 
	      m_Trk_nSCThits_EC->Fill(m_SCThits);

	  m_Trk_nSCThits->Fill(m_SCThits);

	  m_Trk_nSCThits_pt->Fill( measPerigee->pT()/1000.0, m_SCThits );
      }
      
      if ( m_TRT_LL + m_TRT_HL > 0 && m_PIXhits > 0 ){
	m_Trk_nSCThits_PixTRT->Fill(m_SCThits);
      }
      if ( m_TRT_LL + m_TRT_HL > 0 && m_SCThits > 0 && fabs(measPerigee->parameters()[Trk::d0]) < 5 && fabs(measPerigee->parameters()[Trk::z0]) < 140 ){
	m_Trk_nPixhits_SCTTRTPixFid->Fill(m_PIXhits);
      }
      if ( m_PIXhits ){
	  m_Trk_nPIXhits_eta->Fill( measPerigee->eta(),m_PIXhits );
	  m_Trk_nPIXhits_phi->Fill( measPerigee->parameters()[Trk::phi0],m_PIXhits );
	  m_Trk_nPIXhits_eta_phi->Fill( measPerigee->eta(), measPerigee->parameters()[Trk::phi0], m_PIXhits );
	  
	  m_Trk_nPIXhits->Fill(m_PIXhits);
	  
	  if ( measPerigee->eta() < -1.6 )
	      m_Trk_nPIXhits_EA->Fill(m_PIXhits);
	  else if ( measPerigee->eta() < -0.8 )
	      m_Trk_nPIXhits_TA->Fill(m_PIXhits);
	  else if ( measPerigee->eta() < 0.8 )
	      m_Trk_nPIXhits_B->Fill(m_PIXhits);
	  else if ( measPerigee->eta() < 1.6 )
	      m_Trk_nPIXhits_TC->Fill(m_PIXhits);
	  else 
	      m_Trk_nPIXhits_EC->Fill(m_PIXhits);
      }
      
      m_Trk_nPIXhits_fidusial->Fill(m_PIXhits_fid);
      m_Trk_nPIXhits_nSCThits->Fill(m_PIXhits,m_SCThits);
      m_Trk_nSihits_nTRThits->Fill(m_TRT_LL+m_TRT_HL,m_PIXhits+m_SCThits);
      m_Trk_nSCThits_nTRThits->Fill(m_TRT_LL+m_TRT_HL,m_SCThits);
      if (m_PIXhits > 0) 
	m_Trk_nSCThits_withPix->Fill(m_SCThits);
      if (m_PIXhits > 0 && m_SCThits > 0) 
	m_Trk_nTRThits_withSi->Fill(m_TRT_LL+m_TRT_HL);
      
      if ( itrack == m_tracks->begin()) { 
	m_Trk_nTRTLLhits_1trk->Fill(m_TRT_LL);
	m_Trk_nTRTHLhits_1trk->Fill(m_TRT_HL);
	m_Trk_nSCThits_1trk->Fill(m_SCThits);
	m_Trk_nPIXhits_1trk->Fill(m_PIXhits);
      }   
    } // end of loop on tracks
  } // if m_tracks != 0
  
  m_evt++;
  
  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalHitsMonTool::procHistograms()
{
  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalHitsMonTool::processHit( const Trk::Track *track, const Trk::TrackStateOnSurface* trackState )
{
    if (trackState == 0) {
	if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "TrackStateOnSurface == Null, is the tracking OK?" << endmsg;
	return StatusCode::FAILURE;
    }
    //if ( !(trackState->type() == 0) ) {
    //  return StatusCode::SUCCESS;
    //}
    
    //TRT
    const InDet::TRT_DriftCircleOnTrack *trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trackState->measurementOnTrack());
    if (trtcircle!=0) {
	//m_ID_hitmap_x_y->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	if ( trtcircle->globalPosition()[2] < -800 ) 
	    m_ID_hitmap_x_y_ecc->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	else if ( trtcircle->globalPosition()[2] > 800 )
	    m_ID_hitmap_x_y_eca->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	else
	    m_ID_hitmap_x_y->Fill( trtcircle->globalPosition()[0], trtcircle->globalPosition()[1] );
	
	m_ID_hitmap_z_x->Fill( trtcircle->globalPosition()[2], trtcircle->globalPosition()[0] );
	m_ID_hitmap_z_r->Fill( trtcircle->globalPosition()[2], sqrt( pow( trtcircle->globalPosition()[0], 2) + pow( trtcircle->globalPosition()[1], 2) ) );
	
	if  (trtcircle->highLevel()) m_TRT_HL++;
	else m_TRT_LL++;
	
	switch ( m_trtID->barrel_ec( trtcircle->identify() ) ) {
	case -2:
	    m_trt_ecc++;
	    break;
	case -1:
	    m_trt_barrel++;
	    break;
	case 1:
	    m_trt_barrel++;
	    break;
	case 2:
	    m_trt_eca++;
	    break;
	}
	
    }
    //SILICON (SCT + Pixel)
    const InDet::SiClusterOnTrack *clus =
	dynamic_cast<const InDet::SiClusterOnTrack*>(trackState->measurementOnTrack());
    if (clus) {
	m_ID_hitmap_z_x->Fill( clus->globalPosition()[2], clus->globalPosition()[0] );
	m_ID_hitmap_z_r->Fill( clus->globalPosition()[2], sqrt( pow( clus->globalPosition()[0], 2) + pow( clus->globalPosition()[1], 2) ) );
	const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
	if (RawDataClus==0) {
	    if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING)
		<< "SiCluster WITHOUT prepRawData!!!!"
	    << endmsg;
	} else {
	    if (RawDataClus->detectorElement()->isSCT()) {
		switch ( m_sctID->barrel_ec( RawDataClus->identify() ) ) {
		case -2:
		    m_sct_ecc++;
		    m_ID_hitmap_x_y_ecc->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		case 0:
		    m_sct_barrel++;
		    m_ID_hitmap_x_y->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		case 2:
		    m_sct_eca++;
		    m_ID_hitmap_x_y_eca->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		}
		m_SCThits++;
	    }else{
		m_PIXhits++;
		switch ( m_pixelID->barrel_ec( RawDataClus->identify() ) ) {
		case -2:
		    m_pix_ecc++;
		    m_ID_hitmap_x_y_ecc->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		case 0:
		    m_pix_barrel++;
		    m_ID_hitmap_x_y->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		case 2:
		    m_pix_eca++;
		    m_ID_hitmap_x_y_eca->Fill( clus->globalPosition()[0], clus->globalPosition()[1] );
		    break;
		}
		const Trk::Perigee *measPerigee = dynamic_cast<const Trk::Perigee *>(track->perigeeParameters());
		if (fabs(measPerigee->parameters()[Trk::d0]) < 5 && fabs(measPerigee->parameters()[Trk::z0]) < 140)
		    m_PIXhits_fid++;
	    }
	}
    }
    return StatusCode::SUCCESS;
}

