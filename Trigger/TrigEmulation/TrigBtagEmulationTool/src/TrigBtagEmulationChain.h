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

namespace Trig {

class TrigBtagEmulationChain
  : public AthMessaging {
 public:
  /// Constructors and Destructor
  TrigBtagEmulationChain(const std::string& name,
                         const std::vector< std::string >& definition);
  virtual ~TrigBtagEmulationChain() = default;

  /// Name
  const std::string& name() const;
  const std::string& l1_requirement() const;
  const std::vector<std::string>& chainPartName() const;
  const std::vector<int>& jet_multiplicity() const;
  const std::vector<double>& jet_pt() const;
  const std::vector<double>& jet_eta_min() const;
  const std::vector<double>& jet_eta_max() const;
  const std::vector<double>& jvt() const;
  const std::vector<bool>& is_PFlow() const;
  const std::vector<std::string>& tagger() const;
  const std::vector<std::string>& jet_presel() const;
  const std::string& dijetmass() const;
  int shared_idx() const;
  
private:
  void parseChainDefinition(const std::vector<std::string>& definition);
  
private:
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
};

 inline const std::string& TrigBtagEmulationChain::name() const { return m_name; }
 inline const std::string& TrigBtagEmulationChain::l1_requirement() const { return m_l1_requirement; }
 inline const std::vector<std::string>& TrigBtagEmulationChain::chainPartName() const { return m_chainPartName; }
 inline const std::vector<int>& TrigBtagEmulationChain::jet_multiplicity() const { return m_jet_multiplicity; }
 inline const std::vector<double>& TrigBtagEmulationChain::jet_pt() const { return m_jet_pt; }
 inline const std::vector<double>& TrigBtagEmulationChain::jet_eta_min() const { return m_jet_eta_min; }
 inline const std::vector<double>& TrigBtagEmulationChain::jet_eta_max() const { return m_jet_eta_max; }
 inline const std::vector<double>& TrigBtagEmulationChain::jvt() const { return m_jvt; }
 inline const std::vector<bool>& TrigBtagEmulationChain::is_PFlow() const { return m_is_PFlow; }
 inline const std::vector<std::string>& TrigBtagEmulationChain::tagger() const { return m_tagger; }
 inline const std::vector<std::string>& TrigBtagEmulationChain::jet_presel() const { return m_jet_presel; }
 inline const std::string& TrigBtagEmulationChain::dijetmass() const { return m_dijetmass; }
 inline int TrigBtagEmulationChain::shared_idx() const { return m_shared_idx; }
  
} //namespace

#endif
