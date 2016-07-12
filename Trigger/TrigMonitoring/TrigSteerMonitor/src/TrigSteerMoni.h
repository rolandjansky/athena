/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSteerMonitor
 * @Class  : TrigSteerMoni
 * @Created: Nov, 2006
 *
 * @brief  obsolete
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: TrigSteerMoni.h 761301 2016-07-12 11:51:07Z fwinkl $
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
class TH2F;

class TrigSteerMoni: public MonitorToolBase {

 public:
    TrigSteerMoni(const std::string & type, const std::string & name,
		  const IInterface* parent);

    virtual ~TrigSteerMoni();

    StatusCode initialize();

    StatusCode bookHists();
    StatusCode fillHists();

 private:

  std::string m_histoPath;

  const HLT::TrigSteer* m_parentAlg{0}; // should give us pointer to TrigSteer topAlgorithm!!!

  // 1D histograms
  TH1F* m_chainAcceptanceHist{0}; // raw
  TH1F* m_chainAcceptancePSHist{0}; // after Prescale
  TH1F* m_chainAcceptancePTHist{0}; // after Passthrough

  // 2D histograms
  TH2F* m_signatureAcceptanceHist{0};

  long int m_totalEvts{0};
  std::string m_trigLvl;

};


#endif
