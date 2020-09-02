// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTERRMONALG_H
#define SCTERRMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_FlaggedConditionTool.h"

#include "TH2F.h"

#include <array>
#include <atomic>
#include <mutex>
#include <utility>
#include <vector>

class SCT_ID;

class SCTErrMonAlg : public AthMonitorAlgorithm {
 public:
  SCTErrMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTErrMonAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;
  virtual StatusCode stop() override final;

 private:
  // First pair is eta and second pair is phi.
  // First element of pair is minimum second is maximum.
  typedef std::pair<std::pair<double, double>, std::pair<double, double>> moduleGeo_t;
  class categoryErrorMap_t : public std::array<std::array<std::array<std::array<std::array<bool,
             SCT_Monitoring::N_PHI_BINS>, SCT_Monitoring::N_ETA_BINS>, SCT_Monitoring::N_ENDCAPSx2>,
             SCT_Monitoring::N_REGIONS>, SCT_Monitoring::N_ERRCATEGORY> {
    // N_PHI_BINS=56 > N_PHI_BINS_EC=52, N_ETA_BINS=13 > N_ETA_BINS_EC=3, N_ENDCAPSx2=18 > N_BARRELSx2=8 defined in SCT_MonitoringNumbers.h
  public:
    categoryErrorMap_t() {
      for (int iCat{0}; iCat<SCT_Monitoring::N_ERRCATEGORY; iCat++) {
        for (int iReg{0}; iReg<SCT_Monitoring::N_REGIONS; iReg++) {
          for (int iLay{0}; iLay<SCT_Monitoring::N_ENDCAPSx2; iLay++) {
            for (int iEta{0}; iEta<SCT_Monitoring::N_ETA_BINS; iEta++) {
              (*this)[iCat][iReg][iLay][iEta].fill(false);
            }
          }
        }
      }
    };
    int count(int errCate) {
      int cnt{0};
      for (int iReg{0}; iReg<SCT_Monitoring::N_REGIONS; iReg++) {
        for (int iLay{0}; iLay<SCT_Monitoring::N_ENDCAPSx2; iLay++) {
          for (int iEta{0}; iEta<SCT_Monitoring::N_ETA_BINS; iEta++) {
            for (int iPhi{0}; iPhi<SCT_Monitoring::N_PHI_BINS; iPhi++) {
              if ((*this)[errCate][iReg][iLay][iEta][iPhi]) cnt++;
            }
          }
        }
      }
      return cnt;
    }
  };

  struct CacheEntry {
    EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
    std::vector<TH2F> m_mapSCT;
  };

  static const unsigned int s_nBinsEta;
  static const double s_rangeEta;
  static const unsigned int s_nBinsPhi;
  static const double s_wafersThreshold;

  std::vector<moduleGeo_t> m_geo{};

  mutable std::atomic_bool m_isFirstConfigurationDetails{true};
  mutable std::mutex m_mutex{};
  mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
  mutable std::array<std::atomic_int, SCT_Monitoring::N_REGIONS_INC_GENERAL> m_nMaskedLinks ATLAS_THREAD_SAFE;
  // For coverage check because it is time consuming and run at the first event of each lumi block.
  mutable std::array<std::atomic_bool, SCT_Monitoring::NBINS_LBs+1> m_firstEventOfLB ATLAS_THREAD_SAFE;

  BooleanProperty m_makeConfHisto{this, "MakeConfHisto", true};
  BooleanProperty m_coverageCheck{this, "CoverageCheck", true};
  BooleanProperty m_coverageCheckOnlyFirtsEventOfLB{this, "CoverageCheckOnlyFirtsEventOfLB", true};
  BooleanProperty m_useDCS{this, "UseDCS", true};
  BooleanProperty m_doPerLumiErrors{this, "DoPerLumiErrors", true, "Do lumi block 2D error histos"};

  ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool/SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configurationTool{this, "conditionsTool", "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  ToolHandle<ISCT_DCSConditionsTool> m_dcsTool{this, "SCT_DCSConditionsTool", "SCT_DCSConditionsTool/InDetSCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "SCT_ConditionsSummaryTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
  ToolHandle<ISCT_FlaggedConditionTool> m_flaggedTool{this, "SCT_FlaggedConditionTool", "SCT_FlaggedConditionTool/InDetSCT_FlaggedConditionTool", "Tool to retrieve bad wafers with many fired strips"};

  const SCT_ID* m_pSCTHelper{nullptr};

  /// Used in fillHistograms()
  StatusCode fillConfigurationDetails(const EventContext& ctx) const;
  StatusCode fillByteStreamErrors(const EventContext& ctx) const;
  /// Used in fillByteStreamErrors()
  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>& errors,
                                 int err_type,
                                 categoryErrorMap_t& categoryErrorMap,
                                 std::array<int, SCT_Monitoring::N_REGIONS_INC_GENERAL>& nMaskedLinks) const;
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
