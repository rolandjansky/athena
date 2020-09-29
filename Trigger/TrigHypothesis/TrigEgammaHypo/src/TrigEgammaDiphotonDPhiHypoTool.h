/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGEGAMMADIPHOTONDPHIHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGEGAMMADIPHOTONDPHIHYPOTOOL_H

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaDiphotonDPhiHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for photon pairs with 
 **                deltaPhi more than a threshold value; intended for H->gg
 **
 **   Author: D. BakshiGupta  <debottam.bakshigupta@cern.ch>
 **
 **************************************************************************/ 

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODBase/IParticleContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigEgammaDiphotonDPhiHypoTool
 * \brief TrigEgammaDiphotonDPhiHypoTool is a ComboHypoTool that calculates the deltaPhi distance 
 * Apply the threshold cut and accepts the event if condition is 
 * satisfied
 *
 */


class TrigEgammaDiphotonDPhiHypoTool:  public ComboHypoToolBase {

 public:
  
  TrigEgammaDiphotonDPhiHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~TrigEgammaDiphotonDPhiHypoTool() {};
  virtual StatusCode initialize() override;


 private:
  
  virtual bool executeAlg(std::vector<LegDecision>& thecomb) const override;
  
  // flags
  Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };
  
  // cuts
  Gaudi::Property<float> m_thresholdDPhiCut {this,"ThresholdDPhiCut", 1.5, "minimum deltaPhi required between two photons"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };


}; // TRIGEGAMMAHYPO_TRIGEGAMMADIPHOTONDPHIHYPOTOOL_H
#endif



