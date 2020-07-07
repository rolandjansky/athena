/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELATHERRORMONTOOL_H
#define PIXELATHERRORMONTOOL_H

#include "PixelAthMonitoringBase.h"
#include "PixelConditionsTools/IPixelByteStreamErrorsTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

class PixelID;
class IPixelByteStreamErrorsTool;

class ErrorCategory {
 public:
  enum ErrorCategoryID {kSync = 0, kTrunc, kOpt, kSeu, kTout, COUNT};
};

class ErrorCategoryRODMOD {
 public:
  enum ErrorCategoryRODMODID {kSyncMod = 0, kSyncROD, kTruncMod, kTruncROD, kOpt, kSeu, kTout, COUNT};
};

static const int kNumErrorBitsFEI3{32};
static const int kNumErrorBitsFEI4{40};

// error states = error bits we care about but also ordered differently
// order and naming is inherited from Run 2
// (all state histograms go into ErrorsExpert folder)
static constexpr std::array<const char*, 16> error_names_stateFEI3 {
    "Mod_Sync_BCID1_errors",
    "Mod_Sync_BCID2_errors",
    "Mod_Sync_LVL1ID_errors",
    "ROD_Sync_BCID_errors",
    "ROD_Sync_LVL1ID_errors",
    "Mod_Trunc_EOC_errors",
    "Mod_Trunc_Hit_Overflow_errors",
    "Mod_Trunc_EoE_Overflow_errors",
    "ROD_Trunc_HT_Limit_errors",
    "ROD_Trunc_ROD_OF_errors",
    "Optical_Errors",
    "SEU_Hit_Parity",
    "SEU_Register_Parity",
    "SEU_Hamming",
    "ROD_Timeout",
    "FE_Warning"
};

static constexpr std::array<const char*, 27> error_names_stateFEI4 {
    "ROD_BCID_errors",
    "ROD_LVL1ID_errors",
    "SR_BCID_counter_errors",
    "SR_L1_in_counter_errors",
    "SR_L1_request_counter_errors",
    "SR_L1_register_errors",
    "SR_L1_Trigger_ID_errors",
    "SR_Skippped_trig_count_errors",
    "SR_Row-Column_errors",
    "SR_Limit_errors",
    "SR_Truncated_event_flag_errors",
    "ROD_Preamble_errors",
    "SR_Hamming_code_0_errors",
    "SR_Hamming_code_1_errors",
    "SR_Hamming_code_2_errors",
    "SR_Triple_redundant_errors_CNFGMEM",
    "SR_CMD_decoder_bitflip_errors",
    "SR_Triple_redundant_errors_CMD",
    "SR_Triple_redundant_errors_EFUSE",
    "ROD_Trailer_errors",
    "ROD_Timeout_errors",
    "SR_Masked_link",
    "SR_FE_readout_process_errors",
    "SR_Write_reg_data_errors",
    "SR Address_errors",
    "SR_Other_CMD_decoder_errors",
    "SR_Data_bus_address_errors"
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

static const int numErrorBitsLayer[PixLayers::COUNT] = {
  kNumErrorBitsFEI3, kNumErrorBitsFEI3, kNumErrorBitsFEI3, kNumErrorBitsFEI3,
  kNumErrorBitsFEI3, kNumErrorBitsFEI4, kNumErrorBitsFEI4, kNumErrorBitsFEI4
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
  int getErrorState(int bit, bool isibl) const;
  int getErrorCategory(int error_cat_rodmod) const;

 private:

  ToolHandle<IPixelByteStreamErrorsTool> m_pixelErrorTool{this, "PixelByteStreamErrorsTool", "PixelByteStreamErrorsTool", "Tool to retrieve PixelByteStreamErrors"};
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
