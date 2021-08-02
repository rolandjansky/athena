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
 * \brief TrigComboHypoTool is a ComboHypoTool that calculates topological quantities between two particles and
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
  
  virtual bool executeAlg(const std::vector<Combo::LegDecision>& combination) const override;
  bool         executeAlgStep(const std::vector<Combo::LegDecision>& combination, size_t index, std::vector<float>& values) const;
  
  // flags
  Gaudi::Property<std::vector<std::string>> m_varTag_vec     {this, "Variables"  , {   ""}, "Variables to cut on"};
  Gaudi::Property< std::vector<bool> >      m_useMin_vec     {this, "UseMinVec"  , {false}, "Array with the apply_min_cut setting"};
  Gaudi::Property< std::vector<bool> >      m_useMax_vec     {this, "UseMaxVec"  , {false}, "Array with the apply_max_cut setting"};

  //legs
  Gaudi::Property<std::vector<std::string>> m_legA_vec       {this, "LegAVec"      , {   ""}, "Array with the first Leg ID"};
  Gaudi::Property<std::vector<std::string>> m_legB_vec       {this, "LegBVec"      , {   ""}, "Array with the second Leg ID"};
  Gaudi::Property<std::vector< bool >>      m_isLegA_MET_vec {this, "IsLegA_METVec", {false}, "Array with the first Leg MET identifier"};
  Gaudi::Property<std::vector< bool >>      m_isLegB_MET_vec {this, "IsLegB_METVec", {false}, "Array with the second Leg MET identifier"};

  // cuts
  Gaudi::Property<std::vector<float>>       m_varMin_vec     {this, "LowerCutVec", {-9999.}, "Array with the lower cut for legs pair"};
  Gaudi::Property<std::vector<float>>       m_varMax_vec     {this, "UpperCutVec", {-9999.}, "Array with the upper cut for legs pair"};
  
  // monitoring
  ToolHandleArray<GenericMonitoringTool>    m_monTool_vec    {this, "MonTools", {}, "Monitoring tools" };

  std::map<std::string, comboHypoVars>      m_varMap;
  std::vector<comboHypoVars>                m_var_vec;

  void     fillVarMap();

}; // TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#endif



