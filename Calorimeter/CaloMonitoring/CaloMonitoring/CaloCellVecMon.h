/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// AUTHORS:   R. Kehoe    06/15/2006
//            S.Chekanov (TileCal),  08/04/2008 
// MANAGER:   Jun Guo, L. Sawyer   
// MODIFICATIONS: F Spano' (LArg)  Dec2006 -Feb 2007  -Aug 2007 - Feb 2008 - March 2008 - Nov 2008 
//                E Wulf (LArg) Sep-Nov 2008 Feb-Apr 2009
//                D Urbaniec (LArg) Jan-Feb 2009
//                S.Chekanov (Noise histograms for TileCal and LArg), Apr. 2009 
//                S.Chekanov. Corrected binning for TileCal, May 2009
//		  F TIAN (LArg) Jun 2010 - May 2011
//		  D Hu  Jun 2011 - May 2012
//                K ALGhadeer August 2012/2013 add 1D CellEnergyVsTime histograms with diffrent energy thresholds 
//                L.Sawyer - Dec 2013: Modified to conform to new AthenaMonitoring base class. Made histograms MANAGED.
// ********************************************************************


#ifndef CALOCELLVECMON_H
#define CALOCELLVECMON_H

#include "CaloMonitoring/CaloMonToolBase.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "Identifier/IdentifierHash.h"

#include "AthenaMonitoring/IDQFilterTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "CaloMonitoring/CaloLayer.h"

#include <vector>
#include <string>
#include <cstring>
#include <ostream>
#include <math.h>
#include <map>

#include "LWHists/TH2F_LW.h"
#include "TProfile2D.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TTimeStamp.h"
#include "TMath.h"

//Forward Declared Classes:
class ILArBadChanTool;
class ILArBadChannelMasker;
class ICalorimeterNoiseTool;
namespace Trig {
  class TrigDecisionTool;
  class ChainGroup;
}

class CaloCell;
class ComTime;

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class TileID;
class CaloCell_ID;

class LWHist;
//class TH1F_LW;
class TH2F_LW;
class TH1F;
class TH1D;
class TH2F;
class TProfile2D;
class TProfile;


class CaloCellVecMon : public CaloMonToolBase {
 
 public:

  CaloCellVecMon(const std::string & type, const std::string& name, 
		 const IInterface* parent);
  ~CaloCellVecMon();
  
  virtual StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode procHistograms();  
  virtual StatusCode fillHistograms();
  virtual StatusCode finalHists();

  // Enumerate the layers in order to keep track of them when looping
  enum LayerEnum{EMBPA, EMBPC, EMB1A, EMB1C, EMB2A, EMB2C, EMB3A, EMB3C,
		 HEC0A, HEC0C, HEC1A, HEC1C, HEC2A, HEC2C, HEC3A, HEC3C,
		 EMECPA,EMECPC,EMEC1A,EMEC1C,EMEC2A,EMEC2C,EMEC3A,EMEC3C,
		 FCAL1A,FCAL1C,FCAL2A,FCAL2C,FCAL3A,FCAL3C,MAXLAYER};
  enum LayerEnumNoSides{EMBPNS, EMB1NS, EMB2NS, EMB3NS, HEC0NS, HEC1NS, HEC2NS, HEC3NS,
			EMECPNS,EMEC1NS,EMEC2NS,EMEC3NS,FCAL1NS,FCAL2NS,FCAL3NS,MAXLYRNS};

  // Enumerate Histogram types (Correspond to MonGroups, (or lack thereof for Temporary): 
  enum HistTypes {
    KnownBadChannels,
    DatabaseNoise,
    PercentOccupancy_2d,
    PercentOccupancy_1d,
    Occupancy_2d,
    Occupancy_1d,
    AvgEnergy,
    AvgEnergyNoTh,
    TotalEnergy,
    Noise,
    NoiseVsDB,
    Energy,
    EnergyVsTime,
    AvgTime,
    AvgQuality,
    PoorTimeFraction,
    PoorQualityFraction,
    CosmicsROI,
    MaxHistType,
    Temporary
  };

