/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONSEGMENTPLOTS_H
#define MUONHISTUTILS_MUONSEGMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/MuonSegment.h"
#include <vector>
#include <string>

namespace Muon{

class MuonSegmentPlots:public PlotBase {
    public:
      MuonSegmentPlots(PlotBase* pParent, std::string sDir);
      ~MuonSegmentPlots();

      void fill(const xAOD::MuonSegment& muonSeg);

      TH1* segmentfitChi2;
      TH1* segmentfitNdof;
      TH1* segmentfitChi2oNdof;

      TH1* t0;
      TH1* t0err;

      TH1* sector;
      TH1* nPrecisionHits;
      TH1* nPhiLayers;
      TH1* nTrigEtaLayers;

      TH2* xypos_barrel;
      TH2* xypos_endcap;
      TH2* rzpos_sectorSmall;
      TH2* rzpos_sectorLarge;

      TH1* etadir;

      TH1* etadir_barrel;
      TH1* etadir_endcap;

      TH1* phidir;
      TH1* etaphidir;

      TH2* sector_perStation;


};
}

#endif
