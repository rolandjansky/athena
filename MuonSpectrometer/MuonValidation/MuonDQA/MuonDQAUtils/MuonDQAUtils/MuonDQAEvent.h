/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:  N. Benekos(Illinois)
// MuonDQA
// Feb. 2008
//
// DESCRIPTION:
// Subject: This class describes one Event which all information
//which are necessary for the Muon-, DiMuon-, Zmumu- and
//InSitu- Performance
///////////////////////////////////////////////////////////////////////////

#ifndef MuonDQAEvent_H
#define MuonDQAEvent_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include "MuonDQAUtils/MSPhysicsObject.h"
#include "MuonDQAUtils/MuonDQATrackObject.h"
#include "TrigSteeringEvent/TriggerDecision.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

using namespace std;

// Ugly method to use TMinuit

#define INFO_RECO1
#define INFO_TRUE2

// This structure is only usefull for dumping the Information of one:MuonDQATrackObject
// to an AANtuple-tree. It is not important for the analysis itself
struct MuonDQAEventConverterObject
{
  vector<long>        *m_pdgID;
  vector<long>        *m_mother;
  vector<double>*m_fPt;
  vector<double>*m_fEta;
  vector<double>*m_fPhi;
  vector<double>*m_fR0;
  vector<double>*m_fZ0;
  vector<int>        *m_charge;
  
  vector<double>*m_fDeltaPt;
  vector<double>*m_fDeltaEta;
  vector<double>*m_fDeltaPhi;
  
  vector<int>        *m_type;
  vector<int>        *m_numberOfHits;
  vector<double>*m_chi2;
  vector<double>*m_chi2oDOF;
  vector<double>*m_energyLoss;
  
  vector<int>        *m_triggerStatus;
  vector<int>        *m_isElectron;
  vector<int>        *m_isZmuon;
  vector<double>*m_trackPt_iso;
  vector<double>        *m_ntrack_iso;
  vector<double>*m_jet_iso;
  vector<double>*m_et_iso;
  vector<double>*m_mass;
};



class MuonDQAEvent
{
 public:
  /** @brief Default Constructor - Empty */
  MuonDQAEvent();
  
  /** @brief Copy Constructor */
  MuonDQAEvent(const MuonDQAEvent& ob);
    
  /** @brief destructor */
  ~MuonDQAEvent (); //!< Destructor
      
  // Overload =
  void operator = (const MuonDQAEvent& ob);
  
  void clear();
  
  // Which MuonReconstruction should be used as standard?
  void  setDefaultTrackReconstruction(string s) { m_sDefaultReconstruction = s; }
  string  getDefaultTrackReconstruction()  { return m_sDefaultReconstruction; }
  
  // Which Trigger should be used as standard?
  void  setDefaultTrigger(string s)          { m_sDefaultTrigger = s;}
  string  getDefaultTrigger()           { return m_sDefaultTrigger; }
  
  // Adding a Track to the event. The Track will be stored in the collection of the current default MuonReconstruction
  void addTrack(ToolHandle<Muon::MuonDQATrackObject> &Track){ mapTracks[m_sDefaultReconstruction].push_back(Track);}
  // Adding TriggerInformation to the event. The Track will be stored in the collection of the current default TriggerName
  void addTriggerInfo(ToolHandle<Muon::MuonDQATrackObject> &Trigger){ mapMuonTriggerInfo[m_sDefaultTrigger].push_back(Trigger);}
  void addParticle(ToolHandle<Muon::MuonDQATrackObject> &Particle){ vecParticles.push_back(Particle);}
  
  // Setting up the Track/Particle Combinations. This must be done before calling getNextTrackCombination or getNextParticleCombination
  void setupTrackCombination()     {m_aktTrackCombination1=0;  m_aktTrackCombination2=1; }
  void setupParticleCombination()   {m_aktParticleCombination1=0;  m_aktParticleCombination2=1; }

  // Setting up the Track/Particle/Trigger Counters. This must be done before calling getNextParticle,...
  void setupParticleCounter() { m_aktParticle=0; }
  void setupTriggerCounter()  { m_aktTrigger=0; }
  void setupTrackCounter()    { m_aktTrack=0;}

  // The following functions are for retrieving stored Tracks, Particles, ... in any possible combination
  bool getNextTrackCombination    (ToolHandle<Muon::MuonDQATrackObject> &Track1, ToolHandle<Muon::MuonDQATrackObject> &Track2);
  bool getNextParticleCombination  (ToolHandle<Muon::MuonDQATrackObject> &Particle1, ToolHandle<Muon::MuonDQATrackObject> &Particle2);
 
  // The following functions are for retrieving stored Tracks, Particles, ... one after the other
  bool getNextParticle    (ToolHandle<Muon::MuonDQATrackObject> &Particle);
  bool getNextTrack    (ToolHandle<Muon::MuonDQATrackObject> &Track);
  bool getNextTrigger    (ToolHandle<Muon::MuonDQATrackObject> &Trigger);
  
  int getNumberOfTracks()      { return mapTracks[m_sDefaultReconstruction].size(); }
  int getNumberOfParticles()    { return vecParticles.size();}
  int getNumberOfTriggerInfos() { return mapMuonTriggerInfo[m_sDefaultTrigger].size(); }
  
  // The following functions are just for exchanging the Information from the MuonDQAEvent-Description to an AAN and via versa
  void convertTracks(std::string id, MuonDQAEventConverterObject &msConOb);
  void convertTriggerInfo(std::string id, MuonDQAEventConverterObject &msConOb);
  void convertParticles(MuonDQAEventConverterObject &msConOb);
  
  void setTracks(std::string id, MuonDQAEventConverterObject &msConOb);
  void setTriggerInfo(std::string id, MuonDQAEventConverterObject &msConOb);
  void setParticles(MuonDQAEventConverterObject &msConOb);
  
  void clearMuonDQAEventConverterObject(MuonDQAEventConverterObject &msConOb);
  void setupMuonDQAEventConverterObject(MuonDQAEventConverterObject &msConOb);
  
  const   TriggerDecision* getTriggerDecision() {return m_trigDec;}
  void    setTriggerDecision(const TriggerDecision *trigDec) {m_trigDec = trigDec;}
  
  bool    isTriggered(unsigned int,std::string);
 private:
  void convertToMSPhysicsObject(vector< ToolHandle<Muon::MuonDQATrackObject> > &vecOb, MuonDQAEventConverterObject &msConOb);
  void convertFromMSPhysicsObject(vector< ToolHandle<Muon::MuonDQATrackObject> > &vevOb, MuonDQAEventConverterObject &msConOb);
  
  string m_sDefaultReconstruction;
  int m_aktTrackCombination1;
  int m_aktTrackCombination2;
  int m_aktParticleCombination1;
  int m_aktParticleCombination2;
  int m_aktTrack;
  
  string m_sDefaultTrigger;
  int m_aktTrigger;
  
  int m_aktParticle;
  
  map<string, vector< ToolHandle<Muon::MuonDQATrackObject> > >mapTracks;
  map<string, vector< ToolHandle<Muon::MuonDQATrackObject> > >mapMuonTriggerInfo;
  vector< ToolHandle<Muon::MuonDQATrackObject> >vecParticles;
  int  NumberOfRecoJets;
  const TriggerDecision* m_trigDec;
  
};

#endif
