/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOLUMIPLOTS_H
#define MUONTRACKMONITORING_RECOLUMIPLOTS_H

#include "TrkValHistUtils/PlotBase.h"


#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"

class RecoLumiPlots:public PlotBase {

      public:
            RecoLumiPlots(PlotBase* pParent, std::string sDir, std::string recObj);

            void fill(const xAOD::MuonSegmentContainer* MuonSegments, int current_lb, float inst_lumi_bcid, float inst_lumi_lb);
            void fill(const xAOD::TrackParticleContainer* MSTracks, int current_lb, float inst_lumi_bcid, float inst_lumi_lb);
            void fill_CB(const xAOD::MuonContainer* Muons, int current_lb, float inst_lumi_bcid, float inst_lumi_lb);
            void fill_Other(const xAOD::MuonContainer* Muons, int current_lb, float inst_lumi_bcid, float inst_lumi_lb);
            void fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int current_lb, float inst_lumi_bcid, float inst_lumi_lb);

            TH1* m_hNSegment_LB_1D;
            TH1* m_hNMuonTrack_LB_1D;
            TH1* m_hNMuon_LB_1D;
            TH1* m_hNResonance_LB_1D;

            TH2* m_hNSegment_LB;
            TH2* m_hNMuonTrack_LB;
            TH2* m_hNMuon_LB;
            TH2* m_hNResonance_LB;

            int n_lbs;

            //for locating Z and Jpsi
            std::string type;
            std::string name;
            
      private:
            void initializePlots();

};

#endif
