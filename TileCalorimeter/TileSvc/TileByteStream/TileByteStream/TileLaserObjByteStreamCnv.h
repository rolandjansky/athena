/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

#ifndef TILELASEROBJ_BYTESTREAMCNV_H
#define TILELASEROBJ_BYTESTREAMCNV_H

// Gaudi includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"

#include "eformat/ROBFragment.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IROBDataProviderSvc; 
class TileLaserObjByteStreamTool ; 
class TileROD_Decoder;
class TileLaserObject;
class TileHid2RESrcID;

#include <vector>
#include <stdint.h>


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class TileLaserObjByteStreamCnv
  : public Converter
  , public ::AthMessaging
 {
  public:
    TileLaserObjByteStreamCnv(ISvcLocator* svcloc);

    typedef TileLaserObjByteStreamTool  BYTESTREAMTOOL ;

    virtual StatusCode initialize() override;
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;
    
    /// Storage type and class ID
    virtual long repSvcType() const override { return i_repSvcType(); }
    static long storageType();
    static const CLID& classID();
    
  private: 

    std::string m_name;

    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;
    
    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;

    std::vector<uint32_t> m_ROBID;

    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re;
};
#endif
