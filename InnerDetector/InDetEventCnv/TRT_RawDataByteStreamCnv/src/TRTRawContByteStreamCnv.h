/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTCNV_H
#define TRT_RAWDATABYTESTREAMCNV_TRTRAWCONTRAWEVENTCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "TRT_RawDataByteStreamCnv/ITRTRawContByteStreamTool.h"

class DataObject;
class TRTRawContByteStreamTool ; 

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// the converter for writing BS from TRT Raw Data

class TRTRawContByteStreamCnv: public Converter {
 public:
  TRTRawContByteStreamCnv(ISvcLocator* svcloc);

  typedef TRT_RDO_Container       TRTRawContainer; 

  //! Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType()      { return ByteStreamAddress::storageType(); }
  static const CLID& classID()    { return ClassID_traits<TRTRawContainer>::ID(); }
  
  //! initialize
  virtual StatusCode initialize() override;
  
  //! create Obj is not used !
  virtual StatusCode createObj(IOpaqueAddress* /* pAddr */, DataObject*& /* pObj */) override
    { return StatusCode::FAILURE;}

  //! this creates the RawEvent fragments for the TRT
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

private: 
  // for BS infrastructure
  ToolHandle<ITRTRawContByteStreamTool>  m_tool;                  // ME: use tool handles
  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess; // ME: use service handle
};
#endif

