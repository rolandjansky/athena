/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTruthDecorationAlg.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace {
    const SG::AuxElement::Decorator<int> dec_truthOrigin{"truthOrigin"};
    const SG::AuxElement::Decorator<int> dec_truthType{"truthType"};
    const std::vector<float> emptyVec;

    // Only reject muons from light quark deays
    const std::set<int> bad_origins{
        MCTruthPartClassifier::ParticleOrigin ::LightMeson,     // 23
        MCTruthPartClassifier::ParticleOrigin ::StrangeMeson,   // 24
        MCTruthPartClassifier::ParticleOrigin ::LightBaryon,    // 30
        MCTruthPartClassifier::ParticleOrigin ::StrangeBaryon,  // 31
        MCTruthPartClassifier::ParticleOrigin ::PionDecay,      // 34
        MCTruthPartClassifier::ParticleOrigin ::NucReact,       // 41
        MCTruthPartClassifier::ParticleOrigin ::PiZero,         // 42

    };
    constexpr float dummy_val = -1.;
    
     struct RecordPars {
        RecordPars() = default;
        RecordPars(Amg::Vector3D&& _pos, Amg::Vector3D&&_mom):
            pos{std::move(_pos)},
            mom{std::move(_mom)}{}
        RecordPars(const CLHEP::Hep3Vector& _pos, const CLHEP::Hep3Vector& _mom):
            pos{_pos.x(), _pos.y(), _pos.z()},
            mom{_mom.x(), _mom.y(), _mom.z()}{
            
        }
        const Amg::Vector3D pos{Amg::Vector3D::Zero()};
        const Amg::Vector3D mom{Amg::Vector3D::Zero()};
        
        std::string record_name;
        const Trk::TrackingVolume* volume{nullptr};
    };

}  // namespace
namespace Muon {

    // Constructor with parameters:
    MuonTruthDecorationAlg::MuonTruthDecorationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
        AthReentrantAlgorithm(name, pSvcLocator), m_muonMgr(nullptr) {}

