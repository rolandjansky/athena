/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUMUONTPTreeTOOL_H_
#define ZMUMUMUONTPTreeTOOL_H_
#include "MuonTPTools/IMuonTPTreeTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/IBadMuonVetoHelperTool.h"
#include "MuonTPTools/IMuonTPVertexHelper.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
#include "MuonEfficiencyCorrections/DetRegionBinning.h"
#include "AsgTools/AsgTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "FsrUtils/IFsrPhotonTool.h"
#include <map>

// a small helper class used when filling the truth matching info for the tag and the probe
// The constructor fills the information into the internal branches, which can be accessed by the user.
class TruthMatchOutcome {

    public:
        TruthMatchOutcome(const xAOD::IParticle & part);
        int getType() {
            return m_type;
        }
        int getOrigin() {
            return m_origin;
        }
        int getMatchPdg();
        bool isPromptTruthMuon();

    private:

        int m_type;
        int m_origin;
        // avoid a public dependency on xAODTruth via the IParticle here
        const xAOD::IParticle* m_match;
        bool m_is_truthprobe;
};

// tool responsible for filling the user-specified content (not the match / trig branches) of the TP trees
class DiMuonTPTreeTool: public MuonTPTreeTool {         // for now, a 1:1 copy
        ASG_TOOL_CLASS(DiMuonTPTreeTool, IMuonTPTreeTool)

   public:
        DiMuonTPTreeTool(std::string name);
        virtual StatusCode initialize();
        virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
        virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

    protected:
        //fill modified covariance matrix of the muon probes
        void FillMuProbeModifiedCovMtx(const xAOD::Muon* muprobe);
        // fill hit info for muon probes
        void FillMuProbeHitInfo(const xAOD::Muon* muprobe);
        // fill track related detail info for muon probes
        void FillMuProbeTrkDetails(const xAOD::Muon* muprobe);
        // fill event shape info
        void FillEventShapeInfo();
        // Fill the jet FSR info
        void FillFSRInfo(Probe & probe);
        // Fill the jet association info - please call *after* the filling of FSR info!
        void FillJetAssocInfo(Probe & probe);
        // fill info for the prompt lepton tagger
        void FillPromptLeptonTaggerInfo(Probe & probe);
        // fill info for the match objects
        void FillMatchObjectInfo(Probe & probe);
        // helper method for filling qoverp info
        void FillQoverP(const xAOD::TrackParticle* TP, float & qoverp, float & delta_qoverp);
        // helper method for filling the bad muon veto info
        void FillBadMuonVetoInfo();
        // helper method for filling truth matching info
        void FillTruthMatchInfo(Probe & probe);

        // helper method to create branches for the "per match object" info
        void AddBranchesForMatch(std::map<std::string, int> & thing, TTree* tree, std::string name);
        // vertex info
        void FillVertexInfo(Probe & probe);
        //Vertex density
        void FillVertexDensity();

        void SetIsolationValue(const xAOD::IParticle* Particle, float &IsoVar, xAOD::Iso::IsolationType type);
        UInt_t m_runNumber;
        ULong64_t m_eventNumber;
        int m_mcChannelNumber;
        UInt_t m_lumiblock;
        float m_vertex_density;
        float m_average_mu;
        float m_actual_mu;
        int m_PV_n;
        int m_BCID;

        float m_mcEventWeight;
        float m_pt;
        float m_eta;
        float m_etaMS;
        float m_phi;
        int m_type;    // 1 for CB, 2 for SA, 3 for ST, 4 for CT
        int m_author;
        int m_quality;
        bool m_passHighPt;
        int m_fineEtaPhi;
        int m_detregion;
        int m_q;
        float m_d0;
        float m_d0err;
        float m_z0;

        uint8_t m_primarySector;
        uint8_t m_secondarySector;
        uint8_t m_phiLayer1Hits;
        uint8_t m_phiLayer2Hits;
        uint8_t m_phiLayer3Hits;
        uint8_t m_phiLayer4Hits;

        uint8_t m_innerSmallHits;
        uint8_t m_innerLargeHits;
        uint8_t m_middleSmallHits;
        uint8_t m_middleLargeHits;
        uint8_t m_outerSmallHits;
        uint8_t m_outerLargeHits;
        uint8_t m_extendedSmallHits;
        uint8_t m_extendedLargeHits;

