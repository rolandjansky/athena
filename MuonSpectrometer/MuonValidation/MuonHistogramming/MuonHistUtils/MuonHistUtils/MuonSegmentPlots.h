/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      MuonSegmentPlots(PlotBase* pParent, const std::string& sDir);
      ~MuonSegmentPlots();

      void fill(const xAOD::MuonSegment& muonSeg, float weight=1.0);

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

      // TH2* B_MDT_nPhiLayers_phi;
      // TH2* B_MDT_nPhiLayers_eta;
      // TH2* B_MDT_zeroPhiLayers_eta_phi;
      // TH2* B_MDT_withPhiLayers_eta_phi;

      TH1* etaIndex;
      TH1* sector;
      //TH2* sector_perStation;
            
      TH2* xypos_barrel;
      TH2* xypos_endcap;
      TH2* rzpos_sectorSmall;
      //TH2* rzpos_sectorSmall_splitY;
      TH2* rzpos_sectorLarge;
      //TH2* rzpos_sectorLarge_splitY;

      TH1* etadir;
      TH1* etadir_barrel;
      TH1* etadir_endcap;
      TH1* phidir;
      TH1* etaphidir;

      TH1* chamberIndex;
      TH2* chamberIndex_perSector;
      TH2* eff_chamberIndex_perSector_numerator;
      TH2* eff_chamberIndex_perSector_denominator;
      TH2* eff_chamberIndex_perSector;

      //TH2* chamberIndex_dtheta;
      std::vector<TH2*> sector_etaIndex;
      std::vector<TH2*> sector_etaIndex_nPrechit;
      std::vector<TH2*> sector_etaIndex_nTrighit;
      std::vector<TH2*> eff_sector_etaIndex_nPrechit;
      std::vector<TH2*> eff_sector_etaIndex_nTrighit;

      //const char *StationName[9] = {"BI", "BM", "BO", "BE", "EI", "EM", "EO", "EE", "CS"};//For filling in monitoring plots
      const float Chamberarea[17] = {0.465313, 0.589744, 0.393503, 0.516815, 0.404094, 
                                    0.588759, 0.0700091, 0.204258, 0.50283, 0.577781, 
                                    0.902194, 0.484968, 0.746214, 0.111742, 0.192025, 
                                    0.380506, 0.380506};//eta * phi for each chamber
      const float Chamberexpectedhits[17] = {8.0, 8.0, 6.0, 6.0, 6.0, 6.0, 4.0,
                                    8.0, 8.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 4.0, 4.0};//number of expected hits for each chamber
      // BIS, BIL, BMS, BML, BOS, BOL, BEE, EIS, EIL, EMS, EML, EOS, EOL, EES, EEL, CSS, CSL
      // 5 and 3 are sacled down from 6 and 4 based on Z MC; could be wrong
      const float Chamberexpectedtrighits[17] = {1.0, 1.0, 5.0, 5.0, 3.0, 3.0, 1.0,
                                    2.0, 2.0, 7.0, 7.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};//number of expected hits for each chamber


};
}

#endif
