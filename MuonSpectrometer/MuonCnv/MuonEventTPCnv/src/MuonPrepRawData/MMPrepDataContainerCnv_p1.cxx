/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataContainerCnv_p1.h"

#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepData_p1.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

StatusCode Muon::MMPrepDataContainerCnv_p1::initialize(MsgStream& log) {
    // Do not initialize again:
    m_isInitialized = true;

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    // get StoreGate service
    StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::FATAL << "StoreGate service not found !" << endmsg;
        return StatusCode::FAILURE;
    }

    // get DetectorStore service
    StoreGateSvc* detStore;
    sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
    }

    // Get the helper from the detector store
    sc = detStore->retrieve(m_MMId);
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get MM IdHelper !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the MM IdHelper." << endmsg;
    }

    if (m_eventCnvTool.retrieve().isFailure()) {
        log << MSG::FATAL << "Could not get DetectorDescription manager" << endmsg;
        return StatusCode::FAILURE;
    }

    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
    return StatusCode::SUCCESS;
}
const MuonGM::MMReadoutElement* Muon::MMPrepDataContainerCnv_p1::getReadOutElement(const Identifier& id ) const {
    const Trk::ITrkEventCnvTool* cnv_tool = m_eventCnvTool->getCnvTool(id);
    if (!cnv_tool) return nullptr; 
    return dynamic_cast<const MuonGM::MMReadoutElement*>(cnv_tool->getDetectorElement(id));
}
void Muon::MMPrepDataContainerCnv_p1::transToPers(const Muon::MMPrepDataContainer* transCont, Muon::MMPrepDataContainer_p1* persCont,
                                                  MsgStream& log) {
    if (!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize MMPrepDataContainerCnv_p1 " << endmsg;
        }
    }
    // The transient model has a container holding collections (= 64 modules in MmIdHelper
    // each consisting of two MicroMegas quadruplets in reality) and the collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all PRDs
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels.
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the collection.

    MMPrepDataCnv_p1 chanCnv;
    Muon::MMPrepDataContainer::const_iterator it_Coll = transCont->begin();
    Muon::MMPrepDataContainer::const_iterator it_CollEnd = transCont->end();
    unsigned int pcollEnd = 0;    // index to end
    int numColl = transCont->numberOfCollections();
    persCont->m_collections.resize(numColl);
    if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << " Preparing " << persCont->m_collections.size() << "collections" << endmsg; }

    for (unsigned int pcollIndex = 0; it_Coll != it_CollEnd; ++pcollIndex, ++it_Coll) {
        const Muon::MMPrepDataCollection& collection = (**it_Coll);
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "Coll hash for " << pcollIndex << ": " << collection.identifyHash() << endmsg;
        }
        // Add in new collection
        Muon::MuonPRD_Collection_p2& pcollection = persCont->m_collections[pcollIndex];  // get ref to collection we're going to fill

        pcollEnd += collection.size();

        pcollection.m_hashId = collection.identifyHash();  // the collection hash is the module hash in MmIdHelper
        pcollection.m_id = collection.identify().get_identifier32().get_compact();
        pcollection.m_size = collection.size();
        // Add in channels
        persCont->m_prds.reserve(pcollEnd);
        persCont->m_prdDeltaId.reserve(pcollEnd);

        for (unsigned int i = 0; i < collection.size(); ++i) {
            const MMPrepData* chan = collection[i];  // channel being converted
            MMPrepData_p1 pchan{};                   // persistent version to fill
            const Identifier chan_id = chan->identify();
            chanCnv.transToPers(chan, &pchan, log);  // convert from MMPrepData to MMPrepData_p1

            // persCont->m_prdDeltaId is of data type unsigned short, thus we need to encode the channel (starting from the
            // collection (module) is in contained) into 16 bits, we do it by storing multilayer, gasGap and channel
            int multilayer = (m_MMId->multilayer(chan_id) - 1);               // ranges between 1-2 (1bit)
            int gasGap = (m_MMId->gasGap(chan_id) - 1);                       // ranges between 1-4 (2bits)
            int channel = (m_MMId->channel(chan_id) - m_MMId->channelMin());  // ranges between 1-5012 (13bits)

            // created an unsigned short and store multilayer, gasGap and channel by bit-shifts
            unsigned short diff = (channel << 3 | gasGap << 1 | multilayer);
            if (log.level() <= MSG::DEBUG) {
                log << MSG::DEBUG << "Translated id=" << chan_id.get_compact() << " (multilayer=" << multilayer << ", gasGap=" << gasGap
                    << ", channel=" << channel << ") into diff=" << diff << endmsg;
            }

            persCont->m_prdDeltaId.emplace_back(diff);  // store delta identifiers, rather than full identifiers

            const MuonGM::MMReadoutElement* ele_from_mgr = getReadOutElement(chan_id);
            if (log.level() <= MSG::WARNING && chan->detectorElement()->identify() != ele_from_mgr->identify()) {
                log << MSG::WARNING << "DE from det manager (" << m_MMId->print_to_string(ele_from_mgr->identify())
                    << ") does not match that from PRD (" << m_MMId->print_to_string(chan->detectorElement()->identify()) << ") for PRD "
                    << m_MMId->print_to_string(chan_id) << endmsg;
            }
            persCont->m_prds.emplace_back(std::move(pchan));
        }
    }
    log << MSG::DEBUG << " ***  Writing MMPrepDataContainer ***" << endmsg;
}

