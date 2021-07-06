/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCABLING_CSCCABLINGSVC_H
#define CSCCABLING_CSCCABLINGSVC_H

/*----------------------------------------------------
 * Ketevi A. Assamagan - May 2007
 * CSC Cabling service to decode online-offline maps
 *---------------------------------------------------*/

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include <inttypes.h>
#include <atomic>

class MsgStream;

const InterfaceID IID_ICSCcablingSvc("CSCcablingSvc", 1, 0);

class CSCcablingSvc : public AthService, virtual public IInterface {
public:
   
  CSCcablingSvc(const std::string& name,ISvcLocator* sl);
  virtual ~CSCcablingSvc()=default;

  static const InterfaceID& interfaceID() { return IID_ICSCcablingSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

  virtual StatusCode initialize(void);

  /** map offline ROD identifier to online ID */
  bool onlineId(const uint16_t subdetectorID, const uint16_t offlineID, uint32_t& rodId) const;
  bool onlineId(const uint16_t offlineID, uint16_t& rodId) const;

  /** reverse map of online ID into offline ROD ID */
  bool offlineId(const uint32_t onlineIdentifier, uint16_t& rodId ) const; 

  /** check that we have the correct online ROD id */
  bool is_rodId (const uint16_t rodId ) const;

  /** check that we have the correct offline ROD id */
  bool is_offlineRodId (const uint16_t rodId ) const;

  /** calculate the collection Identifier */
  uint16_t collectionId(const uint16_t subdetectorId, const uint16_t rodId) const;

  unsigned int nSide () const { return m_side; }
  unsigned int nROD ()  const { return m_rod; }
  unsigned int maxId()  const { return m_max; }

  /** map PRD collection ID into short ROB ID, e.g. 0x81 **/
  void hash2Rob(const unsigned int&, uint32_t&) const;
  
  /** map PRD collection ID into full ROB ID (with subdetector ID), e.g. 0x6a0081 **/
  void hash2RobFull(const unsigned int&, uint32_t&) const;
  
  /** map PRD collection ID into offline ROD ID **/
  void hash2Rod(const unsigned int&, uint32_t&) const;
  
  /** map PRD collection ID into subdetector ID, i.e. 0x69 or 0x6a **/
  void hash2SubdetectorId(const unsigned int&, uint32_t&) const;
  
  /** map PRD collection ID into RDO collection ID **/
  void hash2CollectionId(const unsigned int&, uint16_t&) const;

 private:

  unsigned int m_side;
  unsigned int m_rod;
  unsigned int m_max;
  bool m_run1;
};

#endif
