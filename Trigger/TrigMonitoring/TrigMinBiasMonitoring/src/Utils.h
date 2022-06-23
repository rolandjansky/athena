/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "StoreGate/ReadHandle.h"

namespace Utils {
    // provide the trk DCA w.r.t. the PV
    double z0wrtPV(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx);
    // finds the Primary Vertex
    const xAOD::Vertex* selectPV(SG::ReadHandle<xAOD::VertexContainer>& container);
}