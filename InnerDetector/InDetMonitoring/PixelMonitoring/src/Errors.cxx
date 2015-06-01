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
#include "TString.h"
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

   /// Set the paths and a group of histograms
   std::string path = "Pixel/Errors";
   std::string path2 = "Pixel/ErrorsExpert";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnPixelTrack");
   MonGroup rodHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup rodExpert( this, path2.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   /// error label
   const char * errorType[7] = {"synch (module)", "synch (ROD)", "trunc/high occ. (module)", "trunc/high occ. (ROD)", "optical transmission", "SEU", "timeout"};
   
   sc = rodHistos.regHist(m_error1  = TH1F_LW::create("pixel_ROD_errors",  ("Errors" + m_histTitleExt + "; error bit; # error").c_str(), 64, -0.5, 63.5));
   sc = rodHistos.regHist(m_errorTypes  = TH1F_LW::create("pixel_ROD_types",  ("Errors" + m_histTitleExt + "; error type; # errors").c_str(), 7, -0.5, 6.5));
   LWHist::LWHistAxis *xaxis_errorTypes = m_errorTypes->GetXaxis();
   for(int k=0; k<7; k++) xaxis_errorTypes->SetBinLabel(k+1,errorType[k]);

   std::string modlabel[7];
   modlabel[0]="PIX"; modlabel[1]="ECA"; modlabel[2]="ECC";
   modlabel[3]="B0";  modlabel[4]="B1";  modlabel[5]="B2"; modlabel[6]="IBL";  

   /// Set the number of bins, max/min bin
   std::string axisTitle_LB = ";lumi block;# errors/event";
   std::string axisTitle_ES = ";Error State;# Errors";
   int nbins_LB = 2500; float minbin_LB = -0.5; float maxbin_LB = minbin_LB + (1.0*nbins_LB);
   int nbins_ES = 32;   float minbin_ES = -0.5; float maxbin_ES = minbin_ES + (1.0*nbins_ES);
   std::string tmp; 
   std::string tmp2;

   /// Define the histograms
   //for(int i=0; i<32; i++) {
   //sc = rodHistos.regHist(m_Errors_per_lumi_IBL[i] = TProfile_LW::create( TString::Format("Errors_kBit%i_per_lumi_IBL",i), ("errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(), nbins_LB, minbin_LB, maxbin_LB));
   //}
   sc = rodHistos.regHist(m_otherROD_per_lumi_IBL = TProfile_LW::create("otherROD_per_lumi_IBL", ("Synchronization errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_chip_per_lumi_IBL     = TProfile_LW::create("chip_per_lumi_IBL",     ("Synchronization errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "errors_per_lumi"; tmp2 = "Number of errors per event per LB";
   sc = rodHistos.regHist(m_errors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_errors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "SyncErrors_per_lumi"; tmp2 = "Synchronization errors per event per LB";
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "OpticalErrors_per_lumi"; tmp2 = "Optical errors per event per LB";
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_OpticalErrors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "SEU_Errors_per_lumi"; tmp2 = "SEU errors per event per LB";
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_SEU_Errors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "TruncationErrors_per_lumi"; tmp2 = "Truncation errors per event per LB";
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TruncationErrors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "TimeoutErrors_per_lumi"; tmp2 = "Timeout errors per event per LB";
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_IBL = TProfile_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

   tmp = "Mod_Sync_BCID1_errors_per_lumi"; tmp2 = "Synchronization Module BCID1 errors per event per LB";
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID1_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Mod_Sync_BCID2_errors_per_lumi"; tmp2 = "Synchronization Module BCID2 errors per event per LB"; 
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_BCID2_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Mod_Sync_LVL1ID_errors_per_lumi"; tmp2 = "Synchronization Module LVL1ID errors per event per LB";
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_mod_LVL1ID_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "ROD_Sync_BCID_errors_per_lumi"; tmp2 = "Synchronization ROD BCID errors per event per LB";
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_BCID_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "ROD_Sync_LVL1ID_errors_per_lumi"; tmp2 = "Synchronization ROD LVL1ID errors per event per LB";
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_sync_rod_LVL1ID_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Mod_Trunc_EOC_errors_per_lumi"; tmp2 = "Truncation Module EOC errors per event per LB";
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EOC_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Mod_Trunc_Hit_Overflow_errors_per_lumi"; tmp2 = "Truncation Module Hit Overflow errors per event per LB";
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_hitOF_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Mod_Trunc_EoE_Overflow_errors_per_lumi"; tmp2 = "Truncation Module EoE Overflow errors per event per LB";
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_mod_EoEOF_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "ROD_Trunc_HT_Limit_errors_per_lumi"; tmp2 = "Truncation ROD H/T Limit errors per event per LB";
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_HTlim_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "ROD_Trunc_ROD_OF_errors_per_lumi_PIX"; tmp2 = "Truncation ROD Overflow errors per event per LB";
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_trunc_rod_FIFOOF_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Optical_Errors_per_lumi"; tmp2 = "Optical Errors per event per LB";
   sc = rodExpert.regHist(m_optical_error_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_optical_error_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_optical_error_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_optical_error_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "SEU_Hit_Parity_per_lumi"; tmp2 = "SEU Hit Parity errors per event per LB";
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hit_parity_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "SEU_Register_Parity_per_lumi"; tmp2 = "SEU Register Parity errors per event per LB";
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_reg_parity_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "SEU_Hamming_per_lumi"; tmp2 = "SEU Hamming Code errors per event per LB";
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_seu_hamming_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "ROD_Timeout_per_lumi"; tmp2 = "ROD Formatter Timeout errors per event per LB";
   sc = rodExpert.regHist(m_timeout_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_timeout_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_timeout_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_timeout_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_timeout_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_timeout_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "FE_Warning_per_lumi"; tmp2 = "FE Warning errors per event per LB";
   sc = rodExpert.regHist(m_FEwarning_per_lumi_PIX = TProfile_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_ECA = TProfile_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_ECC = TProfile_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B0  = TProfile_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B1  = TProfile_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   sc = rodExpert.regHist(m_FEwarning_per_lumi_B2  = TProfile_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
   tmp = "Bad_Module_Errors"; tmp2 = "Errors for Bad Module";
   sc = rodExpert.regHist(m_bad_mod_errors_PIX = TH1I_LW::create( (tmp + "_" + modlabel[0]).c_str(), (tmp2 + ", " + modlabel[0] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_ECA = TH1I_LW::create( (tmp + "_" + modlabel[1]).c_str(), (tmp2 + ", " + modlabel[1] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_ECC = TH1I_LW::create( (tmp + "_" + modlabel[2]).c_str(), (tmp2 + ", " + modlabel[2] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_B0  = TH1I_LW::create( (tmp + "_" + modlabel[3]).c_str(), (tmp2 + ", " + modlabel[3] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_B1  = TH1I_LW::create( (tmp + "_" + modlabel[4]).c_str(), (tmp2 + ", " + modlabel[4] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_B2  = TH1I_LW::create( (tmp + "_" + modlabel[5]).c_str(), (tmp2 + ", " + modlabel[5] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));
   sc = rodExpert.regHist(m_bad_mod_errors_IBL = TH1I_LW::create( (tmp + "_" + modlabel[6]).c_str(), (tmp2 + ", " + modlabel[6] + m_histTitleExt + axisTitle_ES).c_str(), nbins_ES, minbin_ES, maxbin_ES));

   sc = rodExpert.regHist(m_mod_errors_IBL = TH1I_LW::create("Module_Errors_IBL",   ("Errors for Modules, IBL" + m_histTitleExt + ";Error State;# Errors").c_str(),32,-0.5,31.5));
   sc = rodExpert.regHist(m_ROD_errors_IBL = TH1I_LW::create("ROD_Errors_IBL",   ("Errors for RODs, IBL" + m_histTitleExt + ";Error State;# Errors").c_str(),8,-0.5,7.5));
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
   const char * errorBitsPIX[32] = {"ROD Trunc FIFO Overflow",   "ROD Trunc H/T Limit",       "2",                 "3",
                                    "Module Trunc EoC",          "SEU Hit Parity",            "SEU Reg Parity",    "SEU Hamming Code",
                                    "FE Warning (Bit Flip)",     "9",                         "10",                "11",
                                    "Module Trunc Hit Overflow", "Module Trunc EoE Overflow", "Module Sync BCID1", "Module Sync BCID2",
                                    "Module Sync LVL1ID",        "17",                        "18",                "19",
                                    "ROD Sync BCID",             "ROD Sync LVL1ID",           "Timeout",           "Optical",
                                    "24",                        "25",                        "26",                "27",
                                    "28",                        "29",                        "30",                "31"
                                    };

   const char* errorBitsFEI4[32] = {"BCID counter","Hamming code 0","Hamming code 1","Hamming code 2","L1_in counter","L1 request counter","L1 register","L1 Trigger ID","readout processor","Fifo_Full flag pulsed","HitOr bus pulsed","11","12","13","3 MSBs of BC and 7 MSBs of L1AC","Skipped trigger counter","Truncated event flag and counter","17","18","19","20","Reset bar RA2b pulsed","Fast PLL clock phase","Fast reference clock phase","Triple redundant mismatch","Write register data error","Address error","Other CMD decoder","CMD decoder bit flip","CMD decoder SEU","Data bus address","Triple redundant mismatch"};

   const char* errorBitsIBLROD[8] = {"Row/ Column Error", "Limit Error", "Timeout Error", "BCID Error", "LVL1ID Error", "Preamble Error", "Masked Link", "Timeout Error"};  //according to pg 67 of RODBOC v1.2.3
   
   const char* errorBitsIBL[32] = {"Row/ Column Error", "Limit Error",          "Timeout Error",       "BCID Error", 
                                   "LVL1ID Error",      "Preamble Error",       "Masked Link",         "Timeout Error",
                                   "BCID counter",      "Hamming code 0",       "Hamming code 1",      "Hamming code 2",
                                   "L1_in counter",     "L1 request counter",   "L1 register",         "L1 Trigger ID",
                                   "readout processor", "Skipped trig counter", "Truncated event flag","Triple Redundant",
                                   "Write reg data",    "Address error",        "Other CMD decoder",   "CMD decoder bit flip",
                                   "CMD decoder SEU",   "Data bus address",     "Triple redundant",    "27",
                                   "28",                "29",                   "30",                  "31"
                                   };

   for (int i=0;i<32;i++){
      //m_mod_errors_IBL->GetXaxis()->SetBinLabel(i+1,errorBitsFEI4[i]);
      m_mod_errors_IBL->GetXaxis()->SetBinLabel(i+1,errorBitsIBL[i]);
      m_bad_mod_errors_IBL->GetXaxis()->SetBinLabel(i+1,errorBitsIBL[i]);
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
      tmp = "errors_over_time"; tmp2 = "Number of Errors as function of time over";
      int nbin = 99; float min = 0.0; float max = 1.0;
      sc = rodHistos.regHist(m_error_time1 = new TProfile( (tmp + "_10min").c_str(), (tmp2 + " 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));    
      sc = rodHistos.regHist(m_error_time2 = new TProfile( (tmp + "_1hr").c_str(),   (tmp2 + " 1 hour.  36 sec/bin"   + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));    
      sc = rodHistos.regHist(m_error_time3 = new TProfile( (tmp + "_6hr").c_str(),   (tmp2 + " 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
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
      bool errorHist = false;
      m_IBLModErrors = new PixelMon2DMapsLW("IBLModErrors", ("IBL Chip Errors" + m_histTitleExt).c_str(), m_doIBL, false);  
      sc = m_IBLModErrors->regHist(rodHistos, m_doIBL, false);
      m_IBLotherRODErrors = new PixelMon2DMapsLW("IBLotherRODErrors", ("IBL other ROD Errors" + m_histTitleExt).c_str(), m_doIBL, false);  
      sc = m_IBLotherRODErrors->regHist(rodHistos, m_doIBL, false);
      m_IBLSyncErrors = new PixelMon2DMapsLW("IBLSyncErrors", ("IBL Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_IBLSyncErrors->regHist(rodHistos, m_doIBL, false);

      m_SyncErrors_mod = new PixelMon2DMapsLW("SyncErrors_mod", ("Module Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SyncErrors_mod->regHist(rodHistos, m_doIBL, true);
      m_SyncErrors_ROD = new PixelMon2DMapsLW("SyncErrors_ROD", ("ROD Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_SyncErrors_ROD->regHist(rodHistos, m_doIBL, true);
      m_TruncErrors_mod = new PixelMon2DMapsLW("TruncErrors_mod", ("Module Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_TruncErrors_mod->regHist(rodHistos, m_doIBL, true);
      m_TruncErrors_ROD = new PixelMon2DMapsLW("TruncErrors_ROD", ("ROD Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      sc = m_TruncErrors_ROD->regHist(rodHistos, m_doIBL, true);

      m_SyncErrors = new PixelMon2DMapsLW("Sync_Errors", ("Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      sc = m_SyncErrors->regHist(rodHistos, m_doIBL, false);
      m_TruncationErrors = new PixelMon2DMapsLW("Truncation_Errors", ("Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      sc = m_TruncationErrors->regHist(rodHistos, m_doIBL, false);
      m_OpticalErrors = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_OpticalErrors->regHist(rodHistos, m_doIBL, false);
      m_SEU_Errors = new PixelMon2DMapsLW("SEU_Errors", ("SEU Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_SEU_Errors->regHist(rodHistos, m_doIBL, false);
      m_TimeoutErrors = new PixelMon2DMapsLW("Timeout_Errors", ("Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_TimeoutErrors->regHist(rodHistos, m_doIBL, false);

      tmp = "SyncErrors_mod"; tmp2 = "Module Synchronization Errors"; errorHist = true;
      m_sync_mod_BCID1  = new PixelMon2DMapsLW( (tmp + "_BCID1").c_str(),  (tmp2 + ", BCID1" + m_histTitleExt).c_str(), m_doIBL, errorHist);  sc = m_sync_mod_BCID1->regHist(rodExpert, m_doIBL, errorHist);
      m_sync_mod_BCID2  = new PixelMon2DMapsLW( (tmp + "_BCID2").c_str(),  (tmp2 + ", BCID2" + m_histTitleExt).c_str(), m_doIBL, errorHist);  sc = m_sync_mod_BCID2->regHist(rodExpert, m_doIBL, errorHist);
      m_sync_mod_LVL1ID = new PixelMon2DMapsLW( (tmp + "_LVL1ID").c_str(), (tmp2 + ", LVL1ID" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_sync_mod_LVL1ID->regHist(rodExpert, m_doIBL, errorHist);
      tmp = "SyncErrors_ROD"; tmp2 = "ROD Synchronization Errors"; errorHist = true;
      m_sync_rod_BCID   = new PixelMon2DMapsLW( (tmp + "_BCID").c_str(),   (tmp2 + ", BCID" + m_histTitleExt).c_str(), m_doIBL, errorHist);   sc = m_sync_rod_BCID->regHist(rodExpert, m_doIBL, errorHist);
      m_sync_rod_LVL1ID = new PixelMon2DMapsLW( (tmp + "_LVL1ID").c_str(), (tmp2 + ", LVL1ID" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_sync_rod_LVL1ID->regHist(rodExpert, m_doIBL, errorHist);
      tmp = "TruncErrors_mod"; tmp2 = "Module Truncation Errors"; errorHist = true;
      m_trunc_mod_EOC   = new PixelMon2DMapsLW( (tmp + "_EoC").c_str(),   (tmp2 + ", EoC" + m_histTitleExt).c_str(), m_doIBL, errorHist);          sc = m_trunc_mod_EOC->regHist(rodExpert, m_doIBL, errorHist);
      m_trunc_mod_hitOF = new PixelMon2DMapsLW( (tmp + "_hitOF").c_str(), (tmp2 + ", Hit Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_mod_hitOF->regHist(rodExpert, m_doIBL, errorHist);
      m_trunc_mod_EoEOF = new PixelMon2DMapsLW( (tmp + "_EoEOF").c_str(), (tmp2 + ", EoE Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_mod_EoEOF->regHist(rodExpert, m_doIBL, errorHist);
      tmp = "TruncErrors_ROD"; tmp2 = "ROD Truncation Errors"; errorHist = true;
      m_trunc_rod_HTlim  = new PixelMon2DMapsLW( (tmp + "_HTlim").c_str(),  (tmp2 + ", H/T Limit" + m_histTitleExt).c_str(), m_doIBL, errorHist);     sc = m_trunc_rod_HTlim->regHist(rodExpert, m_doIBL, errorHist);
      m_trunc_rod_FIFOOF = new PixelMon2DMapsLW( (tmp + "_FIFOOF").c_str(), (tmp2 + ", FIFO Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_rod_FIFOOF->regHist(rodExpert, m_doIBL, errorHist);
      tmp = "SEUErrors"; tmp2 = "SEU Errors"; errorHist = true;
      m_seu_hit_parity = new PixelMon2DMapsLW( (tmp + "_Hit_Parity").c_str(), (tmp2 + ", Hit Parity" + m_histTitleExt).c_str(), m_doIBL, errorHist);      sc = m_seu_hit_parity->regHist(rodExpert, m_doIBL, errorHist);
      m_seu_reg_parity = new PixelMon2DMapsLW( (tmp + "_Reg_Parity").c_str(), (tmp2 + ", Register Parity" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_seu_reg_parity->regHist(rodExpert, m_doIBL, errorHist);
      m_seu_hamming    = new PixelMon2DMapsLW( (tmp + "_Hamming").c_str(),    (tmp2 + ", Hamming Code" + m_histTitleExt).c_str(), m_doIBL, errorHist);    sc = m_seu_hamming->regHist(rodExpert, m_doIBL, errorHist);

      m_timeout = new PixelMon2DMapsLW("ROD_Timeout", ("ROD Formatter Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_timeout->regHist(rodExpert, m_doIBL, true);
      m_FEwarning = new PixelMon2DMapsLW("FE_Warning", ("FE Warning Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_FEwarning->regHist(rodExpert, m_doIBL, true);
      m_optical_error = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_optical_error->regHist(rodExpert, m_doIBL, true);

      if(m_doModules && !m_doOnline){
         tmp = "SyncErrors_mod"; tmp2 = "Avg Module Synchronization Errors"; errorHist = true;
	      m_sync_mod_BCID1_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID1_per_LB").c_str(), ( tmp2 + ", BCID1" + m_histTitleExt).c_str(),"# Mod BCID1 Errors/Event",m_doIBL,errorHist);
	      sc = m_sync_mod_BCID1_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_sync_mod_BCID2_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID2_per_LB").c_str(), ( tmp2 + ", BCID2" + m_histTitleExt).c_str(),"# Mod BCID2 Errors/Event",m_doIBL,errorHist);
	      sc = m_sync_mod_BCID2_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_sync_mod_LVL1ID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_LVL1ID_per_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# Mod LVL1ID Errors/Event",m_doIBL,errorHist);
	      sc = m_sync_mod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_sync_mod_corr_LVL1ID_BCID2_per_LB = new PixelMon2DLumiProfiles( (tmp + "_corr_LVL1ID_BCID2_per_LB").c_str(), ( tmp2 + " Correlations, LVL1ID and BCID2" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID2 Errors/Events with either LVL1ID or BCID2 errors",m_doIBL,errorHist);
	      sc = m_sync_mod_corr_LVL1ID_BCID2_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         tmp = "SyncErrors_rod"; tmp2 = "Avg ROD Synchronization Errors"; errorHist = true;
	      m_sync_rod_BCID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID_per_LB").c_str(), ( tmp2 + ", BCID" + m_histTitleExt).c_str(),"# ROD BCID Errors/Event",m_doIBL,errorHist);
	      sc = m_sync_rod_BCID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_sync_rod_LVL1ID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_LVL1ID_per_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# ROD LVL1ID Errors/Event",m_doIBL,errorHist);
	      sc = m_sync_rod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	m_sync_rod_corr_LVL1ID_BCID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_corr_LVL1ID_BCID_per_LB").c_str(), ( tmp2 + " Correlations, LVL1ID and BCID" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID Errors/Events with either LVL1ID or BCID errors",m_doIBL,errorHist);
	      sc = m_sync_rod_corr_LVL1ID_BCID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         tmp = "TruncErrors_mod"; tmp2 = "Avg Module Truncation Errors"; errorHist = true;
	      m_trunc_mod_EOC_per_LB = new PixelMon2DLumiProfiles( (tmp + "_EOC_per_LB").c_str(), ( tmp2 + ", EoC" + m_histTitleExt).c_str(),"# Mod EoC Errors/Event",m_doIBL,errorHist);
	      sc = m_trunc_mod_EOC_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_trunc_mod_hitOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_hitOF_per_LB").c_str(), ( tmp2 + ", Hit Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      sc = m_trunc_mod_hitOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_trunc_mod_EoEOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_EoEOF_per_LB").c_str(), ( tmp2 + ", EoE Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      sc = m_trunc_mod_EoEOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         tmp = "TruncErrors_rod"; tmp2 = "Avg ROD Truncation Errors"; errorHist = true;
	      m_trunc_rod_HTlim_per_LB = new PixelMon2DLumiProfiles( (tmp + "_HTlim_per_LB").c_str(), ( tmp2 + ", H/T Limit" + m_histTitleExt).c_str(),"# H/T Errors/Event",m_doIBL,errorHist);
	      sc = m_trunc_rod_HTlim_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_trunc_rod_FIFOOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_FIFOOF_per_LB").c_str(), ( tmp2 + ", FIFO Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      sc = m_trunc_rod_FIFOOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         tmp = "SEUErrors"; tmp2 = "Avg SEU Errors"; errorHist = true;
	      m_seu_hit_parity_per_LB = new PixelMon2DLumiProfiles( (tmp + "_Hit_Parity_per_LB").c_str(), ( tmp2 + ", Hit Parity" + m_histTitleExt).c_str(),"# Hit Parity Errors/Event",m_doIBL,errorHist);
	      sc = m_seu_hit_parity_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_seu_reg_parity_per_LB = new PixelMon2DLumiProfiles( (tmp + "_Reg_Parity_per_LB").c_str(), ( tmp2 + ", Register Parity" + m_histTitleExt).c_str(),"# Reg Parity Errors/Event",m_doIBL,errorHist);
	      sc = m_seu_reg_parity_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      m_seu_hamming_per_LB    = new PixelMon2DLumiProfiles( (tmp + "_Hamming_per_LB").c_str(), ( tmp2 + ", Hamming Code" + m_histTitleExt).c_str(),"# HC Errors/Event",m_doIBL,errorHist);
	      sc = m_seu_hamming_per_LB->regHist(rodExpert,m_doIBL,errorHist);

	      m_optical_error_per_LB = new PixelMon2DLumiProfiles("Optical_Errors_per_LB", ("Avg Optical Errors" + m_histTitleExt).c_str(),"# Optical Errors/Event",m_doIBL,true);
	      sc = m_optical_error_per_LB->regHist(rodExpert,m_doIBL,true);
	      m_timeout_per_LB = new PixelMon2DLumiProfiles("ROD_Timeout_per_LB", ("Avg ROD Formatter Timeout Errors" + m_histTitleExt).c_str(),"# Timeout Errors/Event",m_doIBL,true);
	      sc = m_timeout_per_LB->regHist(rodExpert,m_doIBL,true);
	      m_FEwarning_per_LB = new PixelMon2DLumiProfiles("FE_Warning_per_LB", ("Avg FE Warning Errors" + m_histTitleExt).c_str(),"# FE Warning Errors/Event",m_doIBL,true);
	      sc = m_FEwarning_per_LB->regHist(rodExpert,m_doIBL,true);
      }
      
      tmp = "SyncErrors_mod"; tmp2 = "All Module Synchronization Errors"; errorHist = true;
      m_sync_mod_BCID1_int_LB  = new PixelMon2DLumiMaps( (tmp + "_BCID1_int_LB").c_str(), ( tmp2 + ", BCID1" + m_histTitleExt).c_str(),"# Mod BCID1 Errors",m_doIBL,errorHist);
      sc = m_sync_mod_BCID1_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_sync_mod_BCID2_int_LB  = new PixelMon2DLumiMaps( (tmp + "_BCID2_int_LB").c_str(), ( tmp2 + ", BCID2" + m_histTitleExt).c_str(),"# Mod BCID2 Errors",m_doIBL,errorHist);
      sc = m_sync_mod_BCID2_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_sync_mod_LVL1ID_int_LB = new PixelMon2DLumiMaps( (tmp + "_LVL1ID_int_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# Mod LVL1ID Errors",m_doIBL,errorHist);
      sc = m_sync_mod_LVL1ID_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      tmp = "SyncErrors_rod"; tmp2 = "All ROD Synchronization Errors"; errorHist = true;
      m_sync_rod_BCID_int_LB   = new PixelMon2DLumiMaps( (tmp + "_BCID_int_LB").c_str(), ( tmp2 + ", BCID" + m_histTitleExt).c_str(),"# ROD BCID Errors",m_doIBL,errorHist);
      sc = m_sync_rod_BCID_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_sync_rod_LVL1ID_int_LB = new PixelMon2DLumiMaps( (tmp + "_LVL1ID_int_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# ROD LVL1ID Errors",m_doIBL,errorHist);
      sc = m_sync_rod_LVL1ID_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      tmp = "TruncErrors_mod"; tmp2 = "All Module Truncation Errors"; errorHist = true;
      m_trunc_mod_EOC_int_LB   = new PixelMon2DLumiMaps( (tmp + "_EOC_int_LB").c_str(), ( tmp2 + ", EoC" + m_histTitleExt).c_str(),"# Mod EoC Errors",m_doIBL,errorHist);
      sc = m_trunc_mod_EOC_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_trunc_mod_hitOF_int_LB = new PixelMon2DLumiMaps( (tmp + "_hitOF_int_LB").c_str(), ( tmp2 + ", Hit Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,errorHist);
      sc = m_trunc_mod_hitOF_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_trunc_mod_EoEOF_int_LB = new PixelMon2DLumiMaps( (tmp + "_EoEOF_int_LB").c_str(), ( tmp2 + ", EoE Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,errorHist);
      sc = m_trunc_mod_EoEOF_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      tmp = "TruncErrors_rod"; tmp2 = "All ROD Truncation Errors"; errorHist = true;
      m_trunc_rod_HTlim_int_LB  = new PixelMon2DLumiMaps( (tmp + "_HTlim_int_LB").c_str(), ( tmp2 + ", H/T Limit" + m_histTitleExt).c_str(),"# H/T Errors",m_doIBL,errorHist);
      sc = m_trunc_rod_HTlim_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_trunc_rod_FIFOOF_int_LB = new PixelMon2DLumiMaps( (tmp + "_FIFOOF_int_LB").c_str(), ( tmp2 + ", FIFO Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors",m_doIBL,errorHist);
      sc = m_trunc_rod_FIFOOF_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      tmp = "SEUErrors"; tmp2 = "All SEU Errors"; errorHist = true;
      m_seu_hit_parity_int_LB = new PixelMon2DLumiMaps( (tmp + "_Hit_Parity_int_LB").c_str(), ( tmp2 + ", Hit Parity" + m_histTitleExt).c_str(),"# Hit Parity Errors",m_doIBL,errorHist);
      sc = m_seu_hit_parity_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_seu_reg_parity_int_LB = new PixelMon2DLumiMaps( (tmp + "_Reg_Parity_int_LB").c_str(), ( tmp2 + ", Register Parity" + m_histTitleExt).c_str(),"# Reg Parity Errors",m_doIBL,errorHist);
      sc = m_seu_reg_parity_int_LB->regHist(rodExpert,m_doIBL,errorHist);
      m_seu_hamming_int_LB = new PixelMon2DLumiMaps( (tmp + "_Hamming_int_LB").c_str(), ( tmp2 + ", Hamming Code" + m_histTitleExt).c_str(),"# HC Errors",m_doIBL,errorHist);
      sc = m_seu_hamming_int_LB->regHist(rodExpert,m_doIBL,errorHist);

      m_optical_error_int_LB = new PixelMon2DLumiMaps("Optical_Errors_int_LB", ("All Optical Errors" + m_histTitleExt).c_str(),"# Optical Errors",m_doIBL,true);
      sc = m_optical_error_int_LB->regHist(rodExpert,m_doIBL,true);
      m_timeout_int_LB = new PixelMon2DLumiMaps("ROD_Timeout_int_LB", ("All ROD Formatter Timeout Errors" + m_histTitleExt).c_str(),"# Timeout Errors",m_doIBL,true);
      sc = m_timeout_int_LB->regHist(rodExpert,m_doIBL,true); 
      m_FEwarning_int_LB = new PixelMon2DLumiMaps("FE_Warning_int_LB", ("All FE Warning Errors" + m_histTitleExt).c_str(),"# FE Warning Errors",m_doIBL,true);
      sc = m_FEwarning_int_LB->regHist(rodExpert,m_doIBL,true); 
   }

   if(sc.isFailure()) if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
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

   int LBnum = m_manager->lumiBlockNumber();
   PixelID::const_id_iterator idIt       = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd    = m_pixelid->wafer_end();
   //unsigned int errors=0;
   uint64_t errors=0;
   int nErrors_IBL=0;
   int nErrors_PIX=0;
   int nErrors_ECA=0;
   int nErrors_ECC=0;
   int nErrors_B0=0;
   int nErrors_B1=0;
   int nErrors_B2=0;

   int nErrors_cat_IBL[32] = {0};
   int nErrors_cat_PIX[16] = {0};
   int nErrors_cat_ECA[16] = {0};
   int nErrors_cat_ECC[16] = {0};
   int nErrors_cat_B0[16] = {0};
   int nErrors_cat_B1[16] = {0};
   int nErrors_cat_B2[16] = {0};

   int nOtherRODErrors_IBL=0;
   int nChipErrors_IBL=0;

   int nSyncErrors_IBL=0;
   int nSyncErrors_PIX=0;
   int nSyncErrors_ECA=0;
   int nSyncErrors_ECC=0;
   int nSyncErrors_B0=0;
   int nSyncErrors_B1=0;
   int nSyncErrors_B2=0;
   
   int nOpticalErrors_IBL=0;
   int nOpticalErrors_PIX=0;
   int nOpticalErrors_ECA=0;
   int nOpticalErrors_ECC=0;
   int nOpticalErrors_B0=0;
   int nOpticalErrors_B1=0;
   int nOpticalErrors_B2=0;
   
   int nSEUErrors_IBL=0;
   int nSEUErrors_PIX=0;
   int nSEUErrors_ECA=0;
   int nSEUErrors_ECC=0;
   int nSEUErrors_B0=0;
   int nSEUErrors_B1=0;
   int nSEUErrors_B2=0;
   
   int nTruncErrors_IBL=0;
   int nTruncErrors_PIX=0;
   int nTruncErrors_ECA=0;
   int nTruncErrors_ECC=0;
   int nTruncErrors_B0=0;
   int nTruncErrors_B1=0;
   int nTruncErrors_B2=0;
   
   int nTimeoutErrors_IBL=0;
   int nTimeoutErrors_PIX=0;
   int nTimeoutErrors_ECA=0;
   int nTimeoutErrors_ECC=0;
   int nTimeoutErrors_B0=0;
   int nTimeoutErrors_B1=0;
   int nTimeoutErrors_B2=0;

   /// Pixel ID Loop
   for (; idIt != idItEnd; ++idIt) 
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID); 
      errors = m_ErrorSvc->getModuleErrors(id_hash);

      bool isIBL=false;
      bool isError[16] = {false};

      if (m_ErrorSvc->isActive(id_hash) && m_pixelid->barrel_ec(WaferID)==0 && m_pixelid->layer_disk(WaferID)==0 && m_doIBL){isIBL=true;}

      //for (unsigned int kBit=0; kBit<40; kBit++)
      for (unsigned int kBit=0; kBit<32; kBit++)
      {
         //if( (errors & (1<<kBit)) !=0 )
         if( (errors & (static_cast<uint64_t>(1)<<kBit)) !=0 )
         {
	         /// Fill the kBit
            if(isIBL){
	            if(kBit<8){
	               m_ROD_errors_IBL->Fill(kBit);
	            }else{
	            //   m_mod_errors_IBL->Fill(kBit-8);
	            }
	            m_mod_errors_IBL->Fill(kBit);
	         }
  	         if (m_ErrorSvc->isActive(id_hash)&&!m_ErrorSvc->isGood(id_hash)){
	            if(!isIBL) m_bad_mod_errors_PIX->Fill(kBit);
	            if(isIBL) m_bad_mod_errors_IBL->Fill(kBit);
	            if(m_pixelid->barrel_ec(WaferID)==2)  m_bad_mod_errors_ECA->Fill(kBit);
	            if(m_pixelid->barrel_ec(WaferID)==-2) m_bad_mod_errors_ECC->Fill(kBit);
	            if(m_pixelid->barrel_ec(WaferID)==0) {
		            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_bad_mod_errors_B0->Fill(kBit);
		            if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_bad_mod_errors_B1->Fill(kBit);
		            if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_bad_mod_errors_B2->Fill(kBit);
	            }
            }
            if(m_error1) m_error1->Fill(kBit);
            //if(msgLvl(MSG::ERROR)) msg(MSG::ERROR)  << "NT: ERROR IS " << errors << endreq;
     
            /// Count the errors
            if(!isIBL){
               nErrors_PIX++;
            }else{
               nErrors_IBL++;
            } 
	         if(m_pixelid->barrel_ec(WaferID)==2)  nErrors_ECA++;
	         if(m_pixelid->barrel_ec(WaferID)==-2) nErrors_ECC++;
	         if (m_pixelid->barrel_ec(WaferID)==0) {
	           if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nErrors_B0++;
	           if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nErrors_B1++;
	           if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nErrors_B2++;
	         }

            int ErrorType =0;
	         /*
             * if(kBit==20||kBit==21||kBit==14||kBit==16)ErrorType=1;
             * if(kBit==23)ErrorType=2;
             * if(kBit>=5&&kBit<=7)ErrorType=3;
             * if(kBit==4||kBit==12||kBit==13||kBit==1||kBit==0)ErrorType=4;
             * if(kBit==22)ErrorType=5;
	          */
	         if(!isIBL){
	           if(kBit==14||kBit==15||kBit==16) ErrorType=1;  // module synchronization errors   (14: BCID, 15: BCID. 16: LVL1ID)
	           if(kBit==20||kBit==21)           ErrorType=2;  // ROD synchronization errors      (20: BCID, 21: LVL1ID)
	           if(kBit==4 ||kBit==12||kBit==13) ErrorType=3;  // module truncation errors        (4: EOC, 12: hit overflow, 13: EoE overflow)
	           if(kBit==0 ||kBit==1 )           ErrorType=4;  // ROD truncation errors           (0: FIFO Overflow, 1: H/T Limit)
	           if(kBit==23)                     ErrorType=5;  // optical errors                  (preamble (bitflip))
	           if(kBit>=5 &&kBit<=7 )           ErrorType=6;  // SEU (single event upset) errors (hit parity, register parity, hammingcode)
	           if(kBit==22)                     ErrorType=7;  // timeout errors                  (timeout on ROD formatter)
	         }else{
	            if(kBit==3||kBit==4||kBit==8) ErrorType = 1;  // synchronization error   (3:LVL1ID, 4:BCID, 8:BCID counter error)
	            if(kBit==0||kBit==18)         ErrorType = 3;  // module truncation error (0:Row/Column error, 18:Truncated event)
	            if(kBit==1)                   ErrorType = 4;  // ROD truncation error    (1:Limit error)
	            if(kBit==5)                   ErrorType = 5;  // optical error           (5:Preable error)
	            if(kBit==9||kBit==10||kBit==11||kBit==19||kBit==23||kBit==24||kBit==26) ErrorType = 6;  // SEU   (9,10,11: hammingcode, 19:Triple redundant, 23:Bit flip, 24:SEU, 26:Triple redundant)
	            if(kBit==2||kBit==7)          ErrorType = 7;  // Timeout error           (2:Trailer timeout error, 7:readout timeout
	         }

            if(ErrorType)//if there were any errors we care about
            {
               if(m_errorTypes) m_errorTypes->Fill(ErrorType);
               if(m_errors) m_errors->Fill(ErrorType, WaferID, m_pixelid, m_doIBL);

               //fill all the 2d maps for each error type
               if((ErrorType==1||ErrorType==2) && m_SyncErrors) m_SyncErrors->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               if((ErrorType==3||ErrorType==4) && m_TruncationErrors) m_TruncationErrors->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               if(ErrorType==1 && m_SyncErrors_mod ) m_SyncErrors_mod ->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==2 && m_SyncErrors_ROD ) m_SyncErrors_ROD ->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==3 && m_TruncErrors_mod) m_TruncErrors_mod->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==4 && m_TruncErrors_ROD) m_TruncErrors_ROD->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==5 && m_OpticalErrors  ) m_OpticalErrors  ->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==6 && m_SEU_Errors     ) m_SEU_Errors     ->Fill(WaferID,m_pixelid,m_doIBL, false);
               if(ErrorType==7 && m_TimeoutErrors  ) m_TimeoutErrors  ->Fill(WaferID,m_pixelid,m_doIBL, false);
               /// for IBL
               /*
	            if((ErrorType==11 || ErrorType==12) && m_IBLSyncErrors){
                  m_SyncErrors->Fill(WaferID,m_pixelid,m_doIBL, false); 
                  m_IBLSyncErrors->Fill(WaferID,m_pixelid,m_doIBL,false);
               }
               if((ErrorType==8 || ErrorType==9) && isIBL){
                  if(m_TruncationErrors) m_TruncationErrors->Fill(WaferID,m_pixelid,m_doIBL, false); 
               }
               if((ErrorType==10 || ErrorType==15) && isIBL){
                  if(m_TimeoutErrors) m_TimeoutErrors->Fill(WaferID,m_pixelid,m_doIBL, false); 
               }
               if((ErrorType==13) && isIBL){
                  if(m_OpticalErrors) m_OpticalErrors->Fill(WaferID,m_pixelid,m_doIBL, false); 
               }
               if((ErrorType==13) && isIBL){
                  if(m_OpticalErrors) m_OpticalErrors->Fill(WaferID,m_pixelid,m_doIBL, false); 
               }
               */

	            //if(isIBL && ErrorType>0 && m_IBLotherRODErrors) m_IBLotherRODErrors->Fill(WaferID,m_pixelid,m_doIBL,false);
	            //if(ErrorType==16 && m_IBLModErrors) m_IBLModErrors->Fill(WaferID,m_pixelid,m_doIBL,false);

	            /// for what???
               //if(kBit== 1){ErrorType = 8;}
	            //if(kBit== 2){ErrorType = 9;}
	            //if(kBit== 3){ErrorType = 10;}
	            //if(kBit== 4){ErrorType = 11;}
	            //if(kBit== 5){ErrorType = 12;}
	            //if(kBit== 6){ErrorType = 13;}
	            //if(kBit== 7){ErrorType = 14;}
	            //if(kBit== 8){ErrorType = 15;}
	            //if(kBit> 8) {ErrorType = 16;} //any IBL chip errors

	            if(m_doLumiBlock){
		            if(m_errors_LB)m_errors_LB->Fill(WaferID,m_pixelid,m_doIBL, true);
	            }
	       
	            /// Count the each error
               if (ErrorType==1||ErrorType==2){ // Synchronization error other than IBL
	               if(isIBL) nSyncErrors_IBL++;
	               if(!isIBL) nSyncErrors_PIX++;
		            if(m_pixelid->barrel_ec(WaferID)==2) nSyncErrors_ECA++;
		            if(m_pixelid->barrel_ec(WaferID)==-2) nSyncErrors_ECC++;
		            if(m_pixelid->barrel_ec(WaferID)==0) {
		              if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nSyncErrors_B0++;
		              if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nSyncErrors_B1++;
		              if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nSyncErrors_B2++;
		            }
	            }
	            if (ErrorType==3||ErrorType==4){ // Truncation error other than IBL
		            if(isIBL)nTruncErrors_IBL++;
		            if(!isIBL)nTruncErrors_PIX++;
		            if(m_pixelid->barrel_ec(WaferID)==2) nTruncErrors_ECA++;
		            if(m_pixelid->barrel_ec(WaferID)==-2) nTruncErrors_ECC++;
		            if(m_pixelid->barrel_ec(WaferID)==0) {
		              if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nTruncErrors_B0++;
		              if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nTruncErrors_B1++;
		              if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nTruncErrors_B2++;
		            }
	            }
	            if (ErrorType==5){ // Optical error other than IBL
	              if(isIBL)nOpticalErrors_IBL++;
	              if(!isIBL)nOpticalErrors_PIX++;
		           if(m_pixelid->barrel_ec(WaferID)==2) nOpticalErrors_ECA++;
		           if(m_pixelid->barrel_ec(WaferID)==-2) nOpticalErrors_ECC++;
		           if(m_pixelid->barrel_ec(WaferID)==0) {
		             if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nOpticalErrors_B0++;
		             if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nOpticalErrors_B1++;
		             if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nOpticalErrors_B2++;
		           }
	            }
	            if (ErrorType==6){ // SEU error other than IBL
		            if(isIBL)nSEUErrors_IBL++;
		            if(!isIBL)nSEUErrors_PIX++;
		            if(m_pixelid->barrel_ec(WaferID)==2) nSEUErrors_ECA++;
		            if(m_pixelid->barrel_ec(WaferID)==-2) nSEUErrors_ECC++;
		            if(m_pixelid->barrel_ec(WaferID)==0) {
		              if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nSEUErrors_B0++;
		              if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nSEUErrors_B1++;
		              if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nSEUErrors_B2++;
		            }
	            }
	            if (ErrorType==7){ // Timeout error other than IBL
	              if(isIBL)nTimeoutErrors_IBL++;
	              if(!isIBL)nTimeoutErrors_PIX++;
		           if(m_pixelid->barrel_ec(WaferID)==2) nTimeoutErrors_ECA++;
		           if(m_pixelid->barrel_ec(WaferID)==-2) nTimeoutErrors_ECC++;
		           if(m_pixelid->barrel_ec(WaferID)==0) {
		             if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nTimeoutErrors_B0++;
		             if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nTimeoutErrors_B1++;
		             if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nTimeoutErrors_B2++;
		           }
	            }

               /// for IBL
               //if(isIBL && (ErrorType==8 || ErrorType==9)) nTruncErrors_IBL++;
               //if(isIBL && (ErrorType==10 || ErrorType==15)) nTimeoutErrors_IBL++;
               //if(isIBL && ErrorType==13) nOpticalErrors_IBL++;
	            //if(isIBL && (ErrorType==11 || ErrorType==12)) nSyncErrors_IBL++;
	            //if(isIBL && (ErrorType==8 || ErrorType==9 || ErrorType==10 || ErrorType==13 || ErrorType==14 || ErrorType==15)) nOtherRODErrors_IBL++;
	            //if(isIBL && (ErrorType==16)) nChipErrors_IBL++;
	            //CCC CCCC CCCC CCCC CCCC EPpl bzhv
            } // End of if(ErrorType)
            
            if(kBit==14){ // module synchronization error other than IBL
	            if(m_sync_mod_BCID1) m_sync_mod_BCID1->Fill(WaferID,m_pixelid,m_doIBL, true);
               if(m_sync_mod_BCID1_int_LB) m_sync_mod_BCID1_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==15){ // module synchronization error other than IBL
	            if(m_sync_mod_BCID2) m_sync_mod_BCID2->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_mod_BCID2_int_LB) m_sync_mod_BCID2_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==16){ // module synchronization error (LVL1ID) other than IBL
	            if(m_sync_mod_LVL1ID) m_sync_mod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_mod_LVL1ID_int_LB) m_sync_mod_LVL1ID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==20){ // ROD synchronization error (BCID) other than IBL
	            if(m_sync_rod_BCID) m_sync_rod_BCID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_rod_BCID_int_LB) m_sync_rod_BCID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==21){ // ROD synchronization error (LVL1ID) other than IBL
	            if(m_sync_rod_LVL1ID) m_sync_rod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_sync_rod_LVL1ID_int_LB) m_sync_rod_LVL1ID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==4){ // Module truncation errors (EOC)
	            if(m_trunc_mod_EOC) m_trunc_mod_EOC->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_EOC_int_LB) m_trunc_mod_EOC_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==12){ // Module truncation errors (hit overflow)
	            if(m_trunc_mod_hitOF) m_trunc_mod_hitOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_hitOF_int_LB) m_trunc_mod_hitOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==13){ // Module truncation errors (EoE overflow)
	            if(m_trunc_mod_EoEOF) m_trunc_mod_EoEOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_mod_EoEOF_int_LB) m_trunc_mod_EoEOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==1){ // ROD truncation errors (H/T Limit)
	            if(m_trunc_rod_HTlim) m_trunc_rod_HTlim->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_rod_HTlim_int_LB) m_trunc_rod_HTlim_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==0){ // ROD truncation errors (FIFO Overflow)
	            if(m_trunc_rod_FIFOOF) m_trunc_rod_FIFOOF->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_trunc_rod_FIFOOF_int_LB) m_trunc_rod_FIFOOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            if(kBit==23){ // optical errors (preable)
	            if(m_optical_error) m_optical_error->Fill(WaferID,m_pixelid,m_doIBL,true);
               if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
	            if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(LBnum,WaferID,m_pixelid,m_doIBL,true);
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
               if(m_seu_reg_parity_int_LB) m_seu_reg_parity_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
               if(m_seu_hamming_int_LB) m_seu_hamming_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
               if(m_timeout_int_LB) m_timeout_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
               if(m_FEwarning_int_LB) m_FEwarning_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
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
            // FOR IBL
            if(isIBL){ 
               //nErrors_cat_IBL[kBit]++;
            }

         }//end bit shifting
      }//end for loop over bits

      if(m_sync_mod_BCID1_per_LB){
         if (isError[0]) m_sync_mod_BCID1_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_sync_mod_BCID1_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_BCID2_per_LB){
         if (isError[1]) m_sync_mod_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_sync_mod_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_LVL1ID_per_LB){
         if (isError[2]) m_sync_mod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_sync_mod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_mod_corr_LVL1ID_BCID2_per_LB){
         if (isError[2]&&isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else if (isError[2]||isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_BCID_per_LB){
         if (isError[3]) m_sync_rod_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_sync_rod_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_LVL1ID_per_LB){
         if (isError[4]) m_sync_rod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_sync_rod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_sync_rod_corr_LVL1ID_BCID_per_LB){
         if (isError[3]&&isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else if (isError[3]||isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_EOC_per_LB){
         if (isError[5]) m_trunc_mod_EOC_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_trunc_mod_EOC_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_hitOF_per_LB){
         if (isError[6]) m_trunc_mod_hitOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_trunc_mod_hitOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_mod_EoEOF_per_LB){
         if (isError[7]) m_trunc_mod_EoEOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_trunc_mod_EoEOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_rod_HTlim_per_LB){
         if (isError[8]) m_trunc_rod_HTlim_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_trunc_rod_HTlim_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_trunc_rod_FIFOOF_per_LB){
         if (isError[9]) m_trunc_rod_FIFOOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_trunc_rod_FIFOOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_optical_error_per_LB){
         if (isError[10]) m_optical_error_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_optical_error_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_hit_parity_per_LB){
         if (isError[11]) m_seu_hit_parity_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_seu_hit_parity_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_reg_parity_per_LB){
         if (isError[12]) m_seu_reg_parity_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_seu_reg_parity_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_seu_hamming_per_LB){
         if (isError[13]) m_seu_hamming_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_seu_hamming_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_timeout_per_LB){
         if (isError[14]) m_timeout_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_timeout_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
      if(m_FEwarning_per_LB){
         if (isError[15]) m_FEwarning_per_LB->Fill(LBnum,WaferID,m_pixelid);
         else m_FEwarning_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      }   
   } //end loop over all identifiers
  
   if(m_error_time1&&m_error_time2&&m_error_time3)FillTimeHisto(double(nErrors_PIX),  m_error_time1, m_error_time2, m_error_time3,10.,60.,360.); //occupancy vs time);

   if(m_otherROD_per_lumi_IBL) m_otherROD_per_lumi_IBL->Fill(LBnum,nOtherRODErrors_IBL);
   if(m_chip_per_lumi_IBL) m_chip_per_lumi_IBL->Fill(LBnum,nChipErrors_IBL);
   for(int i=0; i<32; i++){
      //if(m_Errors_per_lumi_IBL[i]) m_Errors_per_lumi_IBL[i]->Fill(LBnum, nErrors_cat_IBL[i]);
   }

   if(m_errors_per_lumi_IBL) m_errors_per_lumi_IBL->Fill(LBnum,nErrors_IBL);
   if(m_errors_per_lumi_PIX) m_errors_per_lumi_PIX->Fill(LBnum,nErrors_PIX);
   if(m_errors_per_lumi_ECA) m_errors_per_lumi_ECA->Fill(LBnum,nErrors_ECA);
   if(m_errors_per_lumi_ECC) m_errors_per_lumi_ECC->Fill(LBnum,nErrors_ECC);
   if(m_errors_per_lumi_B0)  m_errors_per_lumi_B0 ->Fill(LBnum,nErrors_B0);
   if(m_errors_per_lumi_B1)  m_errors_per_lumi_B1 ->Fill(LBnum,nErrors_B1);
   if(m_errors_per_lumi_B2)  m_errors_per_lumi_B2 ->Fill(LBnum,nErrors_B2);

   if(m_sync_mod_BCID1_per_lumi_PIX) m_sync_mod_BCID1_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[0]);
   if(m_sync_mod_BCID1_per_lumi_ECA) m_sync_mod_BCID1_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[0]);
   if(m_sync_mod_BCID1_per_lumi_ECC) m_sync_mod_BCID1_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[0]);
   if(m_sync_mod_BCID1_per_lumi_B0)  m_sync_mod_BCID1_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[0]);
   if(m_sync_mod_BCID1_per_lumi_B1)  m_sync_mod_BCID1_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[0]);
   if(m_sync_mod_BCID1_per_lumi_B2)  m_sync_mod_BCID1_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[0]);

   if(m_sync_mod_BCID2_per_lumi_PIX) m_sync_mod_BCID2_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[1]);
   if(m_sync_mod_BCID2_per_lumi_ECA) m_sync_mod_BCID2_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[1]);
   if(m_sync_mod_BCID2_per_lumi_ECC) m_sync_mod_BCID2_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[1]);
   if(m_sync_mod_BCID2_per_lumi_B0)  m_sync_mod_BCID2_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[1]);
   if(m_sync_mod_BCID2_per_lumi_B1)  m_sync_mod_BCID2_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[1]);
   if(m_sync_mod_BCID2_per_lumi_B2)  m_sync_mod_BCID2_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[1]);

   if(m_sync_mod_LVL1ID_per_lumi_PIX) m_sync_mod_LVL1ID_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[2]);
   if(m_sync_mod_LVL1ID_per_lumi_ECA) m_sync_mod_LVL1ID_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[2]);
   if(m_sync_mod_LVL1ID_per_lumi_ECC) m_sync_mod_LVL1ID_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B0)  m_sync_mod_LVL1ID_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B1)  m_sync_mod_LVL1ID_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[2]);
   if(m_sync_mod_LVL1ID_per_lumi_B2)  m_sync_mod_LVL1ID_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[2]);

   if(m_sync_rod_BCID_per_lumi_PIX) m_sync_rod_BCID_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[3]);
   if(m_sync_rod_BCID_per_lumi_ECA) m_sync_rod_BCID_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[3]);
   if(m_sync_rod_BCID_per_lumi_ECC) m_sync_rod_BCID_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[3]);
   if(m_sync_rod_BCID_per_lumi_B0)  m_sync_rod_BCID_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[3]);
   if(m_sync_rod_BCID_per_lumi_B1)  m_sync_rod_BCID_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[3]);
   if(m_sync_rod_BCID_per_lumi_B2)  m_sync_rod_BCID_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[3]);

   if(m_sync_rod_LVL1ID_per_lumi_PIX) m_sync_rod_LVL1ID_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[4]);
   if(m_sync_rod_LVL1ID_per_lumi_ECA) m_sync_rod_LVL1ID_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[4]);
   if(m_sync_rod_LVL1ID_per_lumi_ECC) m_sync_rod_LVL1ID_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B0)  m_sync_rod_LVL1ID_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B1)  m_sync_rod_LVL1ID_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[4]);
   if(m_sync_rod_LVL1ID_per_lumi_B2)  m_sync_rod_LVL1ID_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[4]);

   if(m_trunc_mod_EOC_per_lumi_PIX) m_trunc_mod_EOC_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[5]);
   if(m_trunc_mod_EOC_per_lumi_ECA) m_trunc_mod_EOC_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[5]);
   if(m_trunc_mod_EOC_per_lumi_ECC) m_trunc_mod_EOC_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[5]);
   if(m_trunc_mod_EOC_per_lumi_B0)  m_trunc_mod_EOC_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[5]);
   if(m_trunc_mod_EOC_per_lumi_B1)  m_trunc_mod_EOC_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[5]);
   if(m_trunc_mod_EOC_per_lumi_B2)  m_trunc_mod_EOC_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[5]);

   if(m_trunc_mod_hitOF_per_lumi_PIX) m_trunc_mod_hitOF_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[6]);
   if(m_trunc_mod_hitOF_per_lumi_ECA) m_trunc_mod_hitOF_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[6]);
   if(m_trunc_mod_hitOF_per_lumi_ECC) m_trunc_mod_hitOF_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[6]);
   if(m_trunc_mod_hitOF_per_lumi_B0)  m_trunc_mod_hitOF_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[6]);
   if(m_trunc_mod_hitOF_per_lumi_B1)  m_trunc_mod_hitOF_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[6]);
   if(m_trunc_mod_hitOF_per_lumi_B2)  m_trunc_mod_hitOF_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[6]);

   if(m_trunc_mod_EoEOF_per_lumi_PIX) m_trunc_mod_EoEOF_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[7]);
   if(m_trunc_mod_EoEOF_per_lumi_ECA) m_trunc_mod_EoEOF_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[7]);
   if(m_trunc_mod_EoEOF_per_lumi_ECC) m_trunc_mod_EoEOF_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B0) m_trunc_mod_EoEOF_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B1) m_trunc_mod_EoEOF_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[7]);
   if(m_trunc_mod_EoEOF_per_lumi_B2) m_trunc_mod_EoEOF_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[7]);

   if(m_trunc_rod_HTlim_per_lumi_PIX) m_trunc_rod_HTlim_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[8]);
   if(m_trunc_rod_HTlim_per_lumi_ECA) m_trunc_rod_HTlim_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[8]);
   if(m_trunc_rod_HTlim_per_lumi_ECC) m_trunc_rod_HTlim_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[8]);
   if(m_trunc_rod_HTlim_per_lumi_B0)  m_trunc_rod_HTlim_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[8]);
   if(m_trunc_rod_HTlim_per_lumi_B1)  m_trunc_rod_HTlim_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[8]);
   if(m_trunc_rod_HTlim_per_lumi_B2)  m_trunc_rod_HTlim_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[8]);

   if(m_trunc_rod_FIFOOF_per_lumi_PIX) m_trunc_rod_FIFOOF_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_ECA) m_trunc_rod_FIFOOF_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_ECC) m_trunc_rod_FIFOOF_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B0)  m_trunc_rod_FIFOOF_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B1)  m_trunc_rod_FIFOOF_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[9]);
   if(m_trunc_rod_FIFOOF_per_lumi_B2)  m_trunc_rod_FIFOOF_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[9]);

   if(m_optical_error_per_lumi_PIX) m_optical_error_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[10]);
   if(m_optical_error_per_lumi_ECA) m_optical_error_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[10]);
   if(m_optical_error_per_lumi_ECC) m_optical_error_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[10]);
   if(m_optical_error_per_lumi_B0)  m_optical_error_per_lumi_B0 ->Fill(LBnum,nErrors_cat_B0[10]);
   if(m_optical_error_per_lumi_B1)  m_optical_error_per_lumi_B1 ->Fill(LBnum,nErrors_cat_B1[10]);
   if(m_optical_error_per_lumi_B2)  m_optical_error_per_lumi_B2 ->Fill(LBnum,nErrors_cat_B2[10]);

   if(m_seu_hit_parity_per_lumi_PIX) m_seu_hit_parity_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[11]);
   if(m_seu_hit_parity_per_lumi_ECA) m_seu_hit_parity_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[11]);
   if(m_seu_hit_parity_per_lumi_ECC) m_seu_hit_parity_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[11]);
   if(m_seu_hit_parity_per_lumi_B0) m_seu_hit_parity_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[11]);
   if(m_seu_hit_parity_per_lumi_B1) m_seu_hit_parity_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[11]);
   if(m_seu_hit_parity_per_lumi_B2) m_seu_hit_parity_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[11]);

   if(m_seu_reg_parity_per_lumi_PIX) m_seu_reg_parity_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[12]);
   if(m_seu_reg_parity_per_lumi_ECA) m_seu_reg_parity_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[12]);
   if(m_seu_reg_parity_per_lumi_ECC) m_seu_reg_parity_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[12]);
   if(m_seu_reg_parity_per_lumi_B0) m_seu_reg_parity_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[12]);
   if(m_seu_reg_parity_per_lumi_B1) m_seu_reg_parity_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[12]);
   if(m_seu_reg_parity_per_lumi_B2) m_seu_reg_parity_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[12]);

   if(m_seu_hamming_per_lumi_PIX) m_seu_hamming_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[13]);
   if(m_seu_hamming_per_lumi_ECA) m_seu_hamming_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[13]);
   if(m_seu_hamming_per_lumi_ECC) m_seu_hamming_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[13]);
   if(m_seu_hamming_per_lumi_B0) m_seu_hamming_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[13]);
   if(m_seu_hamming_per_lumi_B1) m_seu_hamming_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[13]);
   if(m_seu_hamming_per_lumi_B2) m_seu_hamming_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[13]);

   if(m_timeout_per_lumi_PIX) m_timeout_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[14]);
   if(m_timeout_per_lumi_ECA) m_timeout_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[14]);
   if(m_timeout_per_lumi_ECC) m_timeout_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[14]);
   if(m_timeout_per_lumi_B0) m_timeout_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[14]);
   if(m_timeout_per_lumi_B1) m_timeout_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[14]);
   if(m_timeout_per_lumi_B2) m_timeout_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[14]);

   if(m_FEwarning_per_lumi_PIX) m_FEwarning_per_lumi_PIX->Fill(LBnum,nErrors_cat_PIX[15]);
   if(m_FEwarning_per_lumi_ECA) m_FEwarning_per_lumi_ECA->Fill(LBnum,nErrors_cat_ECA[15]);
   if(m_FEwarning_per_lumi_ECC) m_FEwarning_per_lumi_ECC->Fill(LBnum,nErrors_cat_ECC[15]);
   if(m_FEwarning_per_lumi_B0) m_FEwarning_per_lumi_B0->Fill(LBnum,nErrors_cat_B0[15]);
   if(m_FEwarning_per_lumi_B1) m_FEwarning_per_lumi_B1->Fill(LBnum,nErrors_cat_B1[15]);
   if(m_FEwarning_per_lumi_B2) m_FEwarning_per_lumi_B2->Fill(LBnum,nErrors_cat_B2[15]);

   if(m_SyncErrors_per_lumi_IBL)        m_SyncErrors_per_lumi_IBL      ->Fill(LBnum,nSyncErrors_IBL);
   if(m_SyncErrors_per_lumi_PIX)        m_SyncErrors_per_lumi_PIX      ->Fill(LBnum,nSyncErrors_PIX);
   if(m_SyncErrors_per_lumi_ECA)        m_SyncErrors_per_lumi_ECA      ->Fill(LBnum,nSyncErrors_ECA);
   if(m_SyncErrors_per_lumi_ECC)        m_SyncErrors_per_lumi_ECC      ->Fill(LBnum,nSyncErrors_ECC);
   if(m_SyncErrors_per_lumi_B0)         m_SyncErrors_per_lumi_B0       ->Fill(LBnum,nSyncErrors_B0);
   if(m_SyncErrors_per_lumi_B1)         m_SyncErrors_per_lumi_B1       ->Fill(LBnum,nSyncErrors_B1);
   if(m_SyncErrors_per_lumi_B2)         m_SyncErrors_per_lumi_B2       ->Fill(LBnum,nSyncErrors_B2);

   if(m_TruncationErrors_per_lumi_IBL)  m_TruncationErrors_per_lumi_IBL->Fill(LBnum,nTruncErrors_IBL);
   if(m_TruncationErrors_per_lumi_PIX)  m_TruncationErrors_per_lumi_PIX->Fill(LBnum,nTruncErrors_PIX);
   if(m_TruncationErrors_per_lumi_ECA)  m_TruncationErrors_per_lumi_ECA->Fill(LBnum,nTruncErrors_ECA);
   if(m_TruncationErrors_per_lumi_ECC)  m_TruncationErrors_per_lumi_ECC->Fill(LBnum,nTruncErrors_ECC);
   if(m_TruncationErrors_per_lumi_B0)   m_TruncationErrors_per_lumi_B0 ->Fill(LBnum,nTruncErrors_B0);
   if(m_TruncationErrors_per_lumi_B1)   m_TruncationErrors_per_lumi_B1 ->Fill(LBnum,nTruncErrors_B1);
   if(m_TruncationErrors_per_lumi_B2)   m_TruncationErrors_per_lumi_B2 ->Fill(LBnum,nTruncErrors_B2);

   if(m_OpticalErrors_per_lumi_IBL)     m_OpticalErrors_per_lumi_IBL   ->Fill(LBnum,nOpticalErrors_IBL);
   if(m_OpticalErrors_per_lumi_PIX)     m_OpticalErrors_per_lumi_PIX   ->Fill(LBnum,nOpticalErrors_PIX);
   if(m_OpticalErrors_per_lumi_ECA)     m_OpticalErrors_per_lumi_ECA   ->Fill(LBnum,nOpticalErrors_ECA);
   if(m_OpticalErrors_per_lumi_ECC)     m_OpticalErrors_per_lumi_ECC   ->Fill(LBnum,nOpticalErrors_ECC);
   if(m_OpticalErrors_per_lumi_B0)      m_OpticalErrors_per_lumi_B0    ->Fill(LBnum,nOpticalErrors_B0);
   if(m_OpticalErrors_per_lumi_B1)      m_OpticalErrors_per_lumi_B1    ->Fill(LBnum,nOpticalErrors_B1);
   if(m_OpticalErrors_per_lumi_B2)      m_OpticalErrors_per_lumi_B2    ->Fill(LBnum,nOpticalErrors_B2);

   if(m_SEU_Errors_per_lumi_IBL)        m_SEU_Errors_per_lumi_IBL      ->Fill(LBnum,nSEUErrors_IBL);
   if(m_SEU_Errors_per_lumi_PIX)        m_SEU_Errors_per_lumi_PIX      ->Fill(LBnum,nSEUErrors_PIX);
   if(m_SEU_Errors_per_lumi_ECA)        m_SEU_Errors_per_lumi_ECA      ->Fill(LBnum,nSEUErrors_ECA);
   if(m_SEU_Errors_per_lumi_ECC)        m_SEU_Errors_per_lumi_ECC      ->Fill(LBnum,nSEUErrors_ECC);
   if(m_SEU_Errors_per_lumi_B0)         m_SEU_Errors_per_lumi_B0       ->Fill(LBnum,nSEUErrors_B0);
   if(m_SEU_Errors_per_lumi_B1)         m_SEU_Errors_per_lumi_B1       ->Fill(LBnum,nSEUErrors_B1);
   if(m_SEU_Errors_per_lumi_B2)         m_SEU_Errors_per_lumi_B2       ->Fill(LBnum,nSEUErrors_B2);

   if(m_TimeoutErrors_per_lumi_IBL)     m_TimeoutErrors_per_lumi_IBL   ->Fill(LBnum,nTimeoutErrors_IBL);
   if(m_TimeoutErrors_per_lumi_PIX)     m_TimeoutErrors_per_lumi_PIX   ->Fill(LBnum,nTimeoutErrors_PIX);
   if(m_TimeoutErrors_per_lumi_ECA)     m_TimeoutErrors_per_lumi_ECA   ->Fill(LBnum,nTimeoutErrors_ECA);
   if(m_TimeoutErrors_per_lumi_ECC)     m_TimeoutErrors_per_lumi_ECC   ->Fill(LBnum,nTimeoutErrors_ECC);
   if(m_TimeoutErrors_per_lumi_B0)      m_TimeoutErrors_per_lumi_B0    ->Fill(LBnum,nTimeoutErrors_B0);
   if(m_TimeoutErrors_per_lumi_B1)      m_TimeoutErrors_per_lumi_B1    ->Fill(LBnum,nTimeoutErrors_B1);
   if(m_TimeoutErrors_per_lumi_B2)      m_TimeoutErrors_per_lumi_B2    ->Fill(LBnum,nTimeoutErrors_B2);
   
   return StatusCode::SUCCESS;
}
