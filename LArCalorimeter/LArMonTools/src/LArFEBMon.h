/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONTOOLS_LARFEBMON_H
#define LARMONTOOLS_LARFEBMON_H

#include "LArOnlineIDStrHelper.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "LArIdentifier/LArOnlineID.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>
#include <vector>
#include <bitset>

class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile2D_LW;
class TProfile_LW;
class LArFebErrorSummary;
class TTree;

class LArFEBMon : public ManagedMonitorToolBase {
  
public:
  
  LArFEBMon(const std::string & type, const std::string& name, 
	    const IInterface* parent);
  ~LArFEBMon();
  
  StatusCode initialize();
  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode procHistograms();

  void AddHistos(TH2F_LW* h0,TH2F_LW* h1,TH2F_LW* h2);
  void AddHistos(TH1F_LW* h0,TH1F_LW* h1,TH1F_LW* h2, float s1, float s2);
  bool nbOfFebOK(float nfeb,TH1I_LW* h);
  
private:
  
  float m_FEBmin,m_FEBmax;
  int m_FEBnbins;
  /* global params */
  int m_eventsCounter;
  bool m_ignoreMissingHeaderEMB;
  bool m_ignoreMissingHeaderPS;
  std::string m_keyDSPThresholds;
  bool m_isOnline;
  unsigned int m_lumi_blocks; 
  bool m_eventRejected;
  std::bitset<13> m_rejectionBits;
  bool m_currentFebStatus;
  unsigned int m_eventTime;
  unsigned int m_eventTime_ns;

  //Added for Stream aware:
  std::vector<std::string> m_streams;
  std::vector<unsigned int> m_streamsThisEvent;
  std::vector<std::string> m_excoscalo;

  unsigned int m_l1Trig;
  
  std::vector<int> m_febInErrorTree;
  std::vector<int> m_febErrorTypeTree;
  
  const LArOnlineID* m_onlineHelper;
  LArOnlineIDStrHelper* m_strHelper;
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MissingFEBKey", "LArBadFeb", "SG key for missing FEBs"};
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};
  SG::ReadHandleKey<LArFebHeaderContainer> m_LArFebHeaderContainerKey{this, "LArFebHeaderContainerKey", "LArFebHeader"};
  SG::ReadHandleKey<LArFebErrorSummary> m_LArFebErrorSummaryKey{this, "LArFebErrorSummaryKey", "LArFebErrorSummary"};
  // trig. decision tool
  ToolHandle<Trig::TrigDecisionTool>  m_trigDec;
  bool m_trigok;
  
  // the histograms
  struct summaryPartition{
    // These are error histograms-This should be empty
    
    summaryPartition() 
    {
      parity = NULL;
      BCID_2halves = NULL;
      RADD_2halves = NULL;
      EVTID_2halves = NULL;
      SCACStatus = NULL;
      scaOutOfRange = NULL;
      gainMismatch = NULL;
      typeMismatch = NULL;
      badNbOfSamp = NULL;
      zeroSamp = NULL;
      checkSum = NULL;
      missingHeader = NULL;
      badGain = NULL;
      LArAllErrors = NULL;
      LArAllErrorsYield = NULL;
      maskedFEB = NULL;
      missingTriggerType = NULL;
      nbOfEvts = NULL;
      nbOfSweet1 = NULL;
      nbOfSweet2 = NULL;
      nbOfFebBlocks = NULL;
      stream_eventSizeLB = NULL;
      eventSizeLB = NULL;
      m_rejectedLBProfilePart = NULL;
      
      histBooked = false;
    }
    
    TH2I_LW* parity;
    TH2I_LW* BCID_2halves;
    TH2I_LW* RADD_2halves;
    TH2I_LW* EVTID_2halves;
    TH2I_LW* SCACStatus;
    TH2I_LW* scaOutOfRange;
    TH2I_LW* gainMismatch;
    TH2I_LW* typeMismatch;
    TH2I_LW* badNbOfSamp;
    TH2I_LW* zeroSamp;
    TH2I_LW* checkSum;
    TH2I_LW* missingHeader;
    TH2I_LW* badGain;
    TH2I_LW* LArAllErrors;
    TH2F_LW* LArAllErrorsYield;
    TH2I_LW* maskedFEB;
    // Not (yet?) an error - Stored in Misc directory
    TH2I_LW* missingTriggerType;
    // These are general data histograms
    TH2I_LW* nbOfEvts;
    TProfile2D_LW* nbOfSweet1;
    TProfile2D_LW* nbOfSweet2;
    TH1I_LW* nbOfFebBlocks;
     // Per stream, per partition event size vs. lumiblock
     TProfile2D_LW* stream_eventSizeLB;
     // Per partition event size vs. lumiblock
     TProfile_LW* eventSizeLB;
     // Per partition rejected events ?
     TProfile_LW* m_rejectedLBProfilePart;
    // Other flags
    bool histBooked;
  };
  
  summaryPartition m_barrelCSummary;
  summaryPartition m_barrelASummary;
  
  // Partition below are instantiated only if m_compactEndCap == false
  summaryPartition m_emecCSummary;
  summaryPartition m_emecASummary;
  summaryPartition m_hecCSummary;
  summaryPartition m_hecASummary;
  summaryPartition m_fcalCSummary;
  summaryPartition m_fcalASummary;
  
  TH2I_LW* m_LArAllErrors_dE;
  TH1F_LW* m_rejectedYield;
  TH1F_LW* m_rejectedHisto;
  TProfile_LW* m_rejectedLBProfile;
  TH1I_LW* m_rejBitsHisto;
  
  TH1I_LW* m_eventsLB;
  //   TH1I_LW* m_rejectedEventsTime;
  TProfile_LW* m_rejectedYieldLB;
  TProfile_LW* m_rejectedYieldLBout;
  TProfile_LW* m_eventSizeLB;
  TProfile2D_LW* m_stream_eventSizeLB;
  TH2I_LW* m_nbOfEvts2D;
  TH1I_LW* m_eventType;
  TH1I_LW* m_nbOfSamples;
  TH1I_LW* m_totNbSw2;
  TH1I_LW* m_dspThresholds_ADC;
  TH1I_LW* m_dspThresholds_qtime;
  
  TH1I_LW* m_hTriggerType;
  TH1I_LW* m_hTriggerTypeAllDSP;
  TH1I_LW* m_hTimeDistrib;
  TH1I_LW* m_nbOfFebBlocksTotal;
  
  TH1F_LW* m_febInError[1524];
  bool m_bfebIE[1524];
  bool m_anyfebIE;
  
  TTree* m_CorruptTree;
  
  StatusCode bookNewPartitionSumm(summaryPartition& summ,std::string summName);
  void fillErrorsSummary(summaryPartition& summ,int partitNb_2,int ft,int slot,uint16_t error, unsigned lumi_block = 0, bool lar_inerror = false );
  void plotMaskedFEB();
  void fillFebInError(const summaryPartition& summ,int errorType,int barrel_ec,int pos_neg,std::string summName);
  void fillYieldHistos(TH2I_LW* summaryHisto,TH2F_LW* statusHisto);
  int returnPartition(int be,int pn,int ft,int sl);
};

#endif // LArFEBMon_H
