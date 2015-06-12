/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONSEGMENTPLOTS_H
#define MUONHISTUTILS_MUONSEGMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
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
      TH1* t0_top;
      TH1* t0_bottom;
      TH1* t0err;
      TH1* t0err_top;
      TH1* t0err_bottom;
      
      TH1* nPrecisionHits;
      TH1* nPhiLayers;
      TH1* nTrigEtaLayers;
      TH2* nPrecisionHits_nTriggerHits;

      TH2* B_MDT_nPhiLayers_phi;
      TH2* B_MDT_nPhiLayers_eta;
      TH2* B_MDT_zeroPhiLayers_eta_phi;
      TH2* B_MDT_withPhiLayers_eta_phi;

      TH1* etaIndex;
      TH1* sector;
      TH2* sector_perStation;
            
      TH2* xypos_barrel;
      TH2* xypos_endcap;
      TH2* rzpos_sectorSmall;
      TH2* rzpos_sectorSmall_splitY;
      TH2* rzpos_sectorLarge;
      TH2* rzpos_sectorLarge_splitY;

      TH1* etadir;
      TH1* etadir_barrel;
      TH1* etadir_endcap;
      TH1* phidir;
      TH1* etaphidir;

      TH1* chamberIndex;
      TH2* chamberIndex_perSector;

      std::vector<TH2*> sector_etaIndex;

      const float myPi = 3.14159;
      const char *StationName[8] = {"BI", "BM", "BO", "BE", "EI", "EM", "EO", "EE"};//For filling in monitoring plots
};
}

#endif
