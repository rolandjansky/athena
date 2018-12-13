/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderAlg_H
#define TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderAlg_H

// Trigger includes
#include "HLTResultMTByteStreamDecoderTool.h"
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/WriteHandle.h"

/** @class HLTResultMTByteStreamDecoderAlg
 *  @brief Algorithm creating HLTResultMT from ByteStream representation
 **/
class HLTResultMTByteStreamDecoderAlg : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  HLTResultMTByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Properties --------------------------------------
  /// Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_moduleIdsToDecode {
    this, "ModuleIdsToDecode", {0,1,2,3,4,5,6},
    "List of module IDs from which payload will be decoded"
  };
  /// StoreGate key for the output HLTResultMT
  SG::WriteHandleKey<HLT::HLTResultMT> m_hltResultWHKey {this, "HLTResultWHKey", "HLTResultMT",
                                                         "Key of the output HLTResultMT object"};

  // ------------------------- Tool/Service handles ----------------------------
  /// Tool performing the decoding work
  ToolHandle<HLTResultMTByteStreamDecoderTool> m_decoderTool {this, "DecoderTool", "HLTResultMTByteStreamDecoderTool",
                                                              "Tool performing the decoding work"};
  /// ROBDataProvider service handle
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;

  // ------------------------- Other private members ---------------------------
  /// Full ROB IDs constructed from the ModuleIdsToDecode property
  std::vector<uint32_t> m_robIdsToDecode;
};

#endif // TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderAlg_H
