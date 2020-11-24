/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamCnv.h
//   Header file for class BCM_RawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// classID() - return BCM_RDO_Container ID
// createRep() - convert BCM_RawData in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos for pixels
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMCNV_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMCNV_H

#include "AthenaBaseComps/AthConstConverter.h"

#include "ByteStreamData/RawEvent.h" 

class BCM_RawContByteStreamTool;
class IByteStreamEventAccess;
class StoreGateSvc;

class BCM_RawContByteStreamCnv: public AthConstConverter {

 public:


  BCM_RawContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;

  // create the RawEvent fragments for BCM
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  // Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();


private: 
  BCM_RawContByteStreamTool* m_BCMRawContBSTool;
};
#endif
