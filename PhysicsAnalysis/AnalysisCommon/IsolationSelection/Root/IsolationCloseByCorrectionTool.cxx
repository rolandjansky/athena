/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationCloseByCorrectionTool.h>
#include <IsolationSelection/IsolationWP.h>

#include <xAODPrimitives/IsolationHelpers.h>
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <xAODPrimitives/tools/getIsolationCorrectionAccessor.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include <InDetTrackSelectionTool/InDetTrackSelectionTool.h>

#include <xAODBase/IParticleHelpers.h>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include <FourMomUtils/xAODP4Helpers.h>

//Tools includes:
#include <cmath>
#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);

namespace CP {

    IsolationCloseByCorrectionTool::IsolationCloseByCorrectionTool(const std::string& toolName) :
                asg::AsgTool(toolName),
                m_selectorTool(""),
                m_coreCone(0.1),
                m_ptvarconeRadius(1.e4),
                m_accept(),
                m_isInitialised(false),
                m_isCoreSubtracted(false),
                m_indetTrackParticleLocation("InDetTrackParticles"),
                m_VertexContainerName("PrimaryVertices"),
                m_CaloClusterContainerName("CaloCalTopoClusters"),
                m_muon_isoTypes(),
                m_electron_isoTypes(),
                m_photon_isoTypes(),
                m_quality_name(),
                m_passOR_name(),
                m_isoSelection_name(),
                m_acc_quality(),
                m_acc_passOR(),
                m_backup_prefix(),
                m_trkselTool(),
                m_isohelpers() {
        //IMPORTANT USER PROPERTIES
        declareProperty("IsolationSelectionTool", m_selectorTool, "Please give me your configured IsolationSelectionTool!");

        //OPTIONAL PROPERTIES
        m_trkselTool.declarePropertyFor(this, "TrackSelectionTool"); // Makes the track selection tool a settable property of this tool
        declareProperty("SelectionDecorator", m_quality_name, "Name of the char auxdata defining whether the particle shall be considered for iso correction");
        declareProperty("PassOverlapDecorator", m_passOR_name, "Does the particle also need to pass the overlap removal?");
        declareProperty("IsolationSelectionDecorator", m_isoSelection_name, "Name of the final isolation decorator.");
        declareProperty("BackupPrefix", m_backup_prefix, "Prefix in front of the isolation variables, if the original cone values need  to  be backuped");

        //EXPERT PROPERTIES
        declareProperty("TrackParticleLocation", m_indetTrackParticleLocation, "This is the location of the Inner Detector track particle (for the container).");
        declareProperty("VertexContainer", m_VertexContainerName, "Name of the primary vertex container");

        declareProperty("CoreCone", m_coreCone = 0.1, "This is the size of the core cone for the topoetcone variables.");
        declareProperty("PtvarconeRadius", m_ptvarconeRadius = 10000., "This is the kT parameter for the ptvarcone variables.");

    }

    StatusCode IsolationCloseByCorrectionTool::initialize() {

        //set default properties of track selection tool, if the user hasn't configured it
        if (!m_trkselTool.isUserConfigured()) {
            ATH_MSG_INFO("No TrackSelectionTool provided, so I will create and configure my own, called: " << m_trkselTool.name());
            ATH_CHECK(m_trkselTool.setProperty("maxZ0SinTheta", 3.));
            ATH_CHECK(m_trkselTool.setProperty("minPt", 1000.));
            ATH_CHECK(m_trkselTool.setProperty("CutLevel", "Loose"));
        }

        ATH_CHECK(m_trkselTool.retrieve());

        ATH_CHECK(m_selectorTool.retrieve());
        GetIsolationTypes(m_selectorTool->getElectronWPs(), m_electron_isoTypes);
        GetIsolationTypes(m_selectorTool->getMuonWPs(), m_muon_isoTypes);
        GetIsolationTypes(m_selectorTool->getPhotonWPs(), m_photon_isoTypes);
        if (!m_quality_name.empty()) {
            m_acc_quality = SelectionAccessor(new CharAccessor(m_quality_name));
            m_quality_name.clear();
        }
        if (!m_passOR_name.empty()) {
            m_acc_passOR = SelectionAccessor(new CharAccessor(m_passOR_name));
            m_passOR_name.clear();
        }
        if (!m_isoSelection_name.empty()) {
            m_dec_isoselection = SelectionDecorator(new CharDecorator(m_isoSelection_name));
            m_isoSelection_name.clear();
        }
        m_isInitialised = true;
        return StatusCode::SUCCESS;
    }
    void IsolationCloseByCorrectionTool::GetIsolationTypes(const std::vector<IsolationWP*> &WPs, std::vector<xAOD::Iso::IsolationType> & types) {
        types.clear();
        for (const auto& W : WPs) {
            for (auto& C : W->conditions()) {
                if (!IsElementInList(types, C->type())) types.push_back(C->type());
                if (m_isohelpers.find(C->type()) == m_isohelpers.end()) {
                    m_isohelpers.insert(IsoHelperPair(C->type(), IsoHelperPtr(new IsoVariableHelper(C->type(), m_backup_prefix))));
                }
            }
        }
    }

