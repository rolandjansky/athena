/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//    IVtxValidationNtupleTool.h
//   Header file for interface of VtxValidationNtupleTools
//////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
////////////////////////////////////////////////////////////////////////////

#ifndef TRK_IVTXVALIDATIONNTUPLETOOL_H
#define TRK_IVTXVALIDATIONNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "VxVertex/VxCandidate.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

namespace Trk {
static const InterfaceID IID_IVtxValidationNtupleTool("IVtxValidationNtupleTool",1,0);


/** @class IVtxValidationNtupleTool
    provides the interface for validation tools which write special information
    about generated vertices into ntuples.
*/

class IVtxValidationNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** fill and write ntuple data of a given VxCandidate */
    virtual StatusCode fillVxCandidateData (const Trk::VxCandidate&) const = 0;
    virtual StatusCode fillEventInfo(int &) const = 0;
    virtual StatusCode fillTrueTrackAtVertexInfo(const Trk::VxCandidate&, const TrackCollection&, const TrackTruthCollection&) const = 0;
};

inline const InterfaceID& Trk::IVtxValidationNtupleTool::interfaceID() {
    return IID_IVtxValidationNtupleTool;
}

} // end of namespace

#endif // TRK_IVTXVALIDATIONNTUPLETOOL_H
