/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCRDOCONTRAWEVENTCNV_H
#define MUONBYTESTREAM_CSCRDOCONTRAWEVENTCNV_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCSC_CnvTools/ICSC_RDOtoByteStreamTool.h"
#include "AthenaBaseComps/AthConstConverter.h"

class DataObject;
class StatusCode;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


/*
  ByteStream Converter between CSC RDO <---> BYTE STREAM

  @author Ketevi A. Assamagan
  BNL December 27 2003
*/
class CscRdoContByteStreamCnv: public AthConstConverter 
{

public:
  CscRdoContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;
  virtual StatusCode createObjConst(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) const override {
    return StatusCode::FAILURE;
  }
  
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

private: 
   ToolHandle<Muon::ICSC_RDOtoByteStreamTool>               m_tool; 
};

#endif



