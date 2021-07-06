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

  enum comboHypoVars { DR=0, INVM, DPHI, MT}; 

 private:
  
  virtual bool executeAlg(std::vector<LegDecision>& thecomb) const override;
  
  // flags
  Gaudi::Property< bool >      m_acceptAll {this, "AcceptAll", false, "Ignore selection"  };
  Gaudi::Property<std::string> m_varTag    {this, "Variable" ,    "", "Variable to cut on"};
  Gaudi::Property< bool >      m_useMin    {this, "UseMin"   , false, "Apply min_cut"     };
  Gaudi::Property< bool >      m_useMax    {this, "UseMax"   , false, "Apply max_cut"     };

  //legs
  Gaudi::Property<std::string> m_legA       {this, "LegA"      ,    "", "First Leg"};
  Gaudi::Property<std::string> m_legB       {this, "LegB"      ,    "", "Second Leg"};
  Gaudi::Property< bool >      m_isLegA_MET {this, "IsLegA_MET", false, "Is first Leg MET?"};
  Gaudi::Property< bool >      m_isLegB_MET {this, "IsLegB_MET", false, "Is second Leg MET?"};

  // cuts
  Gaudi::Property<float> m_varMin {this,"LowerCut", -9999., "Lower cut for legs pair"};
  Gaudi::Property<float> m_varMax {this,"UpperCut", -9999., "Upper cut for legs pair"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };

  std::map<std::string, comboHypoVars>  m_varMap;
  comboHypoVars                         m_var;

  void     fillVarMap();

}; // TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#endif



