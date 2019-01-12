/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
            virtual CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const = 0;
            virtual void SetSystematicWeight(float SystWeight) = 0;
            virtual bool initialize() = 0;
            virtual ~IKinematicSystHandler() {
            }
            typedef float (IKinematicSystHandler::*KinVariable)(const xAOD::Muon &mu) const;
            KinVariable GetMuonVariableToUse(const std::string &name);

            float Eta(const xAOD::Muon &mu) const;
            float Pt(const xAOD::Muon &mu) const;
            float PtGeV(const xAOD::Muon &mu) const;
            float AbsEta(const xAOD::Muon &mu) const;

    };

    class PtDependentSystHandler: public IKinematicSystHandler {
        public:
            virtual CorrectionCode GetKineDependent(const xAOD::Muon &mu, float& Eff) const;
            virtual void SetSystematicWeight(float SystWeight);

            virtual bool initialize();
            PtDependentSystHandler(std::unique_ptr<HistHandler> HistHandler);
            virtual ~PtDependentSystHandler();
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
