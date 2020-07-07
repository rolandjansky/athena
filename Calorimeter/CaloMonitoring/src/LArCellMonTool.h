//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


//W.Lampl  - Spring 2017: Major re-design

#ifndef CALOMONITORING_LARCELLMONTOOL_H
#define CALOMONITORING_LARCELLMONTOOL_H

#include "CaloMonToolBase.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRecConditions/ILArBadChannelMasker.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "LArIdentifier/LArOnlineID.h"
#include "Identifier/IdentifierHash.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "LArCellBinning.h"
          
#include <vector>
#include <string>
#include <bitset>
#include <array>
#include <map>
#include <unordered_map>
 
namespace Trig {
  class ChainGroup;
}

class CaloCell;
class TileID;
class CaloCell_ID;

class TH1F;
class TH1D;
class TH2F;
class TProfile;


class LArCellMonTool : public CaloMonToolBase {
 
 public:

  LArCellMonTool(const std::string & type, const std::string& name, 
		 const IInterface* parent);
  ~LArCellMonTool();
  
  //Methods implementing the ManagedMonitorToolBase interface
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistograms();
  virtual StatusCode procHistograms();
  virtual StatusCode fillHistograms();

private:

  //enums to help with the conversion of Layer, partitions and such:

  //Enumerate layers 
  enum LayerEnum{EMBPA=0, EMBPC, EMB1A, EMB1C, EMB2A, EMB2C, EMB3A, EMB3C,
		 HEC0A, HEC0C, HEC1A, HEC1C, HEC2A, HEC2C, HEC3A, HEC3C,
		 EMECPA,EMECPC,EMEC1A,EMEC1C,EMEC2A,EMEC2C,EMEC3A,EMEC3C,
		 FCAL1A,FCAL1C,FCAL2A,FCAL2C,FCAL3A,FCAL3C,MAXLAYER};

  //Enumerate layer-types, ignoring sides. Useful for configuration that is per-definition symmetric 
  enum LayerEnumNoSides{EMBPNS=0, EMB1NS, EMB2NS, EMB3NS, HEC0NS, HEC1NS, HEC2NS, HEC3NS,
			EMECPNS,EMEC1NS,EMEC2NS,EMEC3NS,FCAL1NS,FCAL2NS,FCAL3NS,MAXLYRNS};

  //Enumerate partitions
  enum PartitionEnum{EMBA,EMBC,EMECA,EMECC,HECA,HECC,FCALA,FCALC,MAXPARTITIONS};


  //Mapping of layer-enum to their string-names (for histogram titles, etc)
  const std::array<std::string,MAXLAYER> m_layerNames={{"EMBPA", "EMBPC", "EMB1A", "EMB1C", "EMB2A", "EMB2C", "EMB3A", "EMB3C",
							"HEC0A", "HEC0C", "HEC1A", "HEC1C", "HEC2A", "HEC2C", "HEC3A", "HEC3C",
							"EMECPA", "EMECPC", "EMEC1A", "EMEC1C", "EMEC2A", "EMEC2C", "EMEC3A", "EMEC3C", 
							"FCAL1A", "FCAL1C", "FCAL2A", "FCAL2C", "FCAL3A", "FCAL3C"}};

  //Mapping of layer-no-side enum to their string-names (for histogram titles, etc)
  const std::array<std::string,MAXLYRNS> m_lyrNSNames={{"EMBPS","EMB1" ,"EMB2" ,"EMB3" ,"HEC0" ,"HEC1" ,"HEC2","HEC3",
							"EMECP","EMEC1","EMEC2","EMEC3","FCAL1","FCAL2","FCAL3"}};
  //Mapping of partition-enum to their string-name
  const std::array<std::string,MAXPARTITIONS> m_partitionNames{{"EMBA","EMBC","EMECA","EMECC","HECA","HECC","FCALA","FCALC"}};  

  //Mapping of CaloCell nomencature to CaloCellMonitoring nomencature
  const std::map<unsigned,LayerEnumNoSides> m_caloSamplingToLyrNS{ 
    {CaloSampling::PreSamplerB, EMBPNS},{CaloSampling::EMB1,EMB1NS},{CaloSampling::EMB2,EMB2NS},{CaloSampling::EMB3,EMB3NS},         //LAr Barrel
    {CaloSampling::PreSamplerE, EMECPNS},{CaloSampling::EME1,EMEC1NS}, {CaloSampling::EME2,EMEC2NS}, {CaloSampling::EME3,EMEC3NS},   //LAr Endcap     
    {CaloSampling::HEC0,HEC0NS}, {CaloSampling::HEC1,HEC1NS}, {CaloSampling::HEC2,HEC2NS}, {CaloSampling::HEC3,HEC3NS},              //Hadronic endcap
    {CaloSampling::FCAL0,FCAL1NS}, {CaloSampling::FCAL1,FCAL2NS}, {CaloSampling::FCAL2,FCAL3NS}                                      //FCAL
  };

