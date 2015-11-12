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
 * $Id: TrigSteerMoni.h,v 1.5 2007-07-03 10:20:37 risler Exp $
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

  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!

  // 1D histograms
  TH1F* m_chainAcceptanceHist; // raw
  TH1F* m_chainAcceptancePSHist; // after Prescale
  TH1F* m_chainAcceptancePTHist; // after Passthrough

  // 2D histograms
  TH2F* m_signatureAcceptanceHist;

  long int m_totalEvts;
  std::string m_trigLvl;

};


#endif
