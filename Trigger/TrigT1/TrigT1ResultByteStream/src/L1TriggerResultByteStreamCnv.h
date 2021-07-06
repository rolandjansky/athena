/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTBYTESTREAMCNV_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Gaudi includes
#include "GaudiKernel/Converter.h"

/** @class L1TriggerResultByteStreamCnv
 *  @brief ByteStream converter for L1TriggerResult
 **/
class L1TriggerResultByteStreamCnv : public Converter, public AthMessaging {
public:
  /// Standard constructor
  L1TriggerResultByteStreamCnv(ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~L1TriggerResultByteStreamCnv();

  // ------------------------- Converter methods -----------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /// Create xAOD (L1TriggerResult) from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  /// Create ByteStream from xAOD (L1TriggerResult)
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  // ------------------------- Converter definition helpers ------------------
  /// Storage type used by this converter
  static long storageType();
  /// CLID of the class of the L1TriggerResult converted by this converter (xAOD::TrigCompositeContainer)
  static const CLID& classID();

  long repSvcType() const override { return i_repSvcType(); } //!< return repSvcType

private:
  /// Helper to obtain the RawEvent pointer
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess{"ByteStreamCnvSvc", "L1TriggerResultByteStreamCnv"};

  // Tools performing the decoding work - *public* tools hard-coded in C++ because of Converter interface limitations
  /// Encoder tool for L1Muon RoIs
  ToolHandle<IL1TriggerByteStreamTool> m_muonEncoderTool{"MuonRoIByteStreamTool/L1MuonBSEncoderTool"};
  // Placeholder for other L1 xAOD outputs:
  // - CTP result
  // - L1Topo result
  // - L1Calo (Run3) RoIs
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTBYTESTREAMCNV_H
