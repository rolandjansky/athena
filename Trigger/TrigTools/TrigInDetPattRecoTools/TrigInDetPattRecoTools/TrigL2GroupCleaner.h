/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_GROUP_CLEANER_H__
#define __TRIG_L2_GROUP_CLEANER_H__

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2GroupCleanerCuts.h"
#include "TrigInDetPattRecoTools/TrigL2TripletMakerClass.h"


class TrigL2GroupCleaner {

 public:

  TrigL2GroupCleaner(double threshold, TrigL2GroupCleanerCutsTight t, TrigL2GroupCleanerCutsLoose l, 
		     TrigL2HoughTransformClass* pH, long c = 1000) : m_looseTight(threshold), 
    m_tight(t), m_loose(l), m_pHough(pH) {
      m_pTripletMakerTight = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsTight>(t,pH,c);
      m_pTripletMakerLoose = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsLoose>(l,pH,c);
  }

  TrigL2GroupCleaner(const TrigL2GroupCleaner& gc) : m_looseTight(gc.m_looseTight), 
    m_tight(gc.m_tight), m_loose(gc.m_loose), m_pHough(gc.m_pHough) {
    m_pTripletMakerTight = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsTight>(*gc.m_pTripletMakerTight);
    m_pTripletMakerLoose = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsLoose>(*gc.m_pTripletMakerLoose);
  }

  TrigL2GroupCleaner& operator = (const TrigL2GroupCleaner& gc) {
    m_looseTight = gc.m_looseTight; 
    m_tight = gc.m_tight;
    m_loose = gc.m_loose;
    m_pHough = gc.m_pHough;
    m_pTripletMakerTight = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsTight>(*gc.m_pTripletMakerTight);
    m_pTripletMakerLoose = new TrigL2TripletMakerClass<TrigL2GroupCleanerCutsLoose>(*gc.m_pTripletMakerLoose);
    return *this;
  }

  ~TrigL2GroupCleaner() {
    delete m_pTripletMakerTight;
    delete m_pTripletMakerLoose;
  }

  void execute(const TrigL2TrackCandidate*, std::vector<TrigL2TrackCandidate*>&, double, double);

 protected:
  double m_looseTight;
  TrigL2GroupCleanerCutsTight m_tight;
  TrigL2GroupCleanerCutsLoose m_loose;
  TrigL2HoughTransformClass* m_pHough;
  TrigL2TripletMakerClass<TrigL2GroupCleanerCutsTight>*   m_pTripletMakerTight;
  TrigL2TripletMakerClass<TrigL2GroupCleanerCutsLoose>*   m_pTripletMakerLoose;

};


#endif
