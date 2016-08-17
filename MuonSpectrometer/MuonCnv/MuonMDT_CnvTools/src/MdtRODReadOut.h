/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRODREADOUT_H
#define MUONBYTESTREAM_MDTRODREADOUT_H

#include "MdtReadOut.h"

#include <stdint.h>
#include <vector>

// Decoding methods for an MDT ROD
// S. Rosati Feb 2003

class MdtRODReadOut : public MdtReadOut
{

 private:

  //uint32_t m_dataWord;
  // Data members
  uint16_t m_subdetId;   // Sub-detector Id 
  uint16_t m_mrodId;     // MROD Id
  uint32_t m_lvl1Id;     // Lvl1 Id
  uint16_t m_bcId;       // Bunch crossing Id
  uint16_t m_triggerTypeId; // Trigger type Id

  // Data words in the ROD header
  static const uint32_t s_RODstart;
  static const uint32_t s_RODheadersize;
  static const uint32_t s_RODversion;      // ??

 public:
  MdtRODReadOut();
  ~MdtRODReadOut();
  void decodeHeader(const std::vector<uint32_t>& p);
  void decodeFooter(const std::vector<uint32_t>& p);

  uint32_t* encodeFooter();
  
  // Header words
  uint32_t makeHeaderMarker() {return s_RODstart;}
  uint32_t makeHeaderSize() {return s_RODheadersize;}
  uint32_t makeFormatVersion() {return s_RODversion;}
  uint32_t makeRODId(uint16_t subdet, uint16_t mrod);


  // Retrieve decoded results
  uint16_t subdetId()   {return m_subdetId;}
  uint16_t mrodId()     {return m_mrodId;}
  uint32_t lvl1Id()     {return m_lvl1Id;}
  uint16_t bcId()       {return m_bcId;}
  uint16_t trigtypeId() {return m_triggerTypeId;}

 private:
  
  // Private functions
  void setZero();

};   // MUONBYTESTREAM_MDTRODREADOUT_H

#endif
