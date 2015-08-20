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
  virtual StatusCode finalize();

  virtual StatusCode bookHists();
  virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  virtual StatusCode fillHists();
  // does nothing
  virtual StatusCode finalHists();


 private:
  TH1F *SetupOverflowHist();
  TH2F *SetupLinkMultiplicityHist();
  void FillT1ResultHists();
  void FillOverflowHist(const ROIB::RoIBResult* result);
  void FillLinkMultiplicityHist(const ROIB::RoIBResult* result);
  void FillEtaPhiPlots();

  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!
  
  TH1F *m_histonroi;
  TH1F* m_histoeta0;
  TH1F* m_histophi0;
  TH1F* m_histodeta;
  TH1F* m_histodphi;
  TH2F *m_histo_eta0_phi0;
  TH1F *m_histoverflow;
  TH2F *m_histLinkMultiplicity;
  TrigLBNHist<TH2I>  *m_etaphi_EM_all;
  TrigLBNHist<TH2I>  *m_etaphi_J_all;
  TrigLBNHist<TH2I>  *m_etaphi_JF_all;
  TrigLBNHist<TH2I>  *m_etaphi_HA_all;
  TrigLBNHist<TH2I>  *m_etaphi_MU_all;

  TH2I *m_histThresholdMultiplicity;

  ServiceHandle< StoreGateSvc > m_storeGate; //!< pointer to StoreGate

  // L1 trigger config svc
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_trigConfigSvc; //!< access to trigger menu
  bool m_gotL1Config;     //!< retrieve of Lvl1ConfigSvc pointer successful?
  ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool; //!< tool to handle L1 objects

  // muon specific svc
  const LVL1::RecMuonRoiSvc* m_recRPCRoiSvc; //!< Muon RoI svc
  const LVL1::RecMuonRoiSvc* m_recTGCRoiSvc; //!< Muon RoI svc
  
  unsigned m_nxmax;
  std::string m_histoPathexpert;
  MsgStream* m_log;
  unsigned int m_logLvl; //!< MsgStram level
  std::vector< unsigned int > m_configuredTETypes;
  std::map< unsigned int, unsigned int > m_labels; // TriggerElement ID -> bin
  std::map< std::string, std::pair<int, unsigned int> > m_thresholdNames; // Lvl1 threshhold --> histo bin/id
  std::string m_trigLvl;
  unsigned int m_threshMultiMax;
};


#endif
