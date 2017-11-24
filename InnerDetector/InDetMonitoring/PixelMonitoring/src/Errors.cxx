/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing module errors
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TString.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DLumiMaps.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookRODErrorMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Errors" << endmsg;

  std::string path = "Pixel/Errors";
  std::string path2 = "Pixel/ErrorsExpert";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
  MonGroup rodHistos(this, path.c_str(), run, ATTRIB_MANAGED);   // declare a group of histograms
  MonGroup rodExpert(this, path2.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  const std::array<std::pair<std::string, std::string>, kNumErrorStates> error_state_labels = {{
      std::make_pair("Mod_Sync_BCID1_errors", "FE/MCC BCID1 Sync Errors"),
      std::make_pair("Mod_Sync_BCID2_errors", "FE/MCC BCID2 Sync Errors"),
      std::make_pair("Mod_Sync_LVL1ID_errors", "FE/MCC LVL1ID Sync Errors"),
      std::make_pair("ROD_Sync_BCID_errors", "ROD BCID Sync Errors"),
      std::make_pair("ROD_Sync_LVL1ID_errors", "ROD LVL1ID Sync Errors"),
      std::make_pair("Mod_Trunc_EOC_errors", "FE/MCC EoC Trunc Errors"),
      std::make_pair("Mod_Trunc_Hit_Overflow_errors", "FE/MCC Hit Overflow Trunc Errors"),
      std::make_pair("Mod_Trunc_EoE_Overflow_errors", "FE/MCC EoE Overflow Trunc Errors"),
      std::make_pair("ROD_Trunc_HT_Limit_errors", "ROD H/T Limit Trunc Errors"),
      std::make_pair("ROD_Trunc_ROD_OF_errors", "ROD Overflow Trunc Errors"),
      std::make_pair("Optical_Errors", "Preamble/Header Errors"),
      std::make_pair("SEU_Hit_Parity", "SEU Hit Parity Errors"),
      std::make_pair("SEU_Register_Parity", "SEU Register Parity Errors"),
      std::make_pair("SEU_Hamming", "SEU Hamming Code Errors"),
      std::make_pair("ROD_Timeout", "ROD Formatter Timeout Errors"),
      std::make_pair("FE_Warning", "FE Warning Errors"),
  }};

  std::array<std::pair<std::string, std::string>, kNumErrorStatesIBL> error_state_labelsIBL = {{
      std::make_pair("BCID_errors", "Synchronization BCID errors"),
      std::make_pair("LVL1ID_errors", "Synchronization LVL1ID errors"),
      std::make_pair("Row_Column_errors", "Row Column errors"),
      std::make_pair("Limit_errors", "Limit_errors"),
      std::make_pair("Preamble_errors", "Preamble errors"),
      std::make_pair("Masked_link_errors", "Masked link errors"),
      std::make_pair("Hamming_code_0_errors", "Hamming code 0 errors"),
      std::make_pair("Hamming_code_1_errors", "Hamming code 1 errors"),
      std::make_pair("Hamming_code_2_errors", "Hamming code 2 errors"),
      std::make_pair("L1_incounter_errors", "L1 in counter errors"),
      std::make_pair("L1_request_counter_errors", "L1 request counter errors"),
      std::make_pair("L1_register_errors", "L1 register errors"),
      std::make_pair("L1_Trigger_ID_errors", "L1 trigger ID errors"),
      std::make_pair("Readout_processor_errors", "Readout processor errors"),
      std::make_pair("Skipped_trig_counter_errors", "Skipped trigger counter errors"),
      std::make_pair("Truncated_event_flag_errors", "Truncated event errors"),
      std::make_pair("Triple redundant_errors", "Triple redundant errors"),
      std::make_pair("Write_reg_data_errors", "Write register data errors"),
      std::make_pair("Address_errors", "Address errors"),
      std::make_pair("Other_CMD_decoder_errors", "CMD decoder errors"),
      std::make_pair("CMD_decoder_bitflip_errors", "CMD decoder bit flip errors"),
      std::make_pair("CMD_decoder_SEU_errors", "CMD decoder SEU errors"),
      std::make_pair("Data_bus_address_errors", "Data bus address errors"),
      std::make_pair("ROD_Timeout_errors", "ROD Timeout errors"),
      std::make_pair("Timeout_errors", "Timeout errors"),
      std::make_pair("BCID_counter_errors", "BCID counter errors"),
      std::make_pair("Triple_redundant_errors2", "Triple redundant errors 2"),
  }};

  const char* errorBitsPIX[kNumErrorBits] = {
    "ROD Overflow Trunc",        "ROD H/T Limit Trunc",       "2",                     "3",
    "FE/MCC EoC Trunc",          "SEU Hit Parity",            "SEU Register Parity",   "SEU Hamming Code",
    "FE Warning (Bit Flip)",     "9",                         "10",                    "11",
    "FE/MCC Hit Overflow Trunc", "FE/MCC EoE Overflow Trunc", "FE/MCC BCID1 Sync",     "FE/MCC BCID2 Sync",
    "FE/MCC LVL1ID Sync",        "17",                        "18",                    "19",
    "ROD BCID Sync",             "ROD LVL1ID Sync",           "ROD Formatter Timeout", "Preamble/Header",
    "24",                        "25",                        "26",                    "27",
    "28",                        "29",                        "30",                    "31"
  };

  const char* errorBitsIBL[kNumErrorBits] = {
    "Row/ Column Error", "Limit Error",          "Timeout Error",        "BCID Error",
    "LVL1ID Error",      "Preamble Error",       "Masked Link",          "Timeout Error",
    "BCID counter",      "Hamming code 0",       "Hamming code 1",       "Hamming code 2",
    "L1_in counter",     "L1 request counter",   "L1 register",          "L1 Trigger ID",
    "readout processor", "Skipped trig counter", "Truncated event flag", "Triple Redundant",
    "Write reg data",    "Address error",        "Other CMD decoder",    "CMD decoder bit flip",
    "CMD decoder SEU",   "Data bus address",     "Triple redundant",     "27",
    "28",                "29",                   "30",                   "31"
  };

  const std::array<std::pair<std::string, std::string>, ErrorCategoryMODROD::COUNT> error_type_labels = {{
      std::make_pair("SyncErrors_Mod", "FE/MCC Sync Errors"),
      std::make_pair("SyncErrors_ROD", "ROD Sync Errors"),
      std::make_pair("TruncErrors_Mod", "FE/MCC Trunc Errors"),
      std::make_pair("TruncErrors_ROD", "ROD Trunc Errors"),
      std::make_pair("OpticalErrors_RODMod", "Preamble/Header Errors"),
      std::make_pair("SEUErrors_RODMod", "SEU Errors"),
      std::make_pair("TimeoutErrors_RODMod", "Timeout Errors"),
  }};

  const std::array<std::pair<std::string, std::string>, ErrorCategory::COUNT> error_cat_labels = {{
      std::make_pair("SyncErrors", "Sync Errors (FE/MCC & ROD)"),
      std::make_pair("TruncErrors", "Trunc Errors (FE/MCC & ROD)"),
      std::make_pair("OpticalErrors", "Preamble/Header Errors"),
      std::make_pair("SEUErrors", "SEU Errors"),
      std::make_pair("TimeoutErrors", "Timeout Errors"),
  }};

  const std::string modlabel2[PixLayerIBL2D3DDBM::COUNT] = {"ECA", "ECC", "B0", "B1", "B2", "DBMA", "DBMC", "IBL", "IBL2D", "IBL3D"};

  unsigned int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 32};

  std::string atext_LB = ";lumi block";
  std::string atext_err = ";# errors/event";
  std::string atext_erf = ";# errors/module/event";
  std::string atext_erb = ";error bit";
  std::string atext_ers = ";error type";
  int nbins_LB = m_lbRange;
  float minbin_LB = -0.5;
  float maxbin_LB = minbin_LB + (1.0 * nbins_LB);
  std::string hname;
  std::string htitles;

  StatusCode sc;

  hname = makeHistname("SyncErrors_per_lumi_PIX", false);
  htitles = makeHisttitle("Average Synchronization errors per event, PIXEL BARREL", (atext_LB + atext_err), false);
  sc = rodHistos.regHist(m_errhist_syncerr_LB_pix = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    for (int j = 0; j < ErrorCategory::COUNT; ++j) {
      hname = makeHistname((error_cat_labels[j].first + "_per_lumi_" + modlabel2[i]), false);
      htitles = makeHisttitle(("Average " + error_cat_labels[j].second + ", " + modlabel2[i]), (atext_LB + atext_err), false);
      sc = rodHistos.regHist(m_errhist_errcat_LB[i][j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
    }

    for (int j = 0; j < ErrorCategoryMODROD::COUNT - 3; ++j) {
      hname = makeHistname((error_type_labels[j].first + "_per_lumi_" + modlabel2[i]), false);
      htitles = makeHisttitle(("Average " + error_type_labels[j].second + ", " + modlabel2[i]), (atext_LB + atext_err), false);
      sc = rodHistos.regHist(m_errhist_errtype_LB[i][j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
    }

    hname = makeHistname(("errors_per_lumi_" + modlabel2[i]), false);
    htitles = makeHisttitle(("Average Total Errors, " + modlabel2[i]), (atext_LB + atext_err), false);
    sc = rodHistos.regHist(m_errhist_tot_LB[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));

    hname = makeHistname(("ErrorBit_per_lumi_" + modlabel2[i]), false);
    htitles = makeHisttitle(("Average Errors by Error Bits, " + modlabel2[i]), (atext_LB + atext_erb + atext_erf), false);
    sc = rodHistos.regHist(m_errhist_per_bit_LB[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 31, 0., 31.));
    m_errhist_per_bit_LB[i]->SetOption("colz");

    hname = makeHistname(("Error_per_lumi_" + modlabel2[i]), false);
    htitles = makeHisttitle(("Average Errors by Error Types, " + modlabel2[i]), (atext_LB + atext_ers + atext_err), false);
    sc = rodHistos.regHist(m_errhist_per_type_LB[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, 7, 0., 7.));
    m_errhist_per_type_LB[i]->SetOption("colz");

    if (i < PixLayerIBL2D3DDBM::kDBMA) {
      for (unsigned int y = 1; y <= m_errhist_per_bit_LB[i]->GetYaxis()->GetNbins(); y++) {
        m_errhist_per_bit_LB[i]->GetYaxis()->SetBinLabel(y, errorBitsPIX[y - 1]);
      }
    } else {
      for (unsigned int y = 1; y <= m_errhist_per_bit_LB[i]->GetYaxis()->GetNbins(); y++) {
        m_errhist_per_bit_LB[i]->GetYaxis()->SetBinLabel(y, errorBitsIBL[y - 1]);
      }
    }

    for (unsigned int y = 1; y <= m_errhist_per_type_LB[i]->GetYaxis()->GetNbins(); y++) {
      m_errhist_per_type_LB[i]->GetYaxis()->SetBinLabel(y, error_type_labels[y - 1].second.c_str());
    }

    const std::string tmp[ErrorCategory::COUNT] = {"SyncErrorsFrac_per_event", "TruncationErrorsFrac_per_event", "OpticalErrorsFrac_per_event", "SEUErrorsFrac_per_event", "TimeoutErrorsFrac_per_event"};
    for (int j = 0; j < ErrorCategory::COUNT; j++) {
      hname = makeHistname((tmp[j] + "_" + modlabel2[i]), false);
      htitles = makeHisttitle(("Average " + error_cat_labels[j].second + " per Module" + ", " + modlabel2[i]), (atext_LB + atext_erf), false);
      sc = rodHistos.regHist(m_errhist_errcat_avg[j][i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
    }

    const std::string tmp2[ErrorCategoryMODROD::COUNT - 3] = {"SyncErrors_Mod_Frac_per_event", "SyncErrors_ROD_Frac_per_event", "TruncErrors_Mod_Frac_per_event", "TruncErrors_ROD_Frac_per_event"};
    for (int j = 0; j < ErrorCategoryMODROD::COUNT - 3; j++) {
      hname = makeHistname((tmp2[j] + "_" + modlabel2[i]), false);
      htitles = makeHisttitle((tmp2[j] + ", " + modlabel2[i]), (atext_LB + atext_erf), false);
      sc = rodHistos.regHist(m_errhist_errtype_avg[j][i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
    }
  }

  if (m_doOnline) {
    const std::string tmp = "errors_over_time";
    const std::string tmp2 = "Number of Errors as function of time over";
    int nbin = 99;
    float min = 0.0;
    float max = 1.0;
    sc = rodHistos.regHist(m_error_time1 = new TProfile((tmp + "_10min").c_str(), (tmp2 + " 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
    sc = rodHistos.regHist(m_error_time2 = new TProfile((tmp + "_1hr").c_str(), (tmp2 + " 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
    sc = rodHistos.regHist(m_error_time3 = new TProfile((tmp + "_6hr").c_str(), (tmp2 + " 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), nbin, min, max, "i"));
  }

  if (m_doModules) {
    m_errors = std::make_unique<PixelMonModules1D>(PixelMonModules1D("errors", ("Errors in module:ErrorType" + m_histTitleExt + ";Number of Errors").c_str(), 7, 0.5, 7.5));
    sc = m_errors->regHist(this, (path + "/ModulesErrors").c_str(), run);
    for (int k = 0; k < 7; k++) {
      m_errors->setBinLabel(error_type_labels[k].second.c_str(), k + 1);
    }
  }

  if (m_do2DMaps && !m_doOnline) {
    for (int i = 0; i < ErrorCategoryMODROD::COUNT - 3; i++) {
      m_errhist_errtype_map[i] = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW(error_type_labels[i].first, ("Total " + error_type_labels[i].second + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, true));
      sc = m_errhist_errtype_map[i]->regHist(rodHistos);
    }
    for (int i = 0; i < ErrorCategory::COUNT; i++) {
      m_errhist_errcat_map[i] = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW(error_cat_labels[i].first, ("Total " + error_cat_labels[i].second + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, true));
      sc = m_errhist_errcat_map[i]->regHist(rodHistos);
    }
  }

  for (int j = 0; j < kNumErrorStates; j++) {
    for (int i = 0; i < PixLayer::COUNT - 1; i++) {
      hname = makeHistname((error_state_labels[j].first + "_per_lumi_" + modlabel2[i]), false);
      htitles = makeHisttitle(("Average " + error_state_labels[j].second + " per event per LB, " + modlabel2[i]), (atext_LB + atext_erf), false);
      sc = rodExpert.regHist(m_errhist_expert_LB[i][j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
    }
    hname = makeHistname((error_state_labels[j].first + "_Map"), false);
    htitles = makeHisttitle((error_state_labels[j].second + " per event per LB"), "", false);
    m_errhist_expert_maps[j] = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW(hname.c_str(), htitles.c_str(), PixMon::HistConf::kPix, true));
    sc = m_errhist_expert_maps[j]->regHist(rodExpert);
  }

  if (m_do2DMaps) {
    m_errhist_femcc_errwords_map = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("femcc_errorwords", ("Average FE/MCC Error Words" + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D));
    sc = m_errhist_femcc_errwords_map->regHist(rodHistos);
  }

  for (int j = 0; j < kNumErrorStatesIBL; j++) {
    hname = makeHistname((error_state_labelsIBL[j].first + "_per_lumi_" + modlabel2[PixLayerIBL2D3D::kIBL]), false);
    htitles = makeHisttitle(("Average " + error_state_labelsIBL[j].second + " per event per LB, " + modlabel2[PixLayerIBL2D3D::kIBL]), (atext_LB + atext_erf), false);
    sc = rodExpert.regHist(m_errhist_expert_IBL_LB[j] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB));
  }

  for (int j = kNumErrorStates; j < kNumErrorStates + kNumErrorStatesIBL; j++) {
    hname = makeHistname((error_state_labelsIBL[j - kNumErrorStates].first + "_Map"), false);
    htitles = makeHisttitle((error_state_labelsIBL[j - kNumErrorStates].second + " per event per LB"), "", false);
    m_errhist_expert_maps[j] = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW(hname.c_str(), htitles.c_str(), PixMon::HistConf::kIBL, m_doIBL));
    sc = m_errhist_expert_maps[j]->regHist(rodExpert);
  }

  hname = makeHistname("ServiceRecord_Unweighted_IBL", false);
  htitles = makeHisttitle("ServiceRecord Unweighted, IBL", ";SR;Count", false);
  sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_unweighted = TH1F_LW::create(hname.c_str(), htitles.c_str(), 32, -0.5, 31.5));

  hname = makeHistname("ServiceRecord_Weighted_IBL", false);
  htitles = makeHisttitle("ServiceRecord Weighted, IBL", ";SR;Count", false);
  sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_weighted = TH1F_LW::create(hname.c_str(), htitles.c_str(), 32, -0.5, 31.5));

  hname = makeHistname("ServiceRecord_Count_IBL", false);
  htitles = makeHisttitle("ServiceRecord Count, IBL", ";SR;Count", false);
  sc = rodExpert.regHist(m_errhist_expert_servrec_ibl_count = TH1F_LW::create(hname.c_str(), htitles.c_str(), 100, -0.5, 99.5));

  if (m_errhist_expert_servrec_ibl_unweighted) {
    for (int i = 0; i < kNumErrorBits; i++) {
      m_errhist_expert_servrec_ibl_unweighted->GetXaxis()->SetBinLabel(i + 1, errorBitsIBL[i]);
    }
  }

  if (m_errhist_expert_servrec_ibl_weighted) {
    for (int i = 0; i < kNumErrorBits; i++) {
      m_errhist_expert_servrec_ibl_weighted->GetXaxis()->SetBinLabel(i + 1, errorBitsIBL[i]);
    }
  }

  for (int i = 0; i < PixLayer::COUNT; i++) {
    hname = makeHistname(("nFEswithTruncErr_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Number of FEs with FE EoC Trunc error, " + m_modLabel_PixLayerIBL2D3D[i]), ";lumi block;eta index of module;# FEs with errors in a module in an event;# event #times # modules", false);
    sc = rodExpert.regHist(m_errhist_expert_fe_trunc_err_3d[i] = new TH3F(hname.c_str(), htitles.c_str(), nbins_LB, minbin_LB, maxbin_LB, nmod_eta[i], -0.5, -0.5 + nmod_eta[i], 18, -0.5, 17.5));
  }
  if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "histograms not booked" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::bookRODErrorLumiBlockMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Errors for lowStat" << endmsg;

  std::string path = "Pixel/LumiBlock";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
  MonGroup lumiBlockHist(this, path.c_str(), lowStat, ATTRIB_MANAGED);  // declare a group of histograms

  StatusCode sc;

  if (m_do2DMaps && !m_doOnline) {
    m_errors_LB = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Errors_LB", ("Errors" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_errors_LB->regHist(lumiBlockHist);

    m_errors_RODSync_mod = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Errors_RODSync_LB", ("Errors_RODSync" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_errors_RODSync_mod->regHist(lumiBlockHist);

    m_errors_ModSync_mod = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Errors_ModSync_LB", ("Errors_ModSync" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, true));
    sc = m_errors_ModSync_mod->regHist(lumiBlockHist);
  }

  if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "histograms not booked" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillRODErrorMon(void) {
  const int kLumiBlock = m_manager->lumiBlockNumber();
  const int kNumFEs{16};

  static constexpr int kNumModulesPhi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
  static constexpr int kNumModulesEta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

  // Error counters: total; errors by bit; errors by state (sync etc.)
  int num_errors[PixLayerIBL2D3DDBM::COUNT] = {0};
  int num_errors_per_bit[PixLayerIBL2D3DDBM::COUNT][kNumErrorBits] = {0};
  int num_errors_per_state[PixLayer::COUNT - 1][kNumErrorStates] = {0};  // no IBL here
  int num_errors_per_stateIBL[kNumErrorStatesIBL] = {0};                 // IBL

  // Counter for erroneous modules on the layer, per error type and
  // category (error cat. = error type w/o ROD/MOD distinction).
  int num_errormodules_per_cat[PixLayerIBL2D3DDBM::COUNT][ErrorCategory::COUNT] = {0};
  int num_errormodules_per_type[PixLayerIBL2D3DDBM::COUNT][ErrorCategoryMODROD::COUNT] = {0};

  // Counter for erroneous FEs per module per layer.
  int num_errorFEs_EA[kNumModulesPhi[PixLayer::kECA]][kNumModulesEta[PixLayer::kECA]][kNumFEs] = {0};
  int num_errorFEs_EC[kNumModulesPhi[PixLayer::kECC]][kNumModulesEta[PixLayer::kECC]][kNumFEs] = {0};
  int num_errorFEs_B0[kNumModulesPhi[PixLayer::kB0]][kNumModulesEta[PixLayer::kB0]][kNumFEs] = {0};
  int num_errorFEs_B1[kNumModulesPhi[PixLayer::kB1]][kNumModulesEta[PixLayer::kB1]][kNumFEs] = {0};
  int num_errorFEs_B2[kNumModulesPhi[PixLayer::kB2]][kNumModulesEta[PixLayer::kB2]][kNumFEs] = {0};

  const auto& kFeErrorWords = m_ErrorSvc->getAllFeErrors();

  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

  // Pixel ID Loop
  for (; idIt != idItEnd; ++idIt) {
    Identifier WaferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID);
    const uint64_t kErrorWord = m_ErrorSvc->getModuleErrors(id_hash);

    bool is_fei4 = false;
    if (m_ErrorSvc->isActive(id_hash) && m_pixelid->barrel_ec(WaferID) == 0 && m_pixelid->layer_disk(WaferID) == 0 && m_doIBL) is_fei4 = true;
    if (m_ErrorSvc->isActive(id_hash) && (m_pixelid->barrel_ec(WaferID) == 4 || m_pixelid->barrel_ec(WaferID) == -4) && m_doIBL) is_fei4 = true;

    // Determine layer; functions return '99' for non-sensible IDs.
    const int kLayer = getPixLayerIDDBM(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
    const int kLayerIBL = getPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_pixelid->eta_module(WaferID), m_doIBL);
    if (kLayer == 99) continue;

    // Boolean whether current module has an error of type/category.
    bool has_err_cat[ErrorCategory::COUNT] = {false};
    bool has_err_type[ErrorCategoryMODROD::COUNT] = {false};

    for (unsigned int bit = 0; bit < kNumErrorBits; bit++) {
      if ((kErrorWord & (static_cast<uint64_t>(1) << bit)) != 0) {
        // For non-IBL, We deal with FE/MCC errors separately, so ignore them here!
        if (!is_fei4 && bit >= 4 && bit <= 16) continue;

        num_errors[kLayer]++;
        num_errors_per_bit[kLayer][bit]++;
        if (kLayerIBL != 99) {
          num_errors[kLayerIBL]++;
          num_errors_per_bit[kLayerIBL][bit]++;
        }

        int error_type = 0;
        int error_cat = 0;

        if (!is_fei4) {
          // if (bit == 14 || bit == 15 || bit == 16) error_type = 1;  // module synchronization errors   (14: BCID, 15: BCID. 16: LVL1ID)
          if (bit == 20 || bit == 21)              error_type = 2;  // ROD synchronization errors      (20: BCID, 21: LVL1ID)
          // if (bit == 4  || bit == 12 || bit == 13) error_type = 3;  // module truncation errors        (4: EOC, 12: hit overflow, 13: EoE overflow)
          if (bit == 0  || bit == 1)               error_type = 4;  // ROD truncation errors           (0: FIFO Overflow, 1: H/T Limit)
          if (bit == 23)                           error_type = 5;  // optical errors                  (23: preamble (bitflip))
          // if (bit >= 5  && bit <= 7)               error_type = 6;  // SEU (single event upset) errors (5,6,7: hit parity, register parity, hammingcode)
          if (bit == 22)                           error_type = 7;  // timeout errors                  (22: timeout on ROD formatter)
        } else {
          if (bit == 3  || bit == 4  || bit == 8)  error_type = 1;  // synchronization error   (3:LVL1ID, 4:BCID, 8:BCID counter error)
          if (bit == 0 || bit == 18)               error_type = 3;  // module truncation error (0:Row/Column error, 18:Truncated event)
          if (bit == 1)                            error_type = 4;  // ROD truncation error    (1:Limit error)
          if (bit == 5)                            error_type = 5;  // optical error           (5:Preable error)
          if (bit == 9 || bit == 10 || bit == 11 || bit == 19 || bit == 23 || bit == 24 || bit == 26) error_type = 6;  // SEU   (9,10,11: hammingcode, 19:Triple redundant, 23:Bit flip, 24:SEU, 26:Triple redundant)
          if (bit == 2 || bit == 7)                error_type = 7;  // Timeout error           (2:Trailer timeout error, 7:readout timeout
        }

        if (error_type) {  // if there were any errors we care about
          if (error_type == 1 || error_type == 2) error_cat = ErrorCategory::kSync;
          if (error_type == 3 || error_type == 4) error_cat = ErrorCategory::kTrunc;
          if (error_type == 5) error_cat = ErrorCategory::kOpt;
          if (error_type == 6) error_cat = ErrorCategory::kSeu;
          if (error_type == 7) error_cat = ErrorCategory::kTout;

          if (m_errors) m_errors->fill(error_type, WaferID, m_pixelid);

          if (m_doLumiBlock && m_errors_LB) {
            m_errors_LB->fill(WaferID, m_pixelid);
          }

          if (!has_err_type[error_type - 1]) {
            if (m_errhist_errtype_map[error_type - 1] && !m_doOnline) {
              m_errhist_errtype_map[error_type - 1]->fill(WaferID, m_pixelid);
            }
            num_errormodules_per_type[kLayer][error_type - 1]++;
            if (kLayerIBL != 99) num_errormodules_per_type[kLayerIBL][error_type - 1]++;
            has_err_type[error_type - 1] = true;
          }
          if (!has_err_cat[error_cat]) {
            if (m_errhist_errcat_map[error_cat] && !m_doOnline) {
              m_errhist_errcat_map[error_cat]->fill(WaferID, m_pixelid);
            }
            num_errormodules_per_cat[kLayer][error_cat]++;
            if (kLayerIBL != 99) {
              num_errormodules_per_cat[kLayerIBL][error_cat]++;
            }
            has_err_cat[error_cat] = true;
          }

          if (bit == 4) {  // EoC trunc error
            int fephi = 0;
            int feeta = 0;
            if (kLayer == PixLayerDBM::kB0 && getFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
              num_errorFEs_B0[m_pixelid->phi_module(WaferID)][(int)(fabs(6 + m_pixelid->eta_module(WaferID)))][(int)((8 * fephi) + feeta)] = 1;
            }
            if (kLayer == PixLayerDBM::kB1 && getFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
              num_errorFEs_B1[m_pixelid->phi_module(WaferID)][(int)(fabs(6 + m_pixelid->eta_module(WaferID)))][(int)((8 * fephi) + feeta)] = 1;
            }
            if (kLayer == PixLayerDBM::kB2 && getFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
              num_errorFEs_B2[m_pixelid->phi_module(WaferID)][(int)(fabs(6 + m_pixelid->eta_module(WaferID)))][(int)((8 * fephi) + feeta)] = 1;
            }
            if (kLayer == PixLayerDBM::kECA && getFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
              num_errorFEs_EA[m_pixelid->phi_module(WaferID)][(int)m_pixelid->layer_disk(WaferID)][(int)((8 * fephi) + feeta)] = 1;
            }
            if (kLayer == PixLayerDBM::kECC && getFEID(kLayer, m_pixelid->phi_index(WaferID), m_pixelid->eta_index(WaferID), fephi, feeta)) {
              num_errorFEs_EC[m_pixelid->phi_module(WaferID)][(int)m_pixelid->layer_disk(WaferID)][(int)((8 * fephi) + feeta)] = 1;
            }
          }
        }  // End of if(error_type)

        if (getErrorState(bit, is_fei4) != 99) {
          if (kLayer == PixLayerDBM::kIBL) {
            num_errors_per_stateIBL[getErrorState(bit, is_fei4) - kNumErrorStates]++;
          } else if (kLayer <= PixLayerDBM::kB2) {
            num_errors_per_state[kLayer][getErrorState(bit, is_fei4)]++;
          }
          if (m_errhist_expert_maps[getErrorState(bit, is_fei4)]) m_errhist_expert_maps[getErrorState(bit, is_fei4)]->fill(WaferID, m_pixelid);
        }

        if (kLayer == PixLayerDBM::kIBL) {
          if (m_errhist_expert_servrec_ibl_unweighted) m_errhist_expert_servrec_ibl_unweighted->Fill(bit);
          if (m_errhist_expert_servrec_ibl_weighted) m_errhist_expert_servrec_ibl_weighted->Fill(bit, m_ErrorSvc->getServiceRecordCount(bit));
          if (m_errhist_expert_servrec_ibl_count) m_errhist_expert_servrec_ibl_count->Fill(m_ErrorSvc->getServiceRecordCount(bit));
        }
      }  // end bit shifting
    }    // end for loop over bits

    unsigned int num_femcc_errwords = 0;

    // Do the same bit-shifting again, this time for FE/MCC error words.
    if (!is_fei4 && kFeErrorWords.find(id_hash) != kFeErrorWords.end()) {
      // Collection of: FE ID, associated error word
      std::map<unsigned int, unsigned int> fe_errorword_map = kFeErrorWords.find(id_hash)->second;
      if (fe_errorword_map.size() > 0) {
        num_femcc_errwords = fe_errorword_map.size();
      }

      for (const auto& map_entry : fe_errorword_map) {
        const auto& fe_errorword = map_entry.second;

        for (int bit = 0; bit < kNumErrorBits; ++bit) {
          if ((fe_errorword & (static_cast<uint64_t>(1) << bit)) != 0) {
            // FE Error word contains 'bit', so take appropriate actions.
            num_errors[kLayer]++;
            num_errors_per_bit[kLayer][bit]++;

            int error_type = 0;  // same definitions as above
            int error_cat = 0;   // same definitions as above

            if (bit == 14 || bit == 15 || bit == 16) error_type = 1;
            if (bit == 4  || bit == 12 || bit == 13) error_type = 3;
            if (bit >= 5  && bit <= 7)               error_type = 6;

            if (error_type) {  // if there were any errors we care about
              if (error_type == 1) error_cat = ErrorCategory::kSync;
              if (error_type == 3) error_cat = ErrorCategory::kTrunc;
              if (error_type == 6) error_cat = ErrorCategory::kSeu;

              if (m_errors) m_errors->fill(error_type, WaferID, m_pixelid);

              if (m_doLumiBlock && m_errors_LB) {
                m_errors_LB->fill(WaferID, m_pixelid);
              }

              // Should this stay the same? This counts '1' for errors,
              // regardless of how many FEs have that error type.
              if (!has_err_type[error_type - 1]) {
                if (m_errhist_errtype_map[error_type - 1] && !m_doOnline) {
                  m_errhist_errtype_map[error_type - 1]->fill(WaferID, m_pixelid);
                }
                num_errormodules_per_type[kLayer][error_type - 1]++;
                if (kLayerIBL != 99) num_errormodules_per_type[kLayerIBL][error_type - 1]++;
                has_err_type[error_type - 1] = true;
              }
              if (!has_err_cat[error_cat]) {
                if (m_errhist_errcat_map[error_cat] && !m_doOnline) {
                  m_errhist_errcat_map[error_cat]->fill(WaferID, m_pixelid);
                }
                num_errormodules_per_cat[kLayer][error_cat]++;
                if (kLayerIBL != 99) {
                  num_errormodules_per_cat[kLayerIBL][error_cat]++;
                }
                has_err_cat[error_cat] = true;
              }
            }

            if (getErrorState(bit, is_fei4) != 99) {
              num_errors_per_state[kLayer][getErrorState(bit, is_fei4)]++;
              if (m_errhist_expert_maps[getErrorState(bit, is_fei4)]) m_errhist_expert_maps[getErrorState(bit, is_fei4)]->fill(WaferID, m_pixelid);
            }
          }  // end bit shifting
        }    // end for loop over bits
      }      // end loop over FE error words
    }

    m_errhist_femcc_errwords_map->fill(WaferID, m_pixelid, num_femcc_errwords);

    if (m_doLumiBlock) {
      if (m_errors_ModSync_mod && has_err_type[0]) m_errors_ModSync_mod->fill(WaferID, m_pixelid);
      if (m_errors_RODSync_mod && has_err_type[1]) m_errors_RODSync_mod->fill(WaferID, m_pixelid);
    }
  }  // end loop over all identifiers

  double total_errors = 0;
  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    total_errors += num_errors[i];
  }
  if (m_error_time1 && m_error_time2 && m_error_time3) {
    fillTimeHisto(total_errors, m_error_time1, m_error_time2, m_error_time3, 10., 60., 360.);
  }

  for (int i = 0; i < PixLayer::COUNT - 1; i++) {
    for (int j = 0; j < kNumErrorStates; j++) {
      if (m_errhist_expert_LB[i][j] && m_nActive_mod[getPixLayerIDWithDBM(i)] > 0) {
        m_errhist_expert_LB[i][j]->Fill(kLumiBlock, (float)num_errors_per_state[i][j] / m_nActive_mod[getPixLayerIDWithDBM(i)]);
      }
    }
  }

  for (int j = 0; j < kNumErrorStatesIBL; j++) {
    if (m_errhist_expert_IBL_LB[j]) {
      m_errhist_expert_IBL_LB[j]->Fill(kLumiBlock, (float)num_errors_per_stateIBL[j] / m_nActive_mod[PixLayerIBL2D3DDBM::kIBL]);
    }
  }

  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    if (m_errhist_per_bit_LB[i] && m_nActive_mod[i] > 0) {
      for (int j = 0; j < kNumErrorBits; j++) {
        m_errhist_per_bit_LB[i]->Fill(kLumiBlock, j, (float)num_errors_per_bit[i][j] / m_nActive_mod[i]);
      }
    }
    if (m_errhist_per_type_LB[i] && m_nActive_mod[i] > 0) {
      for (int j = 0; j < ErrorCategoryMODROD::COUNT; j++) {
        m_errhist_per_type_LB[i]->Fill(kLumiBlock, j, (float)num_errormodules_per_type[i][j] / m_nActive_mod[i]);
      }
    }
  }

  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    if (m_errhist_tot_LB[i]) m_errhist_tot_LB[i]->Fill(kLumiBlock, num_errors[i]);

    for (int j = 0; j < ErrorCategory::COUNT; ++j) {
      if (m_errhist_errcat_LB[i][j]) m_errhist_errcat_LB[i][j]->Fill(kLumiBlock, num_errormodules_per_cat[i][j]);
    }

    for (int j = 0; j < ErrorCategoryMODROD::COUNT - 3; ++j) {
      if (m_errhist_errtype_LB[i][j]) m_errhist_errtype_LB[i][j]->Fill(kLumiBlock, num_errormodules_per_type[i][j]);
    }

    for (int j = 0; j < ErrorCategory::COUNT; j++) {
      if (m_errhist_errcat_avg[j][i] && m_nActive_mod[i] > 0) {
        m_errhist_errcat_avg[j][i]->Fill(kLumiBlock, (float)num_errormodules_per_cat[i][j] / m_nActive_mod[i]);
      }
    }
    for (int j = 0; j < ErrorCategoryMODROD::COUNT - 3; j++) {
      if (m_errhist_errtype_avg[j][i] && m_nActive_mod[i] > 0) {
        m_errhist_errtype_avg[j][i]->Fill(kLumiBlock, (float)num_errormodules_per_type[i][j] / m_nActive_mod[i]);
      }
    }
  }
  if (m_errhist_syncerr_LB_pix) {
    m_errhist_syncerr_LB_pix->Fill(kLumiBlock, num_errormodules_per_cat[PixLayerIBL2D3DDBM::kB0][ErrorCategory::kSync]
                                   + num_errormodules_per_cat[PixLayerIBL2D3DDBM::kB1][ErrorCategory::kSync]
                                   + num_errormodules_per_cat[PixLayerIBL2D3DDBM::kB2][ErrorCategory::kSync]);
  }

  for (int i = 0; i < PixLayer::COUNT; i++) {
    for (int phi = 0; phi < kNumModulesPhi[i]; phi++) {
      for (int eta = 0; eta < kNumModulesEta[i]; eta++) {
        int nfes = 0;
        for (int j = 0; j < kNumFEs; j++) {
          if (i == PixLayer::kECA) nfes += num_errorFEs_EA[phi][eta][j];
          if (i == PixLayer::kECC) nfes += num_errorFEs_EC[phi][eta][j];
          if (i == PixLayer::kB0) nfes += num_errorFEs_B0[phi][eta][j];
          if (i == PixLayer::kB1) nfes += num_errorFEs_B1[phi][eta][j];
          if (i == PixLayer::kB2) nfes += num_errorFEs_B2[phi][eta][j];
        }
        if (m_errhist_expert_fe_trunc_err_3d[i]) m_errhist_expert_fe_trunc_err_3d[i]->Fill(m_manager->lumiBlockNumber(), eta, nfes);
      }
    }
  }

  return StatusCode::SUCCESS;
}

double PixelMainMon::getErrorBitFraction(const Identifier& WaferID, const unsigned int& num_femcc_errwords) {
  // Do an estimation of the bit fraction consumed by FE/MCC error words in the
  // MCC output. Simplifications: hits are distributed uniformly on all FEs,
  // errors only occur on FEs with hits. Without these simplifications, we
  // would have to consider the FE ID bit blocks (8 bits) more carefully.
  //
  // The assumed bit lengths are:
  //  - 45 bits for event ID, header, trailer
  //  - 9 bits for each FE ID. If more than 16 hits:, count 16 FE ID blocks;
  //    otherwise calculate max(number of hits, number of error words).
  //  - 22 bits for each hit word
  //  - 22 bits for each error word
  const int layer = getPixLayerID(m_pixelid->barrel_ec(WaferID), m_pixelid->layer_disk(WaferID), m_doIBL);
  if (layer == PixLayer::kIBL) return 0.;

  unsigned int num_hits = 0;
  if (layer == PixLayer::kB0) {
    num_hits = m_HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][static_cast<int>(fabs(6 + m_pixelid->eta_module(WaferID)))];
  } else if (layer == PixLayer::kB1) {
    num_hits = m_HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][static_cast<int>(fabs(6 + m_pixelid->eta_module(WaferID)))];
  } else if (layer == PixLayer::kB2) {
    num_hits = m_HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][static_cast<int>(fabs(6 + m_pixelid->eta_module(WaferID)))];
  } else if (layer == PixLayer::kECA) {
    num_hits = m_HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][static_cast<int>(fabs(6 + m_pixelid->eta_module(WaferID)))];
  } else if (layer == PixLayer::kECC) {
    num_hits = m_HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][static_cast<int>(fabs(6 + m_pixelid->eta_module(WaferID)))];
  }

  int total_bits = 45;
  if (num_hits >= 16) {
    total_bits += 16 * 9;
  } else {
    total_bits += std::max(num_hits, num_femcc_errwords) * 9;
  }
  total_bits += num_hits * 22;
  total_bits += num_femcc_errwords * 22;
  return static_cast<double>(num_femcc_errwords * 22 / total_bits);
}

int PixelMainMon::getErrorState(int bit, bool isibl) {
  int erstate = 99;
  if (!isibl) {
    switch (bit) {
      case 14:
        erstate = 0;
        break;
      case 15:
        erstate = 1;
        break;
      case 16:
        erstate = 2;
        break;
      case 20:
        erstate = 3;
        break;
      case 21:
        erstate = 4;
        break;
      case 4:
        erstate = 5;
        break;
      case 12:
        erstate = 6;
        break;
      case 13:
        erstate = 7;
        break;
      case 1:
        erstate = 8;
        break;
      case 0:
        erstate = 9;
        break;
      case 23:
        erstate = 10;
        break;
      case 5:
        erstate = 11;
        break;
      case 6:
        erstate = 12;
        break;
      case 7:
        erstate = 13;
        break;
      case 22:
        erstate = 14;
        break;
      case 8:
        erstate = 15;
        break;
      default:
        erstate = 99;
        break;
    }
  } else {
    switch (bit) {
      case 3:
        erstate = 16;  // BCID, Synch
        break;
      case 4:
        erstate = 17;  // LVL1ID, Synch
        break;
      case 8:
        erstate = 18;  // BCID counter, Synch
        break;
      case 12:
        erstate = 19;  // L1 trigger input in EODCL counter (write pointer), Synch
        break;
      case 13:
        erstate = 20;  // L1 trigger request counter to EODCL (read pointer), Synch
        break;
      case 14:
        erstate = 21;  // L1 register, register is full, Synch
        break;
      case 15:
        erstate = 22;  // L1 trigger ID in BC register, Synch
        break;
      case 17:
        erstate = 23;  // Skipped trigger because the L1 register is full, Synch
        break;
      case 0:
        erstate = 24;  // Row/Column, Trunc
        break;
      case 18:
        erstate = 25;  // Truncated event, Trunc
        break;
      case 1:
        erstate = 26;  // Limit error ROD, Trunc
        break;
      case 5:
        erstate = 27;  // Preamble error, Optical
        break;
      case 9:
        erstate = 28;  // Hamming code in word 0 in EOCHL, SEU
        break;
      case 10:
        erstate = 29;  // Hamming code in word 1 in EOCHL, SEU
        break;
      case 11:
        erstate = 30;  // Hamming code in word 2 in EOCHL, SEU
        break;
      case 19:
        erstate = 31;  // Triple redundant mismatch in Global Configuration Memory (CNFGMEM,) SEU
        break;
      case 23:
        erstate = 32;  // Bit flip in CMD, SEU
        break;
      case 24:
        erstate = 33;  // Triple redundant mismatch in CMD, SEU
        break;
      case 26:
        erstate = 34;  // Triple redundant mismatch in EFUSE,  SEU
        break;
      case 2:
        erstate = 35;  // Trailer timeout, Timeout
        break;
      case 7:
        erstate = 36;  // Timeout ROD, Timeout
        break;
      case 6:
        erstate = 37;  // Masked link
        break;
      case 16:
        erstate = 38;  // FE readout process error
        break;
      case 20:
        erstate = 39;  // Write register data error
        break;
      case 21:
        erstate = 40;  // Address error
        break;
      case 22:
        erstate = 41;  // Other CMD decoder error
        break;
      case 25:
        erstate = 42;  // Data bus address
        break;
      default:
        erstate = 99;
        break;
    }
  }

  return erstate;
}