  // Enumerations of possible threshold directions and trigger types:
  enum Direction{OVER,UNDER,NONE,BOTH}; 
  enum TriggerType{NOTA,RNDM,CALO,MINBIAS,MET,MISC};

 private:
  void initHists();
  void initPointers();
  
  StatusCode retrieveTools();
  void initThreshValues();

  bool checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval);
  void initParam();
  void bookMonGroups(const std::string& TheTrigger, Interval_t theinterval);
  void bookTileHists();
  void bookCaloLayers();
  void bookLarMultThreHists(); 
  void bookSporHists();
  void bookLarNonThreHists();
  void bookSummHists();

  //StatusCode checkFilters(bool& ifPass); 
  //  StatusCode checkBeamBackgroundRemoval();
  void fillTrigPara();
  void fillSporHists(const CaloCell* cell );
  void fillTileHists(const CaloCell* cell );
  void fillLarHists(const CaloCell* cell );
  void maskMissCell();

  StatusCode realProcHistograms();  
  void processTileHists();
  void bookImaskHists(); 
  StatusCode deleteImaskHists(); 
  void setBookSwitch();
  void deleteBookSwitch();
  void maskBadChanHists();
  void bookProcHists();  
  void processHists();
  void processMaskHists();
 // StatusCode deleteProcHists();  

  StatusCode deleteHistograms();
  void deleteMonGroups();
  StatusCode deleteTileHists();
  void deleteCaloLayers(); 
  StatusCode deleteLarMultThreHists();
  StatusCode deleteProcHists(); 
  void deleteLarMultThreHistVectors();
  StatusCode deleteSporHists();
  StatusCode deleteLarNonThreHists();
  StatusCode deleteSummHists();

  void deleteArray();

  // book and delete histograms functions
  template< class THist1D >
    void book1DEtaPhiHists(THist1D** hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle,const char* axisOptions, bool* doHist[],bool* isTemp[]);
  template< class THist1D >
    void book1DEtaPhiHists(THist1D** hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle,const char* axisOptions, bool* doHist[]);

  template< class THist1D >
    void bookLayerEtaPhiHists1D(LayerEnum layerNumber, THist1D** hist[], HistTypes histType,
                                const char* genHistName, const char* genHistTitle,
                                const char* axisOptions, bool* doHist, bool* isTemp = 0);

  template< class THist2D >
    void book2DEtaPhiHists(THist2D** hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle, bool* doHist[], bool* isTemp[]);
  template< class THist2D >
    void book2DEtaPhiHists(THist2D** hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle, bool* doHist[]);

  template< class THist2D >
    void bookLayerEtaPhiHists2D(LayerEnum layerNumber, THist2D** hist[],HistTypes histType,
                                const char* genHistName,const char* genHistTitle, bool* doHist, bool* isTemp = 0);

  template< class THist >
    StatusCode saveAndDeleteHistsInLayers(THist** hist[], HistTypes histType, bool* doHist[], bool* isTemp[]);

  template< class THist >
    StatusCode saveAndDeleteHistsInLayers(THist** hist[], HistTypes histType, bool* doHist[]);


  template< class THist >
    StatusCode saveAndDeleteHistsInLayer(LayerEnum ilyr, THist** hists[], HistTypes histType, bool* doHist, bool* isTemp=0);

  template< class THist >
     StatusCode saveAndDeleteHistFromGroup(THist* &hist, MonGroup* &monGroup, bool if_write);


  template< class THist1D >
    void book1DEtaPhiHists(THist1D* hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle,const char* axisOptions);

  template< class THist1D >
    void bookLayerEtaPhiHists1D(LayerEnum layerNumber, THist1D* hist[], HistTypes histType,
                                const char* genHistName,const char* genHistTitle,
                                const char* axisOptions);

  template< class THist2D >
    void book2DEtaPhiHists(THist2D* hist[],HistTypes histType,const char* genHistName,
                           const char* genHistTitle);
 
 template< class THist2D >
    void bookLayerEtaPhiHists2D(LayerEnum layerNumber, THist2D* hist[],HistTypes histType,
                                const char* genHistName,const char* genHistTitle);

 template< class THist >
    StatusCode saveAndDeleteHistsInLayers(THist* hist[], HistTypes histType);

 template< class THist >
   StatusCode saveAndDeleteHistsInLayer(LayerEnum ilyr, THist* hists[], HistTypes histType);


   // Templated histogram booking methods:
  template< class THist1D >
     THist1D* bookHistogram1D(THist1D* hist, const char* name, const char* title, int nbinsx, const double* xbins,
                              const char* xTitle);

  template< class THist2D >
    THist2D* bookHistogram2D(THist2D* hist, const char* name, const char* title, int nbinsx, const double* xbins,
                             int nbinsy, const double* ybins,const char* xTitle, const char* yTitle);

  TH2F_LW* bookHistogram2D(TH2F_LW* hist, const char* name, const char* title, int nbinsx, const double* xbins,
                           int nbinsy, const double* ybins,const char* xTitle, const char* yTitle);

  // safe delete
  template< class THist >
    void safeDelete(THist** &hists);

  template< class THist >
    void safeDelete(THist* &hist);

  template< class dynArray >
    void safeDeleteArray(dynArray* & array);

  void safeDelete(TH2F_LW* &hist);

  // helpers for processing histograms

  template< class THist2D >
    void maskHistogram( THist2D* hist, TH2F_LW* maskHisto, uint32_t bitsToMask, THist2D* imaskedHist = 0);

  template< class THist2D >
    void maskHistogram( THist2D* hist, TH2F* maskHisto, uint32_t bitsToMask, THist2D* imaskedHist = 0);

  uint32_t buildBitMask(std::vector<std::string> namedProblemsToMask);

  void getRMSdevNoise(TProfile *t1, TProfile *t2, TH1F *t3);

  void unpackTProfile2D(TProfile2D* input,  TH2F* noise, TH2F* average = 0, TH2F* occupancy = 0, TH2F* total = 0, float norm = 1.);

  void makeEtaHist(TH2F* input, TH1F* etaHist);

  void makePhiHist(TH2F* input, TH1F* phiHist);

  void divideHistograms(TH2F* numerator, TH2F* denominator, TH2F* quotient);

  void divideHistograms(TH1F* numerator, TH1F* denominator, TH1F* quotient);

  void divideHistogram(TH2F* numerator, float denominator, TH2F* quotient);

  void divideHistogram(TH1F* numerator, float denominator, TH1F* quotient);

  template< class THist >
    void setCorrectEntries(THist ** hist[], int layer, int tresh);

  void unpackToPercentageDifference(TProfile2D* input, TH2F* baseline, TH2F* output);

  void unpackToPercentageDifference(TProfile2D* input, TH2F* output);

