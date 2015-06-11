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

            void fill(const xAOD::MuonSegmentContainer* MuonSegments, int m_current_lb, float m_inst_lumi);
            void fill(const xAOD::TrackParticleContainer* MSTracks, int m_current_lb, float m_inst_lumi);
            void fill(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi);
            void fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int m_current_lb, float m_inst_lumi);

            //set the useful varaibles

            // Reco only information

            TH2* m_hNSegment_LB;
            TH2* m_hNMuonTrack_LB;
            //TH2* m_hNExtrapMuonTrack_LB;
            TH2* m_hNMuon_LB;
            TH2* m_hNResonance_LB;

            TH2* m_hNSegment_Inst;
            TH2* m_hNMuonTrack_Inst;
            //TH2* m_hNExtrapMuonTrack_Inst;
            TH2* m_hNMuon_Inst;
            TH2* m_hNResonance_Inst;

            //for locating Z and Jpsi
            std::string type;
            std::string name;
      private:
            void initializePlots();

};

#endif
