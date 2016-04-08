/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAM_MDTPRDCOLLCALIBSTREAMCNV_H
#define MUONCALIBSTREAM_MDTPRDCOLLCALIBSTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonPrepRawData/MdtPrepDataCollection.h"

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

class MdtPRDCollCalibStreamCnv: public Converter {
  friend class CnvFactory<MdtPRDCollCalibStreamCnv>;

 public:
  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return MuonCalibStream_StorageType;}
  static long storageType()     { return MuonCalibStream_StorageType; }
  static const CLID& classID();

 protected:
  MdtPRDCollCalibStreamCnv(ISvcLocator* svcloc);
  ~MdtPRDCollCalibStreamCnv();

 private: 
  StatusCode fillCollections(Muon::MdtPrepDataCollection*& mdtCollection, const unsigned long* ipar);
  StoreGateSvc* m_storeGate; 
  MuonCalibStreamDataProviderSvc * m_dataProvider;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  MdtCalibrationSvc* m_calibSvc;
  const MdtIdHelper* m_mdtIdHelper;

};
#endif
