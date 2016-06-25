/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for tracks (combined ID and pixel only tracks)
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"         
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include <sstream>
#include <algorithm>

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "TrkTrack/TrackCollection.h"            
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StatusCode PixelMainMon::BookTrackMon(void)
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Tracks" << endreq;  

   std::string path = "Pixel/Track";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/TrackOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/TrackOnPixelTrack");
   MonGroup trackHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   std::string modlabel[9];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";    modlabel[4]="B2"; 
   modlabel[5]="IBL"; modlabel[6]="IBL2D"; modlabel[7]="IBL3D"; 
   std::string hname;
   std::string htitles;

   if (m_doOnPixelTrack) {
     sc=trackHistos.regHist(m_track_res_phi   = TH1F_LW::create("m_Pixel_track_res_phi", ("Pixel Residual LocX (pixel tracks)" + m_histTitleExt).c_str(),100,-0.1,0.1));
     sc=trackHistos.regHist(m_track_pull_phi  = TH1F_LW::create("m_Pixel_track_pull_phi", ("m_Pixel_track_pull_phi (pixel tracks)" + m_histTitleExt).c_str(),100,-1.2,1.2));      
     sc=trackHistos.regHist(m_track_res_eta   = TH1F_LW::create("m_Pixel_track_res_eta", ("Pixel Residual LocY (pixel tracks)" + m_histTitleExt).c_str(),100,-0.3,0.3));
     sc=trackHistos.regHist(m_track_pull_eta  = TH1F_LW::create("m_Pixel_track_pull_eta", ("m_Pixel_track_pull_eta (pixel tracks)" + m_histTitleExt).c_str(),100,-2,2));   
     sc=trackHistos.regHist(m_track_chi2      = TH1F_LW::create("m_Pixel_track_chi2", ("chi2 of reconstructed track (pixel tracks)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));

     if(m_doModules){
       m_tsos_hiteff_vs_lumi = new PixelMonModulesProf("TSOS_HitEfficiency",("TSOS-based hit efficiency in module" + m_histTitleExt).c_str(),2500,-0.5,2499.5,m_doIBL);
       sc = m_tsos_hiteff_vs_lumi->regHist(this,(path+"/Modules_TSOSHitEff").c_str(),run, m_doIBL);
     }

     if(!m_doOnline && m_doModules){
       sc=trackHistos.regHist(m_clustot_vs_pt = TH2F_LW::create("m_clustot_vs_pt",("Cluster ToT vs Track pT" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),10,0,50,250,0,250)); 
       sc=trackHistos.regHist(m_clustot_lowpt = TH1F_LW::create("m_clustot_lowpt",("Cluster ToT vs Track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_clustot_highpt = TH1F_LW::create("m_clustot_highpt",("Cluster ToT vs Track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_1hitclustot_lowpt = TH1F_LW::create("m_1hitclustot_lowpt",("1-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_1hitclustot_highpt = TH1F_LW::create("m_1hitclustot_highpt",("1-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_2hitclustot_lowpt = TH1F_LW::create("m_2hitclustot_lowpt",("2-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_2hitclustot_highpt = TH1F_LW::create("m_2hitclustot_highpt",("2-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + " Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_track_chi2_bcl1 = TH1F_LW::create("m_Pixel_track_chi2_bcl1", ("track chi2 with 1 1-hit, low-ToT cluster" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl0 = TH1F_LW::create("m_Pixel_track_chi2_bcl0", ("track chi2 with 0 1-hit, low-ToT clusters" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bclgt1 = TH1F_LW::create("m_Pixel_track_chi2_bclgt1", ("track chi2 with >1 1-hit, low-ToT cluster" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl1_highpt", ("track chi2 with 1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl0_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl0_highpt", ("track chi2 with 0 1-hit, low-ToT clusters (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bclgt1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bclgt1_highpt", ("track chi2 with >1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
     }

     //sc=trackHistos.regHist(m_track_clusToT = TH1F_LW::create("m_track_clusToT",("Cluster on track ToT check"+ m_histTitleExt +"; ToT;").c_str(),250,0.,250.));
     //sc=trackHistos.regHist(m_track_qOverP    = TH1F_LW::create("m_Pixel_track_qOverP", ("Reconstructed inverse momentum of track (pixel tracks)" + m_histTitleExt + "; inverse momentum [GeV^{-1}];").c_str(), 100,-7.5,7.5));
     //sc=trackHistos.regHist(m_track_qOverP_inv= TH1F_LW::create("m_Pixel_track_qOverP_inv", ("Reconstructed momentum of track (pixel tracks)" + m_histTitleExt + "; momentum [GeV];").c_str(), 100,-20,20));
     //sc=trackHistos.regHist(m_track_pt        = TH1F_LW::create("m_Pixel_track_pt", ("Reconstructed transverse momentum of track (pixel tracks)" + m_histTitleExt + "; p_{T} [GeV];").c_str(), 50,0.,10.));
     //sc=trackHistos.regHist(m_track_d0        = TH1F_LW::create("m_Pixel_track_d0", ("Reconstructed d0 of track (pixel tracks)" + m_histTitleExt + "; d_{0} [mm];").c_str(), 80,-20.,20.));
     //sc=trackHistos.regHist(m_track_z0        = TH1F_LW::create("m_Pixel_track_z0", ("Reconstructed z0 of track (pixel tracks)" + m_histTitleExt + "; z_{0} [mm];").c_str(), 80,-400.,400.));
     //sc=trackHistos.regHist(m_track_phi0      = TH1F_LW::create("m_Pixel_track_phi0", ("Reconstructed phi0 of track (pixel tracks)" + m_histTitleExt + "; #phi_{0};").c_str(), 40,-4.,4.));
     //sc=trackHistos.regHist(m_track_theta     = TH1F_LW::create("m_Pixel_track_theta", ("Reconstructed theta of track (pixel tracks)" + m_histTitleExt + "; #theta;").c_str(), 40,-0.,4.));
     //sc=trackHistos.regHist(m_track_eta       = TH1F_LW::create("m_Pixel_track_eta", ("Reconstructed eta of track (pixel tracks)" + m_histTitleExt + "; #eta;").c_str(), 40,-4.,4.));
     //sc=trackHistos.regHist(m_tracks_per_lumi = TH1I_LW::create("tracks_per_lumi", ("Number of tracks per LB (pixel tracks)" + m_histTitleExt + ";lumi block;# tracks").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_tracksPerEvt_per_lumi = TProfile_LW::create("tracksPerEvt_per_lumi", ("Number of tracks per event per LB (pixel tracks)" + m_histTitleExt + ";lumi block;tracks/event").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_trackRate_per_lumi    = TH1F_LW::create("trackRate_per_lumi", ("Track rate per LB (pixel tracks)" + m_histTitleExt + ";lumi block;#tracks/sec").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_track_dedx      = TH2F_LW::create("m_Pixel_track_dedx", ("Reconstructed dE/dx of track (pixel tracks)" + m_histTitleExt + "; momentum [MeV]; dE/dx [MeV g cm^{-2}]").c_str(),125,-2500,2500,100,0,10));
     //sc=trackHistos.regHist(m_track_mass_dedx = TH1F_LW::create("m_Pixel_track_mass_dedx", ("Reconstructed mass using dE/dx of track (pixel tracks)" + m_histTitleExt + "; mass [MeV]; ").c_str(),125,0,2500));
   } else {
     sc=trackHistos.regHist(m_track_res_phi   = TH1F_LW::create("m_Pixel_track_res_phi", ("Pixel Residual LocX" + m_histTitleExt).c_str(),100,-0.1,0.1));
     sc=trackHistos.regHist(m_track_pull_phi  = TH1F_LW::create("m_Pixel_track_pull_phi", ("m_Pixel_track_pull_phi" + m_histTitleExt).c_str(),100,-1.2,1.2));      
     sc=trackHistos.regHist(m_track_res_eta   = TH1F_LW::create("m_Pixel_track_res_eta", ("Pixel Residual LocY" + m_histTitleExt).c_str(),100,-0.3,0.3));
     sc=trackHistos.regHist(m_track_pull_eta  = TH1F_LW::create("m_Pixel_track_pull_eta", ("m_Pixel_track_pull_eta" + m_histTitleExt).c_str(),100,-2,2));   
     sc=trackHistos.regHist(m_track_chi2      = TH1F_LW::create("m_Pixel_track_chi2", ("chi2 of reconstructed track" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));

     if(m_do2DMaps && !m_doOnline){
       m_tsos_hitmap = new PixelMon2DMapsLW("TSOS_Measurement", ("TSOS of type Measurement" + m_histTitleExt), m_doIBL, false);
       sc = m_tsos_hitmap->regHist(trackHistos, m_doIBL, false);
       m_tsos_holemap = new PixelMon2DMapsLW("TSOS_Hole", ("TSOS of type Hole" + m_histTitleExt), m_doIBL, false);
       sc = m_tsos_holemap->regHist(trackHistos, m_doIBL, false);
       m_tsos_outliermap = new PixelMon2DMapsLW("TSOS_Outlier", ("TSOS of type Outlier" + m_histTitleExt), m_doIBL, false);
       sc = m_tsos_outliermap->regHist(trackHistos, m_doIBL, false);

       //m_tsos_measratio = new PixelMon2DProfilesLW("TSOS_MeasRatio", ("TSOS of type Meas per track" + m_histTitleExt), m_doIBL, false);
       //sc = m_tsos_measratio->regHist(trackHistos, m_doIBL, false);
       //m_tsos_holeratio = new PixelMon2DProfilesLW("TSOS_HoleRatio", ("TSOS of type Hole per track" + m_histTitleExt), m_doIBL, false);
       //sc = m_tsos_holeratio->regHist(trackHistos, m_doIBL, false);
       m_misshits_ratio = new PixelMon2DProfilesLW("MissHitsRatioOnTrack", ("Hole+Outlier per track" + m_histTitleExt), m_doIBL, false);
       sc = m_misshits_ratio->regHist(trackHistos, m_doIBL, false);
     }
     if(m_doOnline){
       m_tsos_holeratio_tmp = new PixelMon2DProfilesLW("HoleRatio_tmp", ("TSOS of type Hole per track tmp" + m_histTitleExt), m_doIBL, false);
       sc = m_tsos_holeratio_tmp->regHist(trackHistos, m_doIBL, false);
       m_tsos_holeratio_mon = new PixelMon2DProfilesLW("HoleRatio_mon", ("TSOS of type Hole per track for monitoring" + m_histTitleExt), m_doIBL, false);
       sc = m_tsos_holeratio_mon->regHist(trackHistos, m_doIBL, false);
       m_misshits_ratio_tmp = new PixelMon2DProfilesLW("MissHitsRatioOnTrack_tmp", ("Hole+Outlier per track" + m_histTitleExt), m_doIBL, false);
       sc = m_misshits_ratio_tmp->regHist(trackHistos, m_doIBL, false);
       m_misshits_ratio_mon = new PixelMon2DProfilesLW("MissHitsRatioOnTrack_mon", ("Hole+Outlier per track for monitoring" + m_histTitleExt), m_doIBL, false);
       sc = m_misshits_ratio_mon->regHist(trackHistos, m_doIBL, false);
     }

     if(m_doModules){
       m_tsos_hiteff_vs_lumi = new PixelMonModulesProf("TSOS_HitEfficiency",("TSOS-based hit efficiency in module" + m_histTitleExt).c_str(),2500,-0.5,2499.5,m_doIBL);
       sc = m_tsos_hiteff_vs_lumi->regHist(this,(path+"/Modules_TSOSHitEff").c_str(),run, m_doIBL);
     }

     if(!m_doOnline && m_doModules){
       sc=trackHistos.regHist(m_clustot_lowpt = TH1F_LW::create("m_clustot_lowpt",("Cluster ToT vs Track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_clustot_highpt = TH1F_LW::create("m_clustot_highpt",("Cluster ToT vs Track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_1hitclustot_lowpt = TH1F_LW::create("m_1hitclustot_lowpt",("1-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_1hitclustot_highpt = TH1F_LW::create("m_1hitclustot_highpt",("1-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_2hitclustot_lowpt = TH1F_LW::create("m_2hitclustot_lowpt",("2-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_2hitclustot_highpt = TH1F_LW::create("m_2hitclustot_highpt",("2-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + " Track pT; Cluster ToT (on track)").c_str(),250,0,250));
       sc=trackHistos.regHist(m_clustot_vs_pt = TH2F_LW::create("m_clustot_vs_pt",("Cluster ToT vs Track pT" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),10,0,50,250,0,250));
       sc=trackHistos.regHist(m_track_chi2_bcl1 = TH1F_LW::create("m_Pixel_track_chi2_bcl1", ("track chi2 with 1 1-hit, low-ToT cluster" + m_histTitleExt + "; #chi^{ 2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl0 = TH1F_LW::create("m_Pixel_track_chi2_bcl0", ("track chi2 with 0 1-hit, low-ToT clusters" + m_histTitleExt + "; #chi^ {2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bclgt1 = TH1F_LW::create("m_Pixel_track_chi2_bclgt1", ("track chi2 with >1 1-hit, low-ToT cluster" + m_histTitleExt + "; # chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl1_highpt", ("track chi2 with 1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bcl0_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl0_highpt", ("track chi2 with 0 1-hit, low-ToT clusters (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
       sc=trackHistos.regHist(m_track_chi2_bclgt1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bclgt1_highpt", ("track chi2 with >1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
     }

     //sc=trackHistos.regHist(m_track_clusToT = TH1F_LW::create("m_track_clusToT",("Cluster on track ToT check"+ m_histTitleExt +"; ToT;").c_str(),250,0.,250.));
     //sc=trackHistos.regHist(m_track_qOverP    = TH1F_LW::create("m_Pixel_track_qOverP", ("Reconstructed inverse momentum of track" + m_histTitleExt + "; inverse momentum [GeV^{-1}];").c_str(), 100,-7.5,7.5));
     //sc=trackHistos.regHist(m_track_qOverP_inv= TH1F_LW::create("m_Pixel_track_qOverP_inv", ("Reconstructed momentum of track" + m_histTitleExt + "; momentum [GeV];").c_str(), 100,-20,20));
     //sc=trackHistos.regHist(m_track_pt        = TH1F_LW::create("m_Pixel_track_pt", ("Reconstructed transverse momentum of track" + m_histTitleExt + "; p_{T} [GeV];").c_str(), 50,0.,10.));
     //sc=trackHistos.regHist(m_track_d0        = TH1F_LW::create("m_Pixel_track_d0", ("Reconstructed d0 of track" + m_histTitleExt + "; d_{0} [mm];").c_str(), 80,-20.,20.));
     //sc=trackHistos.regHist(m_track_z0        = TH1F_LW::create("m_Pixel_track_z0", ("Reconstructed z0 of track" + m_histTitleExt + "; z_{0} [mm];").c_str(), 80,-400.,400.));
     //sc=trackHistos.regHist(m_track_phi0      = TH1F_LW::create("m_Pixel_track_phi0", ("Reconstructed phi0 of track" + m_histTitleExt + "; #phi_{0};").c_str(), 40,-4.,4.));
     //sc=trackHistos.regHist(m_track_theta     = TH1F_LW::create("m_Pixel_track_theta", ("Reconstructed theta of track" + m_histTitleExt + "; #theta;").c_str(), 40,-0.,4.));
     //sc=trackHistos.regHist(m_track_eta       = TH1F_LW::create("m_Pixel_track_eta", ("Reconstructed eta of track" + m_histTitleExt + "; #eta;").c_str(), 40,-4.,4.));
     //sc=trackHistos.regHist(m_tracks_per_lumi = TH1I_LW::create("tracks_per_lumi", ("Number of tracks per LB" + m_histTitleExt + ";lumi block;# tracks").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_tracksPerEvt_per_lumi = TProfile_LW::create("tracksPerEvt_per_lumi", ("Number of tracks per event per LB" + m_histTitleExt + ";lumi block;tracks/event").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_tracksPerEvtPerMu_per_lumi = TProfile_LW::create("tracksPerEvtPerMu_per_lumi", ("Number of tracks per event per mu per LB (pixel tracks)" + m_histTitleExt + ";lumi block;tracks/event").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_trackRate_per_lumi    = TH1F_LW::create("trackRate_per_lumi", ("Track rate per LB" + m_histTitleExt + ";lumi block;#tracks/sec").c_str(),2500,-0.5,2499.5));
     //sc=trackHistos.regHist(m_track_dedx      = TH2F_LW::create("m_Pixel_track_dedx", ("Reconstructed dE/dx of track" + m_histTitleExt + "; momentum [MeV]; dE/dx [MeV g cm^{-2}]").c_str(),125,-2500,2500,100,0,10));
     //sc=trackHistos.regHist(m_track_mass_dedx = TH1F_LW::create("m_Pixel_track_mass_dedx", ("Reconstructed mass using dE/dx of track" + m_histTitleExt + "; mass [MeV]; ").c_str(),125,0,2500));
     
     sc=trackHistos.regHist(m_degFactorMap = TProfile2D_LW::create("degFactorMap", ("degradation factor map for IP resolution" + m_histTitleExt + ";track #eta;track #phi").c_str(), 60, -3.0, 3.0, 80, -4.0, 4.0));
     m_degFactorMap->SetOption("colz");
     sc=trackHistos.regHist(m_degFactorMap_per_lumi = TProfile_LW::create("degFactorMap_per_lumi", ("overall degradation factor for IP resolution per lumi" + m_histTitleExt + ";lumi block;overall avg deg. factor").c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
     sc=trackHistos.regHist(m_degFactorMap_eta_per_lumi = TProfile2D_LW::create("degFactorMap_eta_per_lumi", ("degradation factor (eta) for IP resolution per lumi" + m_histTitleExt + ";lumi block;track #eta").c_str(),  m_lbRange, -0.5, -0.5+(float)m_lbRange, 60, -3.0, 3.0 ));
     m_degFactorMap_eta_per_lumi->SetOption("colz");
     //sc=trackHistos.regHist(m_degFactorMap_phi_per_lumi = TProfile2D_LW::create("degFactorMap_phi_per_lumi", ("degradation factor (phi) for IP resolution per lumi" + m_histTitleExt + ";lumi block;track #phi").c_str(),  2500, -0.5, 2499.5, 80, -4.0, 4.0 ));
     //m_degFactorMap_phi_per_lumi->SetOption("colz");
      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("HitEff_all_"+modlabel[i]), false);
         htitles = makeHisttitle(("hit efficiency, "+modlabel[i]), ";lumi block;hit efficiency", false);
         sc = trackHistos.regHist(m_hiteff_incl_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
         hname = makeHistname(("HitEff_actv_"+modlabel[i]), false);
         htitles = makeHisttitle(("hit efficiency for active modules, "+modlabel[i]), ";lumi block;hit efficiency", false);
         sc = trackHistos.regHist(m_hiteff_actv_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
         
         /// clusters
         //hname = makeHistname(("ClusterSize_Measurement_"+modlabel[i]), false);
         //htitles = makeHisttitle(("cluster size  for clusters on tracks, "+modlabel[i]), ";cluster size;#clusters", false);
         //sc = trackHistos.regHist(m_clusize_measurement_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));
         //hname = makeHistname(("ClusterSize_Outlier_"+modlabel[i]), false);
         //htitles = makeHisttitle(("cluster size  for outliers , "+modlabel[i]), ";cluster size;#clusters", false);
         //sc = trackHistos.regHist(m_clusize_outlier_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));
      }

   }

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillTrackMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "MainFillTrackMon" << endreq;  

   StatusCode sc;
   sc = evtStore()->retrieve(m_tracks, m_TracksName);
   if (sc.isFailure())
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  <<"No tracks in StoreGate found"<< endreq;
      m_storegate_errors->Fill(4.,2.);  
      return StatusCode::SUCCESS;
   } else{
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  <<"Tracks in StoreGate found" <<endreq;
   }

   int nPixelHits=0;
   //int nTracks=0;
   if(m_doOnTrack || m_doOnPixelTrack)m_RDOIDs.clear();//reset these so you can fill them with the new id's
   if(m_doOnTrack || m_doOnPixelTrack)m_ClusterIDs.clear();
   for (int i=0; i<(int)m_tracks->size(); i++) 
   {
      const Trk::Track *track0=(*m_tracks)[i];
      if (track0 == 0) {
         if (msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "no pointer to track!!!" << endreq;
         break;
      }

      const Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track0->perigeeParameters());

      //double mass = 0;
      //double dedx = 0;
      float degradationFactor = 1.0;
      unsigned int hitslayer = 0;
      bool isEndcaps = false;
      //Check the track summary if it exists to see if there are pixel hits on this track, if no hits, skip ahead

      int nholes=-1;
      const Trk::TrackSummary* summary = track0->trackSummary();

      if (summary) {
	      nholes = summary->get(Trk::numberOfPixelHoles);

      	if (summary->get(Trk::numberOfPixelHits)==0) {
            continue;
	      } else { 
	         //dedx = summary->getPixeldEdx(); 
	         //mass = summary->indetTrackSummary()->massPixeldEdx()*1000;
	      }
      } else {
      	if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "No Track Summary Found" << endreq;
      }

      const Trk::TrackParameters *trkParameters = 0;
      int nbadclus=0;
      int ngoodclus=0;

      const Trk::Track* track = track0;
      //get the track state on surfaces (a vector, on element per surface) and loop over it

      if(m_doHoleSearch && !m_doOnline && nholes>0){
	      track = m_holeSearchTool->getTrackWithHoles(*track0);
      }

      bool passQualityCut = false;
      if( measPerigee->pT()/1000.0 > 5.0 && fabs(measPerigee->eta()) < 2.5) passQualityCut = true;

      const DataVector< const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
      for (DataVector< const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator=trackStates->begin(); trackStateOnSurfaceIterator!=trackStates->end(); trackStateOnSurfaceIterator++)
      {
         //Change the track state on 1 surface into the cluster it represents
	 const Trk::MeasurementBase* mesb=(*trackStateOnSurfaceIterator)->measurementOnTrack();
	 const InDet::SiClusterOnTrack *clus=0;
	 Identifier clusID;
	 Identifier surfaceID;
	 
	 const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : 0;
	 if (mesb && !hit) continue;  // skip pseudomeasurements etc.                                         
	 if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
	    surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	 } else { // holes, perigee                                                                              
	    if (not (*trackStateOnSurfaceIterator)->trackParameters() ) {
	       msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endreq;
	       continue;
	    }
	    surfaceID = (*trackStateOnSurfaceIterator)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();//check ptr
	 }

	 int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
	 if( pixlayer == 99) continue;
         if( pixlayer == PixLayer::kECA || pixlayer == PixLayer::kECC ) isEndcaps = true;
         IdentifierHash id_hash; 
	 bool active = false;
	 if( m_idHelper->is_pixel(surfaceID)){
            id_hash = m_pixelid->wafer_hash(surfaceID); 
            active = m_pixelCondSummarySvc->isActive(id_hash);
	 }

	 //float nMeasurement = 0.;
	 float nOutlier = 0.;
	 float nHole = 0.;

	 if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){
	    clus = dynamic_cast< const InDet::SiClusterOnTrack*>(mesb);
	    if(clus) clusID = clus->identify();
	    //nMeasurement = 1.0;
	 }
	 
	 if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)){
	   clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*trackStateOnSurfaceIterator)->measurementOnTrack());
	   if(clus) clusID = clus->identify();
	   nOutlier = 1.0;
	 }
	  
	 if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)){
	   clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*trackStateOnSurfaceIterator)->measurementOnTrack());
	   if(clus) clusID = clus->identify();
	   nHole = 1.0;
	 }


        /// Calculate the hit efficiency
	 if(m_idHelper->is_pixel(surfaceID) && (*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){
	   if( m_tsos_hitmap ) m_tsos_hitmap->Fill(surfaceID,m_pixelid,m_doIBL,false);
	   if( m_tsos_hiteff_vs_lumi ) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),1.,surfaceID,m_pixelid,m_doIBL);
	   if( m_hiteff_incl_mod[pixlayer] && passQualityCut ) m_hiteff_incl_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 1.0 );
	   if( m_hiteff_actv_mod[pixlayer] && passQualityCut && active) m_hiteff_actv_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 1.0 );
	 }

	 if(m_idHelper->is_pixel(surfaceID) && (*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)){
	   if( m_tsos_holemap ) m_tsos_holemap->Fill(surfaceID,m_pixelid,m_doIBL,false);
	   if( m_tsos_hiteff_vs_lumi ) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),0.,surfaceID,m_pixelid,m_doIBL);
	   if( m_hiteff_incl_mod[pixlayer] && passQualityCut ) m_hiteff_incl_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 0.0 );
	   if( m_hiteff_actv_mod[pixlayer] && passQualityCut && active) m_hiteff_actv_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 0.0 );
	 }
	   
	 if(m_idHelper->is_pixel(surfaceID) && (*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)){
            if(m_tsos_outliermap)m_tsos_outliermap->Fill(surfaceID,m_pixelid,m_doIBL,false);
            if(m_tsos_hiteff_vs_lumi) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),0.,surfaceID,m_pixelid,m_doIBL);
	    if( m_hiteff_incl_mod[pixlayer] && passQualityCut ) m_hiteff_incl_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 0.0 );
	    if( m_hiteff_actv_mod[pixlayer] && passQualityCut && active) m_hiteff_actv_mod[pixlayer]->Fill( m_manager->lumiBlockNumber(), 0.0 );
	 }

	 if(m_idHelper->is_pixel(surfaceID)){
	   //if(m_tsos_measratio && passQualityCut) m_tsos_measratio->Fill(surfaceID,m_pixelid,m_doIBL,false,nMeasurement);
	   //if(m_tsos_holeratio && passQualityCut) m_tsos_holeratio->Fill(surfaceID,m_pixelid,m_doIBL,false,nHole);
	   if(m_doOnline && m_tsos_holeratio_tmp && passQualityCut) m_tsos_holeratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,nHole);
	   if(passQualityCut){
             if(nOutlier + nHole > 0.){
               if(m_misshits_ratio) m_misshits_ratio->Fill(surfaceID,m_pixelid,m_doIBL,false,1.0);
               if(m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,1.0);
             }else{
               if(m_misshits_ratio) m_misshits_ratio->Fill(surfaceID,m_pixelid,m_doIBL,false,0.0);
               if(m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,0.0);
	     }
	   }
	 }

         /// clusters are valid
         if(!(*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){continue;}
         if(!clus)continue;
         const InDet::SiCluster *RawDataClus = dynamic_cast< const InDet::SiCluster*>(clus->prepRawData());
         if(!RawDataClus)continue;
	 
         //if (RawDataClus->detectorElement()->isPixel()){
         //   const InDet::PixelCluster* pixelCluster=dynamic_cast<const InDet::PixelCluster*>(RawDataClus);
         //   if(pixelCluster){
         //      float size = pixelCluster->rdoList().size();
         //      if( m_clusize_measurement_mod[pixlayer] ) m_clusize_measurement_mod[pixlayer]->Fill(size,nMeasurement);
         //      if( m_clusize_outlier_mod[pixlayer] ) m_clusize_outlier_mod[pixlayer]->Fill(size,nOutlier);
         //   }
         //}
         //if(!(*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){continue;}
         //if the cluster is a pixel cluster (not SCT) get the pixel information
         if (RawDataClus->detectorElement()->isPixel()){

            const InDet::PixelCluster* pixelCluster=dynamic_cast<const InDet::PixelCluster*>(RawDataClus);
            if(!pixelCluster){
               //std::cout<<"ERROR! Dynamic-cast to pixel cluster failed!"<<std::endl;
            } else {
               float size = pixelCluster->rdoList().size();
               float tot = pixelCluster->totalToT();
               //m_track_clusToT->Fill(pixelCluster->totalToT());
               if(size==1 && tot<8) { nbadclus++; }
               else{ ngoodclus++; }
      	    
               if(m_doModules && !m_doOnline){
	          if(measPerigee){
	             float pt = measPerigee->pT()/1000.0;
	             if(m_clustot_vs_pt) m_clustot_vs_pt->Fill(pt,tot);
	             if(pt<10){
	                if(m_clustot_lowpt) m_clustot_lowpt->Fill(tot);
	                if(size==1){ if(m_1hitclustot_lowpt) m_1hitclustot_lowpt->Fill(tot);}
	                if(size==2){ if(m_2hitclustot_lowpt) m_2hitclustot_lowpt->Fill(tot);}
	             }else{
	                if(m_clustot_highpt) m_clustot_highpt->Fill(tot);
	                if(size==1){ if(m_1hitclustot_highpt) m_1hitclustot_highpt->Fill(tot);}
	                if(size==2){ if(m_2hitclustot_highpt) m_2hitclustot_highpt->Fill(tot);}
	             }
	          }
               }
               // categorize tracks for IP resolution degradation
               if(pixlayer >= PixLayer::kECC){
                  unsigned int tmplayer = 0x1<<pixlayer;
                  if( hitslayer != tmplayer ) hitslayer += 0x1<<pixlayer;
               }

	    }

	    nPixelHits++;//add another pixel hit 
	    for(unsigned int loopSize=0;loopSize < RawDataClus->rdoList().size(); loopSize++) {
	       if(m_doOnTrack || m_doOnPixelTrack)m_RDOIDs.push_back(RawDataClus->rdoList().at(loopSize));
	    }
	         
            if(m_doOnTrack || m_doOnPixelTrack)m_ClusterIDs.push_back( clus->identify());
	    
	    // Get track parameters for current surface (with AtaPlane)
            trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
            const Trk::AtaPlane *trackAtPlane = dynamic_cast<const Trk::AtaPlane*>(trkParameters);
            if (trackAtPlane)
            {
               const Amg::Vector2D localpos = trackAtPlane->localPosition();

               // Get local error matrix for hit and track and calc pull
               const AmgSymMatrix(5) trackErrMat = (*trackAtPlane->covariance());
               const Amg::MatrixX clusErrMat = clus->localCovariance();

               //pulls and residuals Phi
               double error_sum = sqrt(pow(Amg::error(trackErrMat,Trk::locX),2) + pow(Amg::error(clusErrMat,Trk::locX), 2));
               double res = clus->localParameters()[Trk::locX]-localpos[0];
               if(m_track_res_phi) m_track_res_phi->Fill(res);
               double pull = 0;
               if(error_sum != 0) pull = (res)/error_sum;
               if(m_track_pull_phi) m_track_pull_phi->Fill(pull);

               //pulls and residuals Eta
               error_sum = sqrt(pow(Amg::error(trackErrMat,Trk::locY),2) + pow(Amg::error(clusErrMat,Trk::locY), 2));
               res = clus->localParameters()[Trk::locY]-localpos[1];
               if(m_track_res_eta) m_track_res_eta->Fill(res);
               if(error_sum != 0) pull = (res)/error_sum;
               if(m_track_pull_eta) m_track_pull_eta->Fill(pull);
            }
         } // end of if (RawDataClus->detectorElement()->isPixel())
      } // end of TSOS loop
    
      if(!m_doOnline && m_doModules){
	      float pt = measPerigee->pT()/1000.0;
	      if(nbadclus==1){
	         if(m_track_chi2_bcl1) m_track_chi2_bcl1->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      }
	      if(nbadclus==0){
	         if(m_track_chi2_bcl0) m_track_chi2_bcl0->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      } 
	      if(nbadclus>1){
	         if(m_track_chi2_bclgt1) m_track_chi2_bclgt1->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      } 
	      if(pt>=10){
	         if(nbadclus==1){
	            if(m_track_chi2_bcl1_highpt) m_track_chi2_bcl1_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         }
	         if(nbadclus==0){
	            if(m_track_chi2_bcl1_highpt) m_track_chi2_bcl0_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         } 
	         if(nbadclus>1){
	            if(m_track_chi2_bclgt1_highpt) m_track_chi2_bclgt1_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         } 
	      }
      }
            
      if(nPixelHits>0)//track properties for tracks through the pixel detector
      {
         //const  Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track0->perigeeParameters());
         //if (measPerigee != NULL){
         //   //m_track_qOverP->Fill((measPerigee->parameters()[Trk::qOverP]*1000.0));
	      //   //if (measPerigee->parameters()[Trk::qOverP] != 0) { 
	      //     //m_track_qOverP_inv->Fill((1./(measPerigee->parameters()[Trk::qOverP]*1000.0))); 
	      //     //m_track_dedx->Fill(1./(measPerigee->parameters()[Trk::qOverP]),dedx);
	      //   //}
         //   //if (std::abs(mass-139.57)>.00001&&mass>0&&dedx>1.8) m_track_mass_dedx->Fill(mass);
         //   //m_track_pt->Fill((measPerigee->pT()/1000.0));
         //   //m_track_d0->Fill(measPerigee->parameters()[Trk::d0]);
         //   //m_track_z0->Fill(measPerigee->parameters()[Trk::z0]);
         //   //m_track_phi0->Fill(measPerigee->parameters()[Trk::phi0]);
         //   //m_track_theta->Fill(measPerigee->parameters()[Trk::theta]);
         //   //m_track_eta->Fill(measPerigee->eta());
         //   if (track0->fitQuality()->numberDoF() > 0){ 
	      //      if(m_track_chi2) m_track_chi2->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      //      if(m_track_chi2_LB) m_track_chi2_LB->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      //   }else{ 
	      //      if(m_track_chi2) m_track_chi2->Fill(-1);
	      //      if(m_track_chi2_LB) m_track_chi2_LB->Fill(-1);
	      //   }	 
	      //   if(m_tracks_per_lumi) m_tracks_per_lumi->Fill(m_manager->lumiBlockNumber());
	      //   nTracks++;
	      //}
      }

      /// IP degradation factor map
      if( m_doDegFactorMap ){
         if (measPerigee){

            double eta = measPerigee->eta();
            double phi = measPerigee->parameters()[Trk::phi0];
            //float phi = 1.0; ///TEMPORALLY !!!!
            double pt = measPerigee->pT();

            //
            if(isEndcaps){
               degradationFactor = 1.0;
            }else{
               unsigned int alllayers =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int IBL       =  0x0<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int B0        =  0x1<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int B1        =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int B2        =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
               unsigned int IBLB0     =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int IBLB2     =  0x0<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
               unsigned int B0B2      =  0x1<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
               unsigned int B1B2      =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
               unsigned int IBLB0B1   =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
               unsigned int IBLB0B1B2 =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
               if( hitslayer == alllayers ) degradationFactor = 1.0; // All hits
               else{
                  if( hitslayer == IBL ) degradationFactor = 1.8; // IBL
                  else if( hitslayer == B0 ) degradationFactor = 1.16; // B0
                  else if( hitslayer == B1 ) degradationFactor = 1.0; // B1
                  else if( hitslayer == B2 ) degradationFactor = 1.0; // B2
                  else if( hitslayer == IBLB0 ) degradationFactor = 3.5; // IBL & B0
                  else if( hitslayer == IBLB2 ) degradationFactor = 1.8; // IBL & B2
                  else if( hitslayer == B0B2 ) degradationFactor = 1.17; // B & B2
                  else if( hitslayer == B1B2 ) degradationFactor = 1.0;  // B1 & B2
                  else if( hitslayer == IBLB0B1 ) degradationFactor = 6.0;  // IBL & B0 & B1
                  else if( hitslayer == IBLB0B1B2 ) degradationFactor = 6.0;  // IBL & B0 & B1
                  else degradationFactor = 1.0;
               }
               //std::cout << hitslayer << " " << tests << " " << degradationFactor << std::endl;
            }

            if( fabs(eta) < 2.5 && pt > 400.0 ){ // requirement of tracks
               if(m_degFactorMap) m_degFactorMap->Fill( eta, phi, degradationFactor);
               if(m_degFactorMap_per_lumi) m_degFactorMap_per_lumi->Fill(m_manager->lumiBlockNumber(), degradationFactor);
               if(m_degFactorMap_eta_per_lumi) m_degFactorMap_eta_per_lumi->Fill(m_manager->lumiBlockNumber(), eta, degradationFactor);
               //if(m_degFactorMap_phi_per_lumi) m_degFactorMap_phi_per_lumi->Fill(m_manager->lumiBlockNumber(), measPerigee->parameters()[Trk::phi0], degradationFactor);
            }
         }
      }

      if(m_doHoleSearch && !m_doOnline && nholes>0){delete (track);}
   } // end of track loop

   //if(m_tracksPerEvt_per_lumi) m_tracksPerEvt_per_lumi->Fill(m_manager->lumiBlockNumber(), nTracks);
   //if(m_tracksPerEvtPerMu_per_lumi && m_lumiTool->lbAverageInteractionsPerCrossing() > 0 ){
   //   m_tracksPerEvtPerMu_per_lumi->Fill(m_manager->lumiBlockNumber(),nTracks/m_lumiTool->lbAverageInteractionsPerCrossing());
   //}
   if(m_doOnTrack || m_doOnPixelTrack)sort (m_RDOIDs.begin(), m_RDOIDs.end());
   if(m_doOnTrack || m_doOnPixelTrack)sort (m_ClusterIDs.begin(), m_ClusterIDs.end());

   if(m_doOnline){
      if(m_doRefresh5min) {
         if(m_tsos_holeratio_mon && m_tsos_holeratio_tmp) m_tsos_holeratio_mon->Fill2DMon(m_tsos_holeratio_tmp);
         if(m_misshits_ratio_mon && m_misshits_ratio_tmp) m_misshits_ratio_mon->Fill2DMon(m_misshits_ratio_tmp);
      }
   }//end of doOnline loop processing

   return StatusCode::SUCCESS;
}      

StatusCode PixelMainMon::ProcTrackMon(void)
{
  double lengthLB = 0;
  lengthLB = m_LBendTime - m_LBstartTime;
  if (lengthLB <= 0) {
    if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Luminosity block has length <= 0 sec, cannot calculate track rate." << endreq;  
    return StatusCode::SUCCESS; //if LB length is zero, the rest is pointless and would divide by 0
  }
 
  //if(m_trackRate_per_lumi) {
  //  if(m_tracks_per_lumi) double ntrkLB = m_tracks_per_lumi->GetBinContent(m_manager->lumiBlockNumber()+1); //value LB number corresponds to bin nbr (m_lumiBlockNum+1)
  //  double trackRateLB = (double) ntrkLB/lengthLB;
  //  m_trackRate_per_lumi->Fill(m_manager->lumiBlockNumber(),trackRateLB);
  //}
  for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
     if(m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMinimum(0.8);
     if(m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMaximum(1.01);
     if(m_hiteff_actv_mod[i]) m_hiteff_actv_mod[i]->SetMinimum(0.8);
     if(m_hiteff_actv_mod[i]) m_hiteff_actv_mod[i]->SetMaximum(1.01);
  }
  return StatusCode::SUCCESS;
  
}



