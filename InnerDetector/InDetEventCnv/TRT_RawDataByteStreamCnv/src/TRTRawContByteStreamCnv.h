/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTCNV_H
#define TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "TRT_RawDataByteStreamCnv/ITRTRawContByteStreamTool.h"

class DataObject;
class TRTRawContByteStreamTool ; 
class IByteStreamEventAccess   ;

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

// the converter for writing BS from TRT Raw Data

class TRTRawContByteStreamCnv: public Converter {
  friend class CnvFactory<TRTRawContByteStreamCnv>;
  
  ~TRTRawContByteStreamCnv( );

 protected:

  TRTRawContByteStreamCnv(ISvcLocator* svcloc);
  
 public:
  typedef TRT_RDO_Container       TRTRawContainer; 

  //! Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType() { return ByteStream_StorageType; }
  static const CLID& classID()    { return ClassID_traits<TRTRawContainer>::ID(); }
  
  //! initialize
  virtual StatusCode initialize();
  
  //! create Obj is not used !
  virtual StatusCode createObj(IOpaqueAddress* /* pAddr */, DataObject*& /* pObj */)
    { return StatusCode::FAILURE;}

  //! this creates the RawEvent fragments for the TRT
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

private: 
  // for BS infrastructure
  ToolHandle<ITRTRawContByteStreamTool>  m_tool;                  // ME: use tool handles
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; // ME: use service handle
};
#endif

