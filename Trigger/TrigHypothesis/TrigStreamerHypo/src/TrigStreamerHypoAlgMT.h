/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTREAMERHYPO_TRIGSTREAMERHYPOALGMT_H
#define TRIGSTREAMERHYPO_TRIGSTREAMERHYPOALGMT_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigStreamerHypoToolMT.h"

/**
 * @class TrigStreamerHypoAlgMT
 * @brief Implements a streamer (aka no selection) for the HLT framework.
 **/
class TrigStreamerHypoAlgMT : public ::HypoBase {
 public: 

  TrigStreamerHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  ToolHandleArray<ITrigStreamerHypoToolMT> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
}; 

#endif

