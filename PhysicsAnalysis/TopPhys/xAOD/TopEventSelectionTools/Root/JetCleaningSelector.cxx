/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/JetCleaningSelector.h"
#include "TopEvent/EventTools.h"
#include "xAODJet/JetContainer.h"
#include "TopConfiguration/TopConfig.h"

namespace top {

  JetCleaningSelector::JetCleaningSelector(const std::string& level, std::shared_ptr<top::TopConfig> config) :
    m_jetCleaningToolLooseBad("JetCleaningToolLooseBad"),
    m_jetCleaningToolTightBad("JetCleaningToolTightBad"),
    m_level(level),
    m_useLooseBad(true),
    m_config(config)
  {

    if (m_level != "LooseBad" && m_level != "TightBad") {
      std::cout << "JetCleaningSelector level not recognised - " << m_level << "\n";
      std::cout << "Should be LooseBad or TightBad\n";
      exit(1);
    }
    
    if (m_level == "LooseBad") m_useLooseBad = true;
    if (m_level == "TightBad") m_useLooseBad = false;
    
    // we can't yet use jet cleaning for particle-flow jets
    if (!m_config->useParticleFlowJets()) {
      if (m_useLooseBad)
          top::check( m_jetCleaningToolLooseBad.retrieve() , "Failed to retrieve JetCleaningToolLooseBad" );
      if (!m_useLooseBad)
          top::check( m_jetCleaningToolTightBad.retrieve() , "Failed to retrieve JetCleaningToolTightBad" );
    }
  }

  bool JetCleaningSelector::apply(const top::Event& event) const {
    for (const auto* const jetPtr : event.m_jets){
      // we can't yet use jet cleaning for particle-flow jets, so do nothing in this case
      if (m_config->useParticleFlowJets()) return true;
      
      if (m_useLooseBad) {
        if (m_jetCleaningToolLooseBad->keep(*jetPtr) == 0) {
          return false;
        }
      }
      
      if (!m_useLooseBad) {
        if (m_jetCleaningToolTightBad->keep(*jetPtr) == 0) {
          return false;
        }
      }
      
    }

    return true;
  }

  std::string JetCleaningSelector::name() const 
  {
    return "JETCLEAN " + m_level;
  }

}
