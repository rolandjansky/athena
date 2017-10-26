/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationCloseByCorrectionTool.h>

#include <xAODPrimitives/IsolationHelpers.h>

#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <xAODPrimitives/tools/getIsolationCorrectionAccessor.h>

#include <IsolationSelection/IsolationSelectionTool.h>
#include <InDetTrackSelectionTool/IInDetTrackSelectionTool.h>

#include <xAODBase/ObjectType.h>
#include <xAODBase/IParticleHelpers.h>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"


#include <FourMomUtils/xAODP4Helpers.h>
#include <PATInterfaces/CorrectionCode.h>

//Tools includes:
#include <cmath>

namespace CP {

    IsolationCloseByCorrectionTool::IsolationCloseByCorrectionTool(const std::string& toolName) :
                asg::AsgTool(toolName),
                m_selectorTool(""),
                m_coreCone(0.1),
                m_ptvarconeRadius(1.e4),
                m_maxTopoPolution(1.1),
                m_ConeSizeVariation(1.2),
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
                m_ToCorrect_name(),
                m_acc_quality(),
                m_acc_passOR(),
                m_acc_ToCorrect(),
                m_backup_prefix(),
                m_trkselTool(),
                m_isohelpers() {
        //IMPORTANT USER PROPERTIES
        declareProperty("IsolationSelectionTool", m_selectorTool, "Please give me your configured IsolationSelectionTool!");

        //OPTIONAL PROPERTIES
        m_trkselTool.declarePropertyFor(this, "TrackSelectionTool"); // Makes the track selection tool a settable property of this tool
        declareProperty("SelectionDecorator", m_quality_name, "Name of the char auxdata defining whether the particle shall be considered for iso correction");
        declareProperty("PassoverlapDecorator", m_passOR_name, "Does the particle also need to pass the overlap removal?");
        declareProperty("IsolationSelectionDecorator", m_isoSelection_name, "Name of the final isolation decorator.");
        declareProperty("CorrectIsolationOf", m_ToCorrect_name, "The isolation of particles having this switch set to true are corrected regardless of whether it passes the quality or not ");
        declareProperty("BackupPrefix", m_backup_prefix, "Prefix in front of the isolation variables, if the original cone values need  to  be backuped");

        //EXPERT PROPERTIES
        declareProperty("TrackParticleLocation", m_indetTrackParticleLocation, "This is the location of the Inner Detector track particle (for the container).");
        declareProperty("VertexContainer", m_VertexContainerName, "Name of the primary vertex container");
        declareProperty("CaloClusterContainer", m_CaloClusterContainerName, "Name of the primary calo cluster container");

        declareProperty("CoreCone", m_coreCone, "This is the size of the core cone for the topoetcone variables.");
        declareProperty("PtvarconeRadius", m_ptvarconeRadius, "This is the kT parameter for the ptvarcone variables.");
        declareProperty("MaxClusterFrac", m_maxTopoPolution, "Maximum energy fraction a single cluster can make up to be considered as contributed to the isolation");
        declareProperty("ExtrapolationConeSize", m_ConeSizeVariation, "Constant factor to be multiplied on top of the topo-etcone size if the reference particle is not a calorimeter particle in order to account for extrapolation effects");

    }

    StatusCode IsolationCloseByCorrectionTool::initialize() {

        //set default properties of track selection tool, if the user hasn't configured it
        if (!m_trkselTool.isUserConfigured()) {
            m_trkselTool.setTypeAndName("InDet::InDetTrackSelectionTool/TrackParticleSelectionTool");
            ATH_MSG_INFO("No TrackSelectionTool provided, so I will create and configure my own, called: " << m_trkselTool.name());
            ATH_CHECK(m_trkselTool.setProperty("maxZ0SinTheta", 3.));
            ATH_CHECK(m_trkselTool.setProperty("minPt", 1000.));
            ATH_CHECK(m_trkselTool.setProperty("CutLevel", "Loose"));
        }

        ATH_CHECK(m_trkselTool.retrieve());

        ATH_CHECK(m_selectorTool.retrieve());
        isoTypesFromWP(m_selectorTool->getElectronWPs(), m_electron_isoTypes);
        isoTypesFromWP(m_selectorTool->getMuonWPs(), m_muon_isoTypes);
        isoTypesFromWP(m_selectorTool->getPhotonWPs(), m_photon_isoTypes);
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
        if (!m_ToCorrect_name.empty()) {
            m_acc_ToCorrect = SelectionAccessor(new CharAccessor(m_ToCorrect_name));
            m_ToCorrect_name.clear();
        }
        m_isInitialised = true;
        return StatusCode::SUCCESS;
    }
    void IsolationCloseByCorrectionTool::isoTypesFromWP(const std::vector<IsolationWP*> &WPs, std::vector<xAOD::Iso::IsolationType> & types) {
        types.clear();
        for (const auto& W : WPs) {
            for (auto& C : W->conditions()) {
                if (!isElementInList(types, C->type())) types.push_back(C->type());
                if (m_isohelpers.find(C->type()) == m_isohelpers.end()) {
                    m_isohelpers.insert(IsoHelperPair(C->type(), IsoHelperPtr(new IsoVariableHelper(C->type(), m_backup_prefix))));
                }
            }
        }
    }

