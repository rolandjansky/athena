// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTERRMONALG_H
#define SCTERRMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"

#include <utility>
#include <vector>

class SCT_ID;

class SCTErrMonAlg : public AthMonitorAlgorithm {
 public:
  SCTErrMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTErrMonAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

 private:
  // First pair is eta and second pair is phi.
  // First element of pair is minimum second is maximum.
  typedef std::pair<std::pair<double, double>, std::pair<double, double>> moduleGeo_t;

  enum CategoryErrors {MASKEDLINKALL=0, SUMMARY, BADERR, LINKLEVEL, RODLEVEL, MASKEDCHIP, N_ERRCATEGORY};
  enum ProblemForCoverage {
    all, // All SCT module for counting good module
    disabled, // Disabled
    badLinkError, // BadLinkLevelError
    badRODError, // BadRODLevelError
    badError, // BadError = BadLinkLevelError + BadRODLevelError
    psTripDCS, // Power supply trip using SCT_DCSConditionsSvc
    summary, // Total coverage using SCT_ConditionsSummarySvc
    numberOfProblemForCoverage
  };
  enum {NREGIONS_INC_GENERAL = SCT_Monitoring::N_REGIONS+1};

  static const unsigned int s_nBinsEta;
  static const double s_rangeEta;
  static const unsigned int s_nBinsPhi;
  static const double s_wafersThreshold;

  std::vector<moduleGeo_t> m_geo{};

  BooleanProperty m_makeConfHisto{this, "MakeConfHisto", true};
  BooleanProperty m_coverageCheck{this, "CoverageCheck", true};
  BooleanProperty m_useDCS{this, "UseDCS", true};
  BooleanProperty m_doPerLumiErrors{this, "DoPerLumiErrors", true, "Do lumi block 2D error histos"};

  ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool/SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configurationTool{this, "conditionsTool", "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  ToolHandle<ISCT_DCSConditionsTool> m_dcsTool{this, "SCT_DCSConditionsTool", "SCT_DCSConditionsTool/InDetSCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "SCT_ConditionsSummaryTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};

  const SCT_ID* m_pSCTHelper{nullptr};

  /// Used in fillHistograms()
  StatusCode fillByteStreamErrors(const EventContext& ctx) const;
  /// Used in fillByteStreamErrors()
  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>& errors,
                                 int err_type) const;
  void numByteStreamErrors(const std::set<IdentifierHash>& errors, int& ntot) const;
  bool disabledSCT(std::set<IdentifierHash>& sctHashDisabled) const;
  bool errorSCT(std::set<IdentifierHash>& sctHashBadLinkError,
                    std::set<IdentifierHash>& sctHashBadRODError,
                    std::set<IdentifierHash>& sctHashBadError) const;
  bool summarySCT(std::set<IdentifierHash>& sctHashAll, std::set<IdentifierHash>& sctHashSummary) const;
  bool psTripDCSSCT(std::set<IdentifierHash>& sctHashPSTripDCS, float& PSTripModules) const;
  void fillWafer(moduleGeo_t module, TH2F& histo) const;
  double calculateDetectorCoverage(const TH2F& histo, const TH2F& histoAll) const;
};

#endif // SCTERRMONALG_H
