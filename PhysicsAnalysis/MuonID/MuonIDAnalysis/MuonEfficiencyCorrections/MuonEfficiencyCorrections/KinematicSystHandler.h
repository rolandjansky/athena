/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTION_SYSTHANDLER_H
#define MUONEFFICIENCYCORRECTION_SYSTHANDLER_H

// EDM include(s):
#include "xAODMuon/Muon.h"
// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

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
#include <exception>
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
            virtual CorrectionCode GetKineDependent(const xAOD::Muon& mu, float& eff) const;
            
            virtual void SetSystematicWeight(float syst_weight);
            
            virtual bool initialize();
            
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
            virtual CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const;
    
            virtual void SetSystematicWeight(float SystWeight);

            virtual bool initialize();
            PrimodialPtSystematic(std::unique_ptr<HistHandler> HistHandler);
        protected:
            std::unique_ptr<HistHandler> m_Handler;
            float m_SystWeight;
    };



    class BadMuonVetoSystHandler: public IKinematicSystHandler {
        public:
            virtual CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const;
            virtual void SetSystematicWeight(float SystWeight);

            virtual bool initialize();
            BadMuonVetoSystHandler(TDirectory* InDir);
            virtual ~BadMuonVetoSystHandler();

        protected:
            CP::CorrectionCode FindAppropiatePolynomial(const xAOD::Muon& mu, TF1* &Poly) const;
            std::string GetNextProperty(std::string &sstr);

            typedef std::pair<float, float> Ranges;
            std::map<Ranges, std::unique_ptr<TF1>> m_SystPolynomials;

            KinVariable m_FirstVar;
            KinVariable m_SecondVar;
            float m_SystWeight;

    };

} /* namespace CP */

#endif /* EFFICIENCYSCALEFACTOR_H_ */
