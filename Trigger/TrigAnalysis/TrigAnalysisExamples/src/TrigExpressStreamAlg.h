/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEXPRESSSTREAMALG_H
#define TRIGEXPRESSSTREAMALG_H

//
// Algorithm for offline post-processing of TrigMonEvent and TrigMonConfig objects
// which were recorded online or during reprocessing of debug streams. Both L2 and EF
// are processed in a single algorithm since during reprocessing cost data from both levels
// could be saved to a single BS file.
//

// C/C++
#include <set>

// Framework
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"
class EventInfo;
class StoreGateSvc;
class IIncidentSvc;

class TrigExpressStreamAlg : public AthAlgorithm
{
public:

   TrigExpressStreamAlg(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrigExpressStreamAlg();
   
   StatusCode initialize();
   StatusCode execute(); 
   StatusCode finalize();

private:

   std::vector<std::string> m_streamTriggers;
   std::map<std::string,int> m_numHLTPassedEvents; // events firing RAW trigger 
   std::map<std::string,int> m_numHLTFailedEvents; // events failing
   std::map<std::string,int> m_numL1PrescaledEvents; // events prescaled at L1
   std::map<std::string,int> m_numHLTPrescaledEvents; // events prescaled at HLT
   int m_evtNr;
   float m_streamPrescale; // stream Prescale changes per Lumi
   float m_prescale; //Prescale set per run

   ToolHandle< Trig::TrigDecisionTool > m_trigDec; //!< TDT handle
};

#endif
