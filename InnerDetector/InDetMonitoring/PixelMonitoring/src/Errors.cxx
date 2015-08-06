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
#include "LWHists/TProfile2D_LW.h"
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
   const std::string errorType[ErrorCategoryMODROD::COUNT] = {"SyncErrors_Mod", "SyncErrors_ROD", "TruncErrors_Mod", "TruncErrors_ROD", "OpticalErrors", "SEUErrors", "TimeoutErrors"};
   const std::string errorCategory[ErrorCategory::COUNT] = {"SyncErrors", "TruncErrors", "OpticalErrors", "SEUErrors", "TimeoutErrors"};

   //sc = rodHistos.regHist(m_error1  = TH1F_LW::create("pixel_ROD_errors",  ("Errors" + m_histTitleExt + "; error bit; # error").c_str(), 64, -0.5, 63.5));
   //sc = rodHistos.regHist(m_errorTypes  = TH1F_LW::create("pixel_ROD_types",  ("Errors" + m_histTitleExt + "; error type; # errors").c_str(), 7, -0.5, 6.5));
   //LWHist::LWHistAxis *xaxis_errorTypes = m_errorTypes->GetXaxis();
   //for(int k=0; k<7; k++) xaxis_errorTypes->SetBinLabel(k+1,errorType[k]);

   std::string modlabel[7];
   modlabel[0]="PIX"; modlabel[1]="ECA"; modlabel[2]="ECC";
   modlabel[3]="B0";  modlabel[4]="B1";  modlabel[5]="B2"; modlabel[6]="IBL";  
   std::string modlabel2[8];
   modlabel2[0]="ECA"; modlabel2[1]="ECC";
   modlabel2[2]="B0";  modlabel2[3]="B1";  modlabel2[4]="B2"; 
   modlabel2[5]="IBL"; modlabel2[6]="IBL2D"; modlabel2[7]="IBL3D";
   
   int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 32};
   float min_eta[PixLayer::COUNT] = {-0.5, -0.5, -6.5, -6.5, -6.5, -16.5};
   float max_eta[PixLayer::COUNT] = {2.5, 3.5, 6.5, 6.5, 6.5, 15.5};

   /// Set the number of bins, max/min bin
   std::string axisTitle_LB = ";lumi block;# errors/event";
   std::string axisTitle_ES = ";Error State;# Errors";
   int nbins_LB = 2500; float minbin_LB = -0.5; float maxbin_LB = minbin_LB + (1.0*nbins_LB);
   int nbins_ES = 32;   float minbin_ES = -0.5; float maxbin_ES = minbin_ES + (1.0*nbins_ES);
   std::string tmp; 
   std::string tmp2;
   std::string tmp3[ErrorCategory::COUNT] = {"SyncErrorsFrac_per_event", "TruncationErrorsFrac_per_event", "OpticalErrorsFrac_per_event", "SEUErrorsFrac_per_event", "TimeoutErrorsFrac_per_event"};

   /// Define the histograms
   //sc = rodHistos.regHist(m_otherROD_per_lumi_IBL = TProfile_LW::create("otherROD_per_lumi_IBL", ("Synchronization errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(), nbins_LB, minbin_LB, maxbin_LB));
   //sc = rodHistos.regHist(m_chip_per_lumi_IBL     = TProfile_LW::create("chip_per_lumi_IBL",     ("Synchronization errors per event per LB" + m_histTitleExt + ";lumi block;# errors/event").c_str(), nbins_LB, minbin_LB, maxbin_LB));

   //for( int i=0; i<PixLayer::COUNT; i++){
   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      tmp = "errors_per_lumi"; tmp2 = "Number of errors per event per LB";
      sc = rodHistos.regHist(m_errors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "SyncErrors_per_lumi"; tmp2 = "Synchronization errors per event per LB";
      sc = rodHistos.regHist(m_SyncErrors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "OpticalErrors_per_lumi"; tmp2 = "Optical errors per event per LB";
      sc = rodHistos.regHist(m_OpticalErrors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "SEUErrors_per_lumi"; tmp2 = "SEU errors per event per LB";
      sc = rodHistos.regHist(m_SEU_Errors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "TruncationErrors_per_lumi"; tmp2 = "Truncation errors per event per LB";
      sc = rodHistos.regHist(m_TruncationErrors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "TimeoutErrors_per_lumi"; tmp2 = "Timeout errors per event per LB";
      sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_mod[i] = TProfile_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      tmp = "ErrorBit_per_lumi"; tmp2 = "Errors per event per LB";
      sc = rodHistos.regHist(m_ErrorBit_per_lumi_mod[i] = TProfile2D_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB, 31, 0., 31.));
      m_ErrorBit_per_lumi_mod[i]->SetOption("colz");
      tmp = "Error_per_lumi"; tmp2 = "Errors per event per LB";
      sc = rodHistos.regHist(m_Error_per_lumi_mod[i] = TProfile2D_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt +  axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB, 7, 0., 7.));
      m_Error_per_lumi_mod[i]->SetOption("colz");

      for( int j=0; j<ErrorCategory::COUNT; j++){
         sc = rodHistos.regHist(m_ErrorFraction_per_evt[j][i] = TProfile_LW::create( (tmp3[j] + "_" + modlabel2[i]).c_str(), (tmp3[j] + ", " + modlabel2[i] + m_histTitleExt + ";lumi block;# errors/(module, event)").c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
   }
   

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
   static const int nerrState = 16;
   std::string histErrorName[nerrState][2];
   histErrorName[0][0] = "Mod_Sync_BCID1_errors_per_lumi"; histErrorName[0][1] = "Synchronization Module BCID1 errors per event per LB";
   histErrorName[1][0] = "Mod_Sync_BCID2_errors_per_lumi"; histErrorName[1][1] = "Synchronization Module BCID2 errors per event per LB";
   histErrorName[2][0] = "Mod_Sync_LVL1ID_errors_per_lumi"; histErrorName[2][1] = "Synchronization Module LVL1ID errors per event per LB";
   histErrorName[3][0] = "ROD_Sync_BCID_errors_per_lumi"; histErrorName[3][1] = "Synchronization ROD BCID errors per event per LB";
   histErrorName[4][0] = "ROD_Sync_LVL1ID_errors_per_lumi"; histErrorName[4][1] = "Synchronization ROD LVL1ID errors per event per LB";
   histErrorName[5][0] = "Mod_Trunc_EOC_errors_per_lumi"; histErrorName[5][1] = "Truncation Module EOC errors per event per LB";
   histErrorName[6][0] = "Mod_Trunc_Hit_Overflow_errors_per_lumi"; histErrorName[6][1] = "Truncation Module Hit Overflow errors per event per LB";
   histErrorName[7][0] = "Mod_Trunc_EoE_Overflow_errors_per_lumi"; histErrorName[7][1] = "Truncation Module EoE Overflow errors per event per LB";
   histErrorName[8][0] = "ROD_Trunc_HT_Limit_errors_per_lumi"; histErrorName[8][1] = "Truncation ROD H/T Limit errors per event per LB";
   histErrorName[9][0] = "ROD_Trunc_ROD_OF_errors_per_lumi"; histErrorName[9][1] = "Truncation ROD Overflow errors per event per LB";
   histErrorName[10][0] = "Optical_Errors_per_lumi"; histErrorName[10][1] = "Optical Errors per event per LB";
   histErrorName[11][0] = "SEU_Hit_Parity_per_lumi"; histErrorName[11][1] = "SEU Hit Parity errors per event per LB";
   histErrorName[12][0] = "SEU_Register_Parity_per_lumi"; histErrorName[12][1] = "SEU Register Parity errors per event per LB";
   histErrorName[13][0] = "SEU_Hamming_per_lumi"; histErrorName[13][1] = "SEU Hamming Code errors per event per LB";
   histErrorName[14][0] = "ROD_Timeout_per_lumi"; histErrorName[14][1] = "ROD Formatter Timeout errors per event per LB";
   histErrorName[15][0] = "FE_Warning_per_lumi"; histErrorName[15][1] = "FE Warning errors per event per LB";
   for(int j=0; j<nerrState; j++){
      for (int i=0; i<PixLayer::COUNT-1; i++) {
         sc = rodExpert.regHist(m_errorStates_per_lumi_mod[i][j] = TProfile_LW::create((histErrorName[j][0] + "_" + modlabel2[i]).c_str(), (histErrorName[j][1] + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
      bool errorHist = false;
      m_ErrorStateMap[j] = new PixelMon2DMapsLW( (histErrorName[j][0]+"Map").c_str(),  (histErrorName[j][1] + m_histTitleExt).c_str(), m_doIBL, errorHist);  sc = m_ErrorStateMap[j]->regHist(rodExpert, m_doIBL, errorHist);
   //   m_ErrorStateMap_per_LB[j] = new PixelMon2DLumiProfiles( (histErrorName[j][0] + "_per_LB").c_str(), ( histErrorName[j][1] + m_histTitleExt).c_str(),"# Mod BCID1 Errors",m_doIBL,errorHist);
   //   sc = m_ErrorStateMap_per_LB[j]->regHist(rodExpert,m_doIBL,errorHist);
   }

   for( int i=0; i<PixLayer::COUNT; i++){
      tmp = "Bad_Module_Errors"; tmp2 = "Errors for Bad Module";
      sc = rodExpert.regHist(m_bad_mod_errors_mod[i]  = TH1I_LW::create( (tmp + "_" + modlabel2[i]).c_str(), (tmp2 + ", " + modlabel2[i] + m_histTitleExt + axisTitle_LB).c_str(), nbins_ES, minbin_ES, maxbin_ES));
      tmp = "Errors_EtaID"; tmp2 = "Errors vs Eta Module ID";
      sc = rodExpert.regHist(m_errors_etaid_mod[i] = TH2F_LW::create((tmp + "_" + modlabel2[i]).c_str(),   (tmp2 + ", " + modlabel2[i] + m_histTitleExt + ";#eta module ID;Error State;# Errors").c_str(), nmod_eta[i], min_eta[i], max_eta[i], 32,-0.5,31.5));
      tmp = "Errors_EtaID_per_event"; tmp2 = "Errors vs Eta Module ID/event";
      sc = rodExpert.regHist(m_errors_etaid_per_evt_mod[i] = TH2F_LW::create((tmp + "_" + modlabel2[i]).c_str(),   (tmp2 + ", " + modlabel2[i] + m_histTitleExt + ";#eta module ID;Error State;# Errors").c_str(), nmod_eta[i], min_eta[i], max_eta[i], 32,-0.5,31.5));
   }
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


   const char* errorBitsIBL[32] = {"Row/ Column Error", "Limit Error",          "Timeout Error",       "BCID Error", 
                                   "LVL1ID Error",      "Preamble Error",       "Masked Link",         "Timeout Error",
                                   "BCID counter",      "Hamming code 0",       "Hamming code 1",      "Hamming code 2",
                                   "L1_in counter",     "L1 request counter",   "L1 register",         "L1 Trigger ID",
                                   "readout processor", "Skipped trig counter", "Truncated event flag","Triple Redundant",
                                   "Write reg data",    "Address error",        "Other CMD decoder",   "CMD decoder bit flip",
                                   "CMD decoder SEU",   "Data bus address",     "Triple redundant",    "27",
                                   "28",                "29",                   "30",                  "31"
                                   };

   const char *disk[3] = { "Disk 1", "Disk 2", "Disk 3" };
   const char *mod[13] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
   const char *modIBL[32] = {"C8","","C7","",
				                 "C6","","C5","",
				                 "C4","","C3","",
				                 "C2","","C1","",
				                 "A1","","A2","",
				                 "A3","","A4","",
				                 "A5","","A6","",
				                 "A7","","A8",""};


   for( int i=0; i<PixLayer::COUNT; i++){
      for( unsigned int x=1; x<=m_errors_etaid_mod[i]->GetNbinsX(); x++){
         if(i == PixLayer::kECA || i == PixLayer::kECC){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, disk[x-1]);
            m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, disk[x-1]);
         }
         if(i >= PixLayer::kB0  && i <= PixLayer::kB2){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, mod[x-1]);
            m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, mod[x-1]);
         }
         if(i == PixLayer::kIBL){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, modIBL[x-1]);
            m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, modIBL[x-1]);
         }
      }
      for( unsigned int y=1; y<=m_errors_etaid_mod[i]->GetNbinsY(); y++){
         if(i == PixLayer::kIBL){
            m_errors_etaid_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
            m_errors_etaid_per_evt_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
         }else{
            m_errors_etaid_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
            m_errors_etaid_per_evt_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
         }
      }
   }
   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      for( unsigned int y=1; y<=m_ErrorBit_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++){
         if( i < PixLayerIBL2D3D::kIBL )
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
         else
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
      }
      for( unsigned int y=1; y<=m_Error_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++){
         if( i < PixLayerIBL2D3D::kIBL )
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorType[y-1].c_str());
         else
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorType[y-1].c_str());
      }
   }

   for (int i=0;i<32;i++){
      for( int j=0; j<PixLayer::COUNT; j++){
         if( j<PixLayer::kIBL ){
            m_bad_mod_errors_mod[j]->GetXaxis()->SetBinLabel(i+1,errorBitsPIX[i]);
         }else{
            m_bad_mod_errors_mod[j]->GetXaxis()->SetBinLabel(i+1,errorBitsIBL[i]);
         }
      }
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
      //const char * errorType[7] = {"synch (module)", "synch (ROD)", "trunc/high occ. (module)", "trunc/high occ. (ROD)", "optical transmission", "SEU", "timeout"};
      for(int k=0; k<7; k++) m_errors->SetBinLabel(errorType[k].c_str(),k+1);
   }

   if(m_do2DMaps && !m_doOnline)
   {
      bool errorHist = false;

      for( int i=0; i<ErrorCategoryMODROD::COUNT; i++){
         m_ErrorTypeMap[i] = new PixelMon2DMapsLW(errorType[i].c_str(), (errorType[i] + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorTypeMap[i]->regHist(rodHistos, m_doIBL, false);
      }
      for( int i=0; i<ErrorCategory::COUNT; i++){
         m_ErrorCategoryMap[i] = new PixelMon2DMapsLW(errorCategory[i].c_str(), (errorCategory[i] + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorCategoryMap[i]->regHist(rodHistos, m_doIBL, false);
      }

      //m_SyncErrors_mod = new PixelMon2DMapsLW("SyncErrors_mod", ("Module Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      //sc = m_SyncErrors_mod->regHist(rodHistos, m_doIBL, true);
      //m_SyncErrors_ROD = new PixelMon2DMapsLW("SyncErrors_ROD", ("ROD Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      //sc = m_SyncErrors_ROD->regHist(rodHistos, m_doIBL, true);
      //m_TruncErrors_mod = new PixelMon2DMapsLW("TruncErrors_mod", ("Module Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      //sc = m_TruncErrors_mod->regHist(rodHistos, m_doIBL, true);
      //m_TruncErrors_ROD = new PixelMon2DMapsLW("TruncErrors_ROD", ("ROD Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, true);
      //sc = m_TruncErrors_ROD->regHist(rodHistos, m_doIBL, true);

      //m_SyncErrors = new PixelMon2DMapsLW("Sync_Errors", ("Synchronisation Errors" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_SyncErrors->regHist(rodHistos, m_doIBL, false);
      //m_SyncErrors_per_evt = new PixelMon2DMapsLW("Sync_Errors_per_event", ("Synchronisation Errors/event" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_SyncErrors_per_evt->regHist(rodHistos, m_doIBL, false);
      //m_SyncCategory = new PixelMon2DMapsLW("Sync_Category", ("Synchronisation category" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_SyncCategory->regHist(rodHistos, m_doIBL, false);
      //m_TruncationErrors = new PixelMon2DMapsLW("Truncation_Errors", ("Truncation or High Occupancy Errors" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_TruncationErrors->regHist(rodHistos, m_doIBL, false);
      //m_TruncationErrors_per_evt = new PixelMon2DMapsLW("Truncation_Errors_per_event", ("Truncation or High Occupancy Errors/event" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_TruncationErrors_per_evt->regHist(rodHistos, m_doIBL, false);
      //m_TruncationCategory = new PixelMon2DMapsLW("Truncation_Category", ("Truncation category" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_TruncationCategory->regHist(rodHistos, m_doIBL, false);
      //m_OpticalErrors = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_OpticalErrors->regHist(rodHistos, m_doIBL, false);
      //m_OpticalErrors_per_evt = new PixelMon2DMapsLW("Optical_Errors_per_event", ("Optical Transmision Errors/event" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_OpticalErrors_per_evt->regHist(rodHistos, m_doIBL, false);
      //m_OpticalCategory = new PixelMon2DMapsLW("Optical_Category", ("Optical category" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_OpticalCategory->regHist(rodHistos, m_doIBL, false);
      //m_SEU_Errors = new PixelMon2DMapsLW("SEU_Errors", ("SEU Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_SEU_Errors->regHist(rodHistos, m_doIBL, false);
      //m_SEU_Errors_per_evt = new PixelMon2DMapsLW("SEU_Errors_per_event", ("SEU Errors/event" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_SEU_Errors_per_evt->regHist(rodHistos, m_doIBL, false);
      //m_SEUCategory = new PixelMon2DMapsLW("SEU_Category", ("SEU category" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_SEUCategory->regHist(rodHistos, m_doIBL, false);
      //m_TimeoutErrors = new PixelMon2DMapsLW("Timeout_Errors", ("Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_TimeoutErrors->regHist(rodHistos, m_doIBL, false);
      //m_TimeoutErrors_per_evt = new PixelMon2DMapsLW("Timeout_Errors_per_event", ("Timeout Errors/event" + m_histTitleExt).c_str(), m_doIBL, false);
      //sc = m_TimeoutErrors_per_evt->regHist(rodHistos, m_doIBL, false);
      //m_TimeoutCategory = new PixelMon2DMapsLW("Timeout_Category", ("Timeout category" + m_histTitleExt).c_str(), m_doIBL, false);  //to be removed ?
      //sc = m_TimeoutCategory->regHist(rodHistos, m_doIBL, false);

      //tmp = "SyncErrors_mod"; tmp2 = "Module Synchronization Errors"; errorHist = true;
      //m_sync_mod_BCID1  = new PixelMon2DMapsLW( (tmp + "_BCID1").c_str(),  (tmp2 + ", BCID1" + m_histTitleExt).c_str(), m_doIBL, errorHist);  sc = m_sync_mod_BCID1->regHist(rodExpert, m_doIBL, errorHist);
      //m_sync_mod_BCID2  = new PixelMon2DMapsLW( (tmp + "_BCID2").c_str(),  (tmp2 + ", BCID2" + m_histTitleExt).c_str(), m_doIBL, errorHist);  sc = m_sync_mod_BCID2->regHist(rodExpert, m_doIBL, errorHist);
      //m_sync_mod_LVL1ID = new PixelMon2DMapsLW( (tmp + "_LVL1ID").c_str(), (tmp2 + ", LVL1ID" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_sync_mod_LVL1ID->regHist(rodExpert, m_doIBL, errorHist);
      //tmp = "SyncErrors_ROD"; tmp2 = "ROD Synchronization Errors"; errorHist = true;
      //m_sync_rod_BCID   = new PixelMon2DMapsLW( (tmp + "_BCID").c_str(),   (tmp2 + ", BCID" + m_histTitleExt).c_str(), m_doIBL, errorHist);   sc = m_sync_rod_BCID->regHist(rodExpert, m_doIBL, errorHist);
      //m_sync_rod_LVL1ID = new PixelMon2DMapsLW( (tmp + "_LVL1ID").c_str(), (tmp2 + ", LVL1ID" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_sync_rod_LVL1ID->regHist(rodExpert, m_doIBL, errorHist);
      //tmp = "TruncErrors_mod"; tmp2 = "Module Truncation Errors"; errorHist = true;
      //m_trunc_mod_EOC   = new PixelMon2DMapsLW( (tmp + "_EoC").c_str(),   (tmp2 + ", EoC" + m_histTitleExt).c_str(), m_doIBL, errorHist);          sc = m_trunc_mod_EOC->regHist(rodExpert, m_doIBL, errorHist);
      //m_trunc_mod_hitOF = new PixelMon2DMapsLW( (tmp + "_hitOF").c_str(), (tmp2 + ", Hit Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_mod_hitOF->regHist(rodExpert, m_doIBL, errorHist);
      //m_trunc_mod_EoEOF = new PixelMon2DMapsLW( (tmp + "_EoEOF").c_str(), (tmp2 + ", EoE Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_mod_EoEOF->regHist(rodExpert, m_doIBL, errorHist);
      //tmp = "TruncErrors_ROD"; tmp2 = "ROD Truncation Errors"; errorHist = true;
      //m_trunc_rod_HTlim  = new PixelMon2DMapsLW( (tmp + "_HTlim").c_str(),  (tmp2 + ", H/T Limit" + m_histTitleExt).c_str(), m_doIBL, errorHist);     sc = m_trunc_rod_HTlim->regHist(rodExpert, m_doIBL, errorHist);
      //m_trunc_rod_FIFOOF = new PixelMon2DMapsLW( (tmp + "_FIFOOF").c_str(), (tmp2 + ", FIFO Overflow" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_trunc_rod_FIFOOF->regHist(rodExpert, m_doIBL, errorHist);
      //tmp = "SEUErrors"; tmp2 = "SEU Errors"; errorHist = true;
      //m_seu_hit_parity = new PixelMon2DMapsLW( (tmp + "_Hit_Parity").c_str(), (tmp2 + ", Hit Parity" + m_histTitleExt).c_str(), m_doIBL, errorHist);      sc = m_seu_hit_parity->regHist(rodExpert, m_doIBL, errorHist);
      //m_seu_reg_parity = new PixelMon2DMapsLW( (tmp + "_Reg_Parity").c_str(), (tmp2 + ", Register Parity" + m_histTitleExt).c_str(), m_doIBL, errorHist); sc = m_seu_reg_parity->regHist(rodExpert, m_doIBL, errorHist);
      //m_seu_hamming    = new PixelMon2DMapsLW( (tmp + "_Hamming").c_str(),    (tmp2 + ", Hamming Code" + m_histTitleExt).c_str(), m_doIBL, errorHist);    sc = m_seu_hamming->regHist(rodExpert, m_doIBL, errorHist);

      //m_timeout = new PixelMon2DMapsLW("ROD_Timeout", ("ROD Formatter Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_timeout->regHist(rodExpert, m_doIBL, true);
      //m_FEwarning = new PixelMon2DMapsLW("FE_Warning", ("FE Warning Timeout Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_FEwarning->regHist(rodExpert, m_doIBL, true);
      //m_optical_error = new PixelMon2DMapsLW("Optical_Errors", ("Optical Transmision Errors" + m_histTitleExt).c_str(), m_doIBL, true); sc = m_optical_error->regHist(rodExpert, m_doIBL, true);

      if(m_doModules){
         /// iranai
         //tmp = "SyncErrors_mod"; tmp2 = "Avg Module Synchronization Errors"; errorHist = true;
	      //m_sync_mod_BCID1_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID1_per_LB").c_str(), ( tmp2 + ", BCID1" + m_histTitleExt).c_str(),"# Mod BCID1 Errors/Event",m_doIBL,errorHist);
	      //sc = m_sync_mod_BCID1_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_sync_mod_BCID2_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID2_per_LB").c_str(), ( tmp2 + ", BCID2" + m_histTitleExt).c_str(),"# Mod BCID2 Errors/Event",m_doIBL,errorHist);
	      //sc = m_sync_mod_BCID2_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_sync_mod_LVL1ID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_LVL1ID_per_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# Mod LVL1ID Errors/Event",m_doIBL,errorHist);
	      //sc = m_sync_mod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_sync_mod_corr_LVL1ID_BCID2_per_LB = new PixelMon2DLumiProfiles( (tmp + "_corr_LVL1ID_BCID2_per_LB").c_str(), ( tmp2 + " Correlations, LVL1ID and BCID2" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID2 Errors/Events with either LVL1ID or BCID2 errors",m_doIBL,errorHist);
	      //sc = m_sync_mod_corr_LVL1ID_BCID2_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         //tmp = "SyncErrors_rod"; tmp2 = "Avg ROD Synchronization Errors"; errorHist = true;
	      //m_sync_rod_BCID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_BCID_per_LB").c_str(), ( tmp2 + ", BCID" + m_histTitleExt).c_str(),"# ROD BCID Errors/Event",m_doIBL,errorHist);
	      //sc = m_sync_rod_BCID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_sync_rod_LVL1ID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_LVL1ID_per_LB").c_str(), ( tmp2 + ", LVL1ID" + m_histTitleExt).c_str(),"# ROD LVL1ID Errors/Event",m_doIBL,errorHist);
	      //sc = m_sync_rod_LVL1ID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_sync_rod_corr_LVL1ID_BCID_per_LB = new PixelMon2DLumiProfiles( (tmp + "_corr_LVL1ID_BCID_per_LB").c_str(), ( tmp2 + " Correlations, LVL1ID and BCID" + m_histTitleExt).c_str(),"Events with LVL1ID and BCID Errors/Events with either LVL1ID or BCID errors",m_doIBL,errorHist);
	      //sc = m_sync_rod_corr_LVL1ID_BCID_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         //tmp = "TruncErrors_mod"; tmp2 = "Avg Module Truncation Errors"; errorHist = true;
	      //m_trunc_mod_EOC_per_LB = new PixelMon2DLumiProfiles( (tmp + "_EOC_per_LB").c_str(), ( tmp2 + ", EoC" + m_histTitleExt).c_str(),"# Mod EoC Errors/Event",m_doIBL,errorHist);
	      //sc = m_trunc_mod_EOC_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_trunc_mod_hitOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_hitOF_per_LB").c_str(), ( tmp2 + ", Hit Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      //sc = m_trunc_mod_hitOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_trunc_mod_EoEOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_EoEOF_per_LB").c_str(), ( tmp2 + ", EoE Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      //sc = m_trunc_mod_EoEOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         //tmp = "TruncErrors_rod"; tmp2 = "Avg ROD Truncation Errors"; errorHist = true;
	      //m_trunc_rod_HTlim_per_LB = new PixelMon2DLumiProfiles( (tmp + "_HTlim_per_LB").c_str(), ( tmp2 + ", H/T Limit" + m_histTitleExt).c_str(),"# H/T Errors/Event",m_doIBL,errorHist);
	      //sc = m_trunc_rod_HTlim_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_trunc_rod_FIFOOF_per_LB = new PixelMon2DLumiProfiles( (tmp + "_FIFOOF_per_LB").c_str(), ( tmp2 + ", FIFO Overflow" + m_histTitleExt).c_str(),"# Mod OF Errors/Event",m_doIBL,errorHist);
	      //sc = m_trunc_rod_FIFOOF_per_LB->regHist(rodExpert,m_doIBL,errorHist);
         //tmp = "SEUErrors"; tmp2 = "Avg SEU Errors"; errorHist = true;
	      //m_seu_hit_parity_per_LB = new PixelMon2DLumiProfiles( (tmp + "_Hit_Parity_per_LB").c_str(), ( tmp2 + ", Hit Parity" + m_histTitleExt).c_str(),"# Hit Parity Errors/Event",m_doIBL,errorHist);
	      //sc = m_seu_hit_parity_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_seu_reg_parity_per_LB = new PixelMon2DLumiProfiles( (tmp + "_Reg_Parity_per_LB").c_str(), ( tmp2 + ", Register Parity" + m_histTitleExt).c_str(),"# Reg Parity Errors/Event",m_doIBL,errorHist);
	      //sc = m_seu_reg_parity_per_LB->regHist(rodExpert,m_doIBL,errorHist);
	      //m_seu_hamming_per_LB    = new PixelMon2DLumiProfiles( (tmp + "_Hamming_per_LB").c_str(), ( tmp2 + ", Hamming Code" + m_histTitleExt).c_str(),"# HC Errors/Event",m_doIBL,errorHist);
	      //sc = m_seu_hamming_per_LB->regHist(rodExpert,m_doIBL,errorHist);

	      //m_optical_error_per_LB = new PixelMon2DLumiProfiles("Optical_Errors_per_LB", ("Avg Optical Errors" + m_histTitleExt).c_str(),"# Optical Errors/Event",m_doIBL,true);
	      //sc = m_optical_error_per_LB->regHist(rodExpert,m_doIBL,true);
	      //m_timeout_per_LB = new PixelMon2DLumiProfiles("ROD_Timeout_per_LB", ("Avg ROD Formatter Timeout Errors" + m_histTitleExt).c_str(),"# Timeout Errors/Event",m_doIBL,true);
	      //sc = m_timeout_per_LB->regHist(rodExpert,m_doIBL,true);
	      //m_FEwarning_per_LB = new PixelMon2DLumiProfiles("FE_Warning_per_LB", ("Avg FE Warning Errors" + m_histTitleExt).c_str(),"# FE Warning Errors/Event",m_doIBL,true);
	      //sc = m_FEwarning_per_LB->regHist(rodExpert,m_doIBL,true);
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
   PixelID::const_id_iterator idIt     = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd  = m_pixelid->wafer_end();

   const float nmodLayer[PixLayerIBL2D3D::COUNT] = {144., 144., 286., 494., 676., 280., 168., 112};

   static const int nErrorBits = 32;

   uint64_t errors=0;
   //int nErrors_mod[PixLayer::COUNT]={0};
   int nErrors_mod[PixLayerIBL2D3D::COUNT]={0};
   //int nErrors_PIX=0;

   int nErrorBit_mod[PixLayerIBL2D3D::COUNT][nErrorBits] = {0};
   int nErrors_cat_mod[PixLayer::COUNT-1][16] = {0};
   
   // iranai
   //int nOtherRODErrors_IBL=0;
   //int nChipErrors_IBL=0;

   int nErrorsCategory_permod[ErrorCategory::COUNT][PixLayerIBL2D3D::COUNT] = {0};
   //int nErrorsCategory_permod[ErrorCategory::COUNT][PixLayer::COUNT] = {0};

   int nErrorTypes_mod[PixLayerIBL2D3D::COUNT][ErrorCategoryMODROD::COUNT];// = {0};

   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      for( int j=0; j<ErrorCategoryMODROD::COUNT; j++){
         nErrorTypes_mod[i][j] = 0;
      }
   }

   //int nSyncErrors_mod[PixLayer::COUNT] = {0};
   int nSyncErrors_mod[PixLayerIBL2D3D::COUNT] = {0};
   
   //int nOpticalErrors_mod[PixLayer::COUNT] = {0};
   int nOpticalErrors_mod[PixLayerIBL2D3D::COUNT] = {0};
   
   //int nSEUErrors_mod[PixLayer::COUNT] = {0};
   int nSEUErrors_mod[PixLayerIBL2D3D::COUNT] = {0};
   
   //int nTruncErrors_mod[PixLayer::COUNT] = {0};
   int nTruncErrors_mod[PixLayerIBL2D3D::COUNT] = {0};
   
   //int nTimeoutErrors_mod[PixLayer::COUNT] = {0};
   int nTimeoutErrors_mod[PixLayerIBL2D3D::COUNT] = {0};

   ///// Pixel ID Loop
   for (; idIt != idItEnd; ++idIt) 
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID); 
      errors = m_ErrorSvc->getModuleErrors(id_hash);

      bool isIBL=false;
      //bool isError[16] = {false};
      bool hasError[ErrorCategory::COUNT] = {false};

      if (m_ErrorSvc->isActive(id_hash) && m_pixelid->barrel_ec(WaferID)==0 && m_pixelid->layer_disk(WaferID)==0 && m_doIBL){isIBL=true;}
      
      int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
      int pixlayeribl2d3d = 0;
      if( pixlayer == PixLayerIBL2D3D::kIBL){
         pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_pixelid->eta_module(WaferID), m_doIBL);
      }

      if(pixlayer == 99) continue;

      //for (unsigned int kBit=0; kBit<40; kBit++)
      for (unsigned int kBit=0; kBit<nErrorBits; kBit++)
      {
         //if( (errors & (1<<kBit)) !=0 )
         if( (errors & (static_cast<uint64_t>(1)<<kBit)) !=0 )
         {
            m_errors_etaid_mod[pixlayer]->Fill( GetEtaID(WaferID, m_pixelid, m_doIBL, false), kBit);
	         TH2FSetBinScaled(m_errors_etaid_per_evt_mod[pixlayer], m_errors_etaid_mod[pixlayer], m_event);


  	         if (m_ErrorSvc->isActive(id_hash)&&!m_ErrorSvc->isGood(id_hash)){
               m_bad_mod_errors_mod[pixlayer]->Fill(kBit);
            }

            nErrors_mod[pixlayer]++;
            nErrorBit_mod[pixlayer][kBit]++;
            if(pixlayeribl2d3d != 0){
               nErrors_mod[pixlayeribl2d3d]++;
               nErrorBit_mod[pixlayeribl2d3d][kBit]++;
            }

            int ErrorType = 0;
            int kErrorCategory = 0;
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
	         }else{//CCC CCCC CCCC CCCC CCCC EPpl bzhv  
	            if(kBit==3||kBit==4||kBit==8) ErrorType = 1;  // synchronization error   (3:LVL1ID, 4:BCID, 8:BCID counter error)
	            if(kBit==0||kBit==18)         ErrorType = 3;  // module truncation error (0:Row/Column error, 18:Truncated event)
	            if(kBit==1)                   ErrorType = 4;  // ROD truncation error    (1:Limit error)
	            if(kBit==5)                   ErrorType = 5;  // optical error           (5:Preable error)
	            if(kBit==9||kBit==10||kBit==11||kBit==19||kBit==23||kBit==24||kBit==26) ErrorType = 6;  // SEU   (9,10,11: hammingcode, 19:Triple redundant, 23:Bit flip, 24:SEU, 26:Triple redundant)
	            if(kBit==2||kBit==7)          ErrorType = 7;  // Timeout error           (2:Trailer timeout error, 7:readout timeout
	         }

            if(ErrorType)//if there were any errors we care about
            {
               if(ErrorType == 1 || ErrorType == 2) kErrorCategory = ErrorCategory::kSync;
               if(ErrorType == 3 || ErrorType == 4) kErrorCategory = ErrorCategory::kTrunc;
               if(ErrorType == 5) kErrorCategory = ErrorCategory::kOpt;
               if(ErrorType == 6) kErrorCategory = ErrorCategory::kSeu;
               if(ErrorType == 7) kErrorCategory = ErrorCategory::kTout;

               //if(m_errorTypes) m_errorTypes->Fill(ErrorType);
               if(m_errors) m_errors->Fill(ErrorType, WaferID, m_pixelid, m_doIBL);

               //fill all the 2d maps for each error type
               if(m_ErrorTypeMap[ErrorType-1]) m_ErrorTypeMap[ErrorType-1]->Fill(WaferID,m_pixelid,m_doIBL, false);

	            if(m_doLumiBlock){
		            if(m_errors_LB) m_errors_LB->Fill(WaferID,m_pixelid,m_doIBL, true);
	            }
	       
	            /// Count the each error
               nErrorTypes_mod[pixlayer][ErrorType-1]++;
               if(pixlayeribl2d3d != 0) nErrorTypes_mod[pixlayeribl2d3d][ErrorType-1]++;
               if( !hasError[kErrorCategory] ){
                  m_ErrorCategoryMap[ErrorType-1]->Fill(WaferID,m_pixelid,m_doIBL, false);
                  nErrorsCategory_permod[kErrorCategory][pixlayer]++;
                  if(pixlayeribl2d3d != 0) nErrorsCategory_permod[kErrorCategory][pixlayeribl2d3d]++;
                  hasError[kErrorCategory] = true;
               }
               
               /// iranai--------
               //if (ErrorType==1||ErrorType==2){ 
               //   nSyncErrors_mod[pixlayer]++;
               //   if(pixlayeribl2d3d != 0) nSyncErrors_mod[pixlayeribl2d3d]++;
               //   if( !hasError[ErrorCategory::kSync] ){
               //      m_SyncCategory->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               //      nErrorsCategory_permod[ErrorCategory::kSync][pixlayer]++;
               //      if(pixlayeribl2d3d != 0) nErrorsCategory_permod[ErrorCategory::kSync][pixlayeribl2d3d]++;
               //      hasError[ErrorCategory::kSync] = true;
               //   }
	            //}
	            //if (ErrorType==3||ErrorType==4){
               //   if(pixlayeribl2d3d != 0) nTruncErrors_mod[pixlayeribl2d3d]++;
               //   nTruncErrors_mod[pixlayer]++;
               //   if( !hasError[ErrorCategory::kTrunc] ){
               //      m_TruncationCategory->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               //      nErrorsCategory_permod[ErrorCategory::kTrunc][pixlayer]++;
               //      if(pixlayeribl2d3d != 0) nErrorsCategory_permod[ErrorCategory::kTrunc][pixlayeribl2d3d]++;
               //      hasError[ErrorCategory::kTrunc] = true;
               //   }
	            //}
	            //if (ErrorType==5){
               //   if(pixlayeribl2d3d != 0) nOpticalErrors_mod[pixlayeribl2d3d]++;
               //   nOpticalErrors_mod[pixlayer]++;
               //   if( !hasError[ErrorCategory::kOpt] ){
               //      m_OpticalCategory->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               //      nErrorsCategory_permod[ErrorCategory::kOpt][pixlayer]++;
               //      if(pixlayeribl2d3d != 0) nErrorsCategory_permod[ErrorCategory::kOpt][pixlayeribl2d3d]++;
               //      hasError[ErrorCategory::kOpt] = true;
               //   }
	            //}
	            //if (ErrorType==6){
               //   if(pixlayeribl2d3d != 0) nSEUErrors_mod[pixlayeribl2d3d]++;
               //   nSEUErrors_mod[pixlayer]++;
               //   if( !hasError[ErrorCategory::kSeu] ){
               //      m_SEUCategory->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               //      nErrorsCategory_permod[ErrorCategory::kSeu][pixlayer]++;
               //      if(pixlayeribl2d3d != 0) nErrorsCategory_permod[ErrorCategory::kSeu][pixlayeribl2d3d]++;
               //      hasError[ErrorCategory::kSeu] = true;
               //   }
               //}
	            //if (ErrorType==7){
               //   if(pixlayeribl2d3d != 0) nTimeoutErrors_mod[pixlayeribl2d3d]++;
               //   nTimeoutErrors_mod[pixlayer]++;
               //   if( !hasError[ErrorCategory::kTout] ){
               //      m_TimeoutCategory->Fill(WaferID,m_pixelid,m_doIBL, false);  //to be removed ?
               //      nErrorsCategory_permod[ErrorCategory::kTout][pixlayer]++;
               //      if(pixlayeribl2d3d != 0) nErrorsCategory_permod[ErrorCategory::kTout][pixlayeribl2d3d]++;
               //      hasError[ErrorCategory::kTout] = true;
               //   }
	            //}
            } // End of if(ErrorType)
           
            if( getErrorCategory(kBit, isIBL) != 99 ){
               nErrors_cat_mod[pixlayer][getErrorCategory(kBit, isIBL)]++;
               m_ErrorStateMap[getErrorCategory(kBit, isIBL)]->Fill(WaferID, m_pixelid, m_doIBL, true);
               //m_ErrorStateMap_per_LB[getErrorCategory(kBit, isIBL)]->Fill(LBnum, WaferID, m_pixelid, 1, m_doIBL, true);
            }


            //iranai ----------
            //if(kBit==14){ // module synchronization error other than IBL
	         //   if(m_sync_mod_BCID1) m_sync_mod_BCID1->Fill(WaferID,m_pixelid,m_doIBL, true);
            //   if(m_sync_mod_BCID1_int_LB) m_sync_mod_BCID1_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==15){ // module synchronization error other than IBL
	         //   if(m_sync_mod_BCID2) m_sync_mod_BCID2->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_sync_mod_BCID2_int_LB) m_sync_mod_BCID2_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==16){ // module synchronization error (LVL1ID) other than IBL
	         //   if(m_sync_mod_LVL1ID) m_sync_mod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_sync_mod_LVL1ID_int_LB) m_sync_mod_LVL1ID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==20){ // ROD synchronization error (BCID) other than IBL
	         //   if(m_sync_rod_BCID) m_sync_rod_BCID->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_sync_rod_BCID_int_LB) m_sync_rod_BCID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==21){ // ROD synchronization error (LVL1ID) other than IBL
	         //   if(m_sync_rod_LVL1ID) m_sync_rod_LVL1ID->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_sync_rod_LVL1ID_int_LB) m_sync_rod_LVL1ID_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==4){ // Module truncation errors (EOC)
	         //   if(m_trunc_mod_EOC) m_trunc_mod_EOC->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_trunc_mod_EOC_int_LB) m_trunc_mod_EOC_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==12){ // Module truncation errors (hit overflow)
	         //   if(m_trunc_mod_hitOF) m_trunc_mod_hitOF->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_trunc_mod_hitOF_int_LB) m_trunc_mod_hitOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==13){ // Module truncation errors (EoE overflow)
	         //   if(m_trunc_mod_EoEOF) m_trunc_mod_EoEOF->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_trunc_mod_EoEOF_int_LB) m_trunc_mod_EoEOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==1){ // ROD truncation errors (H/T Limit)
	         //   if(m_trunc_rod_HTlim) m_trunc_rod_HTlim->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_trunc_rod_HTlim_int_LB) m_trunc_rod_HTlim_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==0){ // ROD truncation errors (FIFO Overflow)
	         //   if(m_trunc_rod_FIFOOF) m_trunc_rod_FIFOOF->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_trunc_rod_FIFOOF_int_LB) m_trunc_rod_FIFOOF_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==23){ // optical errors (preable)
	         //   if(m_optical_error) m_optical_error->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==5){
	         //   if(m_seu_hit_parity) m_seu_hit_parity->Fill(WaferID,m_pixelid,m_doIBL,true);
	         //   if(m_optical_error_int_LB) m_optical_error_int_LB->Fill(LBnum,WaferID,m_pixelid,m_doIBL,true);
            //}
            //if(kBit==6){
	         //   if(m_seu_reg_parity) m_seu_reg_parity->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_seu_reg_parity_int_LB) m_seu_reg_parity_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==7){
	         //   if(m_seu_hamming) m_seu_hamming->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_seu_hamming_int_LB) m_seu_hamming_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
            //if(kBit==22){
	         //   if(m_timeout) m_timeout->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_timeout_int_LB) m_timeout_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}            
            //if(kBit==8){
	         //   if(m_FEwarning) m_FEwarning->Fill(WaferID,m_pixelid,m_doIBL,true);
            //   if(m_FEwarning_int_LB) m_FEwarning_int_LB->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            //}
         }//end bit shifting
      }//end for loop over bits

      ///--- iranai
      //if(m_sync_mod_BCID1_per_LB){
      //   if (isError[0]) m_sync_mod_BCID1_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_sync_mod_BCID1_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_mod_BCID2_per_LB){
      //   if (isError[1]) m_sync_mod_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_sync_mod_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_mod_LVL1ID_per_LB){
      //   if (isError[2]) m_sync_mod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_sync_mod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_mod_corr_LVL1ID_BCID2_per_LB){
      //   if (isError[2]&&isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else if (isError[2]||isError[1]) m_sync_mod_corr_LVL1ID_BCID2_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_rod_BCID_per_LB){
      //   if (isError[3]) m_sync_rod_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_sync_rod_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_rod_LVL1ID_per_LB){
      //   if (isError[4]) m_sync_rod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_sync_rod_LVL1ID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_sync_rod_corr_LVL1ID_BCID_per_LB){
      //   if (isError[3]&&isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else if (isError[3]||isError[4]) m_sync_rod_corr_LVL1ID_BCID_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_trunc_mod_EOC_per_LB){
      //   if (isError[5]) m_trunc_mod_EOC_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_trunc_mod_EOC_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_trunc_mod_hitOF_per_LB){
      //   if (isError[6]) m_trunc_mod_hitOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_trunc_mod_hitOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_trunc_mod_EoEOF_per_LB){
      //   if (isError[7]) m_trunc_mod_EoEOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_trunc_mod_EoEOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_trunc_rod_HTlim_per_LB){
      //   if (isError[8]) m_trunc_rod_HTlim_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_trunc_rod_HTlim_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_trunc_rod_FIFOOF_per_LB){
      //   if (isError[9]) m_trunc_rod_FIFOOF_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_trunc_rod_FIFOOF_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_optical_error_per_LB){
      //   if (isError[10]) m_optical_error_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_optical_error_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_seu_hit_parity_per_LB){
      //   if (isError[11]) m_seu_hit_parity_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_seu_hit_parity_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_seu_reg_parity_per_LB){
      //   if (isError[12]) m_seu_reg_parity_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_seu_reg_parity_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_seu_hamming_per_LB){
      //   if (isError[13]) m_seu_hamming_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_seu_hamming_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_timeout_per_LB){
      //   if (isError[14]) m_timeout_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_timeout_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
      //if(m_FEwarning_per_LB){
      //   if (isError[15]) m_FEwarning_per_LB->Fill(LBnum,WaferID,m_pixelid);
      //   else m_FEwarning_per_LB->Fill(LBnum,WaferID,m_pixelid,0);           
      //}   
   } //end loop over all identifiers
 /// 
   double errorSum = 0;
   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++) errorSum += nErrors_mod[i];
   if(m_error_time1&&m_error_time2&&m_error_time3)FillTimeHisto(errorSum,  m_error_time1, m_error_time2, m_error_time3,10.,60.,360.);

   for( int i=0; i<PixLayer::COUNT-1; i++){
      for( int j=0; j<16; j++) {
         if( m_errorStates_per_lumi_mod[i][j] ) m_errorStates_per_lumi_mod[i][j]->Fill(LBnum, nErrors_cat_mod[i][j]);
      }
   }

   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      for( int j=0; j<32; j++){
         if(m_ErrorBit_per_lumi_mod[i]) m_ErrorBit_per_lumi_mod[i]->Fill(LBnum, j, nErrorBit_mod[i][j]);
      }
      for( int j=0; j<ErrorCategoryMODROD::COUNT; j++){
         //if(m_Error_per_lumi_mod[i]) m_Error_per_lumi_mod[i]->Fill(LBnum, j, nErrorTypes_mod[i][j]);
         m_Error_per_lumi_mod[i]->Fill(LBnum, j, nErrorTypes_mod[i][j]);
      }
   }

   
   //for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
   for(int i=0; i<PixLayerIBL2D3D::COUNT; i++){

      if(m_errors_per_lumi_mod[i]) m_errors_per_lumi_mod[i]->Fill(LBnum, nErrors_mod[i]);
      if(m_SyncErrors_per_lumi_mod[i]) m_SyncErrors_per_lumi_mod[i]->Fill(LBnum, nSyncErrors_mod[i]);
      if(m_OpticalErrors_per_lumi_mod[i]) m_OpticalErrors_per_lumi_mod[i]->Fill(LBnum, nOpticalErrors_mod[i]);
      if(m_SEU_Errors_per_lumi_mod[i]) m_SEU_Errors_per_lumi_mod[i]->Fill(LBnum, nSEUErrors_mod[i]);
      if(m_TimeoutErrors_per_lumi_mod[i]) m_TimeoutErrors_per_lumi_mod[i]->Fill(LBnum, nTimeoutErrors_mod[i]);
      if(m_TruncationErrors_per_lumi_mod[i]) m_TruncationErrors_per_lumi_mod[i]->Fill(LBnum, nTruncErrors_mod[i]);

      for( int j=0; j<ErrorCategory::COUNT; j++){
         if(m_ErrorFraction_per_evt[j][i]) m_ErrorFraction_per_evt[j][i]->Fill(LBnum, nErrorsCategory_permod[j][i]/nmodLayer[i]);
      }
   }


   return StatusCode::SUCCESS;
}

int PixelMainMon::getErrorCategory(int bit, bool isibl)
{
   int erstate = 99;
   if(!isibl){
      switch( bit ){
         case 14:
            erstate = 0; break;
         case 15:
            erstate = 1; break;
         case 16:
            erstate = 2; break;
         case 20:
            erstate = 3; break;
         case 21:
            erstate = 4; break;
         case 4:
            erstate = 5; break;
         case 12:
            erstate = 6; break;
         case 13:
            erstate = 7; break;
         case 1:
            erstate = 8; break;
         case 0:
            erstate = 9; break;
         case 23:
            erstate = 10; break;
         case 5:
            erstate = 11; break;
         case 6:
            erstate = 12; break;
         case 7:
            erstate = 13; break;
         case 22:
            erstate = 14; break;
         case 8:
            erstate = 15; break;
         default:
            erstate = 99; break;
      }
   }else{
   }
   return erstate;
}

