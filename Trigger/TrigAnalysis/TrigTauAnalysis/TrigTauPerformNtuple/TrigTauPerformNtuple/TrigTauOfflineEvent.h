/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUOFFLINEEVENT_H_
#define  _TRIGTAUOFFLINEEVENT_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauOfflineEvent.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina
/// Created : December 2007
///
/// DESCRIPTION:
///
/// This class contains the vectors of offline objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauElectron.h"
#include "TrigTauPerformNtuple/TrigTauJet.h"
#include "TrigTauPerformNtuple/TrigTauMuon.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"
#include "TrigTauPerformNtuple/TrigTauRecOfflineObject.h"
#include "TrigTauPerformNtuple/TrigTauCosmicMuon.h"
#include "TrigTauPerformNtuple/TrigTauVertex.h"


class TrigTauOfflineEvent : public TObject
{
 public:

  TrigTauOfflineEvent();
  //TrigTauOfflineEvent(const TrigTauOfflineEvent & other);
  //operator
  virtual ~TrigTauOfflineEvent();
  virtual void reset();


  unsigned int nElectrons()  const { return electrons.size(); };
  unsigned int nMuons()  const { return muons.size(); };
  unsigned int nCosmicMuons()  const { return cosmicMuons.size(); };
  unsigned int nJets()  const { return jets.size(); };
  unsigned int nTauRec()  const {return tauRec.size(); };
  unsigned int nPriVxCandidates()  const {return priVxCandidates.size(); };

  const TrigTauElectron            & electronEntry(unsigned int i ) const
  { return *electrons[i]; };
  const  TrigTauMuon               & muonEntry(unsigned int i ) const
  { return *muons[i]; };
  const  TrigTauJet                & jetEntry(unsigned int i ) const
  { return *jets[i]; };
  const TrigTauRecOfflineObject    & tauRecEntry(unsigned int i ) const
  { return *tauRec[i]; };
  const TrigTauCosmicMuon      & cosmicMuonEntry(unsigned int i ) const
  { return *cosmicMuons[i]; };
  const TrigTauVertex              & priVxCandidateEntry(unsigned int i ) const
  { return *priVxCandidates[i]; };
  
 public:    // ---------------- data members ------------------------  

  TrigTauEtMissObject               etMissCalo;  // same as MET_Base
  TrigTauEtMissObject               etMissCalib;
  TrigTauEtMissObject               etMissTopo;
  TrigTauEtMissObject               etMissFinal;
  TrigTauEtMissObject               etMissMuon;
  TrigTauEtMissObject               etMissCryo;
  TrigTauEtMissObject               etMissRecFinal;
  TrigTauEtMissObject               etMissLocHadTopo;

  std::vector<TrigTauRecOfflineObject*>     tauRec;

  std::vector<TrigTauElectron* >       electrons; // ElectronLoose from ElectronAODCollection
  std::vector<TrigTauMuon*>            muons;          // everything from StacoMuonCollection
  std::vector<TrigTauJet*>             jets;        // everything from Cone4H1TowerParticleJets
  std::vector<TrigTauCosmicMuon*>      cosmicMuons;

  std::vector<TrigTauVertex*>          priVxCandidates;
  
  ClassDef(TrigTauOfflineEvent, 11)  ;
    
};


#endif
