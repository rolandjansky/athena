/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TileEvent/TileContainer.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class IROBDataProviderSvc; 
class TileMuRcvContByteStreamTool ; 
class ByteStreamCnvSvc;
class TileROD_Decoder;
class TileHid2RESrcID;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

/**
 * @class TileMuRcvContByteStreamCnv
 * @brief This Converter class provides conversion between ByteStream and TileMuRcvCont
 * @author Joao Gentil Saraiva
 *
 * This class provides methods to convert the bytestream data into TileMuRcv objects and vice versa.
 */

class TileMuRcvContByteStreamCnv
  : public Converter,
    public IIncidentListener
{
  friend class CnvFactory<TileMuRcvContByteStreamCnv>;

 protected:
  TileMuRcvContByteStreamCnv(ISvcLocator* svcloc);

 public:

  typedef TileMuRcvContByteStreamTool  BYTESTREAMTOOL ; 

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

  /** Pointer to TileMuRcvContByteStreamTool */
  BYTESTREAMTOOL* m_tool ; 

  /** Pointer to TileROD_Decoder */
  TileROD_Decoder* m_decoder;

  /** Pointer to ByteStreamCnvSvc */
  ByteStreamCnvSvc* m_ByteStreamEventAccess;

  /** Pointer to TileMuRcvContainer */
  TileMuonReceiverContainer* m_container ; 

  /** Pointer to StoreGateSvc */
  StoreGateSvc* m_storeGate;

  /** Pointer to IROBDataProviderSvc */
  IROBDataProviderSvc *m_rdpSvc;
};
#endif

