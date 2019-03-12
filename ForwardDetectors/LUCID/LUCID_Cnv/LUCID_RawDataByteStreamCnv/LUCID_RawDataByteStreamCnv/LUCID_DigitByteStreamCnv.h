/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// *************************************************************************
//
//  This package is unashameless based on the same
//  a equivalent package for the BCM. 
//  
// *************************************************************************

#ifndef LUCID_DIGITBYTESTREAMCNV_H
#define LUCID_DIGITBYTESTREAMCNV_H

#include <stdint.h>
#include <map>
#include <string>

#include "GaudiKernel/Converter.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"

#include "eformat/SourceIdentifier.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGate.h"

#include "AthenaKernel/MsgStreamMember.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h"

#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include "LUCID_RawDataByteStreamCnv/LUCID_RodEncoder.h"

typedef std::map<uint32_t, LUCID_RodEncoder> LucidRodEncoder_map;

class LUCID_DigitByteStreamCnv: public Converter {

 public:

  LUCID_DigitByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;

  // create the RawEvent fragments for LUCID
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

  StatusCode fillFEA(LUCID_DigitContainer* RDO_container, RawEventWrite* re);

  unsigned short RodBlockVersion(void) { return m_RodBlockVersion; }
  int            BCs_per_LVL1ID (void) { return m_BCs_per_LVL1ID; }
  
  unsigned int getSourceID() { return 0x00820000; }

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  bool msgLevel (MSG::Level lvl){ return m_msg.get().level() <= lvl; }

private: 
  
  IByteStreamEventAccess* m_ByteStreamEventAccess;
  StoreGateSvc* m_StoreGate;

  mutable Athena::MsgStreamMember m_msg;
  
  FullEventAssembler<SrcIdMap> m_fea;
  unsigned short m_RodBlockVersion;
  int            m_BCs_per_LVL1ID;
};
#endif
