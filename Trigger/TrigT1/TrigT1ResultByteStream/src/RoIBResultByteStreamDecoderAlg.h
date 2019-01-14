/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMDECODERALG_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMDECODERALG_H

// Trigger includes
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"
#include "TrigT1Result/RoIBResult.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/WriteHandle.h"

/** @class RoIBResultByteStreamDecoderAlg
 *  @brief Algorithm creating RoIBResult from ByteStream representation
 **/
class RoIBResultByteStreamDecoderAlg : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  RoIBResultByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Properties --------------------------------------
  /// StoreGate key for the output RoIBResult
  SG::WriteHandleKey<ROIB::RoIBResult> m_roibResultWHKey {this, "RoIBResultWHKey", "RoIBResult",
                                                          "Key of the output RoIBResult object"};

  // ------------------------- Tool/Service handles ----------------------------
  /// Tool performing the decoding work
  ToolHandle<RoIBResultByteStreamTool> m_decoderTool {this, "DecoderTool", "RoIBResultByteStreamTool",
                                                      "Tool performing the decoding work"};
  /// ROBDataProvider service handle
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;

  // ------------------------- Other private members ---------------------------
  /// ROB IDs to be requested from ROBDataProviderSvc for decoding
  std::vector<uint32_t> m_robIdsToDecode;
};

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMDECODERALG_H
