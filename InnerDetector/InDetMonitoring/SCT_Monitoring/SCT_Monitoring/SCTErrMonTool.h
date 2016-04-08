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

//for CondDB
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h" 
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

/** Forward declarations*/
class IInterface;
class TH1F;
class TH2F;
class TH1I;
class TH2I;
class TProfile;
class TProfile2D;
class StatusCode;
class SCT_ID;
class SCT_ModuleStatistics;
class ISCT_ByteStreamErrorsSvc;
class TString;

///Concrete monitoring tool derived from MonitorToolBase
class SCTErrMonTool : public ManagedMonitorToolBase
{
  public:
	  SCTErrMonTool(const std::string & type,const std::string & name,const IInterface* parent);
	  virtual ~SCTErrMonTool();
	  //
	  /**    @name Methods reimplemented from baseclass */
	  //@{
	  //book
	  virtual StatusCode bookHistograms() final;
	  //fill
	  virtual StatusCode fillHistograms() final;
	  //post processing
	  virtual StatusCode procHistograms() final;
	  //@}

  private:
	  static const int NREGIONS_INC_GENERAL=SCT_Monitoring::N_REGIONS+1;
	  typedef TProfile2D * Prof2_t;
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
  TH1F * m_firstHit;
  TH1F * m_firstHit_ECp;
  TH1F * m_firstHit_ECm;
  TH1F * m_secondHit;
  TH1F * m_secondHit_ECp;
  TH1F * m_secondHit_ECm;
  //@}
  
  enum ErrorTypes {ABCD, RAW,TIMEOUT, LVL1ID,BCID, PREAMBLE, FORMATTER, MASKEDLINKS, RODCLOCK, TRUNCATEDROD, ROBFRAG, BSPARSE, MISSINGLINK, SUMMARY, N_ERRTYPES};
  
