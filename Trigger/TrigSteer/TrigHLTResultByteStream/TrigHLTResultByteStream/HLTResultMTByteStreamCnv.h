/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H
#define TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/Converter.h"

// Externals
extern unsigned char ByteStream_StorageType;

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
    static unsigned char storageType() {return ByteStream_StorageType;} 
    /// CLID of the class HLTResultMT converted by this converter
    static const CLID& classID();

    long repSvcType() const override { return i_repSvcType(); } //!< return repSvcType
  };
} // namespace HLT

#endif // TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamCnv_H
