/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


    #ifndef ZMUMUMUONTPTreeTOOL_H_
    #define ZMUMUMUONTPTreeTOOL_H_
    #include "MuonTPTools/IMuonTPTreeTool.h"
    #include "MuonTPTools/IMuonTPSelectionTool.h"
    #include "MuonTPTools/IMuonTPEfficiencyTool.h"
    #include "MuonTPTools/MuonTPTreeTool.h"
    #include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
    #include "MuonPerformanceHistUtils/EtaPhiBinning.h"
    #include "AsgTools/AsgTool.h"
    #include "MuonSelectorTools/MuonSelectionTool.h"
    #include <map>

class DiMuonTPTreeTool:
        public MuonTPTreeTool {         // for now, a 1:1 copy
        ASG_TOOL_CLASS(DiMuonTPTreeTool, IMuonTPTreeTool)

    public:
            DiMuonTPTreeTool(std::string name);
        virtual StatusCode initialize();
        virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
        virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

    protected:
        UInt_t m_runNumber;
        ULong64_t m_eventNumber;
        UInt_t m_lumiblock;
        float m_average_mu;
        float m_actual_mu;
        int m_PV_n;
        
        float m_mcEventWeight;
        float m_pt;
        float m_eta;
        float m_phi;
        int m_type;    // 1 for CB, 2 for SA, 3 for ST, 4 for CT
        int m_author;
        int m_quality;
        int m_fineEtaPhi;
        int m_detregion;
        int m_q;
        float m_d0;
        float m_d0err;
        float m_z0;
        
        float m_tagPt;
        float m_tagEta;
        float m_tagPhi;
        int m_tag_q;
        float m_tag_d0;
        float m_tag_d0err;
        float m_tag_z0;
        
        float m_mll;
        float m_dPhi;
        float m_dilep_pt;
        float m_dilep_tof;
        
        float m_tag_deltaR;
        float m_deltaR;
        
        
        
            // Extra iso stuff
        float m_tagiso_neflowisol20;
        float m_tagiso_topoetcone20;
        float m_tagiso_ptcone20;
        float m_tagiso_etcone20;
        float m_tagiso_ptvarcone20;

        float m_tagiso_neflowisol30;
        float m_tagiso_topoetcone30;
        float m_tagiso_ptcone30;
        float m_tagiso_etcone30;
        float m_tagiso_ptvarcone30;

        float m_tagiso_neflowisol40;
        float m_tagiso_topoetcone40;
        float m_tagiso_ptcone40;
        float m_tagiso_etcone40;
        float m_tagiso_ptvarcone40;

        float m_tag_topocore;

        float m_probeiso_neflowisol20;
        float m_probeiso_topoetcone20;
        float m_probeiso_ptcone20;
        float m_probeiso_etcone20;
        float m_probeiso_ptvarcone20;

        float m_probeiso_neflowisol30;
        float m_probeiso_topoetcone30;
        float m_probeiso_ptcone30;
        float m_probeiso_etcone30;
        float m_probeiso_ptvarcone30;

        float m_probeiso_neflowisol40;
        float m_probeiso_topoetcone40;
        float m_probeiso_ptcone40;
        float m_probeiso_etcone40;
        float m_probeiso_ptvarcone40;

        float m_probe_topocore;

        float m_energyDensity_central;
        float m_energyDensity_forward;
            
        
        fineEtaPhiBinning m_fepb;
        EtaPhiBinning m_epb;
        ToolHandle<CP::IMuonSelectionTool> m_muon_selection_tool;
        
        bool m_extended_Iso_Info;
            
};

    #endif /* ZMUMUMUONTPTreeTOOL_H_ */
