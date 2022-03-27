/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "NswCondTestAlg.h"

// STL
#include <stdio.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <iostream>
#include <sstream>

#include "vector"

// Gaudi and Athena
#include "AthenaKernel/IOVInfiniteRange.h"
#include "CoralBase/Blob.h"
#include "CoralUtilities/blobaccess.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"

// constructor
NswCondTestAlg::NswCondTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

// destructor
NswCondTestAlg::~NswCondTestAlg() = default;

// initialize
StatusCode NswCondTestAlg::initialize() {
    ATH_MSG_INFO("Calling initialize");
    ATH_CHECK(m_readKey_tdopdo.initialize());
    ATH_CHECK(m_readKey_vmm.initialize());
    return StatusCode::SUCCESS;
}

// execute
StatusCode NswCondTestAlg::execute(const EventContext& ctx) const {
    ATH_MSG_INFO("Calling execute");

    // setup parameters
    std::chrono::duration<double> retrieving_MM_TDO_A{};
    std::chrono::duration<double> retrieving_MM_TDO_C{};
    std::chrono::duration<double> retrieving_MM_PDO_A{};
    std::chrono::duration<double> retrieving_MM_PDO_C{};
    std::chrono::duration<double> retrieving_MM_VMM_A{};
    std::chrono::duration<double> retrieving_MM_VMM_C{};
    std::chrono::duration<double> retrieving_STGC_TDO_A{};
    std::chrono::duration<double> retrieving_STGC_TDO_C{};
    std::chrono::duration<double> retrieving_STGC_PDO_A{};
    std::chrono::duration<double> retrieving_STGC_PDO_C{};
    std::chrono::duration<double> retrieving_STGC_VMM_A{};
    std::chrono::duration<double> retrieving_STGC_VMM_C{};

    // retrieve all folders
    if (!retrieveTdoPdo(ctx, "TDO", "MM", "A", retrieving_MM_TDO_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "TDO", "MM", "C", retrieving_MM_TDO_C).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "PDO", "MM", "A", retrieving_MM_PDO_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "PDO", "MM", "C", retrieving_MM_PDO_C).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveVmm(ctx, "MM", "A", retrieving_MM_VMM_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveVmm(ctx, "MM", "C", retrieving_MM_VMM_C).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "TDO", "STGC", "A", retrieving_STGC_TDO_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "TDO", "STGC", "C", retrieving_STGC_TDO_C).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "PDO", "STGC", "A", retrieving_STGC_PDO_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveTdoPdo(ctx, "PDO", "STGC", "C", retrieving_STGC_PDO_C).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveVmm(ctx, "STGC", "A", retrieving_STGC_VMM_A).isSuccess()) return StatusCode::FAILURE;
    if (!retrieveVmm(ctx, "STGC", "C", retrieving_STGC_VMM_C).isSuccess()) return StatusCode::FAILURE;

    // postprocess
    ATH_MSG_INFO("Retrieving time for (MM  , TDO, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_TDO_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (MM  , TDO, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_TDO_C).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (MM  , PDO, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_PDO_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (MM  , PDO, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_PDO_C).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (MM  , VMM, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_VMM_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (MM  , VMM, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_MM_VMM_C).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, TDO, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_TDO_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, TDO, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_TDO_C).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, PDO, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_PDO_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, PDO, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_PDO_C).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, VMM, Side A) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_VMM_A).count() * 1.0) << "s ");
    ATH_MSG_INFO("Retrieving time for (STGC, VMM, Side C) = "
                 << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_STGC_VMM_C).count() * 1.0) << "s ");

    ATH_MSG_INFO("MADE IT TO THE END!!");
    return StatusCode::SUCCESS;
}

// retrieveTdoPdo
StatusCode NswCondTestAlg::retrieveTdoPdo(const EventContext& ctx, const std::string& data, const std::string& tech,
                                          const std::string& side, std::chrono::duration<double>& timer) const {
    ATH_MSG_INFO("Starting with " << data << " data for " << tech << " and " << side << " at " << timestamp());
    auto start1 = std::chrono::high_resolution_clock::now();

    // Start with an infinte range and narrow it down as needed
    EventIDRange rangeW = IOVInfiniteRange::infiniteMixed();

    // Retrieve Data Object
    SG::ReadCondHandle<NswCalibDbTimeChargeData> readHandle{m_readKey_tdopdo, ctx};
    const NswCalibDbTimeChargeData* readCdo{*readHandle};
    //	std::unique_ptr<NswCalibDbTimeChargeData> writeCdo{std::make_unique<NswCalibDbTimeChargeData>(m_idHelperSvc->mmIdHelper())};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    EventIDRange range;
    if (!readHandle.range(range)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }

    // Intersect validity range of this obj with the validity of already-loaded objs
    rangeW = EventIDRange::intersect(range, rangeW);

    // retrieve all channels
    std::vector<Identifier> channelIds = readCdo->getChannelIds(data, tech, side);
    ATH_MSG_INFO("Found data for " << channelIds.size() << " channels!");

    // retrieve data for the first channel
    if (channelIds.size() > 0) {
        Identifier channel = channelIds[0];
        ATH_MSG_INFO("Checking channel 0 (Id = " << channel.get_compact() << ")");

        double slope;
        readCdo->getSlope(data, &channel, slope);
        double error;
        readCdo->getSlopeError(data, &channel, error);
        ATH_MSG_INFO("slope     = " << slope << " (error=" << error << ")");

        double intercept;
        readCdo->getIntercept(data, &channel, intercept);
        double interror;
        readCdo->getInterceptError(data, &channel, interror);
        ATH_MSG_INFO("intercept = " << intercept << " (error=" << interror << ")");
    }

    auto end1 = std::chrono::high_resolution_clock::now();
    timer += end1 - start1;
    ATH_MSG_INFO("Ending at " << timestamp());
    return StatusCode::SUCCESS;
}

// retrieveVmm
StatusCode NswCondTestAlg::retrieveVmm(const EventContext& ctx, const std::string& tech, const std::string& side,
                                       std::chrono::duration<double>& timer) const {
    ATH_MSG_INFO("Starting with VMM data for " << tech << " and " << side << " at " << timestamp());
    auto start1 = std::chrono::high_resolution_clock::now();

    // Start with an infinte range and narrow it down as needed
    EventIDRange rangeW = IOVInfiniteRange::infiniteMixed();

    // Retrieve Data Object
    SG::ReadCondHandle<NswCalibDbThresholdData> readHandle{m_readKey_vmm, ctx};
    const NswCalibDbThresholdData* readCdo{*readHandle};
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    EventIDRange range;
    if (!readHandle.range(range)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }

    // Intersect validity range of this obj with the validity of already-loaded objs
    rangeW = EventIDRange::intersect(range, rangeW);

    // retrieve all channels
    std::vector<Identifier> channelIds = readCdo->getChannelIds(tech, side);
    ATH_MSG_INFO("Found data for " << channelIds.size() << " channels!");

    // retrieve data for the first channel
    if (channelIds.size() > 0) {
        Identifier channel = channelIds[0];
        ATH_MSG_INFO("Checking channel 0 (Id = " << channel.get_compact() << ")");

        double threshold;
        readCdo->getThreshold(&channel, threshold);
        ATH_MSG_INFO("threshold = " << threshold);
    }

    auto end1 = std::chrono::high_resolution_clock::now();
    timer += end1 - start1;
    ATH_MSG_INFO("Ending at " << timestamp());
    return StatusCode::SUCCESS;
}

std::string NswCondTestAlg::timestamp() const {
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
