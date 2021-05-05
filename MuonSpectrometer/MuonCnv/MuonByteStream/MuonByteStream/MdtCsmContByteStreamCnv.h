/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTCSMCONTRAWEVENTCNV_H
#define MUONBYTESTREAM_MDTCSMCONTRAWEVENTCNV_H

#include "AthenaBaseComps/AthConstConverter.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "GaudiKernel/ToolHandle.h"
#include "MuonMDT_CnvTools/IMDT_RDOtoByteStreamTool.h"
#include <string>

class MdtCsmContByteStreamCnv: public AthConstConverter {

 public:
  MdtCsmContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;
  virtual StatusCode createObjConst(IOpaqueAddress* /**pAddr*/, DataObject*& /**pObj*/) const override {
    return StatusCode::FAILURE;
  }
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private: 
   ToolHandle<Muon::IMDT_RDOtoByteStreamTool> m_tool ; 
};
#endif



