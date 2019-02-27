/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_TGCRDOCONTRAWEVENTCNV_H
#define MUONBYTESTREAM_TGCRDOCONTRAWEVENTCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCnvToolInterfaces/ITGC_RDOtoByteStreamTool.h"

class DataObject;
class StatusCode;
class IByteStreamEventAccess;
class StoreGateSvc;
class TgcRdoContainer;

namespace Muon {
    class TgcRdoContByteStreamTool;
}

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/*
  ByteStream Converter between TGC bareRDO and BS

  @author Tadashi Maeno
      based on RpcPadContByteStreamCnv by Ketevi A. Assamagan
*/
class TgcRdoContByteStreamCnv: public Converter
{

public:
  TgcRdoContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;

  virtual StatusCode createObj(IOpaqueAddress* /**pAddr*/, DataObject*& /**pObj*/) override {
    return StatusCode::FAILURE;
  }

  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

private:

   ToolHandle<Muon::ITGC_RDOtoByteStreamTool>  m_tool;
   ServiceHandle<IByteStreamEventAccess>       m_byteStreamEventAccess; 
   ServiceHandle<StoreGateSvc>                 m_storeGate;
};

#endif

