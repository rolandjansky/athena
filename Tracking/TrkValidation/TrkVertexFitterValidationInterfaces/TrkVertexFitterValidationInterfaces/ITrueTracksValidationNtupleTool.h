/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//    ITrueTracksValidationNtupleTool.h
//   Header file for interface of TrueTracksValidationNtupleTools
//////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
////////////////////////////////////////////////////////////////////////////

#ifndef TRK_ITRUETRACKSVALIDATIONNTUPLETOOL_H
#define TRK_ITRUETRACKSVALIDATIONNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

namespace Trk {
static const InterfaceID IID_ITrueTracksValidationNtupleTool("ITrueTracksValidationNtupleTool",1,0);


/** @class ITrueTracksValidationNtupleTool
    provides the interface for helper tool(s) which write true information
    about track collection used for vertex finding into ntuples.
*/

class ITrueTracksValidationNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** fill and write ntuple with vertex finding relevant data of a given TrackCollectiom */
    virtual StatusCode fillTrueTracksInfo(const TrackCollection&, const TrackTruthCollection&) const = 0;
};

inline const InterfaceID& Trk::ITrueTracksValidationNtupleTool::interfaceID() {
    return IID_ITrueTracksValidationNtupleTool;
}

} // end of namespace

#endif // TRK_ITRUETRACKSVALIDATIONNTUPLETOOL_H 
