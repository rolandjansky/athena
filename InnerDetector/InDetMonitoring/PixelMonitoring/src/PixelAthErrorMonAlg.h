/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHERRORMONTOOL_H
#define PIXELATHERRORMONTOOL_H

#include "PixelAthMonitoringBase.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"

class PixelID;

class ErrorCategory {
 public:
  enum ErrorCategoryID {kSync = 0, kTrunc, kOpt, kSeu, kTout, COUNT};
};

class ErrorCategoryRODMOD {
 public:
  enum ErrorCategoryRODMODID {kSyncMod = 0, kSyncROD, kTruncMod, kTruncROD, kOpt, kSeu, kTout, COUNT};
};

static const int kNumErrorStatesFEI3{17};
static const int kNumErrorStatesFEI4{27};

// error states = enumerators of PixelByteStreamErrors and FEI4 SR's we care about
//
static constexpr std::array<const char*, kNumErrorStatesFEI3> error_names_stateFEI3 {
    "ROD_Timeout",
    "ROD_Sync_BCID_errors",
    "ROD_Sync_LVL1ID_errors",
    "Optical_Errors",
    "Mod_Sync_LVL1ID_errors",
    "Mod_Sync_BCID2_errors",
    "Mod_Sync_BCID1_errors",
    "Mod_Trunc_EoE_Overflow_errors",
    "Mod_Trunc_Hit_Overflow_errors",
    "FE_Warning",
    "SEU_Hit_Parity",
    "SEU_Register_Parity",
    "SEU_Hamming",
    "Mod_Trunc_EOC_errors",
    "ROD_Trailer_Bit_errors",
    "ROD_Trunc_HT_Limit_errors",
    "ROD_Trunc_ROD_OF_errors"
};

static constexpr std::array<const char*, kNumErrorStatesFEI4> error_names_stateFEI4 {
    "ROD_Timeout_errors",
    "ROD_BCID_errors",
    "ROD_LVL1ID_errors",
    "ROD_Preamble_errors",
    "ROD_Trailer_errors",
    "ROD_Row-Column_errors",
    "ROD_Masked_link",
    "ROD_Limit_errors",
    "SR0_BCID_counter_errors",
    "SR1_Hamming_code_0_errors",
    "SR2_Hamming_code_1_errors",
    "SR3_Hamming_code_2_errors",
    "SR4_L1_in_counter_errors",
    "SR5_L1_request_counter_errors",
    "SR6_L1_register_errors",
    "SR7_L1_Trigger_ID_errors",
    "SR8_FE_readout_process_errors",
    "SR15_Skippped_trig_count_errors",
    "SR16_Truncated_event_flag_errors",
    "SR24_Triple_redundant_errors_CNFGMEM",
    "SR25_Write_reg_data_errors",
    "SR26_Address_errors",
    "SR27_Other_CMD_decoder_errors",
    "SR28_CMD_decoder_bitflip_errors",
    "SR29_Triple_redundant_errors_CMD",
    "SR30_Data_bus_address_errors",
    "SR31_Triple_redundant_errors_EFUSE"
};

static constexpr std::array<const char*, ErrorCategoryRODMOD::COUNT> error_names_cat_rodmod {
  "SyncErrors_Mod",
  "SyncErrors_ROD",
  "TruncErrors_Mod",
  "TruncErrors_ROD",
  "OpticalErrors_RODMod",
  "SEUErrors_RODMod",
  "TimeoutErrors_RODMod"
};

static constexpr std::array<const char*, 4> error_names_cat_rodmod_norm {
  "SyncErrors_Mod_Frac_per_event",
  "SyncErrors_ROD_Frac_per_event",
  "TruncErrors_Mod_Frac_per_event",
  "TruncErrors_ROD_Frac_per_event"
};

static constexpr std::array<const char*, ErrorCategory::COUNT> error_names_cat {
  "SyncErrors",
  "TruncErrors",
  "OpticalErrors",
  "SEUErrors",
  "TimeoutErrors"
};

static constexpr std::array<const char*, ErrorCategory::COUNT> error_names_cat_norm {
  "SyncErrorsFrac_per_event", 
  "TruncationErrorsFrac_per_event", 
  "OpticalErrorsFrac_per_event", 
  "SEUErrorsFrac_per_event", 
  "TimeoutErrorsFrac_per_event"
};

static const int numErrorStatesLayer[PixLayers::COUNT] = {
  kNumErrorStatesFEI3, kNumErrorStatesFEI3, kNumErrorStatesFEI3, kNumErrorStatesFEI3,
  kNumErrorStatesFEI3, kNumErrorStatesFEI4, kNumErrorStatesFEI4, kNumErrorStatesFEI4
};
static const int kNumErrorCatRODMods{ErrorCategoryRODMOD::COUNT};
static const int numErrorCatRODModsLayer[PixLayers::COUNT] = {
  kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods,
  kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods
};

class PixelAthErrorMonAlg : public PixelAthMonitoringBase {

 public:
  
  PixelAthErrorMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PixelAthErrorMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;
  int getErrorCategory(int error_cat_rodmod) const;
  std::bitset<kNumErrorStatesFEI3> getErrorStateFEI3Mod(uint64_t errorword) const;
  std::bitset<kNumErrorStatesFEI3> getErrorStateFE(uint64_t errorword, bool isibl) const;
  void fillErrorCatRODmod(uint64_t mod_errorword, int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT]) const;
  void fillErrorCatRODmod(uint64_t fe_errorword, bool is_fei4, int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT]) const;
  void fillErrorCatRODmod(int servicecode, int payload, int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT]) const;

 private:

  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

  const PixelID* m_pixelid;

  bool m_doOnline;
  bool m_doModules;
  bool m_doLumiBlock;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doHeavyIonMon;
};
#endif
