// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTErrMonTool.h
 *   Class declaration for SCTErrMonTool
 *
 *    @author Martin White based on code by Luca Fiorini, Shaun Roe, Manuel Diaz Gomez and Maria Jose Casta.
 *    Major tidying/restructuring by Martin Flechl
 *
 */

#ifndef SCTERRMONTOOL_H
#define SCTERRMONTOOL_H
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_FlaggedConditionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ServiceHandle.h"

#include <set>
#include <string>
#include <vector>

/** Forward declarations*/
class ITHistSvc;
class IInterface;
class SCT_ID;
class TH1F_LW;
class TH1I;
class TH2F;
class TH2F_LW;
class TProfile;
class TProfile_LW;
class TProfile2D_LW;
class TString;

///Concrete monitoring tool derived from MonitorToolBase
class SCTErrMonTool : public ManagedMonitorToolBase {
 public:
  SCTErrMonTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCTErrMonTool() = default;
  //
  /**    @name Methods reimplemented from baseclass */
  //@{
  //initialize
  virtual StatusCode initialize() override final;
  //book
  virtual StatusCode bookHistograms() override final;
  //recurrent book
  virtual StatusCode bookHistogramsRecurrent() override final;
  //fill
  virtual StatusCode fillHistograms() override final;
  //post processing
  virtual StatusCode procHistograms() override final;
  //@}

 private:
  //Define pair for FillModule
  //First pair is eta and second pair is phi.
  //First element of pair is minimum second is maximum.
  typedef std::pair<std::pair<double, double>, std::pair<double, double>> moduleGeo_t;

  static const unsigned int s_nBinsEta;
  static const double s_rangeEta;
  static const unsigned int s_nBinsPhi;
  static const double s_WafersThreshold;

  //Count number of events
  int m_numberOfEventsLumi{0};
  int m_numberOfEvents{0};

  bool m_initialize{false};

  int m_current_lb{0};
  int m_last_reset_lb{0};

  std::string m_path{};

  std::vector<moduleGeo_t> m_geo{};

  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  IntegerProperty m_checkrecent{this, "CheckRecent", 20};

  /// flag to run online
  BooleanProperty m_runOnline{this, "runOnline", false};

  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  // Use Summary database
  BooleanProperty m_makeConfHisto{this, "MakeConfHisto", true};
  // Do lumi block 2D error histos
  BooleanProperty m_doPerLumiErrors{this, "DoPerLumiErrors", true};
  // Min stats per layer to use for number of inefficient modules
  FloatProperty m_min_stat_ineff_mod{this, "MinStatsForInEffModules", 500.0};

  BooleanProperty m_CoverageCheck{this, "CoverageCheck", true};
  BooleanProperty m_useDCS{this, "UseDCS", true};

  // Thresholds for the SCTConf histogram
  FloatProperty m_errThreshold{this, "error_threshold", 0.7};
  FloatProperty m_effThreshold{this, "efficiency_threshold", 0.9};
  IntegerProperty m_noiseThreshold{this, "noise_threshold", 150};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo"};

  /// ---------------------------------------
  //@name Service and tool members
  //@{

  /** a handle on the Hist/TTree registration service */
  ServiceHandle<ITHistSvc> m_thistSvc{this, "THistSvc", "THistSvc"};

