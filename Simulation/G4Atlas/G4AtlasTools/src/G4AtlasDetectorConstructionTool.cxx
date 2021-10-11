/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "G4AtlasTools/G4AtlasDetectorConstructionTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : G4AtlasDetectorConstructionTool
//
// 2014-10-03: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=================================
// Standard constructor, initializes variables
//=================================
G4AtlasDetectorConstructionTool::G4AtlasDetectorConstructionTool( const std::string& type,
                                                                  const std::string& nam,const IInterface* parent )
  : base_class( type, nam , parent )
{
}

//=================================
// Athena method overrides
//=================================
StatusCode G4AtlasDetectorConstructionTool::initialize( )
{
  ATH_MSG_DEBUG( "Initializing Geometry configuration tools "  );
  for (auto it: m_configurationTools)
  {
    ATH_CHECK( it.retrieve() );
    ATH_CHECK( it->preGeometryConfigure() );
  }

  ATH_MSG_DEBUG( "Initializing World detectors in " << name() );
  ATH_CHECK( m_detTool.retrieve() );

  ATH_MSG_DEBUG( "Initializing sensitive detectors in " << name() );
  ATH_CHECK( m_senDetTool.retrieve() );

  ATH_MSG_DEBUG( "Setting up G4 physics regions" );
  for (auto& it: m_regionCreators)
  {
    ATH_CHECK( it.retrieve() );
  }

  if (m_activateParallelWorlds)
  {
    ATH_MSG_DEBUG( "Setting up G4 parallel worlds" );
    for (auto& it: m_parallelWorlds)
    {
      ATH_CHECK( it.retrieve() );
    }
  }

  ATH_MSG_DEBUG( "Setting up field managers" );
  ATH_CHECK( m_fieldManagers.retrieve() );

  return StatusCode::SUCCESS;
}

std::vector<std::string>& G4AtlasDetectorConstructionTool::GetParallelWorldNames()
{
  return m_parallelWorldNames;
}

//=================================
// G4VUserDetectorConstruction method overrides
//=================================
G4VPhysicalVolume* G4AtlasDetectorConstructionTool::Construct()
{
  ATH_MSG_DEBUG( "Detectors " << m_detTool.name() <<" being set as World" );
  m_detTool->SetAsWorld();
  m_detTool->Build();

  ATH_MSG_DEBUG( "Setting up G4 physics regions" );
  for (auto& it: m_regionCreators)
  {
    it->Construct();
  }

  if (m_activateParallelWorlds)
  {
    ATH_MSG_DEBUG( "Setting up G4 parallel worlds" );
    for (auto& it: m_parallelWorlds)
    {
      m_parallelWorldNames.push_back(it.name());
      this->RegisterParallelWorld(it->GetParallelWorld());
    }
  }

  ATH_MSG_DEBUG( "Running geometry post-configuration tools" );
  for (auto it: m_configurationTools)
  {
    StatusCode sc = it->postGeometryConfigure();
    if (!sc.isSuccess())
    {
      ATH_MSG_FATAL( "Unable to run post-geometry configuration for " << it->name() );
    }
  }

  return m_detTool->GetWorldVolume();
}

void G4AtlasDetectorConstructionTool::ConstructSDandField()
{
  ATH_MSG_DEBUG( "Setting up sensitive detectors" );
  if (m_senDetTool->initializeSDs().isFailure())
  {
    ATH_MSG_FATAL("Failed to initialize SDs for worker thread");
  }

  ATH_MSG_DEBUG( "Setting up field managers" );
  for (auto& fm : m_fieldManagers)
  {
    StatusCode sc = fm->initializeField();
    if (!sc.isSuccess())
    {
      ATH_MSG_FATAL( "Unable to initialise field with " << fm->name() );
      return;
    }
  }

  return;
}
