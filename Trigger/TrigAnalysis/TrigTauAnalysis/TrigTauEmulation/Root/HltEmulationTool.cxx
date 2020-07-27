/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
//Local Includes

#include "TrigTauEmulation/MsgStream.h"
#include "TrigTauEmulation/ChainRegistry.h"
#include "TrigTauEmulation/HltEmulationTool.h"
#include "TrigDecisionTool/Conditions.h"

namespace TrigTauEmul {

  // Default constructor
  HltEmulationTool::HltEmulationTool(const std::string& name) 
    : asg::AsgTool(name),
    m_l1_emulation_tool()
   // m_hlt_tau_tools()

  {

    declareProperty("hlt_chains", m_hlt_chains_vec, "Vector of the HLT chains to be evaluated");
    declareProperty("PerformL1Emulation", m_perform_l1_emulation=true);
    declareProperty("Level1EmulationTool", m_l1_emulation_tool);
    //declareProperty("HltTauTools", m_hlt_tau_tools);

    declareProperty("HLTTriggerCondition", m_HLTTriggerCondition=TrigDefs::Physics);
    declareProperty("L1TriggerCondition", m_L1TriggerCondition=TrigDefs::Physics);

#ifdef XAOD_STANDALONE
    std::string tdt_name;
    declareProperty("TrigDecTool", tdt_name="TrigDecTool");
    m_trigdec_tool = new ToolHandle<Trig::TrigDecisionTool> (tdt_name);
#endif

#ifndef XAOD_STANDALONE
    m_trigdec_tool = new ToolHandle<Trig::TrigDecisionTool> ();
    declareProperty("TrigDecTool", *m_trigdec_tool);
#endif

    m_matching_tool = new HltL1MatchingTool(name + "_matching");

  }

  HltEmulationTool::~HltEmulationTool() {
    delete m_trigdec_tool;
    delete m_matching_tool;
  }

  // Copy constructor
  HltEmulationTool::HltEmulationTool(const HltEmulationTool& other): asg::AsgTool(other.name() + "_copy")
  {
#ifdef XAOD_STANDALONE
    std::string tdt_name = "TrigDecisionTool_copy";
    m_trigdec_tool = new ToolHandle<Trig::TrigDecisionTool> (tdt_name);
#endif

#ifndef XAOD_STANDALONE
    m_trigdec_tool = new ToolHandle<Trig::TrigDecisionTool> ();
    declareProperty("TrigDecTool", *m_trigdec_tool);
#endif
   
    m_hlt_chains_vec = other.m_hlt_chains_vec;
    m_perform_l1_emulation = other.m_perform_l1_emulation;
    m_l1_emulation_tool = other.m_l1_emulation_tool;

    m_HLTTriggerCondition = other.m_HLTTriggerCondition;
    m_L1TriggerCondition = other.m_L1TriggerCondition;
    
    m_matching_tool = new HltL1MatchingTool(other.name() + "_matching_copy");
  
  }

