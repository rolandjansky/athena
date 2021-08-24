/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTruthAssociationAlg.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "StoreGate/WriteDecorHandle.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

namespace {
    constexpr unsigned int dummy_unsigned = 999;
    void increment_unsigned(unsigned& val) {
        if (val == dummy_unsigned)
            val = 1;
        else
            ++val;
    }
    static const SG::AuxElement::ConstAccessor<int> acc_origin("truthOrigin");
    static const SG::AuxElement::ConstAccessor<int> acc_type("truthType");
    static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > acc_link("truthParticleLink");
    //
    static const SG::AuxElement::Decorator<int> dec_origin("truthOrigin");
    static const SG::AuxElement::Decorator<int> dec_type ("truthType");
    static const SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer> > dec_link("truthParticleLink");
    
}  // namespace
// Constructor with parameters:
MuonTruthAssociationAlg::MuonTruthAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

// Initialize method:
StatusCode MuonTruthAssociationAlg::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    m_muonTruthParticleContainerName = m_muonTruthParticleContainerName.key() + ".recoMuonLink";
    ATH_CHECK(m_muonTruthParticleContainerName.initialize());
    m_muonTruthParticleLink = m_muonName + ".truthParticleLink";
    m_muonTruthParticleType = m_muonName + ".truthType";
    m_muonTruthParticleOrigin = m_muonName + ".truthOrigin";
    m_muonTruthParticleNPrecMatched = m_muonName + ".nprecMatchedHitsPerChamberLayer";
    m_muonTruthParticleNPhiMatched = m_muonName + ".nphiMatchedHitsPerChamberLayer";
    m_muonTruthParticleNTrigEtaMatched = m_muonName + ".ntrigEtaMatchedHitsPerChamberLayer";
    ATH_CHECK(m_muonTruthParticleLink.initialize());
    ATH_CHECK(m_muonTruthParticleOrigin.initialize());
    ATH_CHECK(m_muonTruthParticleType.initialize());
    ATH_CHECK(m_muonTruthParticleNPrecMatched.initialize());
    ATH_CHECK(m_muonTruthParticleNPhiMatched.initialize());
    ATH_CHECK(m_muonTruthParticleNTrigEtaMatched.initialize());
    ATH_CHECK(m_cbMuTrkPartLinkToRead.initialize());
    ATH_CHECK(m_extMuTrkPartLinkToRead.initialize());
    ATH_CHECK(m_indetTrkPartLinkToRead.initialize());
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode MuonTruthAssociationAlg::execute(const EventContext& ctx) const {
    SG::WriteDecorHandle<xAOD::TruthParticleContainer, ElementLink<xAOD::MuonContainer> > muonTruthParticleRecoLink(
        m_muonTruthParticleContainerName, ctx);
    if (!muonTruthParticleRecoLink.isPresent()) return StatusCode::SUCCESS;
    if (!muonTruthParticleRecoLink.isValid()) {
        ATH_MSG_WARNING("truth particle container not valid");
        return StatusCode::FAILURE;
    }
    SG::WriteDecorHandle<xAOD::MuonContainer, ElementLink<xAOD::TruthParticleContainer> > muonTruthParticleLink(m_muonTruthParticleLink,
                                                                                                                ctx);
    if (!muonTruthParticleLink.isPresent()) return StatusCode::SUCCESS;
    if (!muonTruthParticleLink.isValid()) {
        ATH_MSG_WARNING("muon particle container not valid");
        return StatusCode::FAILURE;
    }
    SG::WriteDecorHandle<xAOD::MuonContainer, int> muonTruthParticleType(m_muonTruthParticleType, ctx);
    SG::WriteDecorHandle<xAOD::MuonContainer, int> muonTruthParticleOrigin(m_muonTruthParticleOrigin, ctx);
    SG::WriteDecorHandle<xAOD::MuonContainer, std::vector<unsigned int> > muonTruthParticleNPrecMatched(m_muonTruthParticleNPrecMatched,
                                                                                                        ctx);
    SG::WriteDecorHandle<xAOD::MuonContainer, std::vector<unsigned int> > muonTruthParticleNPhiMatched(m_muonTruthParticleNPhiMatched, ctx);
    SG::WriteDecorHandle<xAOD::MuonContainer, std::vector<unsigned int> > muonTruthParticleNTrigEtaMatched(
        m_muonTruthParticleNTrigEtaMatched, ctx);

    // add link to reco muons and viceversa

    // loop over muons
    for (const xAOD::Muon* muon : *muonTruthParticleLink) {
        // use primary track particle to get the truth link (except for the case of STACO, where we must use the ID track particle, as the
        // combined is not truth-matched)
        ATH_MSG_DEBUG("muon with pT " << muon->pt() << " MeV, eta: " << muon->eta() << ", phi " << muon->phi() << " and author "
                                      << muon->author());
        const xAOD::TrackParticle* tp = nullptr;
        if (m_associateWithInDetTP || muon->author() == xAOD::Muon::STACO || muon->author() == xAOD::Muon::MuGirl) {
            tp = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        } else {
            tp = muon->primaryTrackParticle();
        }

        bool foundTruth{false}, setOrigin{false};
        if (tp) {
            // Associate reco with truth muon. Loop over reconstructed muons, get track particle for each one.
            // Each track particle should carry a link to the corresponding truth particle. Then compare this truth particle link with the
            // given truth muon particle
            if (acc_origin.isAvailable(*tp) && acc_origin(*tp) != 0) {
                muonTruthParticleOrigin(*muon) = acc_origin(*tp);
                muonTruthParticleType(*muon) = acc_type(*tp);
                setOrigin = true;
            }
            
            ElementLink<xAOD::TruthParticleContainer> truthLink;  
            if (acc_link.isAvailable(*tp)) {
                truthLink =  acc_link(*tp);
            } else {
                ATH_MSG_DEBUG("Could not find  any truth link associated with track having pt:"<<tp->pt()<<" MeV, eta: "<<tp->eta()<<", phi: "<<tp->phi()<<", charge: "<<tp->charge()<<". d0:"<<tp->d0()<<", z0: "<<tp->z0());               
            }
           
            if (truthLink.isValid()) {
                ATH_MSG_VERBOSE(" Got valid truth link for muon author " << muon->author() << " barcode " << (*truthLink)->barcode());
                // loop over truth particles
                for (const xAOD::TruthParticle* truthParticle : *muonTruthParticleRecoLink) {
                    if (truthParticle->status() != 1) continue;
                    ATH_MSG_DEBUG("Got truth muon with barcode " << truthParticle->barcode() << " pt " << truthParticle->pt());
                    if (((*truthLink)->barcode() % m_barcodeOffset) != truthParticle->barcode()) {
                        ATH_MSG_VERBOSE("Barcode truth link: " << (*truthLink)->barcode() << " offset:  " << m_barcodeOffset << " --> "
                                                               << ((*truthLink)->barcode() % m_barcodeOffset)
                                                               << " != " << truthParticle->barcode());
                        continue;
                    }
                    ATH_MSG_VERBOSE("Truth muon barcode matches -> creating link with truth particle " << (*truthLink)->barcode());
                    foundTruth = true;
                    ElementLink<xAOD::MuonContainer> muonLink = ElementLink<xAOD::MuonContainer>(muon, *muonTruthParticleLink, ctx);
                    // add the link from xAOD::Muon to TruthParticle in m_muonTruthParticleContainerName
                    ElementLink<xAOD::TruthParticleContainer> muonTruthLink =
                        ElementLink<xAOD::TruthParticleContainer>(*muonTruthParticleRecoLink, truthParticle->index(), ctx);
                    muonTruthLink.toPersistent();
                    muonTruthParticleLink(*muon) = muonTruthLink;
                    if (!setOrigin) {
                        muonTruthParticleOrigin(*muon) = acc_origin(*tp);
                        muonTruthParticleType(*muon) = acc_type(*tp);
                        setOrigin = true;
                    }

                    /// Zero supression do not want to store meaningless zeros
                    std::vector<unsigned int> nprecHitsPerChamberLayer(Muon::MuonStationIndex::ChIndexMax, dummy_unsigned);
                    std::vector<unsigned int> nphiHitsPerChamberLayer(Muon::MuonStationIndex::PhiIndexMax, dummy_unsigned);
                    std::vector<unsigned int> ntrigEtaHitsPerChamberLayer(Muon::MuonStationIndex::PhiIndexMax, dummy_unsigned);

                    count_chamber_layers((muon->author() == xAOD::Muon::MuidCo || muon->author() == xAOD::Muon::MuidSA ||
                                          muon->author() == xAOD::Muon::MuGirl)
                                             ? truthParticle
                                             : nullptr,
                                         tp->track(), nprecHitsPerChamberLayer, nphiHitsPerChamberLayer, ntrigEtaHitsPerChamberLayer);
                    /// Decorate the results
                    muonTruthParticleNPrecMatched(*muon) = nprecHitsPerChamberLayer;
                    muonTruthParticleNPhiMatched(*muon) = nphiHitsPerChamberLayer;
                    muonTruthParticleNTrigEtaMatched(*muon) = ntrigEtaHitsPerChamberLayer;

                    muonLink.toPersistent();
                    muonTruthParticleRecoLink(*truthParticle) = muonLink;
                    break;
                }
            } else {
                ATH_MSG_DEBUG("Invalid truth link");
            }
        } else {
            ATH_MSG_WARNING("Could not find the appropiate track particle for muon with pT: " << muon->pt() * 1.e-3 << " GeV,  eta: "
                                                                                              << muon->eta() << ", phi: " << muon->phi()
                                                                                              << " author: " << muon->author());
        }

        if (!setOrigin) {
            muonTruthParticleOrigin(*muon) = 0;
            muonTruthParticleType(*muon) = 0;
        }
        if (!foundTruth) {
            muonTruthParticleLink(*muon) = ElementLink<xAOD::TruthParticleContainer>();
            // add these empty vectors
            muonTruthParticleNPrecMatched(*muon) = std::vector<unsigned int>{};
            muonTruthParticleNPhiMatched(*muon) = std::vector<unsigned int>{};
            muonTruthParticleNTrigEtaMatched(*muon) = std::vector<unsigned int>{};
        }
        /// Patch for STACO muons: Copy the truth information from the muon back to the combined 
        /// track to avoid file corruptions  reported in ATLASRECTS-6454
        if (muon->author() == xAOD::Muon::STACO) {
            const xAOD::TrackParticle* cmb_trk = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);
            if (!cmb_trk){
                ATH_MSG_WARNING("Even a STACO muon should have a combined track");
                continue;
            } else {
                dec_origin(*cmb_trk) = acc_origin(*muon);
                dec_type(*cmb_trk) = acc_type(*muon);
                dec_link(*cmb_trk) = acc_link(*muon);
            }
        }
    }
    /// one more thing: need to have muonlink set for all truth particles to avoid ELReset errors
    for (const auto truthParticle : *muonTruthParticleRecoLink) {
        if (!truthParticle->isAvailable<ElementLink<xAOD::MuonContainer> >("recoMuonLink")) {
            ATH_MSG_DEBUG("no reco muon link set, add an empty one");
            muonTruthParticleRecoLink(*truthParticle) = ElementLink<xAOD::MuonContainer>();
        }
    }
    return StatusCode::SUCCESS;
}

