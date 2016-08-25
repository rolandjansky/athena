/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTMonTool.h
 *   Class declaration for HLTMonTool
 *   
 *                          
 *                          
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *  
 *   
 * 
 *   
 */

#ifndef HLTMONTOOL_H
#define HLTMONTOOL_H

#include <string>
#include <vector>


#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "LumiCalc/CoolQuery.h"   
#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
//#include "TrigDecision/TrigDecisionTool.h"

// Forward declarations
class IInterface;
class StoreGateSvc;
class TH1F;
class StatusCode;
// class LumiBlockCollection;
///Concrete monitoring tool derived from MonitorToolBase
class HLTMonTool : public IHLTMonTool {

 public:

  HLTMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTMonTool();

  /**    @name Book & fill & check (reimplemented from baseclass) */
  // LS 20 Jan 2014 - Modified to reflect new ManagedMonitorToolBase class
  virtual StatusCode init();
  virtual StatusCode book();
  virtual StatusCode fill();
  virtual StatusCode proc();

 private:
  //private methods for booking and filling
  StatusCode GetL1SummaryAndLBInfo();
  StatusCode bookResultAndConsistencyHistograms(const std::string&);
  StatusCode bookLvl1Histograms();
  StatusCode bookHLTHistograms();
  StatusCode bookHLTHistogramsForStream(const std::string&,const std::string&);

  StatusCode fillResultAndConsistencyHistograms(const std::string&, TH1*, TH1*);
  StatusCode fillLvl1Histograms();
  StatusCode fillForChain(const std::string&);

  //Data
  // Bookkeeping vectors
  //--- L1
  std::vector<std::string>  m_L1_summary; //!< vector of L1 Items
  //--- HLT
  std::vector<std::string>  m_L2_summary; //!< vector of L2 Chains
  std::vector<std::string>  m_EF_summary; //!< vector of EF Chains
  std::map<std::string,std::string>  m_regexlist; //!< maps Groupnames to corresponding regular expressions
  std::map<std::string,std::string>  m_run1regexlist; //!< like m_regexlist but for Run-1
  std::vector<std::string> m_rateregex; //!< chains matching one of these regexes will be rate monitored
  std::map<cool::ValidityKey, cool::UInt63> m_lbStartTimes; //!< map of LB to its start time in UNIX time
  std::map<cool::ValidityKey, cool::UInt63> m_lbStopTimes;  //!< map of LB to its stop time in UNIX time
  
  std::string m_lumikey; //!< StoreGate Key for LumiBlockRangeContainer
  std::string m_basepath; //!< Basepath for MonTool
  //  LumiBlockCollection* m_lbc;
  const xAOD::LumiBlockRangeContainer* m_lbc;
  bool m_scopeLumi;
  int m_nLBs; //# of lumi blocks in run;
  bool m_useRun1Trigger; 

  // this is a hack (recommended by Frank Winklmeier) to determine
  // if we are processing "old/Run-1 data" (and we should expect 
  // trigger chain names to begin with L2_ or EF_) vs "new data" 
  // (where trigger chains names begin with HLT_). It is not the most 
  // eloquent implementation ever, but it should work
  bool useRun1Trigger(){return getRunNr() < 222222 && getRunNr() > 0;}

};

#endif