  //Mapping of layers to the partition the layer belongs to
  const std::array<PartitionEnum,MAXLAYER> m_layerEnumtoPartitionEnum{{
    //EMBPA EMBPC, EMB1A, EMB1C, EMB2A, EMB2C, EMB3A, EMB3C,
      EMBA, EMBC,  EMBA,  EMBC,  EMBA,  EMBC,  EMBA,  EMBC,
   //HEC0A, HEC0C, HEC1A, HEC1C, HEC2A, HEC2C, HEC3A, HEC3C,
      HECA, HECC,  HECA,  HECC,  HECA,  HECC,  HECA,   HECC,
  //EMECPA,  EMECPC,EMEC1A,EMEC1C,EMEC2A,EMEC2C,EMEC3A,EMEC3C,
      EMECA, EMECC, EMECA, EMECC, EMECA, EMECC, EMECA, EMECC,
   //FCAL1A,FCAL1C,FCAL2A,FCAL2C, FCAL3A,FCAL3C,MAXLAYER
      FCALA, FCALC, FCALA, FCALC, FCALA, FCALC
	}};
  
  //Enumerations of possible threshold directions and trigger types:
  enum Direction{OVER,UNDER,NONE,BOTH}; 
  enum TriggerType{RNDM,CALO,MINBIAS,MET,MISC,NOTA,MAXTRIGTYPE};


  //Histogram path in root file:
  const std::string m_lArPath{"/CaloMonitoring/LArCellMon_NoTrigSel/"};

  // Thresholds for time and Time vs Energy plots:
  // Energy thresholds hardcoded following official timing analysis. See for example:
  // https://indico.cern.ch/event/522351/
  const std::array<float ,MAXLYRNS> m_eCutForTiming {
  //EMBPNS=0, EMB1NS, EMB2NS, EMB3NS, HEC0NS, HEC1NS, HEC2NS, HEC3NS,EMECPNS,EMEC1NS,EMEC2NS,EMEC3NS,FCAL1NS,FCAL2NS,FCAL3NS
    {1000.,    1000.,  3000.,  1500.,  3500.,  3500.,  3500.,  3500., 1500.,  3000.,  3000.,  2000.,  10000., 10000., 10000.}
  };


  //Private methods: Initialization and job-option interpretation
  StatusCode initThreshHists();
  void setLArCellBinning();
  void resetInternals();

  //Private methods: Histogramm booking
  StatusCode bookLarMultThreHists(); 
  StatusCode bookSporHists();
  StatusCode bookLarNonThreHists();
  TH2F* newEtaPhiHist(const std::string& hName, const std::string& hTitle, const CaloMonitoring::LArCellBinning& binning) const;
  std::string strToLower(const std::string& input) const;
  void regTempHist(TH1* h, MonGroup& mg);

  //Private methods: Histogram filling
  StatusCode createPerJobHistograms(const CaloCellContainer* cellcont);
  void checkTriggerAndBeamBackground();
  void sporadicNoiseCandidate(const CaloCell* cell, const LArCellMonTool::LayerEnum iLyr,const float threshold, const LArOnOffIdMapping* cabling);

  //Helpers for histogram filling
  void getHistoCoordinates(const CaloDetDescrElement* dde, float& celleta, float& cellphi, unsigned& iLyr, unsigned& iLyrNS) const; 
  void divideByOccupancy(TH2F* fraction, const TH2F* total, const TH2* occupancy) ;


  //Job Properties and other private variables
  bool m_oncePerJobHistosDone=false;
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerName { this, "CaloCellContainer", "AllCalo", "SG key of the input cell container" };

  bool m_useElectronicNoiseOnly;
  ICalorimeterNoiseTool::CalorimeterNoiseType m_noiseType=ICalorimeterNoiseTool::TOTALNOISE;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;


