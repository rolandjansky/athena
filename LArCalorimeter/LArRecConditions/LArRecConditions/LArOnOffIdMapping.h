//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONOFFIDMAPPING_H
#define LARONOFFIDMAPPING_H

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <vector>

class LArOnOffIdMapping {

  friend class LArOnOffMappingAlg; //The conditions alg filling this object

 public:
  LArOnOffIdMapping()=delete;
  
  LArOnOffIdMapping(const LArOnlineID_Base* onlineId, const CaloCell_Base_ID * m_caloId);

  /**
   * create a HWIdentifier from an Identifier (not inline)<br>
   *
   */
  HWIdentifier createSignalChannelID(const Identifier & id) const;


  /**
   * create a HWIdentifier from an Identifier (from hash) (inline)<br>
   *
   */
  HWIdentifier createSignalChannelIDFromHash(const IdentifierHash & id_hash) const;

  /**
   * create an Identifier from a HWIdentifier (inline) <br>
   *
   */
  Identifier cnvToIdentifier(const HWIdentifier & sid) const;

  /**
   * create an Identifier from a HWIdentifier (from hash) (inline) <br>
   */
  Identifier cnvToIdentifierFromHash(const IdentifierHash & sid_hash) const;

  /**
   * Test whether a HWIdentifier is connected of not (by online hash) (inline) <br>
   */
  bool isOnlineConnectedFromHash(const IdentifierHash & sid_hash ) const;

  /**
   * Test whether a HWIdentifier is connected of not (inline)<br>
   */
  bool isOnlineConnected(const HWIdentifier & sid ) const;
  
  

 private:
  const LArOnlineID_Base * m_onlineId ;
  const CaloCell_Base_ID * m_caloId;

  std::vector<Identifier>  m_onlHashToOffline;
  std::vector<HWIdentifier> m_oflHashToOnline;

  ///Invalid default instance of Identifier
  const Identifier m_idEmpty;

  ///Invalid default instance of HWIdentifier
  const HWIdentifier m_hwidEmpty;

};


//Inline methods:

inline bool LArOnOffIdMapping::isOnlineConnectedFromHash(const IdentifierHash & sid_hash ) const {
  if (sid_hash <m_onlHashToOffline.size())
    return (m_idEmpty!=m_onlHashToOffline[sid_hash]);

  return false;
}


inline HWIdentifier LArOnOffIdMapping::createSignalChannelIDFromHash(const IdentifierHash & id_hash) const {
  return m_oflHashToOnline[id_hash];
}


inline Identifier LArOnOffIdMapping::cnvToIdentifierFromHash(const IdentifierHash & sid_hash) const {
  if (sid_hash <m_onlHashToOffline.size())
    return m_onlHashToOffline[sid_hash];

  return m_idEmpty;
}

inline Identifier LArOnOffIdMapping::cnvToIdentifier(const HWIdentifier & sid) const {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->cnvToIdentifierFromHash(sid_hash);
 }

inline bool LArOnOffIdMapping::isOnlineConnected(const HWIdentifier & sid ) const {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->isOnlineConnectedFromHash(sid_hash);
}

inline HWIdentifier LArOnOffIdMapping::createSignalChannelID(const Identifier & id) const {
  const IdentifierHash id_hash=m_caloId->calo_cell_hash(id);
  return this->createSignalChannelIDFromHash(id_hash);
}



#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( LArOnOffIdMapping,148608605 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArOnOffIdMapping>, 155467877 , 0)

#endif
