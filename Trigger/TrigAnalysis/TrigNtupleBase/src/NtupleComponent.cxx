/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNtupleBase/NtupleComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {

  //---------------------------------------------------------

  NtupleComponent::NtupleComponent(NtupleAlgorithm *mainAlg, 
                                   NtComponentParameters parameters): 
    m_mainAlg(mainAlg),
    m_msg(0),
    m_evtStore(0),
    m_detStore(0),
    m_activeStore(0),
    m_trigDecisionTool(0),
    m_tree(0),
    m_parameters(parameters),
    m_warningPrinted(false),
    m_isRunInfoComponent(false){ 
  }

  //---------------------------------------------------------

  NtupleComponent::~NtupleComponent() {
  }

  //---------------------------------------------------------

  StatusCode NtupleComponent::setupServices() {
    m_msg = &(m_mainAlg->msg());
    m_evtStore = &(m_mainAlg->evtStore());
    m_detStore = &(m_mainAlg->detStore());
    m_activeStore = &(m_mainAlg->activeStore());
    m_trigDecisionTool = &(m_mainAlg->trigDecisionTool());
    if (m_isRunInfoComponent) {
      m_tree = m_mainAlg->treeRun();
    } else {
      m_tree = m_mainAlg->tree();
    }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------

}