    CorrectionCode IsolationCloseByCorrectionTool::getCloseByIsoCorrection(xAOD::ElectronContainer* Electrons, xAOD::MuonContainer* Muons, xAOD::PhotonContainer* Photons) const {
        if (!m_isInitialised) {
            ATH_MSG_ERROR("The IsolationCloseByCorrectionTool was not initialised!!!");
            return CorrectionCode::Error;
        }
        const xAOD::Vertex* Vtx = retrieveIDBestPrimaryVertex();
        if (!Vtx) {
            ATH_MSG_ERROR("No vertex was found");
            return CorrectionCode::Error;
        }
        //Retrieve all tracks associated with the objects
        TrackCollection Tracks;
        GetTrackCandidates(Electrons, Vtx, Tracks);
        GetTrackCandidates(Muons, Vtx, Tracks);
        GetTrackCandidates(Photons, Vtx, Tracks);
        //Now grep every cluster with dR< m_CoreCone to the object
        ClusterCollection Clusters;
        GetClusterCandidates(Electrons, Clusters);
        GetClusterCandidates(Muons, Clusters);
        GetClusterCandidates(Photons, Clusters);
        if (performCloseByCorrection(Electrons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        else if (performCloseByCorrection(Muons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        else if (performCloseByCorrection(Photons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        return CorrectionCode::Ok;
    }
    CP::CorrectionCode IsolationCloseByCorrectionTool::performCloseByCorrection(xAOD::IParticleContainer* Container, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const {
        if (!m_isInitialised) {
            ATH_MSG_ERROR("The IsolationCloseByCorrectionTool was not initialised!!!");
            return CorrectionCode::Error;
        }
        if (!Container) {
            ATH_MSG_DEBUG("No particle container was given");
            return CorrectionCode::Ok;
        }
        for (auto Particle : *Container) {
            if (subtractCloseByContribution(Particle, AssocTracks, AssocClusters) == CorrectionCode::Error) return CorrectionCode::Error;
        }

        return CorrectionCode::Ok;
    }
    CP::CorrectionCode IsolationCloseByCorrectionTool::subtractCloseByContribution(xAOD::IParticle* Particle, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const {

        if (!ConsiderForCorrection(Particle)) return CorrectionCode::Ok;
        if (Particle->type() == xAOD::Type::ObjectType::Electron) {
            if (subtractCloseByContribution(Particle, m_electron_isoTypes, AssocTracks, AssocClusters) == CorrectionCode::Error) {
                ATH_MSG_ERROR("Failed to correct the electron isolation with pt: " << Particle->pt() / 1.e3 << " GeV" << " eta: " << Particle->eta() << " phi: " << Particle->phi());
                return CorrectionCode::Error;
            }
        } else if (Particle->type() == xAOD::Type::ObjectType::Muon) {
            if (subtractCloseByContribution(Particle, m_muon_isoTypes, AssocTracks, AssocClusters) == CorrectionCode::Error) {
                ATH_MSG_ERROR("Failed to correct the muon isolation with pt: " << Particle->pt() / 1.e3 << " GeV" << " eta: " << Particle->eta() << " phi: " << Particle->phi());
                return CorrectionCode::Error;
            }

        } else if (Particle->type() == xAOD::Type::ObjectType::Photon) {
            if (subtractCloseByContribution(Particle, m_photon_isoTypes, AssocTracks, AssocClusters) == CorrectionCode::Error) {
                ATH_MSG_ERROR("Failed to correct the photon isolation with pt: " << Particle->pt() / 1.e3 << " GeV" << " eta: " << Particle->eta() << " phi: " << Particle->phi());
                return CorrectionCode::Error;
            }
        } else {
            ATH_MSG_ERROR("Unknown particle type");
            return CorrectionCode::Error;
        }
        return CorrectionCode::Ok;
    }

    CP::CorrectionCode IsolationCloseByCorrectionTool::subtractCloseByContribution(xAOD::IParticle* par, const IsoVector& types, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const {
        std::vector<float> ConeValues(types.size(), 0.);
        std::vector<float>::iterator Cone = ConeValues.begin();
        for (const auto& t : types) {
            if (IsTrackIso(t) && !AssocTracks.empty()) {
                if (getCloseByCorrectionTrackIso((*Cone), par, t, AssocTracks) == CorrectionCode::Error) {
                    ATH_MSG_ERROR("Failed to apply track correction");
                    return CorrectionCode::Error;
                }
            }
            if (IsTopoEtIso(t) && !AssocClusters.empty()) {
                if (getCloseByCorrectionTopoIso((*Cone), par, t, AssocClusters) == CorrectionCode::Error) {
                    ATH_MSG_ERROR("Failed to apply track correction");
                    return CorrectionCode::Error;
                }
            }
            ++Cone;
        }
        return CorrectionCode::Ok;
    }

    const CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        }

        corrections.clear();
        CP::CorrectionCode Error = CP::CorrectionCode::Error;

        for (unsigned int i = 0; i < types.size(); i++) {

            xAOD::Iso::IsolationType isoType = types.at(i);

            float correction = 0.0;

            bool typeFound = false;
            if ((isoType == xAOD::Iso::IsolationType::topoetcone20) || (isoType == xAOD::Iso::IsolationType::topoetcone30) || (isoType == xAOD::Iso::IsolationType::topoetcone40)) {
                if (getCloseByCorrectionTopoetcone(correction, par, isoType, closePar, topoetconeModel) == Error) {
                    return Error;
                } else {
                    typeFound = true;
                }
            }

            if ((isoType == xAOD::Iso::IsolationType::ptcone20) || (isoType == xAOD::Iso::IsolationType::ptcone30) || (isoType == xAOD::Iso::IsolationType::ptcone40)) {
                if (getCloseByCorrectionTrackIso(correction, par, isoType, closePar) == Error) {
                    return Error;
                } else {
                    typeFound = true;
                }
            }
            if ((isoType == xAOD::Iso::IsolationType::ptvarcone20) || (isoType == xAOD::Iso::IsolationType::ptvarcone30) || (isoType == xAOD::Iso::IsolationType::ptvarcone40)) {
                if (getCloseByCorrectionTrackIso(correction, par, isoType, closePar) == Error) {
                    return Error;
                } else {
                    typeFound = true;
                }
            }

            if (!typeFound) {
                ATH_MSG_WARNING("The isolation type was " << isoType << " was not recognized.");
                return Error;
            }

            corrections.push_back(correction);

        }

        return CP::CorrectionCode::Ok;

    }
    TrackCollection IsolationCloseByCorrectionTool::GetAssociatedTracks(const xAOD::IParticle* P) const {
        if (P->type() == xAOD::Type::Muon || P->type() == xAOD::Type::TrackParticle) {
            TrackCollection { getTrackParticle(P) };
        }
        const xAOD::Egamma* EG = dynamic_cast<const xAOD::Egamma*>(P);
        return xAOD::EgammaHelpers::getTrackParticles(EG);
    }
    ClusterCollection IsolationCloseByCorrectionTool::GetAssociatedClusters(const xAOD::IParticle* P) const {
        ClusterCollection clusters;
        const xAOD::CaloClusterContainer* topoClusters = nullptr;
        if (!evtStore()->contains < xAOD::CaloClusterContainer > (m_CaloClusterContainerName) || !evtStore()->retrieve(topoClusters, m_CaloClusterContainerName).isSuccess()) {
            ATH_MSG_DEBUG("The cluster container " << m_CaloClusterContainerName << " is not in the event");
            return clusters;
        }
        for (const auto& cluster : *topoClusters) {
            if (!cluster || fabs(cluster->eta()) > 7.0 || cluster->pt() <= 1.e-3) continue;
            if (Overlap(cluster, P, m_coreCone)) clusters.insert(cluster);
        }
        return clusters;
    }

    void IsolationCloseByCorrectionTool::GetTrackCandidates(const xAOD::IParticleContainer* Particles, const xAOD::Vertex* Vtx, TrackCollection &Tracks) const {
        if (!Particles) {
            ATH_MSG_DEBUG("No particles were given to retrieve track candidates");
            return;
        }
        for (const auto P : *Particles) {
            if (!ConsiderForCorrection(P)) continue;
            TrackCollection AssocCloseTrks = GetAssociatedTracks(P);
            for (auto& T : AssocCloseTrks) {
                if (T && m_trkselTool->accept(*T, Vtx)) Tracks.insert(T);
            }
        }
    }
    void IsolationCloseByCorrectionTool::GetClusterCandidates(const xAOD::IParticleContainer* Particles, ClusterCollection& Clusters) const {
        if (!Particles) {
            ATH_MSG_DEBUG("No particles were given to retrieve cluster candidates");
            return;
        }
        for (const auto P : *Particles) {
            ClusterCollection AssocClusters = GetAssociatedClusters(P);
            for (auto& C : AssocClusters)
                Clusters.insert(C);
        }
    }

    bool IsolationCloseByCorrectionTool::ConsiderForCorrection(const xAOD::IParticle* P) const {
        if (P == nullptr) return false;
        if (m_acc_quality && (!m_acc_quality->isAvailable(*P) || !m_acc_quality->operator()(*P))) return false;
        if (m_acc_passOR && (!m_acc_passOR->isAvailable(*P) || !m_acc_passOR->operator()(*P))) return false;
        return true;
    }
    CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle* par, xAOD::Iso::IsolationType type, const TrackCollection& closePar) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        } else if (!IsTrackIso(type)) {
            ATH_MSG_ERROR("Invalid isolation type");
            return CP::CorrectionCode::Error;
        }
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(type);
        if (Itr == m_isohelpers.end() || Itr->second->BackupIsolation(par) == CorrectionCode::Error || Itr->second->GetOrignalIsolation(par, correction) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
            return CP::CorrectionCode::Error;
        } else if (correction <= 0.0) return CP::CorrectionCode::Ok;

        double MaxDR = ConeSize(par, type);
        TrackCollection ToExclude = GetAssociatedTracks(par);
        const xAOD::IParticle* Ref = TrackIsoRefPart(par);
        for (auto& T : closePar) {
            if (Overlap(Ref, T, MaxDR) && ToExclude.find(T) != ToExclude.end()) {
                correction -= T->pt();
            }
        }
        return CP::CorrectionCode::Ok;
    }
    CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTopoIso(float& correction, const xAOD::IParticle* par, xAOD::Iso::IsolationType type, const ClusterCollection& clusters) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        } else if (!IsTopoEtIso(type)) {
            ATH_MSG_ERROR("Invalid isolation type");
            return CP::CorrectionCode::Error;
        }
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(type);
        if (Itr == m_isohelpers.end() || Itr->second->BackupIsolation(par) == CorrectionCode::Error || Itr->second->GetOrignalIsolation(par, correction) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
            return CP::CorrectionCode::Error;
        } else if (correction <= 0.0) return CP::CorrectionCode::Ok;

        ATH_MSG_DEBUG("Isolation variable of particle with pt " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << " ,phi: " << par->phi() << " before correction: " << correction / 1.e3 << " GeV");
        double MaxDR = ConeSize(par, type);
        for (auto& cluster : clusters) {
            if (Overlap(cluster, par, MaxDR) && !Overlap(cluster, par, m_coreCone)) {
                correction -= ClusterEtMinusTile(cluster);
            }
        }
        ATH_MSG_DEBUG("Isolation variable of particle with pt " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << " ,phi: " << par->phi() << " after correction: " << correction / 1.e3 << " GeV");
        return CP::CorrectionCode::Ok;
    }
    const CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTrackIso(float&, const xAOD::IParticle&, xAOD::Iso::IsolationType, const std::vector<const xAOD::IParticle*>&) const {
        return CP::CorrectionCode::Ok;
    }

    const CP::CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTopoetcone(float&, const xAOD::IParticle&, xAOD::Iso::IsolationType, const std::vector<const xAOD::IParticle*>&, int) const {
//
//        float coneSize = xAOD::Iso::coneSize(type);
//
//        if (!m_isInitialised) {
//            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
//        }
//
//        // If the track isolation is already nil or negative, no need to perform overlap removal.
//        auto accIso = xAOD::getIsolationAccessor(type);
//        if (!accIso->isAvailable(par)) {
//            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
//            return CP::CorrectionCode::Error;
//        }
//        else {
//            float isoVal = (*accIso)(par);
//            if (isoVal <= 0.0) {
//                correction = 0.0;
//                return CP::CorrectionCode::Ok;
//            }
//        }
//
//        // Retrieves the averaged eta and phi of the particle.
//        float par1_eta = par.eta();
//        float par1_phi = par.phi();
//        getExtrapEtaPhi(&par, par1_eta, par1_phi);
//
//        correction = 0.0;
//
//        // Retrieves the topocluster container if not provided and if requested (topoetconeModel == -1).
//        std::vector<const xAOD::CaloCluster*> clusters;
//        const xAOD::CaloClusterContainer* topoClusters = 0;
//        if (topoetconeModel == -1) {
//            bool foundContainer = false;
//            if (!evtStore()->retrieve(topoClusters, "CaloCalTopoClusters").isSuccess()) {
//                ATH_MSG_WARNING("Could not retrieve the topocluster container. Will use a simple model.");
//                topoetconeModel = 0;
//            } else {
//                foundContainer = true;
//            }
//
//            // Only keeps the potential topocluster candidates for removal in a vector.
//            if (foundContainer) {
//                for (auto cluster : *topoClusters) {
//                    float etaClu = cluster->eta();
//                    float phiClu = cluster->phi();
//                    float deltaEtaClu = par1_eta - etaClu;
//                    float deltaPhiClu = xAOD::P4Helpers::deltaPhi(par1_phi, phiClu);
//                    float drClu = sqrt(deltaEtaClu * deltaEtaClu + deltaPhiClu * deltaPhiClu);
//                    if ((drClu > m_coreCone) && (drClu <= coneSize)) {
//                        clusters.push_back(cluster);
//                    }
//                }
//            }
//        }
//
//        for (unsigned int i = 0; i < closePar.size(); i++) {
//
//            // Retrieves the averaged eta and phi of the particle.
//            const xAOD::IParticle* par2 = closePar.at(i);
//            float par2_eta = par2->eta();
//            float par2_phi = par2->phi();
//            getExtrapEtaPhi(par2, par2_eta, par2_phi);
//            float deta = par1_eta - par2_eta;
//            float dphi = xAOD::P4Helpers::deltaPhi(par1_phi, par2_phi);
//            float dr = sqrt(deta * deta + dphi * dphi);
//
//            ATH_MSG_VERBOSE("Cone size: " << coneSize);
//            ATH_MSG_VERBOSE("Particle " << i << " pT: " << par2->pt());
//            ATH_MSG_VERBOSE("Particle " << i << " eta: " << par2_eta);
//            ATH_MSG_VERBOSE("Particle " << i << " phi: " << par2_phi);
//            ATH_MSG_VERBOSE("Particle " << i << " type: " << par2->type());
//
//            // If requested by the model, removes the core topoclusters.
////            if (topoetconeModel == -1) {
////                std::vector<const xAOD::CaloCluster*> clustersTemp;
////                for (unsigned int j = 0; j < clusters.size(); j++) {
////                    const xAOD::CaloCluster *cluster = clusters.at(j);
////                    float etaClu = cluster->eta();
////                    float phiClu = cluster->phi();
////                    float deltaEtaClu = par2_eta - etaClu;
////                    float deltaPhiClu = xAOD::P4Helpers::deltaPhi(par2_phi, phiClu);
////                    float drClu = sqrt(deltaEtaClu * deltaEtaClu + deltaPhiClu * deltaPhiClu);
////                    bool hasToBeRemoved = (drClu <= m_coreCone) && (fabs(cluster->eta()) <= 7.0) && (cluster->pt() > 0.001);
////                    if (hasToBeRemoved) {
////                        // Uncalibrated energy of the topocluster.
////                        float etUncal = cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et();
////                        // Correction to the energy.
////                        float st = 1.0 / TMath::CosH(cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
////                        float tilegap3_et = cluster->eSample(CaloSampling::TileGap3) * st;
////                        etUncal = etUncal - tilegap3_et;
////                        // Only positive energy topoclusters are removed.
////                        // The others can be removed from the vector (they won't be used).
////                        if (etUncal > 0.0) {
////                            correction = correction + etUncal;
////                        }
////                    } else {
////                        // Only keeps the clusters that were not already subtracted.
////                        clustersTemp.push_back(cluster);
////                    }
////                }
////
////                clusters = clustersTemp;
////
////            }
//
//            if (topoetconeModel != -1)  {
//
//                float fraction = 0.0;
//
//                ATH_MSG_VERBOSE("Particle " << i << " delta eta: " << deta);
//                ATH_MSG_VERBOSE("Particle " << i << " delta phi: " << dphi);
//                ATH_MSG_VERBOSE("Particle " << i << " delta R: " << dr);
//
//                if (topoetconeModel == 0) {
//                    if (coneSize <= m_coreCone) {
//                        fraction = 0.0;
//                    } else {
//                        if (dr <= coneSize) {
//                            fraction = 1.0;
//                        }
//                    }
//                }
//
//                if (topoetconeModel > 0) {
//                    if (coneSize <= m_coreCone) {
//                        fraction = 0.0;
//                    } else {
//                        if (topoetconeModel == 1) {
//                            fraction = 0.0;
//                            if ((dr > 2.0 * m_coreCone) && (dr <= (coneSize - m_coreCone))) {
//                                fraction = 1.0;
//                            }
//                        }
//
//                        if (topoetconeModel == 2) {
//                            float drA = fmin(2.0 * m_coreCone, coneSize - m_coreCone);
//                            fraction = fmin(dr / m_coreCone, 1.0);
//                            if (dr > drA) {
//                                float drB = fmax(2 * m_coreCone, coneSize - m_coreCone);
//                                fraction = fmin(drA / m_coreCone, 1.0);
//                                if (dr > drB) {
//                                    float drC = coneSize + m_coreCone;
//                                    fraction = fraction * (drC - dr) / (drC - drB);
//                                }
//                            }
//                        }
//                    }
//                }
//
//                if (fraction < 0.0) {
//                    fraction = 0.0;
//                }
//
//                if (fraction > 1.0) {
//                    fraction = 1.0;
//                }
//
//                float coreToBeRemoved = 0.0;
//                auto acc = xAOD::getIsolationCorrectionAccessor(xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
//                if (!acc->isAvailable(par)) {
//                    bool foundCore = false;
//                    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&par);
//                    if (mu) {
//                        mu->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
//                        foundCore = true;
//                    } else {
//                        const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&par);
//                        if (el) {
//                            el->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
//                            foundCore = true;
//                        } else {
//                            const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(&par);
//                            if (ph) {
//                                ph->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
//                                foundCore = true;
//                            }
//                        }
//                    }
//                    if (!foundCore) {
//                        ATH_MSG_WARNING("Could not retrieve topocore. No correction could be calculated.");
//                    }
//                } else {
//                    coreToBeRemoved = (*acc)(*par2);
//                }
//                correction += coreToBeRemoved * fraction;
//
//            }
//
//        }
//        ATH_MSG_DEBUG("Topoetcone isolation correction = " << correction);
        return CP::CorrectionCode::Ok;
    }

    void IsolationCloseByCorrectionTool::getExtrapEtaPhi(const xAOD::IParticle* par, float& eta, float& phi) const {
        if (par->type() == xAOD::Type::ObjectType::Muon) {
            const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(par);
            eta = 0.0;
            phi = 0.0;
            const xAOD::CaloCluster* cluster = mu->cluster();
            int nSample = 0;
            if (cluster) {
                float etaT = 0.0, phiT = 0.0;
                for (unsigned int i = 0; i < CaloSampling::Unknown; i++) {
                    auto s = static_cast<CaloSampling::CaloSample>(i);
                    if (cluster->hasSampling(s)) {
                        ATH_MSG_VERBOSE("Sampling: " << i << "eta-phi (" << cluster->etaSample(s) << ", " << cluster->phiSample(s) << ")");
                        etaT += cluster->etaSample(s);
                        phiT += cluster->phiSample(s);
                        ++nSample;
                    }
                }
                if (nSample > 0) {
                    eta = etaT / nSample;
                    phi = phiT / nSample;
                } else {
                    eta = mu->eta();
                    phi = mu->phi();
                }
            }

        } else if (par->type() == xAOD::Type::ObjectType::Electron || par->type() == xAOD::Type::ObjectType::Photon) {
            const xAOD::Egamma* eg = dynamic_cast<const xAOD::Egamma*>(par);
            const xAOD::CaloCluster* cluster = eg->caloCluster();
            if (cluster) {
                phi = cluster->phi();
                eta = cluster->eta();
            }
        }
    }

    const Root::TAccept& IsolationCloseByCorrectionTool::acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {

        if (m_selectorTool.empty()) {
            ATH_MSG_ERROR("Please set the IsolationSelectionTool property with a valid IsolationSelectionTool");
            m_accept.clear();
            return m_accept;
        }

        std::vector<xAOD::Iso::IsolationType> isoTypes;
        const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&x);
        if (mu) {
            isoTypes = m_muon_isoTypes;
        } else {
            const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&x);
            if (el) {
                isoTypes = m_electron_isoTypes;
            } else {
                const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(&x);
                if (ph) {
                    isoTypes = m_photon_isoTypes;
                } else {
                    ATH_MSG_WARNING("Could not cast particle for acceptCorrected. Will return false.");
                    m_accept.setCutResult("castCut", false);
                    return m_accept;
                }
            }
        }

        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        }

