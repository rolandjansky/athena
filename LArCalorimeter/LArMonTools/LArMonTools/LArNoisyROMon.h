/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoisyROMon
 * @author Laurent Duflot <duflot at lal.in2p3.fr>
 *
 */

#ifndef LArNoisyROMon_h
#define LArNoisyROMon_h

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TTree.h"


#include <string>

class TH1I_LW;
class TH1F_LW;
class TH2F_LW;
class LWHist2D;
class LWHist1D;
class LArOnlineID;
class LArOnlineIDStrHelper;

class LArNoisyROMon: public ManagedMonitorToolBase
{
public:
  LArNoisyROMon(const std::string& type, 
		const std::string& name, 
		const IInterface* parent);
  
  virtual ~LArNoisyROMon();
  
  StatusCode initialize();
  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode checkHists(bool fromFinalize);
  StatusCode procHistograms();
  
private:
  
  struct NoiseTime
  {
    unsigned int time;
    unsigned int time_ns;
    UChar_t algo;
  };
  NoiseTime m_NoiseTime;
  TTree* m_NoiseTimeTree;
  
  
  struct partitionHistos
  {
    partitionHistos() {
      h_NoisyFEBFracPerEvt = NULL;
      h_NoisyEventFrac = NULL;
      h_NoisyEventTimeVetoFrac = NULL;
      h_NoisyWEventFrac = NULL;
      h_NoisyWEventTimeVetoFrac = NULL;
      h_NoisyEventTrigger = NULL;
      h_NoisyFEBPerEvt = NULL;
      h_NoisyEvent = NULL;
      h_NoisyEventTimeVeto = NULL;
      h_NoisyWEvent = NULL;
      h_NoisyWEventTimeVeto = NULL;
      h_NoisyEventTriggerL1 = NULL;
    }      
    TH2F_LW* h_NoisyFEBFracPerEvt;
    TH1F_LW* h_NoisyEventFrac;
    TH1F_LW* h_NoisyWEventFrac;
    TH1F_LW* h_NoisyEventTimeVetoFrac;
    TH1F_LW* h_NoisyWEventTimeVetoFrac;
    TH1I_LW* h_NoisyEventTrigger;
    TH1I_LW* h_NoisyEventTriggerL1;
    
    // this are temporary histogram that never gets normalised
    // in 'online' mode they are used to produce regularly the 'Frac'
    // histograms
    TH2F_LW* h_NoisyFEBPerEvt;
    TH1I_LW* h_NoisyEvent;
    TH1I_LW* h_NoisyWEvent;
    TH1I_LW* h_NoisyEventTimeVeto;
    TH1I_LW* h_NoisyWEventTimeVeto;
  };
  TH1I_LW* h_NoisyFEB;
  TH1F_LW* h_NoisyEventFrac;
  TH1F_LW* h_NoisyEventTimeVetoFrac;
  TH1F_LW* h_SaturatedTightFrac;
  TH1I_LW* h_NoisyEvent;
  TH1I_LW* h_NoisyEventTimeVeto;
  TH1I_LW* h_SaturatedTight;
  TH1I_LW* h_SaturatedTightTimeVeto;
  TH1F_LW* h_SaturatedTightTimeVetoFrac;
  TH1I_LW* h_LBN;

  
  void bookPartitionHistos(partitionHistos&, const std::string& name, MonGroup& group, MonGroup& groupfrac, MonGroup& groupfracbin);
  void CopyHisto(LWHist2D* from,LWHist2D* to);
  void CopyHisto(LWHist1D* from,LWHist1D* to);
  
  // divide num by denom and store in to, assuming num is a subset of denom
  void Divide(LWHist1D* to,LWHist1D* num,LWHist1D* denom);
  
  // fill histogram of triggers
  void fillTriggerHisto(partitionHistos& partition, uint8_t triggerbits, uint8_t L1triggerbits);

  bool m_IsOnline;
  bool m_doTrigger;
  unsigned int m_eventCounter;
  partitionHistos m_BarrelA, m_BarrelC, m_EMECA, m_EMECC;
  const LArOnlineID* m_LArOnlineIDHelper;
  LArOnlineIDStrHelper* m_strHelper;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  
  std::string m_NoisyFEBDefStr;
  unsigned int m_BadFEBCut;

  std::vector<std::string> m_EF_NoiseBurst_Triggers;
  std::vector<std::string> m_L1_NoiseBurst_Triggers;
};


#endif
