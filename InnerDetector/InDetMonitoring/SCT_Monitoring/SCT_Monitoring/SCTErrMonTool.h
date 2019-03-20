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

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

/** Forward declarations*/
class IInterface;
class TH1F;
class TH2F;
class TH1I;
class TH2I;
class TProfile;
class TProfile2D;
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
class SCTErrMonTool : public ManagedMonitorToolBase
{
  //Define pair for FillModule
  //First pair is eta and second pair is phi.
  //First element of pair is minimum second is maximum.
  typedef std::pair< std::pair<double, double>, std::pair<double, double> > moduleGeo_t;
  typedef std::map< IdentifierHash, moduleGeo_t > geoContainer_t;

 public:
  SCTErrMonTool(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~SCTErrMonTool();
  //
  /**    @name Methods reimplemented from baseclass */
  //@{
  //initialize
  virtual StatusCode initialize() final;
  //book
  virtual StatusCode bookHistograms() final;
  //fill
  virtual StatusCode fillHistograms() final;
  //post processing
  virtual StatusCode procHistograms() final;
  //Recurrent
  virtual StatusCode bookHistogramsRecurrent() final;
  //book
  virtual StatusCode copyHistograms() final;
  virtual StatusCode copyHistograms_book() final;
  //@}

 private:
  static const int NREGIONS_INC_GENERAL=SCT_Monitoring::N_REGIONS+1;
  typedef TProfile2D_LW* Prof2_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  StatusCode checkRateHists();
  StatusCode fillByteStreamErrors();
  StatusCode bookErrHistosGen();
  StatusCode bookErrHistos(int iregion);


  /// ---------------------------------------
  //@name Histograms related members
  //@{
  // Pointers to hit error histograms
  TH1F_LW* m_firstHit[NREGIONS_INC_GENERAL];
  TH1F_LW* m_secondHit[NREGIONS_INC_GENERAL];
  //@}

  enum CategoryErrors { MASKEDLINKALL=0, SUMMARY, BADERR, LINKLEVEL, RODLEVEL, MASKEDCHIP, N_ERRCATEGORY};

  int errorsToGet(int errtype); // transfer [enum ErrorTypes] -> [SCT_ByteStreamErrors]
  TString errorsString(int errtype); // transfer [enum ErrorTypes] -> [TString ErrorName]

  ///rate of errors
  TProfile2D_LW* m_allErrs[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsPerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrs[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors per lumi-block
  TH2F_LW* m_pallErrsPerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];

  ///rate of errors
  TProfile2D_LW* m_allErrsCate[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsCatePerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrsCate[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors per lumi-block
  TH2F_LW* m_pallErrsCatePerLumi[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];

  /// Recent error rate histograms
  TProfile2D_LW* m_summaryErrsRecent[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Default histos to print per lumi block
  TH2F_LW* m_numErrorsPerLumi[NREGIONS_INC_GENERAL];
  TProfile2D_LW* m_rateErrorsPerLumi[NREGIONS_INC_GENERAL];

  TH1I* m_nErrors;
  TH1I* m_nLinksWithErrors;
  std::vector<int> m_nErrors_buf;
  std::vector<int> m_nLinksWithErrors_buf;
  int m_nErrors_pos;

  TH1I* m_MaskedAllLinks; // maskedLinks||maskedRODs

  //Count number of events
  int m_numberOfEventsLumi;
  int m_numberOfEvents;

  bool m_initialize;
  //max number of errors in lbs
  unsigned int m_previous_lb;

  // Book noise map histograms
  StatusCode bookConfMapsGen();
  StatusCode bookConfMaps(int iregion);

  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW* histo[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type);
  void numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc);
  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title, TString titlehitmap, TProfile2D_LW*& tprof, TH2F_LW*& th, const int layer, const bool barrel=true);

  StatusCode bookErrHistosHelper(MonGroup& mg, TString name, TString title, TProfile2D_LW*& tprof, const int layer, const bool barrel=true);

  std::vector<TH2F_LW*> m_p2DmapHistoVectorAll[NREGIONS_INC_GENERAL];

  /// "Magic numbers" for an SCT module
  enum { Confbins = 6, ConfbinsDetailed = 5 };
  // NOTE: The following is not the usual index order, which goes C, Barrel, A
  enum {iECm=0, iBARREL=1, iECp=2, iGEN=3 }; //iECp==EA, iECm==EC

  std::string m_path;
  /// CheckHists() frequency
  int m_checkrate;
  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;

  bool m_sctflag;

  /// flag to run online
  bool m_runOnline;

  int m_evtsbins;
  bool m_doPositiveEndcap;
  bool m_doNegativeEndcap;
  // Use Summary database
  bool m_makeConfHisto;
  // Do lumi block 2D error histos
  bool m_doPerLumiErrors;
  bool m_doErr2DPerLumiHists;
  // Min stats per layer to use for number of inefficient modules
  float m_min_stat_ineff_mod;

  /// ---------------------------------------
  //@name Service members
  //@{

  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;

  //@}

  /// ---------------------------------------
  //@name Service methods
  //@{

  ToolHandle<ISCT_ConfigurationConditionsTool> m_ConfigurationTool{this, "conditionsTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  StatusCode fillCondDBMaps();
  StatusCode fillConfigurationDetails();
  StatusCode resetCondDBMaps();
  StatusCode resetConfigurationDetails();

  /// Pointer to 1D histogram of Number of SCT Clusters per LBs
  TProfile_LW* m_Conf[NREGIONS_INC_GENERAL];
  TProfile_LW* m_ConfRN[NREGIONS_INC_GENERAL];
  TProfile_LW* m_ConfNew[NREGIONS_INC_GENERAL];
  TProfile_LW* m_ConfOutModules[NREGIONS_INC_GENERAL];
  TProfile_LW* m_ConfOnline[NREGIONS_INC_GENERAL];

  TProfile_LW* m_ByteStreamVsLB[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL];
  TProfile_LW* m_ByteStreamWithSctFlagVsLB[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL];
  TProfile_LW* m_ByteStreamCategorisedVsLB[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL];
  TProfile_LW* m_LinksWithCategorisedErrorsVsLB[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL];
  TProfile_LW* m_LinksWithCategorisedErrorsVsLBLayer[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];

  TProfile_LW* m_LinksWithRODErrorsVsLB_check[NREGIONS_INC_GENERAL];

  TH1F_LW* m_NumberOfSCTFlagErrorsVsLB;
  TH1F_LW* m_NumberOfEventsVsLB;
  TProfile_LW* m_FractionOfSCTFlagErrorsPerLB;

  TProfile* m_ConfEffOnline;
  TProfile_LW* m_ConfNoiseOnline;
  TProfile_LW* m_ConfNoiseOnlineRecent;
  TProfile_LW* m_DetailedConfiguration;

  /** a handle on the Hist/TTree registration service */
  ServiceHandle<ITHistSvc> m_thistSvc;
  ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
  ToolHandle<ISCT_DCSConditionsTool> m_dcsTool{this, "SCT_DCSConditionsTool", "InDetSCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "SCT_ConditionsSummaryTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;
  bool                                     m_CoverageCheck;
  bool                                     m_useDCS;

  float m_errThreshold;
  float m_effThreshold;
  int m_noiseThreshold;
  bool getHisto(const int lyr, const int reg, const int type, TH2* histo[2]);
  bool getHistoRecent(const int lyr, const int reg, const int type, TH2* histo[2]);
  /* float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide); */
  /* float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide); */
  bool isBarrel(const int moduleNumber);
  bool isEndcap(const int moduleNumber);
  bool isEndcapA(const int moduleNumber);
  bool isEndcapC(const int moduleNumber);
  //@}

  Prof2_t
    prof2Factory(const std::string& name, const std::string& title, const unsigned int&, VecProf2_t& storageVector);

  bool syncDisabledSCT();
  bool syncErrorSCT();
  bool summarySCT();
  bool psTripDCSSCT();
  bool eventVsWafer();

  void fillWafer( moduleGeo_t module,  TH2F* histo );
  double calculateDetectorCoverage(const TH2F* histo );

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

  std::vector<moduleGeo_t> m_geo;
  std::set<IdentifierHash> m_SCTHash[numberOfProblemForCoverage];
  TH2F* m_mapSCT[numberOfProblemForCoverage];

  const unsigned int m_nBinsEta;
  const double m_rangeEta;
  const unsigned int m_nBinsPhi;
  const double m_WafersThreshold;

  TProfile* m_detectorCoverageVsLbs[numberOfProblemForCoverage];
  TProfile* m_PSTripModulesVsLbs;

  float m_PSTripModules;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo"};
};

#endif
