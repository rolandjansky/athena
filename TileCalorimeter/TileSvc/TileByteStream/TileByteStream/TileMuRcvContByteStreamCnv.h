/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMCNV_H

// Gaudi includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"

#include "TileEvent/TileContainer.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class IROBDataProviderSvc; 
class TileMuRcvContByteStreamTool ; 
class ByteStreamCnvSvc;
class TileROD_Decoder;

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
  : public Converter
  , public IIncidentListener
  , public ::AthMessaging
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

  std::string m_name;

  /** Pointer to TileMuRcvContByteStreamTool */
  ToolHandle<BYTESTREAMTOOL> m_tool;

  ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
  ByteStreamCnvSvc* m_byteStreamCnvSvc;

  /** Pointer to StoreGateSvc */
  ServiceHandle<StoreGateSvc> m_storeGate;

  /** Pointer to IROBDataProviderSvc */
  ServiceHandle<IROBDataProviderSvc> m_robSvc;

  /** Pointer to TileROD_Decoder */
  ToolHandle<TileROD_Decoder> m_decoder;

  /** Pointer to TileMuRcvContainer */
  TileMuonReceiverContainer* m_container;

};
#endif

