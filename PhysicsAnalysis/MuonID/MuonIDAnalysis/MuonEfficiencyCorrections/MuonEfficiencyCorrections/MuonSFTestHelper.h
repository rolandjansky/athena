/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H
#define MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H

// EDM include(s):
#include "xAODMuon/Muon.h"

// Tool Includes
#include <MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h>
#include <MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h>
#include <MuonAnalysisInterfaces/IMuonSelectionTool.h>


#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>

#include <AsgTools/IAsgTool.h>
#include <AsgTools/AsgTool.h>

//General includes
#include <memory>
#include <map>
#include <TTree.h>
#include <TH1.h>
#include <TFile.h>
//Helper class to test the Muon efficiency SFs plus their systematics
namespace TestMuonSF {
    //########################################
    //      Template class to connect the    #
    //      Variables with the TTree         #
    //########################################
    class SFBranches {
        public:
            SFBranches(TTree* tree);
            virtual ~SFBranches()= default;
            virtual std::string name() const =0;
            virtual bool init()=0;
        protected:
            template<typename T> bool initBranch(T& Var, const std::string& Syst) {
                std::string bName = name() + (Syst.empty() ? std::string("") : std::string("_")) + Syst;
                if (m_tree->FindBranch(bName.c_str())) {
                    Error("SFBranches::initBranch()", "The branch %s already exists in TTree %s", bName.c_str(), m_tree->GetName());
                    return false;
                }
                if (m_tree->Branch(bName.c_str(), &Var) == nullptr) {
                    Error("SFBranches::initBranch()", "Could not create the branch %s in TTree %s", bName.c_str(), m_tree->GetName());
                    return false;
                } else Info("SFBranches::initBranch()", "Created the branch %s in TTree %s", bName.c_str(), m_tree->GetName());
                return true;
            }
        private:
            TTree* m_tree;

    };
    //####################################################
    //      Helper class to write ntuples to test the    #
    //      MuonTriggerScaleFactors                      #
    //####################################################
    class TriggerSFBranches: public SFBranches {
        public:
            TriggerSFBranches(TTree* tree, const ToolHandle<CP::IMuonTriggerScaleFactors>& Handle, const std::string& Trigger);
            virtual ~TriggerSFBranches() = default;
            CP::CorrectionCode fill(const xAOD::MuonContainer* Muons);
            virtual bool init();
            virtual std::string name() const;
        private:
            CP::CorrectionCode getSF(const xAOD::MuonContainer* muons, double &Var, const CP::SystematicVariation &syst);
            ToolHandle<CP::IMuonTriggerScaleFactors> m_handle;
            std::string m_trigger;
            double m_nominal_SF;
            double m_stat_up_SF;
            double m_stat_down_SF;
            double m_sys_up_SF;
            double m_sys_down_SF;
    };
    typedef std::unique_ptr<TriggerSFBranches> TriggerSFBranch_Ptr;

    //###################################################
    //      General interface for the Reconstruction    #
    //      Isolation / TTVA scalefactors               #
    //###################################################
    class MuonEffiBranches: public SFBranches {
        public:
            MuonEffiBranches(TTree* tree);
            virtual CP::CorrectionCode fill(const xAOD::Muon& muon)=0;
            ~MuonEffiBranches() = default;

    };
    typedef std::unique_ptr<MuonEffiBranches> EffiBranch_Ptr;
    //###################################################################
    //      Helper class to write the scale-factor ntuples to test the  #
    //      MuonReconstruction/ Isolation/ TTVA scalefactors            #
    //###################################################################
    class MuonSFBranches: public MuonEffiBranches {
        public:
             MuonSFBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle, const std::string& rel_name = "");
             CP::CorrectionCode fill(const xAOD::Muon& muon) override;
             virtual ~MuonSFBranches() = default;
             bool init() override;
             std::string name() const override;
        private:
            ToolHandle<CP::IMuonEfficiencyScaleFactors> m_handle;
            bool m_uncorrelate_sys;
            std::string m_release;

            //SF's
            struct SFSet {
                    SFSet() {
                        scale_factor = mc_eff = data_eff = 1.;
                    }
                    float scale_factor;
                    float mc_eff;
                    float data_eff;
            };
            
            CP::CorrectionCode fill_systematic(const xAOD::Muon muon, std::pair<const CP::SystematicSet, MuonSFBranches::SFSet>& set);
            bool AddToTree(const CP::SystematicSet& syst, MuonSFBranches::SFSet& ScaleFactor);

            std::map<CP::SystematicSet, SFSet> m_SFs;

    };
    //#######################################################
    //          Helper class for writing                    #
    //          scale-factor replicas to the test-ntuples   #
    //#######################################################
    class MuonReplicaBranches: public MuonEffiBranches {
        public:
            MuonReplicaBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle, const std::string& rel_name = "");
            CP::CorrectionCode fill(const xAOD::Muon& muon) override;

            bool init() override;
            std::string name() const override;
        private:
            ToolHandle<CP::IMuonEfficiencyScaleFactors> m_handle;
            std::string m_release;
            std::map<CP::SystematicSet, std::vector<float>> m_SFs;
    };

    //###################################################
    //      Helper class to write                       #
    //      The muon properties for the efficiency sfs  #
    //###################################################
    class MuonInfoBranches: public MuonEffiBranches {
        public:
            MuonInfoBranches(TTree* tree, const ToolHandle<CP::IMuonSelectionTool>& sel_tool);
            virtual ~MuonInfoBranches() = default;
            bool init() override;
            std::string name() const override;
            CP::CorrectionCode fill(const xAOD::Muon& muon) override;

        private:
            const ToolHandle<CP::IMuonSelectionTool>& m_selection_tool;
            float m_pt;
            float m_eta;
            float m_phi;
            unsigned int m_quality;
            unsigned int m_author;
            unsigned int m_type;
            bool m_passLowPt;
            bool m_passHighPt;
            uint8_t m_precLayers;
    };

    class MuonSFTestHelper {
        public:

            //Standalone constructor if only one SF release is asked for
            MuonSFTestHelper(const std::string& release_name = "", bool HasOwnerShip = false);
            // Constructor for the comparisons between two releases
            MuonSFTestHelper(std::shared_ptr<TTree> Tree, const std::string& release_name = "");
            MuonSFTestHelper(TTree*Tree, const std::string& release_name = "");

            ~MuonSFTestHelper() = default;

            //Initialize the tool
            bool init();
            void addTool(const asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> &handle);
            void addTool(const ToolHandle<CP::IMuonEfficiencyScaleFactors>& handle);

            void addReplicaTool(const asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> &handle);
            void addReplicaTool(const ToolHandle<CP::IMuonEfficiencyScaleFactors>& handle);

            void setSelectionTool(const asg::AnaToolHandle<CP::IMuonSelectionTool> & sel_tool);
            void setSelectionTool(const ToolHandle<CP::IMuonSelectionTool> & sel_tool);
            
            TTree* tree() const;
            std::shared_ptr<TTree> tree_shared() const;
            CP::CorrectionCode fill(const xAOD::MuonContainer* muons);
            CP::CorrectionCode fill(const xAOD::Muon* mu);
            CP::CorrectionCode fill(const xAOD::Muon& mu);
            void fillTree();

        private:
            std::string m_name;
            std::shared_ptr<TTree> m_tree;
            TTree* m_tree_raw_ptr;
            std::vector<EffiBranch_Ptr> m_Branches;
            ToolHandle<CP::IMuonSelectionTool> m_sel_tool;
    };

}

#endif
