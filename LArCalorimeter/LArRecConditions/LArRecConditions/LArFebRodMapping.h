//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBRODMAPPING_H
#define LARRECCONDITIONS_LARFEBRODMAPPING_H

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <vector>

class LArOnlineID;

class LArFebRodMapping {

  friend class LArFebRodMappingAlg; //The conditions alg filling this object

 public:
  LArFebRodMapping()=delete;
  LArFebRodMapping(const LArOnlineID* onlineId);

  HWIdentifier getReadoutModuleID(const HWIdentifier id) const;
  HWIdentifier getReadoutModuleIDFromHash(const IdentifierHash h) const;
  
  /**
   *   "iterator" on LArReadoutModuleIDs  <p>
   */
  const std::vector<HWIdentifier>& getLArRoModIDvec() const;

 private:
  const LArOnlineID* m_onlineId;
  const uint32_t* m_pFebHashtoROD;
  std::vector<HWIdentifier> m_readoutModuleIDVec;

};


//Inline methods:
inline HWIdentifier LArFebRodMapping::getReadoutModuleIDFromHash(const IdentifierHash h) const{
  return HWIdentifier(m_pFebHashtoROD[h]);
}

inline const std::vector<HWIdentifier>& LArFebRodMapping::getLArRoModIDvec() const {
  return m_readoutModuleIDVec;
}

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( LArFebRodMapping, 204177791 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArFebRodMapping>, 244452853 , 1)

#endif
