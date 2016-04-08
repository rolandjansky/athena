/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCABLING_LARCABLINGBASE_H
#define LARCABLING_LARCABLINGBASE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h" 

#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "LArIdentifier/LArOnlineID_Base.h" //FIXME need BASE ID

class AthenaAttributeList;

/** 
   @class LArCablingBase 
   @brief Base class to cover the common aspect of the regular
          LArCablingSvc and the SuperCellCablingTool

  @author Walter Lampl
 
*/

class LArCablingBase : public AthAlgTool {

public:
  
  /** constructor */
  LArCablingBase( const std::string& type, const std::string& name, const IInterface* parent ) ;

  ~LArCablingBase();
  
  StatusCode finalize( );


  /**
   * create a HWIdentifier from an Identifier (not inline)<br>
   * 
   */
  HWIdentifier createSignalChannelID(const Identifier & id);


  /**
   * create a HWIdentifier from an Identifier (from hash) (inline)<br>
   * 
   */
  HWIdentifier createSignalChannelIDFromHash(const IdentifierHash & id_hash);

  /**
   * create an Identifier from a HWIdentifier (inline) <br>
   * 
   */
  Identifier cnvToIdentifier(const HWIdentifier & sid); 

  /**
   * create an Identifier from a HWIdentifier (from hash) (inline) <br>
   */
  Identifier cnvToIdentifierFromHash(const IdentifierHash & sid_hash); 
    
  /** 
   * Test wether a HWIdentifier is connected of not (by online hash) (inline) <br>
   */
  bool isOnlineConnectedFromHash(const IdentifierHash & sid_hash ); 
 
  /** 
   * Test wether a HWIdentifier is connected of not (inline)<br>
   */
  bool isOnlineConnected(const HWIdentifier & sid );


protected:

  const LArOnlineID_Base * m_onlineId ;
  const CaloCell_Base_ID * m_caloId;

  const DataHandle<AthenaAttributeList> m_attrOnOff;
  bool m_onOffValid;
  std::string m_onOffIdKey;

  ///Invalid default instance of Identifier
  const Identifier m_idEmpty;
  
  ///Invalid default instance of HWIdentifier
  const HWIdentifier m_hwidEmpty;
   
private:
  std::vector<Identifier>  m_onlHashToOffline;
  std::vector<HWIdentifier> m_oflHashToOnline;
    
  bool readOnlOffMap();
   
};
 
  //Inline methods:

inline bool LArCablingBase::isOnlineConnectedFromHash(const IdentifierHash & sid_hash ) {
  if (m_onOffValid || readOnlOffMap()) 
    if (sid_hash <m_onlHashToOffline.size())
      return (m_idEmpty!=m_onlHashToOffline[sid_hash]);
  
  return false;
}


inline HWIdentifier LArCablingBase::createSignalChannelIDFromHash(const IdentifierHash & id_hash) {
  if (m_onOffValid || readOnlOffMap()) 
    return m_oflHashToOnline[id_hash];
  else 
    return m_hwidEmpty;
}


inline Identifier LArCablingBase::cnvToIdentifierFromHash(const IdentifierHash & sid_hash) {
  if (m_onOffValid || readOnlOffMap()) 
    if (sid_hash <m_onlHashToOffline.size())
      return m_onlHashToOffline[sid_hash];
  
  return m_idEmpty;
}  

inline Identifier LArCablingBase::cnvToIdentifier(const HWIdentifier & sid) {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->cnvToIdentifierFromHash(sid_hash); 
 }

inline bool LArCablingBase::isOnlineConnected(const HWIdentifier & sid ) {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->isOnlineConnectedFromHash(sid_hash);
}  

inline HWIdentifier LArCablingBase::createSignalChannelID(const Identifier & id) {
  const IdentifierHash id_hash=m_caloId->calo_cell_hash(id);
  return this->createSignalChannelIDFromHash(id_hash);
}


#endif // LARCABLING_LARCABLINGBASE_H
