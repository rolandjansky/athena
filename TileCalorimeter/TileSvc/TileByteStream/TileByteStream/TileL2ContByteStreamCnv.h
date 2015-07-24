/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEL2_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEL2_BYTESTREAMCNV_H

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
  : public Converter
  , public IIncidentListener
  , public ::AthMessaging
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

    std::string m_name;

    //    BYTESTREAMTOOL* m_tool ;
    ToolHandle<BYTESTREAMTOOL> m_tool;
    
    ServiceHandle<IByteStreamEventAccess> m_byteStreamEventAccess;
    ByteStreamCnvSvc* m_byteStreamCnvSvc;
    
    /** Pointer to StoreGateSvc */
    ServiceHandle<StoreGateSvc> m_storeGate; 
    
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;
    
    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;
    
    /** Pointer to TileL2Container */
    TileL2Container* m_container; 
    
};
#endif

