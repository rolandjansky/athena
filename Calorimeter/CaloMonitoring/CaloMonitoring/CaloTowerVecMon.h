/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
// NAME:     CaloTowerVecMon.cxx
//
// AUTHOR:   Lee Sawyer
// CREATED:  1 Feb 2009
// Manager Anirvan Sircar May 2010
//         D. Hu (Jun 2011 - May 2012)
//         L. Sawyer (2013-)
// ********************************************************************

#ifndef CALOTOWERMON_H
#define CALOTOWERMON_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "AthenaMonitoring/IDQFilterTool.h"

#include "Identifier/Identifier.h"

#include "CaloEvent/CaloTowerContainer.h"

#include <algorithm>
#include <stdint.h>
#include <string>
#include <vector>
#include <math.h>
#include <functional>
#include <set>
#include <map>
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile2D.h"

namespace Trig {
  class TrigDecisionTool;
  class ChainGroup;
}

class CaloTowerVecMon : public ManagedMonitorToolBase {

 public:

  CaloTowerVecMon(const std::string & type, const std::string& name, const IInterface* parent);
   virtual ~CaloTowerVecMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode bookHistograms();
   virtual StatusCode procHistograms();  
   virtual StatusCode fillHistograms();


 private:
   void initHists();

   StatusCode retrieveTools();

   StatusCode checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval);
   void bookTwrPreHists(const Interval_t theinterval);  
   void bookCellHists(const Interval_t theinterval);
   void bookNothrEnHists(const Interval_t theinterval);
   void bookMuthrHists(const Interval_t theinterval);
   void bookTwrStatHists(const Interval_t theinterval);
  
   StatusCode checkFilters(bool& ifPass);
   void fillTrigPara();
   void fillTwrPreHists(const CaloTowerContainer* towerCont );
   void fillCellHists(const CaloTower* twr);
   void fillNothrEnHists(const CaloTower* twr);
   void fillMuthrHists(const CaloTower* twr);
   void initTwrStat();
   void fillTwrStat(const CaloTower* twr);
   void fillTwrStatHists();

   enum EthreshLvl {	
      NO_E = 0,
      LOW_E = 1,
      LOWMED_E = 2,
      MED_E = 3,
      MEDHIGH_E = 4,
      HIGH_E = 5,
      MAX_E = 6
   };

   std::string m_towerContainerName;

   std::string m_timeGran;

   // Handles on various tools
   bool m_useBadLBTool;
   ToolHandle<IDQFilterTool> m_BadLBTool;
   bool m_useReadyFilterTool;
   ToolHandle<IDQFilterTool> m_ReadyFilterTool;
   bool m_useLArNoisyAlg;

   // trigger Awareness:
   bool m_useTrigger;
   ToolHandle<Trig::TrigDecisionTool> m_trigDec; 
   std::string m_TriggerNames1;
   std::string m_TriggerNames2;
   std::string m_TriggerNames3;
   const Trig::ChainGroup * m_Trigger1;
   const Trig::ChainGroup * m_Trigger2;
   const Trig::ChainGroup * m_Trigger3;
   bool m_isTrigDefined1;
   bool m_isTrigDefined2;
   bool m_isTrigDefined3;
   bool m_isTrigEvent1;
   bool m_isTrigEvent2;
   bool m_isTrigEvent3;

   // thresholds
   int m_threshtests;
   float m_Ethresh[MAX_E];
   bool m_filltranse;
   bool m_filltotal;

   // twr stat variables 
   float m_Fcaltower_sumEt;
   float m_energy_most;
   float m_eta_most;
   float m_phi_most; 
   int   m_ncells_most;

   // twr pre hists
   TH1F* m_n_calos;
   TH1F* m_n_twrs;

   // cell hists 
   TH1F* m_ratioE_leadcell_tower;
   TH2F* m_etaphi_hotrat_tower;
   TProfile2D* m_etaphi_avgenergy_hotrat_tower;

   //No threshold energy hists
   TH1F* m_twr_e;
   TH1F* m_postwr_e_thresh;
   TH1F* m_negtwr_e_thresh; 
   TH1F* m_twr_et_FcalA0;
   TH1F* m_twr_et_FcalA1;
   TH1F* m_twr_et_FcalA2;
   TH1F* m_twr_et_FcalC0;
   TH1F* m_twr_et_FcalC1;
   TH1F* m_twr_et_FcalC2;   

   // multi-threshold hists
   TH2F* m_twr_etaphi_Et_thresh[MAX_E];

   TProfile* m_twr_e_phi_thresh[MAX_E][3];
   TProfile* m_twr_e_eta_thresh[MAX_E];
   TProfile* m_twr_e_phiall_thresh[MAX_E];
   TProfile2D* m_twr_e_etaphi_thresh[MAX_E];

   TProfile* m_twr_e_trans_phi_thresh[MAX_E][3];
   TProfile* m_twr_e_trans_eta_thresh[MAX_E];
   TProfile* m_twr_e_trans_phiall_thresh[MAX_E];
   TProfile2D* m_twr_e_trans_etaphi_thresh[MAX_E];

   TH1F* m_twr_e_total_phi_thresh[MAX_E][3];
   TH1F* m_twr_e_total_eta_thresh[MAX_E];
   TH1F* m_twr_e_total_phiall_thresh[MAX_E];
   TH2F* m_twr_e_total_etaphi_thresh[MAX_E];  

   TProfile* m_twr_nCells_phi_thresh[MAX_E][3];
   TProfile* m_twr_nCells_eta_thresh[MAX_E];
   TProfile* m_twr_nCells_phiall_thresh[MAX_E];
   TProfile2D* m_twr_nCells_etaphi_thresh[MAX_E];

   TH1F* m_twr_phi_thresh[MAX_E][3];
   TH1F* m_twr_eta_thresh[MAX_E];
   TH1F* m_twr_phiall_thresh[MAX_E];
   TH2F* m_twr_etaphi_thresh[MAX_E];

   // twr stat hists
   TH1F* m_twr_et_Fcal0;
   TH1F* m_twr_et_Fcal1;
   TH1F* m_twr_et_Fcal2;

   TH1F* m_twrMost_e;
   TH2F* m_twrMost_etaphi;
   TH1F* m_twrMost_nCells;
};

#endif // CaloTowerVecMon_H