//helpers
  void find_n(std::string partition_name,int & name_flag);
  void adjust_n_1d(TH1F* & );
  void adjust_n_2d(TProfile* & );
  int hashToLayer(CaloMonitoring::CaloLayer::Partition partition, int layerNum, CaloMonitoring::CaloLayer::Side side);

  // CaloCellContainer name 
  std::string m_cellContainerName, m_beamBackgroundKey;
  // Time granularity 
  std::string m_timeGran;

  bool m_isOnline;
  bool m_ifSaveAndDeleteHists;
  bool m_bookProcHists;

  bool m_firstcallofBook; 
  
  bool m_useElectronicNoiseOnly;
  bool m_useTwoGaus;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
  bool m_useNoiseToolGlobal;


  // Trigger Awareness:
  bool m_useTrigger;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!< TDT handle
  std::string m_rndmTriggerNames;
  std::string m_caloTriggerNames;
  std::string m_minBiasTriggerNames;
  std::string m_metTriggerNames;
  std::string m_miscTriggerNames;
  const Trig::ChainGroup * m_rndmTriggerEvent;
  const Trig::ChainGroup * m_caloTriggerEvent;
  const Trig::ChainGroup * m_minBiasTriggerEvent;
  const Trig::ChainGroup * m_metTriggerEvent;
  const Trig::ChainGroup * m_miscTriggerEvent;
  bool m_isRndmTrigDefined;
  bool m_isCaloTrigDefined;
  bool m_isMinBiasTrigDefined;
  bool m_isMetTrigDefined;
  bool m_isMiscTrigDefined;

  // bad channel mask  
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  ToolHandle<ILArBadChanTool> m_badChannelTool;
  bool m_maskKnownBadChannels;
  bool m_maskNoCondChannels;
  bool m_doInverseMasking;
  bool m_doMaskingOnline;
  std::vector<std::string>     m_problemNamesToMask;

  // configuration properties  
  bool m_doEtaLumi;
  bool m_doPhiLumi;

  bool m_doKnownBadChannelsVsEtaPhi;
  bool m_doDBNormalized1DEnergy;
  bool m_doEnergyVsTime;
  bool m_doUnnormalized1DEnergy;
  bool m_useLogarithmicEnergyBinning;
  bool m_useWeightedEffMerging;

  bool m_fillNoThreshNoisePlots;
  int  m_triggersToIncludeInNoisePlots;
  int  m_triggersToExcludeFromNoisePlots;

  //Switch to prevent writeAndDelete from being called if set to false:
  //(Useful for online running, where this is not always desired)
