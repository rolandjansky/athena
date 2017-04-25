/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing module errors
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH3F.h"
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

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMon2DLumiMaps.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookRODErrorMon(void)
{
   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Errors" << endmsg;

   std::string path = "Pixel/Errors";
   std::string path2 = "Pixel/ErrorsExpert";
   if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
   if (m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnPixelTrack");
   MonGroup rodHistos(this, path.c_str(), run, ATTRIB_MANAGED); //declare a group of histograms
   MonGroup rodExpert(this, path2.c_str(), run, ATTRIB_MANAGED); //declare a group of histograms

   const std::array <std::pair <std::string, std::string>, kNumErrorStates > error_state_labels = {{
       std::make_pair("Mod_Sync_BCID1_errors",         "Synchronization Module BCID1 errors"),
       std::make_pair("Mod_Sync_BCID2_errors",         "Synchronization Module BCID2 errors"),
       std::make_pair("Mod_Sync_LVL1ID_errors",        "Synchronization Module LVL1ID errors"),
       std::make_pair("ROD_Sync_BCID_errors",          "Synchronization ROD BCID errors"),
       std::make_pair("ROD_Sync_LVL1ID_errors",        "Synchronization ROD LVL1ID errors"),
       std::make_pair("Mod_Trunc_EOC_errors",          "Truncation Module EOC errors"),
       std::make_pair("Mod_Trunc_Hit_Overflow_errors", "Truncation Module Hit Overflow errors"),
       std::make_pair("Mod_Trunc_EoE_Overflow_errors", "Truncation Module EoE Overflow errors"),
       std::make_pair("ROD_Trunc_HT_Limit_errors",     "Truncation ROD H/T Limit errors"),
       std::make_pair("ROD_Trunc_ROD_OF_errors",       "Truncation ROD Overflow errors"),
       std::make_pair("Optical_Errors",                "Preamble/header Errors"),
       std::make_pair("SEU_Hit_Parity",                "SEU Hit Parity errors"),
       std::make_pair("SEU_Register_Parity",           "SEU Register Parity errors"),
       std::make_pair("SEU_Hamming",                   "SEU Hamming Code errors"),
       std::make_pair("ROD_Timeout",                   "ROD Formatter Timeout errors"),
       std::make_pair("FE_Warning",                    "FE Warning errors"),
     }};

   const char * errorBitsPIX[kNumErrorBits] = {
                                    "ROD Trunc FIFO Overflow",   "ROD Trunc H/T Limit",       "2",                 "3",
                                    "Module Trunc EoC",          "SEU Hit Parity",            "SEU Reg Parity",    "SEU Hamming Code",
                                    "FE Warning (Bit Flip)",     "9",                         "10",                "11",
                                    "Module Trunc Hit Overflow", "Module Trunc EoE Overflow", "Module Sync BCID1", "Module Sync BCID2",
                                    "Module Sync LVL1ID",        "17",                        "18",                "19",
                                    "ROD Sync BCID",             "ROD Sync LVL1ID",           "Timeout",           "Optical",
                                    "24",                        "25",                        "26",                "27",
                                    "28",                        "29",                        "30",                "31"
   };

   const char* errorBitsIBL[kNumErrorBits] = {
                                   "Row/ Column Error", "Limit Error",          "Timeout Error",       "BCID Error",
                                   "LVL1ID Error",      "Preamble Error",       "Masked Link",         "Timeout Error",
                                   "BCID counter",      "Hamming code 0",       "Hamming code 1",      "Hamming code 2",
                                   "L1_in counter",     "L1 request counter",   "L1 register",         "L1 Trigger ID",
                                   "readout processor", "Skipped trig counter", "Truncated event flag","Triple Redundant",
                                   "Write reg data",    "Address error",        "Other CMD decoder",   "CMD decoder bit flip",
                                   "CMD decoder SEU",   "Data bus address",     "Triple redundant",    "27",
                                   "28",                "29",                   "30",                  "31"
   };

   const std::array <std::pair <std::string, std::string>, ErrorCategoryMODROD::COUNT> error_type_labels = {{
       std::make_pair("SyncErrors_Mod",        "SyncErrors_Mod"),
       std::make_pair("SyncErrors_ROD",        "SyncErrors_ROD"),
       std::make_pair("TruncErrors_Mod",       "TruncErrors_Mod"),
       std::make_pair("TruncErrors_ROD",       "TruncErrors_ROD"),
       std::make_pair("OpticalErrors_RODMod",  "Preamble/headerErrors"),
       std::make_pair("SEUErrors_RODMod",      "SEUErrors"),
       std::make_pair("TimeoutErrors_RODMod",  "TimeoutErrors"),
     }};

   const std::array <std::pair <std::string, std::string>, ErrorCategory::COUNT> error_cat_labels = {{
       std::make_pair("SyncErrors",     "SyncErrors"),
       std::make_pair("TruncErrors",    "TruncErrors"),
       std::make_pair("OpticalErrors",  "Preamble/headerErrors"),
       std::make_pair("SEUErrors",      "SEUErrors"),
       std::make_pair("TimeoutErrors",  "TimeoutErrors"),
     }};

   const std::string modlabel2[PixLayerIBL2D3D::COUNT] = {"ECA", "ECC", "B0", "B1", "B2", "IBL", "IBL2D", "IBL3D"};

   unsigned int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 32};

   std::string atext_LB  = ";lumi block";
   std::string atext_err = ";# errors/event";
   std::string atext_erf = ";# errors/module/event";
   std::string atext_erb = ";Error bit";
   std::string atext_ers = ";Error state";
   std::string axisTitle_LB = ";lumi block;# errors/event";
   std::string axisTitle_ES = ";Error State;# Errors";
   int nbins_LB = m_lbRange; float minbin_LB = -0.5; float maxbin_LB = minbin_LB + (1.0*nbins_LB);
   int nbins_ES = 32; float minbin_ES = -0.5; float maxbin_ES = minbin_ES + (1.0*nbins_ES);
   std::string hname;
   std::string htitles;


   hname = makeHistname("SyncErrors_per_lumi_PIX", false);
   htitles = makeHisttitle("Average Synchronization errors per event, PIXEL BARREL", (atext_LB+atext_err), false);
   sc = rodHistos.regHist(m_SyncErrors_per_lumi_PIX = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

   for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
      hname = makeHistname(("errors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average number of errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_errors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SyncErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Synchronization errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SyncErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SyncErrors_Mod_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Module Synchronization errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SyncErrors_Mod_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SyncErrors_ROD_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average ROD Synchronization errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SyncErrors_ROD_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("OpticalErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Preamble/header errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_OpticalErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("SEUErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average SEU errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_SEU_Errors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TruncationErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Truncation errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TruncationErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TruncationErrors_Mod_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Module Truncation errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TruncationErrors_Mod_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TruncationErrors_ROD_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average ROD Truncation errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TruncationErrors_ROD_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("TimeoutErrors_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Timeout errors per event, "+modlabel2[i]), (atext_LB+atext_err), false);
      sc = rodHistos.regHist(m_TimeoutErrors_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

      hname = makeHistname(("ErrorBit_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Errors per module per event, "+modlabel2[i]), (atext_LB+atext_erb+atext_err), false);
      sc = rodHistos.regHist(m_ErrorBit_per_lumi_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 31, 0., 31.));
      m_ErrorBit_per_lumi_mod[i]->SetOption("colz");

      for (unsigned int y = 1; y <= m_ErrorBit_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++) {
         if (i < PixLayerIBL2D3D::kIBL) {
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y-1]);
         } else {
            m_ErrorBit_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y-1]);
         }
      }
      hname = makeHistname(("Error_per_lumi_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Average Errors per module per event, "+modlabel2[i]), (atext_LB+atext_ers+atext_err), false);
      sc = rodHistos.regHist(m_Error_per_lumi_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 7, 0., 7.));
      m_Error_per_lumi_mod[i]->SetOption("colz");

      for (unsigned int y = 1; y <= m_Error_per_lumi_mod[i]->GetYaxis()->GetNbins(); y++) {
         if (i < PixLayerIBL2D3D::kIBL) {
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, error_type_labels[y-1].second.c_str());
         } else {
            m_Error_per_lumi_mod[i]->GetYaxis()->SetBinLabel(y, error_type_labels[y-1].second.c_str());
         }
      }

      const std::string tmp[ErrorCategory::COUNT] = {"SyncErrorsFrac_per_event", "TruncationErrorsFrac_per_event", "OpticalErrorsFrac_per_event", "SEUErrorsFrac_per_event", "TimeoutErrorsFrac_per_event"};
      for (int j = 0; j < ErrorCategory::COUNT; j++) {
         hname = makeHistname((tmp[j]+"_"+modlabel2[i]), false);
         htitles = makeHisttitle((tmp[j]+", "+modlabel2[i]), (atext_LB+atext_erf), false);
         sc = rodHistos.regHist(m_ErrorFraction_per_evt[j][i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
   }

   if (m_doOnline) {
      const std::string tmp = "errors_over_time";
      const std::string tmp2 = "Number of Errors as function of time over";
      int nbin = 99; float min = 0.0; float max = 1.0;
      sc = rodHistos.regHist(m_error_time1 = new TProfile((tmp + "_10min").c_str(), (tmp2 + " 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
      sc = rodHistos.regHist(m_error_time2 = new TProfile((tmp + "_1hr").c_str(),   (tmp2 + " 1 hour.  36 sec/bin"   + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
      sc = rodHistos.regHist(m_error_time3 = new TProfile((tmp + "_6hr").c_str(),   (tmp2 + " 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
   }

   if (m_doModules) {
      m_errors = new PixelMonModules1D("errors", ("Errors in module:ErrorType" + m_histTitleExt + ";Number of Errors").c_str(), 7, 0.5, 7.5, m_doIBL);
      sc = m_errors->regHist(this, (path+"/ModulesErrors").c_str(), run, m_doIBL);
      for (int k = 0; k < 7; k++) m_errors->SetBinLabel(error_type_labels[k].second.c_str(), k+1);
   }

   if (m_do2DMaps && !m_doOnline) {
      for (int i = 0; i < ErrorCategoryMODROD::COUNT - 3; i++) {
         m_ErrorTypeMap[i] = new PixelMon2DMapsLW(error_type_labels[i].first, (error_type_labels[i].second + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorTypeMap[i]->regHist(rodHistos, m_doIBL, false);
      }
      for (int i = 0; i < ErrorCategory::COUNT; i++) {
         m_ErrorCategoryMap[i] = new PixelMon2DMapsLW(error_cat_labels[i].first.c_str(), (error_cat_labels[i].second + m_histTitleExt).c_str(), m_doIBL, false);
         sc = m_ErrorCategoryMap[i]->regHist(rodHistos, m_doIBL, false);
      }
   }

   if (m_do2DMaps && !m_doOnline) {
     for (int j = 0; j < kNumErrorStates; j++) {
       m_errhist_expert_LB_maps[j]  = new PixelMon2DLumiMaps(error_state_labels[j].first + "_int_LB", error_state_labels[j].second + " per event per LB" + m_histTitleExt, "# Errors", m_doIBL, false);
       sc = m_errhist_expert_LB_maps[j]->regHist(rodExpert, m_doIBL, false);
     }
   }

   for (int j = 0; j < kNumErrorStates; j++) {
      for (int i = 0; i < PixLayer::COUNT - 1; i++) {
         hname = makeHistname((error_state_labels[j].first+"_per_lumi_"+modlabel2[i]), false);
         htitles = makeHisttitle(("Average "+error_state_labels[j].second+" per event per LB, "+modlabel2[i]), (atext_LB+atext_erf), false);
         sc = rodExpert.regHist(m_errhist_expert_LB[i][j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
      }
      hname = makeHistname((error_state_labels[j].first+"_Map"), false);
      htitles = makeHisttitle((error_state_labels[j].second + " per event per LB"), "", false);
      m_errhist_expert_maps[j] = new PixelMon2DMapsLW(hname.c_str(), htitles.c_str(), m_doIBL, false);
      sc = m_errhist_expert_maps[j]->regHist(rodExpert, m_doIBL, false);
   }

   hname = makeHistname("ServiceRecord_Unweighted_IBL", false);
   htitles = makeHisttitle("ServiceRecord Unweighted,_IBL", ";SR;Count", false);
   sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_unweighted = TH1F_LW::create(hname.c_str(), htitles.c_str(), 32, -0.5, 31.5));

   hname = makeHistname("ServiceRecord_Weighted_IBL", false);
   htitles = makeHisttitle("ServiceRecord Weighted,_IBL", ";SR;Count", false);
   sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_weighted = TH1F_LW::create(hname.c_str(), htitles.c_str(), 32, -0.5, 31.5));

   hname = makeHistname("ServiceRecord_Count_IBL", false);
   htitles = makeHisttitle("ServiceRecord Count,_IBL", ";SR;Count", false);
   sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_count = TH1F_LW::create(hname.c_str(), htitles.c_str(), 100, -0.5, 99.5));

   for (int i = 0; i < kNumErrorBits; i++) {
      if (m_errhist_expert_servrec_ibl_unweighted)
         m_errhist_expert_servrec_ibl_unweighted->GetXaxis()->SetBinLabel(i+1, errorBitsIBL[i]);
      if (m_errhist_expert_servrec_ibl_weighted)
         m_errhist_expert_servrec_ibl_weighted->GetXaxis()->SetBinLabel(i+1, errorBitsIBL[i]);
   }


   for (int i = 0; i < PixLayer::COUNT; i++) {
      hname   = makeHistname(("nFEswithTruncErr_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Number of FEs with FE EoC Trunc error, "+m_modLabel_PixLayerIBL2D3D[i]), ";lumi block;eta index of module;# FEs with errors in a module in an event;# event #times # modules", false);
      sc = rodExpert.regHist(m_errhist_expert_fe_trunc_err_3d[i] = new TH3F(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, nmod_eta[i], -0.5, -0.5 + nmod_eta[i], 18, -0.5, 17.5));
   }
   if (sc.isFailure() && msgLvl(MSG::WARNING))
      msg(MSG::WARNING)  << "histograms not booked" << endmsg;
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::BookRODErrorLumiBlockMon(void)
{
   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Errors for lowStat" << endmsg;

   std::string path = "Pixel/LumiBlock";
   if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if (m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms

   if (m_do2DMaps && !m_doOnline) {
      m_errors_LB = new PixelMon2DMapsLW("Errors_LB", ("Errors" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_errors_LB->regHist(lumiBlockHist, m_doIBL, false);

      m_errors_RODSync_mod = new PixelMon2DMapsLW("Errors_RODSync_LB", ("Errors_RODSync" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_errors_RODSync_mod->regHist(lumiBlockHist, m_doIBL, true);

      m_errors_ModSync_mod = new PixelMon2DMapsLW("Errors_ModSync_LB", ("Errors_ModSync" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_errors_ModSync_mod->regHist(lumiBlockHist, m_doIBL, false);
   }

   if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "histograms not booked" << endmsg;
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillRODErrorMon(void)
{
   const int kLumiBlock = m_manager->lumiBlockNumber();
   const int kNumFEs{16};

   static constexpr int kNumModulesPhi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
   static constexpr int kNumModulesEta[PixLayer::COUNT] = { 3,  3, 13, 13, 13, 20};

   // Error counters: total; errors by bit; errors by state (sync etc.)
   int num_errors[PixLayerIBL2D3D::COUNT] = {0};
   int num_errors_per_bit[PixLayerIBL2D3D::COUNT][kNumErrorBits] = {0};
   int num_errors_per_state[PixLayer::COUNT - 1][kNumErrorStates] = {0}; // no IBL here

   // Counter for erroneous modules on the layer, per error type and
   // category (error cat. = error type w/o ROD/MOD distinction).
   int num_errormodules_per_cat[PixLayerIBL2D3D::COUNT][ErrorCategory::COUNT] = {0};
   int num_errormodules_per_type[PixLayerIBL2D3D::COUNT][ErrorCategoryMODROD::COUNT] = {0};

   // Counter for erroneous FEs per module per layer.
   int num_errorFEs_EA[kNumModulesPhi[PixLayer::kECA]][kNumModulesEta[PixLayer::kECA]][kNumFEs] = {0};
   int num_errorFEs_EC[kNumModulesPhi[PixLayer::kECC]][kNumModulesEta[PixLayer::kECC]][kNumFEs] = {0};
   int num_errorFEs_B0[kNumModulesPhi[PixLayer::kB0] ][kNumModulesEta[PixLayer::kB0] ][kNumFEs] = {0};
   int num_errorFEs_B1[kNumModulesPhi[PixLayer::kB1] ][kNumModulesEta[PixLayer::kB1] ][kNumFEs] = {0};
   int num_errorFEs_B2[kNumModulesPhi[PixLayer::kB2] ][kNumModulesEta[PixLayer::kB2] ][kNumFEs] = {0};

   PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

   ///// Pixel ID Loop
   for (; idIt != idItEnd; ++idIt) {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID);
      const uint64_t kErrorWord = m_ErrorSvc->getModuleErrors(id_hash);

      bool is_ibl = false;
      if (m_ErrorSvc->isActive(id_hash) && m_pixelid->barrel_ec(WaferID) == 0 && m_pixelid->layer_disk(WaferID) == 0 && m_doIBL) is_ibl = true;

      // Determine layer; functions return '99' for non-sensible IDs.
      const int kLayer = GetPixLayerID(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
      const int kLayerIBL = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_pixelid->eta_module(WaferID), m_doIBL);
      if (kLayer == 99) continue;

      // Boolean whether current module has an error of type/category.
      bool has_err_cat[ErrorCategory::COUNT] = {false};
      bool has_err_type[ErrorCategoryMODROD::COUNT] = {false};

      for (unsigned int bit = 0; bit < kNumErrorBits; bit++) {
         if ((kErrorWord & (static_cast<uint64_t>(1)<<bit)) != 0) {
            num_errors[kLayer]++;
            num_errors_per_bit[kLayer][bit]++;
            if (kLayerIBL != 99) {
               num_errors[kLayerIBL]++;
               num_errors_per_bit[kLayerIBL][bit]++;
            }

            int error_type = 0;
            int error_cat = 0;

            if (!is_ibl) {
               if (bit == 14 || bit == 15 || bit == 16) error_type = 1;  // module synchronization errors   (14: BCID, 15: BCID. 16: LVL1ID)
               if (bit == 20 || bit == 21)              error_type = 2;  // ROD synchronization errors      (20: BCID, 21: LVL1ID)
               if (bit == 4  || bit == 12 || bit == 13) error_type = 3;  // module truncation errors        (4: EOC, 12: hit overflow, 13: EoE overflow)
               if (bit == 0  || bit == 1)               error_type = 4;  // ROD truncation errors           (0: FIFO Overflow, 1: H/T Limit)
               if (bit == 23)                           error_type = 5;  // optical errors                  (23: preamble (bitflip))
               if (bit >= 5  && bit <= 7)               error_type = 6;  // SEU (single event upset) errors (5,6,7: hit parity, register parity, hammingcode)
               if (bit == 22)                           error_type = 7;  // timeout errors                  (22: timeout on ROD formatter)
            } else {
               if (bit == 3  || bit == 4  || bit == 8)  error_type = 1;  // synchronization error   (3:LVL1ID, 4:BCID, 8:BCID counter error)
               if (bit == 0 || bit == 18)               error_type = 3;  // module truncation error (0:Row/Column error, 18:Truncated event)
               if (bit == 1)                            error_type = 4;  // ROD truncation error    (1:Limit error)
               if (bit == 5)                            error_type = 5;  // optical error           (5:Preable error)
               if (bit == 9 || bit == 10 || bit == 11 || bit == 19 || bit == 23 || bit == 24 || bit == 26) error_type = 6;  // SEU   (9,10,11: hammingcode, 19:Triple redundant, 23:Bit flip, 24:SEU, 26:Triple redundant)
               if (bit == 2 || bit == 7)                error_type = 7;  // Timeout error           (2:Trailer timeout error, 7:readout timeout
            }

            if (error_type) { //if there were any errors we care about
               if (error_type == 1 || error_type == 2) error_cat = ErrorCategory::kSync;
               if (error_type == 3 || error_type == 4) error_cat = ErrorCategory::kTrunc;
               if (error_type == 5) error_cat = ErrorCategory::kOpt;
               if (error_type == 6) error_cat = ErrorCategory::kSeu;
               if (error_type == 7) error_cat = ErrorCategory::kTout;

               if (m_errors) m_errors->Fill(error_type, WaferID, m_pixelid, m_doIBL);

               if (m_doLumiBlock && m_errors_LB) {
                  m_errors_LB->Fill(WaferID, m_pixelid, m_doIBL, true);
               }

               if (!has_err_type[error_type-1]) {
                  if (m_ErrorTypeMap[error_type-1] && !m_doOnline) {
                     m_ErrorTypeMap[error_type-1]->Fill(WaferID, m_pixelid, m_doIBL, false);
                  }
                  num_errormodules_per_type[kLayer][error_type-1]++;
                  if (kLayerIBL != 99) num_errormodules_per_type[kLayerIBL][error_type-1]++;
                  has_err_type[error_type-1] = true;
               }
               if (!has_err_cat[error_cat]) {
                  if (m_ErrorCategoryMap[error_cat] && !m_doOnline) {
                     m_ErrorCategoryMap[error_cat]->Fill(WaferID, m_pixelid, m_doIBL, false);
                  }
                  num_errormodules_per_cat[kLayer][error_cat]++;
                  if (kLayerIBL != 99) {
                     num_errormodules_per_cat[kLayerIBL][error_cat]++;
                  }
                  has_err_cat[error_cat] = true;
               }

               if (bit == 4) { /// EoC trunc error
                  int fephi = 0;
                  int feeta = 0;
                  if (kLayer == PixLayer::kB0 && GetFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
                     num_errorFEs_B0[m_pixelid->phi_module(WaferID)][(int)(fabs(6+m_pixelid->eta_module(WaferID)))][(int)((8*fephi)+feeta)] = 1;
                  }
                  if (kLayer == PixLayer::kB1 && GetFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
                     num_errorFEs_B1[m_pixelid->phi_module(WaferID)][(int)(fabs(6+m_pixelid->eta_module(WaferID)))][(int)((8*fephi)+feeta)] = 1;
                  }
                  if (kLayer == PixLayer::kB2 && GetFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
                     num_errorFEs_B2[m_pixelid->phi_module(WaferID)][(int)(fabs(6+m_pixelid->eta_module(WaferID)))][(int)((8*fephi)+feeta)] = 1;
                  }
                  if (kLayer == PixLayer::kECA && GetFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
                     num_errorFEs_EA[m_pixelid->phi_module(WaferID)][(int)m_pixelid->layer_disk(WaferID)][(int)((8*fephi)+feeta)] = 1;
                  }
                  if (kLayer == PixLayer::kECC && GetFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
                     num_errorFEs_EC[m_pixelid->phi_module(WaferID)][(int)m_pixelid->layer_disk(WaferID)][(int)((8*fephi)+feeta)] = 1;
                  }
               }
            } // End of if(error_type)

            if (getErrorState(bit, is_ibl) != 99) {
               num_errors_per_state[kLayer][getErrorState(bit, is_ibl)]++;
               if (m_errhist_expert_maps[getErrorState(bit, is_ibl)])
                  m_errhist_expert_maps[getErrorState(bit, is_ibl)]->Fill(WaferID, m_pixelid, m_doIBL, true);
               if (m_errhist_expert_LB_maps[getErrorState(bit, is_ibl)])
                  m_errhist_expert_LB_maps[getErrorState(bit, is_ibl)]->Fill(kLumiBlock, WaferID, m_pixelid, 1, m_doIBL, true);
            }

            if (kLayer == PixLayer::kIBL) {
               if (m_errhist_expert_servrec_ibl_unweighted) m_errhist_expert_servrec_ibl_unweighted->Fill(bit);
               if (m_errhist_expert_servrec_ibl_weighted)   m_errhist_expert_servrec_ibl_weighted->Fill(bit, m_ErrorSvc->getServiceRecordCount(bit));
               if (m_errhist_expert_servrec_ibl_count)    m_errhist_expert_servrec_ibl_count->Fill(m_ErrorSvc->getServiceRecordCount(bit));
            }
         } //end bit shifting
      } //end for loop over bits

      if (m_doLumiBlock) {
         if (m_errors_ModSync_mod && has_err_type[0])
            m_errors_ModSync_mod->Fill(WaferID, m_pixelid, m_doIBL, true);
         if (m_errors_RODSync_mod && has_err_type[1])
            m_errors_RODSync_mod->Fill(WaferID, m_pixelid, m_doIBL, true);
      }
   } //end loop over all identifiers


   double total_errors = 0;
   for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) total_errors += num_errors[i];
   if (m_error_time1 && m_error_time2 && m_error_time3) {
      FillTimeHisto(total_errors,  m_error_time1, m_error_time2, m_error_time3, 10., 60., 360.);
   }

   for (int i = 0; i < PixLayer::COUNT - 1; i++) {
      for (int j = 0; j < kNumErrorStates; j++) {
         if (m_errhist_expert_LB[i][j]) {
            m_errhist_expert_LB[i][j]->Fill(kLumiBlock, (float) num_errors_per_state[i][j]/m_nActive_mod[i]);
         }
      }
   }

   for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
      for (int j = 0; j < kNumErrorBits; j++) {
         if (m_ErrorBit_per_lumi_mod[i] && m_nActive_mod[i] > 0) {
            m_ErrorBit_per_lumi_mod[i]->Fill(kLumiBlock, j, (float) num_errors_per_bit[i][j]/m_nActive_mod[i]);
         }
      }
      for (int j = 0; j < ErrorCategoryMODROD::COUNT; j++) {
         if (m_Error_per_lumi_mod[i] && m_nActive_mod[i] > 0) {
            m_Error_per_lumi_mod[i]->Fill(kLumiBlock, j, (float) num_errormodules_per_type[i][j]/m_nActive_mod[i]);
         }
      }
   }


   for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
      if (m_errors_per_lumi_mod[i]) m_errors_per_lumi_mod[i]->Fill(kLumiBlock, num_errors[i]);
      if (m_SyncErrors_per_lumi_mod[i]) m_SyncErrors_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_cat[i][ErrorCategory::kSync]);
      if (m_SyncErrors_Mod_per_lumi_mod[i]) m_SyncErrors_Mod_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_type[i][ErrorCategoryMODROD::kSyncMod]);
      if (m_SyncErrors_ROD_per_lumi_mod[i]) m_SyncErrors_ROD_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_type[i][ErrorCategoryMODROD::kSyncROD]);
      if (m_OpticalErrors_per_lumi_mod[i]) m_OpticalErrors_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_cat[i][ErrorCategory::kOpt]);
      if (m_SEU_Errors_per_lumi_mod[i]) m_SEU_Errors_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_cat[i][ErrorCategory::kSeu]);
      if (m_TimeoutErrors_per_lumi_mod[i]) m_TimeoutErrors_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_cat[i][ErrorCategory::kTout]);
      if (m_TruncationErrors_per_lumi_mod[i]) m_TruncationErrors_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_cat[i][ErrorCategory::kTrunc]);
      if (m_TruncationErrors_Mod_per_lumi_mod[i]) m_TruncationErrors_Mod_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_type[i][ErrorCategoryMODROD::kTruncMod]);
      if (m_TruncationErrors_ROD_per_lumi_mod[i]) m_TruncationErrors_ROD_per_lumi_mod[i]->Fill(kLumiBlock, num_errormodules_per_type[i][ErrorCategoryMODROD::kTruncROD]);

      for (int j = 0; j < ErrorCategory::COUNT; j++) {
         if (m_ErrorFraction_per_evt[j][i] && m_nActive_mod[i] > 0) {
            m_ErrorFraction_per_evt[j][i]->Fill(kLumiBlock, (float) num_errormodules_per_cat[i][j]/m_nActive_mod[i]);
         }
      }
   }
   if (m_SyncErrors_per_lumi_PIX) {
      m_SyncErrors_per_lumi_PIX->Fill(kLumiBlock, num_errormodules_per_cat[PixLayerIBL2D3D::kB0][ErrorCategory::kSync]
                                      + num_errormodules_per_cat[PixLayerIBL2D3D::kB1][ErrorCategory::kSync]
                                      + num_errormodules_per_cat[PixLayerIBL2D3D::kB2][ErrorCategory::kSync]);
   }

   for (int i = 0; i < PixLayer::COUNT; i++) {
      for (int phi = 0; phi < kNumModulesPhi[i]; phi++) {
         for (int eta = 0; eta < kNumModulesEta[i]; eta++) {
            int nfes = 0;
            for (int j = 0; j < kNumFEs; j++) {
               if (i == PixLayer::kECA) nfes += num_errorFEs_EA[phi][eta][j];
               if (i == PixLayer::kECC) nfes += num_errorFEs_EC[phi][eta][j];
               if (i == PixLayer::kB0)  nfes += num_errorFEs_B0[phi][eta][j];
               if (i == PixLayer::kB1)  nfes += num_errorFEs_B1[phi][eta][j];
               if (i == PixLayer::kB2)  nfes += num_errorFEs_B2[phi][eta][j];
            }
            if (m_errhist_expert_fe_trunc_err_3d[i]) m_errhist_expert_fe_trunc_err_3d[i]->Fill(m_manager->lumiBlockNumber(), eta, nfes);
         }
      }
   }

   return StatusCode::SUCCESS;
}

int PixelMainMon::getErrorState(int bit, bool isibl)
{
   int erstate = 99;
   if (!isibl) {
      switch (bit) {
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
   }
   return erstate;
}

