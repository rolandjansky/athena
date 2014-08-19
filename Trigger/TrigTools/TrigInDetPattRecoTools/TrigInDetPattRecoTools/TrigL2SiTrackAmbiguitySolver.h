/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_AMBIGUITY_SOLVER_H__
#define __TRIG_L2_SITRACK_AMBIGUITY_SOLVER_H__

class TrigL2SiTrackAmbiguitySolvingCuts;
class TrigL2TrackCandidate;

#include <map>

class TrigL2SiTrackAmbiguitySolver {

  class TrackPairDispatcher {
  public:
    TrackPairDispatcher(TrigL2SiTrackAmbiguitySolver& p, TrigL2TrackCandidate* t) : m_parent(p),
      m_full_trks(t) {

    };
      ~TrackPairDispatcher(){};

      void operator()(std::pair<const float, std::pair<unsigned int, unsigned int> >&);

  protected:
      TrigL2SiTrackAmbiguitySolver& m_parent;
      TrigL2TrackCandidate* m_full_trks;
  };


 public:  
  TrigL2SiTrackAmbiguitySolver(const TrigL2SiTrackAmbiguitySolvingCuts& c) : m_cuts(c)  {};
    ~TrigL2SiTrackAmbiguitySolver(){};

  void execute(TrigL2TrackCandidate*, unsigned int);
  void findClusters(TrigL2TrackCandidate*, unsigned int);
 protected:

  bool compareTracks(const TrigL2TrackCandidate*, const TrigL2TrackCandidate*);
  float trackAmbiguityFraction(const TrigL2TrackCandidate*, const TrigL2TrackCandidate*);
  float trackAmbiguityDistance(const TrigL2TrackCandidate*, const TrigL2TrackCandidate*);
  void trackRemoveAmbiguity(TrigL2TrackCandidate*, TrigL2TrackCandidate*);
  void trackMergeAmbiguity(TrigL2TrackCandidate*, TrigL2TrackCandidate*);
  void trackAmbiguityShared(unsigned int, unsigned int, TrigL2TrackCandidate*, 
			    std::map<unsigned int,std::vector<unsigned int> >&);
  TrigL2SiTrackAmbiguitySolvingCuts m_cuts;



};


#endif