//  bool m_bookProcHists;
  bool m_procimask;
    
  //event and cell counters 
  int m_eventsCounter;
  int * m_nEventsPassingTrigger[MAXLYRNS];
  int * m_nEventsPassingBeamBG[MAXLYRNS];
  bool m_newevent;
  //int m_lb;
  int m_ncells; 

  // tile energy threshold
  double m_tileThreshold;

 //threshold for sporadic plots
  bool m_sporadic_switch;
  float m_threshold;
  float m_threshold_em_S0S1;
  float m_threshold_HECFCALEMS2S3;
  int m_EventNumber_thres; 
  int m_sporadicPlotLimit;
  int m_sporadic_protc;

 // Variables for configuring lar thresholded plots:
 // Used to pass variable sized array through JO
  StringArrayProperty  m_thresholdTypeTemp;
  StringArrayProperty  m_thresholdDirectionTemp;
  StringArrayProperty  m_triggersToIncludeTemp;
  StringArrayProperty  m_triggersToExcludeTemp;

  StringArrayProperty  m_defaultThresholdNames;
  StringArrayProperty  m_defaultThresholdTitles;
  FloatArrayProperty   m_defaultThresholds;
  BooleanArrayProperty m_defaultUseNoiseTool;

  StringArrayProperty  m_thresholdColumnType;

  FloatArrayProperty   m_thresholdsTemp[MAXLYRNS];
  StringArrayProperty  m_threshNamesTemp[MAXLYRNS];
  StringArrayProperty  m_threshTitlesTemp[MAXLYRNS];
  BooleanArrayProperty m_useNoiseToolTemp[MAXLYRNS];

  BooleanArrayProperty m_doPercentageOccupancyTemp;
  BooleanArrayProperty m_doEtaPhiOccupancyTemp;
  BooleanArrayProperty m_doEtaOccupancyTemp;
  BooleanArrayProperty m_doPhiOccupancyTemp;

  BooleanArrayProperty m_doEtaPhiTotalEnergyTemp;
  BooleanArrayProperty m_doEtaPhiAverageEnergyTemp;
  BooleanArrayProperty m_doEtaPhiEnergyRMSTemp;
  BooleanArrayProperty m_doEtaPhiDeviationFromDBnoiseTemp;

  BooleanArrayProperty m_doEtaPhiAverageQualityTemp;
  BooleanArrayProperty m_doEtaPhiFractionOverQthTemp;
  FloatArrayProperty   m_qualityFactorThresholdTemp;

  BooleanArrayProperty m_doEtaPhiAverageTimeTemp;
  BooleanArrayProperty m_doEtaPhiFractionPastTthTemp;
  FloatArrayProperty   m_timeThresholdTemp;

  BooleanArrayProperty m_maskEmptyBinsTemp;
  BooleanArrayProperty m_doBeamBackgroundRemovalTemp; 

  // Actual arrays used
  //std::vector<int> Thresholds;
  int    m_nThresholds[MAXLYRNS];
  float* m_thresholds[MAXLYRNS];
  std::string * m_threshNames[MAXLYRNS];
  std::string * m_threshTitles[MAXLYRNS];
  int*   m_thresholdDirection[MAXLYRNS];
  int*   m_threshTriggersToInclude[MAXLYRNS];
  int*   m_threshTriggersToExclude[MAXLYRNS];
  bool * m_threshTriggerDecision[MAXLYRNS];
  bool*  m_useNoiseTool[MAXLYRNS];

  float* m_qualityThresholds[MAXLYRNS];
  float* m_timeThresholds[MAXLYRNS];
  
  //Switches for deciding which objects are to be made using fillHists & procHists:
  bool* m_maskEmptyBins[MAXLYRNS];
  bool* m_doBeamBackgroundRemoval[MAXLYRNS];
 
  bool* m_doPercentageOccupancy[MAXLYRNS];
  bool* m_doEtaPhiEnergyProfile[MAXLYRNS];
  bool* m_doEtaPhiAverageEnergy[MAXLYRNS]; 
  bool  m_doNActiveCellsFirstEventVsEta;
  bool  m_doNActiveCellsFirstEventVsPhi;
  bool  m_doDatabaseNoiseVsEtaPhi;

  bool* m_fillEtaPhiOccupancy[MAXLYRNS];
  bool* m_tempEtaPhiOccupancy[MAXLYRNS];
  bool* m_procEtaPhiOccupancy[MAXLYRNS];
  bool* m_fillEtaPhiTotalEnergy[MAXLYRNS];
  bool* m_tempEtaPhiTotalEnergy[MAXLYRNS];
  bool* m_procEtaPhiTotalEnergy[MAXLYRNS];

  bool* m_fillEtaOccupancy[MAXLYRNS];
  bool* m_procEtaOccupancy[MAXLYRNS];
  bool* m_fillPhiOccupancy[MAXLYRNS];
  bool* m_procPhiOccupancy[MAXLYRNS];

  bool* m_fillEtaPhiQualityOccupancy[MAXLYRNS];
  bool* m_fillEtaPhiTotalQuality[MAXLYRNS];
  bool* m_fillEtaPhiTimeOccupancy[MAXLYRNS];
  bool* m_fillEtaPhiTotalTime[MAXLYRNS];

  bool* m_procEtaPhiEnergyRMS[MAXLYRNS];
  bool* m_procEtaPhiDeviationFromDBnoise[MAXLYRNS];
  bool* m_procEtaPhiAverageQuality[MAXLYRNS];
  bool* m_procEtaPhiFractionOverQth[MAXLYRNS];
  bool* m_procEtaPhiAverageTime[MAXLYRNS];
  bool* m_procEtaPhiFractionPastTth[MAXLYRNS];

