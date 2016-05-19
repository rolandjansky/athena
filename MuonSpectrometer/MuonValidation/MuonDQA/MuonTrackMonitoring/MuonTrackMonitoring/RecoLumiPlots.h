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

            void fill(const xAOD::MuonSegmentContainer* MuonSegments, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb);
            void fill(const xAOD::TrackParticleContainer* MSTracks, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb);
            void fill_CB(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb);
            void fill_Other(const xAOD::MuonContainer* Muons, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb);
            void fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > resonances, int m_current_lb, float m_inst_lumi_bcid, float m_inst_lumi_lb);

            //set the useful varaibles


            // Set 1D hists for webdisplay

            TH1* m_hNSegment_LB_1D;
            TH1* m_hNMuonTrack_LB_1D;
            TH1* m_hNMuon_LB_1D;
            TH1* m_hNResonance_LB_1D;

            // TH1* m_hNSegment_Inst_1D;
            // TH1* m_hNMuonTrack_Inst_1D;
            // TH1* m_hNMuon_Inst_1D;
            // TH1* m_hNResonance_Inst_1D;

            // TH1* m_hNSegment_IntLumi_1D;
            // TH1* m_hNMuonTrack_IntLumi_1D;
            // TH1* m_hNMuon_IntLumi_1D;
            // TH1* m_hNResonance_IntLumi_1D;

            // TH1* m_hNSegment_LB_BA_1D;
            // TH1* m_hNSegment_LB_BC_1D;
            // TH1* m_hNSegment_LB_EA_1D;
            // TH1* m_hNSegment_LB_EC_1D;

            // Set 2D hists for development

            TH2* m_hNSegment_LB;
            TH2* m_hNMuonTrack_LB;
            TH2* m_hNMuon_LB;
            TH2* m_hNResonance_LB;

            // TH2* m_hNSegment_Inst;
            // TH2* m_hNMuonTrack_Inst;
            // TH2* m_hNMuon_Inst;
            // TH2* m_hNResonance_Inst;

            // TH2* m_hNSegment_IntLumi;
            // TH2* m_hNMuonTrack_IntLumi;
            // TH2* m_hNMuon_IntLumi;
            // TH2* m_hNResonance_IntLumi;

            // TH2* m_hNSegment_LB_BA;
            // TH2* m_hNSegment_LB_BC;
            // TH2* m_hNSegment_LB_EA;
            // TH2* m_hNSegment_LB_EC;


            //for locating Z and Jpsi
            std::string type;
            std::string name;
            
      private:
            void initializePlots();

};

#endif
