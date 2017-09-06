/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace top {

EventSelectionManager::EventSelectionManager(const std::vector<SelectionConfigurationData>& selectionConfigData,  TFile* outputFile, const std::string& toolLoaderNames, std::shared_ptr<top::TopConfig> config, EL::Worker* wk) {
    std::vector<std::string> tokens;

    std::stringstream ss(toolLoaderNames);
    std::string item;
    char delim = ' ';
    while (std::getline(ss, item, delim))
        tokens.push_back(item);

    std::vector<std::unique_ptr<top::ToolLoaderBase>> toolLoaders;
    for (const auto& toolLoaderName : tokens) {
        //remove the lib and add namespace and Loader to the class name
        std::string className = "top::" + toolLoaderName.substr(3) + "Loader";

        std::cout << "Attempting to load Tools from this class: " << className << "\n";
        TClass* c = ::TClass::GetClass(className.c_str());

        //okay, so that failed
        //maybe the user didn't put it in the top namespace - try again without top::?
        if (c == nullptr) {
            className = toolLoaderName.substr(3) + "Loader";
            std::cout << "Attempting to load Tools from this class: " << className << "\n";
            c = ::TClass::GetClass(className.c_str());
        }

        //okay, so that worked!
        if (c != nullptr) {
            std::cout << "   Success part 1 of 2" << "\n";
            top::ToolLoaderBase* bc = static_cast<top::ToolLoaderBase*> (c->New());

            if (bc) {
                std::cout << "   Success part 2 of 2" << "\n";
                toolLoaders.push_back(std::unique_ptr<top::ToolLoaderBase>(bc));
            } else
                std::cout << "   Failure converting to ToolLoaderBase\n";
        } else
            std::cout << "    Class " << className << " in library " << toolLoaderName << " not found - this is only a problem if you wrote one in your library\n";
    }

    std::cout << "\nTelling you how I'm configured, before I do anything:\n";
    for (const auto& currentConfig : selectionConfigData)
        m_selections.emplace_back(currentConfig.m_name, currentConfig.m_cutnames, outputFile, toolLoaders,config,wk);
}

EventSelectionManager::EventSelectionManager(EventSelectionManager&& other) :
    m_selections(std::move(other.m_selections)) {
}
  
void EventSelectionManager::countInitial(const float mcEventWeight,const float pileupWeight,const float zvtxWeight)
{
  for (const auto& currentSelection : m_selections)
    currentSelection.countInitial(mcEventWeight,pileupWeight,zvtxWeight);
}

void EventSelectionManager::countGRL(const float mcEventWeight,const float pileupWeight,const float zvtxWeight)
{
  for (const auto& currentSelection : m_selections)
    currentSelection.countGRL(mcEventWeight,pileupWeight,zvtxWeight);
}

void EventSelectionManager::countGoodCalo(const float mcEventWeight,const float pileupWeight,const float zvtxWeight)
{
  for (const auto& currentSelection : m_selections)
    currentSelection.countGoodCalo(mcEventWeight,pileupWeight,zvtxWeight);
}

void EventSelectionManager::countPrimaryVertex(const float mcEventWeight,const float pileupWeight,const float zvtxWeight)
{
  for (const auto& currentSelection : m_selections)
    currentSelection.countPrimaryVertex(mcEventWeight,pileupWeight,zvtxWeight);
}

bool EventSelectionManager::apply(top::Event& event,const xAOD::SystematicEvent& currentSystematic) {
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
	const bool passedThisSelection = currentSelection.applyParticleLevel( plEvent );

	//save result as new branch (int)
	plEvent.m_selectionDecisions[ currentSelection.name() ] = passedThisSelection;

	//Did any of the selections with SAVE specified pass for this event (if so we might want to keep the event)
	save |= (passedThisSelection && currentSelection.ToBeSaved());
    }

    return save;
}

bool EventSelectionManager::applyUpgradeLevel(const top::ParticleLevelEvent& upgradeEvent) {
    bool save(false);
    for (const auto& currentSelection : m_selections) {
       const bool passedThisSelection = currentSelection.applyUpgradeLevel( upgradeEvent );

       //save result as new branch (int)
       upgradeEvent.m_selectionDecisions[ currentSelection.name() ] = passedThisSelection;

       //Did any of the selections with SAVE specified pass for this event (if so we might want to keep the event)
       save |= (passedThisSelection && currentSelection.ToBeSaved());
    }

    return save;
}

void EventSelectionManager::finalise() {
    std::cout << "Final yields:\n";
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
  std::cout << "WARNING: Attempt to retrieve the FakesMMConfigs for non-existing selection " << selection << std::endl;
  std::cout << "         This is nonense - returning empty list" << std::endl;
  std::vector<std::string> empty;
  return empty;
}

}
