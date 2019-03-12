//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTBEAM_BYTESTREAM_DATA_CNV
#define TESTBEAM_BYTESTREAM_DATA_CNV


#include "TBCnv/TBByteStreamCnvTool.h"
#include "GaudiKernel/Converter.h"

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

template< class DATATYPE >
class TBDataCnv: public Converter {

 public:
  TBDataCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

 private:
  TBByteStreamCnvTool* m_tool;  //Tool that does the actual conversion

};

#include "TBCnv/TBDataCnv.icc"

#endif
