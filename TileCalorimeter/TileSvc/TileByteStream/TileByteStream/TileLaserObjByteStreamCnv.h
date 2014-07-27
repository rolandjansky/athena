/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

#ifndef TILELASEROBJ_BYTESTREAMCNV_H
#define TILELASEROBJ_BYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"

#include "eformat/ROBFragment.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class StoreGateSvc; 
class MsgStream; 
class IROBDataProviderSvc; 
class TileLaserObjByteStreamTool ; 
class ByteStreamCnvSvc;
class TileROD_Decoder;
class TileLaserObject;

#include <vector>
#include <stdint.h>


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

class TileLaserObjByteStreamCnv: public Converter {
  friend class CnvFactory<TileLaserObjByteStreamCnv>;

 protected:
  TileLaserObjByteStreamCnv(ISvcLocator* svcloc);

 public:

  typedef TileLaserObjByteStreamTool  BYTESTREAMTOOL ;

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Storage type and class ID
  virtual long repSvcType() const  { return ByteStream_StorageType; }
  static long storageType()  { return ByteStream_StorageType; }
  static const CLID& classID();

private: 

  BYTESTREAMTOOL* m_tool ;  

  /* Pointer to TileROD_Decoder */
  TileROD_Decoder* m_decoder;

  /* Pointer to ByteStreamCnvSvc */
  ByteStreamCnvSvc* m_ByteStreamEventAccess;
  /* Pointer to TileLaserObject */
  TileLaserObject* m_container;
  /* Poiner to StoreGateSvc */
  StoreGateSvc* m_storeGate;

  const eformat::ROBFragment<const uint32_t*> * m_robFrag;

  /** Pointer to IROBDataProviderSvc */
  IROBDataProviderSvc* m_rdpSvc;

  std::vector<uint32_t> m_ROBID;

};
#endif
