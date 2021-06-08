/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <vector>

#include "ers/ers.h"

#include "MuonNSWCommonDecode/NSWDecodeBitmaps.h"
#include "MuonNSWCommonDecode/NSWDecodeHelper.h"

#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"

Muon::nsw::NSWElink::NSWElink (uint32_t *bs)
  : m_wordCount (0)
{
  // Felix header (2 words)
  // Packet length includes Felix header

  uint32_t word = bs[m_wordCount++];
  unsigned int packet_nbytes = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskFlxLENGTH, Muon::nsw::bitPosFlxLENGTH);
  m_packet_status  = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskFlxSTATUS, Muon::nsw::bitPosFlxSTATUS);

  m_elinkWord = bs[m_wordCount++];
  m_elinkId = new Muon::nsw::NSWResourceId (m_elinkWord);

  // Decode sROC header

  word = bs[m_wordCount++];

  ERS_DEBUG (2, "==============================================================");
  ERS_DEBUG (2, "FELIX HEADER: LENGTH (BYTES) = " << packet_nbytes <<
	     " | STATUS = " << m_packet_status << " | ELINK ID = 0x" << std::hex << m_elinkId << std::dec);

  if ((m_isNull = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocNULL, Muon::nsw::bitPosSRocNULL)) == true)
  {
    m_l1Id  = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocNullL1ID, Muon::nsw::bitPosSRocNullL1ID);
    m_rocId = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocNullID, Muon::nsw::bitPosSRocNullID);

    ERS_DEBUG (2, "ROC HEADER: | NULL = " << m_isNull <<
	       " | ROCID = " << m_rocId << " | L1ID = " << m_l1Id);
  }
  else
  {
    // Calculate packet checksum

    uint8_t *p = reinterpret_cast <uint8_t *> (bs + 2);
    m_running_checksum = this->test_checksum (p, (packet_nbytes - 2 * sizeof (uint32_t)));

    // m_noTDC will only affect data size, hit words should be re-aligned to uint32_t

    m_noTdc     = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocNOTDC, Muon::nsw::bitPosSRocNOTDC);
    m_l1Id      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocL1ID, Muon::nsw::bitPosSRocL1ID);
    m_bcId      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocBCID, Muon::nsw::bitPosSRocBCID);
    m_orbit     = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskSRocORBIT, Muon::nsw::bitPosSRocORBIT);

    // Set hit bytes to correspond to the length in Flx header
    // Everything should be re-aligned in the swROD, though

    unsigned int hit_size  = m_noTdc ? 3 : 4; // set hit bytes to correspond to the length in Flx header

    // Two 32-bits words for Felix header, 1 word for ROC header, 1 word for the ROC trailer

    m_nhits = (packet_nbytes - 4 * sizeof (uint32_t)) / hit_size;
    if (packet_nbytes % 4) ++m_nhits;

    ERS_DEBUG (2, "ROC HEADER: T = " << m_noTdc << " | NULL = " << m_isNull <<
	       " | ORBIT = " << m_orbit << " | BCID = " << m_bcId << " | L1ID = " << m_l1Id);             
    
    // Hit Data

    for (unsigned int i=0; i < m_nhits; ++i)
    {
      Muon::nsw::VMMChannel *c = new Muon::nsw::VMMChannel (bs[m_wordCount++], this);
      m_channels.push_back(c);
    }

    // ROC Trailer (1 word)

    word = bs[m_wordCount++];

    m_checksum   = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailCHECKSUM, Muon::nsw::bitPosTrailCHECKSUM);
    m_nhitsTrail = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailLENGTH, Muon::nsw::bitPosTrailLENGTH);
    m_l0Id       = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailL0ID, Muon::nsw::bitPosTrailL0ID);
    m_flagMiss   = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailFLAGMISS, Muon::nsw::bitPosTrailFLAGMISS);
    m_tout       = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailTO, Muon::nsw::bitPosTrailTO);
    m_extended   = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskTrailEXTENDED, Muon::nsw::bitPosTrailEXTENDED);

    ERS_DEBUG (2, "ROC TRAILER: EXTENDED = " << m_extended << " | TIMEOUT = " << m_tout <<
	       " | FLAGMISS = " << m_flagMiss << " | L0ID = " << m_l0Id << " | NHITS = " << m_nhits <<
	       " | CHECKSUM = 0x" << std::hex << static_cast <unsigned int> (m_checksum) << std::dec);             
    ERS_DEBUG (2, "PACKET CHECKSUM = " << m_running_checksum);
  }
}

Muon::nsw::NSWElink::~NSWElink ()
{
  for (auto i = m_channels.begin (); i != m_channels.end (); ++i)
    delete *i;

  delete m_elinkId;
}

