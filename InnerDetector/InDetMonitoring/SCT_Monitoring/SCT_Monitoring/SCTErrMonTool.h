/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

//for CondDB
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetRawData/SCT_RDO_Container.h"

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
class ISCT_ByteStreamErrorsSvc;
class TString;
namespace InDetDD//11.09.2016
{
  class SCT_DetectorManager;
}

///Concrete monitoring tool derived from MonitorToolBase
class SCTErrMonTool : public ManagedMonitorToolBase
{
  //Define pair for FillModule
  //First pair is eta and second pair is phi.
  //First element of pair is minimum second is maximum.
  typedef std::pair< std::pair<double, double>, std::pair<double, double> > moduleGeo_t;
  typedef std::map< IdentifierHash, moduleGeo_t > geoContainer_t;
  typedef std::map< Identifier, moduleGeo_t > geoContainerPure_t;
 public:
  SCTErrMonTool(const std::string & type,const std::string & name,const IInterface* parent);
  virtual ~SCTErrMonTool();
  //
  /**    @name Methods reimplemented from baseclass */
  //@{
  //initialize 11.09.2016
  virtual StatusCode initialize() final;
  //book
  virtual StatusCode bookHistograms() final;
  //fill
  virtual StatusCode fillHistograms() final;
  //post processing
  virtual StatusCode procHistograms() final;
  //Recurrent 11.09.2016
  virtual StatusCode bookHistogramsRecurrent() final;
  //book
  virtual StatusCode copyHistograms() final;
  //@}

 private:
  static const int NREGIONS_INC_GENERAL=SCT_Monitoring::N_REGIONS+1;
  typedef TProfile2D_LW * Prof2_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  StatusCode checkRateHists();
  StatusCode fillByteStreamErrors();
  StatusCode bookErrHistos();
  StatusCode bookPositiveEndCapErrHistos();
  StatusCode bookNegativeEndCapErrHistos();


  /// ---------------------------------------
  //@name Histograms related members
  //@{
  // Pointers to hit error histograms
  TH1F_LW * m_firstHit;
  TH1F_LW * m_firstHit_ECp;
  TH1F_LW * m_firstHit_ECm;
  TH1F_LW * m_secondHit;
  TH1F_LW * m_secondHit_ECp;
  TH1F_LW * m_secondHit_ECm;
  //@}

  enum ErrorTypes {
    ABCD=0, RAW, TIMEOUT, LVL1ID, BCID, PREAMBLE, FORMATTER, MASKEDLINKS, RODCLOCK, TRUNCATEDROD, ROBFRAG, BSPARSE, MISSINGLINK, MASKEDRODS, // individual errors
    MASKEDLINKALL, // combined errors
    ABCDChip0, ABCDChip1, ABCDChip2, ABCDChip3, ABCDChip4, ABCDChip5, ABCDError1, ABCDError2, ABCDError4, // detailed ABCD errors
    SUMMARY, BADERR, LINKLEVEL, RODLEVEL, // categorized errors
    N_ERRTYPES
  };
  int errorsToGet(int errtype); // transfer [enum ErrorTypes] -> [SCT_ByteStreamErrors]
  TString errorsString(int errtype); // transfer [enum ErrorTypes] -> [TString ErrorName]

