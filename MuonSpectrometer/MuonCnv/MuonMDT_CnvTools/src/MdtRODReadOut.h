/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRODREADOUT_H
#define MUONBYTESTREAM_MDTRODREADOUT_H

#include <stdint.h>

#include <vector>

#include "MdtReadOut.h"

// Decoding methods for an MDT ROD
// S. Rosati Feb 2003

class MdtRODReadOut : public MdtReadOut {
private:
    // uint32_t m_dataWord;
    // Data members
    uint16_t m_subdetId;       // Sub-detector Id
    uint16_t m_mrodId;         // MROD Id
    uint32_t m_lvl1Id;         // Lvl1 Id
    uint16_t m_bcId;           // Bunch crossing Id
    uint16_t m_triggerTypeId;  // Trigger type Id

    // Data words in the ROD header
    static constexpr uint32_t s_RODstart = 0xee1234ee;
    static constexpr uint32_t s_RODheadersize = 0x8;
    static constexpr uint32_t s_RODversion = 0;  // ??

public:
    MdtRODReadOut();
    ~MdtRODReadOut()=default;
    
    void decodeHeader(const std::vector<uint32_t>& p);
    void decodeFooter(const std::vector<uint32_t>& p);

    uint32_t* encodeFooter();

    // Header words
    constexpr uint32_t makeHeaderMarker() { return s_RODstart; }
    constexpr uint32_t makeHeaderSize() { return s_RODheadersize; }
    constexpr uint32_t makeFormatVersion() { return s_RODversion; }
    uint32_t makeRODId(uint16_t subdet, uint16_t mrod);

    // Retrieve decoded results
    uint16_t subdetId() const { return m_subdetId; }
    uint16_t mrodId() const { return m_mrodId; }
    uint32_t lvl1Id() const { return m_lvl1Id; }
    uint16_t bcId() const { return m_bcId; }
    uint16_t trigtypeId() const { return m_triggerTypeId; }

private:
    // Private functions
    void setZero();

};  // MUONBYTESTREAM_MDTRODREADOUT_H

#endif
