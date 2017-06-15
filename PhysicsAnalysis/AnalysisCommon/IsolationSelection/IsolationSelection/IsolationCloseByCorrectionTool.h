/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IsolationSelection_IsolationCloseByCorrectionTool_H
#define IsolationSelection_IsolationCloseByCorrectionTool_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "xAODTracking/VertexContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

namespace CP {

    class IsolationCloseByCorrectionTool: public asg::AsgTool, public virtual IIsolationCloseByCorrectionTool {

            ASG_TOOL_CLASS(IsolationCloseByCorrectionTool, IIsolationCloseByCorrectionTool)

            public:
            IsolationCloseByCorrectionTool (const std::string& name);
            virtual ~IsolationCloseByCorrectionTool() {
            }


            virtual StatusCode initialize();
            virtual StatusCode finalize();

            virtual const CP::CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;

            virtual const Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;

        private:
            CP::CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle* par, xAOD::Iso::IsolationType type, const std::set<const xAOD::TrackParticle*>& closePar) const;

            const CP::CorrectionCode getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar) const;
            const CP::CorrectionCode getCloseByCorrectionTopoetcone(float& correction, const xAOD::IParticle& par, xAOD::Iso::IsolationType type, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const;
            const xAOD::Vertex* retrieveIDBestPrimaryVertex() const;

            const CP::CorrectionCode getExtrapEtaPhi(const xAOD::IParticle& par, float& eta, float& phi) const;
            xAOD::Iso::IsolationType getIsolationTypeFromString(const TString& isoTypeString, xAOD::Type::ObjectType particleType) const;

            //New Functions added by jojungge

            //Returns the Size of the Isolation cone
            double ConeSize(const xAOD::IParticle* P, xAOD::Iso::IsolationType Cone) const;
            //Retrieves the uncalibrated pt from the particle
            double UnCalibPt(const xAOD::IParticle* P) const;

            //Some helper functions for Overlap and DeltaR
            bool IsSame(const xAOD::IParticle* P, const xAOD::IParticle* P1) const;
            bool Overlap(const xAOD::IParticle* P, const xAOD::IParticle* P1, double dR) const;
            double DeltaR2(const xAOD::IParticle* P, const xAOD::IParticle* P1) const;


            bool IsFixedTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool IsVarTrackIso(xAOD::Iso::IsolationType Iso) const;
            bool IsTrackIso(xAOD::Iso::IsolationType Iso) const;

            const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle* par) const;
            const xAOD::IParticle* TrackIsoRefPart(const xAOD::IParticle* P) const;


            std::set<const xAOD::TrackParticle*> GetAssociatedTracks(const xAOD::IParticle* P) const;
            void GetTrackCandidates(const xAOD::IParticleContainer* Container, const xAOD::Vertex* Vtx, std::set<const xAOD::TrackParticle*> &Tracks);

            void GetTrackCandidates(const xAOD::IParticle* P, const xAOD::IParticleContainer* CloseByPar, std::set<const xAOD::TrackParticle*> &Tracks);
            bool ConsiderForCorrection(const xAOD::IParticle* P) const;


            ToolHandle<CP::IIsolationSelectionTool> m_selectorTool;
            float m_coreCone;
            float m_ptvarconeRadius;
            mutable Root::TAccept m_accept;
            bool m_isInitialised;
            bool m_isCoreSubtracted;
            std::string m_indetTrackParticleLocation;
            std::string m_VertexContainerName;
            std::vector<xAOD::Iso::IsolationType> m_muon_isoTypes;
            std::vector<xAOD::Iso::IsolationType> m_electron_isoTypes;
            std::vector<xAOD::Iso::IsolationType> m_photon_isoTypes;

            asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;

    };
}
#endif
