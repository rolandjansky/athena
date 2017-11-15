/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "FastSimulationMasterTool.h"

// For the process setup
#include "G4ParticleTable.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4ProcessManager.hh"

FastSimulationMasterTool::FastSimulationMasterTool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type,name,parent)
  , m_FastSimList(this)
{
  declareProperty( "FastSimulations" , m_FastSimList , "Tool handle array of fast simulation tools" );
}

StatusCode FastSimulationMasterTool::initializeFastSims(){
  ATH_MSG_VERBOSE( name() << "::initializeFastSims()" );
  // Loop through list of fast simulations and retrieve them
  //  This forces a call to initialize() for each of them
  ATH_MSG_INFO( "Initializing list of " << m_FastSimList.size() << " fast simulation tools in " << name() );
  CHECK( m_FastSimList.retrieve() );

  // Initialize the SDs
  for (auto& ifs : m_FastSimList){
    CHECK(ifs->initializeFastSim());
  }

  // If we added a fast simulation, then we will need to add the process to the particles
  if ( m_FastSimList.size() > 0 ){
    G4FastSimulationManagerProcess* fastSimManagerProcess = new G4FastSimulationManagerProcess();
    G4ParticleTable* theParticleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* theParticleIterator = theParticleTable->GetIterator();

    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ProcessManager* pmanager = theParticleIterator->value()->GetProcessManager();
      pmanager->AddProcess(fastSimManagerProcess, -1, 1, 1);
    } // Done with loop over particles
  } // Done with process assignment

  return StatusCode::SUCCESS;
}

StatusCode FastSimulationMasterTool::EndOfAthenaEvent(){
  // Call gather for all sensitive detectors
  for (auto& ifs : m_FastSimList){
    CHECK(ifs->EndOfAthenaEvent());
  }
  return StatusCode::SUCCESS;
}