    // Initialize method:
    StatusCode MuonTruthDecorationAlg::initialize() {
        ATH_CHECK(m_truthParticleContainerName.initialize());
        ATH_CHECK(m_muonTruthParticleContainerName.initialize());
        ATH_CHECK(m_muonTruthSegmentContainerName.initialize());
        ATH_CHECK(m_trackRecordCollectionNames.initialize());
        ATH_CHECK(m_PRD_TruthNames.initialize());
        ATH_CHECK(m_SDO_TruthNames.initialize());
        if (!m_CSC_SDO_TruthNames.empty()) ATH_CHECK(m_CSC_SDO_TruthNames.initialize());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_truthClassifier.retrieve());
        ATH_CHECK(m_extrapolator.retrieve());
        ATH_CHECK(detStore()->retrieve(m_muonMgr));
        return StatusCode::SUCCESS;
    }

    // Execute method:
    StatusCode MuonTruthDecorationAlg::execute(const EventContext& ctx) const {
        // skip if no input data found
        SG::ReadHandle<xAOD::TruthParticleContainer> truthContainer(m_truthParticleContainerName, ctx);
        if (!truthContainer.isPresent()) return StatusCode::SUCCESS;
        if (!truthContainer.isValid()) {
            ATH_MSG_WARNING("truth container " << truthContainer.name() << " not valid");
            return StatusCode::FAILURE;
        }

        // create output container
        SG::WriteHandle<xAOD::TruthParticleContainer> muonTruthContainer(m_muonTruthParticleContainerName, ctx);
        ATH_CHECK(muonTruthContainer.record(std::make_unique<xAOD::TruthParticleContainer>(),
                                            std::make_unique<xAOD::TruthParticleAuxContainer>()));
        ATH_MSG_DEBUG("Recorded TruthParticleContainer with key: " << m_muonTruthParticleContainerName);

        SG::WriteHandle<xAOD::MuonSegmentContainer> segmentContainer(m_muonTruthSegmentContainerName, ctx);
        if (m_createTruthSegment) {
            ATH_CHECK(
                segmentContainer.record(std::make_unique<xAOD::MuonSegmentContainer>(), std::make_unique<xAOD::MuonSegmentAuxContainer>()));
            ATH_MSG_DEBUG("Recorded MuonSegmentContainer with key: " << segmentContainer.name());
        }

        // loop over truth coll
        for (const auto truth : *truthContainer) {
            if (truth->status() != 1 || !truth->isMuon() || truth->pt() < 1000.) continue;
            xAOD::TruthParticle* truthParticle = new xAOD::TruthParticle();
            muonTruthContainer->push_back(truthParticle);
            truthParticle->setPdgId(truth->pdgId());
            truthParticle->setBarcode(truth->barcode());
            truthParticle->setStatus(truth->status());
            truthParticle->setPx(truth->px());
            truthParticle->setPy(truth->py());
            truthParticle->setPz(truth->pz());
            truthParticle->setE(truth->e());
            truthParticle->setM(truth->m());
            if (truth->hasProdVtx()) truthParticle->setProdVtxLink(truth->prodVtxLink());
            ElementLink<xAOD::TruthParticleContainer> truthLink(*muonTruthContainer, muonTruthContainer->size() - 1);
            truthLink.toPersistent();
            ATH_MSG_DEBUG("Found stable muon: " << truth->pt() << " eta " << truth->eta() << " phi " << truth->phi() << " mass "
                                                << truth->m() << " barcode " << truth->barcode() << " truthParticle->barcode "
                                                << truthParticle->barcode() << " (*truthLink)->barcode " << (*truthLink)->barcode() << " "
                                                << truthLink);
            int iType = 0;
            int iOrigin = 0;

            // if configured look up truth classification
            if (!m_truthClassifier.empty()) {
                // if configured also get truth classification
                std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> truthClass =
                    m_truthClassifier->particleTruthClassifier(truth);
                iType = truthClass.first;
                iOrigin = truthClass.second;
                ATH_MSG_VERBOSE("Got truth type  " << iType << "  origin " << iOrigin);
                dec_truthOrigin(*truthParticle) = iOrigin;
                dec_truthType(*truthParticle) = iType;
            }

            /// add track records
            addTrackRecords(ctx, *truthParticle);
            ChamberIdMap ids;

            // add hit counts
            addHitCounts(ctx, *truthParticle, ids);

            // add hit ID vectors
            addHitIDVectors(*truthParticle, ids);

            bool goodMuon = bad_origins.find(iOrigin) == bad_origins.end();

            ATH_MSG_DEBUG("good muon with type " << iType << " and origin" << iOrigin);

            // create segments
            if (m_createTruthSegment && goodMuon) { createSegments(ctx, truthLink, segmentContainer, ids); }
        }

        ATH_MSG_DEBUG("Registered " << muonTruthContainer->size() << " truth muons ");
        if (m_createTruthSegment) ATH_MSG_DEBUG("Registered " << segmentContainer->size() << " truth muon segments ");

        return StatusCode::SUCCESS;
    }

    void MuonTruthDecorationAlg::createSegments(const EventContext& ctx, const ElementLink<xAOD::TruthParticleContainer>& truthLink,
                                                SG::WriteHandle<xAOD::MuonSegmentContainer> segmentContainer,
                                                const MuonTruthDecorationAlg::ChamberIdMap& ids) const {
        std::vector<SG::ReadHandle<MuonSimDataCollection> > sdoCollections(6);
        for (const SG::ReadHandleKey<MuonSimDataCollection>& k : m_SDO_TruthNames) {
            SG::ReadHandle<MuonSimDataCollection> col(k, ctx);
            if (!col.isPresent()) {
                ATH_MSG_DEBUG("MuonSimDataCollection " << col.name() << " not in StoreGate");
                continue;
            }
            if (!col->empty()) {
                Identifier id = col->begin()->first;
                int index = m_idHelperSvc->technologyIndex(id);
                if (index >= (int)sdoCollections.size()) {
                    ATH_MSG_WARNING("SDO collection index out of range " << index << "  " << m_idHelperSvc->toStringChamber(id));
                } else {
                    sdoCollections[index] = col;
                }
            }
        }

        bool useSDO = (!sdoCollections.empty() || !m_CSC_SDO_TruthNames.empty());
        std::map<Muon::MuonStationIndex::ChIndex, int> matchMap;
        ATH_MSG_DEBUG(" Creating Truth segments ");
        // loop over chamber layers
        for (const auto& lay : ids) {
            // skip empty layers
            Amg::Vector3D firstPos{Amg::Vector3D::Zero()}, secondPos{Amg::Vector3D::Zero()};
            bool firstPosSet{false}, secondPosSet{false};
            Identifier chId;
            int index = -1;
            uint8_t nprecLayers{0}, nphiLayers{0}, ntrigEtaLayers{0};
            std::set<int> phiLayers, etaLayers, precLayers;
            ATH_MSG_DEBUG(" new chamber layer " << Muon::MuonStationIndex::chName(lay.first) << " hits " << ids.size());
            // loop over hits
            for (const auto& id : lay.second) {
                ATH_MSG_VERBOSE(" hit " << m_idHelperSvc->toString(id));
                bool measPhi = m_idHelperSvc->measuresPhi(id);
                bool isCsc = m_idHelperSvc->isCsc(id);
                bool isMM = m_idHelperSvc->isMM(id);
                bool isTrig = m_idHelperSvc->isTrigger(id);
                bool isEndcap = m_idHelperSvc->isEndcap(id);
                if (measPhi) {
                    phiLayers.insert(m_idHelperSvc->gasGap(id));
                } else {
                    if (!isTrig) {
                        if (!chId.is_valid()) chId = id;  // use first precision hit in list
                        if (isCsc || isMM) {
                            precLayers.insert(m_idHelperSvc->gasGap(id));
                        } else {
                            int iid = 10 * m_idHelperSvc->mdtIdHelper().multilayer(id) + m_idHelperSvc->mdtIdHelper().tubeLayer(id);
                            precLayers.insert(iid);
                            // ATH_MSG_VERBOSE("iid " << iid << " precLayers size " << precLayers.size() );
                        }
                    } else {
                        etaLayers.insert(m_idHelperSvc->gasGap(id));
                    }
                }
                // use SDO to look-up truth position of the hit
                if (!useSDO) {
                    continue;
               }
                Amg::Vector3D gpos{Amg::Vector3D::Zero()};
                if (!isCsc) {
                    bool ok = false;
                    int index = m_idHelperSvc->technologyIndex(id);
                    if (index < (int)sdoCollections.size() && sdoCollections[index]->size() != 0) {
                        auto pos = sdoCollections[index]->find(id);
                        if (pos != sdoCollections[index]->end()) {
                            gpos = pos->second.globalPosition();
                            if (gpos.perp() > 0.1) ok = true;  // sanity check
                        }
                    }
                    // look up successful, calculate
                    if (!ok) continue;
                    
                    // small comparison function
                    auto isSmaller = [isEndcap](const Amg::Vector3D& p1, const Amg::Vector3D& p2) {
                        if (isEndcap)
                            return std::abs(p1.z()) < std::abs(p2.z());
                        else
                            return p1.perp() < p2.perp();
                    };
                    if (!firstPosSet) {
                        firstPos = gpos;
                        firstPosSet = true;
                    } else if (!secondPosSet) {
                        secondPos = gpos;
                        secondPosSet = true;
                        if (isSmaller(secondPos, firstPos)) std::swap(firstPos, secondPos);
                    } else {
                        // update position if we can increase the distance between the two positions
                        if (isSmaller(gpos, firstPos))
                            firstPos = gpos;
                        else if (isSmaller(secondPos, gpos))
                            secondPos = gpos;
                    }                    
                } else {
                    SG::ReadHandle<CscSimDataCollection> cscCollection(m_CSC_SDO_TruthNames, ctx);
                    auto pos = cscCollection->find(id);
                    if (pos == cscCollection->end()) {
                        continue;
                    }
                    const MuonGM::CscReadoutElement* descriptor = m_muonMgr->getCscReadoutElement(id);
                    ATH_MSG_DEBUG("found csc sdo with " << pos->second.getdeposits().size() << " deposits");
                    Amg::Vector3D locpos(0, pos->second.getdeposits()[0].second.ypos(), pos->second.getdeposits()[0].second.zpos());
                    gpos = descriptor->localToGlobalCoords(locpos, m_idHelperSvc->cscIdHelper().elementID(id));
                    ATH_MSG_DEBUG("got CSC global position " << gpos);
                    if (!firstPosSet) {
                        firstPos = gpos;
                        firstPosSet = true;
                    } else if (!secondPosSet) {
                        secondPos = gpos;
                        secondPosSet = true;
                        if (secondPos.perp() < firstPos.perp()) std::swap(firstPos, secondPos);
                    } else {
                        if (gpos.perp() < firstPos.perp())
                            firstPos = gpos;
                        else if (secondPos.perp() < gpos.perp())
                            secondPos = gpos;
                    }                    
                }                
            }
            if (precLayers.size() > 2) {
                matchMap[lay.first] = index;
                if (!phiLayers.empty()) nphiLayers = phiLayers.size();
                ntrigEtaLayers = etaLayers.size();
                nprecLayers = precLayers.size();
                ATH_MSG_DEBUG(" total counts: precision " << static_cast<int>(nprecLayers) << " phi layers " << static_cast<int>(nphiLayers)
                                                          << " eta trig layers " << static_cast<int>(ntrigEtaLayers)
                                                          << " associated reco muon " << index << " barcode " << (*truthLink)->barcode()
                                                          << " truthLink " << truthLink);
                xAOD::MuonSegment* segment = new xAOD::MuonSegment();
                segmentContainer->push_back(segment);
                segment->setNHits(nprecLayers, nphiLayers, ntrigEtaLayers);
                segment->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink") = truthLink;
                if (chId.is_valid()) {
                    int eta = m_idHelperSvc->stationEta(chId);
                    int sector = m_idHelperSvc->sector(chId);
                    MuonStationIndex::TechnologyIndex technology = m_idHelperSvc->technologyIndex(chId);
                    MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
                    segment->setIdentifier(sector, chIndex, eta, technology);
                }
                if (firstPosSet && secondPosSet) {
                    Amg::Vector3D gpos = (firstPos + secondPos) / 2.;
                    Amg::Vector3D gdir = (firstPos - secondPos).unit();
                    ATH_MSG_DEBUG(" got position : r " << gpos.perp() << " z " << gpos.z() << "  and direction: theta " << gdir.theta()
                                                       << " phi " << gdir.phi());
                    segment->setPosition(gpos.x(), gpos.y(), gpos.z());
                    segment->setDirection(gdir.x(), gdir.y(), gdir.z());
                }
            }
        }
    }

    void MuonTruthDecorationAlg::addTrackRecords(const EventContext& ctx, xAOD::TruthParticle& truthParticle) const {
        // first loop over track records, store parameters at the different positions
        const int barcode = truthParticle.barcode();
        const xAOD::TruthVertex* vertex = truthParticle.prodVtx();        
        const Trk::TrackingGeometry* trackingGeometry = !m_extrapolator.empty()? m_extrapolator->trackingGeometry() : nullptr;
      
        std::vector<RecordPars> parameters;
        if (vertex) {
            parameters.emplace_back(Amg::Vector3D{vertex->x(), vertex->y(), vertex->z()},
                                    Amg::Vector3D{truthParticle.px(), truthParticle.py(), truthParticle.pz()});
        }
        for (SG::ReadHandle<TrackRecordCollection>& col : m_trackRecordCollectionNames.makeHandles(ctx)) {
            if (!col.isPresent()) continue;
            const std::string r_name = col.key();
            
            float& x = truthParticle.auxdata<float>(r_name + "_x");
            float& y = truthParticle.auxdata<float>(r_name + "_y");
            float& z = truthParticle.auxdata<float>(r_name + "_z");
            float& px = truthParticle.auxdata<float>(r_name + "_px");
            float& py = truthParticle.auxdata<float>(r_name + "_py");
            float& pz = truthParticle.auxdata<float>(r_name + "_pz");
            bool& found_truth = truthParticle.auxdata<bool>(r_name + "_is_matched");
            x = y = z = px = py = pz = dummy_val;
            found_truth = false;
            // Need to always make these, to avoid crashes later
            float& ex = truthParticle.auxdata<float>(r_name + "_x_extr");
            float& ey = truthParticle.auxdata<float>(r_name + "_y_extr");
            float& ez = truthParticle.auxdata<float>(r_name + "_z_extr");
            float& epx = truthParticle.auxdata<float>(r_name + "_px_extr");
            float& epy = truthParticle.auxdata<float>(r_name + "_py_extr");
            float& epz = truthParticle.auxdata<float>(r_name + "_pz_extr");
            truthParticle.auxdata<std::vector<float> >(r_name + "_cov_extr") = emptyVec;
            truthParticle.auxdata<bool>(r_name+"_is_extr") = false;
            ex = ey = ez = epx = epy = epz = dummy_val;
            
            // loop over collection and find particle with the same bar code
            for (const auto& particle : *col) {
                if ((particle.GetBarCode()) % m_barcodeOffset != barcode) continue;
                CLHEP::Hep3Vector pos = particle.GetPosition();
                CLHEP::Hep3Vector mom = particle.GetMomentum();
                ATH_MSG_VERBOSE("Found associated  " << r_name << " pt " << mom.perp() << " position: r " << pos.perp() << " z " << pos.z());
                x = pos.x(); y = pos.y(); z = pos.z();
                px = mom.x(); py = mom.y(); pz = mom.z();
                parameters.emplace_back(pos, mom);                
                found_truth = true;
                break;
            }
            
            if (!trackingGeometry) continue;
            
            /// Make sure that the parameter vector has the same size
            if (!found_truth) parameters.emplace_back();
            RecordPars& r_pars = parameters.back();
            r_pars.record_name = r_name;
            const Trk::TrackingVolume* volume = nullptr;
            if (r_name == "CaloEntryLayer")
                volume = trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
            else if (r_name == "MuonEntryLayer")
                volume = trackingGeometry->trackingVolume("Calo::Container");
            else if (r_name == "MuonExitLayer")
                volume = trackingGeometry->trackingVolume("Muon::Containers::MuonSystem");
            else {
                ATH_MSG_WARNING("no destination surface for track record "<<r_name);
            }
            r_pars.volume = found_truth ? volume : nullptr;
        }
        /// Second loop, extrapolate between the points
        
        /// extrapolation needs to be setup correctly
        if (!trackingGeometry) return;
       
        
        AmgSymMatrix(5) cov;
        cov.setIdentity();
        cov(0, 0) = cov(1, 1) = 1e-3;
        cov(2, 2) = cov(3, 3) = 1e-6;
        cov(4, 4) = 1e-3 / truthParticle.p4().P();
        for (unsigned int i = 0; i < parameters.size() - 1; ++i) {
            const RecordPars& start_pars = parameters[i];
            const RecordPars& end_pars = parameters[i+1];
            /// If the track record is named, then we should have a volume
            if ( (start_pars.record_name.size() && !start_pars.volume) || !end_pars.volume) continue;
            Trk::CurvilinearParameters pars(start_pars.pos, start_pars.mom, truthParticle.charge(), cov);
            
            const Trk::TrackingVolume* volume = end_pars.volume;
            const std::string& r_name = end_pars.record_name;
            float& ex = truthParticle.auxdata<float>(r_name + "_x_extr");
            float& ey = truthParticle.auxdata<float>(r_name + "_y_extr");
            float& ez = truthParticle.auxdata<float>(r_name + "_z_extr");
            float& epx = truthParticle.auxdata<float>(r_name + "_px_extr");
            float& epy = truthParticle.auxdata<float>(r_name + "_py_extr");
            float& epz = truthParticle.auxdata<float>(r_name + "_pz_extr");
            std::vector<float>& covMat = truthParticle.auxdata<std::vector<float> >(r_name + "_cov_extr");
           
            std::unique_ptr<const Trk::TrackParameters> exPars{
                m_extrapolator->extrapolateToVolume(ctx, pars, *volume, Trk::alongMomentum, Trk::muon)};
            if (! exPars || !exPars->covariance() || !Amg::saneCovarianceDiagonal(*exPars->covariance())) {
                ATH_MSG_VERBOSE("Extrapolation to "<<r_name<<" failed. ");
                continue;                    
            }
            truthParticle.auxdata<bool>(r_name+"_is_extr") = true;            
            ex = exPars->position().x();
            ey = exPars->position().y();
            ez = exPars->position().z();
            epx = exPars->momentum().x();
            epy = exPars->momentum().y();
            epz = exPars->momentum().z();
            Amg::compress(*exPars->covariance(), covMat);
            
            const double errorp = Amg::error(*exPars->covariance(), Trk::qOverP);
            ATH_MSG_VERBOSE( " Extrapolated to " << r_name << std::endl
                << " truth: r " << end_pars.pos.perp() << " z "
                << end_pars.pos.z() << " p "
                << end_pars.mom.mag() << std::endl
                << " extrp: r " << exPars->position().perp() << " z "
                << exPars->position().z() << " p "
                << exPars->momentum().mag() << " res p "
                << (end_pars.mom.mag() -
                    exPars->momentum().mag())
                << " error " << errorp << " cov "
                << (*exPars->covariance())(Trk::qOverP, Trk::qOverP)
                << " pull p "
                << (end_pars.mom.mag() -
                    exPars->momentum().mag()) /
                     errorp);                
        }                
    }

    void MuonTruthDecorationAlg::addHitCounts(const EventContext& ctx, xAOD::TruthParticle& truthParticle,
                                              MuonTruthDecorationAlg::ChamberIdMap& ids) const {
        int barcode = truthParticle.barcode();

        std::vector<unsigned int> nprecHitsPerChamberLayer;
        nprecHitsPerChamberLayer.resize(Muon::MuonStationIndex::ChIndexMax);
        std::vector<unsigned int> nphiHitsPerChamberLayer;
        nphiHitsPerChamberLayer.resize(Muon::MuonStationIndex::PhiIndexMax);
        std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
        ntrigEtaHitsPerChamberLayer.resize(Muon::MuonStationIndex::PhiIndexMax);
        ATH_MSG_DEBUG("addHitCounts: barcode " << barcode);
        // loop over detector technologies
        for (SG::ReadHandle<PRD_MultiTruthCollection>& col : m_PRD_TruthNames.makeHandles(ctx)) {
            if (!col.isPresent()) {
                ATH_MSG_DEBUG("PRD_MultiTruthCollection " << col.name() << " not in StoreGate");
                continue;
            } else
                ATH_MSG_DEBUG("PRD_MultiTruthCollection " << col.name() << " in StoreGate");
            // loop over trajectories
            for (const auto& trajectory : *col) {
                // check if gen particle same as input
                if ((HepMC::barcode(trajectory.second)) % m_barcodeOffset != barcode) continue;

                const Identifier& id = trajectory.first;
                bool measPhi = m_idHelperSvc->measuresPhi(id);
                bool isTgc = m_idHelperSvc->isTgc(id);
                Muon::MuonStationIndex::ChIndex chIndex = !isTgc ? m_idHelperSvc->chamberIndex(id) : Muon::MuonStationIndex::ChUnknown;

                // add identifier to map
                if (isTgc) {  // TGCS should be added to both EIL and EIS
                    Muon::MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(id);
                    if (index == Muon::MuonStationIndex::T4) {
                        ids[Muon::MuonStationIndex::EIS].push_back(id);
                        ids[Muon::MuonStationIndex::EIL].push_back(id);
                    } else {
                        ids[Muon::MuonStationIndex::EMS].push_back(id);
                        ids[Muon::MuonStationIndex::EML].push_back(id);
                    }
                } else {
                    ids[chIndex].push_back(id);
                }

                if (m_idHelperSvc->issTgc(id)) {
                    int index = m_idHelperSvc->phiIndex(id);
                    if (index >= 0) {
                      if (measPhi)
                        ++nphiHitsPerChamberLayer.at(index);
                      else
                        ++ntrigEtaHitsPerChamberLayer.at(index);
                    }
                } else if (m_idHelperSvc->isMM(id)) {
                  if (chIndex >= 0) {
                    ++nprecHitsPerChamberLayer.at(chIndex);
                  }
                } else if (m_idHelperSvc->isTrigger(id)) {
                    int index = m_idHelperSvc->phiIndex(id);
                    if (index >= 0) {
                      if (measPhi)
                        ++nphiHitsPerChamberLayer.at(index);
                      else
                        ++ntrigEtaHitsPerChamberLayer.at(index);
                    }
                } else {
                    if (measPhi) {
                        Muon::MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(id);
                        if (index >= 0) {
                          ++nphiHitsPerChamberLayer.at(index);
                        }
                    } else {
                      if (chIndex >= 0) {
                        ++nprecHitsPerChamberLayer.at(chIndex);
                      }
                    }
                }
            }
        }

        uint8_t innerSmallHits = nprecHitsPerChamberLayer[Muon::MuonStationIndex::BIS] +
                                 nprecHitsPerChamberLayer[Muon::MuonStationIndex::EIS] +
                                 nprecHitsPerChamberLayer[Muon::MuonStationIndex::CSS];
        uint8_t innerLargeHits = nprecHitsPerChamberLayer[Muon::MuonStationIndex::BIL] +
                                 nprecHitsPerChamberLayer[Muon::MuonStationIndex::EIL] +
                                 nprecHitsPerChamberLayer[Muon::MuonStationIndex::CSL];
        uint8_t middleSmallHits =
            nprecHitsPerChamberLayer[Muon::MuonStationIndex::BMS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EMS];
        uint8_t middleLargeHits =
            nprecHitsPerChamberLayer[Muon::MuonStationIndex::BML] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EML];
        uint8_t outerSmallHits =
            nprecHitsPerChamberLayer[Muon::MuonStationIndex::BOS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EOS];
        uint8_t outerLargeHits =
            nprecHitsPerChamberLayer[Muon::MuonStationIndex::BML] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EOL];
        uint8_t extendedSmallHits =
            nprecHitsPerChamberLayer[Muon::MuonStationIndex::EES] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::BEE];
        uint8_t extendedLargeHits = nprecHitsPerChamberLayer[Muon::MuonStationIndex::EEL];

        uint8_t phiLayer1Hits = nphiHitsPerChamberLayer[Muon::MuonStationIndex::BM1] + nphiHitsPerChamberLayer[Muon::MuonStationIndex::T4] +
                                nphiHitsPerChamberLayer[Muon::MuonStationIndex::CSC] +
                                nphiHitsPerChamberLayer[Muon::MuonStationIndex::STGC1] +
                                nphiHitsPerChamberLayer[Muon::MuonStationIndex::STGC2];
        uint8_t phiLayer2Hits = nphiHitsPerChamberLayer[Muon::MuonStationIndex::BM2] + nphiHitsPerChamberLayer[Muon::MuonStationIndex::T1];
        uint8_t phiLayer3Hits = nphiHitsPerChamberLayer[Muon::MuonStationIndex::BO1] + nphiHitsPerChamberLayer[Muon::MuonStationIndex::T2];
        uint8_t phiLayer4Hits = nphiHitsPerChamberLayer[Muon::MuonStationIndex::BO2] + nphiHitsPerChamberLayer[Muon::MuonStationIndex::T3];

        uint8_t etaLayer1Hits =
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BM1] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T4] +
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::CSC] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::STGC1] +
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::STGC2];
        uint8_t etaLayer2Hits =
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BM2] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T1];
        uint8_t etaLayer3Hits =
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BO1] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T2];
        uint8_t etaLayer4Hits =
            ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BO2] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T3];

        uint8_t nprecLayers = 0;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::BIS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::BIL] > 3)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::BMS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::BML] > 2)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::BOS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::BOL] > 2)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::EIS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EIL] > 3)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::EMS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EML] > 2)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::EOS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EOL] > 2)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::EES] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::EEL] > 3)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::CSS] + nprecHitsPerChamberLayer[Muon::MuonStationIndex::CSL] > 2)
            ++nprecLayers;
        if (nprecHitsPerChamberLayer[Muon::MuonStationIndex::BEE] > 3) ++nprecLayers;

        uint8_t nphiLayers = 0;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::BM1] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::BM2] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::BO1] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::BO2] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::T1] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::T2] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::T3] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::T4] > 0) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::CSC] > 2) ++nphiLayers;
        if (nphiHitsPerChamberLayer[Muon::MuonStationIndex::STGC1] + nphiHitsPerChamberLayer[Muon::MuonStationIndex::STGC2] > 3)
            ++nphiLayers;

        uint8_t ntrigEtaLayers = 0;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BM1] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BM2] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BO1] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::BO2] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T1] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T2] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T3] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::T4] > 0) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::CSC] > 2) ++ntrigEtaLayers;
        if (ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::STGC1] + ntrigEtaHitsPerChamberLayer[Muon::MuonStationIndex::STGC2] > 3)
            ++ntrigEtaLayers;

        // copy hit counts onto TruthParticle
        truthParticle.auxdata<uint8_t>("nprecLayers") = nprecLayers;
        truthParticle.auxdata<uint8_t>("nphiLayers") = nphiLayers;
        truthParticle.auxdata<uint8_t>("ntrigEtaLayers") = ntrigEtaLayers;
        truthParticle.auxdata<uint8_t>("innerSmallHits") = innerSmallHits;
        truthParticle.auxdata<uint8_t>("innerLargeHits") = innerLargeHits;
        truthParticle.auxdata<uint8_t>("middleSmallHits") = middleSmallHits;
        truthParticle.auxdata<uint8_t>("middleLargeHits") = middleLargeHits;
        truthParticle.auxdata<uint8_t>("outerSmallHits") = outerSmallHits;
        truthParticle.auxdata<uint8_t>("outerLargeHits") = outerLargeHits;
        truthParticle.auxdata<uint8_t>("extendedSmallHits") = extendedSmallHits;
        truthParticle.auxdata<uint8_t>("extendedLargeHits") = extendedLargeHits;

        truthParticle.auxdata<uint8_t>("phiLayer1Hits") = phiLayer1Hits;
        truthParticle.auxdata<uint8_t>("phiLayer2Hits") = phiLayer2Hits;
        truthParticle.auxdata<uint8_t>("phiLayer3Hits") = phiLayer3Hits;
        truthParticle.auxdata<uint8_t>("phiLayer4Hits") = phiLayer4Hits;

        truthParticle.auxdata<uint8_t>("etaLayer1Hits") = etaLayer1Hits;
        truthParticle.auxdata<uint8_t>("etaLayer2Hits") = etaLayer2Hits;
        truthParticle.auxdata<uint8_t>("etaLayer3Hits") = etaLayer3Hits;
        truthParticle.auxdata<uint8_t>("etaLayer4Hits") = etaLayer4Hits;

        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Precision layers " << static_cast<int>(nprecLayers) << " phi layers " << static_cast<int>(nphiLayers)
                                              << " triggerEta layers " << static_cast<int>(ntrigEtaLayers));

            if (nprecLayers > 0) {
                msg(MSG::VERBOSE) << " Precision chambers ";

                for (int index = 0; index < static_cast<int>(nprecHitsPerChamberLayer.size()); ++index) {
                    if (nprecHitsPerChamberLayer[index] > 0)
                        msg(MSG::VERBOSE) << " " << Muon::MuonStationIndex::chName(static_cast<Muon::MuonStationIndex::ChIndex>(index))
                                          << " hits " << nprecHitsPerChamberLayer[index];
                }
            }
            if (nphiLayers > 0) {
                msg(MSG::VERBOSE) << endmsg << " Phi chambers ";
                for (int index = 0; index < static_cast<int>(nphiHitsPerChamberLayer.size()); ++index) {
                    if (nphiHitsPerChamberLayer[index] > 0)
                        msg(MSG::VERBOSE) << " " << Muon::MuonStationIndex::phiName(static_cast<Muon::MuonStationIndex::PhiIndex>(index))
                                          << " hits " << nphiHitsPerChamberLayer[index];
                }
            }

            if (ntrigEtaLayers > 0) {
                msg(MSG::VERBOSE) << endmsg << " Trigger Eta ";
                for (int index = 0; index < static_cast<int>(ntrigEtaHitsPerChamberLayer.size()); ++index) {
                    if (ntrigEtaHitsPerChamberLayer[index] > 0)
                        msg(MSG::VERBOSE) << " " << Muon::MuonStationIndex::phiName(static_cast<Muon::MuonStationIndex::PhiIndex>(index))
                                          << " hits " << ntrigEtaHitsPerChamberLayer[index];
                }
            }
            msg(MSG::VERBOSE) << endmsg;
        }
    }

    void MuonTruthDecorationAlg::addHitIDVectors(xAOD::TruthParticle& truthParticle,
                                                 const MuonTruthDecorationAlg::ChamberIdMap& ids) const {
        
        std::vector<unsigned long long>& mdtTruthHits = truthParticle.auxdata<std::vector<unsigned long long> >("truthMdtHits");
        std::vector<unsigned long long>& tgcTruthHits = truthParticle.auxdata<std::vector<unsigned long long> >("truthTgcHits");
        std::vector<unsigned long long>& rpcTruthHits = truthParticle.auxdata<std::vector<unsigned long long> >("truthRpcHits");
        
        std::vector<unsigned long long> stgcTruthHits;
        std::vector<unsigned long long> cscTruthHits;
        std::vector<unsigned long long> mmTruthHits;

        // loop over chamber layers
        int nEI = 0, nEM = 0;
        for (const auto& lay : ids) {
            // loop over hits
            if (lay.first == Muon::MuonStationIndex::EIS || lay.first == Muon::MuonStationIndex::EIL) nEI++;
            if (lay.first == Muon::MuonStationIndex::EMS || lay.first == Muon::MuonStationIndex::EML) nEM++;
            for (const auto& id : lay.second) {
                if (m_idHelperSvc->isMdt(id))
                    mdtTruthHits.push_back(id.get_compact());
                else if (m_idHelperSvc->isCsc(id))
                    cscTruthHits.push_back(id.get_compact());
                else if (m_idHelperSvc->isTgc(id)) {
                    if ((lay.first == Muon::MuonStationIndex::EIS || lay.first == Muon::MuonStationIndex::EIL) && nEI > 1)
                        continue;  // otherwise we double-count
                    if ((lay.first == Muon::MuonStationIndex::EMS || lay.first == Muon::MuonStationIndex::EML) && nEM > 1)
                        continue;  // otherwise we double-count
                    tgcTruthHits.push_back(id.get_compact());
                } else if (m_idHelperSvc->issTgc(id))
                    stgcTruthHits.push_back(id.get_compact());
                else if (m_idHelperSvc->isRpc(id))
                    rpcTruthHits.push_back(id.get_compact());
                else if (m_idHelperSvc->isMM(id))
                    mmTruthHits.push_back(id.get_compact());
            }
        }
        if (m_idHelperSvc->hasCSC()) {
            truthParticle.auxdata<std::vector<unsigned long long> >("truthCscHits") = cscTruthHits;
        }
        if (m_idHelperSvc->hasSTgc()) {
            truthParticle.auxdata<std::vector<unsigned long long> >("truthStgcHits") = stgcTruthHits;
        }
        if (m_idHelperSvc->hasMM()) {
            truthParticle.auxdata<std::vector<unsigned long long> >("truthMMHits") = mmTruthHits;
        }
        ATH_MSG_VERBOSE("Added " << mdtTruthHits.size() << " mdt truth hits, " << cscTruthHits.size() << " csc truth hits, "
                                 << rpcTruthHits.size() << " rpc truth hits, and " << tgcTruthHits.size() << " tgc truth hits");
    }

}  // namespace Muon
