/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGEGAMMATOPOHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGEGAMMATOPOHYPOTOOL_H

/*************************************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/combos/TrigEgammaTopoHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for electron/photon pairs with 
 **                invariant mass or deltaPhi in some interval; intended for e.g Z->ee, H->gg, etc
 **                
 **
 **   Author: Debottam Bakshi Gupta <debottam.bakshi.gupta@cern.ch>
 **
 **   Created:   May 15 2021
 **
 **************************************************************************************************/ 

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigEgammaTopoHypoTool
 * \brief TrigEgammaTopoHypoTool is a ComboHypoTool that calculates the inv mass and deltaPhi distance
 * Apply inv mass cuts (upper and lower cut) or deltaPhi cuts or both  to the two electrons/photons and accepts the event if condition is 
 * satisfied
 *
 */


class TrigEgammaTopoHypoTool:  public ComboHypoToolBase {

 public:
  
  TrigEgammaTopoHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual StatusCode initialize() override;


 private:
  
  virtual bool executeAlg(const std::vector<Combo::LegDecision>& combination) const override;
  
  // flags
  Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property< bool > m_applyMassCut {this, "ApplyMassCut", false, "Cut on lower limit of mass" };
  Gaudi::Property< bool > m_applyDPhiCut {this, "ApplyDPhiCut", false, "Cut on both delta phi distance and mass" };
  
  // cuts
  Gaudi::Property<float> m_lowerMassEgammaClusterCut {this,"LowerMassEgammaClusterCut", 50000.0, "Lower mass cut for electron-cluster pair"}; //!<  lower inv mass cut (e,cluster)
  Gaudi::Property<float> m_upperMassEgammaClusterCut {this,"UpperMassEgammaClusterCut", -999, "Upper mass cut for electron-cluster pair"}; //!<  upper inv mass cut (e,cluster)
  Gaudi::Property<float> m_thresholdDPhiCut {this,"ThresholdDPhiCut", 1.5, "minimum deltaPhi required between two photons"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };


}; // TRIGEGAMMAHYPO_TRIGEGAMMATOPOHYPOTOOL_H
#endif



