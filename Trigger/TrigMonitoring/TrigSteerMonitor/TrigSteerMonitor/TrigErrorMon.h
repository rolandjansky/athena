/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSteerMonitor
 * @Class  : TrigErrorMon
 *
 * @brief  :  monitor HLT error code (HLT and chain status)
 *
 * @author : Christiane Risler <risler@mail.desy.de>  - HU Berlin, Germany
 * 
 **********************************************************************************/



#ifndef TRIGERRORMON_H
#define TRIGERRORMON_H

#include "GaudiKernel/ITHistSvc.h"
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include <vector>
#include <string>

namespace HLT {
   class TrigSteer;
}

class TH1I;
class TH2I;
class TrigErrorMon: public TrigMonitorToolBase {

 public:
  TrigErrorMon(const std::string & type, const std::string & name,
	     const IInterface* parent);

  virtual ~TrigErrorMon() {};

  virtual StatusCode initialize();

  virtual StatusCode bookHists();
  virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  virtual StatusCode fillHists();

 private:
  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!
  TrigLBNHist<TH1I>* m_histo_reason;
  TrigLBNHist<TH1I>* m_histo_action;
  TrigLBNHist<TH1I>* m_histo_steeringInternalReason;

  TH2I* m_histo2d_reason;
  TH2I* m_histo2d_action;
  TH2I* m_histo2d_steeringInternalReason;

  TH2I* m_histo_te_errors_etaphi;
  TH1I* m_histo_te_errors_names;

  TH2I* m_histo_tau_errors_etaphi;
  TH2I* m_histo_trk_errors_etaphi;
  TH2I* m_histo_e_errors_etaphi;
  TH2I* m_histo_g_errors_etaphi;
  TH2I* m_histo_j_errors_etaphi;
  TH2I* m_histo_b_errors_etaphi;
  TH2I* m_histo_mu_errors_etaphi;
  TH2I* m_histo_xe_errors_etaphi;

  std::string m_trigLvl;  
  std::map< unsigned int, unsigned int > m_binmap; // maps chain_counter to histogram bin

  std::vector<std::string> m_te_names;
  bool m_expertMode;
  
};

#endif
