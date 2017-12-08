/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// header files from Geant4
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#if G4VERSION_NUMBER >= 1010
#include "G4ParallelWorldPhysics.hh"
#endif

G4AtlasSvc::G4AtlasSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),m_detGeoSvc("DetectorGeometrySvc","G4AtlasSvc")
{
  ATH_MSG_INFO( "G4AtlasSvc being created!" );
  declareProperty( "ActivateParallelWorlds",m_activateParallelGeometries,"Toggle on/off the G4 parallel geometry system");
}

G4AtlasSvc::~G4AtlasSvc()
{
}

StatusCode G4AtlasSvc::initialize(){
  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools

  ATH_MSG_INFO( "this is G4AtlasSvc::initialize() " );

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
