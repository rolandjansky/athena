/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "CscRdoToCscPrepDataTool.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

CscRdoToCscPrepDataTool::CscRdoToCscPrepDataTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent) {}

StatusCode CscRdoToCscPrepDataTool::initialize() {
    ATH_MSG_VERBOSE("Starting init");
    ATH_CHECK(CscRdoToCscPrepDataToolCore::initialize());
    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepDataTool::decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) const {
    // WARNING : Trigger Part is not finished.
    unsigned int sizeVectorRequested = givenIdhs.size();
    ATH_MSG_DEBUG("decode for " << sizeVectorRequested << " offline collections called");

    // clear output vector of selected data collections containing data
    decodedIdhs.clear();

    m_outputCollection = nullptr;
    if (!evtStore()->contains<Muon::CscStripPrepDataContainer>(m_outputCollectionKey.key())) {
        /// record the container in storeGate
        SG::WriteHandle<Muon::CscStripPrepDataContainer> outputHandle(m_outputCollectionKey);
        ATH_CHECK(outputHandle.record(std::make_unique<Muon::CscStripPrepDataContainer>(m_idHelperSvc->cscIdHelper().module_hash_max())));

        if (!outputHandle.isValid()) {
            ATH_MSG_FATAL("Could not record container of CSC PrepData Container at " << m_outputCollectionKey.key());
            return StatusCode::FAILURE;
        }
        m_outputCollection = outputHandle.ptr();

        if (sizeVectorRequested == 0) {
            m_fullEventDone = true;
            ATH_MSG_DEBUG("decoding the entire event ");
        } else {
            m_fullEventDone = false;
        }

    } else {
        ATH_MSG_DEBUG("CSC PrepData Container is already in StoreGate ");
        const Muon::CscStripPrepDataContainer* outputCollection_c = nullptr;
        ATH_CHECK(evtStore()->retrieve(outputCollection_c, m_outputCollectionKey.key()));

        m_outputCollection = const_cast<Muon::CscStripPrepDataContainer*>(outputCollection_c);
        if (m_fullEventDone) {
            ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do");
            return StatusCode::SUCCESS;
        }

        if (sizeVectorRequested == 0) m_fullEventDone = true;
    }

    // retrieve the pointer to the RDO container
    // this will just get the pointer from memory if the container is already recorded in SG
    // or
    // will activate the TP converter for reading from pool root the RDO container and recording it in SG

    auto rdoContainerHandle = SG::makeHandle(m_rdoContainerKey);
    if (!rdoContainerHandle.isValid()) {
        ATH_MSG_WARNING("No CSC RDO container in StoreGate!");
        return StatusCode::SUCCESS;
    }
    const CscRawDataContainer* rdoContainer = rdoContainerHandle.cptr();

    ATH_MSG_DEBUG("Retrieved " << rdoContainer->size() << " CSC RDOs.");
    // here the RDO container is in SG and its pointer rdoContainer is initialised
    // decoding
    if (sizeVectorRequested) {
        // seeded decoding
        for (unsigned int i = 0; i < sizeVectorRequested; ++i) {
            if (decodeImpl(m_outputCollection, rdoContainer, givenIdhs[i], decodedIdhs).isFailure()) {
                ATH_MSG_ERROR("Unable to decode CSC RDO " << i << "th into CSC PrepRawData");
                return StatusCode::FAILURE;
            }
        }
    } else {
        // unseeded decoding
        if (decodeImpl(m_outputCollection, rdoContainer, decodedIdhs).isFailure()) {
            ATH_MSG_ERROR("Unable to decode CSC RDO ");
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}

void CscRdoToCscPrepDataTool::printPrepData() const { return printPrepDataImpl(m_outputCollection); }

//*****************************************
//************** Process for the givenId EF Filter case...
/// This decode function is for single-thread running only
StatusCode CscRdoToCscPrepDataTool::decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdoContainer,
                                               IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIdhs) const {
    IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
    const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

    // if CSC decoding is switched off stop here
    if (!m_decodeData) {
        ATH_MSG_DEBUG("Stored empty container; Decoding CSC RDO into CSC PrepRawData is switched off");
        return StatusCode::SUCCESS;
    }

    // These collections can be empty for the trigger
    if (!outputCollection || outputCollection->size() == 0) {
        ATH_MSG_DEBUG("Stored empty collection.");
        return StatusCode::SUCCESS;
    }

    // identifiers of collections already decoded and stored in the container will be skipped
    if (outputCollection->numberOfCollections() != 0) {
        decodedIdhs.push_back(givenHashId);
        ATH_MSG_DEBUG("A collection already exists in the container for offline id hash. " << (int)givenHashId);
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Decoding CSC RDO into CSC PrepRawData");
    /// create the CSC RDO decoder

    //**********************************************
    // retrieve specific collection for the givenID
    uint16_t idColl = 0xffff;
    m_cabling->hash2CollectionId(givenHashId, idColl);
    const CscRawDataCollection* rawCollection = rdoContainer->indexFindPtr(idColl);
    if (nullptr == rawCollection) {
        // unsigned int coll_hash = idColl;
        ATH_MSG_DEBUG("Specific CSC RDO collection retrieving failed for collection hash = " << idColl);
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Retrieved " << rawCollection->size() << " CSC RDOs.");
    //************************************************
    Identifier oldId;
    CscStripPrepDataCollection* collection = nullptr;
    IdentifierHash cscHashId;

    unsigned int samplingTime = rawCollection->rate();
    unsigned int numSamples = rawCollection->numSamples();
    bool samplingPhase = rawCollection->samplingPhase();
    std::vector<float> charges;
    charges.reserve(4);
    std::vector<uint16_t> samples;
    samples.reserve(4);

    if (int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
        ATH_MSG_WARNING(" CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! ");
    }

    /** for each Rdo, loop over RawData, converter RawData to PrepRawData
        retrieve/create PrepRawData collection, and insert PrepRawData into collection */
    CscRawDataCollection::const_iterator itD = rawCollection->begin();
    CscRawDataCollection::const_iterator itD_e = rawCollection->end();

    for (; itD != itD_e; ++itD) {
        const CscRawData* data = (*itD);
        uint16_t width = data->width();
        uint16_t totalSamples = (data->samples()).size();

        Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data, &m_idHelperSvc->cscIdHelper());
        uint32_t hashOffset = data->hashId();

        ATH_MSG_DEBUG(" Size of online cluster in this RawData: "
                      << " Width = " << width << " Samples = " << totalSamples << " stationId : " << stationId
                      << "  hashOffset : " << hashOffset);

        if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
            ATH_MSG_WARNING("Unable to get CSC digiti collection hash id "
                            << "context begin_index = " << cscContext.begin_index() << " context end_index  = " << cscContext.end_index()
                            << " the identifier is ");
            stationId.show();
        }

        if (oldId != stationId) {
            auto newCollection = std::make_unique<CscStripPrepDataCollection>(cscHashId);
            newCollection->setIdentifier(stationId);
            collection = newCollection.get();
            if (outputCollection->addCollection(newCollection.release(), cscHashId).isFailure()) {
                ATH_MSG_ERROR("Couldn't add CscStripPrepdataCollection with key=" << (unsigned int)cscHashId << " to container!");
                return StatusCode::FAILURE;
            }
            decodedIdhs.push_back(cscHashId);  // Record that this collection contains data
            oldId = stationId;
        }

        for (unsigned int j = 0; j < width; ++j) {
            const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, &m_idHelperSvc->cscIdHelper(), j);
            ATH_MSG_DEBUG("        LOOP over width  " << j << " " << channelId);

            const CscReadoutElement* descriptor = muDetMgr->getCscReadoutElement(channelId);
            // calculate local positions on the strip planes
            if (!descriptor) {
                ATH_MSG_WARNING("Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                                                          << " Skipping channel ");
                continue;
            } else if (!descriptor->containsId(channelId)) {
                ATH_MSG_WARNING("Identifier from the cabling service <"
                                << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                                << "> inconsistent with the geometry of detector element <"
                                << m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify()) << ">  =>>ignore this hit");
                continue;
            }

            float timeOfFirstSample = 0.0;
            bool extractSamples = data->samples(j, numSamples, samples);
            if (!extractSamples) {
                ATH_MSG_WARNING("Unable to extract samples for strip " << j << " Online Cluster width = " << width
                                                                       << " for number of Samples = " << numSamples << " continuing ...");
                continue;
            }

            //      uint32_t stripHash = hashOffset+j;
            IdentifierHash stripHash;
            if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
                ATH_MSG_WARNING("Unable to get CSC strip hash id");
                channelId.show();
            }

            bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
            if (!adctocharge) {
                ATH_MSG_WARNING(" CSC conversion ADC to Charge failed "
                                << "CSC PrepData not build ... skipping ");
                continue;
            }
            if (samples.size() >= 4)
                ATH_MSG_DEBUG("ADC: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) << " " << samples[0] << " " << samples[1]
                                      << " " << samples[2] << " " << samples[3] << " Charges: "
                                      << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3]);

            int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

            Amg::Vector2D localWirePos1(descriptor->xCoordinateInTrackingFrame(channelId), 0.);

            int chamberLayer = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
            float stripWidth = descriptor->cathodeReadoutPitch(chamberLayer, measuresPhi);
            double errPos = stripWidth / sqrt(12.0);

            AmgSymMatrix(2) covariance;
            covariance.setIdentity();
            covariance *= errPos * errPos;
            auto errClusterPos = Amg::MatrixX(covariance);

            /** new CscStripPrepRawData */
            CscStripPrepData* newPrepData = new CscStripPrepData(channelId, cscHashId, localWirePos1, std::move(errClusterPos), descriptor,
                                                                 charges, timeOfFirstSample, samplingTime);

            if (samplingPhase) newPrepData->set_samplingPhase();
            newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
            collection->push_back(newPrepData);
        }
    }

    return StatusCode::SUCCESS;
}

