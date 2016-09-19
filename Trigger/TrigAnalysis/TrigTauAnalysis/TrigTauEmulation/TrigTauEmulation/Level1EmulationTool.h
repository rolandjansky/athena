/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef LEVEL1EMULATIONTOOL_LEVEL1EMULATIONTOOL_H
#define LEVEL1EMULATIONTOOL_LEVEL1EMULATIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/ToolHandle.h"

//Local includes
#include "TrigTauEmulation/ILevel1EmulationTool.h"
#include "TrigTauEmulation/Parser.h"


#include "TrigTauEmulation/Level1TopoSelectionTool.h"
#include "TrigTauEmulation/TrigTauORLTool.h"

/* #include "TrigTauEmulation/ILevel1TopoSelectionTool.h" */
/* #include "TrigTauEmulation/ITrigTauORLTool.h" */

#include "TrigTauEmulation/IEmTauSelectionTool.h"
#include "TrigTauEmulation/IEnergySumSelectionTool.h"
#include "TrigTauEmulation/IJetRoISelectionTool.h"
#include "TrigTauEmulation/IMuonRoISelectionTool.h"
#include "TrigTauEmulation/ToolsRegistry.h"

namespace TrigTauEmul {
  class Level1EmulationTool : public asg::AsgTool, virtual public ILevel1EmulationTool

  {

    /// Proper constructor for Athena
    ASG_TOOL_CLASS(Level1EmulationTool, ILevel1EmulationTool)

  public:
  
    /// Standard constructor for standalone usage
    Level1EmulationTool(const std::string& name);

    /// Copy constructor for reflex in Athena
    Level1EmulationTool(const Level1EmulationTool& other);
    Level1EmulationTool& operator=(const Level1EmulationTool& other) = delete;

    /// virtual destructor
    virtual ~Level1EmulationTool();
    
    /// Initialize the tool
    virtual StatusCode initialize();
    virtual StatusCode initializeTools();

    /// Initialize the tool
    virtual StatusCode finalize();

    void GetChains();

    /// 
    virtual StatusCode calculate(const xAOD::EmTauRoIContainer* l1taus, 
				 const xAOD::JetRoIContainer* l1jets,
				 const xAOD::MuonRoIContainer* l1muons,
				 const xAOD::EnergySumRoI* l1xe);

    virtual bool decision(const std::string & chain_name);

    virtual StatusCode PrintReport(const xAOD::EmTauRoIContainer* l1taus, 
				   const xAOD::JetRoIContainer* l1jets,
				   const xAOD::MuonRoIContainer* l1muons,
				   const xAOD::EnergySumRoI* l1xe);

    virtual StatusCode PrintCounters();

    virtual std::vector<std::string> GetL1Chains() { return m_l1_chains_vec; }
    virtual StatusCode removeUnusedTools(const std::set<std::string>& usedTools);

  private:
  
    template<typename T> ToolHandleArray<T> removeTools(const ToolHandleArray<T>& tools, const std::set<std::string>& usedTools);

    std::vector<std::string> m_l1_chains_vec;
    bool m_useShallowCopies;
    std::map<std::string, int> m_counters;

    void reset_counters();
    Parser* m_name_parser;
 
    ToolsRegistry* m_registry; //!

    // Declaration of the L1 tools handles (jets, taus/ele, met, muons)
    //ToolHandleArray<IJetRoISelectionTool> m_l1jet_tools;
    //ToolHandleArray<IEmTauSelectionTool> m_l1tau_tools;
    //ToolHandleArray<IEnergySumSelectionTool> m_l1xe_tools;
    //ToolHandleArray<IMuonRoISelectionTool> m_l1muon_tools;

    /* ToolHandle<ITrigTauORLTool> m_l1orl_tool; */
    /* ToolHandle<ILevel1TopoSelectionTool> m_l1topo_tool; */
    TrigTauORLTool *m_l1orl_tool;
    Level1TopoSelectionTool *m_l1topo_tool;

  };
} // end of the namespace

#endif
