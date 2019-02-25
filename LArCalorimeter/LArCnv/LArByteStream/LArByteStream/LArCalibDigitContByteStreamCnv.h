//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARCALIBDIGITCONTRAWEVENTCNV_H
#define LARBYTESTREAM_LARCALIBDIGITCONTRAWEVENTCNV_H

#include <stdint.h>
#include <map>
#include <string>
#include "GaudiKernel/Converter.h"
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
//#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
//#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class LArCalibDigitContainer; 
class LArRawDataContByteStreamTool ; 
class IROBDataProviderSvc; 
class ByteStreamCnvSvc;

/** This class is the coverter to read/write LArCalibDigitContainer from/to ByteStream
   * @author W. Lampl, R. Lafaye
*/

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class LArCalibDigitContByteStreamCnv: public Converter {
  
public:
  LArCalibDigitContByteStreamCnv(ISvcLocator* svcloc);
  virtual ~LArCalibDigitContByteStreamCnv();

  typedef LArRawDataContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static long storageType()     { return ByteStreamAddress::storageType(); }
  static const CLID& classID();

private: 
  MsgStream* m_log;
  BYTESTREAMTOOL* m_tool ; 
   //IByteStreamEventAccess* m_ByteStreamEventAccess; 
  ByteStreamCnvSvc* m_ByteStreamEventAccess;
//   ByteStreamInputSvc *m_inputSvc;
  IROBDataProviderSvc *m_rdpSvc;
  StoreGateSvc* m_storeGate; 
};
#endif