        uint8_t m_innerSmallHoles;
        uint8_t m_innerLargeHoles;
        uint8_t m_middleSmallHoles;
        uint8_t m_middleLargeHoles;
        uint8_t m_outerSmallHoles;
        uint8_t m_outerLargeHoles;
        uint8_t m_extendedSmallHoles;
        uint8_t m_extendedLargeHoles;

        uint8_t m_phiLayer1Holes;
        uint8_t m_phiLayer2Holes;
        uint8_t m_phiLayer3Holes;
        uint8_t m_phiLayer4Holes;

        uint8_t m_probe_precisionLayers;
        uint8_t m_probe_precisionHoleLayers;

        uint8_t m_probe_phiLayers;
        uint8_t m_probe_phiHoleLayers;

        uint8_t m_probe_trigEtaLayers;
        uint8_t m_probe_trigEtaHoleLayers;

        float m_probe_EnergyLoss;
        float m_probe_ParamEnergyLoss;
        float m_probe_MeasEnergyLoss;

        uint8_t m_numberOfGoodPrecisionLayers;

        uint8_t m_innerClosePrecisionHits;
        uint8_t m_middleClosePrecisionHits;
        uint8_t m_outerClosePrecisionHits;
        uint8_t m_extendedClosePrecisionHits;

        uint8_t m_innerOutBoundsPrecisionHits;
        uint8_t m_middleOutBoundsPrecisionHits;
        uint8_t m_outerOutBoundsPrecisionHits;
        uint8_t m_extendedOutBoundsPrecisionHits;
        uint8_t m_combinedTrackOutBoundsPrecisionHits;

        uint8_t m_numberOfOutliersOnTrack;

        uint8_t m_isEndcapGoodLayers;
        uint8_t m_isSmallGoodSectors;

        uint16_t m_allauthors;

        float m_qoverp_CB;
        float m_qoverp_err_CB;

        float m_qoverp_MS;
        float m_qoverp_err_MS;

        float m_qoverp_ME;
        float m_qoverp_err_ME;

        float m_qoverp_ID;
        float m_qoverp_err_ID;

        float m_probe_mod_qoverp;
        float m_probe_mod_qoverp_err;


        float m_chi2;
        float m_nDof;

        float m_momentumBalanceSignificance;

        int m_probe_CaloMuonIDTag;
        float m_probe_CaloLRLikelihood;

        float m_tagPt;
        float m_tagEta;
        float m_tagPhi;
        int m_tag_q;
        float m_tag_d0;
        float m_tag_d0err;
        float m_tag_z0;

        float m_mll;
        float m_dPhi;
        float m_dEta;
        float m_dPhi_Pivot;
        float m_dEta_Pivot;
        float m_dR_Pivot;
        float m_Phi_Pivot;
        float m_Eta_Pivot;
        float m_dilep_pt;
        // additional variables associated with vertex refitting
        bool m_dilep_vertex_found;
        float m_dilep_vertex_refInvMass;
        float m_dilep_vertex_lxy; // uses MIN_A0
        float m_dilep_vertex_lxy_MAX_SUM_PT2;
        float m_dilep_vertex_lxy_MIN_A0;
        float m_dilep_vertex_lxy_MIN_Z0;
        float m_dilep_vertex_tof; // uses MIN_A0
        float m_dilep_vertex_tof_MAX_SUM_PT2;
        float m_dilep_vertex_tof_MIN_A0;
        float m_dilep_vertex_tof_MIN_Z0;
        float m_dilep_vertex_chi2_fit;
        int m_dilep_vertex_dof_fit;

        UInt_t m_n_Jet20;
        float m_tag_deltaR;
        float m_tag_jetpt;
        float m_deltaR;
        float m_closest_jetpt;
        float m_closest_jeteta;
        float m_closest_jetphi;
        UInt_t m_closest_ntrks500;
        UInt_t m_closest_ntrks4000;
        float m_closest_jetjvt;
        float m_closest_jetptrel;
        float m_closest_jetemfrac;
        float m_closest_jetchargedfrac;
        float m_closest_jetfsrdr;

