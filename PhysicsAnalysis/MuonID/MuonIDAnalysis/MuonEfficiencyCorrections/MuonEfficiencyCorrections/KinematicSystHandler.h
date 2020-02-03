/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_SYSTHANDLER_H
#define MUONEFFICIENCYCORRECTION_SYSTHANDLER_H

// EDM include(s):
#include "xAODMuon/Muon.h"

#include <PATInterfaces/CorrectionCode.h>
#include <MuonEfficiencyCorrections/HistHandler.h>
#include <MuonEfficiencyCorrections/MuonEfficiencyType.h>
#include <PATInterfaces/ISystematicsTool.h>

// further ROOT includes
#include <TFile.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TF1.h>

// STL includes
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <cmath>

namespace CP {

    //Interface class to retrieve the Muon dependent systematics
    class IKinematicSystHandler {
        public:
            /// Add an additional uncertainty to the muon depending on its kinematics.
            virtual CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const = 0;
            /// Set's the absolute scaling of the systematic. For daily puposes it's usually
            /// either 1 or -1 indicating if the instance is an upwards or downwards variation.
            virtual void SetSystematicWeight(float SystWeight) = 0;
            
            /// Initialize method to load the inputs and check that everything will work as expected.
            virtual bool initialize() = 0;
            virtual ~IKinematicSystHandler() =default;
            
            /// Typedef to prepare function pointers to the muon
            typedef float (IKinematicSystHandler::*KinVariable)(const xAOD::Muon &mu) const;
            KinVariable GetMuonVariableToUse(const std::string &name);

            float Eta(const xAOD::Muon &mu) const;
            float Pt(const xAOD::Muon &mu) const;
            float PtGeV(const xAOD::Muon &mu) const;
            float AbsEta(const xAOD::Muon &mu) const;

    };
    ///     Z->mumu reconstruction scale-factors are delivered in eta-phi maps integrating out any pt-dependence.
    ///     However, the scale-factors as a function of pt show a slight dependence on that variable. An extra binning
    ///     along pt is not feasable, because of the low statistics in each bin. To account for something we might missed
    ///     an additional histogram is saved in the scale-factor map showing the absolute deviation from the bin where
    ///     the probe-distribution has its maximium. On the other hand high pt muons might hit the borders of the T&P method.
    ///     Once in terms of statistics and once in terms of modeling where the muon might suffer from a so-called catastrophic
    ///     energy-loss. To estimate this inefficiency the slope in the efficiency of high-pt muons in the drell-yan sample is
    ///     determined in different spectrometer bins. The systematic designed for muons beyond 200 GeV in transverse momentum.
    ///     
    ///     The two systematics are not applied simultaenously. The first systematic is preferred over the latter if its unvertainty
    ///     is smaller than the total value of the latter itself.
    class PtKinematicSystHandler : public IKinematicSystHandler {
        public:
             CorrectionCode GetKineDependent(const xAOD::Muon& mu, float& eff) const override;
            
            void SetSystematicWeight(float syst_weight) override;
            
            bool initialize() override;
            
            /// Constructor having two histhandler objects inside. The 
            PtKinematicSystHandler(std::unique_ptr<HistHandler> pt_flatnesss, std::unique_ptr<HistHandler> energy_loss);
        private:
            std::unique_ptr<HistHandler> m_flatness;
            std::unique_ptr<HistHandler> m_loss;
            float m_SystWeight;
    };
    ///    The PrimodialPtSystematic was the former PtKinematicSystHandler. It's kept for backwards
    ///    compability to run MuonEfficiencyScaleFactors on files before the Moriond_2019 release. 
    ///     
    
    class PrimodialPtSystematic: public IKinematicSystHandler {
        public:
            CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const override;
    
            void SetSystematicWeight(float SystWeight) override;

            bool initialize() override;
            PrimodialPtSystematic(std::unique_ptr<HistHandler> HistHandler);
        private:
            std::unique_ptr<HistHandler> m_Handler;
            float m_SystWeight;
    };
    ///
    ///     Extra systematic assigned for the TTVA non-closure
    ///
    class TTVAClosureSysHandler: public IKinematicSystHandler {
        public:
            TTVAClosureSysHandler(std::unique_ptr<HistHandler> HistHandler);
            
            void SetSystematicWeight( float SystWeight) override;
            bool initialize() override;
            CorrectionCode GetKineDependent(const xAOD::Muon&mu, float& Eff) const override;
        private:
            std::unique_ptr<HistHandler> m_Handler;
            float m_SystWeight;
    };

    class BadMuonVetoSystHandler: public IKinematicSystHandler {
        public:
             CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const override;
             void SetSystematicWeight(float SystWeight)override;

            bool initialize() override;
            BadMuonVetoSystHandler(TDirectory* InDir_3Stations, TDirectory* InDir_2Stations = nullptr);
            virtual ~BadMuonVetoSystHandler();

        private:
            typedef std::pair<float, float> Ranges;
            
            CP::CorrectionCode findAppropiatePolynomial(const xAOD::Muon& mu, TF1* &Poly) const;
            
            std::string getNextProperty(std::string &sstr) const;
            void fillMap(TDirectory* InDir, std::map<Ranges, std::unique_ptr<TF1>>& systPolynomials); 
            
            std::map<Ranges, std::unique_ptr<TF1>> m_syst3Stations;
            std::map<Ranges, std::unique_ptr<TF1>> m_syst2Stations;
            
            KinVariable m_uncertVar;
            KinVariable m_polySelVar;
            float m_SystWeight;
    };
    
} /* namespace CP */

#endif /* EFFICIENCYSCALEFACTOR_H_ */