  // Initialize
  StatusCode HltEmulationTool::initialize() {
    auto chainRegistry = asg::ToolStore::get<ChainRegistry>("ChainRegistry");
    m_chains.clear();
    if (m_hlt_chains_vec.size() !=0) {
      for (auto ch: m_hlt_chains_vec) { 
        try {
          MY_MSG_INFO("Grabbing chain " << ch);
          m_chains[ch] = chainRegistry->getChain(ch);
        } catch (...) {
          MY_MSG_WARNING("Skipping unknown HLT chain " << ch);
          continue;
        }
      }
    } else {
      MY_MSG_WARNING("No HLT Chains are passed to the tool");
    }

    if(m_chains.size() == 0){
      MY_MSG_FATAL("No initialised HLT chains found!");
      return StatusCode::FAILURE;
    }

    m_l1_emulation_tool->GetChains();

    // This is a fallback initialization mostly meant for athena running - in RootCore, TriggerValidation does this already
    //auto registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    ToolsRegistry *registry = nullptr;
    if(Utils::toolStoreContains<ToolsRegistry>("ToolsRegistry")) {
      registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    } else {
      registry = new ToolsRegistry("ToolsRegistry");
    }

    //m_hlt_tau_tools = registry->GetHltTauTools();

    // Build the list of l1 tools 
    std::set<std::string> l1_items;
    std::set<std::string> l1_seeds;
    for (auto &chain : m_chains)  {
      MY_MSG_INFO("Consider " << chain.first << " with " << chain.second.n_tau_items() << " taus.");

      // 1) consider the l1 accept
      MY_MSG_INFO("L1 accept = " << chain.second.l1_accept());
      l1_items.insert("L1_" + chain.second.l1_accept());
      m_L1_tau_decision[chain.second.l1_accept()] = false;
      m_L1_tau_decision_calculated[chain.second.l1_accept()] = false;

      // 2) loop over the items and consider the seed
      for (auto &item : chain.second.items()) {
        MY_MSG_INFO("HLT item " << item.name());
        l1_items.insert("L1_" + item.seed());
        l1_seeds.insert(item.seed());
        m_L1_tau_decision[item.seed()] = false;
        m_L1_tau_decision_calculated[item.seed()] = false;
      }
    }

    ATH_CHECK(m_l1_emulation_tool.retrieve());  

    auto l1tool_chains = m_l1_emulation_tool->GetL1Chains();
    for (auto &ch: l1_items) {
      MY_MSG_INFO("Attempting to load L1 chain " << ch);
      bool found = std::find(l1tool_chains.begin(), l1tool_chains.end(), ch) != l1tool_chains.end();
      if (not found) {
        MY_MSG_ERROR("The chain "<< ch 
            <<" has not been passed to the Level1EmulationTool "<< m_l1_emulation_tool->name());
        return StatusCode::FAILURE;
      }
    }

    MY_MSG_INFO("Available L1 seeds:");
    for (auto s: l1_seeds) {
      MY_MSG_INFO("\t" << s);
    }

    // pass these on to the matching tool
    m_matching_tool->msg().setLevel(this->msg().level());

    std::vector<std::string> l1_seeds_vec(l1_seeds.begin(), l1_seeds.end());
    ATH_CHECK(m_matching_tool->setProperty("L1seeds", l1_seeds_vec));
    ATH_CHECK(m_matching_tool->initialize());
    
    //clean up obsolete L1 tools
    MY_MSG_INFO("removing unused L1 tools");
    MY_MSG_DEBUG("the following tools are in used:");
    for(auto s: l1_seeds){
      MY_MSG_DEBUG("\t" << s);
    }
    ATH_CHECK(m_l1_emulation_tool->removeUnusedTools(l1_seeds));

    ATH_CHECK(m_trigdec_tool->retrieve());

    auto tools = registry->selectTools<HltTauSelectionTool*>();
    for(auto it = tools.begin(); it != tools.end(); ) {
      if(m_chains.find("HLT_"+it->name()) == m_chains.end()){
        MY_MSG_INFO("not using tool " << it->name());
        it.erase();
      } else {
        MY_MSG_INFO("will use tool " << it->name());
        ++it;
      }

    }

    //// erasing from a ToolHandleArray creates an odd segfault, so write a new one
    //ToolHandleArray<IHltTauSelectionTool> used_hlt_tau_tools;
    //for(auto it: m_hlt_tau_tools) {
      //if(m_chains.find("HLT_"+it->name()) == m_chains.end()){
        //MY_MSG_INFO("not using tool " << it->name());
      //} else {
        //MY_MSG_INFO("will use tool " << it->name());
        //used_hlt_tau_tools.push_back(it);
      //}
    //}

    //m_hlt_tau_tools = used_hlt_tau_tools;
   
    //for (auto it: m_hlt_tau_tools) {
    for (auto it: registry->selectTools<HltTauSelectionTool*>()){
      MY_MSG_INFO("Initializing " << it->name());
      ATH_CHECK(it->initialize());
      // it->msg().setLevel(this->msg().level());
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode HltEmulationTool::execute(const xAOD::EmTauRoIContainer* l1taus,
      const xAOD::JetRoIContainer* l1jets,
      const xAOD::MuonRoIContainer* l1muons,
      const xAOD::EnergySumRoI* l1xe,
      const std::vector<DecoratedHltTau>& hlt_taus)
  {
    ATH_CHECK(m_l1_emulation_tool->calculate(l1taus, l1jets, l1muons, l1xe));
    ATH_CHECK(m_matching_tool->execute(hlt_taus, l1taus));
    ATH_CHECK(calculate(hlt_taus, l1taus));

    return StatusCode::SUCCESS;
  }

  // Event calculate -- The meaty part of this algorithm
  // Method uses EDM tau candidate plus EDM track collection
  StatusCode HltEmulationTool::calculate(const std::vector<DecoratedHltTau>& hlt_taus,
      const xAOD::EmTauRoIContainer* l1taus) {
    
    // Reset the counters to 0;
    reset_tau_decision();
    clearL1Decision();
    
    ToolsRegistry *registry = nullptr;
    if(Utils::toolStoreContains<ToolsRegistry>("ToolsRegistry")) {
      registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    } else {
      registry = new ToolsRegistry("ToolsRegistry");
    }
    
    for (const auto& hlt_tau : hlt_taus) {
      //for (auto it: m_hlt_tau_tools) {
      for (auto it: registry->selectTools<HltTauSelectionTool*>()){
        hlt_tau.getHltTau()->auxdecor<bool>(it->name()) = false;

        if (it->accept(hlt_tau)) {
          MY_MSG_DEBUG("ACCEPT FOR " << it->name());
          hlt_tau.getHltTau()->auxdecor<bool>(it->name()) = true;
        } else {
          MY_MSG_DEBUG("REJECT FOR " << it->name());
        }
      }
    }

    ATH_CHECK(calculate_tau_decision(hlt_taus, l1taus));

    return StatusCode::SUCCESS;
  }
  
  StatusCode HltEmulationTool::calculate_tau_decision(const std::vector<DecoratedHltTau>& hlt_taus, 
      const xAOD::EmTauRoIContainer* l1taus) {
    
    // 1 -- Loop over all the chains
    for (auto &ch: m_chains) {
      m_HLT_tau_decision[ch.first] = false;
      MY_MSG_DEBUG("Calculating tau decision for " << ch.first);

      // Case 1: only 1 HLT tau in the chain
      if (ch.second.n_tau_items() == 1) {
        // 1 -- Iterate over the items
        for (auto &item: ch.second.items()) { 
          MY_MSG_DEBUG("\t" << item.name() << " / " << item.seed() << "/ isTau = "<< item.isTau());
          // 2 -- check if this is a tau item
          if (not item.isTau()) {
            MY_MSG_DEBUG("\t\t not a tau -> ignoring");
            continue;
          }

          // 3 -- loop over the hlt taus
          for (const auto& decoratedTau : hlt_taus) {
            const auto hlt_tau = decoratedTau.getHltTau();

            MY_MSG_DEBUG("\t\t HLT TAU "<< hlt_tau->index() 
                << " (Pt/Eta/Phi): "<< hlt_tau->pt() 
                << ", " << hlt_tau->eta() << ", " << hlt_tau->phi());

            if(m_perform_l1_emulation){
              // 3.1 -- only consider hlt taus with a L1 match
              MY_MSG_DEBUG("Testing l1 match as we'll perform L1 emulation later");
              if (hlt_tau->auxdataConst<int>("l1_index_" + item.seed()) == -1 and hlt_tau->auxdataConst<int>("l1_index") == -1) {
                MY_MSG_DEBUG("\t\t\t -> not matching L1 item -> ignoring");
                continue;
              }

              // use our seed; if this is -1, the generic has to not be -1
              auto seed_str = "l1_index_" + item.seed();
              MY_MSG_DEBUG("\t using L1 seed str " << seed_str);
              auto l1_index = hlt_tau->auxdataConst<int>(seed_str);
              if (l1_index == -1) { 
                seed_str = "l1_index";
                l1_index = hlt_tau->auxdataConst<int>("l1_index");
                MY_MSG_DEBUG("\t seed str not found; using L1 seed str " << seed_str);
              }
              auto l1_matched_tau = l1taus->at(l1_index);

              MY_MSG_DEBUG("\t\t index: " << l1_index << " => l1 pt/eta/phi = " 
                  << l1_matched_tau->tauClus() << ", " 
                  << l1_matched_tau->eta() << ", " 
                  << l1_matched_tau->phi());
              MY_MSG_DEBUG("\t\t Is seed " << item.seed() << " passed ? :" 
                  << l1_matched_tau->auxdataConst<bool>(item.seed()));

              // 3.2 -- Check if L1 matched item passes the seed
              if (not l1_matched_tau->auxdataConst<bool>(item.seed())) {
                MY_MSG_DEBUG("\t\t matched L1 item did not pass trigger -> ignoring");
                continue;
              }
              MY_MSG_DEBUG("\t\t Is item passed ? :"
                  << hlt_tau->auxdataConst<bool>(item.name()));
            } else {
              MY_MSG_DEBUG("not looking at L1 decision");
            }

            // 3.3 -- Check if the HLT item passes
            if (not hlt_tau->auxdataConst<bool>(item.name())) {
              MY_MSG_DEBUG("\t\t HLT item did not pass -> ignoring");
              continue;
            }

            // 3.4 -- declare the decision true and break the loop
            MY_MSG_DEBUG("\t\t => PASSED <=");
            m_HLT_tau_decision[ch.first] = true;
            break;
          } // loop over hlt_taus
        } // loop over the items (ch.second.items())
      } else if (ch.second.n_tau_items() == 2) {
        MY_MSG_DEBUG(ch.first << " has 2 hlt taus");
        std::vector<HltItem> tau_items;
        for (auto &item: ch.second.items()) { 
          if (item.isTau()) {
            tau_items.push_back(item);
          }
        }

        // protection
        if (tau_items.size() != 2) {
          MY_MSG_ERROR("Wrong number of tau items (not taken into account)");
          return StatusCode::FAILURE;
        }

        HltItem item1 = tau_items[0];
        HltItem item2 = tau_items[1];
        int n_item1_alone = 0;
        int n_item2_alone = 0;
        int n_doublets = 0;

        for (const auto& decoratedTau : hlt_taus) {
          const auto hlt_tau = decoratedTau.getHltTau();
          bool pass_item1 = false;
          bool pass_item2 = false;

          // skip unmatched taus to L1
          if (hlt_tau->auxdataConst<int>("l1_index") == -1) {
            continue;
          }

          auto l1_matched_tau = l1taus->at(hlt_tau->auxdataConst<int>("l1_index"));

          if (l1_matched_tau->auxdataConst<bool>(item1.seed()) and hlt_tau->auxdataConst<bool>(item1.name())) {
            pass_item1 = true;
          }

          if (l1_matched_tau->auxdataConst<bool>(item2.seed()) and hlt_tau->auxdataConst<bool>(item2.name())) {
            pass_item2 = true;
          }

          if (pass_item1 and not pass_item2) {
            n_item1_alone += 1;
          } else if (not pass_item1 and pass_item2) {
            n_item2_alone += 1;
          } else if (pass_item1 and pass_item2) {
            n_doublets += 1;
          }
        } // loop over hlt_taus

        if ((n_item1_alone > 0 and n_item2_alone > 0) or 
            (n_item1_alone > 0 and n_doublets > 0) or
            (n_item2_alone > 0 and n_doublets > 0) or
            (n_doublets > 1)) {
          m_HLT_tau_decision[ch.first] = true;
        }

      } else {
        MY_MSG_ERROR("Wrong number of taus (not taken into account");
        return StatusCode::FAILURE;
      }

    }

    return StatusCode::SUCCESS;

  }

  bool HltEmulationTool::decision(const std::string& chain_name) {
    MY_MSG_DEBUG("HltEmulationTool::decision(): consider " << chain_name);
    HltChain chain = m_chains[chain_name];

    if(m_perform_l1_emulation){
      MY_MSG_DEBUG("Check the L1 accept: "<< chain.l1_accept());
    
      bool pass_L1 = false;
      if(not m_L1_tau_decision_calculated[chain.l1_accept()]){
        MY_MSG_DEBUG("Calculating and caching L1 decision for L1" << chain.l1_accept());
        m_L1_tau_decision_calculated[chain.l1_accept()] = true;
        m_L1_tau_decision[chain.l1_accept()] = m_l1_emulation_tool->decision("L1_" + chain.l1_accept());
      } else {
        MY_MSG_DEBUG("Using cached L1 decision for L1" << chain.l1_accept());
      }
      pass_L1 = m_L1_tau_decision[chain.l1_accept()]; 

      //bool pass_L1 = m_l1_emulation_tool->decision("L1_" + chain.l1_accept());

      if (not pass_L1) {
        MY_MSG_DEBUG(" => Did not pass the emulated L1 accept");
        return false;
      }

      MY_MSG_DEBUG(" => Passed the emulated L1 accept");
    } else {
      MY_MSG_DEBUG("Not emulating the L1 accept; consider HLT only");
    }

    // Compute the tau part
    if (chain.n_tau_items() > 0) {
      if (not m_HLT_tau_decision[chain_name]) {
        MY_MSG_DEBUG("Not passing HLT tau decision for " + chain_name);
        return false;
      }
    }
    MY_MSG_DEBUG("Pass the HLT tau decision");

    // non-tau part from the TDT
    for (auto &item: chain.items()) {
      if (not item.isTau()) {
        MY_MSG_DEBUG("Evaluate "<< item.seed());
        bool pass_l1 = (*m_trigdec_tool)->isPassed("L1_" + item.seed(), m_L1TriggerCondition);
        MY_MSG_DEBUG(item.seed() << " has pass = " << pass_l1);
        if (not pass_l1) {
          return false;
        }

        MY_MSG_DEBUG("Evaluate "<< item.name());
        bool pass = (*m_trigdec_tool)->isPassed("HLT_" + item.name(), m_HLTTriggerCondition);
        MY_MSG_DEBUG(item.name() << " has pass = " << pass);
        if (not pass) { 
          return false;
        }
      }

      MY_MSG_DEBUG("Pass the non-tau decision");


    }

    // If passes all the step, returns true
    return true;

  }

  // Finalize
  StatusCode HltEmulationTool::finalize() {

    return StatusCode::SUCCESS;
  }

  void HltEmulationTool::clearL1Decision() {
    MY_MSG_DEBUG("Clearing L1 decisions");
    for (auto &it: m_L1_tau_decision) {
      it.second = false;
    }
    for (auto &it: m_L1_tau_decision_calculated) {
      it.second = false;
    }
  }

  void HltEmulationTool::reset_tau_decision() {
    for (auto &it: m_HLT_tau_decision) {
      it.second = false;
    }
  }

} // end of the namespace
