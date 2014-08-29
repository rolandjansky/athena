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
#include "FadsKinematics/G4EventAnalyzer.h"
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

GeneratorCenter* GeneratorCenter::thePointer=GeneratorCenter::GetGeneratorCenter();

GeneratorCenter::GeneratorCenter()
  : currentGenerator(0), hepmcEvent(0), eventIsDeleted(true), hepmcCollection("GEN_EVENT") , m_IncludeParentsInG4Event(false), m_randomnumberservice("AtRndmGenSvc")
{
  HepMCconverter= new HepMCEventConverter;
}

void GeneratorCenter::SetHepMCEventConverter(const HepMCEventConverter* hc)
{
	// check if, by chance, they're one and the same
	if (hc==HepMCconverter) return ;
	delete HepMCconverter;
	HepMCconverter=hc;
}

GeneratorCenter::~GeneratorCenter()
{
  if(hepmcEvent) delete hepmcEvent;  
  if(HepMCconverter) delete HepMCconverter;
}

GeneratorCenter* GeneratorCenter::GetGeneratorCenter()
{
  if (!thePointer) thePointer=new  GeneratorCenter;
  return thePointer;
}

void GeneratorCenter::RegisterGenerator(FadsGenerator* g)
{
  generatorCatalog[g->GetName()]=g;
}

void GeneratorCenter::SelectGenerator(std::string n)
{
  if (generatorCatalog.find(n)==generatorCatalog.end()) {
    std::cout<<" GeneratorCenter Warning!!! Generator "<<n
	     <<" not found!!!!"<<std::endl;
    return;
  }
  //if (currentGenerator && n==currentGenerator->GetName()) return;
  if (currentGenerator) currentGenerator->Terminate();
  currentGenerator=generatorCatalog[n];
  currentGenerator->Create();
  //currentGenerator->Initialize();
}

void GeneratorCenter::InitializeGenerator()
{
  currentGenerator->Initialize();
}

void GeneratorCenter::PrintGeneratorList()
{
  genMap::const_iterator it;
  std::cout<<" List of Kinematics Generators available: "<<std::endl;
  for (it=generatorCatalog.begin();it!=generatorCatalog.end();++it)
    std::cout<<"\t---> "<<(*it).first<<"\t\t<---"<<std::endl;
  std::cout<<" <--------------------------------------->"<<std::endl;
}

void GeneratorCenter::AddVertexManipulator(VertexManipulator* v)
{
  vertexMod[v->GetName()]=v;
}

void GeneratorCenter::AddParticleManipulator(ParticleManipulator* v)
{
  particleMod[v->GetName()]=v;
}

void GeneratorCenter::AddEventAnalyzer(G4EventAnalyzer *v)
{
  evtAnalyses[v->GetName()]=v;
}

///////////////////////////////////////////////////////
void GeneratorCenter::GenerateAnEvent(G4Event* anEvent)
///////////////////////////////////////////////////////
{  
  if (currentGenerator==0){
    std::cout<<"GeneratorCenter: Something wrong - no generator assigned!!!!"<<std::endl;
    G4RunManager::GetRunManager()-> AbortRun();
    return;
  }

  // clear previous event
  if (eventIsDeleted) delete hepmcEvent; // delete previous HepMC event
//  g4primary2genMap.clear();

  // generate next event
  hepmcEvent = currentGenerator-> GenerateAnEvent();
  if(! hepmcEvent) {
    G4cout << "GeneratorCenter: no generated particles. run terminated..." 
           << G4endl;
    G4RunManager::GetRunManager()-> AbortRun();
    return;
  }
  HepMC2G4(hepmcEvent, anEvent);
}


////////////////////////////////////////////////////////////////////////////
void GeneratorCenter::HepMC2G4(const HepMC::GenEvent* evt, G4Event* anEvent)
////////////////////////////////////////////////////////////////////////////
{
	HepMCconverter->HepMC2G4(evt,anEvent,m_IncludeParentsInG4Event);
}

}	// end namespace
