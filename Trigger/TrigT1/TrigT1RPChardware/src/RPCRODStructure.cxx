/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/RPCRODStructure.h"

#include <cstring>
#include <fstream>
#include <iostream>

#include "TrigT1RPChardware/RPCReadOut.h"

using namespace std;

//----------------------------------------------------------------------------//
RPCRODStructure::RPCRODStructure() {
    //
    // Constructor used by the simulation program
    //
    setInit();
}  // end-of-RPCRODStructure::RPCRODStructure()
//----------------------------------------------------------------------------//
RPCRODStructure::RPCRODStructure(ubit16 /*inputData*/) {
    //
    // Constructor used by the simulation program
    //
    // decodeFragment;
    m_sourceIDreserved = 0xffff;
    m_moduleType = 0xffff;
    m_subDetectorID = 0xffff;
    m_RODID = 0xffff;
}  // end-of-RPCRODStructure::RPCRODStructure()
//----------------------------------------------------------------------------//
RPCRODStructure::~RPCRODStructure() {}  // end-of-RPCRODStructure::~RPCRODStructure
//----------------------------------------------------------------------------//
void RPCRODStructure::setInit() {
    m_sourceIDreserved = 0xffff;
    m_moduleType = 0xffff;
    m_subDetectorID = 0xffff;
    m_RODID = 0xffff;
}  // end-of-setInit
//----------------------------------------------------------------------------//
RODword RPCRODStructure::getSourceID(ubit16 subsystem, ubit16 RODID) {
    RODword sourceIdentifier = 0;
    sourceIdentifier = s_sourceIDreserved << 24 | s_moduleType << 16 | getSubDetectorID(subsystem) << 8 | RODID;
    return sourceIdentifier;
}  // end-of-RPCRODStructure::getSourceID
//----------------------------------------------------------------------------//
void RPCRODStructure::decodeSourceID(RODword sourceID) {
    m_sourceIDreserved = (sourceID & 0xff000000) >> 24;
    m_moduleType = (sourceID & 0x00ff0000) >> 16;
    m_subDetectorID = (sourceID & 0x0000ff00) >> 8;
    m_RODID = (sourceID & 0x000000ff);
}  // end-of-decodeSourceID
//----------------------------------------------------------------------------//
ubit16 RPCRODStructure::getSubDetectorID(ubit16 subsystem) {
    ubit16 id = s_subDetectorIDA;
    if (subsystem) id = s_subDetectorIDC;
    return id;
}  // end-of-getSubDetectorID
