/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigSignatureMonitor
 * @Class  : TrigSignatureMoni
 * @Created: Nov, 2006
 *
 * @brief  stepwise monitoring of number of events accepted by signatures 
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: TrigSignatureMoni.h 772074 2016-09-07 09:01:25Z fwinkl $
 **********************************************************************************/
#ifndef TRIGSIGNATUREMONI_H
#define TRIGSIGNATUREMONI_H

#include "GaudiKernel/ServiceHandle.h"

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include <string>
#include <map>

namespace HLT {
   class TrigSteer;
}

class TH1I;
class TH2I;
class GeneralBlock;
class StreamBlock;
class GroupBlock;
class ChainBlock;


class TrigSignatureMoni: public TrigMonitorToolBase {

 public:
    TrigSignatureMoni(const std::string & type, const std::string & name,
                      const IInterface* parent);

    virtual ~TrigSignatureMoni();

    virtual StatusCode initialize();
    virtual StatusCode bookHists();
    virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
    virtual StatusCode fillHists();
    virtual StatusCode finalHists();

 private:

  ServiceHandle<TrigConf::ITrigConfigSvc > m_trigConfigSvc;

  std::string m_histoPathexpert;
  const HLT::TrigSteer* m_parentAlg{0}; // should give us pointer to TrigSteer topAlgorithm!!!

  // These are helpers, for defining and filling hists
  GeneralBlock *m_generalBlock{0};
  StreamBlock  *m_streamBlock{0};
  GroupBlock   *m_groupBlock{0};
  ChainBlock   *m_chainBlock{0};

   // histograms
  TH2I* m_streamCorrel{0};
  TH1I* m_chainlengthHist{0};
  TH1I* m_stepForEBHist{0};

  // Lumi Block histo sets
  TrigLBNHist<TH2I> *m_signatureAcceptanceHist{0};
  TrigLBNHist<TH1I> *m_eventsPassingStepHist{0};
  TrigLBNHist<TH1I> *m_totaleventsPassingStepHist{0};
  std::map<std::string, TH1I*> m_chainsInStream;

  std::string m_trigLvl;
};


#endif

