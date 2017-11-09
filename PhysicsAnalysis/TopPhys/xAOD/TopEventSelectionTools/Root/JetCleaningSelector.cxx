/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/JetCleaningSelector.h"
#include "TopEvent/EventTools.h"
#include "xAODJet/JetContainer.h"
#include "TopConfiguration/TopConfig.h"

namespace top {

  JetCleaningSelector::JetCleaningSelector(const std::string& level, std::shared_ptr<top::TopConfig> config) :
    m_level(level),
    m_useLooseBad(true),
    m_config(config)
  {
    if (!m_config->isTruthDxAOD()) {

      m_jetCleaningToolLooseBad = ToolHandle<IJetSelector>("JetCleaningToolLooseBad");
      m_jetCleaningToolTightBad = ToolHandle<IJetSelector>("JetCleaningToolTightBad");
      m_jetEventCleaningToolLooseBad = ToolHandle<ECUtils::IEventCleaningTool>("JetEventCleaningToolLooseBad");
      m_jetEventCleaningToolTightBad = ToolHandle<ECUtils::IEventCleaningTool>("JetEventCleaningToolTightBad");

      if (m_level != "LooseBad" && m_level != "TightBad") {
        std::cout << "JetCleaningSelector level not recognised - " << m_level << "\n";
        std::cout << "Should be LooseBad or TightBad\n";
        exit(1);
      }
      
      if (m_level == "LooseBad") m_useLooseBad = true;
      if (m_level == "TightBad") m_useLooseBad = false;
      
      // we can't yet use jet cleaning for particle-flow jets
      if (!m_config->useParticleFlowJets()) {
        if (m_useLooseBad){
          top::check( m_jetCleaningToolLooseBad.retrieve()      , "Failed to retrieve JetCleaningToolLooseBad" );
          top::check( m_jetEventCleaningToolLooseBad.retrieve() , "Failed to retrieve JetEventCleaningToolLooseBad" );
        }
        if (!m_useLooseBad){
          top::check( m_jetCleaningToolTightBad.retrieve()      , "Failed to retrieve JetCleaningToolTightBad" );
          top::check( m_jetEventCleaningToolTightBad.retrieve() , "Failed to retrieve JetEventCleaningToolTightBad" );
        }
      }
    }
  }

  bool JetCleaningSelector::apply(const top::Event& event) const {

    if (m_config->isTruthDxAOD()) return true;
    
    // There are two jet cleaning tools and we have a request to test the event level one
    // These should be very close/ equivalent as we already handle the OR and JVT elsewhere
    if(m_config->useEventLevelJetCleaningTool()){
      // If we are to use the event object, we can just do acceptEvent
      if (m_useLooseBad)  return m_jetEventCleaningToolLooseBad->acceptEvent(&event.m_jets);
      if (!m_useLooseBad) return m_jetEventCleaningToolTightBad->acceptEvent(&event.m_jets);
    }
    // This is the default/standard method for jet cleaning
    else{
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
    }

    return true;
  }

  std::string JetCleaningSelector::name() const 
  {
    return "JETCLEAN " + m_level;
  }

}
