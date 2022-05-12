//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARLATOMEHEADERCONTRAWEVENTCNV_H
#define LARBYTESTREAM_LARLATOMEHEADERCONTRAWEVENTCNV_H

#include <stdint.h>
#include <map>
#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "AthenaBaseComps/AthConstConverter.h"


class DataObject;
class IROBDataProviderSvc; 
class LArLATOMEDecoder;


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class LArLATOMEHeaderContByteStreamCnv: public AthConstConverter {

public: 
  LArLATOMEHeaderContByteStreamCnv(ISvcLocator* svcloc);
  virtual ~LArLATOMEHeaderContByteStreamCnv();

  virtual StatusCode initialize() override;
  virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override; 
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType()     { return ByteStreamAddress::storageType(); }
  static const CLID& classID();

private: 
   ToolHandle<LArLATOMEDecoder> m_scTool;
   ServiceHandle<IROBDataProviderSvc> m_rdpSvc;
};
#endif



