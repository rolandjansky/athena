/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAM_CSCRDOCONTCALIBSTREAMCNV_H
#define MUONCALIBSTREAM_CSCRDOCONTCALIBSTREAMCNV_H

#include <stdint.h>
#include <map>
#include "GaudiKernel/Converter.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/CscRawDataCollection.h"

#include "CSCcabling/CSCcablingSvc.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class StatusCode;
class StoreGateSvc; 
class MuonCalibStreamDataProviderSvc;
namespace Muon{
  class ICSC_ROD_Decoder;
}

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long MuonCalibStream_StorageType;

class CscRDOContCalibStreamCnv: public Converter {
  friend class CnvFactory<CscRDOContCalibStreamCnv>;

 public:
  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return MuonCalibStream_StorageType;}
  static long storageType()     { return MuonCalibStream_StorageType; }
  static const CLID& classID();
  
 protected:
  CscRDOContCalibStreamCnv(ISvcLocator* svcloc);
  ~CscRDOContCalibStreamCnv();

 private: 
  StatusCode fillCollections(DataObject*& pObj);
  /* extractRpu
     When passed, word shoudl point to the first word of the RPU block
     The data inside of the word list is trasnfered to the rawCollection.
     Word after the run will point to last word of the block.*/
  StatusCode extractRpu(std::list<uint32_t>::const_iterator & word, 
      CscRawDataCollection * rawCollection, const uint16_t & numSamples, const uint16_t & idColl);
 
  //int getRodIdFromSectorId(int tmp_sectorId);
  //uint16_t bcTagCnv(uint16_t bcBitMap);
    
 private: 
  StoreGateSvc* m_storeGate; 
  MuonCalibStreamDataProviderSvc * m_dataProvider;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  
  const CscIdHelper* m_cscIdHelper;
 
  std::string m_detdescr; 
  ServiceHandle<CSCcablingSvc> m_cabling;
  StoreGateSvc* m_detStore;
  CscRawDataContainer * m_rdoContainer;
  ToolHandle<Muon::ICSC_ROD_Decoder> m_cscRodDecoderTool;  
  
  bool m_isClusterWordsUnrealistic;

  unsigned int m_numMissingRPUs;
};
#endif
