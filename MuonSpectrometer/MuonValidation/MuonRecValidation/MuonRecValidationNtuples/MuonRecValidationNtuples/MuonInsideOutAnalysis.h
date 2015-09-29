/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTANALYSIS_H
#define MUON_MUONINSIDEOUTANALYSIS_H

#include "MuonRecValidationNtuples/MuonInsideOutAnalysisPlots.h"
#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"
#include "MuonRecValidationNtuples/TimePointBetaFit.h"

class TTree;
class TDirectory;

namespace Muon {

  class MuonInsideOutAnalysis {
  public:
    enum MatchingStrategy {
      SameTrack = 0,
      NotSameTrack = 1,
      All = 2
    };

    /** create analsyis for a given tree */
    MuonInsideOutAnalysis( TDirectory* dir );
    
    /** analyse tree */
    void analyse( TTree& tree );
    
  private:
    bool testStrategy( MuonInsideOutAnalysis::MatchingStrategy matchingStrategy, bool sameTrack ) const;

    unsigned int handleHits( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                             const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                             HitPlots& plots );
    unsigned int handleHough( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                              const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                              HoughPlots& plots );
    unsigned int handleSegments( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                                 const std::vector<int>& indexVec, int stage, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy,
                                 SegmentPlots& segmentPlots );

    void handleTimes( MuonInsideOutValidationNtuple& ntuple, unsigned int trkIndex,
                      const std::vector<int>& indexVec, MuonInsideOutAnalysis::MatchingStrategy matchingStrategy, 
                      TimePlots& plotsType0, TimePlots& plotsType1 );

    TimePointBetaFit::FitResult fitBeta( MuonInsideOutValidationNtuple& ntuple, const std::vector<int>& indexVec, int type );
    
    MuonInsideOutAnalysisPlots plots;
  };
}

#endif
