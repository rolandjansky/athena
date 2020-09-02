/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//    IMcEventNtupleTool.h
//   Header file for interface of McEventNtupleTools
//////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
////////////////////////////////////////////////////////////////////////////

#ifndef TRK_IMCEVENTNTUPLETOOL_H
#define TRK_IMCEVENTNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AtlasHepMC/GenEvent.h"

namespace Trk {
static const InterfaceID IID_IMcEventNtupleTool("IMcEventNtupleTool",1,0);


/** @class IMcEventNtupleTool
    provides the interface for validation tools which write special information
    about MC particles & vertices into ntuples.
*/

class IMcEventNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** fill and write ntuple data of a given VxCandidate */
    virtual StatusCode fillMcEventData(const HepMC::GenEvent&) const = 0;
};

inline const InterfaceID& Trk::IMcEventNtupleTool::interfaceID() {
    return IID_IMcEventNtupleTool;
}

} // end of namespace

#endif // TRK_IMCEVENTNTUPLETOOL_H
