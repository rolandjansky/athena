/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArOddCellsMonTool
 * @author Benjamin Trocme <benjamin.trocme@lpsc.in2p3.fr>
 *
 */

#ifndef LARODDCELLSMONTOOL_H
#define LARODDCELLSMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/HWIdentifier.h"
#include "LArMonitoring/LArOnlineIDStrHelper.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArCabling/LArCablingService.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

#include <map>
#include <string>
#include <vector>
#include <bitset>

class LArEM_ID;
class LArOnlineID;
class TH1I_LW;
class TH1D_LW;
class TH2I_LW;
class TH2F_LW;
class LWHist;
class LWHist2D;
class TProfile2D_LW;
class TTree;

class LArOddCellsMonTool: public ManagedMonitorToolBase
{
 public:
  LArOddCellsMonTool(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArOddCellsMonTool();

  StatusCode initialize();

  // Book general histograms
  StatusCode bookHistograms();

  // Called each event
  StatusCode fillHistograms();
  StatusCode checkHists(bool fromFinalize);
  StatusCode procHistograms();

 protected:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID* m_LArEM_IDHelper;
  LArOnlineIDStrHelper* m_strHelper;
  /** Handle to LArCablingService */
  ToolHandle<LArCablingService> m_LArCablingService;
   /** Handle to bad-channel mask */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  const DataHandle<ILArPedestal>  m_larPedestal;
  
 private:

  struct summaryHistos {
    summaryHistos();
    TH2F_LW* YieldPerChan;
    TH2I_LW* NbOddPerFEB;
    TH2F_LW* YieldPerFEB;
    TH2I_LW* NbOddSumPerFEB;
    TH2F_LW* YieldSumPerFEB;
    TH1D_LW* YieldPerEvent;
    TH1D_LW* YieldPerEventCleanQ;
    TH1I_LW* TimeBursty;
    TH1I_LW* BurstStatus;
    TH2I_LW* CellNbPerSlot;
  };

  class Simple2DSumHist;

  struct feedthroughInfo {
    bool bookedHists;
    bool toBeMonitored;
    int barrel_ec;
    int pos_neg;
    int feedthrough;
    TProfile2D_LW* hReference;
    TH2F_LW* hReferenceCool;
    TH1D_LW* hReferenceCool_sum2;
    TH2I_LW* NbOddPerChan;
    TH2F_LW* YieldPerChan;
    TH2I_LW* hIndivTimeDistrib;
    Simple2DSumHist * hTempSumComput;
  };

  // initialize monitoring bookkeeping info
  StatusCode initMonInfo();
  // Summary histograms booking
  void bookHistsPartition(summaryHistos& summary,std::string partitionName);
  // book per feedthrough reference and odd cells histos
  StatusCode bookPerFeedthrough(HWIdentifier feedthroughID, CaloGain::CaloGain gain);
  // retrieve reference pedestal per feb
  StatusCode retrievePedPerFeb(HWIdentifier febID, HWIdentifier feedthroughID, CaloGain::CaloGain gain);
  // Copy yield from FT TH2 fo parition TH2
  void copyChanYield(feedthroughInfo ftInfo);
  // get the max slot number in the corresponding feedthrough - HEC:10  EMECStandard:13  EMECSpecial:15  FCAL:14  Barrel:14
  int feedthroughSlotMax(HWIdentifier id);
  // get gain string
  std::string gain_str(CaloGain::CaloGain gain);
  // Fill feedthrough summary histos
  void fillFeedthroughHistos(HWIdentifier FTId,int slotNumber,int weight,bool cellCnt = false);

  void fillYieldHistos(TH2I_LW* summaryHisto,TH2F_LW* statusHisto,TH2I_LW* cellCountingHisto = NULL);

  void fillYieldPerEvent(summaryHistos& summaryPart,double yieldEvent,int typeOfBursty);

  void fillSumPerFEBHistos(feedthroughInfo& ftInfo);

  void defineAxisTitle(LWHist2D* hist,int typeH);

  void deregisterHisto(LWHist* hist);

// Data members related to job options property================================

  // List used to restrict monitoring to certain feedthrough
  std::vector<std::string> m_feedthroughNames;
  // Key to retrieve pedestal ref from COOL
  std::string m_larPedestalKey;
  // Key to retrieve LArDigit
  std::string m_LArDigitContainerKey;
  // Monitor only gain per partition : [EM+FCAL;HEC]
  std::vector < std::string> m_gainMonitored;
  // m_sampleNumber contains the sample to be checked - If negative or greater than the nb of samples,all samples are accumulated
  int m_sampleNumber;
  // An event is said odd if m_oddnessThreshold times noise away from its pedestal
  float m_oddnessThreshold;
  // If m_posNeg = [-1,1,0], takes only odd events with digit [lower,higher,all] than pedestal reference
  int m_posNeg;
  // If the reference noise if outside [m_minimumRefNoise,m_maximumRefNoise], the channel is not monitored
  std::vector<double> m_maximumRefNoise;
  std::vector<double> m_minimumRefNoise;
  // Threshold used on a partition basis to flag an event as bursty (and store it if activated)
  float m_burstyThreshold;
  float m_burstyThresholdQ;
  // Activation of bad channels masking. Exact policy of masking defined in LArAllMonitoring
  bool m_maskBadChannels;
  // If m_coolRetrieval is set to false, the reference pedestal/noise are computed from the m_computePeds first events
  bool m_coolRetrieval;
  int m_computePeds;
  // If useEvtCounter set to true, use the event counter to flag bursty event instead of evt id
  bool m_useEvtCounter;
  // Maximum time used in temporal distribution histogram of bursty events
  int m_maxTimingHisto;
  // If set to tree, store caracteristics of odd cells of bursty events
  bool m_createTree;
  // If set to true, switch off alg if physic mode + non zero dsp thresholds
  bool m_monPhys;
  // If set to true, also stores histograms of pedestal/noise (reference or computed ones)
  bool m_storeRefs;
  // If set to true; also stores histogram per FT (useless as priori as copied in partition TH2)
  bool m_storePerFT;
  // If set to true, perform monitoring per detailed endcap (i. EM + FCAL + HEC)
  bool m_detailEndCap;
  // Key to retrieve DSP thresholds (used to switch off algo if physic mode + non zero dsp thresholds)
  std::string m_keyDSPThresholds;

  std::vector<std::string> m_noise_streams;

  float m_evtTime;


  // Tree used to store odd cells characteristics of bursty events
  TTree* m_tOdd;
  int m_treeNbOfOdd;
  int m_treeBarrelEc[60000];
  int m_treePosNeg[60000];
  int m_treeFT[60000];
  int m_treeSlot[60000];
  int m_treeChannel[60000];
  int m_treeSample[60000];
  int m_treeSampling[60000];
  int m_treeRegion[60000];
  int m_treeEta[60000];
  int m_treePhi[60000];
  float m_treeSignif[60000];
  //Info on which partition is bursty
  int m_treeBurstyPartition[10];
  // Event counter incremented at each event read
  int m_eventsCounter;
  // EvtId retrieved from event info block
  int m_evtId;
  // Nb of samples (useful for normalization in determination of status)
  int m_nbOfSamples;
  // Nb of channel monitored per partition (sub partition of endcap also available) and in total
  int m_nbMonitoredChan[10];
  int m_nbMonitoredTotal;
  // If any bursty partition is detected, set thie flag to true so save odd events in the tree.
  bool m_eventToBeStored;
  // If physic mode with non zero dsp thresholds, and m_monPhys = true, skip the monitoring
  bool m_skipAlgo;
  bool m_skipEvent;

  // Basic histograms per feedthrough/gain to monitor odd cells
  std::vector< std::map< CaloGain::CaloGain, feedthroughInfo> > m_feedthroughInfo;//TK: maps are things to avoid for performance reasons.
  // m_febPedRetrieved[febHash] is true if the pedestals corresponding to this 
  // FEB has been retrieved from the reference file and filled in the reference
  // histogram in m_feedthroughInfo
  std::vector< std::map< CaloGain::CaloGain, bool> > m_febPedRetrieved;//TK: maps are things to avoid for performance reasons.

  // Summary of nb of monitored channels per partition
  TH1I_LW* m_hMonitoredChan;
  // Nb of odd cells per event vs time (evtid or event counter)
  TH1I_LW* m_hTimeDistrib;
  // Yield of odd cells per event
  TH1D_LW* m_hOddEventsYield;
  // Summary per partition of yield of odd events
  summaryHistos m_barrelCSummary;
  summaryHistos m_barrelASummary;
  summaryHistos m_endcapCSummary;
  summaryHistos m_endcapASummary;
  summaryHistos m_emecCSummary;
  summaryHistos m_emecASummary;
  summaryHistos m_hecCSummary;
  summaryHistos m_hecASummary;
  summaryHistos m_fcalCSummary;
  summaryHistos m_fcalASummary;
};

#endif
