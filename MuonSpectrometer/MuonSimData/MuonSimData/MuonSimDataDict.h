/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_MUONSIMDATADICT_H
#define MUONSIMEVENT_MUONSIMDATADICT_H

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonSimDataDict
{
    std::pair< HepMcParticleLink ,MuonMCData>  pair1;
    std::pair< HepMcParticleLink,CscMcData>    pair2;
    // Amg::Vector3D                              dummy(1.,2.,3.);
}

#endif
