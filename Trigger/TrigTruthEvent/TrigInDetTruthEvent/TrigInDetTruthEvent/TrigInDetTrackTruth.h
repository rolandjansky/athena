/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         TrigInDetTrackTruth.h        
 **
 **   Description:  - Stores a vector of pointers to GenParticles which match 
 **                   a TrigInDetTrack and associated matching quality 
 **                   quantities (nr of common hits only for now)
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  18 19:55:56 GMT 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 
#ifndef TRIGINDETTRACK_TRUTH_H
#define TRIGINDETTRACK_TRUTH_H

#include "AthenaKernel/CLASS_DEF.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetTruthEvent/TrigIDHitStats.h"

#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <iostream>

class TrigInDetTrackTruth {
    
 public:

  /** Constructors: POOL needs default constructor */
  TrigInDetTrackTruth() :  
    m_best_match_hits(-1), m_best_Si_match_hits(-1), m_best_TRT_match_hits(-1), m_true_part_vec(0), m_nr_common_hits(0), m_family_tree()
    { };

  /** initialized constructor: easier way to construct an instance 
      if there is just one true particle associated with a track */
  TrigInDetTrackTruth(const HepMcParticleLink p_tru_part, TrigIDHitStats hits)  
    {
      // add this particle to the vector
      m_true_part_vec.push_back(p_tru_part); 

      // add number of hits
      m_nr_common_hits.push_back( hits );
      
      // set the best match index (only 1 match so far)
      m_best_match_hits = 0;
      m_best_Si_match_hits = 0;
      m_best_TRT_match_hits = 0;
    };

  // Destructor
  virtual ~TrigInDetTrackTruth() { };
  
  // accessors: to add links to GenParticles to object and get 
  // index if already filled; addMatch returns index of new entry  
  int addMatch(HepMcParticleLink p_tru_part, TrigIDHitStats hits);
  int index(HepMcParticleLink&) const;
  
  // to get quantities
  const HepMcParticleLink* bestMatch() const;  
  const HepMcParticleLink* bestSiMatch() const;  
  const HepMcParticleLink* bestTRTMatch() const;  
  const HepMcParticleLink* truthMatch(unsigned int i) const;
  unsigned int nrMatches()                  const;
  unsigned int nrCommonHits(unsigned int i) const;
  unsigned int nrCommonSiHits(unsigned int i) const;
  unsigned int nrCommonTRTHits(unsigned int i) const;
  unsigned int nrCommonHitsBestSi() const;
  unsigned int nrCommonHitsBestTRT() const;

  // to handle mother-daughter relationships within matching particles   
  int updateFamilyTree();
  std::vector< std::pair<unsigned int, unsigned int> > getFamilyTree() const;
  bool motherInChain(unsigned int) const;
  int motherIndexInChain(unsigned int) const;
  bool daughtersInChain(unsigned int) const;
  std::vector<unsigned int> daughterIndicesInChain(unsigned int) const;

 private:
  friend class TrigInDetTrackTruthCnv_p1;
  friend class TrigInDetTrackTruthCnv_p1_test;

  // reference best match quantities
  int m_best_match_hits;
  int m_best_Si_match_hits;
  int m_best_TRT_match_hits;

  // vector of HepMcParticleLink pointers and matching quantities
  std::vector<HepMcParticleLink>                  m_true_part_vec;
  std::vector<TrigIDHitStats>                     m_nr_common_hits;
  // vector<pair<int,int>> to act as bidirectional map "mother index <-> daughter index"
  // where the indices refer to the GenPaticle positions in m_true_part_vec vector
  std::vector< std::pair<unsigned int, unsigned int> > m_family_tree; 


};

CLASS_DEF( TrigInDetTrackTruth , 243330893 , 1 )
#endif 
