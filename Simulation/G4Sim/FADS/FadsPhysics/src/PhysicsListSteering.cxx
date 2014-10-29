/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsPhysics/FadsPhysicsList.h"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/UserPhysicsDefinition.h"

#include "G4VUserPhysicsList.hh"
#include "G4UImanager.hh"
#include "G4EmProcessOptions.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include <sstream>

namespace FADS {

  PhysicsListSteering::PhysicsListSteering():G4VUserPhysicsList(),theList(0),PList(0),m_neutronTimeCut(0),m_neutronEnergyCut(0),m_applyCuts(false)
  {
    // std::cout<< " Physics List Steering being created now "<<std::endl;
  }

  PhysicsListSteering::~PhysicsListSteering()
  {
    // std::cout<< " deleting PhysicsListSteering"<<std::endl;
    // std::cout<< " \ttrying deleting the actual list first!"<<std::endl;
    if (PList) delete PList;
    // std::cout<< " \tdone! terminating"<<std::endl;
  }

  void PhysicsListSteering::ConstructParticle()
  {
    // std::cout<< "this is PhysicsListSteering::ConstructParticle:"<<
    //                      " setting G4ParticleTable readiness "<<std::endl;
    // G4ParticleTable::GetParticleTable()->SetReadiness();
  }

  void PhysicsListSteering::SetCuts()
  {
    if (!PList)
      {
        std::cout<< "this is PhysicsListSteering::SetCuts: PList not set!!!!!"<<std::endl;
        return;
      }
    PList->SetCuts();
  }

  void PhysicsListSteering::ConstructProcess()
  {
    // std::cout<< "this is PhysicsListSteering::ConstructProcess"<<std::endl;
    theList=PhysicsListCatalog::GetInstance()->currentPL;
    if (!theList)
      std::cout<< "this is PhysicsListSteering: the List is not set!"<<std::endl;
    else
      {
        theList->Construct();
        PList=theList->GetPhysicsList();

        // this block replaces the standard PhysicsList's ConstructParticle

        PList->ConstructParticle();

        for (unsigned int i=0;i<userPhysics.size();i++)
          userPhysics[i]->ConstructParticle();

        InitializeProcessManager();

        // this block is for PhysicsList's ConstructProcess

        PList->ConstructProcess();

        for (unsigned int i=0;i<userPhysics.size();i++)
          userPhysics[i]->ConstructProcess();
      }

    if (m_neutronTimeCut)
      {
        std::cout<<"+---------------------------------------------------+"<<std::endl;
        std::cout<<"|   setting neutron time cut to "<<m_neutronTimeCut<<std::endl;
        std::cout<<"+---------------------------------------------------+"<<std::endl;
        std::ostringstream oss;
        oss<<"/physics_engine/neutron/timeLimit "<<m_neutronTimeCut<<" ns";
        G4UImanager* UImanager = G4UImanager::GetUIpointer();
        UImanager->ApplyCommand(oss.str());
      }

    if (m_neutronEnergyCut > 0)
      {
        std::cout<<"+---------------------------------------------------+"<<std::endl;
        std::cout<<"|   setting neutron energy cut to "<<m_neutronEnergyCut<<std::endl;
        std::cout<<"+---------------------------------------------------+"<<std::endl;
        std::ostringstream oss;
        oss<<"/physics_engine/neutron/energyLimit "<<m_neutronEnergyCut<<" MeV";
        G4UImanager* UImanager = G4UImanager::GetUIpointer();
        UImanager->ApplyCommand(oss.str());
      }

    G4EmProcessOptions emo;
    emo.SetMaxEnergy(7*CLHEP::TeV);
    emo.SetDEDXBinning(77);
    emo.SetLambdaBinning(77);
    emo.SetMinEnergy(100*CLHEP::eV);
    if (m_applyCuts) emo.SetApplyCuts(true);
  }


  void PhysicsListSteering::SetNeutronTimeCut(double v)
  {
    m_neutronTimeCut=v;
  }

  double PhysicsListSteering::GetNeutronTimeCut()
  {
    return m_neutronTimeCut;
  }

  void PhysicsListSteering::SetNeutronEnergyCut(double v)
  {
    m_neutronEnergyCut=v;
  }

  double PhysicsListSteering::GetNeutronEnergyCut()
  {
    return m_neutronEnergyCut;
  }

}
