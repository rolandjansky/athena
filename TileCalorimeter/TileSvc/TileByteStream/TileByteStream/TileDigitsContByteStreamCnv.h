/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class TileDigitsContainer; 
class TileDigitsContByteStreamTool;
class ByteStreamCnvSvc; 
class IROBDataProviderSvc; 
class TileHid2RESrcID;
class TileROD_Decoder;


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

/**
 * @class TileDigitsContByteStreamCnv
 * @brief This Converter class provides conversion from ByteStream to TileDigitsContainer
 * @author Alexander Solodkov
 *
 * This class provides methods to convert the bytestream data into TileDigits.
 * The TileDigitsContainer contains a TileDigitsCollection per superdrawer.
 */

class TileDigitsContByteStreamCnv
  : public Converter,
    public IIncidentListener
{
  friend class CnvFactory<TileDigitsContByteStreamCnv>;

 protected:
  TileDigitsContByteStreamCnv(ISvcLocator* svcloc);

 public:
 
  typedef TileDigitsContByteStreamTool  BYTESTREAMTOOL ; 

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

   /** Pointer to TileDigitsContainer */
   TileDigitsContainer* m_container ; 

   /** Pointer to StoreGateSvc */
   StoreGateSvc* m_storeGate; 

  /** Pointer to IROBDataProviderSvc */
  IROBDataProviderSvc* m_RobSvc;

  /** Pointer to TileROD_Decoder */
  TileROD_Decoder* m_decoder;

  /** Pointer to TileHid2RESrcID */
  const TileHid2RESrcID * m_hid2re; 
};
#endif