  ///rate of errors
  TProfile2D_LW* m_allErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors
  TProfile2D_LW* m_allErrs_tmp[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D_LW* m_allErrsPerLumi_tmp[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrs_tmp[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F_LW* m_pallErrsPerLumi_tmp[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Recent error rate histograms
  TProfile2D_LW* m_summaryErrsRecent[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Default histos to print per lumi block
  TH2F_LW* m_numErrorsPerLumi[NREGIONS_INC_GENERAL];
  TProfile2D_LW* m_rateErrorsPerLumi[NREGIONS_INC_GENERAL];

  TH1I *m_nErrors;
  TH1I *m_nLinksWithErrors;
  int *m_nErrors_buf;
  int *m_nLinksWithErrors_buf;
  int m_nErrors_pos;

  TH1I *m_MaskedAllLinks; // maskedLinks||maskedRODs

  //Count number of events
  int m_numberOfEventsLumi;
  int m_numberOfEvents;

  bool m_initialize;
  //max number of errors in lbs
  unsigned int m_previous_lb;

  // Book noise map histograms
  StatusCode bookConfMaps();
  StatusCode bookPositiveEndCapConfMaps();
  StatusCode bookNegativeEndCapConfMaps();

  //int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW* histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type);
  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F_LW* histo[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumi2DHist, int err_type);
  void numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc);
  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TString titlehitmap, TProfile2D_LW* &tprof, TH2F_LW* &th, const int layer, const bool barrel=true);

  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TProfile2D_LW* &tprof, const int layer, const bool barrel=true);

  std::vector<TH2F_LW *> m_p2DmapHistoVectorAll[NREGIONS_INC_GENERAL];

  /// "Magic numbers" for an SCT module
  //unsigned int m_nplanes; //to be determined from SCT Helper
  enum { Confbins = 6, ConfbinsDetailed = 5,n_lumiErrBins = 15, n_lumiBins = 3000 };
  // NOTE: The following is not the usual index order, which goes C, Barrel, A
  enum { iBARREL = 0, iECp = 1, iECm=2, iGEN=3 }; //iECp==EA, iECm==EC

  std::string m_stream;
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
  SG::ReadHandle<SCT_RDO_Container> m_dataObjectName;

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper;

  //@}

  /// ---------------------------------------
  //@name Service methods
  //@{

  //SCT_ModuleConditionsTool* m_ModuleConditions;
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_ConfigurationSvc;
  ServiceHandle<ISCT_FlaggedConditionSvc>   m_flaggedSvc;
  StatusCode fillCondDBMaps();
  StatusCode fillConfigurationDetails();
  StatusCode resetCondDBMaps();
  StatusCode resetConfigurationDetails();

  /// Pointer to 1D histogram of Number of SCT Clusters per LBs
  TProfile_LW * m_Conf[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfRN[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfNew[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfOutModules[NREGIONS_INC_GENERAL];
  TProfile_LW * m_ConfOnline[NREGIONS_INC_GENERAL];

  TProfile_LW * m_ByteStreamVsLB[N_ERRTYPES][NREGIONS_INC_GENERAL];
  TProfile_LW * m_ByteStreamWithSctFlagVsLB[N_ERRTYPES][NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithAnyErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithBadErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithLnkErrorsVsLB[NREGIONS_INC_GENERAL];
  TProfile_LW * m_LinksWithRODErrorsVsLB[NREGIONS_INC_GENERAL];

  TProfile_LW * m_LinksWithAnyErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithBadErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithLnkErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];
  TProfile_LW * m_LinksWithRODErrorsVsLBLayer[NREGIONS_INC_GENERAL][SCT_Monitoring::N_DISKSx2];

  TProfile_LW * m_LinksWithRODErrorsVsLB_check[NREGIONS_INC_GENERAL];

  TH1F_LW * m_NumberOfSCTFlagErrorsVsLB;
  TH1F_LW * m_NumberOfEventsVsLB;
  TProfile_LW * m_FractionOfSCTFlagErrorsPerLB;

  TProfile * m_ConfEffOnline;
  TProfile_LW * m_ConfNoiseOnline;
  TProfile_LW * m_ConfNoiseOnlineRecent;
  TProfile_LW * m_DetailedConfiguration;


  /** a handle on the Hist/TTree registration service */
  ServiceHandle<ITHistSvc> m_thistSvc;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_byteStreamErrSvc;
  //  ServiceHandle<IInDetConditionsSvc>       m_pSummarySvc;
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;
  bool                                     m_CoverageCheck;

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
    prof2Factory(const std::string & name, const std::string & title, const unsigned int&, VecProf2_t & storageVector);

  bool SyncDisabledSCT();
  bool SyncErrorSCT();

  void fillModule( moduleGeo_t module,  TH2F* histo );
  double calculateDetectorCoverage(const TH2F * histo );

  const InDetDD::SCT_DetectorManager * m_sctManager;

  geoContainerPure_t m_disabledGeoSCT;
  geoContainer_t m_errorGeoSCT;

  TH2F * m_disabledModulesMapSCT;
  TH2F * m_errorModulesMapSCT;
  TH2F * m_totalModulesMapSCT;

  const unsigned int m_nBinsEta;
  const double 		 m_rangeEta;
  const unsigned int m_nBinsPhi;
  const double m_ModulesThreshold;

  //TProfile * m_DisabledDetectorCoverageVsLB;
  //TProfile * m_ErrorDetectorCoverageVsLB;
  TProfile * m_TotalDetectorCoverageVsLB;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;
};

#endif