  ///rate of errors
  TProfile2D* m_allErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///rate of errors per lumi-block
  TProfile2D* m_allErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F* m_pallErrs[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  ///total number of errors
  TH2F* m_pallErrsPerLumi[N_ERRTYPES][NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Recent error rate histograms
  TProfile2D* m_summaryErrsRecent[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2];
  /// Default histos to print per lumi block
  TH2F* m_numErrorsPerLumi[NREGIONS_INC_GENERAL];
  TProfile2D* m_rateErrorsPerLumi[NREGIONS_INC_GENERAL];

  TH1I *m_nErrors;
  TH1I *m_nLinksWithErrors;
  int *nErrors_buf;
  int *nLinksWithErrors_buf;
  int nErrors_pos;

  TH1I *m_MaskedLinks;

  //Count number of events
  int m_numberOfEventsLumi;
  int m_numberOfEvents;

  bool m_initialize;
  //max number of errors in lbs                                                                          
  unsigned int m_previous_lb;
  int m_maskedLinkErrsMax[4];
  int m_robFragmentErrsMax[4];
  int m_abcdErrsMax[4];
  int m_rawErrsMax[4];
  int m_timeoutErrsMax[4];
  int m_lvl1idErrsMax[4];
  int m_bcidErrsMax[4];
  int m_preambleErrsMax[4];
  int m_formatterErrsMax[4];
  int m_rodClockErrsMax[4];
  int m_truncRodErrsMax[4];
  int m_bsParseErrsMax[4];
  int m_misslinkErrsMax[4];
  int m_totErrsMax[4];
  int m_totModErrsMax[4];
  // Book noise map histograms
  StatusCode bookConfMaps();
  StatusCode bookPositiveEndCapConfMaps();
  StatusCode bookNegativeEndCapConfMaps();

  int m_nLink0[4088];
  int m_nLink1[4088];

  bool m_goodModules[4088];
  VecProf2_t m_pnoiseoccupancymapHistoVectorECC;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECCSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBar;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorBarSide1;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECA;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide0;
  VecProf2_t m_pnoiseoccupancymapHistoVectorECASide1;

  bool m_noSidesHit;
  bool m_oneSideHit;
  int nZero[4088];
  int nOne[4088];
  int nOneSide0[4088];
  int nOneSide1[4088];
  int nLayer[4088];
  int nEta[4088];
  int nPhi[4088];
  int nNonGoodModule[4088];

  int tbin;
  int modNum;
  float ratio;
  float ratioside0;
  float ratioside1;

  int fillByteStreamErrorsHelper(const std::set<IdentifierHash>* errors, TH2F *histo[NREGIONS_INC_GENERAL][SCT_Monitoring::N_ENDCAPSx2], bool lumiHist, int err_type, bool b_maskedlinks);
  void numByteStreamErrors(const std::set<IdentifierHash>* errors, int& ntot, int& nbar, int& neca, int& necc);
  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TString titlehitmap, TProfile2D* &tprof, TH2F* &th, const int layer, const bool barrel=true);

  StatusCode bookErrHistosHelper(MonGroup & mg, TString name, TString title, TProfile2D* &tprof, const int layer, const bool barrel=true);

  std::vector<TH2F *> m_p2DmapHistoVectorAll[NREGIONS_INC_GENERAL];

  /// "Magic numbers" for an SCT module
  //unsigned int m_nplanes; //to be determined from SCT Helper
  enum { Confbins = 6, ConfbinsDetailed = 5,n_lumiErrBins = 12, n_lumiBins = 3000 };
  // NOTE: The following is not the usual index order, which goes C, Barrel, A
  enum { iBARREL = 0, iECp = 1, iECm=2, iGEN=3 }; //iECp==EA, iECm==EC

  std::string m_stream;
  std::string m_path;
  /// CheckHists() frequency
  int m_checkrate;
  int m_checkrecent;
  int m_current_lb;
  int m_last_reset_lb;

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
  std::string m_dataObjectName;

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

  /// Pointer to 1D histogram of Number of SCT Clusters per Even
  TProfile * m_Conf[4];
  TProfile * m_ConfRN[4];
  // TProfile * m_ConfOnline[4];
  TH1F     * m_ConfOnline[4];
  TProfile * m_MaskedLinksVsLB[4];
  TProfile * m_ROBFragmentVsLB[4];
  TProfile * m_ABCDVsLB[4];
  TProfile * m_RawErrsVsLB[4];
  TProfile * m_TimeOutVsLB[4];
  TProfile * m_LVL1IDVsLB[4];
  TProfile * m_BCIDVsLB[4];
  TProfile * m_PreambleVsLB[4];
  TProfile * m_FormatterVsLB[4];
  TProfile * m_RODClockVsLB[4];
  TProfile * m_TruncRODVsLB[4];
  TProfile * m_BSParseVsLB[4];
  TProfile * m_MissingLinkHeaderVsLB[4];

  TH1F * m_MaxMaskedLinksVsLB[4];
  TH1F * m_MaxROBFragmentVsLB[4];
  TH1F * m_MaxABCDVsLB[4];
  TH1F * m_MaxRawErrsVsLB[4];
  TH1F * m_MaxTimeOutVsLB[4];
  TH1F * m_MaxLVL1IDVsLB[4];
  TH1F * m_MaxBCIDVsLB[4];
  TH1F * m_MaxPreambleVsLB[4];
  TH1F * m_MaxFormatterVsLB[4];
  TH1F * m_MaxRODClockVsLB[4];
  TH1F * m_MaxTruncRODVsLB[4];
  TH1F * m_MaxBSParseVsLB[4];
  TH1F * m_MaxMissingLinkHeaderVsLB[4];

  // TH2I * m_2d_MaxMaskedLinksVsLB[4];
  // TH2I * m_2d_MaxROBFragmentVsLB[4];
  // TH2I * m_2d_MaxABCDVsLB[4];
  // TH2I * m_2d_MaxRawErrsVsLB[4];
  // TH2I * m_2d_MaxTimeOutVsLB[4];
  // TH2I * m_2d_MaxLVL1IDVsLB[4];
  // TH2I * m_2d_MaxBCIDVsLB[4];
  // TH2I * m_2d_MaxPreambleVsLB[4];
  // TH2I * m_2d_MaxFormatterVsLB[4];
  // TH2I * m_2d_MaxRODClockVsLB[4];
  // TH2I * m_2d_MaxTruncRODVsLB[4];
  // TH2I * m_2d_MaxBSParseVsLB[4];
  // TH2I * m_2d_MaxMissingLinkHeaderVsLB[4];

  TProfile * m_NumberOfErrorsVsLB[4];
  TProfile * m_ModulesWithErrorsVsLB[4];
  TProfile * m_LinksWithErrorsVsLBBarrel[4];
  TProfile * m_LinksWithErrorsVsLBEndcapA[9];
  TProfile * m_LinksWithErrorsVsLBEndcapC[9]; 

  TH1F * m_MaxNumberOfErrorsVsLB[4];
  TH1F * m_MaxModulesWithErrorsVsLB[4];
  // TH2I * m_2d_MaxNumberOfErrorsVsLB[4];
  // TH2I * m_2d_MaxModulesWithErrorsVsLB[4];

  TProfile * m_ConfEffOnline;
  TProfile * m_ConfNoiseOnline;
  TProfile * m_ConfNoiseOnlineRecent;
  TProfile * m_DetailedConfiguration;

  /** a handle on the Hist/TTree registration service */
  ServiceHandle<ITHistSvc> m_thistSvc;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_byteStreamErrSvc;
  //  ServiceHandle<IInDetConditionsSvc>       m_pSummarySvc;
  bool                                     m_checkBadModules;
  bool                                     m_ignore_RDO_cut_online;

  float m_errThreshold;
  float m_effThreshold;
  int m_noiseThreshold;
  bool getHisto(const int lyr, const int reg, const int type, TH2* histo[2]);
  bool getHistoRecent(const int lyr, const int reg, const int type, TH2* histo[2]);
  float calculateNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  float calculateOneSideNoiseOccupancyUsingRatioMethod(const float numberOneSide, const float numberZeroSide);
  bool isBarrel(const int moduleNumber);
  bool isEndcap(const int moduleNumber);
  bool isEndcapA(const int moduleNumber);
  bool isEndcapC(const int moduleNumber);
  //@}

  Prof2_t
    prof2Factory(const std::string & name, const std::string & title, const unsigned int&, VecProf2_t & storageVector);
};

#endif