void Muon::MMPrepDataContainerCnv_p1::persToTrans(const Muon::MMPrepDataContainer_p1* persCont, Muon::MMPrepDataContainer* transCont,
                                                  MsgStream& log) {
    // The transient model has a container holding collections (= 64 modules in MmIdHelper
    // each consisting of two MicroMegas quadruplets in reality) and the collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels.
    //
    // So here we loop over all collection and extract their channels
    // from the vector.

    Muon::MMPrepDataCollection* coll = nullptr;

    MMPrepDataCnv_p1 chanCnv;
    unsigned int pchanIndex(0);  // position within persCont->m_prds. Incremented inside innermost loop
    unsigned int pCollEnd = persCont->m_collections.size();
    if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << " Reading " << pCollEnd << "Collections" << endmsg; }
    for (unsigned int pcollIndex = 0; pcollIndex < pCollEnd; ++pcollIndex) {
        const Muon::MuonPRD_Collection_p2& pcoll = persCont->m_collections[pcollIndex];
        IdentifierHash collIDHash(pcoll.m_hashId);
        if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << "Coll hash for " << pcollIndex << ": " << collIDHash << endmsg; }

        coll = new Muon::MMPrepDataCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));

        unsigned int pchanEnd = pchanIndex + pcoll.m_size;
        unsigned int chanIndex = 0;  // transient index

        coll->reserve(pcoll.m_size);
        // Fill with channels
        for (; pchanIndex < pchanEnd; ++pchanIndex, ++chanIndex) {
            const MMPrepData_p1* pchan = &(persCont->m_prds[pchanIndex]);

            /// get the cluster Identifier from the collection Identifier and the persCont->m_prdDeltaId (unsigned short)
            unsigned short diff = persCont->m_prdDeltaId[pchanIndex];
            // we need to redo the bit-shift to retrieve channel, gasGap and multilayer
            int channel = (diff >> 3);
            int gasGap = (3 & (diff >> 1));
            int multilayer = (1 & diff);
            Identifier clusId = m_MMId->channelID(Identifier(pcoll.m_id), multilayer + 1, gasGap + 1, channel + m_MMId->channelMin());
            if (log.level() <= MSG::DEBUG) {
                log << MSG::DEBUG << "Diff of " << diff << " translated into multilayer=" << multilayer << ", gasGap=" << gasGap
                    << ", channel=" << channel << " -> id=" << clusId.get_compact() << endmsg;
            }

            const MuonGM::MMReadoutElement* detEl = getReadOutElement(clusId);
            if (!detEl) {
                if (log.level() <= MSG::WARNING) {
                    log << MSG::WARNING << "Muon::MMPrepDataContainerCnv_p1::persToTrans: could not get valid det element for PRD with id="
                        << m_MMId->show_to_string(clusId) << ". Skipping." << endmsg;
                }
                continue;
            }

            auto chan = std::make_unique<MMPrepData>(chanCnv.createMMPrepData(pchan, clusId, detEl, log));

            if (!m_MMId->valid(chan->identify())) {
                log << MSG::WARNING << "MM PRD has invalid Identifier of " << chan->identify().get_compact()
                    << " - are you sure you have the correct geometry loaded, and NSW enabled?" << endmsg;
            }

            // chanCnv.persToTrans(pchan, chan, log); // Fill chan with data from pchan FIXME! Put this back once diff is sane.
            if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << "chan identify(): " << chan->identify().get_compact() << endmsg; }

            chan->setHashAndIndex(collIDHash, chanIndex);
            coll->push_back(std::move(chan));
        }

        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) { throw std::runtime_error("Failed to add collection to Identifiable Container"); }
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int)collIDHash << " / "
                << coll->identify().get_compact() << ", added to Identifiable container." << endmsg;
        }
    }
    if (log.level() <= MSG::DEBUG) { log << MSG::DEBUG << " ***  Reading MMPrepDataContainer ***" << endmsg; }
}

Muon::MMPrepDataContainer* Muon::MMPrepDataContainerCnv_p1::createTransient(const Muon::MMPrepDataContainer_p1* persObj, MsgStream& log) {
    if (!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize MMPrepDataContainerCnv_p1 " << endmsg;
            return nullptr;
        }
    }
    std::unique_ptr<Muon::MMPrepDataContainer> trans = std::make_unique<Muon::MMPrepDataContainer>(m_MMId->module_hash_max());
    persToTrans(persObj, trans.get(), log);
    return (trans.release());
}
