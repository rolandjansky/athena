/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFEBMON_H
#define LARFEBMON_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>
#include <bitset>

#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

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

  void addHistos(TH2F_LW* h0,TH2F_LW* h1,TH2F_LW* h2);
  void addHistos(TH1F_LW* h0,TH1F_LW* h1,TH1F_LW* h2, float s1, float s2);
  //  bool nbOfFebOK(float nfeb,TH1I_LW* h);
  
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
  std::bitset<13> m_rejectionBits;
  bool m_currentFebStatus;
  unsigned int m_eventTime;
  unsigned int m_eventTime_ns;

  std::vector<std::string> m_partitionNames;
  //Added for Stream aware:
  std::vector<std::string> m_streams;
  std::vector<unsigned int> m_streamsThisEvent;
  std::vector<std::string> m_excoscalo;

  unsigned int m_l1Trig;
  
  std::vector<int> m_febInErrorTree;
  std::vector<int> m_febErrorTypeTree;
  
  // a handle on StoreGate 
  //   StoreGateSvc* m_storeGate;
  const LArOnlineID* m_onlineHelper;
  LArOnlineIDStrHelper* m_strHelper;
  ToolHandle<ILArBadChanTool> m_badChannelTool;
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
    // Stored in Misc directory
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
  
  //  0->EMBC / 1->EMBA / 2->EMECC / 3->EMECA / 4->HECC / 5->HECA / 6->FCalC / 7->FCalA    
  std::vector<summaryPartition> m_partHistos; 
  
  TH2I_LW* m_LArAllErrors_dE;
  TH1F_LW* m_rejectedYield;
  TH1F_LW* m_rejectedHisto;
  TProfile_LW* m_rejectedLBProfile;
  TH1I_LW* m_rejBitsHisto;
  
  TH1I_LW* m_eventsLB;
  TProfile_LW* m_oneErrorYieldLB;
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
  
  StatusCode bookNewPartitionSumm(int partNb);
  void fillErrorsSummary(int partitNb_2,int ft,int slot,uint16_t error, unsigned lumi_block = 0, bool lar_inerror = false );
  void plotMaskedFEB();
  //  void fillFebInError(const summaryPartition& summ,int errorType,int barrel_ec,int pos_neg,std::string summName);
  void fillFebInError(int partNb,int errorType);
  void fillYieldHistos(TH2I_LW* summaryHisto,TH2F_LW* statusHisto);
  int returnPartition(int be,int pn,int ft,int sl);
};

#endif // LArFEBMon_H
