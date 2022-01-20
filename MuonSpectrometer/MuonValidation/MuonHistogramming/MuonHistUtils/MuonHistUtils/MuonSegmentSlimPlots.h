/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONSEGMENTSLIMPLOTS_H
#define MUONHISTUTILS_MUONSEGMENTSLIMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

namespace Muon{

class MuonSegmentSlimPlots:public PlotBase {
    public:
      MuonSegmentSlimPlots(PlotBase* pParent, const std::string& sDir);
      ~MuonSegmentSlimPlots();

      void fill(const xAOD::MuonSegment& muonSeg, float weight=1.0);

      TH1* segmentfitChi2{nullptr};
      TH1* segmentfitNdof{nullptr};
      TH1* segmentfitChi2oNdof{nullptr};

      TH1* t0{nullptr};
      TH1* t0_top{nullptr};
      TH1* t0_bottom{nullptr};
      TH1* t0err{nullptr};
      TH1* t0err_top{nullptr};
      TH1* t0err_bottom{nullptr};
      
      TH1* nPrecisionHits{nullptr};
      TH1* nPhiLayers{nullptr};
      TH1* nTrigEtaLayers{nullptr};

      TH1* etaIndex{nullptr};
      TH1* sector{nullptr};
      TH1* etadir{nullptr};
      TH1* etadir_barrel{nullptr};
      TH1* etadir_endcap{nullptr};
      TH1* phidir{nullptr};
      TH2* etaphidir{nullptr};

      TH1* chamberIndex{nullptr};
      TH2* chamberIndex_perSector{nullptr};
      TH2* eff_chamberIndex_perSector_numerator{nullptr};
      TH2* eff_chamberIndex_perSector_denominator{nullptr};
      TH2* eff_chamberIndex_perSector{nullptr};

      static constexpr std::array<float,17> Chamberarea {0.465313, 0.589744, 0.393503, 0.516815, 0.404094, 
                                    0.588759, 0.0700091, 0.204258, 0.50283, 0.577781, 
                                    0.902194, 0.484968, 0.746214, 0.111742, 0.192025, 
                                    0.380506, 0.380506};//eta * phi for each chamber
      static constexpr std::array<float,17> Chamberexpectedhits {8.0, 8.0, 6.0, 6.0, 6.0, 6.0, 4.0,
                                    8.0, 8.0, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0, 4.0, 4.0};//number of expected hits for each chamber
      // BIS, BIL, BMS, BML, BOS, BOL, BEE, EIS, EIL, EMS, EML, EOS, EOL, EES, EEL, CSS, CSL
      // 5 and 3 are sacled down from 6 and 4 based on Z MC; could be wrong
      static constexpr std::array<float,17> Chamberexpectedtrighits {1.0, 1.0, 5.0, 5.0, 3.0, 3.0, 1.0,
                                    2.0, 2.0, 7.0, 7.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};//number of expected hits for each chamber


};
}

#endif
