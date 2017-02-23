/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthBase/TruthStrategyManager.h"
#include "MCTruthBase/TruthStrategy.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"
#include "SimHelpers/SecondaryTracksHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "HepMC/GenEvent.h"


TruthStrategyManager::TruthStrategyManager()
  : isEmpty(true),
    secondarySavingLevel(0),
    nSecondaries(0),
    m_msg("TruthStrategyManager")
{}

TruthStrategyManager* TruthStrategyManager::GetStrategyManager()
{
  static TruthStrategyManager theMgr;
  return &theMgr;
}

void TruthStrategyManager::RegisterStrategy(TruthStrategy* strategy)
{
  theStrategies[strategy->StrategyName()] = strategy;
  isEmpty = false;
  ATH_MSG_INFO("MCTruth::TruthStrategyManager: registered strategy " <<
               strategy->StrategyName());
}

TruthStrategy* TruthStrategyManager::GetStrategy(const std::string& name)
{
  auto itr = theStrategies.find(name);
  if (itr != theStrategies.end()) {
    return itr->second;
  }
  ATH_MSG_WARNING("Strategy " << name << " not found: returning null");
  return nullptr;
}

void TruthStrategyManager::ListStrategies()
{
  ATH_MSG_INFO("List of all defined strategies (an X means active)");
  for (const auto& strategyPair : theStrategies) {
    if (strategyPair.second->IsActivated())
      ATH_MSG_INFO("---> " << strategyPair.first << "\t\t X");
    else ATH_MSG_INFO("---> " << strategyPair.first);
  }
}

bool TruthStrategyManager::AnalyzeVertex(const G4Step* aStep)
{
  for (const auto& strategyPair : theStrategies) {
    TruthStrategy* currentStrategy = strategyPair.second;
    if (currentStrategy->IsActivated() &&
        currentStrategy->IsApplicable(aStep) &&
        currentStrategy->AnalyzeVertex(aStep)) return true;
  }
  return false;
}

EventInformation* TruthStrategyManager::GetEventInformation() const
{
  return static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
}

std::vector<G4Track*> TruthStrategyManager::GetSecondaries()
{
  static SecondaryTracksHelper helper;
  return helper.GetSecondaries(nSecondaries);
}

void TruthStrategyManager::SetTruthParameter(const std::string& n, double val)
{
  truthParams[n] = val;
}

double TruthStrategyManager::GetTruthParameter(const std::string& n)
{
  if (truthParams.find(n) != truthParams.end()) {
    return truthParams[n];
  }
  ATH_MSG_WARNING("TruthStrategyManager: parameter " << n <<
                  " not found in the available set");
  return 0;
}
void TruthStrategyManager::PrintParameterList()
{
  ATH_MSG_INFO("List of all MCTruth configuration parameters");
  for (const auto& paramPair : truthParams)
    ATH_MSG_INFO("---> " << std::setw(30) << paramPair.first <<
                 "\t\t value= " << paramPair.second);
}
