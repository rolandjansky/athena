/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef TRIGBTAGEMULATIONCHAIN_H
#define TRIGBTAGEMULATIONCHAIN_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ToolHandle.h"
#include "src/JetManagerTool.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Trig {

class TrigBtagEmulationChain
  : public AthMessaging {
 public:
  /// Constructors and Destructor                                                                                                                                                                                                                                                             
  TrigBtagEmulationChain(const std::string& name,
                         const std::vector< std::string >& definition,
                         PublicToolHandle< Trig::TrigDecisionTool >& );
  virtual ~TrigBtagEmulationChain() = default;

  /// Name
  const std::string& name() const;

  /// JetManagers that will provide the jet and btagging objects
  StatusCode addJetManager(const std::string& name, 
			   ToolHandle<Trig::JetManagerTool>&);
  
  /// Chain evaluation 
  bool isPassed() const;
  
private:
  const PublicToolHandle< Trig::TrigDecisionTool >& tdt() const;

  void parseChainDefinition(const std::vector<std::string>& definition);
  
  bool evaluate_L1() const;
  bool evaluate_HLT() const;
  
  bool evaluate_preselection(const ToolHandle<Trig::JetManagerTool>& jm) const;
  bool evaluate_dijetmass(const ToolHandle<Trig::JetManagerTool>& jm) const;
  std::vector<std::vector<bool>> evaluate_HLT_chainParts(const ToolHandle<Trig::JetManagerTool>& jm, 
							 int idx_begin, 
							 int idx_end) const;
  bool allocate_jets_to_chainParts(const std::vector<std::vector<bool>>& matrix, 
				   const std::vector<std::string>& chainPartNames, 
				   const std::vector<int>& multiplicities) const;
  
  void parse_preselection(const std::string& presel_part, 
			  int& presel_multiplicity, 
			  double& presel_ptcut, 
			  double& presel_eta_min, 
			  double& presel_eta_max) const;
  void parse_dijetmass(double& dijet_mass, 
		       double& dijet_minjetpt, 
		       double& dijet_dphi, 
		       double& dijet_deta) const;
  
  
  // Chain name
  std::string m_name;
  
  std::string m_l1_requirement = "";
  std::vector<std::string> m_chainPartName {};
  std::vector<int> m_jet_multiplicity {};
  std::vector<double> m_jet_pt {};
  std::vector<double> m_jet_eta_min {};
  std::vector<double> m_jet_eta_max {};
  std::vector<double> m_jvt {};
  std::vector<bool> m_is_PFlow {};
  std::vector<std::string> m_tagger {};
  std::vector<std::string> m_jet_presel {};
  std::string m_dijetmass = "";
  int m_shared_idx = 0;


  static const inline std::unordered_map<std::string, double> s_tagger_wp = {
    {"dl1r60", 4.31},
    {"dl1r70", 2.98},
    {"dl1r77", 2.23},
    {"dl1r85", 1.32},

    {"dl1d40", 6.957},
    {"dl1d45", 6.344},
    {"dl1d50", 5.730},
    {"dl1d55", 5.121},
    {"dl1d60", 4.512},
    {"dl1d65", 3.882},
    {"dl1d70", 3.251},
    {"dl1d72", 2.489},
    {"dl1d75", 2.489},
    {"dl1d77", 2.157},
    {"dl1d80", 1.626},
    {"dl1d82", 1.254},
    {"dl1d85", 0.634},
    {"dl1d90", -0.465},
    {"dl1d95", -1.616},

    {"newTagger", 1.234},
    {"offperf", -999}
  };
  
  PublicToolHandle< Trig::TrigDecisionTool >& m_trigDec;
  
  std::unordered_map< std::string, ToolHandle<Trig::JetManagerTool>* > m_jetmanagers;
};

 inline const std::string& TrigBtagEmulationChain::name() const { return m_name; }
 inline const PublicToolHandle< Trig::TrigDecisionTool >& TrigBtagEmulationChain::tdt() const { return m_trigDec; }

} //namespace

#endif
