/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAM_RPCRDOCONTCALIBSTREAMCNV_H
#define MUONCALIBSTREAM_RPCRDOCONTCALIBSTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/RpcPadContainer.h"

#include "RPCcablingInterface/IRPCcablingSvc.h"

class DataObject;
class StatusCode;
class StoreGateSvc; 
class MuonCalibStreamDataProviderSvc;

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long MuonCalibStream_StorageType;

class RpcRDOContCalibStreamCnv: public Converter {
  friend class CnvFactory<RpcRDOContCalibStreamCnv>;

 protected:
  RpcRDOContCalibStreamCnv(ISvcLocator* svcloc);
  ~RpcRDOContCalibStreamCnv();

 public:


  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return MuonCalibStream_StorageType;}
  static long storageType()     { return MuonCalibStream_StorageType; }
  static const CLID& classID();

private: 
  StatusCode fillCollections(DataObject*& pObj);

private: 

  Muon::RpcPrepDataContainer* m_container ; 
  StoreGateSvc* m_storeGate; 
  MuonCalibStreamDataProviderSvc * m_dataProvider;
  const MuonGM::MuonDetectorManager * m_muonMgr;

  const RpcIdHelper* m_rpcIdHelper;
  int m_rpcOffset;

  const IRPCcablingSvc* m_cabling;
  RpcPadContainer * m_padContainer;
};
#endif



