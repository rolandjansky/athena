/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
using namespace std;
#include "TrigTauPerformNtuple/TrigTauOfflineEvent.h"

ClassImp(TrigTauOfflineEvent)  

  TrigTauOfflineEvent::TrigTauOfflineEvent() {reset();}
TrigTauOfflineEvent::~TrigTauOfflineEvent() 
{
  reset();
}

void TrigTauOfflineEvent::reset()
{
  for(std::vector<TrigTauElectron*>::iterator itr = electrons.begin(); itr != electrons.end(); ++itr)
    delete *itr;
  electrons.erase(electrons.begin(), electrons.end() );
  for(std::vector<TrigTauMuon*>::iterator itr = muons.begin(); itr != muons.end(); ++itr)
    delete *itr;
  muons.erase(muons.begin(), muons.end() );
  for(std::vector<TrigTauCosmicMuon*>::iterator itr = cosmicMuons.begin(); itr != cosmicMuons.end(); ++itr)
    delete *itr;
  cosmicMuons.erase(cosmicMuons.begin(), cosmicMuons.end() );
  for(std::vector<TrigTauJet*>::iterator itr = jets.begin(); itr != jets.end(); ++itr)
    delete *itr;
  jets.erase(jets.begin(), jets.end() );
  for(std::vector<TrigTauRecOfflineObject*>::iterator itr = tauRec.begin(); itr != tauRec.end(); ++itr)
    delete *itr;
  tauRec.erase(tauRec.begin(), tauRec.end() );
  for(std::vector<TrigTauVertex*>::iterator itr = priVxCandidates.begin();
      itr != priVxCandidates.end(); ++itr)
    delete *itr;
  priVxCandidates.clear();
  
  etMissCalo.reset();
  etMissCalib.reset(); //Topo.reset();
  etMissTopo.reset();//Calib.reset(); //Topo.reset();
  etMissFinal.reset();
  etMissMuon.reset();
  etMissCryo.reset();
  etMissRecFinal.reset();
  etMissLocHadTopo.reset();
}

