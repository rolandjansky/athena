/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H
#define MUONEFFICIENCYCORRECTION_MUONSFTESTHELPER_H

// EDM include(s):
#include "xAODMuon/Muon.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"

//Helper class to test the Muon efficiency SFs plus their systematics

namespace TestMuonSF {
    void WriteHistogram(TFile* File, TH1* &Histo);

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
