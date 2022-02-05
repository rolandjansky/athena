/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/EventSelectionManager.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"
#include "TopConfiguration/TopConfig.h"

#include "TopEventSelectionTools/ToolLoaderBase.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "TFile.h"
#include "TClass.h"
#include "TROOT.h"

#include "TopParticleLevel/ParticleLevelEvent.h"

#include "TopAnalysis/MsgCategory.h"
// use ATH_MSG macros defined in the namespace TopAnalysis
using namespace TopAnalysis;

namespace top {
  EventSelectionManager::EventSelectionManager(const std::vector<SelectionConfigurationData>& selectionConfigData,
                                               TFile* outputFile, const std::string& toolLoaderNames,
                                               std::shared_ptr<top::TopConfig> config, EL::Worker* wk) {
    std::vector<std::string> tokens;

    std::stringstream ss(toolLoaderNames);
    std::string item;
    char delim = ' ';
    while (std::getline(ss, item, delim))
      tokens.push_back(item);

    std::vector<std::unique_ptr<top::ToolLoaderBase> > toolLoaders;
    for (const auto& toolLoaderName : tokens) {
      //remove the lib and add namespace and Loader to the class name
      std::string className = "top::" + toolLoaderName.substr(3) + "Loader";

      ATH_MSG_INFO("Attempting to load Tools from this class: " << className);
      TClass* c = ::TClass::GetClass(className.c_str());

      //okay, so that failed
      //maybe the user didn't put it in the top namespace - try again without top::?
      if (c == nullptr) {
        className = toolLoaderName.substr(3) + "Loader";
        ATH_MSG_INFO("Attempting to load Tools from this class: " << className);
        c = ::TClass::GetClass(className.c_str());
      }

      //okay, so that worked!
      if (c != nullptr) {
        ATH_MSG_INFO("   Success part 1 of 2");
        top::ToolLoaderBase* bc = static_cast<top::ToolLoaderBase*> (c->New());

        if (bc) {
          ATH_MSG_INFO("   Success part 2 of 2");
          toolLoaders.push_back(std::unique_ptr<top::ToolLoaderBase>(bc));
        } else ATH_MSG_ERROR("   Failure converting to ToolLoaderBase");
      } else ATH_MSG_WARNING("    Class " << className << " in library " << toolLoaderName <<
        " not found - this is only a problem if you wrote one in your library");
    }

    ATH_MSG_INFO("Telling you how I'm configured, before I do anything:");
    for (const auto& currentConfig : selectionConfigData)
      m_selections.emplace_back(currentConfig.m_name, currentConfig.m_cutnames, outputFile, toolLoaders, config, wk);
  }

  EventSelectionManager::EventSelectionManager(EventSelectionManager&& other) :
    m_selections(std::move(other.m_selections)) {
  }

  void EventSelectionManager::countInitial(const float mcEventWeight, const float pileupWeight) {
    for (const auto& currentSelection : m_selections)
      currentSelection.countInitial(mcEventWeight, pileupWeight);
  }

  void EventSelectionManager::countGRL(const float mcEventWeight, const float pileupWeight) {
    for (const auto& currentSelection : m_selections)
      currentSelection.countGRL(mcEventWeight, pileupWeight);
  }

  void EventSelectionManager::countGoodCalo(const float mcEventWeight, const float pileupWeight) {
    for (const auto& currentSelection : m_selections)
      currentSelection.countGoodCalo(mcEventWeight, pileupWeight);
  }

  void EventSelectionManager::countPrimaryVertex(const float mcEventWeight, const float pileupWeight) {
    for (const auto& currentSelection : m_selections)
      currentSelection.countPrimaryVertex(mcEventWeight, pileupWeight);
  }

  bool EventSelectionManager::apply(top::Event& event, const xAOD::SystematicEvent& currentSystematic) {
    bool save(false);

    for (const auto& currentSelection : m_selections) {
      const bool passedThisSelection = currentSelection.apply(event);

      //save result as new branch (int)
      event.m_info->auxdecor<int>(currentSelection.name()) = passedThisSelection;
      //save result as decoration
      currentSystematic.auxdecor<int>(currentSelection.name()) = passedThisSelection ? 1 : 0;

      //Did any of the selections with SAVE specified pass for this event (if so we might want to keep the event)
      save |= (passedThisSelection && currentSelection.ToBeSaved());
    }

    return save;
  }

  bool EventSelectionManager::applyParticleLevel(const top::ParticleLevelEvent& plEvent) {
    bool save(false);

    for (const auto& currentSelection : m_selections) {
      const bool passedThisSelection = currentSelection.applyParticleLevel(plEvent);

      //save result as new branch (int)
      plEvent.m_selectionDecisions[ currentSelection.name() ] = passedThisSelection;

      //Did any of the selections with SAVE specified pass for this event (if so we might want to keep the event)
      save |= (passedThisSelection && currentSelection.ToBeSaved());
    }

    return save;
  }

  void EventSelectionManager::finalise() {
    ATH_MSG_INFO("Final yields:");
    for (const auto& currentSelection : m_selections)
      currentSelection.finalise();
  }

  void EventSelectionManager::addExtraBranches(std::vector<std::string>& extraBranchList) {
    for (const auto& currentSelection : m_selections)
      extraBranchList.push_back(currentSelection.name());
  }

  std::vector<std::string> EventSelectionManager::GetFakesMMConfigs(std::string selection) const {
    for (const auto& currentSelection : m_selections)
      if (currentSelection.name() == selection) return currentSelection.GetFakesMMConfigs();

    ATH_MSG_WARNING("Attempt to retrieve the FakesMMConfigs for non-existing selection " << selection
        << "\n         This is nonense - returning empty list");
    std::vector<std::string> empty;
    return empty;
  }
}
