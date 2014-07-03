/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAYERANALYSIS_H
#define LAYERANALYSIS_H


#include "MuonLayerHough/HitNtuple.h"

namespace MuonHough {
  
  class MuonDetectorHough;

  class LayerAnalysis {
  public:
    
    LayerAnalysis( TTree& tree ) : m_tree(&tree) {
      m_ntuple.initForRead(tree);
    }
    
    void analyse();
    
  private:

    void analysis( std::map<int,SectorData>& data );
    void drawSector( int region, int sector, SectorData& data, MuonDetectorHough& detectorHough, MuonDetectorHough& detectorHoughTruth );

    TTree*    m_tree;
    HitNtuple m_ntuple;
    EventData m_event;
    int       m_ncalls;
    std::vector<MuonDebugInfo> m_muons;
  };


}


#endif
