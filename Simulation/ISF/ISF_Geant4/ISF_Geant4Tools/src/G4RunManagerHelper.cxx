/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// G4RunManagerHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////

// class header
#include "G4RunManagerHelper.h"

// package includes
#include "G4DetectorConstruction.h"

// Geant4 includes
#include "G4VUserPhysicsList.hh"
#include "QGSP_BERT.hh"

iGeant4::G4RunManagerHelper::G4RunManagerHelper(const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : base_class(type,name,parent),
    m_g4RunManager(0),
    m_fastG4RunManager(0)
{
}

iGeant4::G4RunManagerHelper::~G4RunManagerHelper()
{
}

StatusCode iGeant4::G4RunManagerHelper::initialize()
{
  ATH_MSG_INFO("Initializing G4RunManagerHelper");

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::G4RunManagerHelper::finalize()
{
  return StatusCode::SUCCESS;
}


iGeant4::G4AtlasRunManager* iGeant4::G4RunManagerHelper::g4RunManager() const

{
  if (m_g4RunManager) return m_g4RunManager;

  m_g4RunManager = G4AtlasRunManager::GetG4AtlasRunManager();

  ATH_MSG_INFO("Initializing G4RunManager");

  return m_g4RunManager;

}


G4RunManager* iGeant4::G4RunManagerHelper::fastG4RunManager() const

{
  if (m_g4RunManager) return m_g4RunManager;

  if (m_fastG4RunManager) return m_fastG4RunManager;

  m_fastG4RunManager = new G4RunManager;

  ATH_MSG_INFO("Initializing fast version of G4RunManager");

  // initialize here
  G4VUserPhysicsList *thePL = new QGSP_BERT;

  m_fastG4RunManager->SetUserInitialization(thePL);
  m_fastG4RunManager->SetUserInitialization(new G4DetectorConstruction());

  // initialize Geant4
  m_fastG4RunManager->Initialize();

  return m_fastG4RunManager;

}
