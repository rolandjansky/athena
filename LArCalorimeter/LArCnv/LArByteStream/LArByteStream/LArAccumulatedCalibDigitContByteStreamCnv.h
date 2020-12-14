//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARACCUMULATEDCDIGITCONTRAWEVENTCNV_H
#define LARBYTESTREAM_LARACCUMULATEDCDIGITCONTRAWEVENTCNV_H

#include <stdint.h>
#include <map>
#include <string>
#include "AthenaBaseComps/AthConstConverter.h"
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class LArAccumulatedCalibDigitContainer; 
class LArRawDataContByteStreamTool ; 
class IROBDataProviderSvc; 
class ByteStreamCnvSvc;
class LArCalibLineMapping;

/** This class is the converter to read/write LArAccumulatedCalibDigitContainer from/to ByteStream
   * @author W. Lampl, R. Lafaye
*/

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class LArAccumulatedCalibDigitContByteStreamCnv: public AthConstConverter {

public:
  LArAccumulatedCalibDigitContByteStreamCnv(ISvcLocator* svcloc);
  virtual ~LArAccumulatedCalibDigitContByteStreamCnv();


  typedef LArRawDataContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize() override;
  virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override; 
  virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType()     { return ByteStreamAddress::storageType(); }
  static const CLID& classID();

private: 
   ToolHandle<BYTESTREAMTOOL> m_tool;
   ServiceHandle<IROBDataProviderSvc> m_rdpSvc;
   SG::ReadCondHandleKey<LArCalibLineMapping> m_calibLineMappingKey;
};
#endif



