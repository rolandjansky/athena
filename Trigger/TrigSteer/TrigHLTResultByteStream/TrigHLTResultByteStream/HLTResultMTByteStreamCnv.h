/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H
#define TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H

// Trigger includes
#include "TrigHLTResultByteStream/HLTSrcIdMap.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Gaudi includes
#include "GaudiKernel/Converter.h"

namespace HLT {
  /** @class HLTResultMTByteStreamCnv
   *  @brief ByteStream converter for HLTResultMT
   **/
  class HLTResultMTByteStreamCnv : public Converter, public AthMessaging {
  public:
    /// Standard constructor
    HLTResultMTByteStreamCnv(ISvcLocator* svcLoc);
    /// Standard destructor
    virtual ~HLTResultMTByteStreamCnv();

    // ------------------------- Converter methods -----------------------------
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /// Create RDO (HLTResultMT) from ByteStream
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
    /// Create ByteStream from RDO (HLTResultMT)
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

    // ------------------------- Converter definition helpers ------------------
    /// Storage type used by this converter
    static long storageType();
    /// CLID of the class HLTResultMT converted by this converter
    static const CLID& classID();

    long repSvcType() const override { return i_repSvcType(); } //!< return repSvcType

  private:
    /// Helper to obtain the RawEvent pointer
    ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;

    /// Helper for filling ROBFragments
    FullEventAssembler<HLTSrcIdMap> m_fullEventAssembler;

    /// Buffer for serialised StreamTag data
    std::unique_ptr<uint32_t[]> m_streamTagData;
  };
} // namespace HLT

#endif // TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H
