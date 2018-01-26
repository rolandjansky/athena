/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @brief validate the logic of the TDR-like pad trigger

   davide.gerbaudo@gmail.com
   Oct 2015
 */
#undef NDEBUG // allow 'assert' (disabled in optimized builds)

#include "PadDummyData.h"

#include "../src/L1TdrStgcTriggerLogic.h"
#include "../src/PadTriggerLogicOfflineTool.h" // convertPadData2PadWithHits
#include "../src/PadWithHits.h"
#include "../src/SectorTriggerCandidate.h"
#include "../src/vector_utils.h" // indices
#include "TrigT1NSWSimTools/PadTrigger.h"

#include <iostream>

using namespace std;
using NSWL1::PadData;
using NSWL1::PadDummyData;
using NSWL1::PadTrigger;
using nsw::PadWithHits;
using nsw::SectorTriggerCandidate;
using nsw::L1TdrStgcTriggerLogic;

using NSWL1::PadTriggerLogicOfflineTool;

//------------------------------------------------------------------------------
vector<PadWithHits> generate_8_pads_aligned(const int &side, const int &sector)
{
    PadDummyData p; // template pad
    p.sideId(side).sectorId(sector).multipletId(1).padEtaId(1).padPhiId(2);
    const int innerMultiplet(0), outerMultiplet(1);
    vector<PadDummyData> pads;
    pads.push_back(p.gasGapId(0).multipletId(innerMultiplet));
    pads.push_back(p.gasGapId(1).multipletId(innerMultiplet));
    pads.push_back(p.gasGapId(2).multipletId(innerMultiplet));
    pads.push_back(p.gasGapId(3).multipletId(innerMultiplet));

    pads.push_back(p.gasGapId(0).multipletId(outerMultiplet));
    pads.push_back(p.gasGapId(1).multipletId(outerMultiplet));
    pads.push_back(p.gasGapId(2).multipletId(outerMultiplet));
    pads.push_back(p.gasGapId(3).multipletId(outerMultiplet));
    
    vector<PadWithHits> pwhs;
    for(PadDummyData &p : pads)
        pwhs.push_back(PadTriggerLogicOfflineTool::convertPadData2PadWithHits(p));
    return pwhs;
}
//------------------------------------------------------------------------------
int main()
{
    cout<<"running L1TdrStgcTriggerLogic__test"<<endl;
    L1TdrStgcTriggerLogic triggerLogic;
    triggerLogic.m_verbose = true;
    vector<PadWithHits> triggeringPads = generate_8_pads_aligned(0, 5);
    vector<size_t> padIndicesThisSector = nsw::indices(triggeringPads);
    vector< SectorTriggerCandidate > triggers;
    triggerLogic.buildSectorTriggers(triggeringPads, padIndicesThisSector);
    cout<<"got "<<triggers.size()<<" triggers from generate_8_pads_aligned"<<endl;

    /// @todo test 3/4
    /// @todo test mismatching inner/outer
    /// @todo test missing hits

    return 0;
}
