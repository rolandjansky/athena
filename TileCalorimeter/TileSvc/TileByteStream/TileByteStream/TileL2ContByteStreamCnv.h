/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEL2_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEL2_BYTESTREAMCNV_H

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
class TileL2ContByteStreamTool ; 
class ByteStreamCnvSvc;
class TileROD_Decoder;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

/**
 * @class TileL2ContByteStreamCnv
 * @brief This Converter class provides conversion between ByteStream and TileL2Container
 * @author Aranzazu Ruiz
 *
 * This class provides methods to convert the bytestream data into
 * TileL2 objects (muon and transverse energy info) and vice versa. The
 * TileL2Container contains a TileL2 object per superdrawer.
 */

class TileL2ContByteStreamCnv
  : public Converter,
    public IIncidentListener
{
  friend class CnvFactory<TileL2ContByteStreamCnv>;

 protected:
  TileL2ContByteStreamCnv(ISvcLocator* svcloc);

 public:

  typedef TileL2ContByteStreamTool  BYTESTREAMTOOL ; 

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

  /** Pointer to TileL2ContByteStreamTool */
  BYTESTREAMTOOL* m_tool ; 

  /** Pointer to TileROD_Decoder */
  TileROD_Decoder* m_decoder;

  /** Pointer to ByteStreamCnvSvc */
  ByteStreamCnvSvc* m_ByteStreamEventAccess;

  /** Pointer to TileL2Container */
  TileL2Container* m_container ; 

  /** Pointer to StoreGateSvc */
  StoreGateSvc* m_storeGate; 

  /** Pointer to IROBDataProviderSvc */
  IROBDataProviderSvc *m_rdpSvc;
};
#endif

