/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMCSYM_H
#define LARRAWCONDITIONS_LARMCSYM_H


/** Helper class to handle z-phi symmetry of calibration constants in MC
 */

#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

class LArMCSym {

 public:
  LArMCSym() = delete;
  LArMCSym(const LArOnlineID* onlId, 
	   const CaloCell_ID* caloId,
	   std::vector<HWIdentifier>&& oflHashtoSymOnl,
	   std::vector<HWIdentifier>&& onlHashtoSymOnl
	   ); 
   
  HWIdentifier ZPhiSymOfl(const Identifier notSymOffId) const {
    const IdentifierHash h=m_caloCellID->calo_cell_hash(notSymOffId);
    return ZPhiSymOfl(h);
  }

  HWIdentifier ZPhiSymOfl(const IdentifierHash notSymOffHash) const {
    assert(notSymOffHash < m_oflHashtoSymOnl.size());
    return m_oflHashtoSymOnl[notSymOffHash];
  }
  
  HWIdentifier ZPhiSymOnl(const HWIdentifier notSymOnlId) const {
    const IdentifierHash h=m_onlineID->channel_Hash(notSymOnlId);
    return ZPhiSymOnl(h);
  }
  

  HWIdentifier ZPhiSymOnl(const IdentifierHash notSymOnlHash) const {
    assert(notSymOnlHash < m_onlHashtoSymOnl.size());
    return m_onlHashtoSymOnl[notSymOnlHash];
  }

 private:
  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellID;
  const std::vector<HWIdentifier> m_oflHashtoSymOnl;
  const std::vector<HWIdentifier> m_onlHashtoSymOnl;

};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArMCSym , 33771274 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( LArMCSym, 159334782 );

#endif 
