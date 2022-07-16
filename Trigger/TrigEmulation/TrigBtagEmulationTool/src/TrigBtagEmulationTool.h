/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef TrigBtagEmulationTool_H
#define TrigBtagEmulationTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigBtagEmulationTool/ITrigBtagEmulationTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "src/JetManagerTool.h"
#include "src/TrigBtagEmulationChain.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Trig {

class TrigBtagEmulationTool : 
  public extends<AthAlgTool, Trig::ITrigBtagEmulationTool> {
public:
  TrigBtagEmulationTool(const std::string& type, 
			const std::string& name, 
			const IInterface* parent);
  virtual ~TrigBtagEmulationTool() = default;
	
  virtual StatusCode initialize() override;
  virtual StatusCode populateJetManagersTriggerObjects() override;  
  virtual bool isPassed(const std::string& chain) const override;

private:
  StatusCode addEmulatedChain(const std::string& name, 
			      const std::vector< std::string >& definition);

  StatusCode checkInputChainExists(const std::string&) const;
  StatusCode retrieveTriggerObjects(ToolHandle< Trig::JetManagerTool >&, 
				    bool useTriggerNavigation);

private:
  PublicToolHandle<Trig::TrigDecisionTool> m_trigDec {this, "TrigDecisionTool", "",""};
  
  // Input properties
  Gaudi::Property< std::string > m_inputChains_PFlow {this, "InputChain", "HLT_j45_pf_subjesgsc_ftf_L1J15", ""};
    
  // jet Managers
  ToolHandle< Trig::JetManagerTool > m_manager_EMTopo_cnt {this, "JM_EMTopo_CNT", "",""};
  ToolHandle< Trig::JetManagerTool > m_manager_PFlow_cnt {this, "JM_PFlow_CNT", "",""};
  ToolHandle< Trig::JetManagerTool > m_manager_EMTopo_presel {this, "JM_EMTopo_PRESEL", "",""};
  
  // EMULATED CHAINS
  Gaudi::Property< std::map< std::string, std::vector< std::string >>> m_emulatedChainDefinitions {this, "EmulatedChainDefinitions", {}, ""};

  std::unordered_map< std::string, std::unique_ptr< TrigBtagEmulationChain >> m_emulatedChains;  
};

} // namespace

#endif
