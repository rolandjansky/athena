/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H

// Gaudi includes
#include "AthenaBaseComps/AthConstConverter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ContextSpecificPtr.h"


// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "TileEvent/TileMutableBeamElemContainer.h"
#include "AthenaKernel/RecyclableDataObject.h"


#include "eformat/ROBFragment.h"
#include "eformat/FullEventFragment.h"

#include "CxxUtils/checker_macros.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IROBDataProviderSvc;
class TileHid2RESrcID;
class TileROD_Decoder;

#include <stdint.h>
#include <vector> 


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/**
 * @class TileBeamElemContByteStreamCnv
 * @brief This AthConstConverter class provides conversion from ByteStream to TileBeamElemContainer
 * @author Alexander Solodkov
 */

class TileBeamElemContByteStreamCnv
  : public AthConstConverter
{
  public:
    TileBeamElemContByteStreamCnv(ISvcLocator* svcloc);

    virtual StatusCode initialize() override;
    virtual StatusCode createObjConst(IOpaqueAddress* pAddr, DataObject*& pObj) const override;
    virtual StatusCode createRepConst(DataObject* pObj, IOpaqueAddress*& pAddr) const override;
    virtual StatusCode finalize() override;

    /// Storage type and class ID
    virtual long repSvcType() const override { return i_repSvcType(); }
    static long storageType();
    static const CLID& classID();

    // FIXME: Eliminate this backdoor access to RAW data.
    //        If clients need raw data, they should get it from the
    //        existing ByteStreamSvc interfaces.
    inline const eformat::FullEventFragment<const uint32_t*> * eventFragment() const { return m_event.get(); }
    inline const eformat::ROBFragment<const uint32_t*> * robFragment() const { return m_robFrag.get(); }
    inline bool  validBeamFrag() const { return m_robFrag.isValid(); }
  
  private: 
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;

    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;

    mutable Gaudi::Hive::ContextSpecificPtr<
      const eformat::FullEventFragment<const uint32_t*> > m_event ATLAS_THREAD_SAFE;

    mutable Gaudi::Hive::ContextSpecificPtr<
      const eformat::ROBFragment<const uint32_t*> > m_robFrag  ATLAS_THREAD_SAFE;

    std::vector<uint32_t> m_ROBID;

    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re;

    /** Queue of data objects to recycle. */
    mutable Athena::RecyclableDataQueue<TileMutableBeamElemContainer> m_queue ATLAS_THREAD_SAFE;
};
#endif