// switches for booking proHists
  bool * m_procPercentOccEtaPhi[MAXLYRNS];
  bool * m_procPercentOccEta[MAXLYRNS];
  bool * m_procPercentOccPhi[MAXLYRNS];
  bool * m_procAbsOccEtaPhi[MAXLYRNS];
  bool * m_procAbsOccEta[MAXLYRNS];
  bool * m_procAbsOccPhi[MAXLYRNS];
  bool * m_doEtaPhiAvgEnNoTh[MAXLYRNS];
  bool * m_doEtaPhiAvgEnTh[MAXLYRNS];


  // Services
  const LArOnlineID* m_LArOnlineIDHelper;
  ToolHandle<LArCablingService> m_LArCablingService;
  const CaloIdManager*       m_caloIdMgr;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;

  const TileID* m_tile_id;
  const CaloCell_ID* m_calo_id;
  const LArEM_ID* m_em_id;
  
 // MonGroups
  
  MonGroup * m_summaryGroup;
  MonGroup * m_tile_cells_shift;

  MonGroup * m_tempMonGroup;
  MonGroup * m_shifterMonGroup[MaxHistType];
  //MonGroup * ExpertMonGroup[MaxHistType];

  MonGroup * m_badCell;
  MonGroup * m_badCell_EMBC;
  MonGroup * m_badCell_EMBA;
  MonGroup * m_badCell_EMECC;
  MonGroup * m_badCell_EMECA;
  MonGroup * m_badCell_HECC;
  MonGroup * m_badCell_HECA;
  MonGroup * m_badCell_FCALC;
  MonGroup * m_badCell_FCALA;
  
  // Layers
  CaloMonitoring::CaloLayer *   m_layer[MAXLAYER];

  //Sporadic monitoring related variables 
  std::vector<IdentifierHash> m_monitored;
  std::vector<Int_t> m_occ;
  std::vector<bool> m_hasAdded;
  std::map<IdentifierHash, long> m_listOfNoiseBurstLumiblock;

  int m_counter_sporadic_protc;

  int m_nbh_embc;
  int m_nbh_emba;
  int m_nbh_emecc;
  int m_nbh_emeca;
  int m_nbh_hecc;
  int m_nbh_heca;
  int m_nbh_fcalc;
  int m_nbh_fcala;
        
  bool m_badCell_flag;
  bool m_badCell_flag_embc;
  bool m_badCell_flag_emba;
  bool m_badCell_flag_emecc;
  bool m_badCell_flag_emeca;
  bool m_badCell_flag_hecc;
  bool m_badCell_flag_heca;
  bool m_badCell_flag_fcalc;
  bool m_badCell_flag_fcala;

 // Sporadic histograms
  TH1F* m_h_NoiseBurstLumiblock;
  TH1F * m_h_sporad;
  TH1F * m_h_sporad_embc;
  TH1F * m_h_sporad_emba;
  TH1F * m_h_sporad_emecc;
  TH1F * m_h_sporad_emeca;
  TH1F * m_h_sporad_hecc;
  TH1F * m_h_sporad_heca;
  TH1F * m_h_sporad_fcalc;
  TH1F * m_h_sporad_fcala;

  std::vector<TH1F*> m_h_energy_s;
  std::vector<TProfile*> m_h_energy_lb;
  std::vector<TH1F*> m_h_quality;    

 //   Tile histograms parameters 
  std::vector<double> m_b_Ncells;

  std::vector<double> m_b_CellsE;

  std::vector<double> m_b_CellsEta;
  std::vector<double> m_b_CellsEtaTile;

  std::vector<double> m_b_CellsPhi;

  std::vector<double> m_b_CellsTower;
  std::vector<double> m_b_CellsModule;

  std::vector<double> m_b_CellsEtaPhi;

  // Tile histograms
  TH1F* m_h_CellsNoiseE;  // E/DBnoise distribution for all not bad tile cells in sample <3

  // for tile cells with energy > m_tileThreshold 
  TH1F* m_h_Ncells;
  TH1F* m_h_CellsE;  
  TH2F* m_h_CellsEtaPhi;
  TProfile*  m_h_CellsEta;
  TProfile*  m_h_CellsPhi;
  TProfile* m_h_CellsModuleS1;
  TProfile* m_h_CellsModuleS2;
  TProfile* m_h_CellsModuleS3;
  TProfile* m_h_CellsModuleS4;
  TProfile* m_h_CellsTower;

  // for not bad tile cells with |E/DBnoise| >4.0 in sample<3
  TH2F* m_h_CellsNoiseEtaPhi;
  TH2F* m_h_CellsNoiseEtaPhiA;
  TH2F* m_h_CellsNoiseEtaPhiBC;
  TH2F* m_h_CellsNoiseEtaPhiD;
  TH1F* m_h_CellsNoiseHash;

  // for all not bad tile cells in sample <3; h_CellsRMSdivDBnoiseEta(Phi) are derived  from temporary plots h_CellsNoiseEta(phi) and h_CellsRMSEta(Phi) 
  TProfile*  m_h_CellsNoiseEta;
  TProfile*  m_h_CellsNoisePhi;
  TProfile*  m_h_CellsRMSPhi;
  TProfile*  m_h_CellsRMSEta;
  TH1F * m_h_CellsRMSdivDBnoiseEta;
  TH1F * m_h_CellsRMSdivDBnoisePhi;

  // calo histograms

  // summary plot
  TH1F* m_h_n_trigEvent;
  TH1F* m_h_EvtRejSumm; // km add

  // monitoring cell with energy > 4*DBnoise
  TH2F * m_h_cellOccupancyEtaLumi[MAXLAYER];
  TH2F * m_h_cellOccupancyPhiLumi[MAXLAYER];
  TProfile2D *m_h_cellAvgEnergyEtaLumi[MAXLAYER];
  TProfile2D *m_h_cellAvgEnergyPhiLumi[MAXLAYER];

  // energy and time distribution for cell neither badcell nor noconditions 
  TH1F*         m_h_energy[MAXLAYER];  // define when m_doUnnormalized1DEnergy==1
  TH1D*         m_h_dbNoiseNormalizedEnergy[MAXLAYER]; // define when m_doDBNormalized1DEnergy==1
  TH2F*         m_h_energyVsTime[MAXLAYER];
  TH1F*         m_h_energyVsTime_DifferThresholds[MAXLAYER];

  TH1F*         m_h_energy_imask[MAXLAYER];
  TH2F*         m_h_energyVsTime_imask[MAXLAYER];

  // occupancy, energy, quality and time plots for several different thresholds for each layer 
 
  TH2F**        m_h_occupancy_etaphi[MAXLAYER];
  TH1F**        m_h_occupancy_eta[MAXLAYER];
  TH1F**        m_h_occupancy_phi[MAXLAYER];

  TH2F**        m_h_occupancy_etaphi_imask[MAXLAYER];
  TH1F**        m_h_occupancy_eta_imask[MAXLAYER];
  TH1F**        m_h_occupancy_phi_imask[MAXLAYER];

  TH2F**        m_h_percentOccupancy_etaphi[MAXLAYER];
  TH1F**        m_h_percentOccupancy_eta[MAXLAYER];
  TH1F**        m_h_percentOccupancy_phi[MAXLAYER];

  TH2F**        m_h_percentOccupancy_etaphi_imask[MAXLAYER];
  TH1F**        m_h_percentOccupancy_eta_imask[MAXLAYER];
  TH1F**        m_h_percentOccupancy_phi_imask[MAXLAYER];

  TProfile2D**  m_h_energyProfile_etaphi[MAXLAYER];
  TH2F**        m_h_totalEnergy_etaphi[MAXLAYER];
  TH2F**        m_h_averageEnergy_etaphi[MAXLAYER];
  TH2F**        m_h_noise_etaphi[MAXLAYER];
  TH2F**        m_h_deviationFromDBnoise_etaphi[MAXLAYER];

  TProfile2D**  m_h_energyProfile_etaphi_imask[MAXLAYER];
  TH2F**        m_h_totalEnergy_etaphi_imask[MAXLAYER];
  TH2F**        m_h_averageEnergy_etaphi_imask[MAXLAYER];
  TH2F**        m_h_noise_etaphi_imask[MAXLAYER];

  TH2F**        m_h_poorQualityOccupancy_etaphi[MAXLAYER];
  TH2F**        m_h_totalQuality_etaphi[MAXLAYER];
  TH2F**        m_h_fractionOverQth_etaphi[MAXLAYER];
  TH2F**        m_h_averageQuality_etaphi[MAXLAYER];

  TH2F**        m_h_poorTimeOccupancy_etaphi[MAXLAYER];
  TH2F**        m_h_totalTime_etaphi[MAXLAYER];
  TH2F**        m_h_fractionPastTth_etaphi[MAXLAYER];
  TH2F**        m_h_averageTime_etaphi[MAXLAYER];

  // Reference Type Histograms (Filled solely with database information)
//  TH2F_LW*      h_badChannels_etaphi[MAXLAYER];
  TH2F*         m_h_badChannels_etaphi[MAXLAYER];
  TH2F*         m_h_dbnoise_etaphi[MAXLAYER];
  TH2F*         m_h_missingCellMask[MAXLAYER];
    
  // temporary plots used for m_percentOccupancy_eta(phi) 
  TH1F*         m_h_nActiveCellsFirstEvent_eta[MAXLAYER]; 
  TH1F*         m_h_nActiveCellsFirstEvent_phi[MAXLAYER];
};

#endif // CaloCellVecMon_H: R. Kehoe 07/5/06
