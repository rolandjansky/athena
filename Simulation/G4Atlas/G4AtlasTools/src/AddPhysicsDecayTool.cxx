/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "G4AtlasTools/AddPhysicsDecayTool.h"

#include <sstream>

// Geant4 physics lists


#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"
//-----------------------------------------------------------------------------
// Implementation file for class : AddPhysicsDecayTool
//
// 15-05-2015 : Edoardo Farina
//-----------------------------------------------------------------------------
#include "G4Version.hh"
#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AddPhysicsDecayTool::AddPhysicsDecayTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_VERBOSE("AddPhysicsDecayTool "<<type<<" "<<nam);
  declareInterface<IPhysicsOptionTool >( this ) ;
  declareProperty("ParticleName",m_ParticleName="NoFile","Particle name");
  declareProperty("BR",m_BR=0,"Branching Ratio");
  declareProperty("Daughters",m_Daughters="NoFile","Daughters");
}

//=============================================================================
// Destructor
//=============================================================================

AddPhysicsDecayTool::~AddPhysicsDecayTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode AddPhysicsDecayTool::initialize( )
{
  ATH_MSG_DEBUG("AddPhysicsDecayTool initialize( )");
  ATH_MSG_DEBUG("Add Decay to the particle: "<<m_ParticleName);

  std::istringstream ss(m_Daughters);
  std::string token;

  while(std::getline(ss, token, ','))
    {
      m_Daughters_vec.push_back(token);
    }

  return StatusCode::SUCCESS;
}

void AddPhysicsDecayTool::ConstructProcess()
{
  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() )
    {
      G4ParticleDefinition *particle = PARTICLEITERATOR->value();
      if (m_ParticleName== particle->GetParticleName())
        {

          G4DecayTable *table = particle->GetDecayTable();
          if (!table)
            {
              table = new G4DecayTable();
            }

          G4VDecayChannel *mode_vec(nullptr);

          if (m_Daughters_vec.size()==2)
            {
              mode_vec = new G4PhaseSpaceDecayChannel(m_ParticleName,m_BR,2,m_Daughters_vec[0],m_Daughters_vec[1]);
            }
          else if (m_Daughters_vec.size()==3)
            {
              mode_vec = new G4PhaseSpaceDecayChannel(m_ParticleName,m_BR,3,m_Daughters_vec[0],m_Daughters_vec[1],m_Daughters_vec[2]);
            }
          else if (m_Daughters_vec.size()==4)
            {
              mode_vec = new G4PhaseSpaceDecayChannel(m_ParticleName,m_BR,4,m_Daughters_vec[0],m_Daughters_vec[1],m_Daughters_vec[2],m_Daughters_vec[3]);
            }
          else
            {
              //This could be checked during initialize also
              ATH_MSG_FATAL("No decay in more than 4 particles allowed");
              throw "Decay with more than four particles";
            }
          /*
            for (int i=0; i<table->entries(); i++)
            {
            if(*(table->GetDecayChannel(i))==*mode_vec)
            {
            ATH_MSG_FATAL("Trying to Add a decay already present" );
            throw "FailedAddDecay";
            }
            }
          */

          table->Insert(mode_vec);

          ATH_MSG_DEBUG( "Adding decay to "<<m_ParticleName );
          particle->SetDecayTable(table);
        }
    }
}


AddPhysicsDecayTool* AddPhysicsDecayTool::GetPhysicsOption()
{

  return this;
}



void AddPhysicsDecayTool::ConstructParticle()
{
  //This should remain empty
}
