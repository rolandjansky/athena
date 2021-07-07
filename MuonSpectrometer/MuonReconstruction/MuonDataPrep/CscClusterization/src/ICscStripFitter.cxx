/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ICscStripFitter.cxx
#include "CscClusterization/ICscStripFitter.h"

using Muon::CscStripPrepData;

typedef ICscStripFitter::Result Result;
using ChargeList = ICscStripFitter::ChargeList;

// Result ICscStripFitter::fit(const ChargeList&, double, Identifier& /*sid*/) const {
//  return Result();
//}
Result ICscStripFitter::fit(const ChargeList& /*ChargeList*/, double /*samplingTime*/, bool /*samplingPhase*/, Identifier& /*sid*/) const {
    return Result();
}

Result ICscStripFitter::fit(const CscStripPrepData& strip) const {
    Identifier sid = strip.identify();
    //  IdentifierHash coll_hash = strip.collectionHash();

    Result res = fit(strip.sampleCharges(), strip.samplingTime(), strip.samplingPhase(), sid);
    res.strip = &strip;
    if (res.status) return res;
    res.time += strip.timeOfFirstSample();
    // Do we also need a phase correction here?
    return res;
}