    CorrectionCode IsolationCloseByCorrectionTool::getCloseByIsoCorrection(xAOD::ElectronContainer* Electrons, xAOD::MuonContainer* Muons, xAOD::PhotonContainer* Photons, int topoetconeModel) const {
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
        getTrackCandidates(Electrons, Vtx, Tracks);
        getTrackCandidates(Muons, Vtx, Tracks);
        getTrackCandidates(Photons, Vtx, Tracks);
        //Now grep every cluster with dR< m_CoreCone to the object
        ClusterCollection Clusters;
        if (topoetconeModel == TopoConeCorrectionModel::DirectCaloClusters) {
            getClusterCandidates(Electrons, Clusters);
            getClusterCandidates(Muons, Clusters);
            getClusterCandidates(Photons, Clusters);
        }
        if (performCloseByCorrection(Electrons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        else if (performCloseByCorrection(Muons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        else if (performCloseByCorrection(Photons, Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;
        //get the other way
        if (topoetconeModel != TopoConeCorrectionModel::DirectCaloClusters || Clusters.empty()) {

        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsolationCloseByCorrectionTool::performCloseByCorrection(xAOD::IParticleContainer* Container, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const {
        if (!m_isInitialised) {
            ATH_MSG_ERROR("The IsolationCloseByCorrectionTool was not initialised!!!");
            return CorrectionCode::Error;
        }
        if (!Container) {
            ATH_MSG_DEBUG("No particle container was given");
            return CorrectionCode::Ok;
        }
        for (auto Particle : *Container) {
            if (!considerForCorrection(Particle)) continue;
            if (!getIsolationTypes(Particle) || subtractCloseByContribution(Particle, *getIsolationTypes(Particle), AssocTracks, AssocClusters) == CorrectionCode::Error) {
                ATH_MSG_ERROR("Failed to correct the isolation of particle with pt: " << Particle->pt() / 1.e3 << " GeV" << " eta: " << Particle->eta() << " phi: " << Particle->phi());
                return CorrectionCode::Error;
            }
            if (m_dec_isoselection) m_dec_isoselection->operator()(*Particle) = m_selectorTool->accept(*Particle);
        }
        return CorrectionCode::Ok;
    }
    const IsoVector* IsolationCloseByCorrectionTool::getIsolationTypes(const xAOD::IParticle* particle) const {
        if (!particle) return nullptr;
        if (particle->type() == xAOD::Type::ObjectType::Electron) return &m_electron_isoTypes;
        else if (particle->type() == xAOD::Type::ObjectType::Muon) return &m_muon_isoTypes;
        else if (particle->type() == xAOD::Type::ObjectType::Photon) return &m_photon_isoTypes;
        return nullptr;
    }
    CorrectionCode IsolationCloseByCorrectionTool::performCloseByCaloCorrection(xAOD::IParticleContainer* Cont1, xAOD::IParticleContainer* Cont2) const {
        if (!Cont1 || !Cont2) {
            ATH_MSG_DEBUG("One of the containers is empty");
            return CorrectionCode::Ok;
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsolationCloseByCorrectionTool::subtractCloseByContribution(xAOD::IParticle& x, const xAOD::IParticleContainer& closebyPar, int topoetconeModel) const {
        TrackCollection Tracks;
        ClusterCollection Clusters;
        const xAOD::Vertex* Vtx = retrieveIDBestPrimaryVertex();
        if (!Vtx) {
            ATH_MSG_ERROR("No vertex was found");
            return CorrectionCode::Error;
        }
        getTrackCandidates(&closebyPar, Vtx, Tracks);
        if (topoetconeModel == TopoConeCorrectionModel::DirectCaloClusters) getClusterCandidates(&closebyPar, Clusters);
        if (!getIsolationTypes(&x) || subtractCloseByContribution(&x, *getIsolationTypes(&x), Tracks, Clusters) == CorrectionCode::Error) return CorrectionCode::Error;

        return CorrectionCode::Ok;
    }

    CorrectionCode IsolationCloseByCorrectionTool::subtractCloseByContribution(xAOD::IParticle* par, const IsoVector& types, const TrackCollection& AssocTracks, const ClusterCollection& AssocClusters) const {
        for (const auto t : types) {
            float Cone = 0.;
            if (isTrackIso(t)) {
                if (getCloseByCorrectionTrackIso(Cone, par, t, AssocTracks) == CorrectionCode::Error || m_isohelpers.at(t)->setIsolation(par, Cone) == CorrectionCode::Error) {
                    ATH_MSG_ERROR("Failed to apply track correction");
                    return CorrectionCode::Error;
                }
            }
            if (isTopoEtIso(t)) {
                if (getCloseByCorrectionTopoIso(Cone, par, t, AssocClusters) == CorrectionCode::Error || m_isohelpers.at(t)->setIsolation(par, Cone) == CorrectionCode::Error) {
                    ATH_MSG_ERROR("Failed to apply track correction");
                    return CorrectionCode::Error;
                }
            }
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const xAOD::IParticleContainer& closePar, int topoetconeModel) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        }
        corrections = std::vector<float>(types.size(), 0);
        TrackCollection Tracks;
        getTrackCandidates(&closePar, retrieveIDBestPrimaryVertex(), Tracks);
        ClusterCollection Clusters;
        if (topoetconeModel == TopoConeCorrectionModel::DirectCaloClusters) getClusterCandidates(&closePar, Clusters);
        std::vector<float>::iterator Cone = corrections.begin();
        for (auto& t : types) {
            IsoHelperMap::const_iterator Itr = m_isohelpers.find(t);
            if (Itr == m_isohelpers.end()) {
                m_isohelpers.insert(IsoHelperPair(t, IsoHelperPtr(new IsoVariableHelper(t, m_backup_prefix))));
                Itr = m_isohelpers.find(t);
            }
            if (isTrackIso(t) && getCloseByCorrectionTrackIso((*Cone), &par, t, Tracks) == CorrectionCode::Error) {
                ATH_MSG_ERROR("Failed to apply track correction");
                return CorrectionCode::Error;
            }
            if (isTopoEtIso(t)) {
                if (!Clusters.empty()) {
                    if (getCloseByCorrectionTopoIso((*Cone), &par, t, Clusters) == CorrectionCode::Error) {
                        ATH_MSG_ERROR("Failed to apply calo correction");
                        return CorrectionCode::Error;
                    }
                } else topoetconeModel = TopoConeCorrectionModel::ParticleCaloCorrection;
                if (topoetconeModel != TopoConeCorrectionModel::DirectCaloClusters && getCloseByCaloCorrection((*Cone), &par, &closePar, t, topoetconeModel) == CorrectionCode::Error) return CorrectionCode::Error;
            }
            ++Cone;
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {
        xAOD::IParticleContainer Container(SG::VIEW_ELEMENTS);
        for (auto&P : closePar)
            Container.push_back(const_cast<xAOD::IParticle*>(P));
        return getCloseByCorrection(corrections, par, types, Container, topoetconeModel);

    }
    CorrectionCode IsolationCloseByCorrectionTool::getCloseByCaloCorrection(float& correction, const xAOD::IParticle* par, const xAOD::IParticleContainer* CloseByPars, IsoType type, int Model) const {

        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        } else if (!isTopoEtIso(type)) {
            ATH_MSG_ERROR("Invalid isolation type");
            return CorrectionCode::Error;
        }
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(type);
        if (Itr == m_isohelpers.end() || Itr->second->backupIsolation(par) == CorrectionCode::Error || Itr->second->getOrignalIsolation(par, correction) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable " << xAOD::Iso::toString(type));
            return CorrectionCode::Error;
        } else if (!CloseByPars) {
            ATH_MSG_DEBUG("No container given for getCloseByCaloCorrection");
            return CorrectionCode::Ok;
        }

        //else if (correction <= 0.0) return CorrectionCode::Ok;
        float cone = coneSize(par, type);
        for (const auto closeBy : *CloseByPars) {
            if (passSelectionQuality(closeBy)) correction -= caloCorrectionFromDecorator(par, closeBy, cone, Model);
        }
        return CorrectionCode::Ok;
    }

    TrackCollection IsolationCloseByCorrectionTool::getAssociatedTracks(const xAOD::IParticle* P) const {
        if (P->type() == xAOD::Type::Muon) {
            return TrackCollection { getTrackParticle(P) };
        } else if (P->type() == xAOD::Type::TrackParticle) {
            return TrackCollection { getTrackParticle(P) };
        } else if (P->type() == xAOD::Type::Electron) {
            const xAOD::Electron* E = dynamic_cast<const xAOD::Electron*>(P);
            return xAOD::EgammaHelpers::getTrackParticles(E);
        } else if (P->type() == xAOD::Type::Photon) {
            const xAOD::Photon* Ph = dynamic_cast<const xAOD::Photon*>(P);
            return xAOD::EgammaHelpers::getTrackParticles(Ph);
        }
        return TrackCollection();
    }
    bool IsolationCloseByCorrectionTool::isEgamma(const xAOD::IParticle* P) const {
        return P && (P->type() == xAOD::Type::ObjectType::Electron || P->type() == xAOD::Type::ObjectType::Photon);
    }

    ClusterCollection IsolationCloseByCorrectionTool::getAssociatedClusters(const xAOD::IParticle* P) const {
        ClusterCollection clusters;
        const xAOD::CaloClusterContainer* topoClusters = nullptr;
        if (!evtStore()->contains < xAOD::CaloClusterContainer > (m_CaloClusterContainerName) || !evtStore()->retrieve(topoClusters, m_CaloClusterContainerName).isSuccess()) {
            ATH_MSG_DEBUG("The cluster container " << m_CaloClusterContainerName << " is not in the event");
            return clusters;
        }
        const xAOD::IParticle* Ref = topoEtIsoRefPart(P);
//        if (isEgamma(P)) {
//            const xAOD::Egamma* E = dynamic_cast<const xAOD::Egamma*>(P);
//            for (unsigned int i = 0; i < E->nCaloClusters(); ++i) {
//                const xAOD::CaloCluster* cluster = E->caloCluster(i);
//                if (!cluster || fabs(cluster->eta()) > 7.0 || cluster->pt() <= 1.e-3) continue;
//                clusters.push_back(cluster);
//            }
//        }
        for (const auto& cluster : *topoClusters) {
            if (!cluster || fabs(cluster->eta()) > 7.0 || cluster->pt() <= 1.e-3) continue;
            //Consider also the cluster of Egamma if they are in the container
            if (isSame(Ref, cluster) || overlap(cluster, Ref, m_coreCone)){
                 ATH_MSG_DEBUG("Add cluster with pt: "<<cluster->pt()/1.e3<<" GeV, eta: "<<cluster->eta()<<" phi: "<<cluster->phi()<< " associated with " << particleName(P) << " having pt: " << P->pt() / 1.e3 << " eta: " << P->eta() << " phi: " << P->phi());
                 clusters.push_back(cluster);
            }
        }
        return clusters;
    }
    void IsolationCloseByCorrectionTool::getTrackCandidates(const xAOD::IParticleContainer* Particles, const xAOD::Vertex* Vtx, TrackCollection &Tracks) const {
        if (!Particles) {
            ATH_MSG_DEBUG("No particles were given to retrieve track candidates");
            return;
        }
        for (const auto P : *Particles) {
            if (!passSelectionQuality(P)) continue;
            TrackCollection AssocCloseTrks = getAssociatedTracks(P);
            for (const auto& T : AssocCloseTrks) {
                if (!T || !m_trkselTool->accept(*T, Vtx)) continue;
                ATH_MSG_VERBOSE("Found ID-track with pt:" << T->pt() / 1.e3 << " GeV, eta: " << T->eta() << ", phi: " << T->phi() << " associated with " << particleName(P) << " having pt: " << P->pt() / 1.e3 << " eta: " << P->eta() << " phi: " << P->phi());
                Tracks.insert(T);
            }
        }
    }
    void IsolationCloseByCorrectionTool::getClusterCandidates(const xAOD::IParticleContainer* Particles, ClusterCollection& Clusters) const {
        if (!Particles) {
            ATH_MSG_DEBUG("No particles were given to retrieve cluster candidates");
            return;
        }
        for (const auto P : *Particles) {
            if (!passSelectionQuality(P)) continue;
            ClusterCollection AssocClusters = getAssociatedClusters(P);
            Clusters.reserve(AssocClusters.size() + Clusters.size());
            for (auto& C : AssocClusters)
                if (!isElementInList(Clusters, C)) Clusters.push_back(C);
        }
        Clusters.shrink_to_fit();
        //Sort the clusters in ascending order
        std::sort(Clusters.begin(), Clusters.end(), [] (const xAOD::CaloCluster* C , const xAOD::CaloCluster* C1) {return C->pt() > C1->pt();});

    }
    bool IsolationCloseByCorrectionTool::considerForCorrection(const xAOD::IParticle* P) const {
        if (passSelectionQuality(P)) return true;
        if (m_acc_ToCorrect && (!m_acc_ToCorrect->isAvailable(*P) || !m_acc_ToCorrect->operator()(*P))) return false;
        return true;
    }

    bool IsolationCloseByCorrectionTool::passSelectionQuality(const xAOD::IParticle* P) const {
        if (P == nullptr) return false;
        if (m_acc_quality && (!m_acc_quality->isAvailable(*P) || !m_acc_quality->operator()(*P))) return false;
        if (m_acc_passOR && (!m_acc_passOR->isAvailable(*P) || !m_acc_passOR->operator()(*P))) return false;
        return true;
    }
    CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTrackIso(float& correction, const xAOD::IParticle* par, IsoType type, const TrackCollection& tracks) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        } else if (!isTrackIso(type)) {
            ATH_MSG_ERROR("Invalid isolation type " << xAOD::Iso::toString(type));
            return CorrectionCode::Error;
        }
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(type);
    
        if (Itr == m_isohelpers.end() || Itr->second->backupIsolation(par) == CorrectionCode::Error || Itr->second->getOrignalIsolation(par, correction) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable " << xAOD::Iso::toString(type));
            return CorrectionCode::Error;
        } else if (tracks.empty()) return CorrectionCode::Ok;

        double MaxDR = coneSize(par, type);
        TrackCollection ToExclude = getAssociatedTracks(par);

        const xAOD::IParticle* Ref = trackIsoRefPart(par);
        ATH_MSG_DEBUG(xAOD::Iso::toString(type) << " of " << particleName(par) << " with pt: " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << ", phi: " << par->phi() << " before correction: " << correction / 1.e3 << " GeV");

        for (auto& T : tracks) {
            if (overlap(Ref, T, MaxDR) && !isElementInList(ToExclude, T)) {
                ATH_MSG_VERBOSE("Subtract track with " << T->pt() / 1.e3 << " GeV, eta: " << T->eta() << ", phi: " << T->phi() << " with dR: " << sqrt(deltaR2(Ref, T)) << " from the isolation cone " << xAOD::Iso::toString(type) << " " << (correction / 1.e3) << " GeV.");
                correction -= T->pt();
            }
        }
        ATH_MSG_DEBUG(xAOD::Iso::toString(type) << " of " << particleName(par) << " with pt: " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << ", phi: " << par->phi() << " after correction: " << correction / 1.e3 << " GeV");

        return CorrectionCode::Ok;
    }
    CorrectionCode IsolationCloseByCorrectionTool::getCloseByCorrectionTopoIso(float& correction, const xAOD::IParticle* par, IsoType type, const ClusterCollection& clusters) const {
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        } else if (!isTopoEtIso(type)) {
            ATH_MSG_ERROR("Invalid isolation type");
            return CorrectionCode::Error;
        }
        IsoHelperMap::const_iterator Itr = m_isohelpers.find(type);
        if (Itr == m_isohelpers.end() || Itr->second->backupIsolation(par) == CorrectionCode::Error || Itr->second->getOrignalIsolation(par, correction) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not retrieve the isolation variable.");
            return CorrectionCode::Error;
        } else if (clusters.empty()) return CorrectionCode::Ok;
        //Disable the correction of already isolated objects
        else if (correction <= 0.0) return CorrectionCode::Ok;

        ATH_MSG_DEBUG(xAOD::Iso::toString(type) << " of " << particleName(par) << " with pt " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << ", phi: " << par->phi() << " before correction: " << correction / 1.e3 << " GeV");
        const xAOD::IParticle* Ref = topoEtIsoRefPart(par);
        if (!Ref) {
            ATH_MSG_ERROR("Could not find a reference particle for " << particleName(par) << " with pt " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << ", phi: " << par->phi());
            return CorrectionCode::Error;
        }
        double MaxDR = coneSize(par, type) * (Ref->type() == xAOD::Type::ObjectType::CaloCluster ? 1. : m_ConeSizeVariation);
        for (auto& cluster : clusters) {
            ATH_MSG_DEBUG("Cluster with pt: " << cluster->pt() / 1.e3 << " GeV, eta: " << cluster->eta() << ", phi: " << cluster->phi() << " dR: " << sqrt(deltaR2(cluster, par)) << " (" << MaxDR << ")");
            bool Subtract = false;            
            if (overlap(cluster, Ref, m_coreCone)){
                ATH_MSG_DEBUG("The cluster belongs to the core cone");
            } else if (Ref->type() == xAOD::Type::ObjectType::CaloCluster) {
                Subtract = overlap(cluster, Ref, MaxDR);
            }else {
                float Polution = clusterEtMinusTile(cluster) / (correction != 0 ? correction : 1.);
                if (Polution > m_maxTopoPolution || Polution < 0.) {
                    ATH_MSG_VERBOSE("The cluster could not contributed to the isolation cone. As it has " << Polution << " times more energy");
                } else if (overlap(cluster, Ref, MaxDR)) Subtract = true;
            }
            if (Subtract) {
                ATH_MSG_VERBOSE("Subtract " << clusterEtMinusTile(cluster) / 1.e3 << " GeV from cone " << correction / 1.e3 << " GeV");
                correction -= clusterEtMinusTile(cluster);
            }
        }
        ATH_MSG_DEBUG(xAOD::Iso::toString(type) << " of " << particleName(par) << " with pt " << par->pt() / 1.e3 << " GeV, eta: " << par->eta() << ", phi: " << par->phi() << " after correction: " << correction / 1.e3 << " GeV");
        return CorrectionCode::Ok;
    }
    float IsolationCloseByCorrectionTool::caloCorrectionFraction(const xAOD::IParticle* P, const xAOD::IParticle* P1, float coneSize, int Model) const {
        if (isSame(P, P1)) return 0.;
        float fraction = 0.;
        double dR = sqrt(deltaR2(P, P1));
        if (Model == TopoConeCorrectionModel::ParticleCaloCorrection && coneSize > m_coreCone && dR <= coneSize) fraction = 1.;
        else if (Model == TopoConeCorrectionModel::CaloCorrectionExtCore && dR > 2.0 * m_coreCone && dR <= (coneSize - m_coreCone)) fraction = 1.;
        else if (Model == TopoConeCorrectionModel::CaloCorrectionVarAnulus) {
            float drA = fmin(2.0 * m_coreCone, coneSize - m_coreCone);
            fraction = fmin(dR / m_coreCone, 1.0);
            if (dR > drA) {
                float drB = fmax(2 * m_coreCone, coneSize - m_coreCone);
                fraction = fmin(drA / m_coreCone, 1.0);
                if (dR > drB) {
                    float drC = coneSize + m_coreCone;
                    fraction = fraction * (drC - dR) / (drC - drB);
                }
            }
            fraction = fmin(fmax(fraction, 0.), 1.);
        }
        return fraction;
    }
    float IsolationCloseByCorrectionTool::caloCorrectionFromDecorator(const xAOD::IParticle* ToCorrect, const xAOD::IParticle* CloseBy, float ConeSize, int Model) const {
        float fraction = caloCorrectionFraction(ToCorrect, CloseBy, ConeSize, Model);
//No contribution from the close by particle expected
        if (fraction == 0) return fraction;
        float coreToBeRemoved = 0;
        auto acc = xAOD::getIsolationCorrectionAccessor(xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::coreEnergy);
        if (!acc->isAvailable(*ToCorrect)) {
            if (ToCorrect->type() == xAOD::Type::ObjectType::Muon) {
                const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(ToCorrect);
                mu->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
            } else if (ToCorrect->type() == xAOD::Type::ObjectType::Electron || ToCorrect->type() == xAOD::Type::ObjectType::Photon) {
                const xAOD::Egamma* EG = dynamic_cast<const xAOD::Egamma*>(ToCorrect);
                EG->isolationCaloCorrection(coreToBeRemoved, xAOD::Iso::topoetcone, xAOD::Iso::coreCone, xAOD::Iso::IsolationCorrectionParameter::coreEnergy);
            } else ATH_MSG_WARNING("Could not retrieve topocore. No correction could be calculated.");

        } else coreToBeRemoved = (*acc)(*ToCorrect);

        return coreToBeRemoved * fraction;
    }
    void IsolationCloseByCorrectionTool::getExtrapEtaPhi(const xAOD::IParticle* par, float& eta, float& phi) const {
        phi = par->phi();
        eta = par->eta();
        if (par->type() == xAOD::Type::ObjectType::Muon) {
            const xAOD::CaloCluster* cluster = getCluster(par);
            //At the moment no cluster associated with muons is in the derivations
            int nSample = 0;
            float etaT = 0.0, phiT = 0.0;
            if (cluster) {
                for (unsigned int i = 0; i < CaloSampling::Unknown; i++) {
                    auto s = static_cast<CaloSampling::CaloSample>(i);
                    if (cluster->hasSampling(s)) {
                        ATH_MSG_VERBOSE("Sampling: " << i << "eta-phi (" << cluster->etaSample(s) << ", " << cluster->phiSample(s) << ")");
                        etaT += cluster->etaSample(s);
                        phiT += cluster->phiSample(s);
                        ++nSample;
                    }
                }
            }
            if (nSample > 0) {
                ATH_MSG_DEBUG("Eta, phi before sampling: " << eta << ", " << phi << " and after sampling: " << etaT / nSample << ", " << phiT / nSample);
                eta = etaT / nSample;
                phi = phiT / nSample;
            }
        } else if (isEgamma(par)) {
            const xAOD::CaloCluster* cluster = getCluster(par);
            if (cluster) {
                phi = cluster->phi();
                eta = cluster->eta();
            }
        }
    }
    Root::TAccept IsolationCloseByCorrectionTool::acceptCorrected(const xAOD::IParticle& x, const xAOD::IParticleContainer& closePar, int topoetconeModel) const {
        Root::TAccept accept;
        if (!m_isInitialised) {
            ATH_MSG_WARNING("The IsolationCloseByCorrectionTool was not initialised!!!");
        }
        if (m_selectorTool.empty()) {
            ATH_MSG_ERROR("Please set the IsolationSelectionTool property with a valid IsolationSelectionTool");
            accept.clear();
            if (m_dec_isoselection) m_dec_isoselection->operator()(x) = false;
            return accept;
        }
        if (!getIsolationTypes(&x)) {
            ATH_MSG_WARNING("Could not cast particle for acceptCorrected. Will return false.");
            accept.setCutResult("castCut", false);
            if (m_dec_isoselection) m_dec_isoselection->operator()(x) = false;
            return accept;
        }

        if (closePar.empty()) return m_selectorTool->accept(x);

        accept.clear();
        strObj strPar;
        strPar.isolationValues.resize(xAOD::Iso::numIsolationTypes);
        strPar.pt = x.pt();
        strPar.eta = x.eta();
        strPar.type = x.type();
        std::vector<float> corrections;
        if (getCloseByCorrection(corrections, x, *getIsolationTypes(&x), closePar, topoetconeModel) == CorrectionCode::Error) {
            ATH_MSG_WARNING("Could not calculate the corrections. acceptCorrected(x) is done without the corrections.");
            if (m_dec_isoselection) m_dec_isoselection->operator()(x) = m_selectorTool->accept(x);
            return m_selectorTool->accept(x);
        }
        for (unsigned int i = 0; i < getIsolationTypes(&x)->size(); i++) {
            SG::AuxElement::Accessor<float> *acc = xAOD::getIsolationAccessor(getIsolationTypes(&x)->at(i));
            float old = acc->operator()(x);
            ATH_MSG_DEBUG("Correcting " << xAOD::Iso::toString(getIsolationTypes(&x)->at(i)) << " from " << old << " to " << corrections.at(i));
            strPar.isolationValues[getIsolationTypes(&x)->at(i)] = corrections.at(i);
        }
        accept = m_selectorTool->accept(strPar);
        if (m_dec_isoselection) m_dec_isoselection->operator()(x) = accept;
        return accept;
    }

    Root::TAccept IsolationCloseByCorrectionTool::acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel) const {
        xAOD::IParticleContainer Container(SG::VIEW_ELEMENTS);
        for (auto&P : closePar)
            Container.push_back(const_cast<xAOD::IParticle*>(P));
        return acceptCorrected(x, Container, topoetconeModel);
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

    double IsolationCloseByCorrectionTool::coneSize(const xAOD::IParticle* P, IsoType Cone) const {
        double ConeDR = xAOD::Iso::coneSize(Cone);
        if (isVarTrackIso(Cone)) {
            const xAOD::IParticle* Reference = trackIsoRefPart(P);
            double MiniIso = m_ptvarconeRadius / unCalibPt(Reference);
            if (MiniIso < ConeDR) return MiniIso;
        }
        return ConeDR;
    }
    double IsolationCloseByCorrectionTool::unCalibPt(const xAOD::IParticle* P) const {
        if (!P) {
            ATH_MSG_WARNING("No partcile given. Return stupidly big number. ");
            return 1.e25;
        }
        const xAOD::IParticle* OR = xAOD::getOriginalObject(*P);
        if (!OR) {
            ATH_MSG_VERBOSE("No reference from the shallow copy container of " << particleName(P) << " could be found");
            return P->pt();
        }
        return OR->pt();
    }

    const xAOD::IParticle* IsolationCloseByCorrectionTool::trackIsoRefPart(const xAOD::IParticle* P) const {
        if (!P) {
            ATH_MSG_ERROR("Nullptr given");
            return nullptr;
        }
        //Use for muons the associated ID track. Else the particle itself
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
    bool IsolationCloseByCorrectionTool::isSame(const xAOD::IParticle* P, const xAOD::IParticle* P1) const {
        if (!P || !P1) {
            ATH_MSG_WARNING("Null ptr were given");
            return true;
        }
        return (P == P1);
    }

    double IsolationCloseByCorrectionTool::deltaR2(const xAOD::IParticle* P, const xAOD::IParticle* P1, bool /*AvgCalo*/) const {
        if (!P || !P1) {
            ATH_MSG_WARNING("IsolationCloseByCorrectionTool::deltaR2(): One of the given Particles points to nullptr return 1.e4");
            return 1.e4;
        }
        if (isSame(P, P1)) return 0.;
        //Check if one of the objects is a CaloCluster or the Averaging over the clusters is requested.
//        if (AvgCalo || (P->type() != P1->type() && (P->type() == xAOD::Type::ObjectType::CaloCluster || P1->type() == xAOD::Type::ObjectType::CaloCluster))) {
//            float phi1(0), eta1(0), eta2(0), phi2(0);
//            getExtrapEtaPhi(P, eta1, phi1);
//            getExtrapEtaPhi(P1, eta2, phi2);
//            double dPhi = xAOD::P4Helpers::deltaPhi(phi1, phi2);
//            double dEta = eta1 - eta2;
//            return dEta * dEta + dPhi * dPhi;
//        }
        double dPhi = xAOD::P4Helpers::deltaPhi(P, P1);
        double dEta = P->eta() - P1->eta();
        return dEta * dEta + dPhi * dPhi;
    }
    bool IsolationCloseByCorrectionTool::overlap(const xAOD::IParticle* P, const xAOD::IParticle* P1, double dR) const {
        return (!isSame(P, P1) && deltaR2(P, P1) < (dR * dR));
    }
    bool IsolationCloseByCorrectionTool::isFixedTrackIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::ptcone == xAOD::Iso::isolationFlavour(Iso);
    }
    bool IsolationCloseByCorrectionTool::isVarTrackIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::ptvarcone == xAOD::Iso::isolationFlavour(Iso);
    }
    bool IsolationCloseByCorrectionTool::isTrackIso(xAOD::Iso::IsolationType Iso) const {
        return isVarTrackIso(Iso) || isFixedTrackIso(Iso);
    }
    bool IsolationCloseByCorrectionTool::isTopoEtIso(xAOD::Iso::IsolationType Iso) const {
        return xAOD::Iso::IsolationFlavour::topoetcone == xAOD::Iso::isolationFlavour(Iso);
    }
    float IsolationCloseByCorrectionTool::getOriginalIsolation(const xAOD::IParticle* particle, IsoType isoVariable) const {
        IsoHelperMap::const_iterator itr = m_isohelpers.find(isoVariable);
        float isovalue = 0;
        if (itr == m_isohelpers.end() || itr->second->getOrignalIsolation(particle, isovalue) == CorrectionCode::Error) {
            ATH_MSG_ERROR("Failed to retrive the original  isolation  cone ");
            isovalue = nanf("nan");
        }
        return isovalue;
    }
    float IsolationCloseByCorrectionTool::getOriginalIsolation(const xAOD::IParticle& particle, IsoType type) const {
        return getOriginalIsolation(&particle, type);
    }
    float IsolationCloseByCorrectionTool::clusterEtMinusTile(const xAOD::CaloCluster* cluster) const {
        float Et = 0.;
        if (cluster != nullptr) {
            try {
                Et = cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et();
                Et = Et - cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3) / TMath::CosH(cluster->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
            } catch (...) {
                ATH_MSG_DEBUG("Could not retrieve the uncalibrated Cluster state");
                Et = cluster->p4().Et();
            }
        } else ATH_MSG_WARNING("No CaloCluster was given. Return 0.");
        return fmax(Et, 0);
    }
    template<typename T> bool IsolationCloseByCorrectionTool::isElementInList(const std::vector<T> &List, const T& Element) const {
        for (const auto&Test : List) {
            if (Test == Element) {
                return true;
            }
        }
        return false;
    }
    template<typename T> bool IsolationCloseByCorrectionTool::isElementInList(const std::set<T> &List, const T& Element) const {
        for (auto&Test : List) {
            if (Test == Element) return true;
        }
        return false;
    }
    std::string IsolationCloseByCorrectionTool::particleName(const xAOD::IParticle* particle) const {
        if (particle->type() == xAOD::Type::ObjectType::Electron) return "Electron";
        if (particle->type() == xAOD::Type::ObjectType::Photon) return "Photon";
        if (particle->type() == xAOD::Type::ObjectType::Muon) return "Muon";
        if (particle->type() == xAOD::Type::ObjectType::TrackParticle) return "Track";
        if (particle->type() == xAOD::Type::ObjectType::CaloCluster) return "Cluster";
        return "Unknown";
    }
    const xAOD::IParticle* IsolationCloseByCorrectionTool::topoEtIsoRefPart(const xAOD::IParticle* particle) const {
        if (particle == nullptr) {
            ATH_MSG_WARNING("topoEtIsoRefPart(): Nullptr given");
            return nullptr;
        }
//Use for Muons the associated track particle
//        if (particle->type() == xAOD::Type::ObjectType::Muon) return getTrackParticle(P);
//Electrons and photons shall use the cluster as reference
        if (isEgamma(particle)) return getCluster(particle);
        return particle;
    }
    const xAOD::CaloCluster* IsolationCloseByCorrectionTool::getCluster(const xAOD::IParticle* particle) const {
        if (!particle) return nullptr;
        else if (particle->type() == xAOD::Type::ObjectType::CaloCluster) return dynamic_cast<const xAOD::CaloCluster*>(particle);
        else if (isEgamma(particle)) {
            const xAOD::Egamma* EG = dynamic_cast<const xAOD::Egamma*>(particle);
            return EG->caloCluster();
        } else if (particle->type() == xAOD::Type::ObjectType::Muon) {
            const xAOD::Muon* Mu = dynamic_cast<const xAOD::Muon*>(particle);
            return Mu->cluster();
        }
        ATH_MSG_WARNING("No cluster was found");
        return nullptr;
    }
}
