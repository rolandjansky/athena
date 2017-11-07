/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "MCTruthBase/TruthStrategyManager.h"

// Framework includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Geant4 Includes
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Step.hh"
#include "G4RunManagerKernel.hh"
#include "G4TrackingManager.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

// Truth-related includes
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

TruthStrategyManager::TruthStrategyManager()
  : m_msg("TruthStrategyManager")
  , m_truthSvc(nullptr)
  , m_geoIDSvc()
  , m_sHelper()
  , m_subDetVolLevel(-1) // please crash if left unset
{
}

TruthStrategyManager* TruthStrategyManager::GetStrategyManager()
{
  static TruthStrategyManager theMgr;
  return &theMgr;
}

void TruthStrategyManager::SetISFTruthSvc(ISF::ITruthSvc *truthSvc)
{
  m_truthSvc = truthSvc;
}


void TruthStrategyManager::SetISFGeoIDSvc(ISF::IGeoIDSvc *geoIDSvc)
{
  m_geoIDSvc = geoIDSvc;
}

StatusCode TruthStrategyManager::InitializeWorldVolume()
{
  const G4LogicalVolume * logicalWorld = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();
  const auto& logicalWorldName = logicalWorld->GetName();

  const G4String atlasWorldName("Atlas::Atlas");
  if (logicalWorldName==atlasWorldName) {
    // simulation w/o cavern volume (e.g. normal collision simulation)
    m_subDetVolLevel = 1;
    return StatusCode::SUCCESS;
  }

  const G4String cavernWorldName("World::World");
  if (logicalWorldName==cavernWorldName) {
    // simulation w/ cavern volume (e.g. cosmics simulation)
    m_subDetVolLevel = 2;
    return StatusCode::SUCCESS;
  }

  const G4String ctbWorldName("CTB::CTB");
  if (logicalWorldName==ctbWorldName) {
    // test beam setup
    m_subDetVolLevel = 1;
    return StatusCode::SUCCESS;
  }

  ATH_MSG_ERROR("Unknown World Volume name: '" << logicalWorldName << "'");
  return StatusCode::FAILURE;
}

bool TruthStrategyManager::CreateTruthIncident(const G4Step* aStep, int numSecondaries)
{
  G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
  m_sHelper.SetTrackingManager(rmk->GetTrackingManager());

  AtlasDetDescr::AtlasRegion geoID = iGeant4::ISFG4GeoHelper::nextGeoId(aStep, m_subDetVolLevel, m_geoIDSvc);

  auto* eventInfo = static_cast<EventInformation*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  // This is pretty ugly and but necessary because the Geant4TruthIncident
  // requires an ISFParticle at this point.
  // TODO: cleanup Geant4TruthIncident to not require an ISFParticle instance any longer
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