        float m_closest_truth_jetpt;
        float m_closest_truth_jeteta;
        float m_closest_truth_jetphi;
        float m_closest_truth_jetptrel;


        float m_leading_jetptrel;
        float m_leading_jetemfrac;
        float m_leading_jetchargedfrac;
        float m_leading_jetfsrdr;

        float m_sum_jetptrel;

        bool m_probe_hasFSRcand;
        int m_probe_FSRtype;
        float m_probe_FSR_dR;
        float m_probe_FSR_Eta;
        float m_probe_FSR_Phi;
        float m_probe_FSR_F1;
        float m_probe_FSR_Et;
        float m_probe_FSR_Etrel;

        float m_closest_jetDrToFsr;
        float m_closest_jetPtRel_fsrfrac;
        float m_closest_jetPt_fsrfrac;

        float m_leading_jetDrToFsr;
        float m_leading_jetPtRel_fsrfrac;
        float m_leading_jetPt_fsrfrac;

        int m_probe_truth_pdgId;
        int m_probe_truth_type;
        int m_probe_truth_origin;

        int m_tag_truth_pdgId;
        int m_tag_truth_type;
        int m_tag_truth_origin;

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

        float m_probe_pl_tagweight;
        short m_probe_pl_trackjetntrack;

        float m_tag_pl_tagweight;
        short m_tag_pl_trackjetntrack;

        float m_energyDensity_central;
        float m_energyDensity_forward;
        float m_FCAL_Et;

        float m_EventBadMuonVetoVar_VeryLoose_ME;
        float m_EventBadMuonVetoVar_VeryLoose_ID;

        float m_EventBadMuonVetoVar_Loose_ME;
        float m_EventBadMuonVetoVar_Loose_ID;

        float m_EventBadMuonVetoVar_Medium_ME;
        float m_EventBadMuonVetoVar_Medium_ID;

        float m_EventBadMuonVetoVar_Tight_ME;
        float m_EventBadMuonVetoVar_Tight_ID;

        float m_EventBadMuonVetoVar_HighPt_ME;
        float m_EventBadMuonVetoVar_HighPt_ID;

        // auxiliary var - cache the FSR candidate for later jet association
        TLorentzVector m_FSR_candidate;

        fineEtaPhiBinning m_fepb;
        DetRegionBinning m_epb;
        ToolHandle<CP::IMuonSelectionTool> m_muon_selection_tool;
        ToolHandle<IBadMuonVetoHelperTool> m_bad_muon_veto_helper;
        ToolHandle<IJetSelector> m_jet_cleaning_tool;

        bool m_extended_Iso_Info;
        bool m_extended_Trig_Info;
        bool m_write_matchobj_info;

        bool m_add_Truth_Matching;
        bool m_tag_isTruthMatched;
        bool m_probe_isTruthMatched;

        std::map<std::string, int> m_matchobj_combinedTrackOutBoundsPrecisionHits;
        std::map<std::string, int> m_matchobj_innerClosePrecisionHits;
        std::map<std::string, int> m_matchobj_middleClosePrecisionHits;
        std::map<std::string, int> m_matchobj_outerClosePrecisionHits;
        std::map<std::string, int> m_matchobj_extendedClosePrecisionHits;
        std::map<std::string, int> m_matchobj_innerOutBoundsPrecisionHits;
        std::map<std::string, int> m_matchobj_middleOutBoundsPrecisionHits;
        std::map<std::string, int> m_matchobj_outerOutBoundsPrecisionHits;
        std::map<std::string, int> m_matchobj_extendedOutBoundsPrecisionHits;
        std::map<std::string, int> m_matchobj_numberOfOutliersOnTrack;

        ToolHandle<FSR::IFsrPhotonTool> m_IFsrPhotonTool;
        ToolHandle<IMuonTPVertexHelper> m_vertexHelper;

        // this flag turns off a lot of aux info we do not really need at truth level
        bool m_is_truthprobe;

        // this flag is responsible for toggling the J/Psi specific vertexing stuff
        bool m_do_jpsiVertexing;

};

#endif /* ZMUMUMUONTPTreeTOOL_H_ */
