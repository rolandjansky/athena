/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "NswPassivationTestAlg.h"

// STL
#include <stdio.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Gaudi and Athena
#include "AthenaKernel/IOVInfiniteRange.h"
#include "CoralBase/Blob.h"
#include "CoralUtilities/blobaccess.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"

// constructor
NswPassivationTestAlg::NswPassivationTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

// destructor
NswPassivationTestAlg::~NswPassivationTestAlg() = default;

// initialize
StatusCode NswPassivationTestAlg::initialize() {
	ATH_MSG_INFO("Calling initialize");
	ATH_CHECK(m_readKey.initialize());
	ATH_CHECK(m_idHelperSvc.retrieve());
	return StatusCode::SUCCESS;
}

// execute
StatusCode NswPassivationTestAlg::execute(const EventContext& ctx) const {
    ATH_MSG_INFO("Calling execute");

    // setup parameters
    std::chrono::duration<double> retrieving{};

    // retrieve all folders
    if (!retrieve(ctx, retrieving).isSuccess()) return StatusCode::FAILURE;

    // postprocess
    ATH_MSG_INFO("Retrieving time = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving).count() * 1.0) << "s ");

    ATH_MSG_INFO("MADE IT TO THE END!!");
    return StatusCode::SUCCESS;
}

// retrieveVmm
StatusCode NswPassivationTestAlg::retrieve(const EventContext& ctx, std::chrono::duration<double>& timer) const {
    ATH_MSG_INFO("Starting retrieval " << timestamp());
    auto start1 = std::chrono::high_resolution_clock::now();

    // Start with an infinte range and narrow it down as needed
    EventIDRange rangeW = IOVInfiniteRange::infiniteMixed();

    // Retrieve Data Object
    SG::ReadCondHandle<NswPassivationDbData> readHandle{m_readKey, ctx};
    const NswPassivationDbData* readCdo{*readHandle};
    if (!readCdo) {
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
    std::vector<Identifier> channelIds = readCdo->getChannelIds();
    ATH_MSG_INFO("Found data for " << channelIds.size() << " channels!");
	ATH_MSG_INFO("Going to display passivation params for all channels (left, right, top, bottom)");

	std::ofstream fout;
	fout.open("passivationDump.txt");
	fout << "athenaId,PCB,stationName,stationEta,stationPhi,multiLayer,gasGap,left,right,top,bottom\n";

    // retrieve data for the first channel
	for(unsigned int i=0; i<channelIds.size(); ++i){
        const Identifier& channel = channelIds[i];
        NswPassivationDbData::PCBPassivation passiv = readCdo->getPassivation(channel);
		int eta  = m_idHelperSvc->mmIdHelper().stationEta(channel);
		int chnl = m_idHelperSvc->mmIdHelper().channel   (channel);
		int pcb  = (chnl-1)/1024+1; // int division should round downwards
		if(std::abs(eta)>1) pcb+=5;
        ATH_MSG_INFO("Board "<<i<<" ("<<channel.get_compact()<<"): "<<passiv.left<<", "<<passiv.right<<", "<<passiv.top<<", "<<passiv.bottom);
		fout << channel.get_compact()<<","<<pcb<<","<<m_idHelperSvc->toString(channel)<<","<<passiv.left<<","<<passiv.right<<","<<passiv.top<<","<<passiv.bottom<<std::endl;
    }

	fout.close();

    auto end1 = std::chrono::high_resolution_clock::now();
    timer += end1 - start1;
    ATH_MSG_INFO("Ending at " << timestamp());
    return StatusCode::SUCCESS;
}

std::string NswPassivationTestAlg::timestamp() const {
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
