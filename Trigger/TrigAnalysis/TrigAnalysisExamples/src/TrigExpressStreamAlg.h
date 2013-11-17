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
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class EventInfo;
class StoreGateSvc;
class IIncidentSvc;

class TrigExpressStreamAlg : public Algorithm
{
public:

   TrigExpressStreamAlg(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrigExpressStreamAlg();
   
   StatusCode initialize();
   StatusCode execute(); 
   StatusCode finalize();

private:
   
   MsgStream& log() const { return *m_log; }
   
 public:

   MsgStream                          *m_log;
   ServiceHandle<StoreGateSvc>         m_storeGate;
};

#endif
