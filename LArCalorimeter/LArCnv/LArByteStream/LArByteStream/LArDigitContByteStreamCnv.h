//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARDIGITCONTRAWEVENTCNV_H
#define LARBYTESTREAM_LARDIGITCONTRAWEVENTCNV_H

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
class LArDigitContainer; 
class LArRawDataContByteStreamTool ; 
class IROBDataProviderSvc; 
class ByteStreamCnvSvc;
class LArABBADecoder;

/** This class is the coverter to read/write LArDigitContainer from/to ByteStream
   * @author W. Lampl, R. Lafaye
*/

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

class LArDigitContByteStreamCnv: public Converter {
  friend class CnvFactory<LArDigitContByteStreamCnv>;

public: 
  LArDigitContByteStreamCnv(ISvcLocator* svcloc);
  virtual ~LArDigitContByteStreamCnv();


  typedef LArRawDataContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType; }
  static long storageType()     { return ByteStream_StorageType; }
  static const CLID& classID();

private: 

   MsgStream* m_log;
   BYTESTREAMTOOL* m_tool ; 
   LArABBADecoder* m_scTool;
   ByteStreamCnvSvc* m_ByteStreamEventAccess; 
   IROBDataProviderSvc *m_rdpSvc;
   StoreGateSvc* m_storeGate; 
};
#endif



