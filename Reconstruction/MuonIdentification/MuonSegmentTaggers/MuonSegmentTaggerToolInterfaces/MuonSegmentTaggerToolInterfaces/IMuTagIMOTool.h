/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuTagIMOTool_H
#define IMuTagIMOTool_H

#include "GaudiKernel/IAlgTool.h"

namespace Rec {
    class TrackParticleContainer;
}

class MuTagContainer;

static const InterfaceID IID_IMuTagIMOTool("IMuTagIMOTool", 1, 0);

class IMuTagIMOTool : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID();

    virtual MuTagContainer* tag(const Rec::TrackParticleContainer* trackParticles, const Trk::SegmentCollection* segments) const = 0;
};

inline const InterfaceID& IMuTagIMOTool::interfaceID() { return IID_IMuTagIMOTool; }

#endif  // IMuTagIMOTool_H
