/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_IISOLATIONCLOSEBYCORRECTIONTOOL_H
#define ISOLATIONSELECTION_IISOLATIONCLOSEBYCORRECTIONTOOL_H

#include <PATInterfaces/CorrectionCode.h>
#include <IsolationSelection/Defs.h>
#include <PATCore/TAccept.h>

#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/PhotonContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODBase/IParticleContainer.h>
#include <AsgTools/IAsgTool.h>


namespace CP {
    class IIsolationCloseByCorrectionTool: public virtual asg::IAsgTool {

            ASG_TOOL_INTERFACE (CP::IIsolationCloseByCorrectionTool)

        public:
            enum TopoConeCorrectionModel {
                DirectCaloClusters = -1, ParticleCaloCorrection = 0, CaloCorrectionExtCore = 1, CaloCorrectionVarAnulus = 2
            };

            // This function calculates and applies (the particle is not redecorated) the corrections for close-by objects for each isolation variables and tests whether the particle passes the isolation working point after the corrections.
            // Note that to use this functionality, a IsolationSelectionTool must have been passed to the tool (which must have been intialised indicating which isolation working point to use).
            // The result returned is a TAccept object which is the decision made by the tool with respect to the particle passing the working point.
            virtual Root::TAccept acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const = 0;
            virtual Root::TAccept acceptCorrected(const xAOD::IParticle& x, const xAOD::IParticleContainer& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const = 0;

            // This function calculates the values of the corrections for close-by objects to be applied to the isolation variables (without redecorating the particles).
            // The corrections are returned in a vector (one correction per isolation type provided).
            // This function is intended for experts only who want to check the effects of the corrections.
            virtual CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const = 0;
            virtual CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const xAOD::IParticleContainer& closePar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const=0;

            virtual CorrectionCode getCloseByIsoCorrection(xAOD::ElectronContainer* Electrons = nullptr, xAOD::MuonContainer* Muons = nullptr, xAOD::PhotonContainer* Photons = nullptr, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const=0;
            virtual CorrectionCode subtractCloseByContribution(xAOD::IParticle& x, const xAOD::IParticleContainer& closebyPar, int topoetconeModel = TopoConeCorrectionModel::DirectCaloClusters) const= 0;

            virtual float getOriginalIsolation(const xAOD::IParticle& P, IsoType type) const=0;
            virtual float getOriginalIsolation(const xAOD::IParticle* P, IsoType type) const=0;

            virtual ~IIsolationCloseByCorrectionTool() {
            }
    };

}
#endif