  // Trigger Awareness:
  bool m_useTrigger;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!< TDT handle
  std::array<std::string,NOTA> m_triggerNames; 
  std::array<const Trig::ChainGroup*, NOTA> m_chainGroups{{}};
 
  // bad channel mask  
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  bool m_maskKnownBadChannels;
  bool m_maskNoCondChannels;

  bool m_doSaveTempHists;
  bool m_doKnownBadChannelsVsEtaPhi;
  bool m_doDatabaseNoiseVsEtaPhi;
  bool m_doEnergyVsTime;
  bool m_doUnnormalized1DEnergy;
  bool m_useLogarithmicEnergyBinning;
  bool m_useWeightedEffMerging;
    
 //threshold for sporadic plots
  bool m_sporadic_switch;
  float m_threshold_em_S0S1;
  float m_threshold_HECFCALEMS2S3;
  unsigned m_sporadicPlotLimit;
  unsigned m_sporadic_protc;
  unsigned m_minSporadicNoiseEventsPerCell;

 // Variables to configure LAr threshold plots:
 // Used to pass variable sized array through JO
  StringArrayProperty  m_thresholdNameProp;
  StringArrayProperty  m_thresholdDirectionProp;
  StringArrayProperty  m_triggersToIncludeProp;
  StringArrayProperty  m_triggersToExcludeProp;
  StringArrayProperty  m_thresholdTitleTemplates;
  FloatArrayProperty   m_defaultThresholds;
  BooleanArrayProperty m_inSigNoise;
  BooleanArrayProperty m_doPercentageOccupancyProp;
  BooleanArrayProperty m_doEtaPhiOccupancyProp;
  BooleanArrayProperty m_doEtaOccupancyProp;
  BooleanArrayProperty m_doPhiOccupancyProp;
  BooleanArrayProperty m_doEtaPhiTotalEnergyProp;
  BooleanArrayProperty m_doEtaPhiAverageEnergyProp;
  BooleanArrayProperty m_doEtaPhiAverageQualityProp;
  BooleanArrayProperty m_doEtaPhiFractionOverQthProp;
  FloatArrayProperty   m_qualityFactorThresholdProp;
  BooleanArrayProperty m_doEtaPhiAverageTimeProp;
  BooleanArrayProperty m_doEtaPhiFractionPastTthProp;
  FloatArrayProperty   m_timeThresholdProp;
  BooleanArrayProperty m_doBeamBackgroundRemovalProp; 

  StringArrayProperty  m_thresholdColumnType;
  FloatArrayProperty   m_thresholdsProp[MAXLYRNS];

  //The threshold-related variables and histograms are grouped in the following struct
  //one instance per threshold-type
  struct thresholdHist_t {
    //Configuration variables
    std::string m_threshName;                        /// Name of this threshold
    std::string m_threshTitleTemplate;               /// Histogram title template
    std::array<float,MAXLYRNS> m_threshValue{{}};    /// The actual threshold (per layer)
    std::array<std::string,MAXLYRNS> m_threshTitles; /// Part of the histogram title (containing the threshold value, so per-layer)
    bool m_inSigNoise=false;                         /// Absolute threshold or in sigma noise?
    Direction m_threshDirection=OVER;
    bool m_doBeamBackgroundRemoval=false; 
    float m_qualityFactorThreshold=4000;
    float m_timeThreshold=4.;

    //Variables related to trigger-filtering
    std::bitset<MAXTRIGTYPE> m_triggersToInclude;
    std::bitset<MAXTRIGTYPE> m_triggersToExclude;
    bool m_threshTriggerDecision=true;

    //Switch histograms on/off
    bool m_doPercentageOccupancy=false;
    bool m_doEtaPhiOccupancy=false;
    bool m_doEtaOccupancy=false;
    bool m_doPhiOccupancy=false;

    bool m_doEtaPhiTotalEnergy=false;
    bool m_doEtaPhiAverageEnergy=false;

    bool m_doEtaPhiAverageQuality=false;
    bool m_doEtaPhiFractionOverQth=false;
    
    bool m_doEtaPhiAverageTime=false;
    bool m_doEtaPhiFractionPastTth=false;

    //Histogram pointers, per layer
    std::array<TH2F*,MAXLAYER> m_h_occupancy_etaphi{{}};  //Filled from  m_occupancyCounter
    std::array<TH1F*,MAXLAYER> m_h_occupancy_eta{{}};     //Filled from  m_occupancyCounter
    std::array<TH1F*,MAXLAYER> m_h_occupancy_phi{{}};     //Filled from  m_occupancyCounter
    
