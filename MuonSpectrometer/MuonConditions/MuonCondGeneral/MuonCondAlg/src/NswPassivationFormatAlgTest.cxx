/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/NswPassivationFormatAlgTest.h"
#include "CoralBase/Blob.h"
#include "CoralUtilities/blobaccess.h"
#include "nlohmann/json.hpp"

// constructor
NswPassivationFormatAlgTest::NswPassivationFormatAlgTest(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

// destructor
NswPassivationFormatAlgTest::~NswPassivationFormatAlgTest() { ATH_MSG_DEBUG("Calling destructor"); }

// initialize
StatusCode NswPassivationFormatAlgTest::initialize() {
    ATH_MSG_DEBUG("Calling initialize");
    ATH_CHECK(m_readKey.initialize());
    return StatusCode::SUCCESS;
}

// execute
StatusCode NswPassivationFormatAlgTest::execute(const EventContext& ctx) const {
    ATH_MSG_DEBUG("Calling execute");

    // setup parameters
    std::chrono::duration<double> retrieving{};

    // retrieve all folders
    if (!retrieve(ctx, retrieving).isSuccess()) return StatusCode::FAILURE;

    // postprocess
    ATH_MSG_DEBUG("Retrieving time = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving).count() * 1.0) << "s ");

    ATH_MSG_DEBUG("MADE IT TO THE END!!");
    return StatusCode::SUCCESS;
}

// finalize
StatusCode NswPassivationFormatAlgTest::finalize() {
    ATH_MSG_DEBUG("Calling finalize");
    return StatusCode::SUCCESS;
}

// retrieve
StatusCode NswPassivationFormatAlgTest::retrieve(const EventContext& ctx, std::chrono::duration<double>& timer) const {
    ATH_MSG_DEBUG("Starting extraction " << timestamp());

    // Start with an infinte range and narrow it down as needed
    EventIDRange rangeW = IOVInfiniteRange::infiniteMixed();

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
    const CondAttrListCollection* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    EventIDRange range;
    if (!readHandle.range(range)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }

    // intersect validity range of this obj with the validity of already-loaded objs
    rangeW = EventIDRange::intersect(range, rangeW);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << range << ", range of output is " << rangeW);

    CondAttrListCollection::const_iterator itr;
    for (itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
        auto start1 = std::chrono::high_resolution_clock::now();
        const coral::AttributeList& atr = itr->second;
        auto end1 = std::chrono::high_resolution_clock::now();
        timer += end1 - start1;
        // m_retrieving[folder][setup] += end1-start1;

        std::string data;
        if (atr["data"].specification().type() == typeid(coral::Blob)) {
            ATH_MSG_VERBOSE("Loading data as a BLOB, uncompressing...");
            if (!CoralUtilities::readBlobAsString(atr["data"].data<coral::Blob>(), data)) {
                ATH_MSG_FATAL("Cannot uncompress BLOB! Aborting...");
                return StatusCode::FAILURE;
            }
        } else {
            ATH_MSG_VERBOSE("Loading data as a STRING");
            data = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
        }

        // write it to the buffer to compare it
		nlohmann::json yy = nlohmann::json::parse(data);
		std::cout << "OUTPUT START" << std::endl;
		for(auto &it : yy.items()) {
			nlohmann::json yx = it.value();
			for(auto &jt : yx.items())
				std::cout << jt.key() << "=" << jt.value() << ", ";
			std::cout << std::endl;
		}
		std::cout << "OUTPUT END" << std::endl;
    }
    ATH_MSG_DEBUG("Ending at " << timestamp());
    return StatusCode::SUCCESS;
}

std::string NswPassivationFormatAlgTest::timestamp() {
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration td = now.time_of_day();
    const long hours = td.hours();
    const long minutes = td.minutes();
    const long seconds = td.seconds();
    const long milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, milliseconds);
    return buf;
}
