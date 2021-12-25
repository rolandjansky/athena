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
      TH1* dxpos;
      TH1* dypos;
      TH1* dzpos;

      TH1* detapos;
      TH1* dphipos;

      TH1* detadir;
      TH1* dphidir;

};
}

#endif
