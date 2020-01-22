/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H
#define TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/WriteHandle.h"

/** @class L1TriggerResultMaker
 *  @brief Algorithm creating RoIBResult from ByteStream representation
 **/
class L1TriggerResultMaker : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  L1TriggerResultMaker(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Properties --------------------------------------
  /// StoreGate key for the output RoIBResult
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultWHKey {
    this, "L1TriggerResultWHKey", "L1TriggerResult", "Key of the output L1 Trigger Result"};

  // ------------------------- Tool/Service handles ----------------------------
  /// Tool performing the decoding work
  ToolHandleArray<IL1TriggerByteStreamTool> m_decoderTools {
    this, "DecoderTools", {}, "Array of tools performing the decoding work"};
  /// ROBDataProvider service handle
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc {
    this, "ROBDataProviderSvc", "ROBDataProviderSvc", "ROB data provider"};
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H
