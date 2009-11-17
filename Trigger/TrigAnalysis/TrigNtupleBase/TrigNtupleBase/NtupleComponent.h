/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNTUPLEBASE_NTUPLECOMPONENT_H
#define TRIGNTUPLEBASE_NTUPLECOMPONENT_H

#include "TTree.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <map>
#include <vector>

class NtupleAlgorithm;
class MsgStream;

namespace NtComponent {
  struct NtComponentParameters {
    std::string name;
    std::map<std::string,std::string> containerKeys;
    std::vector<std::string> options;
  };
  

  class NtupleComponent {
    
  public:
    NtupleComponent(NtupleAlgorithm *mainAlg,
                    NtComponentParameters parameters);
    virtual ~NtupleComponent();
    
    virtual StatusCode setupServices(void);
    virtual StatusCode book(void) = 0;
    virtual StatusCode fill(void) = 0;
    void setIsRunInfoComponent(bool runInfo) {m_isRunInfoComponent = runInfo;}

  protected:


    NtupleAlgorithm *m_mainAlg;
    MsgStream *m_msg;
    ServiceHandle<StoreGateSvc> *m_evtStore;
    ServiceHandle<StoreGateSvc> *m_detStore;
    ServiceHandle<ActiveStoreSvc> *m_activeStore;
    ToolHandle<Trig::TrigDecisionTool> *m_trigDecisionTool;
    TTree *m_tree;
    NtComponentParameters m_parameters;
    bool m_warningPrinted;
    bool m_isRunInfoComponent;
  };
}

#endif