//************** Process for all in case of Offline
/// This decode function is for single-thread running only
StatusCode CscRdoToCscPrepDataTool::decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdoContainer,
                                               std::vector<IdentifierHash>& decodedIdhs) const {
    typedef CscRawDataContainer::const_iterator collection_iterator;

    IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
    const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

    // if CSC decoding is switched off stop here
    if (!m_decodeData) {
        ATH_MSG_DEBUG("Stored empty container. "
                      << "Decoding CSC RDO into CSC PrepRawData is switched off");
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Decoding CSC RDO into CSC PrepRawData");

    collection_iterator rdoColl = rdoContainer->begin();
    collection_iterator lastRdoColl = rdoContainer->end();
    std::vector<float> charges;
    charges.reserve(4);
    std::vector<uint16_t> samples;
    samples.reserve(4);

    Identifier oldId;
    CscStripPrepDataCollection* collection = nullptr;
    IdentifierHash cscHashId;
    for (; rdoColl != lastRdoColl; ++rdoColl) {
        if ((*rdoColl)->size() > 0) {
            ATH_MSG_DEBUG(" Number of RawData in this rdo " << (*rdoColl)->size());

            const CscRawDataCollection* cscCollection = *rdoColl;
            unsigned int samplingTime = cscCollection->rate();
            unsigned int numSamples = cscCollection->numSamples();
            bool samplingPhase = cscCollection->samplingPhase();

            if (int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
                ATH_MSG_WARNING(" CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! ");
            }
            /** for each Rdo, loop over RawData, converter RawData to PrepRawData
                retrieve/create PrepRawData collection, and insert PrepRawData into collection */
            CscRawDataCollection::const_iterator itD = cscCollection->begin();
            CscRawDataCollection::const_iterator itD_e = cscCollection->end();

            for (; itD != itD_e; ++itD) {
                const CscRawData* data = (*itD);
                uint16_t width = data->width();
                uint16_t totalSamples = (data->samples()).size();

                Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data, &m_idHelperSvc->cscIdHelper());
                uint32_t hashOffset = data->hashId();

                ATH_MSG_DEBUG("DecodeAll*Size of online cluster in this RawData: "
                              << " Width = " << width << " Samples = " << totalSamples << " stationId : " << stationId
                              << "  hashOffset : " << hashOffset);

                if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
                    ATH_MSG_WARNING("Unable to get CSC digiti collection hash id "
                                    << "context begin_index = " << cscContext.begin_index()
                                    << " context end_index  = " << cscContext.end_index() << " the identifier is ");
                    stationId.show();
                }

                if (oldId != stationId) {
                    auto newCollection = std::make_unique<CscStripPrepDataCollection>(cscHashId);
                    newCollection->setIdentifier(stationId);
                    collection = newCollection.get();
                    if (outputCollection->addCollection(newCollection.release(), cscHashId).isFailure()) {
                        ATH_MSG_ERROR("Couldn't add CscStripPrepdataCollection with key=" << (unsigned int)cscHashId << " to container!");
                        return StatusCode::FAILURE;
                    }
                    decodedIdhs.push_back(cscHashId);  // Record that this collection contains data

                    oldId = stationId;
                }

                for (unsigned int j = 0; j < width; ++j) {
                    const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, &m_idHelperSvc->cscIdHelper(), j);
                    ATH_MSG_DEBUG("DecodeAll**LOOP over width  " << j << " " << channelId);

                    const CscReadoutElement* descriptor = muDetMgr->getCscReadoutElement(channelId);
                    // calculate local positions on the strip planes
                    if (!descriptor) {
                        ATH_MSG_WARNING("Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                                                                  << " Skipping channel ");
                        continue;
                    } else if (!descriptor->containsId(channelId)) {
                        ATH_MSG_WARNING("Identifier from the cabling service <"
                                        << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                                        << "> inconsistent with the geometry of detector element <"
                                        << m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify()) << ">  =>>ignore this hit");
                        continue;
                    }

                    float timeOfFirstSample = 0.0;
                    bool extractSamples = data->samples(j, numSamples, samples);
                    if (!extractSamples) {
                        ATH_MSG_WARNING("Unable to extract samples for strip " << j << " Online Cluster width = " << width
                                                                               << " for number of Samples = " << numSamples
                                                                               << " continuing ...");
                        continue;
                    }

                    IdentifierHash stripHash;
                    if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
                        ATH_MSG_WARNING("Unable to get CSC strip hash id");
                        channelId.show();
                    }

                    Identifier channelIdFromHash;
                    m_idHelperSvc->cscIdHelper().get_id(stripHash, channelIdFromHash, &cscContext);

                    bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
                    if (!adctocharge) {
                        ATH_MSG_WARNING(" CSC conversion ADC to Charge failed "
                                        << "CSC PrepData not build ... skipping ");
                        continue;
                    }
                    if (samples.size() >= 4)
                        ATH_MSG_DEBUG("DecodeAll*** ADC: "
                                      << m_idHelperSvc->cscIdHelper().show_to_string(channelId) << " " << (int)stripHash << " "
                                      << m_idHelperSvc->cscIdHelper().show_to_string(channelIdFromHash) << " " << samples[0] << " "
                                      << samples[1] << " " << samples[2] << " " << samples[3] << " Charges: "
                                      << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3]);
                    if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
                        ATH_MSG_WARNING("Unable to get CSC hash id from CSC RDO collection "
                                        << "context begin_index = " << cscContext.begin_index()
                                        << " context end_index  = " << cscContext.end_index() << " the identifier is ");
                        stationId.show();
                    }

                    // Check if this strip is already decoded.. Then we don't have to decode it again
                    bool IsThisStripDecoded = 0;
                    for (CscStripPrepDataCollection::const_iterator idig = collection->begin(); idig != collection->end(); ++idig) {
                        const CscStripPrepData& dig = **idig;
                        Identifier did = dig.identify();
                        if (did == channelId) {
                            IsThisStripDecoded = 1;
                            break;
                        }
                    }
                    if (IsThisStripDecoded) continue;

                    int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

                    Amg::Vector2D localWirePos1(descriptor->xCoordinateInTrackingFrame(channelId), 0.);

                    int chamberLayer = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
                    float stripWidth = descriptor->cathodeReadoutPitch(chamberLayer, measuresPhi);
                    double errPos = stripWidth / sqrt(12.0);

                    AmgSymMatrix(2) covariance;
                    covariance.setIdentity();
                    covariance *= errPos * errPos;
                    auto errClusterPos = Amg::MatrixX(covariance);

                    /** new CscPrepRawData */
                    CscStripPrepData* newPrepData = new CscStripPrepData(channelId, cscHashId, localWirePos1, std::move(errClusterPos),
                                                                         descriptor, charges, timeOfFirstSample, samplingTime);

                    if (samplingPhase) newPrepData->set_samplingPhase();
                    newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
                    collection->push_back(newPrepData);
                }
            }
        }
    }
    return StatusCode::SUCCESS;
}
