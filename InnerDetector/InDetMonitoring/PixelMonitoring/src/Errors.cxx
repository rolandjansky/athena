/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing module errors
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
//#include "InDetRawData/InDetTimeCollection.h"
//#include "TrkSpacePoint/SpacePointContainer.h"
//#include "InDetPrepRawData/PixelClusterContainer.h"         
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
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
#include "TProfile.h"
#include "LWHists/TProfile_LW.h"
#include <sstream>

//#include "InDetRawData/InDetRawDataContainer.h"
//#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
//#include "TrkTrack/TrackCollection.h"            
#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "TrkTrackSummary/TrackSummary.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMon2DLumiMaps.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookRODErrorMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Errors" << endreq;  

   std::string path = "Pixel/Errors";
   std::string path2 = "Pixel/ErrorsExpert";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnPixelTrack");
   MonGroup rodHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup rodExpert( this, path2.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   sc = rodHistos.regHist(m_error1  = TH1F_LW::create("pixel_ROD_errors",  ("Errors" + m_histTitleExt + "; error bit; # error").c_str(), 64, -0.5, 63.5));
   sc = rodHistos.regHist(m_errorTypes  = TH1F_LW::create("pixel_ROD_types",  ("Errors" + m_histTitleExt + "; error type; # errors").c_str(), 7, -0.5, 6.5));
   LWHist::LWHistAxis *xaxis_errorTypes = m_errorTypes->GetXaxis();
   const char * errorType[7] = {"synch (module)", "synch (ROD)", "trunc/high occ. (module)", "trunc/high occ. (ROD)", "optical transmission", "SEU", "timeout"};
   for(int k=0; k<7; k++) xaxis_errorTypes->SetBinLabel(k+1,errorType[k]);

   sc = rodHistos.regHist(m_errors_per_lumi_PIX      = TProfile_LW::create("errors_per_lumi_PIX",    ("Number of errors per event per LB (PIX)" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_ECA  = TProfile_LW::create("errors_per_lumi_ECA",("Number of errors per event per LB (IBL), endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_ECC  = TProfile_LW::create("errors_per_lumi_ECC",("Number of errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_IBL   = TProfile_LW::create("errors_per_lumi_IBL", ("Number of errors per event per LB, IBL" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_B0   = TProfile_LW::create("errors_per_lumi_B0", ("Number of errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_B1   = TProfile_LW::create("errors_per_lumi_B1", ("Number of errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_errors_per_lumi_B2   = TProfile_LW::create("errors_per_lumi_B2", ("Number of errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_PIX      = TProfile_LW::create("SyncErrors_per_lumi_PIX",      ("Synchronization errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_ECA      = TProfile_LW::create("SyncErrors_per_lumi_ECA",      ("Synchronization errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_ECC      = TProfile_LW::create("SyncErrors_per_lumi_ECC",      ("Synchronization errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B0      = TProfile_LW::create("SyncErrors_per_lumi_B0",      ("Synchronization errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B1      = TProfile_LW::create("SyncErrors_per_lumi_B1",      ("Synchronization errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B2      = TProfile_LW::create("SyncErrors_per_lumi_B2",      ("Synchronization errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_PIX   = TProfile_LW::create("OpticalErrors_per_lumi_PIX",   ("Optical errors per event  per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_ECA   = TProfile_LW::create("OpticalErrors_per_lumi_ECA",   ("Optical errors per event  per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_ECC   = TProfile_LW::create("OpticalErrors_per_lumi_ECC",   ("Optical errors per event  per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B0   = TProfile_LW::create("OpticalErrors_per_lumi_B0",   ("Optical errors per event  per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B1   = TProfile_LW::create("OpticalErrors_per_lumi_B1",   ("Optical errors per event  per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B2   = TProfile_LW::create("OpticalErrors_per_lumi_B2",   ("Optical errors per event  per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_PIX      = TProfile_LW::create("SEU_Errors_per_lumi_PIX",      ("SEU errors per event  per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_ECA      = TProfile_LW::create("SEU_Errors_per_lumi_ECA",      ("SEU errors per event  per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_ECC      = TProfile_LW::create("SEU_Errors_per_lumi_ECC",      ("SEU errors per event  per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B0      = TProfile_LW::create("SEU_Errors_per_lumi_B0",      ("SEU errors per event  per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B1      = TProfile_LW::create("SEU_Errors_per_lumi_B1",      ("SEU errors per event  per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B2      = TProfile_LW::create("SEU_Errors_per_lumi_B2",      ("SEU errors per event  per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_PIX= TProfile_LW::create("TruncationErrors_per_lumi_PIX",("Truncation errors per event  per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_ECA= TProfile_LW::create("TruncationErrors_per_lumi_ECA",("Truncation errors per event  per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_ECC= TProfile_LW::create("TruncationErrors_per_lumi_ECC",("Truncation errors per event  per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B0= TProfile_LW::create("TruncationErrors_per_lumi_B0",("Truncation errors per event  per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B1= TProfile_LW::create("TruncationErrors_per_lumi_B1",("Truncation errors per event  per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B2= TProfile_LW::create("TruncationErrors_per_lumi_B2",("Truncation errors per event  per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_PIX   = TProfile_LW::create("TimeoutErrors_per_lumi_PIX",   ("Timeout errors per event  per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_ECA   = TProfile_LW::create("TimeoutErrors_per_lumi_ECA",   ("Timeout errors per event  per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_ECC   = TProfile_LW::create("TimeoutErrors_per_lumi_ECC",   ("Timeout errors per event  per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B0   = TProfile_LW::create("TimeoutErrors_per_lumi_B0",   ("Timeout errors per event  per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B1   = TProfile_LW::create("TimeoutErrors_per_lumi_B1",   ("Timeout errors per event  per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B2   = TProfile_LW::create("TimeoutErrors_per_lumi_B2",   ("Timeout errors per event  per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_PIX = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_PIX",   ("Synchronization Module BCID1 errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_ECA = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_ECA",   ("Synchronization Module BCID1 errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_ECC = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_ECC",   ("Synchronization Module BCID1 errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B0 = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_B0",   ("Synchronization Module BCID1 errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B1 = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_B1",   ("Synchronization Module BCID1 errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B2 = TProfile_LW::create("Mod_Sync_BCID1_errors_per_lumi_B2",   ("Synchronization Module BCID1 errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_PIX = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_PIX",   ("Synchronization Module BCID2 errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_ECA = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_ECA",   ("Synchronization Module BCID2 errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_ECC = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_ECC",   ("Synchronization Module BCID2 errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B0 = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_B0",   ("Synchronization Module BCID2 errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B1 = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_B1",   ("Synchronization Module BCID2 errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B2 = TProfile_LW::create("Mod_Sync_BCID2_errors_per_lumi_B2",   ("Synchronization Module BCID2 errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_PIX = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_PIX",   ("Synchronization Module LVL1ID errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_ECA = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_ECA",   ("Synchronization Module LVL1ID errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_ECC = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_ECC",   ("Synchronization Module LVL1ID errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B0 = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_B0",   ("Synchronization Module LVL1ID errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B1 = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_B1",   ("Synchronization Module LVL1ID errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B2 = TProfile_LW::create("Mod_Sync_LVL1ID_errors_per_lumi_B2",   ("Synchronization Module LVL1ID errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_PIX = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_PIX",   ("Synchronization ROD BCID errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_ECA = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_ECA",   ("Synchronization ROD BCID errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_ECC = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_ECC",   ("Synchronization ROD BCID errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B0 = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_B0",   ("Synchronization ROD BCID errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B1 = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_B1",   ("Synchronization ROD BCID errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B2 = TProfile_LW::create("ROD_Sync_BCID_errors_per_lumi_B2",   ("Synchronization ROD BCID errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_PIX = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_PIX",   ("Synchronization ROD LVL1ID errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_ECA = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_ECA",   ("Synchronization ROD LVL1ID errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_ECC = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_ECC",   ("Synchronization ROD LVL1ID errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B0 = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_B0",   ("Synchronization ROD LVL1ID errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B1 = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_B1",   ("Synchronization ROD LVL1ID errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B2 = TProfile_LW::create("ROD_Sync_LVL1ID_errors_per_lumi_B2",   ("Synchronization ROD LVL1ID errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_PIX = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_PIX",   ("Truncation Module EOC errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_ECA = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_ECA",   ("Truncation Module EOC errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_ECC = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_ECC",   ("Truncation Module EOC errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B0 = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_B0",   ("Truncation Module EOC errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B1 = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_B1",   ("Truncation Module EOC errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B2 = TProfile_LW::create("Mod_Trunc_EOC_errors_per_lumi_B2",   ("Truncation Module EOC errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_PIX = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_PIX",   ("Truncation Module Hit Overflow errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_ECA = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_ECA",   ("Truncation Module Hit Overflow errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_ECC = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_ECC",   ("Truncation Module Hit Overflow errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B0 = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_B0",   ("Truncation Module Hit Overflow errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B1 = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_B1",   ("Truncation Module Hit Overflow errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B2 = TProfile_LW::create("Mod_Trunc_Hit_Overflow_errors_per_lumi_B2",   ("Truncation Module Hit Overflow errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_PIX = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_PIX",   ("Truncation Module EoE Overflow errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_ECA = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_ECA",   ("Truncation Module EoE Overflow errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_ECC = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_ECC",   ("Truncation Module EoE Overflow errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B0 = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_B0",   ("Truncation Module EoE Overflow errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B1 = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_B1",   ("Truncation Module EoE Overflow errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B2 = TProfile_LW::create("Mod_Trunc_EoE_Overflow_errors_per_lumi_B2",   ("Truncation Module EoE Overflow errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_PIX = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_PIX",   ("Truncation ROD H/T Limit errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_ECA = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_ECA",   ("Truncation ROD H/T Limit errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_ECC = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_ECC",   ("Truncation ROD H/T Limit errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B0 = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_B0",   ("Truncation ROD H/T Limit errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B1 = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_B1",   ("Truncation ROD H/T Limit errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B2 = TProfile_LW::create("ROD_Trunc_HT_Limit_errors_per_lumi_B2",   ("Truncation ROD H/T Limit errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_PIX = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_PIX",   ("Truncation ROD Overflow errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_ECA = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_ECA",   ("Truncation ROD Overflow errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_ECC = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_ECC",   ("Truncation ROD Overflow errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B0 = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_B0",   ("Truncation ROD Overflow errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B1 = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_B1",   ("Truncation ROD Overflow errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B2 = TProfile_LW::create("ROD_Trunc_ROD_OF_errors_per_lumi_B2",   ("Truncation ROD Overflow errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_optical_error_per_lumi_PIX = TProfile_LW::create("Optical_Errors_per_lumi_PIX",   ("Optical Errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_optical_error_per_lumi_ECA = TProfile_LW::create("Optical_Errors_per_lumi_ECA",   ("Optical Errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_optical_error_per_lumi_ECC = TProfile_LW::create("Optical_Errors_per_lumi_ECC",   ("Optical Errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B0 = TProfile_LW::create("Optical_Errors_per_lumi_B0",   ("Optical Errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B1 = TProfile_LW::create("Optical_Errors_per_lumi_B1",   ("Optical Errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B2 = TProfile_LW::create("Optical_Errors_per_lumi_B2",   ("Optical Errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_PIX = TProfile_LW::create("SEU_Hit_Parity_per_lumi_PIX",   ("SEU Hit Parity errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_ECA = TProfile_LW::create("SEU_Hit_Parity_per_lumi_ECA",   ("SEU Hit Parity errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_ECC = TProfile_LW::create("SEU_Hit_Parity_per_lumi_ECC",   ("SEU Hit Parity errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B0 = TProfile_LW::create("SEU_Hit_Parity_per_lumi_B0",   ("SEU Hit Parity errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B1 = TProfile_LW::create("SEU_Hit_Parity_per_lumi_B1",   ("SEU Hit Parity errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B2 = TProfile_LW::create("SEU_Hit_Parity_per_lumi_B2",   ("SEU Hit Parity errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_PIX = TProfile_LW::create("SEU_Register_Parity_per_lumi_PIX",   ("SEU Register Parity errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_ECA = TProfile_LW::create("SEU_Register_Parity_per_lumi_ECA",   ("SEU Register Parity errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_ECC = TProfile_LW::create("SEU_Register_Parity_per_lumi_ECC",   ("SEU Register Parity errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B0 = TProfile_LW::create("SEU_Register_Parity_per_lumi_B0",   ("SEU Register Parity errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B1 = TProfile_LW::create("SEU_Register_Parity_per_lumi_B1",   ("SEU Register Parity errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B2 = TProfile_LW::create("SEU_Register_Parity_per_lumi_B2",   ("SEU Register Parity errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_seu_hamming_per_lumi_PIX = TProfile_LW::create("SEU_Hamming_per_lumi_PIX",   ("SEU Hamming Code errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_ECA = TProfile_LW::create("SEU_Hamming_per_lumi_ECA",   ("SEU Hamming Code errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_ECC = TProfile_LW::create("SEU_Hamming_per_lumi_ECC",   ("SEU Hamming Code errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B0 = TProfile_LW::create("SEU_Hamming_per_lumi_B0",   ("SEU Hamming Code errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B1 = TProfile_LW::create("SEU_Hamming_per_lumi_B1",   ("SEU Hamming Code errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B2 = TProfile_LW::create("SEU_Hamming_per_lumi_B2",   ("SEU Hamming Code errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_timeout_per_lumi_PIX = TProfile_LW::create("ROD_Timeout_per_lumi_PIX",   ("ROD Formatter Timeout errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_timeout_per_lumi_ECA = TProfile_LW::create("ROD_Timeout_per_lumi_ECA",   ("ROD Formatter Timeout errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_timeout_per_lumi_ECC = TProfile_LW::create("ROD_Timeout_per_lumi_ECC",   ("ROD Formatter Timeout errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_timeout_per_lumi_B0 = TProfile_LW::create("ROD_Timeout_per_lumi_B0",   ("ROD Formatter Timeout errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_timeout_per_lumi_B1 = TProfile_LW::create("ROD_Timeout_per_lumi_B1",   ("ROD Formatter Timeout errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_timeout_per_lumi_B2 = TProfile_LW::create("ROD_Timeout_per_lumi_B2",   ("ROD Formatter Timeout errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_FEwarning_per_lumi_PIX = TProfile_LW::create("FE_Warning_per_lumi_PIX",   ("FE Warning errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_ECA = TProfile_LW::create("FE_Warning_per_lumi_ECA",   ("FE Warning errors per event per LB, endcap A" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_ECC = TProfile_LW::create("FE_Warning_per_lumi_ECC",   ("FE Warning errors per event per LB, endcap C" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B0 = TProfile_LW::create("FE_Warning_per_lumi_B0",   ("FE Warning errors per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B1 = TProfile_LW::create("FE_Warning_per_lumi_B1",   ("FE Warning errors per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B2 = TProfile_LW::create("FE_Warning_per_lumi_B2",   ("FE Warning errors per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# errors/event").c_str(),2500,-0.5,2499.5));

   sc = rodExpert.regHist(m_bad_mod_errors_PIX = TH1I_LW::create("Bad_Module_Errors_PIX",   ("Errors for Bad Modules, PIX" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_bad_mod_errors_ECA = TH1I_LW::create("Bad_Module_Errors_ECA",   ("Errors for Bad Modules, endcap A" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_bad_mod_errors_ECC = TH1I_LW::create("Bad_Module_Errors_ECC",   ("Errors for Bad Modules, endcap C" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_mod_errors_IBL = TH1I_LW::create("Module_Errors_IBL",   ("Errors for Modules, IBL" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_ROD_errors_IBL = TH1I_LW::create("ROD_Errors_IBL",   ("Errors for RODs, IBL" + m_histTitleExt + ";Error State;# Errors").c_str(),8,-0.5,7.5));
   sc = rodExpert.regHist(m_bad_mod_errors_B0 = TH1I_LW::create("Bad_Module_Errors_B0",   ("Errors for Bad Modules, barrel layer 0" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_bad_mod_errors_B1 = TH1I_LW::create("Bad_Module_Errors_B1",   ("Errors for Bad Modules, barrel layer 1" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_bad_mod_errors_B2 = TH1I_LW::create("Bad_Module_Errors_B2",   ("Errors for Bad Modules, barrel layer 2" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   
   const char * errorBitsPIX[32] = {"ROD Trunc FIFO Overflow", "ROD Trunc H/T Limit","2","3","Module Trunc EoC","SEU Hit Parity","SEU Reg Parity","SEU Hamming Code","FE Warning (Bit Flip)","9","10","11","Module Trunc Hit Overflow","Module Trunc EoE Overflow","Module Sync BCID1","Module Sync BCID2","Module Sync LVL1ID","17","18","19","ROD Sync BCID","ROD Sync LVL1ID","Timeout","Optical","24","25","26","27","28","29","30","31"};

   const char* errorBitsFEI4[32] = {"BCID counter","Hamming code 0","Hamming code 1","Hamming code 2","L1_in counter","L1 request counter","L1 register","L1 Trigger ID","readout processor","Fifo_Full flag pulsed","HitOr bus pulsed","11","12","13","3 MSBs of BC and 7 MSBs of L1AC","Skipped trigger counter","Truncated event flag and counter","17","18","19","20","Reset bar RA2b pulsed","Fast PLL clock phase","Fast reference clock phase","Triple redundant mismatch","Write register data error","Address error","Other CMD decoder","CMD decoder bit flip","CMD decoder SEU","Data bus address","Triple redundant mismatch"};

  const char* errorBitsIBLROD[8] = {"Row/ Column Error", "Limit Error", "Timeout Error", "BCID Error", "LVL1ID Error", "Preamble Error", "Masked Link", "Timeout Error"};  //according to pg 67 of RODBOC v1.2.3

   for (int i=0;i<32;i++){
      m_mod_errors_IBL->GetXaxis()->SetBinLabel(i+1,errorBitsFEI4[i]);
   } 

   for (int i=0;i<8;i++){
      m_ROD_errors_IBL->GetXaxis()->SetBinLabel(i+1,errorBitsIBLROD[i]);
   } 

   for (int i=0;i<32;i++){
      m_bad_mod_errors_PIX->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
      m_bad_mod_errors_ECA->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
      m_bad_mod_errors_ECC->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
      m_bad_mod_errors_B0->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
      m_bad_mod_errors_B1->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
      m_bad_mod_errors_B2->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
   } 

   if(m_doOnline)
   {
      sc = rodHistos.regHist(m_error_time1 = new TProfile("errors_over_time_10min", ("Number of Errors as function of time over 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));    
      sc = rodHistos.regHist(m_error_time2 = new TProfile("errors_over_time_1hr",   ("Number of Errors as function of time over 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(),   99,0.,1.,"i"));    
      sc = rodHistos.regHist(m_error_time3 = new TProfile("errors_over_time_6hr",   ("Number of Errors as function of time over 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(),  99,0.,1.,"i"));
   }

   if (m_doModules)  
   {
      m_errors = new PixelMonModules1D("errors", ("Errors in module:ErrorType" + m_histTitleExt + ";Number of Errors").c_str(), 7,1.,8.,m_doIBL);
      sc = m_errors->regHist(this,(path+"/ModulesErrors").c_str(),run,m_doIBL);
      const char * errorType[7] = {"synch (module)", "synch (ROD)", "trunc/high occ. (module)", "trunc/high occ. (ROD)", "optical transmission", "SEU", "timeout"};
      for(int k=0; k<7; k++) m_errors->SetBinLabel(errorType[k],k+1);
   }
   if(m_do2DMaps && !m_doOnline)
   {

     m_IBLModErrors = new PixelMon2DMapsLW("IBLModErrors", ("IBL Chip Errors" + m_histTitleExt).c_str(), m_doIBL, false);  
      sc = m_IBLModErrors->regHist(rodHistos, m_doIBL, false);
      m_IBLRODErrors = new PixelMon2DMapsLW("IBLRODErrors", ("IBL ROD Errors" + m_histTitleExt).c_str(), m_doIBL, false);  
      sc = m_IBLRODErrors->regHist(rodHistos, m_doIBL, false);
      m_SyncErrors = new PixelMon2DMapsLW("SyncErrors", ("Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SyncErrors->regHist(rodHistos, m_doIBL, true);
      m_TruncationErrors = new PixelMon2DMapsLW("Truncation_Errors", ("Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);  //to be removed ?
      sc = m_TruncationErrors->regHist(rodHistos, m_doIBL, true);
      m_SyncErrors_mod = new PixelMon2DMapsLW("SyncErrors_mod", ("Module Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SyncErrors_mod->regHist(rodHistos, m_doIBL, true);
      m_SyncErrors_ROD = new PixelMon2DMapsLW("SyncErrors_ROD", ("ROD Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SyncErrors_ROD->regHist(rodHistos, m_doIBL, true);
      m_TruncErrors_mod = new PixelMon2DMapsLW("TruncErrors_mod", ("Module Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_TruncErrors_mod->regHist(rodHistos, m_doIBL, true);
      m_TruncErrors_ROD = new PixelMon2DMapsLW("TruncErrors_ROD", ("ROD Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_TruncErrors_ROD->regHist(rodHistos, m_doIBL, true);

      m_OpticalErrors = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_OpticalErrors->regHist(rodHistos, m_doIBL, true);
      m_SEU_Errors = new PixelMon2DMapsLW("SEU_Errors", ("SEU Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SEU_Errors->regHist(rodHistos, m_doIBL, true);
      m_TimeoutErrors = new PixelMon2DMapsLW("Timeout_Errors", ("Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_TimeoutErrors->regHist(rodHistos, m_doIBL, true);

      m_sync_mod_BCID1 = new PixelMon2DMapsLW("SyncErrors_mod_BCID1", ("Module Synchronization Errors, BCID1" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_sync_mod_BCID1->regHist(rodExpert, m_doIBL, true);
      m_sync_mod_BCID2 = new PixelMon2DMapsLW("SyncErrors_mod_BCID2", ("Module Synchronization Errors, BCID2" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_sync_mod_BCID2->regHist(rodExpert, m_doIBL, true);
      m_sync_mod_LVL1ID = new PixelMon2DMapsLW("SyncErrors_mod_LVL1ID", ("Module Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_sync_mod_LVL1ID->regHist(rodExpert, m_doIBL, true);
      m_sync_rod_BCID = new PixelMon2DMapsLW("SyncErrors_ROD_BCID", ("ROD Synchronization Errors, BCID" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_sync_rod_BCID->regHist(rodExpert, m_doIBL, true);
      m_sync_rod_LVL1ID = new PixelMon2DMapsLW("SyncErrors_ROD_LVL1ID", ("ROD Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_sync_rod_LVL1ID->regHist(rodExpert, m_doIBL, true);
      m_trunc_mod_EOC = new PixelMon2DMapsLW("TruncErrors_mod_EoC", ("Module Truncation Errors, EoC" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_trunc_mod_EOC->regHist(rodExpert, m_doIBL, true);
      m_trunc_mod_hitOF = new PixelMon2DMapsLW("TruncErrors_mod_hitOF", ("Module Truncation Errors, Hit Overflow" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_trunc_mod_hitOF->regHist(rodExpert, m_doIBL, true);
      m_trunc_mod_EoEOF = new PixelMon2DMapsLW("TruncErrors_mod_EoEOF", ("Module Truncation Errors, EoE Overflow" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_trunc_mod_EoEOF->regHist(rodExpert, m_doIBL, true);
      m_trunc_rod_HTlim = new PixelMon2DMapsLW("TruncErrors_ROD_HTlim", ("ROD Truncation Errors, H/T Limit" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_trunc_rod_HTlim->regHist(rodExpert, m_doIBL, true);
      m_trunc_rod_FIFOOF = new PixelMon2DMapsLW("TruncErrors_ROD_FIFOOF", ("ROD Truncation Errors, FIFO Overflow" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_trunc_rod_FIFOOF->regHist(rodExpert, m_doIBL, true);
      m_optical_error = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_optical_error->regHist(rodExpert, m_doIBL, true);
      m_seu_hit_parity = new PixelMon2DMapsLW("SEUErrors_Hit_Parity", ("SEU Errors, Hit Parity" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_seu_hit_parity->regHist(rodExpert, m_doIBL, true);
      m_seu_reg_parity = new PixelMon2DMapsLW("SEUErrors_Reg_Parity", ("SEU Errors, Register Parity" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_seu_reg_parity->regHist(rodExpert, m_doIBL, true);
      m_seu_hamming = new PixelMon2DMapsLW("SEUErrors_Hamming", ("SEU Errors, Hamming Code" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_seu_hamming->regHist(rodExpert, m_doIBL, true);
      m_timeout = new PixelMon2DMapsLW("ROD_Timeout", ("ROD Formatter Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_timeout->regHist(rodExpert, m_doIBL, true);
      m_FEwarning = new PixelMon2DMapsLW("FE_Warning", ("FE Warning Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_FEwarning->regHist(rodExpert, m_doIBL, true);

      if(m_doModules && !m_doOnline){
	m_sync_mod_BCID1_per_LB = new PixelMon2DLumiProfiles("SyncErrors_mod_BCID1_per_LB", ("Avg Module Synchronization Errors, BCID1" + m_histTitleExt).c_str(),"# Mod BCID1 Errors/Event",m_doIBL,true);
	sc = m_sync_mod_BCID1_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_mod_BCID2_per_LB = new PixelMon2DLumiProfiles("SyncErrors_mod_BCID2_per_LB", ("Avg Module Synchronization Errors, BCID2" + m_histTitleExt).c_str(),"# Mod BCID2 Errors/Event",m_doIBL,true);
	sc = m_sync_mod_BCID2_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_mod_LVL1ID_per_LB = new PixelMon2DLumiProfiles("SyncErrors_mod_LVL1ID_per_LB", ("Avg Module Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(),"# Mod LVL1ID Errors/Event",m_doIBL,true);
	sc = m_sync_mod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_mod_corr_LVL1ID_BCID2_per_LB = new PixelMon2DLumiProfiles("SyncErrors_mod_corr_LVL1ID_BCID2_per_LB", ("Avg Module Synchronization Error Correlations, LVL1ID and BCID2" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID2 Errors/Events with either LVL1ID or BCID2 errors",m_doIBL,true);
	sc = m_sync_mod_corr_LVL1ID_BCID2_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_rod_BCID_per_LB = new PixelMon2DLumiProfiles("SyncErrors_rod_BCID_per_LB", ("Avg ROD Synchronization Errors, BCID" + m_histTitleExt).c_str(),"# ROD BCID Errors/Event",m_doIBL,true);
	sc = m_sync_rod_BCID_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_rod_LVL1ID_per_LB = new PixelMon2DLumiProfiles("SyncErrors_rod_LVL1ID_per_LB", ("Avg ROD Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(),"# ROD LVL1ID Errors/Event",m_doIBL,true);
	sc = m_sync_rod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,true);
	m_sync_rod_corr_LVL1ID_BCID_per_LB = new PixelMon2DLumiProfiles("SyncErrors_rod_corr_LVL1ID_BCID_per_LB", ("Avg ROD Synchronization Error Correlations, LVL1ID and BCID" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID Errors/Events with either LVL1ID or BCID errors",m_doIBL,true);
	sc = m_sync_rod_corr_LVL1ID_BCID_per_LB->regHist(rodExpert,m_doIBL,true);
	m_trunc_mod_EOC_per_LB = new PixelMon2DLumiProfiles("TruncErrors_mod_EOC_per_LB", ("Avg Module Truncation Errors, EoC" + m_histTitleExt).c_str(),"# Mod EoC Errors/Event",m_doIBL,true);
	sc = m_trunc_mod_EOC_per_LB->regHist(rodExpert,m_doIBL,true);
	m_trunc_mod_hitOF_per_LB = new PixelMon2DLumiProfiles("TruncErrors_mod_hitOF_per_LB", ("Avg Module Truncation Errors, Hit Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,true);
	sc = m_trunc_mod_hitOF_per_LB->regHist(rodExpert,m_doIBL,true);
	m_trunc_mod_EoEOF_per_LB = new PixelMon2DLumiProfiles("TruncErrors_mod_EoEOF_per_LB", ("Avg Module Truncation Errors, EoE Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,true);
	sc = m_trunc_mod_EoEOF_per_LB->regHist(rodExpert,m_doIBL,true);
	m_trunc_rod_HTlim_per_LB = new PixelMon2DLumiProfiles("TruncErrors_rod_HTlim_per_LB", ("Avg ROD Truncation Errors, H/T Limit" + m_histTitleExt).c_str(),"# H/T Errors/Event",m_doIBL,true);
	sc = m_trunc_rod_HTlim_per_LB->regHist(rodExpert,m_doIBL,true);
	m_trunc_rod_FIFOOF_per_LB = new PixelMon2DLumiProfiles("TruncErrors_rod_FIFOOF_per_LB", ("Avg ROD Truncation Errors, FIFO Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,true);
	sc = m_trunc_rod_FIFOOF_per_LB->regHist(rodExpert,m_doIBL,true);
	m_optical_error_per_LB = new PixelMon2DLumiProfiles("Optical_Errors_per_LB", ("Avg Optical Errors" + m_histTitleExt).c_str(),"# Optical Errors/Event",m_doIBL,true);
	sc = m_optical_error_per_LB->regHist(rodExpert,m_doIBL,true);
	m_seu_hit_parity_per_LB = new PixelMon2DLumiProfiles("SEUErrors_Hit_Parity_per_LB", ("Avg SEU Errors, Hit Parity" + m_histTitleExt).c_str(),"# Hit Parity Errors/Event",m_doIBL,true);
	sc = m_seu_hit_parity_per_LB->regHist(rodExpert,m_doIBL,true);
	m_seu_reg_parity_per_LB = new PixelMon2DLumiProfiles("SEUErrors_Reg_Parity_per_LB", ("Avg SEU Errors, Register Parity" + m_histTitleExt).c_str(),"# Reg Parity Errors/Event",m_doIBL,true);
	sc = m_seu_reg_parity_per_LB->regHist(rodExpert,m_doIBL,true);
	m_seu_hamming_per_LB = new PixelMon2DLumiProfiles("SEUErrors_Hamming_per_LB", ("Avg SEU Errors, Hamming Code" + m_histTitleExt).c_str(),"# HC Errors/Event",m_doIBL,true);
	sc = m_seu_hamming_per_LB->regHist(rodExpert,m_doIBL,true);
	m_timeout_per_LB = new PixelMon2DLumiProfiles("ROD_Timeout_per_LB", ("Avg ROD Formatter Timeout Errors" + m_histTitleExt).c_str(),"# Timeout Errors/Event",m_doIBL,true);
	sc = m_timeout_per_LB->regHist(rodExpert,m_doIBL,true);
	m_FEwarning_per_LB = new PixelMon2DLumiProfiles("FE_Warning_per_LB", ("Avg FE Warning Errors" + m_histTitleExt).c_str(),"# FE Warning Errors/Event",m_doIBL,true);
	sc = m_FEwarning_per_LB->regHist(rodExpert,m_doIBL,true);
      }

      m_sync_mod_BCID1_int_LB = new PixelMon2DLumiMaps("SyncErrors_mod_BCID1_int_LB", ("All Module Synchronization Errors, BCID1" + m_histTitleExt).c_str(),"# Mod BCID1 Errors",m_doIBL,true);
      sc = m_sync_mod_BCID1_int_LB->regHist(rodExpert,m_doIBL,true);
      m_sync_mod_BCID2_int_LB = new PixelMon2DLumiMaps("SyncErrors_mod_BCID2_int_LB", ("All Module Synchronization Errors, BCID2" + m_histTitleExt).c_str(),"# Mod BCID2 Errors",m_doIBL,true);
      sc = m_sync_mod_BCID2_int_LB->regHist(rodExpert,m_doIBL,true);
      m_sync_mod_LVL1ID_int_LB = new PixelMon2DLumiMaps("SyncErrors_mod_LVL1ID_int_LB", ("All Module Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(),"# Mod LVL1ID Errors",m_doIBL,true);
      sc = m_sync_mod_LVL1ID_int_LB->regHist(rodExpert,m_doIBL,true);
      m_sync_rod_BCID_int_LB = new PixelMon2DLumiMaps("SyncErrors_rod_BCID_int_LB", ("All ROD Synchronization Errors, BCID" + m_histTitleExt).c_str(),"# ROD BCID Errors",m_doIBL,true);
      sc = m_sync_rod_BCID_int_LB->regHist(rodExpert,m_doIBL,true);
      m_sync_rod_LVL1ID_int_LB = new PixelMon2DLumiMaps("SyncErrors_rod_LVL1ID_int_LB", ("All ROD Synchronization Errors, LVL1ID" + m_histTitleExt).c_str(),"# ROD LVL1ID Errors",m_doIBL,true);
      sc = m_sync_rod_LVL1ID_int_LB->regHist(rodExpert,m_doIBL,true);
      m_trunc_mod_EOC_int_LB = new PixelMon2DLumiMaps("TruncErrors_mod_EOC_int_LB", ("All Module Truncation Errors, EoC" + m_histTitleExt).c_str(),"# Mod EoC Errors",m_doIBL,true);
      sc = m_trunc_mod_EOC_int_LB->regHist(rodExpert,m_doIBL,true);
      m_trunc_mod_hitOF_int_LB = new PixelMon2DLumiMaps("TruncErrors_mod_hitOF_int_LB", ("All Module Truncation Errors, Hit Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,true);
      sc = m_trunc_mod_hitOF_int_LB->regHist(rodExpert,m_doIBL,true);
      m_trunc_mod_EoEOF_int_LB = new PixelMon2DLumiMaps("TruncErrors_mod_EoEOF_int_LB", ("All Module Truncation Errors, EoE Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,true);
      sc = m_trunc_mod_EoEOF_int_LB->regHist(rodExpert,m_doIBL,true);
      m_trunc_rod_HTlim_int_LB = new PixelMon2DLumiMaps("TruncErrors_rod_HTlim_int_LB", ("All ROD Truncation Errors, H/T Limit" + m_histTitleExt).c_str(),"# H/T Errors",m_doIBL,true);
      sc = m_trunc_rod_HTlim_int_LB->regHist(rodExpert,m_doIBL,true);
      m_trunc_rod_FIFOOF_int_LB = new PixelMon2DLumiMaps("TruncErrors_rod_FIFOOF_int_LB", ("All ROD Truncation Errors, FIFO Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,true);
      sc = m_trunc_rod_FIFOOF_int_LB->regHist(rodExpert,m_doIBL,true);
      m_optical_error_int_LB = new PixelMon2DLumiMaps("Optical_Errors_int_LB", ("All Optical Errors" + m_histTitleExt).c_str(),"# Optical Errors",m_doIBL,true);
      sc = m_optical_error_int_LB->regHist(rodExpert,m_doIBL,true);
      m_seu_hit_parity_int_LB = new PixelMon2DLumiMaps("SEUErrors_Hit_Parity_int_LB", ("All SEU Errors, Hit Parity" + m_histTitleExt).c_str(),"# Hit Parity Errors",m_doIBL,true);
      sc = m_seu_hit_parity_int_LB->regHist(rodExpert,m_doIBL,true);
      m_seu_reg_parity_int_LB = new PixelMon2DLumiMaps("SEUErrors_Reg_Parity_int_LB", ("All SEU Errors, Register Parity" + m_histTitleExt).c_str(),"# Reg Parity Errors",m_doIBL,true);
      sc = m_seu_reg_parity_int_LB->regHist(rodExpert,m_doIBL,true);
      m_seu_hamming_int_LB = new PixelMon2DLumiMaps("SEUErrors_Hamming_int_LB", ("All SEU Errors, Hamming Code" + m_histTitleExt).c_str(),"# HC Errors",m_doIBL,true);
      sc = m_seu_hamming_int_LB->regHist(rodExpert,m_doIBL,true);
      m_timeout_int_LB = new PixelMon2DLumiMaps("ROD_Timeout_int_LB", ("All ROD Formatter Timeout Errors" + m_histTitleExt).c_str(),"# Timeout Errors",m_doIBL,true);
      sc = m_timeout_int_LB->regHist(rodExpert,m_doIBL,true); 
      m_FEwarning_int_LB = new PixelMon2DLumiMaps("FE_Warning_int_LB", ("All FE Warning Errors" + m_histTitleExt).c_str(),"# FE Warning Errors",m_doIBL,true);
      sc = m_FEwarning_int_LB->regHist(rodExpert,m_doIBL,true); 
   }

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::BookRODErrorLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Errors for lowStat" << endreq;  
   
   std::string path = "Pixel/LumiBlock";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(   this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms
   
   if (m_do2DMaps && !m_doOnline) {
     m_errors_LB = new PixelMon2DMapsLW("Errors_LB", ("Errors" + m_histTitleExt).c_str(), m_doIBL, true);
     sc = m_errors_LB->regHist(lumiBlockHist, m_doIBL, true);
   }
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillRODErrorMon(void)
{
   PixelID::const_id_iterator idIt       = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd    = m_pixelid->wafer_end();
   unsigned int errors=0;
   int nErrors_IBL=0;
   int nErrors_PIX=0;
   int nErrors_ECA=0;
   int nErrors_ECC=0;
   int nErrors_B0=0;
   int nErrors_B1=0;
   int nErrors_B2=0;

   int nErrors_cat_PIX[16];
   int nErrors_cat_ECA[16];
   int nErrors_cat_ECC[16];
   int nErrors_cat_B0[16];
   int nErrors_cat_B1[16];
   int nErrors_cat_B2[16];

   for (int i=0;i<16;i++){
      nErrors_cat_PIX[i]=0;
      nErrors_cat_ECA[i]=0;
      nErrors_cat_ECC[i]=0;
      nErrors_cat_B0[i]=0;
      nErrors_cat_B1[i]=0;
      nErrors_cat_B2[i]=0;
   }

   int nSyncErrors_PIX=0;
   int nSyncErrors_ECA=0;
   int nSyncErrors_ECC=0;
   int nSyncErrors_B0=0;
   int nSyncErrors_B1=0;
   int nSyncErrors_B2=0;
   
   int nOpticalErrors_PIX=0;
   int nOpticalErrors_ECA=0;
   int nOpticalErrors_ECC=0;
   int nOpticalErrors_B0=0;
   int nOpticalErrors_B1=0;
   int nOpticalErrors_B2=0;
   
   int nSEUErrors_PIX=0;
   int nSEUErrors_ECA=0;
   int nSEUErrors_ECC=0;
   int nSEUErrors_B0=0;
   int nSEUErrors_B1=0;
   int nSEUErrors_B2=0;
   
   int nTruncErrors_PIX=0;
   int nTruncErrors_ECA=0;
   int nTruncErrors_ECC=0;
   int nTruncErrors_B0=0;
   int nTruncErrors_B1=0;
   int nTruncErrors_B2=0;
   
   int nTimeoutErrors_PIX=0;
   int nTimeoutErrors_ECA=0;
   int nTimeoutErrors_ECC=0;
   int nTimeoutErrors_B0=0;
   int nTimeoutErrors_B1=0;
   int nTimeoutErrors_B2=0;


   for (; idIt != idItEnd; ++idIt) 
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID); 
      errors = m_ErrorSvc->getModuleErrors(id_hash);

      bool isIBL=false;
      bool isError[16];
      if (m_ErrorSvc->isActive(id_hash) && m_pixelid->barrel_ec(WaferID)==0 && m_pixelid->layer_disk(WaferID)==0 && m_doIBL){isIBL=true;}

      for (int i=0;i<16;i++) isError[i]=false;
      for (unsigned int kBit=0; kBit<40; kBit++)
      {
         if( (errors & (1<<kBit)) !=0 )
         {
	   if(isIBL){
	     if(kBit<8){
	       m_ROD_errors_IBL->Fill(kBit);
	     }
	     else{
	       m_mod_errors_IBL->Fill(kBit-8);
	     }
	   }
	   
  	   if (m_ErrorSvc->isActive(id_hash)&&!m_ErrorSvc->isGood(id_hash)){
	      if(!isIBL)m_bad_mod_errors_PIX->Fill(kBit);
	      if(m_pixelid->barrel_ec(WaferID)==2)  m_bad_mod_errors_ECA->Fill(kBit);
	      if(m_pixelid->barrel_ec(WaferID)==-2) m_bad_mod_errors_ECC->Fill(kBit);
	      if (m_pixelid->barrel_ec(WaferID)==0) {
		if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_bad_mod_errors_B0->Fill(kBit);
		if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_bad_mod_errors_B1->Fill(kBit);
		if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_bad_mod_errors_B2->Fill(kBit);
	      }               
            }
            //if(msgLvl(MSG::ERROR)) msg(MSG::ERROR)  << "NT: ERROR IS " << errors << endreq;
	   if(!isIBL){nErrors_PIX++;} //count the errors
	   else{nErrors_IBL++;} //count the errors
	    //count separetly the errors in different regions
	    if(m_pixelid->barrel_ec(WaferID)==2)  nErrors_ECA++;
	    if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_ECC++;
	    if (m_pixelid->barrel_ec(WaferID)==0) {
	      if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_B0++;
	      if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_B1++;
	      if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_B2++;
	    }

            if(m_error1)m_error1->Fill(kBit);
            int ErrorType =0;
	    /**
            if(kBit==20||kBit==21||kBit==14||kBit==16)ErrorType=1;
            if(kBit==23)ErrorType=2;
            if(kBit>=5&&kBit<=7)ErrorType=3;
            if(kBit==4||kBit==12||kBit==13||kBit==1||kBit==0)ErrorType=4;
            if(kBit==22)ErrorType=5;
	    */
	    if(!isIBL){
	      if(kBit==14||kBit==15||kBit==16)ErrorType=1; // module synchronization errors   (14: BCID, 15: BCID. 16: LVL1ID)
	      if(kBit==20||kBit==21)ErrorType=2;           // ROD synchronization errors      (20: BCID, 21: LVL1ID)
	      if(kBit==4||kBit==12||kBit==13)ErrorType=3;  // module truncation errors        (4: EOC, 12: hit overflow, 13: EoE overflow)
	      if(kBit==0||kBit==1)ErrorType=4;             // ROD truncation errors           (0: FIFO Overflow, 1: H/T Limit)
	      if(kBit==23)ErrorType=5;                     // optical errors                  (preamble (bitflip))
	      if(kBit>=5&&kBit<=7)ErrorType=6;             // SEU (single event upset) errors (hit parity, register parity, hammingcode)
	      if(kBit==22)ErrorType=7;                     // timeout errors                  (timeout on ROD formatter)
	    }
	    else{
	      if(kBit<8){ErrorType = 8;}//any IBL ROD errors
	      else{ErrorType = 9;}//any IBL chip errors
	    }

            if(ErrorType)//if there were any errors we care about
            {
               if(m_errorTypes)m_errorTypes->Fill(ErrorType);
               if(m_errors)m_errors->Fill(ErrorType, WaferID, m_pixelid, m_doIBL);

               //fill all the 2d maps for each error type
               if((ErrorType==1||ErrorType==2) && m_SyncErrors) m_SyncErrors      ->Fill(WaferID,m_pixelid,m_doIBL, true);  //to be removed ?
               if((ErrorType==3||ErrorType==4) && m_TruncationErrors) m_TruncationErrors->Fill(WaferID,m_pixelid,m_doIBL, true);  //to be removed ?
               if(ErrorType==1 && m_SyncErrors_mod ) m_SyncErrors_mod ->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==2 && m_SyncErrors_ROD ) m_SyncErrors_ROD ->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==3 && m_TruncErrors_mod) m_TruncErrors_mod->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==4 && m_TruncErrors_ROD) m_TruncErrors_ROD->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==5 && m_OpticalErrors  ) m_OpticalErrors  ->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==6 && m_SEU_Errors     ) m_SEU_Errors     ->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(ErrorType==7 && m_TimeoutErrors  ) m_TimeoutErrors  ->Fill(WaferID,m_pixelid,m_doIBL, true);
	       if(ErrorType==8 && m_IBLModErrors  ) m_IBLModErrors  ->Fill(WaferID,m_pixelid,m_doIBL,false);
	       if(ErrorType==9 && m_IBLRODErrors  ) m_IBLRODErrors  ->Fill(WaferID,m_pixelid,m_doIBL,false);
	       
	       if(m_doLumiBlock){
		 if(m_errors_LB)m_errors_LB->Fill(WaferID,m_pixelid,m_doIBL, true);
	       }

	       if (ErrorType==1||ErrorType==2){
	         if(!isIBL)nSyncErrors_PIX++;
		 if(m_pixelid->barrel_ec(WaferID)==2) nSyncErrors_ECA++;
		 if(m_pixelid->barrel_ec(WaferID)==-2) nSyncErrors_ECC++;
		 if(m_pixelid->barrel_ec(WaferID)==0) {
		   if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nSyncErrors_B0++;
		   if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nSyncErrors_B1++;
		   if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nSyncErrors_B2++;
		 }
	       }
	       if (ErrorType==3||ErrorType==4){
		 if(!isIBL)nTruncErrors_PIX++;
		 if(m_pixelid->barrel_ec(WaferID)==2) nTruncErrors_ECA++;
		 if(m_pixelid->barrel_ec(WaferID)==-2) nTruncErrors_ECC++;
		 if(m_pixelid->barrel_ec(WaferID)==0) {
		   if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nTruncErrors_B0++;
		   if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nTruncErrors_B1++;
		   if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nTruncErrors_B2++;
		 }
	       }
	       if (ErrorType==5){
	         if(!isIBL)nOpticalErrors_PIX++;
		 if(m_pixelid->barrel_ec(WaferID)==2) nOpticalErrors_ECA++;
		 if(m_pixelid->barrel_ec(WaferID)==-2) nOpticalErrors_ECC++;
		 if(m_pixelid->barrel_ec(WaferID)==0) {
		   if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nOpticalErrors_B0++;
		   if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nOpticalErrors_B1++;
		   if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nOpticalErrors_B2++;
		 }
	       }
	       if (ErrorType==6){
		 if(!isIBL)nSEUErrors_PIX++;
		 if(m_pixelid->barrel_ec(WaferID)==2) nSEUErrors_ECA++;
		 if(m_pixelid->barrel_ec(WaferID)==-2) nSEUErrors_ECC++;
		 if(m_pixelid->barrel_ec(WaferID)==0) {
		   if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nSEUErrors_B0++;
		   if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nSEUErrors_B1++;
		   if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nSEUErrors_B2++;
		 }
	       }
	       if (ErrorType==7){
	         if(!isIBL)nTimeoutErrors_PIX++;
		 if(m_pixelid->barrel_ec(WaferID)==2) nTimeoutErrors_ECA++;
		 if(m_pixelid->barrel_ec(WaferID)==-2) nTimeoutErrors_ECC++;
		 if(m_pixelid->barrel_ec(WaferID)==0) {
		   if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nTimeoutErrors_B0++;
		   if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nTimeoutErrors_B1++;
		   if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nTimeoutErrors_B2++;
		 }
	       }

            }//end if ErrorType
            
            if(kBit==14){
	      if(m_sync_mod_BCID1) m_sync_mod_BCID1->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(m_sync_mod_BCID1_int_LB) m_sync_mod_BCID1_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[0]=true;
               if(!isIBL)nErrors_cat_PIX[0]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[0]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[0]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[0]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[0]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[0]++;
               }
            }
            if(kBit==15){
	      if(m_sync_mod_BCID2) m_sync_mod_BCID2->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_mod_BCID2_int_LB) m_sync_mod_BCID2_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[1]=true;
               if(!isIBL)nErrors_cat_PIX[1]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[1]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[1]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[1]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[1]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[1]++;
               }
            }
            if(kBit==16){
	      if(m_sync_mod_LVL1ID) m_sync_mod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_mod_LVL1ID_int_LB) m_sync_mod_LVL1ID_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[2]=true;
               if(!isIBL)nErrors_cat_PIX[2]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[2]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[2]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[2]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[2]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[2]++;
               }
            }
            if(kBit==20){
	      if(m_sync_rod_BCID) m_sync_rod_BCID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_rod_BCID_int_LB) m_sync_rod_BCID_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[3]=true;
               if(!isIBL)nErrors_cat_PIX[3]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[3]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[3]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[3]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[3]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[3]++;
               }
            }
            if(kBit==21){
	      if(m_sync_rod_LVL1ID) m_sync_rod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_rod_LVL1ID_int_LB) m_sync_rod_LVL1ID_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[4]=true;
               if(!isIBL)nErrors_cat_PIX[4]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[4]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[4]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[4]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[4]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[4]++;
               }
            }
            if(kBit==4){
	      if(m_trunc_mod_EOC) m_trunc_mod_EOC->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_EOC_int_LB) m_trunc_mod_EOC_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[5]=true;
               if(!isIBL)nErrors_cat_PIX[5]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[5]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[5]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[5]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[5]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[5]++;
               }
            }
            if(kBit==12){
	      if(m_trunc_mod_hitOF) m_trunc_mod_hitOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_hitOF_int_LB) m_trunc_mod_hitOF_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[6]=true;
               if(!isIBL)nErrors_cat_PIX[6]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[6]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[6]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[6]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[6]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[6]++;
               }
            }
            if(kBit==13){
	      if(m_trunc_mod_EoEOF) m_trunc_mod_EoEOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_EoEOF_int_LB) m_trunc_mod_EoEOF_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[7]=true;
               if(!isIBL)nErrors_cat_PIX[7]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[7]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[7]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[7]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[7]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[7]++;
               }
            }
            if(kBit==1){
	      if(m_trunc_rod_HTlim) m_trunc_rod_HTlim->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_rod_HTlim_int_LB) m_trunc_rod_HTlim_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[8]=true;
               if(!isIBL)nErrors_cat_PIX[8]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[8]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[8]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[8]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[8]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[8]++;
               }
            }
            if(kBit==0){
	      if(m_trunc_rod_FIFOOF) m_trunc_rod_FIFOOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_rod_FIFOOF_int_LB) m_trunc_rod_FIFOOF_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[9]=true;
               if(!isIBL)nErrors_cat_PIX[9]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[9]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[9]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[9]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[9]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[9]++;
               }
            }
            if(kBit==23){
	      if(m_optical_error) m_optical_error->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[10]=true;
               if(!isIBL)nErrors_cat_PIX[10]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[10]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[10]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[10]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[10]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[10]++;
               }
            }
            if(kBit==5){
	      if(m_seu_hit_parity) m_seu_hit_parity->Fill(WaferID,m_pixelid,m_doIBL,true);
	      if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,m_doIBL,true);
               if(!isIBL)isError[11]=true;
               if(!isIBL)nErrors_cat_PIX[11]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[11]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[11]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[11]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[11]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[11]++;
               }
            }
            if(kBit==6){
	      if(m_seu_reg_parity) m_seu_reg_parity->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_seu_reg_parity_int_LB) m_seu_reg_parity_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[12]=true;
               if(!isIBL)nErrors_cat_PIX[12]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[12]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[12]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[12]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[12]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[12]++;
               }
            }
            if(kBit==7){
	      if(m_seu_hamming) m_seu_hamming->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_seu_hamming_int_LB) m_seu_hamming_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[13]=true;
               if(!isIBL)nErrors_cat_PIX[13]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[13]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[13]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[13]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[13]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[13]++;
               }
            }
            if(kBit==22){
	      if(m_timeout) m_timeout->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_timeout_int_LB) m_timeout_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[14]=true;
               if(!isIBL)nErrors_cat_PIX[14]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[14]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[14]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[14]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[14]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[14]++;
               }
            }            
            if(kBit==8){
	      if(m_FEwarning) m_FEwarning->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_FEwarning_int_LB) m_FEwarning_int_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,1,m_doIBL,true);
               if(!isIBL)isError[15]=true;
               if(!isIBL)nErrors_cat_PIX[15]++;
               if(m_pixelid->barrel_ec(WaferID)==2) nErrors_cat_ECA[15]++;
               if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_cat_ECC[15]++;
               if(m_pixelid->barrel_ec(WaferID)==0) {
                  if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_cat_B0[15]++;
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_cat_B1[15]++;
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_cat_B2[15]++;
               }
            }            
         }//end bit shifting
      }//end for loop over bits
      if(m_sync_mod_BCID1_per_LB){
         if (isError[0]) m_sync_mod_BCID1_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_sync_mod_BCID1_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_BCID2_per_LB){
         if (isError[1]) m_sync_mod_BCID2_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_sync_mod_BCID2_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_LVL1ID_per_LB){
         if (isError[2]) m_sync_mod_LVL1ID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_sync_mod_LVL1ID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_corr_LVL1ID_BCID2_per_LB){
         if (isError[2]&&isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else if (isError[2]||isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_BCID_per_LB){
         if (isError[3]) m_sync_rod_BCID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_sync_rod_BCID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_LVL1ID_per_LB){
         if (isError[4]) m_sync_rod_LVL1ID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_sync_rod_LVL1ID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_corr_LVL1ID_BCID_per_LB){
         if (isError[3]&&isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else if (isError[3]||isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_EOC_per_LB){
         if (isError[5]) m_trunc_mod_EOC_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_trunc_mod_EOC_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_hitOF_per_LB){
         if (isError[6]) m_trunc_mod_hitOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_trunc_mod_hitOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_EoEOF_per_LB){
         if (isError[7]) m_trunc_mod_EoEOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_trunc_mod_EoEOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_rod_HTlim_per_LB){
         if (isError[8]) m_trunc_rod_HTlim_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_trunc_rod_HTlim_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_rod_FIFOOF_per_LB){
         if (isError[9]) m_trunc_rod_FIFOOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_trunc_rod_FIFOOF_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_optical_error_per_LB){
         if (isError[10]) m_optical_error_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_optical_error_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_hit_parity_per_LB){
         if (isError[11]) m_seu_hit_parity_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_seu_hit_parity_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_reg_parity_per_LB){
         if (isError[12]) m_seu_reg_parity_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_seu_reg_parity_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_hamming_per_LB){
         if (isError[13]) m_seu_hamming_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_seu_hamming_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_timeout_per_LB){
         if (isError[14]) m_timeout_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_timeout_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
      if(m_FEwarning_per_LB){
         if (isError[15]) m_FEwarning_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid);
         else m_FEwarning_per_LB->Fill(m_lumiBlockNum,WaferID,m_pixelid,0);           
      }   
   }//end loop over all identifiers
  
   if(m_error_time1&&m_error_time2&&m_error_time3)FillTimeHisto(double(nErrors_PIX),  m_error_time1, m_error_time2, m_error_time3,10.,60.,360.); //occupancy vs time

   //number of errors as function of lumi block
   if(m_errors_per_lumi_IBL)    m_errors_per_lumi_IBL    ->Fill(m_lumiBlockNum,nErrors_IBL);
   if(m_errors_per_lumi_PIX)    m_errors_per_lumi_PIX    ->Fill(m_lumiBlockNum,nErrors_PIX);
   if(m_errors_per_lumi_ECA)m_errors_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_ECA);
   if(m_errors_per_lumi_ECC)m_errors_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_ECC);
   if(m_errors_per_lumi_B0) m_errors_per_lumi_B0 ->Fill(m_lumiBlockNum,nErrors_B0);
   if(m_errors_per_lumi_B1) m_errors_per_lumi_B1 ->Fill(m_lumiBlockNum,nErrors_B1);
   if(m_errors_per_lumi_B2) m_errors_per_lumi_B2 ->Fill(m_lumiBlockNum,nErrors_B2);

   if(m_sync_mod_BCID1_per_lumi_PIX) m_sync_mod_BCID1_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[0]);
   if(m_sync_mod_BCID1_per_lumi_ECA) m_sync_mod_BCID1_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[0]);
   if(m_sync_mod_BCID1_per_lumi_ECC) m_sync_mod_BCID1_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[0]);
   if(m_sync_mod_BCID1_per_lumi_B0) m_sync_mod_BCID1_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[0]);
   if(m_sync_mod_BCID1_per_lumi_B1) m_sync_mod_BCID1_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[0]);
   if(m_sync_mod_BCID1_per_lumi_B2) m_sync_mod_BCID1_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[0]);

   if(m_sync_mod_BCID2_per_lumi_PIX) m_sync_mod_BCID2_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[1]);
   if(m_sync_mod_BCID2_per_lumi_ECA) m_sync_mod_BCID2_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[1]);
   if(m_sync_mod_BCID2_per_lumi_ECC) m_sync_mod_BCID2_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[1]);
   if(m_sync_mod_BCID2_per_lumi_B0) m_sync_mod_BCID2_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[1]);
   if(m_sync_mod_BCID2_per_lumi_B1) m_sync_mod_BCID2_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[1]);
   if(m_sync_mod_BCID2_per_lumi_B2) m_sync_mod_BCID2_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[1]);

   if(m_sync_mod_LVL1ID_per_lumi_PIX) m_sync_mod_LVL1ID_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[2]);
   if(m_sync_mod_LVL1ID_per_lumi_ECA) m_sync_mod_LVL1ID_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[2]);
   if(m_sync_mod_LVL1ID_per_lumi_ECC) m_sync_mod_LVL1ID_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B0) m_sync_mod_LVL1ID_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B1) m_sync_mod_LVL1ID_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B2) m_sync_mod_LVL1ID_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[2]);

   if(m_sync_rod_BCID_per_lumi_PIX) m_sync_rod_BCID_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[3]);
   if(m_sync_rod_BCID_per_lumi_ECA) m_sync_rod_BCID_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[3]);
   if(m_sync_rod_BCID_per_lumi_ECC) m_sync_rod_BCID_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[3]);
   if(m_sync_rod_BCID_per_lumi_B0) m_sync_rod_BCID_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[3]);
   if(m_sync_rod_BCID_per_lumi_B1) m_sync_rod_BCID_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[3]);
   if(m_sync_rod_BCID_per_lumi_B2) m_sync_rod_BCID_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[3]);

   if(m_sync_rod_LVL1ID_per_lumi_PIX) m_sync_rod_LVL1ID_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[4]);
   if(m_sync_rod_LVL1ID_per_lumi_ECA) m_sync_rod_LVL1ID_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[4]);
   if(m_sync_rod_LVL1ID_per_lumi_ECC) m_sync_rod_LVL1ID_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B0) m_sync_rod_LVL1ID_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B1) m_sync_rod_LVL1ID_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B2) m_sync_rod_LVL1ID_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[4]);

   if(m_trunc_mod_EOC_per_lumi_PIX) m_trunc_mod_EOC_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[5]);
   if(m_trunc_mod_EOC_per_lumi_ECA) m_trunc_mod_EOC_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[5]);
   if(m_trunc_mod_EOC_per_lumi_ECC) m_trunc_mod_EOC_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[5]);
   if(m_trunc_mod_EOC_per_lumi_B0) m_trunc_mod_EOC_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[5]);
   if(m_trunc_mod_EOC_per_lumi_B1) m_trunc_mod_EOC_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[5]);
   if(m_trunc_mod_EOC_per_lumi_B2) m_trunc_mod_EOC_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[5]);

   if(m_trunc_mod_hitOF_per_lumi_PIX) m_trunc_mod_hitOF_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[6]);
   if(m_trunc_mod_hitOF_per_lumi_ECA) m_trunc_mod_hitOF_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[6]);
   if(m_trunc_mod_hitOF_per_lumi_ECC) m_trunc_mod_hitOF_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[6]);
   if(m_trunc_mod_hitOF_per_lumi_B0) m_trunc_mod_hitOF_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[6]);
   if(m_trunc_mod_hitOF_per_lumi_B1) m_trunc_mod_hitOF_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[6]);
   if(m_trunc_mod_hitOF_per_lumi_B2) m_trunc_mod_hitOF_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[6]);

   if(m_trunc_mod_EoEOF_per_lumi_PIX) m_trunc_mod_EoEOF_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[7]);
   if(m_trunc_mod_EoEOF_per_lumi_ECA) m_trunc_mod_EoEOF_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[7]);
   if(m_trunc_mod_EoEOF_per_lumi_ECC) m_trunc_mod_EoEOF_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B0) m_trunc_mod_EoEOF_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B1) m_trunc_mod_EoEOF_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B2) m_trunc_mod_EoEOF_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[7]);

   if(m_trunc_rod_HTlim_per_lumi_PIX) m_trunc_rod_HTlim_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[8]);
   if(m_trunc_rod_HTlim_per_lumi_ECA) m_trunc_rod_HTlim_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[8]);
   if(m_trunc_rod_HTlim_per_lumi_ECC) m_trunc_rod_HTlim_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[8]);
   if(m_trunc_rod_HTlim_per_lumi_B0) m_trunc_rod_HTlim_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[8]);
   if(m_trunc_rod_HTlim_per_lumi_B1) m_trunc_rod_HTlim_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[8]);
   if(m_trunc_rod_HTlim_per_lumi_B2) m_trunc_rod_HTlim_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[8]);

   if(m_trunc_rod_FIFOOF_per_lumi_PIX) m_trunc_rod_FIFOOF_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_ECA) m_trunc_rod_FIFOOF_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_ECC) m_trunc_rod_FIFOOF_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B0) m_trunc_rod_FIFOOF_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B1) m_trunc_rod_FIFOOF_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B2) m_trunc_rod_FIFOOF_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[9]);

   if(m_optical_error_per_lumi_PIX) m_optical_error_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[10]);
   if(m_optical_error_per_lumi_ECA) m_optical_error_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[10]);
   if(m_optical_error_per_lumi_ECC) m_optical_error_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[10]);
   if(m_optical_error_per_lumi_B0) m_optical_error_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[10]);
   if(m_optical_error_per_lumi_B1) m_optical_error_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[10]);
   if(m_optical_error_per_lumi_B2) m_optical_error_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[10]);

   if(m_seu_hit_parity_per_lumi_PIX) m_seu_hit_parity_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[11]);
   if(m_seu_hit_parity_per_lumi_ECA) m_seu_hit_parity_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[11]);
   if(m_seu_hit_parity_per_lumi_ECC) m_seu_hit_parity_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[11]);
   if(m_seu_hit_parity_per_lumi_B0) m_seu_hit_parity_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[11]);
   if(m_seu_hit_parity_per_lumi_B1) m_seu_hit_parity_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[11]);
   if(m_seu_hit_parity_per_lumi_B2) m_seu_hit_parity_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[11]);

   if(m_seu_reg_parity_per_lumi_PIX) m_seu_reg_parity_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[12]);
   if(m_seu_reg_parity_per_lumi_ECA) m_seu_reg_parity_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[12]);
   if(m_seu_reg_parity_per_lumi_ECC) m_seu_reg_parity_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[12]);
   if(m_seu_reg_parity_per_lumi_B0) m_seu_reg_parity_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[12]);
   if(m_seu_reg_parity_per_lumi_B1) m_seu_reg_parity_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[12]);
   if(m_seu_reg_parity_per_lumi_B2) m_seu_reg_parity_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[12]);

   if(m_seu_hamming_per_lumi_PIX) m_seu_hamming_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[13]);
   if(m_seu_hamming_per_lumi_ECA) m_seu_hamming_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[13]);
   if(m_seu_hamming_per_lumi_ECC) m_seu_hamming_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[13]);
   if(m_seu_hamming_per_lumi_B0) m_seu_hamming_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[13]);
   if(m_seu_hamming_per_lumi_B1) m_seu_hamming_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[13]);
   if(m_seu_hamming_per_lumi_B2) m_seu_hamming_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[13]);

   if(m_timeout_per_lumi_PIX) m_timeout_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[14]);
   if(m_timeout_per_lumi_ECA) m_timeout_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[14]);
   if(m_timeout_per_lumi_ECC) m_timeout_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[14]);
   if(m_timeout_per_lumi_B0) m_timeout_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[14]);
   if(m_timeout_per_lumi_B1) m_timeout_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[14]);
   if(m_timeout_per_lumi_B2) m_timeout_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[14]);

   if(m_FEwarning_per_lumi_PIX) m_FEwarning_per_lumi_PIX->Fill(m_lumiBlockNum,nErrors_cat_PIX[15]);
   if(m_FEwarning_per_lumi_ECA) m_FEwarning_per_lumi_ECA->Fill(m_lumiBlockNum,nErrors_cat_ECA[15]);
   if(m_FEwarning_per_lumi_ECC) m_FEwarning_per_lumi_ECC->Fill(m_lumiBlockNum,nErrors_cat_ECC[15]);
   if(m_FEwarning_per_lumi_B0) m_FEwarning_per_lumi_B0->Fill(m_lumiBlockNum,nErrors_cat_B0[15]);
   if(m_FEwarning_per_lumi_B1) m_FEwarning_per_lumi_B1->Fill(m_lumiBlockNum,nErrors_cat_B1[15]);
   if(m_FEwarning_per_lumi_B2) m_FEwarning_per_lumi_B2->Fill(m_lumiBlockNum,nErrors_cat_B2[15]);

   if(m_SyncErrors_per_lumi_PIX)            m_SyncErrors_per_lumi_PIX          ->Fill(m_lumiBlockNum,nSyncErrors_PIX);
   if(m_SyncErrors_per_lumi_ECA)        m_SyncErrors_per_lumi_ECA      ->Fill(m_lumiBlockNum,nSyncErrors_ECA);
   if(m_SyncErrors_per_lumi_ECC)        m_SyncErrors_per_lumi_ECC      ->Fill(m_lumiBlockNum,nSyncErrors_ECC);
   if(m_SyncErrors_per_lumi_B0)         m_SyncErrors_per_lumi_B0       ->Fill(m_lumiBlockNum,nSyncErrors_B0);
   if(m_SyncErrors_per_lumi_B1)         m_SyncErrors_per_lumi_B1       ->Fill(m_lumiBlockNum,nSyncErrors_B1);
   if(m_SyncErrors_per_lumi_B2)        m_SyncErrors_per_lumi_B2       ->Fill(m_lumiBlockNum,nSyncErrors_B2);

   if(m_TruncationErrors_per_lumi_PIX)      m_TruncationErrors_per_lumi_PIX    ->Fill(m_lumiBlockNum,nTruncErrors_PIX);
   if(m_TruncationErrors_per_lumi_ECA)  m_TruncationErrors_per_lumi_ECA->Fill(m_lumiBlockNum,nTruncErrors_ECA);
   if(m_TruncationErrors_per_lumi_ECC)  m_TruncationErrors_per_lumi_ECC->Fill(m_lumiBlockNum,nTruncErrors_ECC);
   if(m_TruncationErrors_per_lumi_B0)   m_TruncationErrors_per_lumi_B0 ->Fill(m_lumiBlockNum,nTruncErrors_B0);
   if(m_TruncationErrors_per_lumi_B1)   m_TruncationErrors_per_lumi_B1 ->Fill(m_lumiBlockNum,nTruncErrors_B1);
   if(m_TruncationErrors_per_lumi_B2)   m_TruncationErrors_per_lumi_B2 ->Fill(m_lumiBlockNum,nTruncErrors_B2);

   if(m_OpticalErrors_per_lumi_PIX)         m_OpticalErrors_per_lumi_PIX       ->Fill(m_lumiBlockNum,nOpticalErrors_PIX);
   if(m_OpticalErrors_per_lumi_ECA)     m_OpticalErrors_per_lumi_ECA   ->Fill(m_lumiBlockNum,nOpticalErrors_ECA);
   if(m_OpticalErrors_per_lumi_ECC)     m_OpticalErrors_per_lumi_ECC   ->Fill(m_lumiBlockNum,nOpticalErrors_ECC);
   if(m_OpticalErrors_per_lumi_B0)      m_OpticalErrors_per_lumi_B0    ->Fill(m_lumiBlockNum,nOpticalErrors_B0);
   if(m_OpticalErrors_per_lumi_B1)      m_OpticalErrors_per_lumi_B1    ->Fill(m_lumiBlockNum,nOpticalErrors_B1);
   if(m_OpticalErrors_per_lumi_B2)      m_OpticalErrors_per_lumi_B2    ->Fill(m_lumiBlockNum,nOpticalErrors_B2);

   if(m_SEU_Errors_per_lumi_PIX)            m_SEU_Errors_per_lumi_PIX          ->Fill(m_lumiBlockNum,nSEUErrors_PIX);
   if(m_SEU_Errors_per_lumi_ECA)        m_SEU_Errors_per_lumi_ECA      ->Fill(m_lumiBlockNum,nSEUErrors_ECA);
   if(m_SEU_Errors_per_lumi_ECC)        m_SEU_Errors_per_lumi_ECC      ->Fill(m_lumiBlockNum,nSEUErrors_ECC);
   if(m_SEU_Errors_per_lumi_B0)         m_SEU_Errors_per_lumi_B0       ->Fill(m_lumiBlockNum,nSEUErrors_B0);
   if(m_SEU_Errors_per_lumi_B1)         m_SEU_Errors_per_lumi_B1       ->Fill(m_lumiBlockNum,nSEUErrors_B1);
   if(m_SEU_Errors_per_lumi_B2)         m_SEU_Errors_per_lumi_B2       ->Fill(m_lumiBlockNum,nSEUErrors_B2);

   if(m_TimeoutErrors_per_lumi_PIX)         m_TimeoutErrors_per_lumi_PIX       ->Fill(m_lumiBlockNum,nTimeoutErrors_PIX);
   if(m_TimeoutErrors_per_lumi_ECA)     m_TimeoutErrors_per_lumi_ECA   ->Fill(m_lumiBlockNum,nTimeoutErrors_ECA);
   if(m_TimeoutErrors_per_lumi_ECC)     m_TimeoutErrors_per_lumi_ECC   ->Fill(m_lumiBlockNum,nTimeoutErrors_ECC);
   if(m_TimeoutErrors_per_lumi_B0)      m_TimeoutErrors_per_lumi_B0    ->Fill(m_lumiBlockNum,nTimeoutErrors_B0);
   if(m_TimeoutErrors_per_lumi_B1)      m_TimeoutErrors_per_lumi_B1    ->Fill(m_lumiBlockNum,nTimeoutErrors_B1);
   if(m_TimeoutErrors_per_lumi_B2)      m_TimeoutErrors_per_lumi_B2    ->Fill(m_lumiBlockNum,nTimeoutErrors_B2);
   
   return StatusCode::SUCCESS;
}
