/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTREAMERHYPO_TRIGSTREAMERHYPOALG_H
#define TRIGSTREAMERHYPO_TRIGSTREAMERHYPOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigStreamerHypoTool.h"

/**
 * @class TrigStreamerHypoAlg
 * @brief Implements a streamer (aka no selection) for the HLT framework.
 **/
class TrigStreamerHypoAlg : public ::HypoBase {
 public: 

  TrigStreamerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  ToolHandleArray<ITrigStreamerHypoTool> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};

  Gaudi::Property<bool> m_featureIsROI{this, "FeatureIsROI", true, "If the Streamer alg should re-use the initialRoI as the 'feature'. Will re-use the previous step's feature if false"};     
}; 

#endif

