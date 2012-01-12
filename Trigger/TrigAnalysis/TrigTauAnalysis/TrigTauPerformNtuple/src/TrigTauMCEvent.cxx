/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauMCEvent.h"
#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"

ClassImp(TrigTauMCEvent)  

  TrigTauMCEvent::TrigTauMCEvent() : m_generatorWeight(1) {}
TrigTauMCEvent::~TrigTauMCEvent() 
{
  reset();
}

void TrigTauMCEvent::reset()
{
  for(std::vector<TrigTauTrueParticle*>::iterator itr = particleList.begin(); itr != particleList.end(); ++itr)
    delete *itr;
  particleList.erase(particleList.begin(), particleList.end() );
  etMissTrue.reset();
  for(std::vector<TLorentzVector*>::iterator itr = trueJets.begin(); itr != trueJets.end(); ++itr)
    delete *itr;
  trueJets.erase(trueJets.begin(), trueJets.end() );
  m_generatorWeight = 1;
}

void TrigTauMCEvent::print(const TrigTauTrueParticle & dau ) const
{
  dau.print();
}

void TrigTauMCEvent::print() const
{
  cout << " Event generator Weight is " << m_generatorWeight << endl;
  cout << " There are " << particleList.size() << " MC particles (partial list!)\n";
  for(std::vector<TrigTauTrueParticle*>::const_iterator itr =particleList.begin();
      itr !=particleList.end(); ++itr )
    {
      if( !(*itr)->mother())
	(*itr)->print();
    }

   cout << " There are " << trueJets.size() << " tru jets\n";
 
   for( std::vector<TLorentzVector*>::const_iterator ijet = trueJets.begin();
	ijet != trueJets.end(); ++ijet )
     {
       cout << " true jet Pt="<< (*ijet)->Pt() << " , Eta="<<(*ijet)->Eta()<< ", Phi="<< (*ijet)->Phi() << endl;
     }
}

float TrigTauMCEvent::recalculateEtMissTrue() // overwrite original value with one based on neutrinos
{
  TLorentzVector missVec;

  for(std::vector<TrigTauTrueParticle*>::const_iterator itr =particleList.begin();
      itr !=particleList.end(); ++itr )
    {
      if( (*itr)->pdgId() != 12 &&  (*itr)->pdgId() != 14 &&  (*itr)->pdgId() != 16 &&  (*itr)->pdgId() != 1000022 
	  &&  (*itr)->pdgId() != 39 ) continue;
      missVec += **itr; 
    }

  etMissTrue.m_Ex = missVec.Px();
  etMissTrue.m_Ey = missVec.Py();
  etMissTrue.m_EtMiss = missVec.Et();
  return etMissTrue.m_EtMiss;
}