  ToolHandle<ISCT_ConfigurationConditionsTool> m_ConfigurationTool{this, "conditionsTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
  ToolHandle<ISCT_DCSConditionsTool> m_dcsTool{this, "SCT_DCSConditionsTool", "InDetSCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "SCT_ConditionsSummaryTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
  ToolHandle<ISCT_FlaggedConditionTool> m_flaggedTool{this, "SCT_FlaggedConditionTool", "SCT_FlaggedConditionTool/InDetSCT_FlaggedConditionTool", "Tool to retrieve bad wafers with many fired strips"};

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};

  //@}

  /// Profiles/histograms not requiring postprocessing
  /// Under run directories
  TProfile_LW* m_ConfOutModules{}; // Filled in fillCondDBMaps
  TProfile_LW* m_ByteStreamVsLB[SCT_ByteStreamErrors::NUM_ERROR_TYPES]{}; // Filled in fillByteStreamErrors
  TProfile_LW* m_DetailedConfiguration{}; // Filled in fillConfigurationDetails
  TProfile_LW* m_FractionOfSCTFlagErrorsPerLB{}; // Filled in fillHistograms
  TH1F_LW* m_NumberOfSCTFlagErrorsVsLB{}; // Filled in fillHistograms
  TH1F_LW* m_NumberOfEventsVsLB{}; // Filled in fillHistograms
  TH1I* m_MaskedAllLinks{}; // Filled in fillByteStreamErrorsHelper
  TProfile* m_flaggedWafers{}; // Filled in fillHistograms
  // maskedLinks or maskedRODs
  // Unnecessary but necessary to fill necessary m_ConfNew
  /// Under LB directories
  // total number of errors
  TH2F_LW* m_pallErrsCate[SCT_Monitoring::CategoryErrors::N_ERRCATEGORY][SCT_Monitoring::N_REGIONS][SCT_Monitoring::N_ENDCAPSx2]{}; // Filled in fillByteStreamErrorsHelper. Used to fill m_allErrsCate and m_summaryErrsRecent
  // Default histos to print per lumi block
  TH2F_LW* m_numErrorsPerLumi[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Filled in fillByteStreamErrorsHelper. Used to fill m_rateErrorsPerLumi
  /// Detector coverage
  TH2F* m_mapSCT[SCT_Monitoring::numberOfProblemForCoverage]{nullptr}; // Filled in fillByteStreamErrors
  TProfile* m_PSTripModulesVsLbs{}; // Filled in fillByteStreamErrors

  /// Profiles requiring postprocessing
  /// Under run directories
  TProfile_LW* m_ConfNew{}; // Filled in fillCondDBMaps using m_MaskedAllLinks and m_allErrsCate. Noise plots are also used.
  TProfile_LW* m_LinksWithCategorisedErrorsVsLB[SCT_Monitoring::CategoryErrors::N_ERRCATEGORY]{}; // Fillded in fillByteStreamErrors using m_pallErrsCate
  /// Under LB directories
  TProfile2D_LW* m_allErrsCate[SCT_Monitoring::CategoryErrors::N_ERRCATEGORY][SCT_Monitoring::N_REGIONS][SCT_Monitoring::N_ENDCAPSx2]{}; // Rate of errors. Filled in fillByteStreamErrors. Used to fill necessary m_ConfNew
  TProfile2D_LW* m_rateErrorsPerLumi[SCT_Monitoring::N_REGIONS]{}; // Filled in checkRateHists using m_numErrorsPerLumi
  /// Only online
  TProfile_LW* m_ConfOnline[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Filled in fillCondDBMaps using m_MaskedAllLinks and m_allErrsCate
  TProfile_LW* m_ConfNoiseOnline{}; // Filled in fillCondDBMaps using noise plots
  TProfile_LW* m_ConfNoiseOnlineRecent{}; // Filled in fillCondDBMaps using noise plots
  TProfile2D_LW* m_summaryErrsRecent[SCT_Monitoring::N_REGIONS][SCT_Monitoring::N_ENDCAPSx2]{}; // Recent error rate histograms. Filled in fillByteStreamErrors using m_pallErrsCate
  TProfile* m_ConfEffOnline{}; // Filled in fillByteStreamErrors using efficiency plots
  /// Detector coverage
  TProfile* m_detectorCoverageVsLbs[SCT_Monitoring::numberOfProblemForCoverage]{nullptr}; // Filled in fillByteStreamErrors using m_mapSCT

  /// Used in bookHistograms()
  StatusCode bookConfMapsGen();
  StatusCode bookErrHistosGen();
  StatusCode bookErrHistos(int iregion);
  /// Used in bookErrHistosGen() and bookErrHistos
  TString errorsString(int errtype) const; // transfer [enum ErrorTypes] -> [TString ErrorName]
  /// Used in bookErrHistos()
  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title, TString titlehitmap,
                                 TProfile2D_LW*& tprof, TH2F_LW*& th, const int layer, const bool barrel=true) const;
  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title,
                                 TProfile2D_LW*& tprof, const int layer, const bool barrel=true) const;

  /// Used in fillHistograms()
  StatusCode fillByteStreamErrors();
  /// Used in fillByteStreamErrors()
  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>& errors,
                                 bool lumi2DHist, int err_type);
  void numByteStreamErrors(const std::set<IdentifierHash>& errors, int& ntot) const;
  bool syncDisabledSCT(std::set<IdentifierHash>& sctHashDisabled) const;
  bool syncErrorSCT(std::set<IdentifierHash>& sctHashBadLinkError,
                    std::set<IdentifierHash>& sctHashBadRODError,
                    std::set<IdentifierHash>& sctHashBadError) const;
  bool summarySCT(std::set<IdentifierHash>& sctHashAll, std::set<IdentifierHash>& sctHashSummary) const;
  bool psTripDCSSCT(std::set<IdentifierHash>& sctHashPSTripDCS, float& PSTripModules) const;
  void fillWafer(moduleGeo_t module, TH2F* histo) const;
  double calculateDetectorCoverage(const TH2F* histo) const;

  /// Used in procHistograms()
  StatusCode checkRateHists();

  /// ---------------------------------------
  //@name Service methods
  //@{
  /// Used in fillHistograms() and checkRateHists()
  StatusCode resetCondDBMaps();
  StatusCode fillCondDBMaps();
  StatusCode resetConfigurationDetails();
  StatusCode fillConfigurationDetails();
  /// Used in fillCondDBMaps()
  bool getHisto(const int lyr, const int reg, const int type, TH2* histo[2]) const;
  bool getHistoRecent(const int lyr, const int reg, const int type, TH2* histo[2]) const;
  //@}
};

#endif
