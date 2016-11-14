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
  const HLT::TrigSteer* m_parentAlg{0}; // should give us pointer to TrigSteer topAlgorithm!!!
  TrigLBNHist<TH1I>* m_histo_reason{0};
  TrigLBNHist<TH1I>* m_histo_action{0};
  TrigLBNHist<TH1I>* m_histo_steeringInternalReason{0};

  TH2I* m_histo2d_reason{0};
  TH2I* m_histo2d_action{0};
  //TH2I* m_histo2d_steeringInternalReason{0};
  TH2I* m_histo_te_errors_etaphi{0};

  std::string m_trigLvl;
  std::map< unsigned int, unsigned int > m_binmap; // maps chain_counter to histogram bin
  bool m_expertMode;  
};

#endif
