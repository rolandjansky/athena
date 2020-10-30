/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H


#include "AthenaBaseComps/AthConstConverter.h"
#include "TileEvent/TileMutableDigitsContainer.h"
#include "AthenaKernel/RecyclableDataObject.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CxxUtils/checker_macros.h"


class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class MsgStream; 
class TileDigitsContainer; 
class TileDigitsContByteStreamTool;
class ByteStreamCnvSvc; 
class IROBDataProviderSvc; 
class TileHid2RESrcID;
class TileROD_Decoder;

#include <vector>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/**
 * @class TileDigitsContByteStreamCnv
 * @brief This AthConstConverter class provides conversion from ByteStream to TileDigitsContainer
 * @author Alexander Solodkov
 *
 * This class provides methods to convert the bytestream data into TileDigits.
 * The TileDigitsContainer contains a TileDigitsCollection per superdrawer.
 */

class TileDigitsContByteStreamCnv
  : public AthConstConverter
{
  
  public:
    TileDigitsContByteStreamCnv(ISvcLocator* svcloc);
  
    typedef TileDigitsContByteStreamTool  BYTESTREAMTOOL; 
  
    virtual StatusCode initialize() override;
    virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override;
    virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;
    virtual StatusCode finalize() override;
    
    /// Storage type and class ID
    virtual long repSvcType() const override { return i_repSvcType(); }
    static long storageType();
    static const CLID& classID();

  private: 
    //    BYTESTREAMTOOL* m_tool ;
    ToolHandle<TileDigitsContByteStreamTool> m_tool;
    
    ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
    ByteStreamCnvSvc* m_byteStreamCnvSvc;
    
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;
    
    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;
    
    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re; 

    /** Queue of data objects to recycle. */
    mutable Athena::RecyclableDataQueue<TileMutableDigitsContainer> m_queue ATLAS_THREAD_SAFE;
};
#endif

