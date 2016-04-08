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
   static const int nerror = ErrorCategory::COUNT;
   static const int nerror_modrod = ErrorCategoryMODROD::COUNT;
   const std::string errorType[nerror_modrod] = {"SyncErrors_Mod", "SyncErrors_ROD", "TruncErrors_Mod", "TruncErrors_ROD", "OpticalErrors_RODMod", "SEUErrors_RODMod", "TimeoutErrors_RODMod"};
   const std::string errorType2[nerror_modrod] = {"SyncErrors_Mod", "SyncErrors_ROD", "TruncErrors_Mod", "TruncErrors_ROD", "Preamble/headerErrors", "SEUErrors", "TimeoutErrors"};
   const std::string errorCategory[nerror] = {"SyncErrors", "TruncErrors", "OpticalErrors", "SEUErrors", "TimeoutErrors"};
   const std::string errorCategory2[nerror] = {"SyncErrors", "TruncErrors", "Preamble/headerErrors", "SEUErrors", "TimeoutErrors"};
   
   /// module label
   static const int nlayers = PixLayer::COUNT;
   static const int nlayersIBL2d3d = PixLayerIBL2D3D::COUNT;
   const std::string modlabel2[nlayersIBL2d3d] = {"ECA", "ECC", "B0", "B1", "B2", "IBL", "IBL2D", "IBL3D"};

   unsigned int  nmod_eta[nlayers] = {3, 3, 13, 13, 13, 32};
   float min_eta[nlayers] = {-0.5, -0.5, -6.5, -6.5, -6.5, -16.5};
   float max_eta[nlayers] = {2.5, 3.5, 6.5, 6.5, 6.5, 15.5};

   /// Set the number of bins, max/min bin
   std::string atext_LB = ";lumi block"; 
   std::string atext_err = ";# errors/event"; 
   std::string atext_erf = ";# errors/module/event"; 
   std::string atext_erb = ";Error bit"; 
   std::string atext_ers = ";Error state"; 
   std::string axisTitle_LB = ";lumi block;# errors/event";
   std::string axisTitle_ES = ";Error State;# Errors";
   int nbins_LB = 2500; float minbin_LB = -0.5; float maxbin_LB = minbin_LB + (1.0*nbins_LB);
   int nbins_ES = 32;   float minbin_ES = -0.5; float maxbin_ES = minbin_ES + (1.0*nbins_ES);
   std::string tmp;
   std::string tmp2;
   std::string tmp3[nerror] = {"SyncErrorsFrac_per_event", "TruncationErrorsFrac_per_event", "OpticalErrorsFrac_per_event", "SEUErrorsFrac_per_event", "TimeoutErrorsFrac_per_event"};
   std::string hname;
   std::string htitles;


   for( int i=0; i<nlayersIBL2d3d; i++){
      hname = makeHistname(("errors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average number of errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_errors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SyncErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Synchronization errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SyncErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("OpticalErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Preamble/header errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_OpticalErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SEUErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average SEU errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SEU_Errors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TruncationErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Truncation errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TruncationErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TimeoutErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Timeout errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("ErrorBit_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Errors per module per event, "+modlabel2[i]), (atext_LB+atext_erb+atext_err), false);
      sc = rodHistos.regHist(m_ErrorBit_per_lumi_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 31, 0., 31.));
      m_ErrorBit_per_lumi_mod[i]->SetOption("colz");

      hname = makeHistname(("Error_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Errors per module per event, "+modlabel2[i]), (atext_LB+atext_ers+atext_err), false);
      sc = rodHistos.regHist(m_Error_per_lumi_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 7, 0., 7.));
      m_Error_per_lumi_mod[i]->SetOption("colz");

      for( int j=0; j<nerror; j++){
         hname = makeHistname((tmp3[j]+"_"+modlabel2[i]), false);
         htitles = makeHisttitle((tmp3[j]+", "+modlabel2[i]), (atext_LB+atext_erf), false);
         sc = rodHistos.regHist(m_ErrorFraction_per_evt[j][i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
   }
   

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
   static const int nerrState = 16;
   std::string histErrorName[nerrState][2];
   histErrorName[0][0] = "Mod_Sync_BCID1_errors";         histErrorName[0][1] = "Synchronization Module BCID1 errors per event per LB";
   histErrorName[1][0] = "Mod_Sync_BCID2_errors";         histErrorName[1][1] = "Synchronization Module BCID2 errors per event per LB";
   histErrorName[2][0] = "Mod_Sync_LVL1ID_errors";        histErrorName[2][1] = "Synchronization Module LVL1ID errors per event per LB";
   histErrorName[3][0] = "ROD_Sync_BCID_errors";          histErrorName[3][1] = "Synchronization ROD BCID errors per event per LB";
   histErrorName[4][0] = "ROD_Sync_LVL1ID_errors";        histErrorName[4][1] = "Synchronization ROD LVL1ID errors per event per LB";
   histErrorName[5][0] = "Mod_Trunc_EOC_errors";          histErrorName[5][1] = "Truncation Module EOC errors per event per LB";
   histErrorName[6][0] = "Mod_Trunc_Hit_Overflow_errors"; histErrorName[6][1] = "Truncation Module Hit Overflow errors per event per LB";
   histErrorName[7][0] = "Mod_Trunc_EoE_Overflow_errors"; histErrorName[7][1] = "Truncation Module EoE Overflow errors per event per LB";
   histErrorName[8][0] = "ROD_Trunc_HT_Limit_errors";     histErrorName[8][1] = "Truncation ROD H/T Limit errors per event per LB";
   histErrorName[9][0] = "ROD_Trunc_ROD_OF_errors";       histErrorName[9][1] = "Truncation ROD Overflow errors per event per LB";
   histErrorName[10][0] = "Optical_Errors";               histErrorName[10][1]= "Preamble/header Errors per event per LB";
   histErrorName[11][0] = "SEU_Hit_Parity";               histErrorName[11][1]= "SEU Hit Parity errors per event per LB";
   histErrorName[12][0] = "SEU_Register_Parity";          histErrorName[12][1]= "SEU Register Parity errors per event per LB";
   histErrorName[13][0] = "SEU_Hamming";                  histErrorName[13][1]= "SEU Hamming Code errors per event per LB";
   histErrorName[14][0] = "ROD_Timeout";                  histErrorName[14][1]= "ROD Formatter Timeout errors per event per LB";
   histErrorName[15][0] = "FE_Warning";                   histErrorName[15][1]= "FE Warning errors per event per LB";
   for(int j=0; j<nerrState; j++){
      for (int i=0; i<nlayers-1; i++) {
         hname = makeHistname((histErrorName[j][0]+"_per_lumi_"+modlabel2[i]), false);
         htitles = makeHisttitle(("Average "+histErrorName[j][1]+", "+modlabel2[i]), (atext_LB+atext_erf), false);
         sc = rodExpert.regHist(m_errorStates_per_lumi_mod[i][j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
      bool errorHist = false;
      hname = makeHistname((histErrorName[j][0]+"_Map"), false);
      htitles = makeHisttitle((histErrorName[j][1]), (atext_LB+atext_erf), false);
      m_ErrorStateMap[j] = new PixelMon2DMapsLW(hname.c_str(), htitles.c_str(), m_doIBL, errorHist);
      sc = m_ErrorStateMap[j]->regHist(rodExpert, m_doIBL, errorHist);
   }

   for( int i=0; i<nlayers; i++){
      hname = makeHistname(("Bad_Module_Errors_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Errors for Bad Module, "+modlabel2[i]), (atext_LB+atext_erf), false);
      sc = rodExpert.regHist(m_bad_mod_errors_mod[i]  = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_ES, minbin_ES, maxbin_ES));

      hname = makeHistname(("Errors_EtaID_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Errors vs Eta Module ID, "+modlabel2[i]), ";eta module ID;Error State;# Errors", false);
      sc = rodExpert.regHist(m_errors_etaid_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nmod_eta[i], min_eta[i], max_eta[i], 32,-0.5,31.5));

      //tmp = "Errors_EtaID_per_event_" + modlabel2[i];
      //tmp2 = "Errors vs Eta Module ID/event, " + modlabel2[i] + m_histTitleExt + ";#eta module ID;Error State;# Errors";
      //sc = rodExpert.regHist(m_errors_etaid_per_evt_mod[i] = TH2F_LW::create( tmp.c_str(), tmp2.c_str(), nmod_eta[i], min_eta[i], max_eta[i], 32,-0.5,31.5));
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

   for(int i=0; i<nlayers; i++){
      for( unsigned int x=1; x<=nmod_eta[i]; x++){
         if(i == PixLayer::kECA || i == PixLayer::kECC){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, disk[x-1]);
            //m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, disk[x-1]);
         }
         if(i >= PixLayer::kB0  && i <= PixLayer::kB2){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, mod[x-1]);
            //m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, mod[x-1]);
         }
         if(i == PixLayer::kIBL){
            m_errors_etaid_mod[i]->GetXaxis()->SetBinLabel(x, modIBL[x-1]);
            //m_errors_etaid_per_evt_mod[i]->GetXaxis()->SetBinLabel(x, modIBL[x-1]);
         }
      }
      for( unsigned int y=1; y<=32; y++){
         if(i != PixLayer::kIBL){
            m_errors_etaid_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
            //m_errors_etaid_per_evt_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
         }else{
            m_errors_etaid_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
            //m_errors_etaid_per_evt_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
         }
      }
   }
   for( int i=0; i<nlayersIBL2d3d; i++){
      for( unsigned int y=1; y<=m_ErrorBit_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++){
         if( i < PixLayerIBL2D3D::kIBL )
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
         else
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
      }
      for( unsigned int y=1; y<=m_Error_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++){
         if( i < PixLayerIBL2D3D::kIBL )
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorType2[y-1].c_str());
         else
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorType2[y-1].c_str());
      }
   }

   for( int j=0; j<nlayers; j++){
      for (int i=0;i<32;i++){
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
      m_errors = new PixelMonModules1D("errors", ("Errors in module:ErrorType" + m_histTitleExt + ";Number of Errors").c_str(), 7,0.5,7.5,m_doIBL);
      sc = m_errors->regHist(this,(path+"/ModulesErrors").c_str(),run,m_doIBL);
      for(int k=0; k<7; k++) m_errors->SetBinLabel(errorType2[k].c_str(),k+1);
   }

   if(m_do2DMaps && !m_doOnline)
   {
      for( int i=0; i<nerror_modrod; i++){
         m_ErrorTypeMap[i] = new PixelMon2DMapsLW(errorType[i].c_str(), (errorType2[i] + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorTypeMap[i]->regHist(rodHistos, m_doIBL, false);
      }
      for( int i=0; i<nerror; i++){
         m_ErrorCategoryMap[i] = new PixelMon2DMapsLW( errorCategory[i].c_str(), (errorCategory2[i] + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorCategoryMap[i]->regHist(rodHistos, m_doIBL, false);
      }

      //for (int i=0; i<nlayers-1; i++) {
      for (int i=0; i<1; i++) {
         for(int j=0; j<nerrState; j++){
      tmp = histErrorName[j][0] + "_int_LB";
      tmp2 = histErrorName[j][1] + m_histTitleExt;
      m_errors_int_LB[j]  = new PixelMon2DLumiMaps( tmp.c_str(), tmp2.c_str(), "# Errors", m_doIBL, false);
      sc = m_errors_int_LB[j]->regHist(rodExpert,m_doIBL, false);
         }
      }

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

   int nErrorsCategory_permod[PixLayerIBL2D3D::COUNT][ErrorCategory::COUNT] = {0};
   //int nErrorsCategory_permod[ErrorCategory::COUNT][PixLayer::COUNT] = {0};

   int nErrorTypes_mod[PixLayerIBL2D3D::COUNT][ErrorCategoryMODROD::COUNT];// = {0};

   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      for( int j=0; j<ErrorCategoryMODROD::COUNT; j++){
         nErrorTypes_mod[i][j] = 0;
      }
   }

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
	         //TH2FSetBinScaled(m_errors_etaid_per_evt_mod[pixlayer], m_errors_etaid_mod[pixlayer], m_event);


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
               if(m_ErrorTypeMap[ErrorType-1] && !m_doOnline) m_ErrorTypeMap[ErrorType-1]->Fill(WaferID,m_pixelid,m_doIBL, false);

	            if(m_doLumiBlock){
		            if(m_errors_LB) m_errors_LB->Fill(WaferID,m_pixelid,m_doIBL, true);
	            }
	       
	            /// Count the each error
               nErrorTypes_mod[pixlayer][ErrorType-1]++;
               if(pixlayeribl2d3d != 0) nErrorTypes_mod[pixlayeribl2d3d][ErrorType-1]++;
               if( !hasError[kErrorCategory] ){
                  if(m_ErrorCategoryMap[kErrorCategory] && !m_doOnline) m_ErrorCategoryMap[kErrorCategory]->Fill(WaferID,m_pixelid,m_doIBL, false);
                  nErrorsCategory_permod[pixlayer][kErrorCategory]++;
                  if(pixlayeribl2d3d != 0) nErrorsCategory_permod[pixlayeribl2d3d][kErrorCategory]++;
                  hasError[kErrorCategory] = true;
               }
               
            } // End of if(ErrorType)
           
            if( getErrorCategory(kBit, isIBL) != 99 ){
               nErrors_cat_mod[pixlayer][getErrorCategory(kBit, isIBL)]++;
               if(m_ErrorStateMap[getErrorCategory(kBit, isIBL)])
                  m_ErrorStateMap[getErrorCategory(kBit, isIBL)]->Fill(WaferID, m_pixelid, m_doIBL, true);
               if( m_errors_int_LB[getErrorCategory(kBit, isIBL)])
                  m_errors_int_LB[getErrorCategory(kBit, isIBL)]->Fill(LBnum,WaferID,m_pixelid,1,m_doIBL,true);
            }
         }//end bit shifting
      }//end for loop over bits
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
         if(m_ErrorBit_per_lumi_mod[i] && nmodLayer[i] > 0) m_ErrorBit_per_lumi_mod[i]->Fill(LBnum, j, nErrorBit_mod[i][j]/nmodLayer[i]);
      }
      for( int j=0; j<ErrorCategoryMODROD::COUNT; j++){
         if(m_Error_per_lumi_mod[i] && nmodLayer[i] > 0) m_Error_per_lumi_mod[i]->Fill(LBnum, j, nErrorTypes_mod[i][j]/nmodLayer[i]);
      }
   }

   
   for(int i=0; i<PixLayerIBL2D3D::COUNT; i++){

      if(m_errors_per_lumi_mod[i]) m_errors_per_lumi_mod[i]->Fill(LBnum, nErrors_mod[i]);
      if(m_SyncErrors_per_lumi_mod[i]) m_SyncErrors_per_lumi_mod[i]->Fill(LBnum, nErrorsCategory_permod[i][ErrorCategory::kSync]);
      if(m_OpticalErrors_per_lumi_mod[i]) m_OpticalErrors_per_lumi_mod[i]->Fill(LBnum, nErrorsCategory_permod[i][ErrorCategory::kOpt]);
      if(m_SEU_Errors_per_lumi_mod[i]) m_SEU_Errors_per_lumi_mod[i]->Fill(LBnum, nErrorsCategory_permod[i][ErrorCategory::kSeu]);
      if(m_TimeoutErrors_per_lumi_mod[i]) m_TimeoutErrors_per_lumi_mod[i]->Fill(LBnum, nErrorsCategory_permod[i][ErrorCategory::kTout]);
      if(m_TruncationErrors_per_lumi_mod[i]) m_TruncationErrors_per_lumi_mod[i]->Fill(LBnum, nErrorsCategory_permod[i][ErrorCategory::kTrunc]);

      for( int j=0; j<ErrorCategory::COUNT; j++){
         if(m_ErrorFraction_per_evt[j][i] && nmodLayer[i] > 0) m_ErrorFraction_per_evt[j][i]->Fill(LBnum, nErrorsCategory_permod[i][j]/nmodLayer[i]);
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

