/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "MCTruthBase/TruthStrategyManager.h"
// other includes from this package
#include "MCTruthBase/TruthStrategy.h"

// Framework includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Geant4 Includes
#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4TrackingManager.hh"
#include "G4Trajectory.hh"
#include "G4EventManager.hh"
#include "G4RunManagerKernel.hh"

// Truth-related includes
#include "HepMC/GenEvent.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"

// ISF includes
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Event/ISFParticle.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegionHelper.h"
#include "ISF_Geant4Event/Geant4TruthIncident.h"
#include "ISF_Geant4Event/ISFG4GeoHelper.h"

//G4 includes
#include "G4PhysicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

//#include <stdlib.h>


TruthStrategyManager::TruthStrategyManager()
  : isEmpty(true),
    secondarySavingLevel(0),
    nSecondaries(0),
    m_msg("TruthStrategyManager"),
    m_truthSvc(nullptr),
    m_geoIDSvc(),
    m_sHelper(),
    m_subDetVolLevel(-1) // please crash if left unset
{
}

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

void TruthStrategyManager::SetISFTruthSvc(ISF::ITruthSvc *truthSvc){
  m_truthSvc = truthSvc;
}


void TruthStrategyManager::SetISFGeoIDSvc(ISF::IGeoIDSvc *geoIDSvc){
  m_geoIDSvc = geoIDSvc;
}

StatusCode TruthStrategyManager::InitializeWorldVolume() {
  const G4LogicalVolume * logicalWorld = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();

  const auto& logicalWorldName = logicalWorld->GetName();
  if (logicalWorldName=="Atlas::Atlas") {
    // simulation w/o cavern volume (e.g. normal collision simulation)
    m_subDetVolLevel = 1;
  }
  else if (logicalWorldName=="World::World") {
    // simulation w/ cavern volume (e.g. cosmics simulation)
    m_subDetVolLevel = 2;
  } 
  else if (logicalWorldName=="CTB::CTB") {
    // test beam setup
    m_subDetVolLevel = 1;
  }
  else {
    ATH_MSG_ERROR("Unknown World Volume name: '" << logicalWorldName << "'");
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

bool TruthStrategyManager::AnalyzeVertex(const G4Step* aStep)
{
  G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
  m_sHelper.SetTrackingManager(rmk->GetTrackingManager());
  
  AtlasDetDescr::AtlasRegion geoID = iGeant4::ISFG4GeoHelper::nextGeoId(aStep, m_subDetVolLevel, m_geoIDSvc);

  auto* eventInfo = static_cast<EventInformation*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  // This is pretty ugly and but necessary because the Geant4TruthIncident
  // requires an ISFParticle at this point.
  // TODO: cleanup Geant4TruthIncident to not require an ISFParticle instance any longer
  int numSecondaries = GetNrOfSecondaries();
  const Amg::Vector3D myPos(0,0,0);
  const Amg::Vector3D myMom(0,0,0);
  double myMass = 0.0;
  double myCharge = 0.0;
  int    myPdgCode = 0;
  double myTime =0.;
  const ISF::DetRegionSvcIDPair origin(geoID, ISF::fUndefinedSimID);
  int myBCID = 0;
  ISF::ISFParticle myISFParticle(myPos, myMom, myMass, myCharge, myPdgCode, myTime, origin, myBCID);

  iGeant4::Geant4TruthIncident truth(aStep, myISFParticle, geoID, numSecondaries, m_sHelper, eventInfo);

  m_truthSvc->registerTruthIncident(truth);
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