    std::array<TH2F*,MAXLAYER> m_h_totalEnergy_etaphi{{}}; //Filled directly
    std::array<TH2F*,MAXLAYER> m_h_averageEnergy_etaphi{{}}; //derived
    
    std::array<TH2F*,MAXLAYER> m_h_poorQualityOccupancy_etaphi{{}}; //Filled directly
    std::array<TH2F*,MAXLAYER> m_h_totalQuality_etaphi{{}}; //Filled directly
    std::array<TH2F*,MAXLAYER> m_h_fractionOverQth_etaphi{{}}; //derived
    std::array<TH2F*,MAXLAYER> m_h_averageQuality_etaphi{{}};//derived

    std::array<TH2F*,MAXLAYER> m_h_poorTimeOccupancy_etaphi{{}}; //Filled directly
    std::array<TH2F*,MAXLAYER> m_h_totalTime_etaphi{{}}; //Filled directly
    std::array<TH2F*,MAXLAYER> m_h_fractionPastTth_etaphi{{}}; //derived
    std::array<TH2F*,MAXLAYER> m_h_averageTime_etaphi{{}}; //derived

    //Occupancy-counter
    std::array<unsigned,185000> m_occupancyCounter{}; //Hardwired max not ideal but more efficient

    //Counter (for normalization)
    unsigned m_eventsPassed=0;
  };

  std::vector<thresholdHist_t> m_thresholdHists;

  StatusCode fillOccupancyHist(LArCellMonTool::thresholdHist_t& thr);
 
  // Identifer helpers and such

  const LArOnlineID* m_LArOnlineIDHelper;
  const CaloCell_ID* m_calo_id;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  
  std::array<CaloMonitoring::LArCellBinning,MAXLAYER> m_binning;

  //Sporadic monitoring related variables and structs
  std::string m_sporadicDir;
  unsigned m_counter_sporadic_protc=0;

  struct SporadicNoiseCell_t{
    TH1F* m_h_energy=nullptr;
    TH1F* m_h_quality=nullptr;
    TProfile* m_h_energyVsLB=nullptr;
    unsigned m_counter=0; 
    //Note wrt AthenaMP: Counting noisy event is obviously flawed, even for 
    //serial processing as events are scattered over many files
  };

  struct idHasher {
    std::size_t operator()(const Identifier& id) const {
      return std::hash<Identifier::value_type>{}(id.get_compact());
    }
  };

  std::unordered_map<Identifier,SporadicNoiseCell_t,idHasher> m_sporadicNoiseCells;
  
  void bookNoisyCellHistos(SporadicNoiseCell_t& result, const CaloDetDescrElement* dde, const PartitionEnum part, const float threshold, const LArOnOffIdMapping* cabling);

  std::array<TH1F*,MAXPARTITIONS> m_h_sporadicHists;
  std::array<unsigned,MAXPARTITIONS> m_sporadicPerPartCounter{};


  // summary plot
  TH1F* m_h_n_trigEvent=nullptr;

  // energy and time distribution for cell neither badcell nor noconditions 
  std::array<TH1F*,MAXLAYER> m_h_energy{{}};  // define when m_doUnnormalized1DEnergy==1
  std::array<TH2F*,MAXLAYER> m_h_energyVsTime{{}};
  std::array<TH1F*,MAXLAYER> m_h_cellTime{{}};
  std::array<TH1F*,MAXLAYER> m_h_energyVsTime_DifferThresholds{{}};


  // Reference Type Histograms (Filled solely with database information)
  std::array<TH2F*,MAXLAYER> m_h_badChannels_etaphi{{}};
  std::array<TH2F*,MAXLAYER> m_h_dbnoise_etaphi{{}};

  // temporary plots used for m_percentOccupancy_eta(phi) 
  std::array<TH1F*,MAXLAYER>  m_h_nActiveCellsFirstEvent_eta{{}}; 
  std::array<TH1F*,MAXLAYER>  m_h_nActiveCellsFirstEvent_phi{{}};

};

#endif 

//Histograms dropped during the redesign in spring 2017:
// * All inverse-masking
// * Cell Occupancy Eta/Phi Vs LumiBlock
// * DBNoiseNormalizedEnergy per layer 
// * doEtaPhiEnergyRMS per layer and threshold
// * DoEtaPhiRMSvsDBnoise per layer and threshold

