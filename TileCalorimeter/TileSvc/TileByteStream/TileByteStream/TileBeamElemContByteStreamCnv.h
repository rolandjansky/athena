/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"

#include "eformat/ROBFragment.h"
#include "eformat/FullEventFragment.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class ByteStreamCnvSvc; 
class StoreGateSvc; 
class MsgStream; 
class TileBeamElemContainer; 
class IROBDataProviderSvc;
class TileHid2RESrcID;
class TileROD_Decoder;

#include <stdint.h>
#include <vector> 


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals 
extern long ByteStream_StorageType;

/**
 * @class TileBeamElemContByteStreamCnv
 * @brief This Converter class provides conversion from ByteStream to TileBeamElemContainer
 * @author Alexander Solodkov
 */

class TileBeamElemContByteStreamCnv: public Converter {
  friend class CnvFactory<TileBeamElemContByteStreamCnv>;

 protected:
  TileBeamElemContByteStreamCnv(ISvcLocator* svcloc);

 public:

  virtual StatusCode initialize();
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); 
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);
  virtual StatusCode finalize();

  /// Storage type and class ID
  virtual long repSvcType() const  { return ByteStream_StorageType; }
  static long storageType()  { return ByteStream_StorageType; }
  static const CLID& classID();

  inline const eformat::FullEventFragment<const uint32_t*> * eventFragment() const { return m_event; }
  inline const eformat::ROBFragment<const uint32_t*> * robFragment() const { return m_robFrag; }
  inline bool  validBeamFrag() const { return m_robFrag != 0; }
  
 private: 

   /** Pointer to TileBeamElemContainer */
   TileBeamElemContainer* m_container ; 

   /** Pointer to StoreGateSvc */
   StoreGateSvc* m_storeGate; 

   const eformat::FullEventFragment<const uint32_t*> * m_event;
   //const EventFormat::Abstract::FullEventFragment * m_event;
   //const EventFormat::FullEventHeader m_eventHeader;

   const eformat::ROBFragment<const uint32_t*> * m_robFrag;
   // const EventFormat::Abstract::RODFragment * m_rodFrag;
   // const EventFormat::RODHeader m_RODHeader;

   /** Pointer to IROBDataProviderSvc */
   IROBDataProviderSvc* m_RobSvc;

   std::vector<uint32_t> m_ROBID;

  /** Pointer to TileROD_Decoder */
  TileROD_Decoder* m_decoder;

  /** Pointer to TileHid2RESrcID */
  const TileHid2RESrcID * m_hid2re; 
};
#endif

