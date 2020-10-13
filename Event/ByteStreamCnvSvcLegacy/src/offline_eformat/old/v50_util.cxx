/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "v50_util.h"
#include "eformat/FullEventFragmentNoTemplates.h"
#include "eformat/ROBFragmentNoTemplates.h"
#include "v40_FullEventFragment.h"
#include "v40_write_FullEventFragment.h"
#include "eformat/Issue.h"

namespace {

// This should have been defined in eformat itself but it's not
const uint16_t MAJOR_V50_VERSION = 0x0500;

/**
 * This exception is supposed to be thrown when I decide to skip a fragment
 */
ERS_DECLARE_ISSUE_BASE(offline_eformat_err, SkippedFragmentIssue, eformat::Issue,
                       "Skipping " << ftype << " fragment in " << ptype
           << " fragment with identifier 0x"
           << EFORMAT_HEX(source_id), ERS_EMPTY,
           ((std::string) ftype) ((std::string) ptype) ((uint32_t) source_id))

/**
 * Simplifies the use of this Issue
 *
 * @param ftype A string representing the fragment type
 * @param sid The source identifier for the skipped fragment
 */
#define EFORMAT_SKIPPED_FRAGMENT(ftype,ptype,sid) \
    offline_eformat_err::SkippedFragmentIssue(ERS_HERE, ftype, ptype, sid)

}

namespace offline_eformat {
namespace v50 {

uint32_t convert_to_40(const uint32_t* src, uint32_t* dest, uint32_t max,
                       eformat::CheckSum event_checksum,
                       eformat::CheckSum rob_checksum)

{
    // this has to do the opposite of what eformat::v40::convert
    // is doing (for current = v50).

    eformat::helper::Version version(0, eformat::MAJOR_V40_VERSION);

    eformat::read::FullEventFragment fe(src);
    fe.check(); //this may throw

    // new version has 64-bit global id, cast it explicitly
    uint32_t global_id = static_cast<uint32_t>(fe.global_id());

    //create the base FullEvent
    v40_write::FullEventFragment nfe(fe.source_id(), fe.bc_time_seconds(),
                                     fe.bc_time_nanoseconds(),
                                     global_id, fe.run_type(),
                                     fe.run_no(), fe.lumi_block(),
                                     fe.lvl1_id(), fe.bc_id(),
                                     fe.lvl1_trigger_type());

    nfe.lvl1_trigger_info(fe.nlvl1_trigger_info(), fe.lvl1_trigger_info());
    nfe.lvl2_trigger_info(fe.nlvl2_trigger_info(), fe.lvl2_trigger_info());
    nfe.event_filter_info(fe.nevent_filter_info(), fe.event_filter_info());
    nfe.stream_tag(fe.nstream_tag(), fe.stream_tag());
    nfe.status(fe.nstatus(), fe.status());
    nfe.minor_version(version.minor_version());
    nfe.checksum_type(event_checksum);

    std::vector<v40_write::ROBFragment*> acc_rob;
    for (size_t i=0; i<fe.nchildren(); ++i) {
      try {
        eformat::read::ROBFragment rob(fe.child(i));
        rob.check(MAJOR_V50_VERSION);
        v40_write::ROBFragment* nrob =
          new v40_write::ROBFragment(rob.source_id(),
                                     rob.rod_run_no(),
                                     rob.rod_lvl1_id(), rob.rod_bc_id(),
                                     rob.rod_lvl1_trigger_type(),
                                     rob.rod_detev_type(),
                                     rob.rod_ndata(), rob.rod_data(),
                                     rob.rod_status_position());
        nrob->status(rob.nstatus(), rob.status());
        nrob->rod_status(rob.rod_nstatus(), rob.rod_status());
        eformat::helper::Version rob_version(rob.rod_version());
        nrob->minor_version(rob_version.minor_version());
        eformat::helper::Version rod_version(rob.rod_version());
        nrob->rod_minor_version(rod_version.minor_version());
        nrob->checksum_type(rob_checksum);

        //make this new ROB part of the new ROS
        nfe.append(nrob);
        //make sure we don't forget to delete this guy
        acc_rob.push_back(nrob);
      }
      catch (eformat::Issue& e) {
        ers::warning(e);
        ers::warning(EFORMAT_SKIPPED_FRAGMENT("ROB","FULL_EVENT",0));
        //we skip this fragment, but do not loose the whole event
        continue;
      }
    }

    //now the FullEvent is in `nfe', bind
    const eformat::write::node_t* top = nfe.bind();
    //memcpy the list of pages into contiguous memory
    uint32_t retval = eformat::write::copy(*top, dest, max);

    //delete the allocated stuff
    for (size_t i=0; i<acc_rob.size(); ++i) delete acc_rob[i];

    return retval;
}

}} // namespace offline_eformat::v50
