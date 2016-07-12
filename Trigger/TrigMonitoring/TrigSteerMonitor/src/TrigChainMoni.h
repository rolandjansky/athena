/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigChainMonitor
 * @Class  : TrigChainMoni
 * @Created: Nov, 2006
 *
 * @brief number of events vs chain for raw accepted and events after passthrough and 
 *        prescale
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: TrigChainMoni.h 761301 2016-07-12 11:51:07Z fwinkl $
 **********************************************************************************/


#ifndef TRIGCHAINMONI_H
#define TRIGCHAINMONI_H

#include <string>
#include <map>
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"

namespace HLT {
   class TrigSteer;
}

class TH1I;

class TrigChainMoni: public TrigMonitorToolBase {

public:
   TrigChainMoni(const std::string & type, const std::string & name,
                 const IInterface* parent);

   virtual ~TrigChainMoni();

   virtual StatusCode initialize();
   virtual StatusCode bookHists();
   virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
   virtual StatusCode fillHists();

   // does noting
   virtual StatusCode finalHists();
  
private:
   std::string m_histoPathshift;
   const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!

   // 1D histograms

   //Lumi Block Histos

   TH1I *m_chainAcceptanceHist;
   TH1I *m_chainAcceptancePSHist;
   TH1I *m_chainAcceptancePTHist;
   TH1I *m_activeChainsHist;
   TH1I *m_runChainsHist;
   TH1I *m_rerunChainsHist;

   std::map< unsigned int, unsigned int > m_binmap;
   std::string m_trigLvl;
   bool m_useLBHistos;
};


#endif
