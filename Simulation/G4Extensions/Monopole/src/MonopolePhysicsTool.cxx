/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "MonopolePhysicsTool.h"
// package headers
#include "CustomMonopole.h"
#include "CustomMonopoleFactory.h"
#include "G4mplAtlasTransportation.h"
#include "G4mplAtlasIonisation.hh"
// Geant4 headers
#include "G4Version.hh"
#include "G4Transportation.hh"
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"
#include "G4hIonisation.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : MonopolePhysicsTool
//
// 14-05-2015 Edoardo Farina
//-----------------------------------------------------------------------------

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MonopolePhysicsTool::MonopolePhysicsTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : base_class ( type, nam , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================

MonopolePhysicsTool::~MonopolePhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MonopolePhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("MonopolePhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

MonopolePhysicsTool* MonopolePhysicsTool::GetPhysicsOption()
{
  return this;
}


void MonopolePhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG(" ConstructParticle for the Monopole being run");
  CustomMonopoleFactory::loadCustomMonopoles();

}
void MonopolePhysicsTool::ConstructProcess()
{

  PARTICLEITERATOR->reset();

  while((*PARTICLEITERATOR)())
    {

      CustomMonopole* particle = dynamic_cast<CustomMonopole*>(PARTICLEITERATOR->value());
      if(CustomMonopoleFactory::isCustomMonopole(particle))
        {
          ATH_MSG_DEBUG( particle->GetParticleName() << " is Custom"  );

          G4ProcessManager* pmanager = particle->GetProcessManager();

          G4double magnCharge = particle->MagneticCharge();


          pmanager->RemoveProcess(0);
          pmanager->AddProcess(new G4mplAtlasTransportation(particle), -1, 0, 0);

          if (magnCharge != 0.0){
            pmanager->AddProcess(new G4mplAtlasIonisation(magnCharge, G4String("mplAtlasIonisation")), -1, 1, 1);

          }


          if(particle->GetPDGCharge() != 0.0) {
            pmanager->AddProcess(new G4hIonisation(),  -1, 2, 2);
          }
          pmanager->DumpInfo();


        }
    }

  ATH_MSG_DEBUG("ConstructProcess for Monopole finished");
}
