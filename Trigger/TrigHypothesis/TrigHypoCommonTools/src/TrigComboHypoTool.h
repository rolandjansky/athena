/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#define TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"

#include "xAODBase/IParticleContainer.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigComboHypoTool
 * \brief TrigComboHypoTool is a ComboHypoTool that calculates the deltaR between two particles and
 * apply cuts on it (upper and lower cut) accepting the event if the required condition is 
 * satisfied
 *
 */


class TrigComboHypoTool:  public ComboHypoToolBase {
 public:
  
  TrigComboHypoTool(const std::string& type,
		    const std::string& name,
		    const IInterface*  parent);

  virtual ~TrigComboHypoTool() {};
  virtual StatusCode initialize() override;


 private:
  
  virtual bool executeAlg(std::vector<LegDecision>& thecomb) const override;
  
  // flags
  Gaudi::Property< bool >      m_acceptAll {this, "AcceptAll", false, "Ignore selection"  };
  Gaudi::Property<std::string> m_varTag    {this, "Variable" ,    "", "Variable to cut on"};

  //legs
  Gaudi::Property<int>         m_legA      {this, "LegA"     ,    -1, "First Leg"};
  Gaudi::Property<int>         m_legB      {this, "LegB"     ,    -1, "Second Leg"};

  // cuts
  Gaudi::Property<float> m_varMin {this,"LowerCut",  0.3, "Lower cut for legs pair"};
  Gaudi::Property<float> m_varMax {this,"UpperCut", 10.0, "Upper cut for legs pair"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };


}; // TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#endif



