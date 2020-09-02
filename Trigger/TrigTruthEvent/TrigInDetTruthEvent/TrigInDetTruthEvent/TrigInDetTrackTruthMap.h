/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         TrigInDetTrackTruthMap.h        
 **
 **   Description:  - Stores a vector of pointers to GenParticles which match 
 **                   a TrigInDetTrack and associated matching quality 
 **                   quantities (nr of common hits only, for now)
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  18 19:55:56 GMT 2006
 **   Modified:     RG - 19 Jun 06 - changed the class to deal with persistency
 **                 it now behaves a bit like a:
 **                 map<ElementLink to TrigInDetTrack, TrigInDetTrackTruth>
 **
 **************************************************************************/ 
#ifndef TRIGINDETTRACK_TRUTH_MAP_H
#define TRIGINDETTRACK_TRUTH_MAP_H

#include "AthenaKernel/CLASS_DEF.h"

#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLinkVector.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"
#include "AtlasHepMC/GenParticle.h" 

#include <iostream>


class TrigInDetTrackTruthMap {
    

 public:

  /** Constructors: POOL needs default constructor */
  TrigInDetTrackTruthMap() : 
    m_elink_vec(0),
    m_truth_vec(0)
    { };
    
  // Destructor
  virtual ~TrigInDetTrackTruthMap() { };
  

  /** accessors to fill map */
  void addMatch(const TrigInDetTrackCollection* trkColl, 
		unsigned int trk_indx, 
		TrigInDetTrackTruth& p_trk_tru);

  /** methods to get truth-match objects */
  // returns true if truth association exists for track
  bool hasTruth(const TrigInDetTrack* p_trig_trk) const; 
  
  // returns the track truth association object
  const TrigInDetTrackTruth* truth(const TrigInDetTrack* p_trig_trk) const;

  // to make the map more useful: return the link to GenParticle which better 
  // matches this track  according to number of common hits
  const HepMcParticleLink* bestMatchSi(const TrigInDetTrack* p_trig_trk) const;
  const HepMcParticleLink* bestMatchTRT(const TrigInDetTrack* p_trig_trk) const;

  // return number of hits for best match
  int bestMatchSiHits(const TrigInDetTrack* p_trig_trk) const; 
  int bestMatchTRTHits(const TrigInDetTrack* p_trig_trk) const; 
   
  void print() const;

  size_t size() const;
  const TrigInDetTrackTruth* truthi (size_t i) const; 
  const ElementLink<TrigInDetTrackCollection> trackiLink (size_t i) const;
  const TrigInDetTrack* tracki (size_t i) const;

 private:
  friend class TrigInDetTrackTruthMapCnv_p1;
  friend class TrigInDetTrackTruthMap_old_cnv;

  // used to be map for fast lookup, but changed to use ElemLink as key
  //  std::vector< ElementLink< DataVector<TrigInDetTrack> > > m_elink_vec;
  //std::vector< ElementLink< TrigInDetTrackCollection > > m_elink_vec;
  ElementLinkVector< TrigInDetTrackCollection >          m_elink_vec;
  std::vector< TrigInDetTrackTruth >                     m_truth_vec;
};

CLASS_DEF( TrigInDetTrackTruthMap , 78130186 , 1 )
#endif 
