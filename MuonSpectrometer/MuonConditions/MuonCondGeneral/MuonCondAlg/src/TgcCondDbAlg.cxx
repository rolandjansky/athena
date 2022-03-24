/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcCondDbAlg.h"

/*
  ATTENTION: This is the migrated thread-safe version of the original TGCCondSummarySvc;
  at the time of writing (Summer 2019) it is not clear whether or not it will be actually
  used; so this algorithm is merely a template!
*/

// constructor
TgcCondDbAlg::TgcCondDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {
    declareProperty("isOnline", m_isOnline);
    declareProperty("isData", m_isData);
    declareProperty("isRun1", m_isRun1);
}

// Initialize
StatusCode TgcCondDbAlg::initialize() {
    ATH_MSG_DEBUG("initializing " << name());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_readKey_folder_detectorStatus.initialize());

    return StatusCode::SUCCESS;
}

// execute
StatusCode TgcCondDbAlg::execute() {
    ATH_MSG_DEBUG("execute " << name());

    if (m_isOnline) {
        ATH_MSG_DEBUG("IsOnline is set to True; nothing to do!");
        return StatusCode::SUCCESS;
    }

    // launching Write Cond Handle
    SG::WriteCondHandle<TgcCondDbData> writeHandle{m_writeKey};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << " In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }
    std::unique_ptr<TgcCondDbData> writeCdo{std::make_unique<TgcCondDbData>()};
    EventIDRange rangeW;
    StatusCode sc = StatusCode::SUCCESS;

    // retrieving data
    // if(loadDetectorStatus(rangeW, writeCdo).isFailure()) sc = StatusCode::FAILURE; // keep for future development

    if (sc.isFailure()) {
        ATH_MSG_WARNING("Could not read data from the DB");
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
        ATH_MSG_FATAL("Could not record TgcCondDbData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    return StatusCode::SUCCESS;
}

// loadDetectorStatus
StatusCode TgcCondDbAlg::loadDetectorStatus(EventIDRange& rangeW, std::unique_ptr<TgcCondDbData>& writeCdo) {
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey_folder_detectorStatus};
    const CondAttrListCollection* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    if (!readHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);

    CondAttrListCollection::const_iterator itr;
    for (itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
        const coral::AttributeList& atr = itr->second;
        int detector_status;

        detector_status = *(static_cast<const int*>((atr["detector_status"]).addressOfData()));
        ATH_MSG_DEBUG("TGC detector status is " << detector_status);

        if (detector_status != 0) {
            int channum = itr->first;
            Identifier chamberId = m_idHelperSvc->tgcIdHelper().elementID(Identifier(channum));
            writeCdo->setDeadStation(chamberId);
        }
    }

    return StatusCode::SUCCESS;
}
