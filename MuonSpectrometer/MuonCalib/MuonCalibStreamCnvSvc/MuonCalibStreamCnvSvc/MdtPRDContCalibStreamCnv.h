/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAM_MDTPRDCONTCALIBSTREAMCNV_H
#define MUONCALIBSTREAM_MDTPRDCONTCALIBSTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

class DataObject;
class StatusCode;
class StoreGateSvc; 
class MuonCalibStreamDataProviderSvc;
class MdtCalibrationSvc;

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long MuonCalibStream_StorageType;

class MdtPRDContCalibStreamCnv: public Converter {
  friend class CnvFactory<MdtPRDContCalibStreamCnv>;

 public:
  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return MuonCalibStream_StorageType;}
  static long storageType()     { return MuonCalibStream_StorageType; }
  static const CLID& classID();

 protected:
  MdtPRDContCalibStreamCnv(ISvcLocator* svcloc);
  ~MdtPRDContCalibStreamCnv();

 private: 
  StatusCode fillCollections(IOpaqueAddress* pAddr, DataObject*& pObj);
  Muon::MdtPrepDataContainer* m_container ; 
  StoreGateSvc* m_storeGate; 
  MuonCalibStreamDataProviderSvc * m_dataProvider;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  MdtCalibrationSvc* m_calibSvc;
  const MdtIdHelper* m_mdtIdHelper;
  int m_mdtOffset;

};
#endif



