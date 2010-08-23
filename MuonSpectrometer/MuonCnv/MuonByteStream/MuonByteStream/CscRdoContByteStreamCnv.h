/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCRDOCONTRAWEVENTCNV_H
#define MUONBYTESTREAM_CSCRDOCONTRAWEVENTCNV_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCSC_CnvTools/ICSC_RDOtoByteStreamTool.h"

class DataObject;
class StatusCode;
class IByteStreamEventAccess;
class StoreGateSvc; 

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

/*
  ByteStream Converter between CSC RDO <---> BYTE STREAM

  @author Ketevi A. Assamagan
  BNL December 27 2003
*/
class CscRdoContByteStreamCnv: public Converter 
{
  friend class CnvFactory<CscRdoContByteStreamCnv>;

protected:
  CscRdoContByteStreamCnv(ISvcLocator* svcloc);
  ~CscRdoContByteStreamCnv();

public:

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/){
    return StatusCode::FAILURE;
  }
  
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static long storageType() { return ByteStream_StorageType;}
  static const CLID& classID();

private: 

   ToolHandle<Muon::ICSC_RDOtoByteStreamTool>               m_tool; 
   //CSC_Hid2RESrcID m_hid2re; 

   ServiceHandle<IByteStreamEventAccess>    m_byteStreamEventAccess; 
   //CscRawDataContainer* m_container; 
   ServiceHandle<StoreGateSvc>              m_storeGate;
   //CSCcablingSvc * m_cabling;
  
};

#endif



