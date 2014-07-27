/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H
#define TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class TileRawChannelContainer; 
class TileRawChannelContByteStreamTool ; 
class ByteStreamCnvSvc;
class IROBDataProviderSvc; 
class TileHid2RESrcID;
class TileROD_Decoder;


/**
 * @class TileRawChannelContByteStreamCnv
 * @brief Contains methods to do the conversion from bytestream format to TileRawChannel stored in a TileRawChannelContainer and vice-versa making use of the TileRawChannelContByteStreamTool.
 */


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

class TileRawChannelContByteStreamCnv
  : public Converter,
    public IIncidentListener
{
  friend class CnvFactory<TileRawChannelContByteStreamCnv>;

 protected:
  TileRawChannelContByteStreamCnv(ISvcLocator* svcloc);

 public:

  typedef TileRawChannelContByteStreamTool  BYTESTREAMTOOL ; 

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);
  virtual StatusCode finalize();

  /// Incident listener
  virtual void handle( const Incident& incident );

  /// Storage type and class ID
  virtual long repSvcType() const  { return ByteStream_StorageType; }
  static long storageType()  { return ByteStream_StorageType; }
  static const CLID& classID();

private: 

  BYTESTREAMTOOL* m_tool ; 

  ByteStreamCnvSvc* m_ByteStreamEventAccess;

  /** Pointer to TileRawChannelContainer */
  TileRawChannelContainer* m_container ; 
  
   /** Pointer to StoreGateSvc */
  StoreGateSvc* m_storeGate; 

  /** Pointer to IROBDataProviderSvc */
  IROBDataProviderSvc* m_RobSvc;

  /** Pointer to TileROD_Decoder */
  TileROD_Decoder * m_decoder;

  /** Pointer to TileHid2RESrcID */
  const TileHid2RESrcID * m_hid2re; 
};
#endif

