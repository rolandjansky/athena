/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONSEGMENTTRUTHRELATEDPLOTS_H
#define MUONHISTUTILS_MUONSEGMENTTRUTHRELATEDPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/HitTypePlots.h"
#include "xAODMuon/MuonSegment.h"
#include <vector>
#include <string>

namespace Muon{

class MuonSegmentTruthRelatedPlots:public PlotBase {
    public:
      MuonSegmentTruthRelatedPlots(PlotBase* pParent, const std::string& sDir);
      ~MuonSegmentTruthRelatedPlots();

      void fill(const xAOD::MuonSegment& muonSeg,const xAOD::MuonSegment& truthMuonSeg,float weight=1.0);

      //hit difference plots
      Trk::HitTypePlots nPrecisionHits;
      Trk::HitTypePlots nPhiLayers;
      Trk::HitTypePlots nTrigEtaLayers;

      //resolution plots     
      TH1* dxpos{nullptr};
      TH1* dypos{nullptr};
      TH1* dzpos{nullptr};

      TH1* detapos{nullptr};
      TH1* dphipos{nullptr};

      TH1* detadir{nullptr};
      TH1* dphidir{nullptr};

};
}

#endif
