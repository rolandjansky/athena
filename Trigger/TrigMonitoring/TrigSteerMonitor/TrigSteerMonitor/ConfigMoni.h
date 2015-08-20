/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSteerMonitor
 * @Class  : TrigSteerMoni
 * @Created: Nov, 2006
 *
 * @brief  
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: ConfigMoni.h,v 1.2 2007-07-03 10:20:36 risler Exp $
 **********************************************************************************/


#ifndef TRIGSTEERMONI_H
#define TRIGSTEERMONI_H


#include "AthenaMonitoring/MonitorToolBase.h"
#include <vector>
#include <string>

namespace HLT {
   class TrigSteer;
}

class TH1F;
class TH2F;//GF
class TLegend;
class TrigSteerMoni: public MonitorToolBase {

 public:
    TrigSteerMoni(const std::string & type, const std::string & name,
		  const IInterface* parent);

    virtual ~TrigSteerMoni();

    StatusCode initialize();
    StatusCode bookHists();
    StatusCode fillHists();
    // does noting
    StatusCode finalHists();
    
 private:

  MsgStream* m_log; 
  std::string m_histoPathexpert;
  std::string m_histoPathshift;
  
  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!

  // 1D histograms
  TH1F* m_activeChainsHist;//number of active chains
  TH1F* m_chainAcceptanceHist; // raw
  TH1F* m_chainAcceptancePSHist; // after Prescale
  TH1F* m_chainAcceptancePTHist; // after Passthrough
  TH1F* m_eventAcceptanceHist; // event counter
  // 2D histograms
  TH2F* m_signatureAcceptanceHist;//GF
  TH2F* m_triggerAcceptanceHist;//GF TE
  long int m_totalEvts;
  long int m_eventPassed; // Event
  std::string m_trigLvl;

};


#endif
