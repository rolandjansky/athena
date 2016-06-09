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
 * $Id: TrigSignatureMoni.h,v 1.17 2009-04-14 14:59:18 medinnis Exp $
 **********************************************************************************/
#ifndef TRIGSIGNATUREMONI_H
#define TRIGSIGNATUREMONI_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include <set>
#include <string>
#include <map>

namespace HLT {
   class TrigSteer;
}


class TH1I;
class TH2I;
class EventInfo;
class StoreGateSvc;

// these are defined in TrigSignatureMoni.cxx
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
    virtual StatusCode finalize();
    virtual StatusCode bookHists();
    virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );

    virtual  StatusCode fillHists();
    // does noting
    virtual StatusCode finalHists();

 private:

  ServiceHandle<TrigConf::ITrigConfigSvc > m_trigConfigSvc;

  std::string m_histoPathexpert;
  const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!

  // These are helpers, for defining and filling hists
  GeneralBlock *m_generalBlock;
  StreamBlock  *m_streamBlock;
  GroupBlock   *m_groupBlock;
  ChainBlock   *m_chainBlock;

   // histograms
  TH2I* m_streamCorrel;
  TH1I* m_chainlengthHist;
  TH1I* m_lumiBlockNumDiffHist;
  TH1I* m_stepForEBHist;

  // Lumi Block histo sets
  TrigLBNHist<TH2I> *m_signatureAcceptanceHist;
  TrigLBNHist<TH1I> *m_eventsPassingStepHist;
  TrigLBNHist<TH1I> *m_totaleventsPassingStepHist;
  std::map<std::string, TH1I*> m_chainsInStream;

  std::string m_trigLvl;
  unsigned int m_highestLBN;
  int m_eventsPerLBHack;

};


#endif

