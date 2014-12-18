/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREVENTATHENAPOOL_MDTCSM_P1_H
#define MUONREVENTATHENAPOOL_MDTCSM_P1_H

#include "MuonEventAthenaPool/MdtAmtHit_p1.h"

#include <inttypes.h>
#include <vector>

// MDT RDOs : Chamber Service Module, container of AmtHits of a single Mdt chamber
// Stefano Rosati Feb 2003

class MdtCsm_p1 : public std::vector<MdtAmtHit_p1>
{
public:
    friend class  MdtCsmCnv_p1;
    MdtCsm_p1() : m_Id(0), m_idHash(0), m_SubDetId(0), m_MrodId(0), m_CsmId(0) {}
    
 private:

  uint32_t m_Id;
  uint32_t m_idHash; 

  uint16_t m_SubDetId;   // Sub-detector identifier
  uint16_t m_MrodId;     // Mrod number
  uint16_t m_CsmId;      // Csm link number
};

#endif   // MUONREVENTATHENAPOOL_MDTCSM_P1_H

