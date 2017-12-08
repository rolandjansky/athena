/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GeneratorCenter.cxx,v 1.2.2.1 2008-10-01 12:26:38 dellacqu Exp $
// ====================================================================
//
//    GeneratorCenter.cc
//
// ====================================================================
#include "FadsKinematics/GeneratorCenter.h"
#include "FadsKinematics/FadsGenerator.h"
#include "FadsKinematics/VertexManipulator.h"
#include "FadsKinematics/ParticleManipulator.h"
#include "FadsKinematics/HepMCEventConverter.h"

#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4LorentzVector.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4LorentzVector.hh"
#include "G4Geantino.hh"
#include "G4RunManager.hh"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"

#include <iostream>

namespace FADS {

  // ====================================================================
  //
  // class description
  //
  // ====================================================================

  GeneratorCenter* GeneratorCenter::m_thePointer=GeneratorCenter::GetGeneratorCenter();

  GeneratorCenter::GeneratorCenter()
    : m_currentGenerator(0), m_hepmcEvent(0), m_eventIsDeleted(true), m_IncludeParentsInG4Event(false)
  {
    m_HepMCconverter= new HepMCEventConverter;
  }

  void GeneratorCenter::SetHepMCEventConverter(const HepMCEventConverter* hc)
  {
    // check if, by chance, they're one and the same
    if (hc==m_HepMCconverter) return ;
    delete m_HepMCconverter;
    m_HepMCconverter=hc;
  }

  GeneratorCenter::~GeneratorCenter()
  {
    if(m_hepmcEvent) delete m_hepmcEvent;
    if(m_HepMCconverter) delete m_HepMCconverter;
  }

  GeneratorCenter* GeneratorCenter::GetGeneratorCenter()
  {
    if (!m_thePointer) m_thePointer=new  GeneratorCenter;
    return m_thePointer;
  }

  void GeneratorCenter::RegisterGenerator(FadsGenerator* g)
  {
    m_generatorCatalog[g->GetName()]=g;
  }

  void GeneratorCenter::SelectGenerator(std::string n)
  {
    if (m_generatorCatalog.find(n)==m_generatorCatalog.end()) {
      std::cout<<" GeneratorCenter Warning!!! Generator "<<n
               <<" not found!!!!"<<std::endl;
      return;
    }
    //if (m_currentGenerator && n==m_currentGenerator->GetName()) return;
    if (m_currentGenerator) m_currentGenerator->Terminate();
    m_currentGenerator=m_generatorCatalog[n];
    m_currentGenerator->Create();
    //m_currentGenerator->Initialize();
  }

  void GeneratorCenter::InitializeGenerator()
  {
    m_currentGenerator->Initialize();
  }

  void GeneratorCenter::PrintGeneratorList()
  {
    genMap::const_iterator it;
    std::cout<<" List of Kinematics Generators available: "<<std::endl;
    for (it=m_generatorCatalog.begin();it!=m_generatorCatalog.end();++it)
      std::cout<<"\t---> "<<(*it).first<<"\t\t<---"<<std::endl;
    std::cout<<" <--------------------------------------->"<<std::endl;
  }

  void GeneratorCenter::AddVertexManipulator(VertexManipulator* v)
  {
    m_vertexMod[v->GetName()]=v;
  }

  void GeneratorCenter::AddParticleManipulator(ParticleManipulator* v)
  {
    m_particleMod[v->GetName()]=v;
  }

  ///////////////////////////////////////////////////////
  void GeneratorCenter::GenerateAnEvent(G4Event* anEvent)
  ///////////////////////////////////////////////////////
  {
    if (m_currentGenerator==0){
      std::cout<<"GeneratorCenter: Something wrong - no generator assigned!!!!"<<std::endl;
      G4RunManager::GetRunManager()->AbortRun();
      return;
    }

    // clear previous event
    if (m_eventIsDeleted) delete m_hepmcEvent; // delete previous HepMC event
    //  g4primary2genMap.clear();

    // generate next event
    m_hepmcEvent = m_currentGenerator->GenerateAnEvent();
    if(!m_hepmcEvent) {
      G4cout << "GeneratorCenter: no generated particles. run terminated..."
             << G4endl;
      G4RunManager::GetRunManager()->AbortRun();
      return;
    }
    HepMC2G4(m_hepmcEvent, anEvent);
  }


  ////////////////////////////////////////////////////////////////////////////
  void GeneratorCenter::HepMC2G4(const HepMC::GenEvent* evt, G4Event* anEvent)
  ////////////////////////////////////////////////////////////////////////////
  {
    m_HepMCconverter->HepMC2G4(evt,anEvent,m_IncludeParentsInG4Event);
  }

} // end namespace
