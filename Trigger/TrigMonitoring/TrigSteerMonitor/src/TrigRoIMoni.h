/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSteerMonitor
 * @Class  : TrigRoIMoni
 *
 * @brief  : fill monitoring histos for eta, phi, delta eta and delta phi of RoIs
 *
 * @author : Christiane Risler <risler@mail.desy.de>  - HU Berlin, Germany
 * 
 **********************************************************************************/


#ifndef TRIGROIMONI_H
#define TRIGROIMONI_H

#include "GaudiKernel/ToolHandle.h"

// These are needed for the eta/phi plots of L1 RoIs
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"

#include <vector>
#include <string>

#include "TrigSteering/Lvl1ResultAccessTool.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"


namespace HLT {
   class TrigSteer;
}

namespace ROIB {
   class RoIBResult;
}

namespace LVL1 {
   class RecMuonRoiSvc;
}

class StoreGateSvc;
class TH2F;
class TH1F;

class TrigRoIMoni: public TrigMonitorToolBase {

 public:
  TrigRoIMoni(const std::string & type, const std::string & name,
	     const IInterface* parent);

  virtual ~TrigRoIMoni() {};

  virtual StatusCode initialize();

  virtual StatusCode bookHists();
  virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  virtual StatusCode fillHists();

 private:
  TH1F *SetupOverflowHist();
  TH2F *SetupLinkMultiplicityHist();
  void FillT1ResultHists();
  void FillOverflowHist(const ROIB::RoIBResult* result);
  void FillLinkMultiplicityHist(const ROIB::RoIBResult* result);
  void FillEtaPhiPlots();

  const HLT::TrigSteer* m_parentAlg{0}; // should give us pointer to TrigSteer topAlgorithm!!!
  
  TH1F *m_histonroi{0};
  TH1F* m_histoeta0{0};
  TH1F* m_histophi0{0};
  TH1F* m_histodeta{0};
  TH1F* m_histodphi{0};
  TH2F *m_histo_eta0_phi0{0};
  TH1F *m_histoverflow{0};
  TH2F *m_histLinkMultiplicity{0};
  TH2I *m_histThresholdMultiplicity{0};
  TrigLBNHist<TH2I>  *m_etaphi_EM_all{0};
  TrigLBNHist<TH2I>  *m_etaphi_J_all{0};
  TrigLBNHist<TH2I>  *m_etaphi_JF_all{0};
  TrigLBNHist<TH2I>  *m_etaphi_HA_all{0};
  TrigLBNHist<TH2I>  *m_etaphi_MU_all{0};

  // L1 trigger config svc
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_trigConfigSvc; //!< access to trigger menu
  bool m_gotL1Config;     //!< retrieve of Lvl1ConfigSvc pointer successful?
  ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool; //!< tool to handle L1 objects

  // muon specific svc
  const LVL1::RecMuonRoiSvc* m_recRPCRoiSvc{0}; //!< Muon RoI svc
  const LVL1::RecMuonRoiSvc* m_recTGCRoiSvc{0}; //!< Muon RoI svc
  
  std::string m_histoPathexpert;
  std::vector< unsigned int > m_configuredTETypes;
  std::map< unsigned int, unsigned int > m_labels; // TriggerElement ID -> bin
  std::map< std::string, std::pair<int, unsigned int> > m_thresholdNames; // Lvl1 threshhold --> histo bin/id
  unsigned int m_threshMultiMax;
  bool m_ignoreAbortGap;
};


#endif
