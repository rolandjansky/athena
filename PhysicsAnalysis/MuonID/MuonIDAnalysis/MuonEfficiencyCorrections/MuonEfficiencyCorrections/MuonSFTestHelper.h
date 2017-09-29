/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H
#define MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H

// EDM include(s):
#include "xAODMuon/Muon.h"
#include <TTree.h>
#include <TH1.h>

//Helper class to test the Muon efficiency SFs plus their systematics
namespace CP{
    class IMuonTriggerScaleFactors;
    class IMuonEfficiencyScaleFactors;
}
namespace TestMuonSF {
    
    void WriteHistogram(TFile* File, TH1* &Histo);
    
    
    //####################################################
    //      Helper class to write ntuples to test the 
    //      MuonTriggerScaleFactors
    //###################################################
    class TriggerSFBranches {
        public:
            TriggerSFBranches(TTree* tree,const  ToolHandle<CP::IMuonTriggerScaleFactors>& Handle, const std::string& Trigger);
            CP::CorrectionCode fill(const xAOD::MuonContainer* Muons);
            bool init();
            std::string name() const;
        private:
            bool initBranch(double& Var, const std::string &Syst);
            CP::CorrectionCode getSF(const xAOD::MuonContainer* muons, double &Var, const CP::SystematicVariation &syst);
            
            TTree* m_tree;
            const ToolHandle<CP::IMuonTriggerScaleFactors>& m_handle;
            std::string m_trigger;
            
            double m_nominal_SF;
            double m_stat_up_SF;
            double m_stat_down_SF;
            double m_sys_up_SF;
            double m_sys_down_SF;
    };
    typedef std::unique_ptr<TriggerSFBranches> TriggerSFBranch_Ptr;
    //#################################################
    //      Helper class to write ntuples to test the
    //      MuonReconstruction/ Isolation/ TTVA scalefactors
    //#################################################
    class MuonEffiBranches{
        public:
            MuonEffiBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle);
            CP::CorrectionCode fill (const xAOD::Muon* muon);
            CP::CorrectionCode fill (const xAOD::Muon& muon);
            
            bool init();
            std::string name() const;
        private:
            bool initBranch(float& Var, const std::string &Syst);
            
            TTree* m_tree;
            const ToolHandle<CP::IMuonEfficiencyScaleFactors>& m_handle;
            
            float m_nominal_SF;
            std::map<CP::SystematicSet, float> m_systematics;
    };
    typedef std::unique_ptr<MuonEffiBranches> EffiBranch_Ptr;
    
    
    class MuonSFTestHelper {
        public:
            MuonSFTestHelper(const std::string& Name);
            StatusCode initialize();
            template<typename T> StatusCode setProperty(const std::string &Name, T Value) {
                return m_Tool.setProperty(Name, Value);
            }
            CP::CorrectionCode TestSF(const xAOD::Muon& mu);
            bool WriteHistosToFile(TFile* file);
            void PrintSFs(bool B);
            void FillHistos(bool B);
            ~MuonSFTestHelper();

            CP::SystematicCode GetMCEfficiency(float &eff, const CP::SystematicSet &Set) const;
            CP::SystematicCode GetEfficiency(float &eff, const CP::SystematicSet &Set) const;
            CP::SystematicCode GetScaleFactor(float &eff, const CP::SystematicSet &Set) const;

            const CP::MuonEfficiencyScaleFactors& GetTool() const;

        protected:
            void CreateHistogram(CP::SystematicSet& Set);

            CP::MuonEfficiencyScaleFactors m_Tool;
            CP::SystematicSet m_Nominal;
            std::vector<CP::SystematicSet*> m_Syst;
            struct TestScaleFactors {
                    TH1* SF;
                    TH1* eff;
                    TH1* mceff;
                    TH1* relSystSF;
                    TH1* relSystEff;
                    TH1* relSystmcEff;
                    float sf_Value;
                    float efficiency_Value;
                    float mcefficiency_Value;
            };
            std::map<CP::SystematicSet*, TestScaleFactors> m_Histos;
            bool m_init;
            bool m_Print;
            bool m_FillHistos;
    };
    class MuonSFReleaseComparer {
        public:
            MuonSFReleaseComparer(MuonSFTestHelper& Rel1, MuonSFTestHelper& Rel2);
            StatusCode initialize();
            CP::CorrectionCode TestSF(const xAOD::Muon& mu);
            bool WriteHistosToFile(TFile* file);
            ~MuonSFReleaseComparer();
            protected:
            void CreateHistogram(const CP::SystematicSet &Set);

            MuonSFTestHelper& m_Rel1;
            MuonSFTestHelper& m_Rel2;

            struct ComparingHistos {
                    TH1* SF;
                    TH1* eff;
            };
            std::map<CP::SystematicSet, ComparingHistos> m_Histos;

    };

}

#endif
