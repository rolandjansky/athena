/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "RHadronsPhysicsTool.h"
// package headers
#include "CustomParticleFactory.h"
#include "FullModelHadronicProcess.hh"
#include "RHadronPythiaDecayer.h"
// Geant4 headers
#include "globals.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4Transportation.hh"
#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4Transportation.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4BaryonConstructor.hh"
#include "G4ProcessManager.hh"

// STL headers
#include <string>


//-----------------------------------------------------------------------------
// Implementation file for class : RHadronsPhysicsTool
//
// 2015-05-14 Edoardo Farina
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RHadronsPhysicsTool::RHadronsPhysicsTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : base_class ( type, nam , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================

RHadronsPhysicsTool::~RHadronsPhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode RHadronsPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("RHadronsPhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

RHadronsPhysicsTool* RHadronsPhysicsTool::GetPhysicsOption()
{
  return this;
}


void RHadronsPhysicsTool::ConstructParticle()
{
  ATH_MSG_DEBUG("RHadronProcessDefinition::ConstructParticle() called");
  CustomParticleFactory::loadCustomParticles();
}
void RHadronsPhysicsTool::ConstructProcess()
{

  ATH_MSG_DEBUG("RHadronProcessDefinition::ConstructProcess() called");
  G4Decay* theDecayProcess = new G4Decay();
  theDecayProcess->SetExtDecayer( new RHadronPythiaDecayer("RHadronPythiaDecayer") );
  G4ParticleTable::G4PTblDicIterator* aParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
  aParticleIterator->reset();

  //First deal with the standard particles that G4 doesn't know about...
  //G4Etac::Definition();
  G4BaryonConstructor::ConstructParticle();

  // 521   B+  5279.17 (MeV/c) meson B 0
  // -521   B-  5279.17 (MeV/c) meson B 0
  // 511   B0  5279.5 (MeV/c) meson B 0
  // -511   anti_B0  5279.5 (MeV/c) meson B 0
  // 531   Bs0  5366.3 (MeV/c) meson Bs 0
  // -531   anti_Bs0  5366.3 (MeV/c) meson Bs 0
  // 541   Bc+  6277 (MeV/c) meson Bc 0
  // -541   Bc-  6277 (MeV/c) meson Bc 0
  // 553   Upsilon  9460.3 (MeV/c) meson Upsilon 0

  // 411   D+  1869.57 (MeV/c) meson D 0
  // -411   D-  1869.57 (MeV/c) meson D 0
  // 421   D0  1864.8 (MeV/c) meson D 0
  // -421   anti_D0  1864.8 (MeV/c) meson D 0
  // 431   Ds+  1968.45 (MeV/c) meson Ds 0
  // -431   Ds-  1968.45 (MeV/c) meson Ds 0
  // 441   etac  2980.3 (MeV/c) meson etac 0
  // 443   J/psi  3096.92 (MeV/c) meson J/psi 0

  // 4122   lambda_c+  2286.46 (MeV/c) baryon lambda_c 0
  // 4222   sigma_c++  2454.02 (MeV/c) baryon sigma_c 0
  // 4212   sigma_c+  2452.9 (MeV/c) baryon sigma_c 0
  // 4112   sigma_c0  2453.76 (MeV/c) baryon sigma_c 0
  // 4232   xi_c+  2467.8 (MeV/c) baryon xi_c 0
  // 4132   xi_c0  2470.88 (MeV/c) baryon xi_c 0
  // 4332   omega_c0  2695.2 (MeV/c) baryon omega_c 0

  // 5122   lambda_b  5620.2 (MeV/c) baryon lambda_b 0
  // 5222   sigma_b+  5807.8 (MeV/c) baryon sigma_b 0
  // 5212   sigma_b0  5807.8 (MeV/c) baryon sigma_b 0
  // 5112   sigma_b-  5815.2 (MeV/c) baryon sigma_b 0
  // 5232   xi_b0  5790.5 (MeV/c) baryon xi_b 0
  // 5132   xi_b-  5790.5 (MeV/c) baryon xi_b 0
  // 5332   omega_b-  6071 (MeV/c) baryon omega_b 0

  std::vector<int> standardpdgidtodecay;
  standardpdgidtodecay.push_back(4122);
  standardpdgidtodecay.push_back(4222);
  standardpdgidtodecay.push_back(4212);
  standardpdgidtodecay.push_back(4112);
  standardpdgidtodecay.push_back(4232);
  standardpdgidtodecay.push_back(4132);
  standardpdgidtodecay.push_back(4332);
  standardpdgidtodecay.push_back(5122); //G4Lambdab::Definition();
  standardpdgidtodecay.push_back(5222);
  standardpdgidtodecay.push_back(5212);
  standardpdgidtodecay.push_back(5112);
  standardpdgidtodecay.push_back(5232);
  standardpdgidtodecay.push_back(5132);
  standardpdgidtodecay.push_back(5332);

  standardpdgidtodecay.push_back(-4122);
  standardpdgidtodecay.push_back(-4222);
  standardpdgidtodecay.push_back(-4212);
  standardpdgidtodecay.push_back(-4112);
  standardpdgidtodecay.push_back(-4232);
  standardpdgidtodecay.push_back(-4132);
  standardpdgidtodecay.push_back(-4332);
  standardpdgidtodecay.push_back(-5122); //G4AntiLambdab::Definition();
  standardpdgidtodecay.push_back(-5222);
  standardpdgidtodecay.push_back(-5212);
  standardpdgidtodecay.push_back(-5112);
  standardpdgidtodecay.push_back(-5232);
  standardpdgidtodecay.push_back(-5132);
  standardpdgidtodecay.push_back(-5332);

  standardpdgidtodecay.push_back(411);
  standardpdgidtodecay.push_back(-411);
  standardpdgidtodecay.push_back(421);
  standardpdgidtodecay.push_back(-421);
  standardpdgidtodecay.push_back(431);
  standardpdgidtodecay.push_back(-431);
  standardpdgidtodecay.push_back(441);
  standardpdgidtodecay.push_back(443);

  standardpdgidtodecay.push_back(521);
  standardpdgidtodecay.push_back(-521);
  standardpdgidtodecay.push_back(511);
  standardpdgidtodecay.push_back(-511);
  standardpdgidtodecay.push_back(531);
  standardpdgidtodecay.push_back(-531);
  standardpdgidtodecay.push_back(541);
  standardpdgidtodecay.push_back(-541);
  standardpdgidtodecay.push_back(553);

//  Unknown to Geant4, so removed for the time being
//  standardpdgidtodecay.push_back(423);
//  standardpdgidtodecay.push_back(-423);
//  standardpdgidtodecay.push_back(433);
//  standardpdgidtodecay.push_back(-433);
//  standardpdgidtodecay.push_back(513);
//  standardpdgidtodecay.push_back(-513);
//  standardpdgidtodecay.push_back(523);
//  standardpdgidtodecay.push_back(-523);
//  standardpdgidtodecay.push_back(551);
//  standardpdgidtodecay.push_back(-551);

  for (unsigned int i=0; i<standardpdgidtodecay.size(); ++i) {
    int pid=standardpdgidtodecay[i];
    ATH_MSG_VERBOSE ( "Adding decay for "<<pid );
    G4ParticleDefinition *particle = G4ParticleTable::GetParticleTable()->FindParticle( pid );
    if (particle) {
      ATH_MSG_VERBOSE ( particle->GetParticleName()<<" is standard for Pythia, lifetime is "<<particle->GetPDGLifeTime() );
      G4ProcessManager *pmanager = particle->GetProcessManager();
      if (!pmanager) {
        ATH_MSG_VERBOSE ( "Copying process manager from 4122 (one we know works) to this particle" );
        particle->SetProcessManager(new G4ProcessManager(*(G4ParticleTable::GetParticleTable()->FindParticle(4122)->GetProcessManager())));
        pmanager = particle->GetProcessManager();
      }
      G4ProcessVector *pros = pmanager->GetProcessList();
      for (int pi=0; pi<pros->size(); ++pi) {
        if ((*pros)[pi]->GetProcessType()==fDecay) {
          pmanager->RemoveProcess(pi);
          break;
        }
      }
      for (int pi=0; pi<pros->size(); ++pi) {
        if ((*pros)[pi]->GetProcessType()==fDecay) {
          ATH_MSG_WARNING ( "There is another decay process for this particle already defined!" );
          pmanager ->DumpInfo();
        }
      }
      pmanager ->AddProcess(theDecayProcess);
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep); pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
      //pmanager ->DumpInfo();
    } else {
      ATH_MSG_WARNING ( "Particle with pdgid "<<pid<<" has no definition in G4?" );
    }
  } // Loop over all particles that we need to define

  // Now add RHadrons... Keep a vector of those we've already dealt with
  std::vector<int> handled;
  // Use the G4 particle iterator
  while((*aParticleIterator)()) {
    G4ParticleDefinition *particle = aParticleIterator->value();
    if(CustomParticleFactory::isCustomParticle(particle)) {
      if(find(handled.begin(),handled.end(),particle->GetPDGEncoding())==handled.end()) {
        handled.push_back(particle->GetPDGEncoding());
        ATH_MSG_VERBOSE ( particle->GetParticleName() << " is Custom" );
        G4ProcessManager *pmanager = particle->GetProcessManager();
        if(particle->GetParticleType()=="rhadron"  ||
           particle->GetParticleType()=="mesonino" ||
           particle->GetParticleType()=="sbaryon"  ) {
          pmanager->AddDiscreteProcess(new FullModelHadronicProcess());
          if (theDecayProcess->IsApplicable(*particle)) {
            ATH_MSG_VERBOSE ( "Adding decay..." );
            pmanager ->AddProcess(theDecayProcess);
            // set ordering for PostStepDoIt and AtRestDoIt
            pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
            pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
          } else {
            ATH_MSG_VERBOSE ( "No decay allowed for " << particle->GetParticleName() );
            if (!particle->GetPDGStable() && particle->GetPDGLifeTime()<0.1*CLHEP::ns) {
              ATH_MSG_VERBOSE ( "Gonna decay it anyway!!!" );
              pmanager ->AddProcess(theDecayProcess);
              // set ordering for PostStepDoIt and AtRestDoIt
              pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
              pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
            }
          }
        }
        if (particle->GetPDGCharge()/CLHEP::eplus != 0) {
          pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
          pmanager->AddProcess(new G4hIonisation,       -1, 2,2);
          ATH_MSG_VERBOSE ( "Processes for charged particle added." );
        } else {
          ATH_MSG_VERBOSE ( "It is neutral!!" );
        }
        pmanager->DumpInfo();
      } else {
        ATH_MSG_VERBOSE ( "Skipping already handled particle: "<<particle->GetParticleName() );
      } // If it has not been handled yet
    } // If this is one of our custom particles (RHadrons)
  } // End of the particle iterator
}
