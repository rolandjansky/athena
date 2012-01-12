/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUEVENT_H
#define TRIGTAUEVENT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauEvent.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina
/// Created : January 2006
///
/// DESCRIPTION:
///
/// Object containing all neccessary information for tau trigger performance studies. 
/// To be dumped to root ntuple
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "TObject.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"
#include "TrigTauPerformNtuple/TrigTauMCEvent.h"
#include "TrigTauPerformNtuple/TrigTauRecOfflineObject.h"
#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"
#include "TrigTauPerformNtuple/TrigTauTrackCollection.h"
#include "TrigTauPerformNtuple/TrigTauL1Object.h"
#include "TrigTauPerformNtuple/TrigTauJetL1Object.h"
#include "TrigTauPerformNtuple/TrigTauEtMissL1Object.h"
#include "TrigTauPerformNtuple/TrigTauEtMissL2Object.h"
#include "TrigTauPerformNtuple/TrigTauEtMissEFObject.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TrigTauPerformNtuple/TrigTauRoI.h"
#include "TrigTauPerformNtuple/TrigTauOfflineEvent.h"
#include "TrigTauPerformNtuple/TrigTauTauChain.h"
#include "TrigTauPerformNtuple/TrigTauDetectorStatus.h"

//----------------------------------------------------------------
// This class will be changed in time according to EDM development
//----------------------------------------------------------------


class TrigTauEvent : public TObject
{
 public:

  TrigTauEvent();
  //TrigTauEvent(const TrigTauEvent & other);
  //operator
  virtual ~TrigTauEvent();

  virtual void reset();

  unsigned int nTauRec()        const {return offlineEvent.tauRec.size(); };
  unsigned int nTauL2()         const {return tauL2.size(); };
  unsigned int nTauL1()         const {return tauL1.size(); };
  unsigned int nJetL1()         const {return jetL1.size(); };
  unsigned int nTauEF()         const {return tauEF.size(); };
  unsigned int nTauChains()      const {return tauChains.size(); };
  unsigned int nTracksRoIEF()   const {return tracksRoIEF.size(); };
  unsigned int nMC()            const { return mcEvent.particleList.size(); };
  unsigned int nOtherTrigRoI()  const { return otherTrigRoI.size(); };

  const TrigTauRecOfflineObject & tauRecEntry(unsigned int i ) const
  { return *offlineEvent.tauRec[i]; };
  const TrigTauL2Object     & tauL2Entry(unsigned int i ) const
  { return *tauL2[i]; };
  const TrigTauL1Object     & tauL1Entry(unsigned int i ) const
  { return *tauL1[i]; };
  const TrigTauJetL1Object  & jetL1Entry(unsigned int i ) const
  { return *jetL1[i]; };
  const TrigTauRecEFObject  & tauEFEntry(unsigned int i ) const
  { return *tauEF[i]; };
  const TrigTauTauChain    & tauChainEntry(unsigned int i ) const
  { return *tauChains[i]; };
  const TrigTauTrackCollection & tracksRoIEFEntry(unsigned int i ) const
  { return *tracksRoIEF[i]; };
  const  TrigTauRoI         & otherTrigRoIEntry(unsigned int i ) const
  { return *otherTrigRoI[i]; };
  const  TrigTauTrueParticle & MCEntry(unsigned int i ) const
  { return *mcEvent.particleList[i]; };

  TrigTauRecOfflineObject    & tauRecEntry(unsigned int i ) 
  { return *offlineEvent.tauRec[i]; };
  TrigTauL2Object     & tauL2Entry(unsigned int i ) 
  { return *tauL2[i]; };
  TrigTauL1Object     & tauL1Entry(unsigned int i ) 
  { return *tauL1[i]; };
  TrigTauJetL1Object  & jetL1Entry(unsigned int i ) 
  { return *jetL1[i]; };
  TrigTauRecEFObject  & tauEFEntry(unsigned int i ) 
  { return *tauEF[i]; };
  TrigTauTauChain    & tauChainEntry(unsigned int i ) 
  { return *tauChains[i]; };
  TrigTauTrackCollection & trackRoIEFEntry(unsigned int i )
  { return *tracksRoIEF[i]; };
  TrigTauRoI         & otherTrigRoIEntry(unsigned int i ) 
  { return *otherTrigRoI[i]; };
  TrigTauTrueParticle & MCEntry(unsigned int i ) 
  { return *mcEvent.particleList[i]; };



  bool same(const TLorentzVector & oneObject, const TLorentzVector & anotherObject,
	    float dPhiMax = 0.3, float dEtaMax = 0.3) const ;

  // Use following function to find a match for a given object, e.g. L2Object for TrueParticle
  // -1 is returned if no match is found
   int bestMatch(const TLorentzVector & object, 
		 std::vector<TLorentzVector*>::const_iterator ifirst,
		 std::vector<TLorentzVector*>::const_iterator iend,
		 float dPhiMax = 0.3, float dEtaMax = 0.3) const;


  int eventHour() const;
  int eventMinute() const;
  int eventSecond() const;
  TrigTauDetectorStatus               detectorStatus;

  // ---------------- data members ------------------------  

 public:
  int                                 runNumber;
  int                                 eventNumber;
  int                                 time_stamp;
  int                                 time_stamp_ns_offset;
  int                                 lumi_block;
  int                                 bunch_crossing;

  // extended bunch info
  bool bunch_isFilled;        // false for empty bunches, true otherwise
  bool bunch_isInTrain;       // false for single bunches, true otherwise
  int  bunch_distFromFrontBC; // distance from front in # of bunch crossings
  int  bunch_distFromTailBC;  // distance from tail in # of bunch crossings
  int  bunch_distFromFrontFilled; // distance from front in # of filled bunches
  int  bunch_distFromTailFilled;  // distance from tail in # of filled bunches
  
  TrigTauFlags                        decision;   
  TrigTauEtMissL1Object               etMissL1;
  TrigTauEtMissL2Object               etMissL2;
  TrigTauEtMissEFObject               etMissEF;
  TrigTauEtMissEFObject               etMissEF_FEB;
  TrigTauEtMissEFObject               etMissEF_noiseSupp;
  TrigTauEtMissEFObject               etMissEF_allCells;
  std::vector<TrigTauJetL1Object*>     jetL1;
  std::vector<TrigTauL1Object*>        tauL1;
  std::vector<TrigTauL2Object*>        tauL2;
  std::vector<TrigTauRecEFObject*>     tauEF;
  std::vector<TrigTauTrackCollection*> tracksRoIEF;
  
  std::vector<TrigTauTauChain*>       tauChains; // this links L1/L2/EF exactly as HLT does


  TrigTauMCEvent                      mcEvent;
  TrigTauOfflineEvent                 offlineEvent;

  std::vector<TrigTauRoI*>             otherTrigRoI; // does NOT include tau RoIs, but might have overlaping RoIs
  
  float                              mbts_A_time;
  float                              mbts_C_time;
  float                              lar_A_time;
  float                              lar_C_time;

  ClassDef(TrigTauEvent, 16) 
    
};

#endif //TRIGTAUEVENT_H
