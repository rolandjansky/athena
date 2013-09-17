//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTBEAM_BYTESTREAM_DATA_CNV
#define TESTBEAM_BYTESTREAM_DATA_CNV


#include "TBCnv/TBByteStreamCnvTool.h"
#include "GaudiKernel/Converter.h"

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern unsigned char ByteStream_StorageType;

template< class DATATYPE > 
class TBDataCnv: public Converter {
  friend class CnvFactory<TBDataCnv>;

 protected:
  TBDataCnv(ISvcLocator* svcloc);


 public:

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType; }
  static  long storageType()     { return ByteStream_StorageType; }
  static const CLID& classID();

 private:
  TBByteStreamCnvTool* m_tool;  //Tool that does the actual conversion

};

#include "TBCnv/TBDataCnv.icc"

#endif
