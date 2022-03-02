/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PIXELATHERRORMONTOOL_H
#define PIXELATHERRORMONTOOL_H

#include "PixelAthMonitoringBase.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"

class PixelID;

class ErrorCategory {
public:
  enum ErrorCategoryID {
    kSync = 0, kTrunc, kOpt, kSeu, kTout, COUNT
  };
};

class ErrorCategoryRODMOD {
public:
  enum ErrorCategoryRODMODID {
    kSyncMod = 0, kSyncROD, kTruncMod, kTruncROD, kOpt, kSeu, kTout, COUNT
  };
};

static const int kNumErrorStatesFEI3 {
  17
};
static const int kNumErrorStatesFEI4 {
  27
};

// error states = enumerators of PixelByteStreamErrors and FEI4 SR's we care about
//
static constexpr std::array<const char*, kNumErrorStatesFEI3> error_names_stateFEI3 {
  "RODTimeout",
  "RODSyncBCIDErrors",
  "RODSyncLVL1IDErrors",
  "OpticalErrors",
  "ModSyncLVL1IDErrors",
  "ModSyncBCID2Errors",
  "ModSyncBCID1Errors",
  "ModTruncEoEOverflowErrors",
  "ModTruncHitOverflowErrors",
  "FEWarning",
  "SEUHitParity",
  "SEURegisterParity",
  "SEUHamming",
  "ModTruncEOCErrors",
  "RODTrailerBitErrors",
  "RODTruncHTLimitErrors",
  "RODTruncRODOFErrors"
};

static constexpr std::array<const char*, kNumErrorStatesFEI4> error_names_stateFEI4 {
  "RODTimeoutErrors",
  "RODBCIDErrors",
  "RODLVL1IDErrors",
  "RODPreambleErrors",
  "RODTrailerErrors",
  "RODRowColumnErrors",
  "RODMaskedLink",
  "RODLimitErrors",
  "SR0BCIDCounterErrors",
  "SR1HammingCode0Errors",
  "SR2HammingCode1Errors",
  "SR3HammingCode2Errors",
  "SR4L1InCounterErrors",
  "SR5L1RequestCounterErrors",
  "SR6L1RegisterErrors",
  "SR7L1TriggerIDErrors",
  "SR8FEReadoutProcessErrors",
  "SR15SkipppedTrigCountErrors",
  "SR16TruncatedEventFlagErrors",
  "SR24TripleRedundantErrorsCNFGMEM",
  "SR25WriteRegDataErrors",
  "SR26AddressErrors",
  "SR27OtherCMDDecoderErrors",
  "SR28CMDDecoderBitflipErrors",
  "SR29TripleRedundantErrorsCMD",
  "SR30DataBusAddressErrors",
  "SR31TripleRedundantErrorsEFUSE"
};

static constexpr std::array<const char*, ErrorCategoryRODMOD::COUNT> error_names_cat_rodmod {
  "SyncErrorsMod",
  "SyncErrorsROD",
  "TruncErrorsMod",
  "TruncErrorsROD",
  "OpticalErrorsRODMod",
  "SEUErrorsRODMod",
  "TimeoutErrorsRODMod"
};

static constexpr std::array<const char*, 4> error_names_cat_rodmod_norm {
  "SyncErrorsModFracPerEvent",
  "SyncErrorsRODFracPerEvent",
  "TruncErrorsModFracPerEvent",
  "TruncErrorsRODFracPerEvent"
};

static constexpr std::array<const char*, ErrorCategory::COUNT> error_names_cat {
  "SyncErrors",
  "TruncErrors",
  "OpticalErrors",
  "SEUErrors",
  "TimeoutErrors"
};

static constexpr std::array<const char*, ErrorCategory::COUNT> error_names_cat_norm {
  "SyncErrorsFracPerEvent",
  "TruncationErrorsFracPerEvent",
  "OpticalErrorsFracPerEvent",
  "SEUErrorsFracPerEvent",
  "TimeoutErrorsFracPerEvent"
};

static const int numErrorStatesLayer[PixLayers::COUNT] = {
  kNumErrorStatesFEI3, kNumErrorStatesFEI3, kNumErrorStatesFEI3, kNumErrorStatesFEI3,
  kNumErrorStatesFEI3, kNumErrorStatesFEI4, kNumErrorStatesFEI4
};
static const int kNumErrorCatRODMods {
  ErrorCategoryRODMOD::COUNT
};
static const int numErrorCatRODModsLayer[PixLayers::COUNT] = {
  kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods,
  kNumErrorCatRODMods, kNumErrorCatRODMods, kNumErrorCatRODMods
};

static const int nFEIBL2D {
  2
};

class PixelAthErrorMonAlg: public PixelAthMonitoringBase {
public:
  PixelAthErrorMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PixelAthErrorMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  std::string findComponentString(int bec, int ld) const;
  int getErrorCategory(int error_cat_rodmod) const;
  std::bitset<kNumErrorStatesFEI3> getErrorStateFEI3Mod(uint64_t errorword) const;
  std::bitset<kNumErrorStatesFEI3> getErrorStateFE(uint64_t errorword, bool isibl) const;
  void fillErrorCatRODmod(uint64_t mod_errorword,
                          int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D]) const;
  void fillErrorCatRODmod(uint64_t fe_errorword, bool is_fei4,
                          int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D], int ife) const;
  void fillErrorCatRODmod(int servicecode, int payload,
                          int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D], int ife) const;
private:
  ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
  {
    this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager"
  };

  ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool {
    this, "PixelConditionsSummaryTool",
    "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"
  };

  const PixelID* m_pixelid;

  bool m_doOnline;
  bool m_doLumiBlock;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doHeavyIonMon;
};
#endif
