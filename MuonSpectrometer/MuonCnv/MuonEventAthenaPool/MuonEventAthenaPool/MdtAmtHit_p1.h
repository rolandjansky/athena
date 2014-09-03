/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREVENTATHENAPOOL_MDTAMTHIT_P1_H
#define MUONREVENTATHENAPOOL_MDTAMTHIT_P1_H

#include <inttypes.h>

#include<vector>

// MDT RDO's : data from a single channel of an AMT 
// Atlas Muon TDC
// Stefano Rosati, Feb 2003

class MdtAmtHit_p1
{
public:
    friend class  MdtAmtHitCnv_p1;
    MdtAmtHit_p1() : m_tdcId(0), m_channelId(0), m_leading(false), m_coarse(0), m_fine(0),  m_width(0), m_isMasked(false) {}
    
 private:
  
  /** TDC Id in the CSM (TDC online Id)*/
  uint16_t m_tdcId;
  /** Channel number */
  uint16_t m_channelId;

  // Leading edge boolean flag
  bool m_leading;
  
  // Decoded time of the first leading edge
  uint16_t m_coarse;
  uint16_t m_fine;
  // Decoded width in case of a combined measurement
  uint16_t m_width;

  // Masked channel flag
  bool m_isMasked;
  
  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using MdtReadOut methods
  std::vector<uint32_t> m_dataWords;
};

#endif // MUONREVENTATHENAPOOL_MDTAMTHIT_P1_H


