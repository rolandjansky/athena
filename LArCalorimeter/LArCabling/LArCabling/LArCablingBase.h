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

  virtual ~LArCablingBase();
  
  virtual StatusCode finalize( ) override;


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

  /**
   * return Calibration slot and channel numbers 
   */
  virtual const std::vector<HWIdentifier>& calibSlotLine(const HWIdentifier & id) = 0;

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
   
  bool readOnlOffMap();

private:
  std::vector<Identifier>  m_onlHashToOffline;
  std::vector<HWIdentifier> m_oflHashToOnline;
    
   
};
 
  //Inline methods:

inline bool LArCablingBase::isOnlineConnectedFromHash(const IdentifierHash & sid_hash ) const {
  if (!m_onOffValid) std::abort();
  if (sid_hash <m_onlHashToOffline.size())
    return (m_idEmpty!=m_onlHashToOffline[sid_hash]);
  
  return false;
}


inline HWIdentifier LArCablingBase::createSignalChannelIDFromHash(const IdentifierHash & id_hash) const {
  if (!m_onOffValid) std::abort();
  return m_oflHashToOnline[id_hash];
}


inline Identifier LArCablingBase::cnvToIdentifierFromHash(const IdentifierHash & sid_hash) const {
  if (!m_onOffValid) std::abort();
  if (sid_hash <m_onlHashToOffline.size())
    return m_onlHashToOffline[sid_hash];
  
  return m_idEmpty;
}  

inline Identifier LArCablingBase::cnvToIdentifier(const HWIdentifier & sid) const {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->cnvToIdentifierFromHash(sid_hash); 
 }

inline bool LArCablingBase::isOnlineConnected(const HWIdentifier & sid ) const {
  const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid);
  return this->isOnlineConnectedFromHash(sid_hash);
}  

inline HWIdentifier LArCablingBase::createSignalChannelID(const Identifier & id) const {
  const IdentifierHash id_hash=m_caloId->calo_cell_hash(id);
  return this->createSignalChannelIDFromHash(id_hash);
}


#endif // LARCABLING_LARCABLINGBASE_H