        m_accept.clear();
        strObj strPar;
        strPar.isolationValues.resize(xAOD::Iso::numIsolationTypes);
        strPar.pt = x.pt();
        strPar.eta = x.eta();
        strPar.type = x.type();
        std::vector<float> corrections;
        if (getCloseByCorrection(corrections, x, isoTypes, closePar, topoetconeModel) == CP::CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not calculate the corrections. acceptCorrected(x) is done without the corrections.");
            m_accept = m_selectorTool->accept(x);
            return m_accept;
        }

        for (unsigned int i = 0; i < isoTypes.size(); i++) {
            SG::AuxElement::Accessor<float> *acc = xAOD::getIsolationAccessor(isoTypes.at(i));
            float old = (*acc)(x);
            ATH_MSG_DEBUG("Correcting " << xAOD::Iso::toString(isoTypes.at(i)) << " from " << old << " to " << old - corrections.at(i));
            strPar.isolationValues[isoTypes.at(i)] = old - corrections.at(i);
        }

        m_accept = m_selectorTool->accept(strPar);
        return m_accept;
    }

    const xAOD::Vertex* IsolationCloseByCorrectionTool::retrieveIDBestPrimaryVertex() const {
        const xAOD::VertexContainer* Verticies = nullptr;
        if (evtStore()->contains < xAOD::VertexContainer > (m_VertexContainerName)) {
            if (evtStore()->retrieve(Verticies, m_VertexContainerName).isFailure()) {
                ATH_MSG_ERROR("IsoCorrection::RetriveBestVtx(): Unable to retrieve VertexContainer " << m_VertexContainerName);
                return nullptr;
            } else if (Verticies->size() > 0) {
                for (const auto& V : *Verticies) {
                    if (V->vertexType() == xAOD::VxType::VertexType::PriVtx) return V;
                }
            }
        }
        ATH_MSG_WARNING("The vertex collection " << m_VertexContainerName << " does not  contain any vertex");
        return nullptr;
    }

    double IsolationCloseByCorrectionTool::ConeSize(const xAOD::IParticle* P, xAOD::Iso::IsolationType Cone) const {
        double ConeDR = xAOD::Iso::coneSize(Cone);
        if (IsVarTrackIso(Cone)) {
            const xAOD::IParticle* Reference = TrackIsoRefPart(P);
            double MiniIso = m_ptvarconeRadius / UnCalibPt(Reference);
            if (MiniIso < ConeDR) return MiniIso;
        }
        return ConeDR;

    }
    double IsolationCloseByCorrectionTool::UnCalibPt(const xAOD::IParticle* P) const {
        if (!P) {
            ATH_MSG_WARNING("No partcile given. Return stupidly big number. ");
            return 1.e25;
        }
        const xAOD::IParticle* OR = xAOD::getOriginalObject(*P);
        if (!OR) {
            ATH_MSG_DEBUG("No reference from the shallow copy container could be found");
            return P->pt();
        }
        return OR->pt();

    }

    const xAOD::IParticle* IsolationCloseByCorrectionTool::TrackIsoRefPart(const xAOD::IParticle* P) const {
        if (!P) {
            ATH_MSG_ERROR("Nullptr given");
            return nullptr;
        }
        if (P->type() == xAOD::Type::ObjectType::Muon) return getTrackParticle(P);
        return P;
    }
    const xAOD::TrackParticle* IsolationCloseByCorrectionTool::getTrackParticle(const xAOD::IParticle* particle) const {
        if (particle->type() == xAOD::Type::ObjectType::TrackParticle) {
            return dynamic_cast<const xAOD::TrackParticle*>(particle);
        } else if (particle->type() == xAOD::Type::ObjectType::Electron) {
            const xAOD::Electron* El = dynamic_cast<const xAOD::Electron*>(particle);
            const xAOD::TrackParticle* T = xAOD::EgammaHelpers::getOriginalTrackParticle(El);
            if (T == nullptr) {
                ATH_MSG_WARNING("Could not find the Original InDet track of the electron. Use the GSF track instead");
                return El->trackParticle();
            } else return T;
        } else if (particle->type() == xAOD::Type::ObjectType::Muon) {
            const xAOD::Muon* Mu = dynamic_cast<const xAOD::Muon*>(particle);
            const xAOD::TrackParticle* T = Mu->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
            if (T == nullptr) {
                ATH_MSG_DEBUG("The muon has no InDet track. Return the next primary track");
                return Mu->primaryTrackParticle();
            } else return T;
        }
        ATH_MSG_WARNING("Could not find an associated track to the Particle");
        return nullptr;
    }
    bool IsolationCloseByCorrectionTool::IsSame(const xAOD::IParticle* P, const xAOD::IParticle* P1) const {
        if (!P || !P1) {
            ATH_MSG_WARNING("Null ptr were given");
            return true;
        }
        return (P == P1);
    }

    double IsolationCloseByCorrectionTool::DeltaR2(const xAOD::IParticle* P, const xAOD::IParticle* P1, bool AvgCalo) const {
        if (!P || !P1) {
            ATH_MSG_WARNING("IsolationCloseByCorrectionTool::DeltaR2(): One of the given Particles points to nullptr return 1.e4");
            return 1.e4;
        }
        if (IsSame(P, P1)) return 0.;
        if (AvgCalo || (P->type() != P1->type() && (P->type() == xAOD::Type::ObjectType::CaloCluster || P1->type() == xAOD::Type::ObjectType::CaloCluster))) {
            float phi1(0), eta1(0), eta2(0), phi2(0);
            getExtrapEtaPhi(P, eta1, phi1);
            getExtrapEtaPhi(P1, eta2, phi2);
            double dPhi = xAOD::P4Helpers::deltaPhi(phi1, phi2);
            double dEta = eta1 - eta2;
            return dEta * dEta + dPhi * dPhi;

        }
        double dPhi = xAOD::P4Helpers::deltaPhi(P, P1);
        double dEta = P->eta() - P1->eta();
        return dEta * dEta + dPhi * dPhi;
    }
    bool IsolationCloseByCorrectionTool::Overlap(const xAOD::IParticle* P, const xAOD::IParticle* P1, double dR) const {
        return (!IsSame(P, P1) && DeltaR2(P, P1) < (dR * dR));
    }

    bool IsolationCloseByCorrectionTool::IsFixedTrackIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::ptcone == xAOD::Iso::isolationFlavour(Iso);
    }
    bool IsolationCloseByCorrectionTool::IsVarTrackIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::ptvarcone == xAOD::Iso::isolationFlavour(Iso);
    }
    bool IsolationCloseByCorrectionTool::IsTrackIso(xAOD::Iso::IsolationType Iso) const {
        return IsVarTrackIso(Iso) || IsFixedTrackIso(Iso);
    }
    bool IsolationCloseByCorrectionTool::IsTopoEtIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::topoetcone == xAOD::Iso::isolationFlavour(Iso);
    }
    float IsolationCloseByCorrectionTool::GetOriginalIsolation(const xAOD::IParticle* P, xAOD::Iso::IsolationType T) const {
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(T);
        float IsoValue = 0;
        if (Itr == m_isohelpers.end() || Itr->second->GetOrignalIsolation(P, IsoValue) == CorrectionCode::Error) {
            ATH_MSG_ERROR("Failed to retrive the original  isolation  cone ");
            IsoValue = nanf("nan");
        }
        return IsoValue;
    }
    float IsolationCloseByCorrectionTool::ClusterEtMinusTile(const xAOD::CaloCluster* C) const {
        float Et = 0.;
        if (C != nullptr) {
            try {
                Et = C->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et();
                Et = Et - C->eSample(xAOD::CaloCluster::CaloSample::TileGap3) / TMath::CosH(C->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
            } catch (...) {
                ATH_MSG_DEBUG("Could not retrieve the uncalibrated Cluster state");
                Et = C->p4().Et();
            }
        } else ATH_MSG_DEBUG("No CaloCluster was given. Return 0.");
        return fmax(Et, 0);
    }
    template<typename T> bool IsolationCloseByCorrectionTool::IsElementInList(const std::vector<T> &List, const T& Element) const {
        for (auto&Test : List) {
            if (Test == Element) return true;
        }
        return false;
    }

    //######################################################################################################
    //                                      IsoVariableHelper
    //######################################################################################################
    IsoVariableHelper::IsoVariableHelper(xAOD::Iso::IsolationType type, const std::string& BackupPreFix) :
                m_IsoType(type),
                m_BackupIso(!BackupPreFix.empty()),
                m_dec_IsoIsBackup("IsBackup_" + std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_acc_IsoIsBackup("IsBackup_" + std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_acc_iso_variable(xAOD::Iso::toString(type)),
                m_dec_iso_variable(xAOD::Iso::toString(type)),
                m_acc_iso_backup(std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_dec_iso_backup(std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix) {
    }

    CorrectionCode IsoVariableHelper::GetOrignalIsolation(const xAOD::IParticle* P, float& Value) const {
        if (!P) {
            Error("IsoVariableHelper::GetOrignalIsolation()", "No particle given");
            return CorrectionCode::Error;
        }
        if (!m_BackupIso) {
            const xAOD::IParticle* OR = xAOD::getOriginalObject(*P);
            if (OR && GetIsolation(OR, Value) == CP::CorrectionCode::Error) return CorrectionCode::Error;
            else if (!OR) {
                Warning("IsoVariableHelper::GetOrignalIsolation()", "No original object was found");
                return GetIsolation(P, Value);
            }
        } else {
            if (!m_acc_IsoIsBackup.isAvailable(*P) || !m_acc_IsoIsBackup(*P)) {
                Warning("IsoVariableHelper::GetOrignalIsolation()", "No isolation value was backuped thus far. Did you call the BackupIsolation before for %s?", xAOD::Iso::toString(m_IsoType));
                return CorrectionCode::Error;
            } else {
                Value = m_acc_iso_backup(*P);
            }
        }
        return CorrectionCode::Ok;

    }
    CorrectionCode IsoVariableHelper::GetIsolation(const xAOD::IParticle* P, float& Value) const {
        if (!P || !m_acc_iso_variable.isAvailable(*P)) {
            Error("IsoVariableHelper::GetIsolation()", "Failed to retrieve isolation %s", xAOD::Iso::toString(m_IsoType));
            return CorrectionCode::Error;
        }
        Value = m_acc_iso_variable(*P);
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::BackupIsolation(const xAOD::IParticle* P) const {
        if (!P) {
            Error("IsoVariableHelper::GetIsolation()", "No particle  given");
            return CorrectionCode::Error;
        }
        if (m_BackupIso && (!m_acc_IsoIsBackup.isAvailable(*P) || !m_acc_IsoIsBackup(*P))) {
            float Isovalue = 0;
            if (GetIsolation(P, Isovalue) == CorrectionCode::Error) {
                return CorrectionCode::Error;
            }
            m_dec_IsoIsBackup(*P) = true;
            m_dec_iso_backup(*P) = Isovalue;
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::SetIsolation(xAOD::IParticle* P, float Value) const {
        if (!P) {
            Error("IsoVariableHelper::SetIsolation()", "No particle given");
            return CorrectionCode::Error;
        }
        if (std::isnan(Value) || std::isinf(Value)) {
            Error("IsoVariableHelper::SetIsolation()", "The value is not a number");
            return CorrectionCode::Error;
        }
        m_dec_iso_variable(*P) = Value;
        return CorrectionCode::Ok;
    }
    xAOD::Iso::IsolationType IsoVariableHelper::IsoType() const {
        return m_IsoType;
    }

}