void MuonTruthAssociationAlg::count_chamber_layers(const xAOD::IParticle* truthParticle, const Trk::Track* ptrk,
                                                   std::vector<unsigned int>& nprecHitsPerChamberLayer,
                                                   std::vector<unsigned int>& nphiHitsPerChamberLayer,
                                                   std::vector<unsigned int>& ntrigEtaHitsPerChamberLayer) const {
    if (!truthParticle || !truthParticle->isAvailable<std::vector<unsigned long long> >("truthMdtHits")) {
        ATH_MSG_DEBUG("muon has no truth hits vector in the truth association alg");
        nprecHitsPerChamberLayer.clear();
        nphiHitsPerChamberLayer.clear();
        ntrigEtaHitsPerChamberLayer.clear();
        return;
    }
    std::vector<unsigned long long> mdtTruth = truthParticle->auxdata<std::vector<unsigned long long> >("truthMdtHits");
    std::vector<unsigned long long> cscTruth;
    if (m_idHelperSvc->hasCSC()) cscTruth = truthParticle->auxdata<std::vector<unsigned long long> >("truthCscHits");
    std::vector<unsigned long long> rpcTruth = truthParticle->auxdata<std::vector<unsigned long long> >("truthRpcHits");
    std::vector<unsigned long long> tgcTruth = truthParticle->auxdata<std::vector<unsigned long long> >("truthTgcHits");

    for (const Trk::TrackStateOnSurface* tsit : *ptrk->trackStateOnSurfaces()) {
        if (!tsit || !tsit->trackParameters() || !tsit->measurementOnTrack()) continue;
        const Trk::MeasurementBase* meas = tsit->measurementOnTrack();
        Identifier id;
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
        if (rot)
            id = rot->identify();
        else {
            const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
            if (crot && !crot->containedROTs().empty() && crot->containedROTs().front()) id = crot->containedROTs().front()->identify();
        }
        if (!m_idHelperSvc->isMuon(id)) continue;

        bool measPhi = m_idHelperSvc->measuresPhi(id);
        bool isTgc = m_idHelperSvc->isTgc(id);
        Muon::MuonStationIndex::ChIndex chIndex = !isTgc ? m_idHelperSvc->chamberIndex(id) : Muon::MuonStationIndex::ChUnknown;
        if (m_idHelperSvc->isMdt(id)) {
            for (unsigned int i = 0; i < mdtTruth.size(); i++) {
                if (id == mdtTruth[i]) {
                    increment_unsigned(nprecHitsPerChamberLayer[chIndex]);
                    break;
                }
            }
        } else if (m_idHelperSvc->hasCSC() && m_idHelperSvc->isCsc(id)) {
            for (unsigned int i = 0; i < cscTruth.size(); i++) {
                if (id != cscTruth[i]) continue;
                if (measPhi) {
                    Muon::MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(id);
                    increment_unsigned(nphiHitsPerChamberLayer[index]);
                } else {
                    increment_unsigned(nprecHitsPerChamberLayer[chIndex]);
                }
                break;
            }
        } else if (m_idHelperSvc->isRpc(id)) {
            for (unsigned int i = 0; i < rpcTruth.size(); i++) {
                if (id != rpcTruth[i]) { continue; }
                int index = m_idHelperSvc->phiIndex(id);
                if (measPhi) {
                    increment_unsigned(nphiHitsPerChamberLayer[index]);
                } else {
                    increment_unsigned(ntrigEtaHitsPerChamberLayer[index]);
                }
                break;
            }
        } else if (m_idHelperSvc->isTgc(id)) {
            for (unsigned int i = 0; i < tgcTruth.size(); i++) {
                if (id != tgcTruth[i]) { continue; }
                int index = m_idHelperSvc->phiIndex(id);
                if (measPhi) {
                    increment_unsigned(nphiHitsPerChamberLayer[index]);
                } else {
                    increment_unsigned(ntrigEtaHitsPerChamberLayer[index]);
                }
                break;
            }
        }
    }  // end loop over TSOS
    ATH_MSG_DEBUG("finished loop over TSOS");

    // now, have to check if there are non-zero truth hits in indices without reco hits
    clear_dummys(mdtTruth, nprecHitsPerChamberLayer);
    clear_dummys(cscTruth, nprecHitsPerChamberLayer);

    clear_dummys(cscTruth, nphiHitsPerChamberLayer);
    clear_dummys(rpcTruth, nphiHitsPerChamberLayer);
    clear_dummys(tgcTruth, nphiHitsPerChamberLayer);

    clear_dummys(rpcTruth, ntrigEtaHitsPerChamberLayer);
    clear_dummys(tgcTruth, ntrigEtaHitsPerChamberLayer);
}
void MuonTruthAssociationAlg::clear_dummys(const std::vector<unsigned long long>& identifiers, std::vector<unsigned int>& vec) const {
    /// If the identifiers are empty then there
    /// is no change that a dummy value could be cleared from this list
    if (identifiers.empty()) { return; }
    for (unsigned int i = 0; i < vec.size(); i++) {
        if (vec[i] != dummy_unsigned) continue;
        for (unsigned j = 0; j < identifiers.size(); ++j) {
            Identifier id(identifiers[j]);
            if ((m_idHelperSvc->measuresPhi(id) && m_idHelperSvc->phiIndex(id) == (Muon::MuonStationIndex::PhiIndex)i) ||
                (!m_idHelperSvc->measuresPhi(id) && m_idHelperSvc->chamberIndex(id) == (Muon::MuonStationIndex::ChIndex)i)) {
                vec[i] = 0;
                break;
            }
        }
    }
}