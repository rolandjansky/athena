/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAM_TGCRDOCONTCALIBSTREAMCNV_H
#define MUONCALIBSTREAM_TGCRDOCONTCALIBSTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/TgcRdoContainer.h"

class DataObject;
class StatusCode;
class StoreGateSvc; 
class MuonCalibStreamDataProviderSvc;

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long MuonCalibStream_StorageType;

class TgcRDOContCalibStreamCnv: public Converter {
  friend class CnvFactory<TgcRDOContCalibStreamCnv>;

 protected:
  TgcRDOContCalibStreamCnv(ISvcLocator* svcloc);
  ~TgcRDOContCalibStreamCnv();

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
  int getRodIdFromSectorId(int tmp_sectorId);
  uint16_t bcTagCnv(uint16_t bcBitMap);
    
private: 
  
  StoreGateSvc* m_storeGate; 
  MuonCalibStreamDataProviderSvc * m_dataProvider;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  
  const TgcIdHelper* m_tgcIdHelper;
  
  TgcRdoContainer * m_rdoContainer;
};
#endif
