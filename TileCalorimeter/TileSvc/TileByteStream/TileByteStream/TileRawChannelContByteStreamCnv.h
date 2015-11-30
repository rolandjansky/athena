/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H
#define TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H

// Gauid includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"

#include <vector>

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class ActiveStoreSvc;
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
  : public Converter
  , public IIncidentListener
  , public ::AthMessaging
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

    std::string m_name;

    //    BYTESTREAMTOOL* m_tool ;
    ToolHandle<TileRawChannelContByteStreamTool> m_tool;
    
    ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
    ByteStreamCnvSvc* m_byteStreamCnvSvc;
    
    /** Pointer to StoreGateSvc */
    ServiceHandle<ActiveStoreSvc> m_activeStore; 
    
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;
    
    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;
    
    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re; 

    /** Pointer to TileDigitsContainer */
    std::vector<TileRawChannelContainer*> m_containers; 

};
#endif

