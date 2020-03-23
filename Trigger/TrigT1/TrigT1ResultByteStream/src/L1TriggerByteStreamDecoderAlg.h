/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_L1TRIGGERBYTESTREAMDECODERALG_H
#define TRIGT1RESULTBYTESTREAM_L1TRIGGERBYTESTREAMDECODERALG_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

/** @class L1TriggerByteStreamDecoderAlg
 *  @brief Algorithm calling tools to convert L1 ByteStream into xAOD collections
 **/
class L1TriggerByteStreamDecoderAlg : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  L1TriggerByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Tool/Service handles ----------------------------
  /// Tool performing the decoding work
  ToolHandleArray<IL1TriggerByteStreamTool> m_decoderTools {
    this, "DecoderTools", {}, "Array of tools performing the decoding work"};
  /// ROBDataProvider service handle
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc {
    this, "ROBDataProviderSvc", "ROBDataProviderSvc", "ROB data provider"};
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERBYTESTREAMDECODERALG_H
