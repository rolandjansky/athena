/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H
#define TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTCNV_H

#include "AthenaBaseComps/AthConstConverter.h"
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "AthenaKernel/RecyclableDataObject.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <vector>

class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
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


class TileRawChannelContByteStreamCnv
  : public AthConstConverter
{
  public:
    TileRawChannelContByteStreamCnv(ISvcLocator* svcloc);

    typedef TileRawChannelContByteStreamTool  BYTESTREAMTOOL ; 

    virtual StatusCode initialize() override;
    virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override;
    virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;
    virtual StatusCode finalize() override;
    
    /// Storage type and class ID
    virtual long repSvcType() const override  { return i_repSvcType(); }
    static long storageType();
    static const CLID& classID();
    
  private: 
    //    BYTESTREAMTOOL* m_tool ;
    ToolHandle<TileRawChannelContByteStreamTool> m_tool;
    
    ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
    ByteStreamCnvSvc* m_byteStreamCnvSvc;
    
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;
    
    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;
    
    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re; 


    /** Queue of data objects to recycle. */
    mutable Athena::RecyclableDataQueue<TileMutableRawChannelContainer> m_queue ATLAS_THREAD_SAFE;
};
#endif

