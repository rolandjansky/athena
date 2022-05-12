/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#define TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H

#include <string>
#include <vector>
#include <tuple>
#include <limits>

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
  enum ComboHypoVars { UNDEF=-1, DR=0, DPHI, INVM, MT, DETA};
  
  TrigComboHypoTool(const std::string& type,
		    const std::string& name,
		    const IInterface*  parent);

  virtual ~TrigComboHypoTool() {};
  virtual StatusCode initialize() override;

 private:

  static constexpr float FLOATDEFAULT = std::numeric_limits<float>::lowest();

  /// Organise info per var selection in a struct
  struct VarInfo {
    std::string varTag{""};
    ComboHypoVars var{UNDEF};
    size_t index{0};
    std::string monToolName{""};

    bool useMin{false};
    float varMin{FLOATDEFAULT};
    bool useMax{false};
    float varMax{FLOATDEFAULT};

    bool legA_is_MET{false};
    uint32_t legA{0};
    bool legB_is_MET{false};
    uint32_t legB{0};
    bool legsAreEqual{false};

    /// Check consistency of single var config
    bool validate(std::string& errmsg) const;
    /// Generate range string for printing
    std::string rangeStr() const {
      return (useMin ? std::to_string(varMin) + " < " : "") + varTag + (useMax ? " < " + std::to_string(varMax): "");
    }
    bool test(float value) const {
      return (useMin ? value > varMin : true) && (useMax ? value < varMax : true);
    }

  };

  /// Typedef for convenience, will contain eta/phi/pt info
  typedef std::tuple<float,float,float> KineInfo;
  typedef std::vector<Combo::LegDecision> Combination;

  /// Top-level function to make chain-level decision
  /// This applies the AND of all configured var selections
  virtual bool executeAlg(const Combination& combination) const override;

  /// Implementation of selection on individual variables
  bool executeAlgStep(const Combination& combination, const VarInfo&, std::vector<float>& values) const;
  /// Computation of the variables from the specified kinematics
  float compute(const std::pair<KineInfo,KineInfo>& kinepair, ComboHypoVars var) const;

  /// Helpers to extract kinematics from the specified legs of the chain
  /// Specialised for two cases -- exactly two objects from the same leg
  /// or exactly one object each from two legs.
  bool fillLegDecisions_sameLeg(std::pair<Combo::LegDecision,Combo::LegDecision>& legpair, const Combination& combination, uint32_t leg) const;
  bool fillLegDecisions_diffLeg(std::pair<Combo::LegDecision,Combo::LegDecision>& legpair, const Combination& combination, uint32_t legA, uint32_t legB) const;
  bool fillPairKinematics(std::pair<KineInfo,KineInfo>& kinepair, const Combination& combination, const VarInfo& varInfo) const;
  bool fillKineInfo(KineInfo& kinematics, Combo::LegDecision decision, bool isMET) const;

  /// Gaudi configuration hooks
  // flags
  Gaudi::Property<std::vector<std::string>> m_varTag_vec     {this, "Variables"  , {""}, "Variables to cut on"};
  Gaudi::Property<std::vector<bool> >       m_useMin_vec     {this, "UseMinVec"  , {false}, "Array with the apply_min_cut setting"};
  Gaudi::Property<std::vector<bool> >       m_useMax_vec     {this, "UseMaxVec"  , {false}, "Array with the apply_max_cut setting"};

  //legs
  Gaudi::Property<std::vector<uint32_t>>    m_legA_vec       {this, "LegAVec"      , {0}, "Array with the first Leg ID"};
  Gaudi::Property<std::vector<uint32_t>>    m_legB_vec       {this, "LegBVec"      , {0}, "Array with the second Leg ID"};
  Gaudi::Property<std::vector< bool >>      m_isLegA_MET_vec {this, "IsLegA_METVec", {false}, "Array with the first Leg MET identifier"};
  Gaudi::Property<std::vector< bool >>      m_isLegB_MET_vec {this, "IsLegB_METVec", {false}, "Array with the second Leg MET identifier"};
  Gaudi::Property<bool>                     m_skipLegCheck   {this, "SkipLegCheck" , {false}, "Ignore leg IDs for chains with only one leg"};

  // cuts
  Gaudi::Property<std::vector<float>>       m_varMin_vec     {this, "LowerCutVec", {FLOATDEFAULT}, "Array with the lower cut for legs pair"};
  Gaudi::Property<std::vector<float>>       m_varMax_vec     {this, "UpperCutVec", {FLOATDEFAULT}, "Array with the upper cut for legs pair"};
  
  // monitoring
  ToolHandleArray<GenericMonitoringTool>    m_monTool_vec    {this, "MonTools", {}, "Monitoring tools" };

  /// Internal variables for more efficient config lookup
  std::vector<VarInfo>                      m_varInfo_vec;

}; // TRIGCOMBOHYPO_TRIGCOMBOHYPOTOOL_H
#endif



