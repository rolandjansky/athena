/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @brief validate the logic of the 4-out-of-4 pad trigger

   davide.gerbaudo@gmail.com
   Oct 2015
 */
#undef NDEBUG // allow 'assert' (disabled in optimized builds)

#include "PadDummyData.h"

#include "../src/PadTriggerLogicOfflineTool.h"
#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"

#include <iostream>

using namespace std;
using NSWL1::PadData;
using NSWL1::PadDummyData;
using NSWL1::PadTrigger;
using NSWL1::PadTriggerLogicOfflineTool;

//------------------------------------------------------------------------------
vector<PadDummyData> generate_4_pads_aligned(const int &side, const int &sector)
{
    PadDummyData p; // template pad
    p.sideId(side).sectorId(sector).multipletId(1).padEtaId(1).padPhiId(2);
    vector<PadDummyData> pads;
    pads.push_back(p.gasGapId(0));
    pads.push_back(p.gasGapId(1));
    pads.push_back(p.gasGapId(2));
    pads.push_back(p.gasGapId(3));
    return pads;
}
//------------------------------------------------------------------------------
vector<PadData*> vector_obj_2_vector_ptrs(vector<PadDummyData> &in)
{
    vector<PadData*> out(in.size(), nullptr);
    for(size_t i=0; i<in.size(); ++i)
        out[i] = static_cast<PadData*>(&(in[i]));
    return out;
}
//------------------------------------------------------------------------------
int main()
{
    cout<<"running PadTriggerLogicOfflineTool_test"<<endl;
    vector<PadDummyData> triggeringPads = generate_4_pads_aligned(0, 5);
    std::vector<PadData*> padData = vector_obj_2_vector_ptrs(triggeringPads);
    vector<PadTrigger*> triggers = PadTriggerLogicOfflineTool::build4of4SingleWedgeTriggers(padData);
    cout<<"got "<<triggers.size()<<" triggers from generate_4_pads_aligned"<<endl;
    
    return 0;
}
