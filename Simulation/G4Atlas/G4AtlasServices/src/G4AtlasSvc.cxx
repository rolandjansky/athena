/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// header files from Geant4
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4RunManager.hh"

G4AtlasSvc::G4AtlasSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator)
  , m_detGeoSvc("DetectorGeometrySvc",name)
  , m_physicsListTool("PhysicsListToolBase")
  , m_isMT(false)
  , m_activateParallelGeometries(false)
{
  ATH_MSG_INFO( "G4AtlasSvc being created!" );
  declareProperty( "ActivateParallelWorlds",m_activateParallelGeometries,"Toggle on/off the G4 parallel geometry system");
  declareProperty("DetectorGeometrySvc", m_detGeoSvc );
  declareProperty("PhysicsListTool", m_physicsListTool);
  declareProperty("isMT", m_isMT);
}

G4AtlasSvc::~G4AtlasSvc()
{
}

StatusCode G4AtlasSvc::initialize(){
  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools

  ATH_MSG_INFO( "this is G4AtlasSvc::initialize() " );
  auto* rm = G4RunManager::GetRunManager();
  if(!rm) {
    ATH_MSG_ERROR("Run manager retrieval has failed");
    return StatusCode::FAILURE;
  }
  rm->Initialize();     // Initialization differs slightly in multi-threading.
  // TODO: add more details about why this is here.
  if(!m_isMT && rm->ConfirmBeamOnCondition()) {
    rm->RunInitialization();
  }

  ATH_MSG_INFO( "retireving the Detector Geometry Service" );
  CHECK(m_detGeoSvc.retrieve());

  ATH_MSG_INFO( "retrieving the Physics List Tool" );
  CHECK(m_physicsListTool.retrieve());

  if (m_activateParallelGeometries)
    {
      G4VModularPhysicsList* thePhysicsList=dynamic_cast<G4VModularPhysicsList*>(m_physicsListTool->GetPhysicsList());
      if (!thePhysicsList)
        {
          ATH_MSG_FATAL("Failed dynamic_cast!! this is not a G4VModularPhysicsList!");
          return StatusCode::FAILURE;
        }
#if G4VERSION_NUMBER >= 1010
      std::vector<std::string>& parallelWorldNames=m_detGeoSvc->GetParallelWorldNames();
      for (auto& it: parallelWorldNames)
        {
          thePhysicsList->RegisterPhysics(new G4ParallelWorldPhysics(it,true));
        }
#endif
    }

  return StatusCode::SUCCESS;
}

StatusCode G4AtlasSvc::finalize()
{
  ATH_MSG_INFO( "G4AtlasSvc being ifinalized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode G4AtlasSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IG4AtlasSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IG4AtlasSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

