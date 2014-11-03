/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAYERANALYSIS_H
#define LAYERANALYSIS_H


#include "MuonLayerHough/HitNtuple.h"
#include "MuonLayerHough/MuonLayerHoughSelector.h"

class TH1F;
class TH2F;

namespace MuonHough {
  
  class MuonDetectorHough;

  struct Plots {
    Plots(const char* title, int nBinsX, float xMin, float xMax, int nBinsY, float yMin, float yMax);
      
    TH2F* Reco;
    TH2F* Truth;
    TH2F* Matched;
    TH2F* Unmatched;
    TH1F* Efficiency;
    TH1F* FakeEfficiency;
	TH2F* Diff;
  };


  class LayerAnalysis {
  public:
    
    LayerAnalysis( TTree& tree ) : m_tree(&tree) {
      m_ntuple.initForRead(tree);
    }
    
    void initialize();
    void analyse();
    void finalize();
    
  private:

    void analysis( std::map<int,SectorData>& data );
    void drawSector( int region, int sector, SectorData& data, MuonDetectorHough& detectorHough, MuonDetectorHough& detectorHoughTruth );
    void calculateVariables(Plots* Plot);

    TTree*    m_tree;
    HitNtuple m_ntuple;
    EventData m_event;
    int       m_ncalls;
    std::vector<MuonDebugInfo> m_muons;
    std::vector<MuonHough::MuonLayerHoughSelector> m_selectors;
    
    std::vector<Plots*> m_hMaximaHeightPerChIndex;    
  };


}


#endif
