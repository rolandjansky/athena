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

class LArMCSym {

 public:
  LArMCSym() = delete;
  LArMCSym(const LArOnlineID* onlId, 
	   std::vector<HWIdentifier>&& oflHashtoSymOnl); 
   
  // method to handle z-phi symmetry
  HWIdentifier ZPhiSym(const HWIdentifier& notSymOffId) const {
    const IdentifierHash h=m_onlineID->channel_Hash(notSymOffId);
    return ZPhiSym(h);
  }

  HWIdentifier ZPhiSym(const IdentifierHash notSymOffHash) const {
    assert(notSymOffHash < m_oflHashtoSymOnl.size());
    return m_oflHashtoSymOnl[notSymOffHash];
  }
  
  


 private:
  const LArOnlineID* m_onlineID;
  const std::vector<HWIdentifier> m_oflHashtoSymOnl;
  const std::vector<HWIdentifier> m_onlHashtoSymOnl;

};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( LArMCSym , 33771274 , 1 )
//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF(  CondCont<LArMCSym> ,159334782 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( CondCont<LArMCSym>, CondContBase );

#endif 
