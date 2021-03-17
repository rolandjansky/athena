/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_TRIGTAUXCOMBOHYPOTOOL_H
#define TRIGTAUHYPO_TRIGTAUXCOMBOHYPOTOOL_H

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODBase/IParticleContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigTauXComboHypoTool
 * \brief TrigTauXComboHypoTool is a ComboHypoTool that calculates the deltaR between two particles and
 * apply cuts on it (upper and lower cut) accepting the event if the required condition is 
 * satisfied
 *
 */


class TrigTauXComboHypoTool:  public ComboHypoToolBase {

 public:
  
  TrigTauXComboHypoTool(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);
  
  virtual ~TrigTauXComboHypoTool() {};
  virtual StatusCode initialize() override;


 private:
  
  virtual bool executeAlg(std::vector<LegDecision>& thecomb) const override;
  
  // flags
  Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };
  
  // cuts
  Gaudi::Property<float> m_dRmin {this,"LowerdRCut", 0.3, "Lower dR cut for tau-lepton pair"};
  Gaudi::Property<float> m_dRmax {this,"UpperdRCut", 10.0, "Upper dR cut for tau-lepton pair"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };


}; // TRIGTAUHYPO_TRIGTAUXCOMBOHYPOTOOL_H
#endif



