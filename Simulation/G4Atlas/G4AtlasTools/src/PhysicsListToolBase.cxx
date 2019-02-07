/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/PhysicsListToolBase.h"

#include "G4VUserPhysicsList.hh"
#include "G4StateManager.hh"
#include "G4RunManager.hh"
#include "G4EmParameters.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include "G4PhysicsLists/AtlasPhysListFactory.h"

#include <limits>

PhysicsListToolBase::PhysicsListToolBase(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type,name,parent)
  , m_phys_option(this)
  , m_phys_decay(this)
  , m_physicsList(nullptr)
{
  declareProperty("PhysicsList"     , m_physicsListName = "FTFP_BERT", "Name for physics list");
  declareProperty("NeutronTimeCut"  , m_neutronTimeCut = 0,            "Time cut for neutron killer");
  declareProperty("NeutronEnergyCut", m_neutronEnergyCut = 0,          "Energy cut for neutron killer");
  declareProperty("GeneralCut"      , m_generalCut = 0,                "General cut");
  declareProperty("EMMaxEnergy"     , m_emMaxEnergy = -1.,             "Maximum energy for EM tables");
  declareProperty("EMMinEnergy"     , m_emMinEnergy = -1.,             "Minimum energy for EM tables");
  /* --- ATLASSIM-3967 ---
     Old methods SetDEDXBinning and SetLambdaBinning are replaced in G4 10.4 by
     SetNumberOfBinsPerDecade which now controls both settings with the same
     value. Instead of changing the overall number of bins, user should
     specify the number of bins per energy decade instead.
  */
  declareProperty("EMNumberOfBinsPerDecade"   , m_emNumberOfBinsPerDecade = -1,  "Number of bins per Energy decade. Used for both DeDx and for the Lambda binning.");
  declareProperty("ApplyEMCuts"     , m_applyEMCuts = true,            "Apply cuts EM flag in Geant4");
  declareProperty("PhysOption"      , m_phys_option ,                  "Tool handle array of physics options" );
  declareProperty("PhysicsDecay"    , m_phys_decay ,                   "Tool handle array of physics decays" );
}


StatusCode PhysicsListToolBase::initialize( )
{
  if (m_phys_option.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_option.size() << " physics options"  );
      CHECK( m_phys_option.retrieve() );
    }

  if (m_phys_decay.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_decay.size() << " Decays "  );
      CHECK( m_phys_decay.retrieve() );
    }

  return StatusCode::SUCCESS;
}


void PhysicsListToolBase::CreatePhysicsList()
{
  ATH_MSG_DEBUG("PhysicsListToolBase::CreatePhysicsList()");
  if (m_physicsListName != ""){
    G4PhysListFactory factory;
    AtlasPhysListFactory Atlasfactory;
    if (factory.IsReferencePhysList(m_physicsListName))
    {
      m_physicsList = factory.GetReferencePhysList(m_physicsListName);
    }
    else if (Atlasfactory.IsReferencePhysList(m_physicsListName))
    {
      m_physicsList = Atlasfactory.GetReferencePhysList(m_physicsListName);
    }
  }
    
  if (!m_physicsList)
    {
      ATH_MSG_ERROR("Unable to initialize physics List: " << m_physicsList);
      throw "PhysicsListInitializationError";
    }
  // Call these as functions.  As this could be used as a base class, having
  //   these as separate functions lets someone who is inheriting from this use
  //   them...

  //Register physics options to the G4VModularPhysicsList
  for (auto& physOptTool: m_phys_option)
    {
      m_physicsList->RegisterPhysics(physOptTool->GetPhysicsOption());
    }
  //Register decays to the G4VModularPhysicsList
  for (auto& physDecayTool: m_phys_decay)
    {
      m_physicsList->RegisterPhysics(physDecayTool->GetPhysicsOption());
    }

  //ConstructProcess();
  ATH_MSG_DEBUG("end of PhysicsListToolBase::CreatePhysicsList()");
}

G4VUserPhysicsList* PhysicsListToolBase::GetPhysicsList()
{
  if (!m_physicsList) {
    this->CreatePhysicsList();
  }
  return m_physicsList;
}

void PhysicsListToolBase::SetPhysicsList()
{
  if(!m_physicsList) {
    this->CreatePhysicsList();
  }
  G4RunManager::GetRunManager()->SetUserInitialization(m_physicsList);
}

void PhysicsListToolBase::SetPhysicsOptions()
{
  if (!m_physicsList)
    {
      ATH_MSG_WARNING("Physics list not initialized before calling ConstructProcess()");
      return;
    }

  if(m_generalCut > 0. && std::fabs(m_generalCut)>std::numeric_limits<double>::epsilon())
    {
      m_physicsList->SetDefaultCutValue(m_generalCut);
    }

  std::vector<std::string> g4commands;
  if (m_neutronTimeCut > 0. && std::fabs(m_neutronTimeCut)>std::numeric_limits<double>::epsilon())
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/timeLimit "<<m_neutronTimeCut<<" ns";
      g4commands.push_back(oss.str());
    }

  if (m_neutronEnergyCut > 0. && std::fabs(m_neutronEnergyCut)>std::numeric_limits<double>::epsilon())
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/energyLimit "<<m_neutronEnergyCut<<" MeV";
      g4commands.push_back(oss.str());
    }

  if(!g4commands.empty()) {
    // Send UI commands
    ATH_MSG_DEBUG("G4 Command: Trying in SetPhysicsOptions()");
    G4UImanager* ui = G4UImanager::GetUIpointer();
    for (auto g4command : g4commands) {
      int returnCode = ui->ApplyCommand( g4command );
      CommandLog(returnCode, g4command);
    }
  }

  G4EmParameters* emp = G4EmParameters::Instance();
  if (m_emMaxEnergy>=0) emp->SetMaxEnergy(m_emMaxEnergy);
  if (m_emNumberOfBinsPerDecade>=0) emp->SetNumberOfBinsPerDecade(m_emNumberOfBinsPerDecade);
  if (m_emMinEnergy>=0) emp->SetMinEnergy(m_emMinEnergy);
  if (m_applyEMCuts)
    {
      emp->SetApplyCuts(true);
    }
  return;
}

void PhysicsListToolBase::CommandLog(int returnCode, const std::string& commandString) const
{
  switch(returnCode) {
  case 0: { ATH_MSG_DEBUG("G4 Command: " << commandString << " - Command Succeeded"); } break;
  case 100: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Command Not Found!"); } break;
  case 200: {
    auto* stateManager = G4StateManager::GetStateManager();
    ATH_MSG_DEBUG("G4 Command: " << commandString << " - Illegal Application State (" <<
                    stateManager->GetStateString(stateManager->GetCurrentState()) << ")!");
  } break;
  case 300: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Range!"); } break;
  case 400: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Unreadable!"); } break;
  case 500: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Candidates!"); } break;
  case 600: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Alias Not Found!"); } break;
  default: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Unknown Status!"); } break;
  }

}
