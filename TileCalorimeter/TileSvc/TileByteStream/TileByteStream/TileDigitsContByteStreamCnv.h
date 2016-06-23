/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEDIGITS_BYTESTREAMCNV_H

// Gaudi inludes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"



class DataObject;
class StatusCode;
class IAddressCreator;
class IByteStreamEventAccess;
class ActiveStoreSvc;
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
  : public Converter
  , public IIncidentListener
  , public ::AthMessaging
{
    friend class CnvFactory<TileDigitsContByteStreamCnv>;

  protected:
    TileDigitsContByteStreamCnv(ISvcLocator* svcloc);
  
  public:
  
    typedef TileDigitsContByteStreamTool  BYTESTREAMTOOL; 
  
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
    ToolHandle<TileDigitsContByteStreamTool> m_tool;
    
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
    std::vector<TileDigitsContainer*> m_containers; 


};
#endif

