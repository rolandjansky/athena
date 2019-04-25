// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTErrMonTool.h
 *   Class declaration for SCTErrMonTool
 *
 *    @author Martin White based on code by Luca Fiorini,Shaun Roe, Manuel Diaz Gomez and Maria Jose Casta.
 *    Major tidying/restructuring by Martin Flechl
 *
 */

#ifndef SCTERRMONTOOL_H
#define SCTERRMONTOOL_H
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_MonitoringNumbers.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ServiceHandle.h"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

/** Forward declarations*/
class IInterface;
class TH1F;
class TH2F;
class TH1I;
class TH2I;
class TProfile;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile_LW;
class TProfile2D_LW;
class StatusCode;
class SCT_ID;
class SCT_ModuleStatistics;
class TString;

///Concrete monitoring tool derived from MonitorToolBase
class SCTErrMonTool : public ManagedMonitorToolBase {
 public:
  SCTErrMonTool(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~SCTErrMonTool();
  //
  /**    @name Methods reimplemented from baseclass */
  //@{
  //initialize
  virtual StatusCode initialize() override final;
  //book
  virtual StatusCode bookHistograms() override final;
  //fill
  virtual StatusCode fillHistograms() override final;
  //post processing
  virtual StatusCode procHistograms() override final;
  //Recurrent
  virtual StatusCode bookHistogramsRecurrent() override final;
  //book
  StatusCode copyHistograms();
  StatusCode copyHistograms_book();
  //@}

 private:
  //Define pair for FillModule
  //First pair is eta and second pair is phi.
  //First element of pair is minimum second is maximum.
  typedef std::pair<std::pair<double, double>, std::pair<double, double>> moduleGeo_t;

  enum CategoryErrors {MASKEDLINKALL=0, SUMMARY, BADERR, LINKLEVEL, RODLEVEL, MASKEDCHIP, N_ERRCATEGORY};

  enum ProblemForCoverage {
    all, //All SCT module for counting good module
    disabled, //Disabled
    badLinkError, //BadLinkLevelError
    badRODError, //BadRODLevelError
    badError, //BadError = BadLinkLevelError + BadRODLevelError
    psTripDCS, //Power supply trip using SCT_DCSConditionsSvc
    summary, //Total coverage using SCT_ConditionsSummarySvc
    numberOfProblemForCoverage
  };

  /// "Magic numbers" for an SCT module
  enum {ConfbinsSummary = 6, ConfbinsDetailed = 5, ConfbinsOnline = 4};

  static const int NREGIONS_INC_GENERAL=SCT_Monitoring::N_REGIONS+1;

  const unsigned int m_nBinsEta{100};
  const double m_rangeEta{2.5};
  const unsigned int m_nBinsPhi{100};
  const double m_WafersThreshold{3.0};

  std::vector<int> m_nErrors_buf{};
  std::vector<int> m_nLinksWithErrors_buf{};
  int m_nErrors_pos{0};

  //Count number of events
  int m_numberOfEventsLumi{0};
  int m_numberOfEvents{0};

  bool m_initialize{false};

  int m_current_lb{0};
  int m_last_reset_lb{0};

  std::string m_path{};

  std::vector<moduleGeo_t> m_geo{};
  std::set<IdentifierHash> m_SCTHash[numberOfProblemForCoverage]{{}};

  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  IntegerProperty m_checkrecent{this, "CheckRecent", 20};

  /// flag to run online
  BooleanProperty m_runOnline{this, "runOnline", false};

  IntegerProperty m_evtsbins{this, "EvtsBins", 5000};
  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  // Use Summary database
  BooleanProperty m_makeConfHisto{this, "MakeConfHisto", true};
  // Do lumi block 2D error histos
  BooleanProperty m_doPerLumiErrors{this, "DoPerLumiErrors", true};
  BooleanProperty m_doErr2DPerLumiHists{this, "DoErr2DPerLumiHists", false};
  // Min stats per layer to use for number of inefficient modules
  FloatProperty m_min_stat_ineff_mod{this, "MinStatsForInEffModules", 500.0};

  BooleanProperty m_checkBadModules{this, "checkBadModules", true};
  BooleanProperty m_ignore_RDO_cut_online{this, "IgnoreRDOCutOnline", true};
  BooleanProperty m_CoverageCheck{this, "CoverageCheck", true};
  BooleanProperty m_useDCS{this, "UseDCS", true};

  // Thresholds for the SCTConf histogram
  FloatProperty m_errThreshold{this, "error_threshold", 0.7};
  FloatProperty m_effThreshold{this, "efficiency_threshold", 0.9};
  IntegerProperty m_noiseThreshold{this, "noise_threshold", 150};

  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};
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

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};

  //@}

  ///rate of errors
  TProfile2D_LW* m_allErrs[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsPerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///total number of errors
  TH2F_LW* m_pallErrs[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///total number of errors per lumi-block
  TH2F_LW* m_pallErrsPerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};

  ///rate of errors
  TProfile2D_LW* m_allErrsCate[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsCatePerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///total number of errors
  TH2F_LW* m_pallErrsCate[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  ///total number of errors per lumi-block
  TH2F_LW* m_pallErrsCatePerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};

  /// Recent error rate histograms
  TProfile2D_LW* m_summaryErrsRecent[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2]{};
  /// Default histos to print per lumi block
  TH2F_LW* m_numErrorsPerLumi[NREGIONS_INC_GENERAL]{};
  TProfile2D_LW* m_rateErrorsPerLumi[NREGIONS_INC_GENERAL]{};

  TH1I* m_nErrors{};
  TH1I* m_nLinksWithErrors{};

  TH1I* m_MaskedAllLinks{}; // maskedLinks or maskedRODs

  /// ---------------------------------------
  //@name Histograms related members
  //@{
  // Pointers to hit error histograms
  TH1F_LW* m_firstHit[NREGIONS_INC_GENERAL]{};
  TH1F_LW* m_secondHit[NREGIONS_INC_GENERAL]{};
  //@}

  std::vector<TH2F_LW*> m_p2DmapHistoVectorAll[NREGIONS_INC_GENERAL]{};

  /// Pointer to 1D histogram of Number of SCT Clusters per LBs
  TProfile_LW* m_Conf[NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ConfRN[NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ConfNew[NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ConfOutModules[NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ConfOnline[NREGIONS_INC_GENERAL]{};

  TProfile_LW* m_ByteStreamVsLB[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ByteStreamWithSctFlagVsLB[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_ByteStreamCategorisedVsLB[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_LinksWithCategorisedErrorsVsLB[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL]{};
  TProfile_LW* m_LinksWithCategorisedErrorsVsLBLayer[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2]{};

  TProfile_LW* m_LinksWithRODErrorsVsLB_check[NREGIONS_INC_GENERAL]{};

  TH1F_LW* m_NumberOfSCTFlagErrorsVsLB{};
  TH1F_LW* m_NumberOfEventsVsLB{};
  TProfile_LW* m_FractionOfSCTFlagErrorsPerLB{};

  TProfile* m_ConfEffOnline{};
  TProfile_LW* m_ConfNoiseOnline{};
  TProfile_LW* m_ConfNoiseOnlineRecent{};
  TProfile_LW* m_DetailedConfiguration{};

  TH2F* m_mapSCT[numberOfProblemForCoverage]{nullptr};

  TProfile* m_detectorCoverageVsLbs[numberOfProblemForCoverage]{nullptr};
  TProfile* m_PSTripModulesVsLbs{};

  StatusCode checkRateHists();
  StatusCode fillByteStreamErrors();
  StatusCode bookErrHistosGen();
  StatusCode bookErrHistos(int iregion);

  TString errorsString(int errtype) const; // transfer [enum ErrorTypes] -> [TString ErrorName]

  // Book noise map histograms
  StatusCode bookConfMapsGen();
  StatusCode bookConfMaps(int iregion);

  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors,
                                 TH2F_LW* histo[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2],
                                 bool lumi2DHist, int err_type);
  void numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc) const;
  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title, TString titlehitmap,
                                 TProfile2D_LW*& tprof, TH2F_LW*& th, const int layer, const bool barrel=true) const;
  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title,
                                 TProfile2D_LW*& tprof, const int layer, const bool barrel=true) const;

  /// ---------------------------------------
  //@name Service methods
  //@{
  StatusCode fillCondDBMaps();
  StatusCode fillConfigurationDetails();
  StatusCode resetCondDBMaps();
  StatusCode resetConfigurationDetails();

  bool getHisto(const int lyr, const int reg, const int type, TH2* histo[2]) const;
  bool getHistoRecent(const int lyr, const int reg, const int type, TH2* histo[2]) const;
  bool isBarrel(const int moduleNumber) const;
  bool isEndcapA(const int moduleNumber) const;
  bool isEndcapC(const int moduleNumber) const;
  //@}

  TProfile2D_LW*
    prof2Factory(const std::string& name, const std::string& title, const unsigned int&, std::vector<TProfile2D_LW*>& storageVector) const;

  bool syncDisabledSCT(std::set<IdentifierHash>& sctHashDisabled) const;
  bool syncErrorSCT(std::set<IdentifierHash>& sctHashBadLinkError,
                    std::set<IdentifierHash>& sctHashBadRODError,
                    std::set<IdentifierHash>& sctHashBadError) const;
  bool summarySCT(std::set<IdentifierHash>& sctHashAll, std::set<IdentifierHash>& sctHashSummary) const;
  bool psTripDCSSCT(std::set<IdentifierHash>& sctHashPSTripDCS, float& PSTripModules) const;

  void fillWafer(moduleGeo_t module,  TH2F* histo) const;
  double calculateDetectorCoverage(const TH2F* histo) const;
};

#endif
