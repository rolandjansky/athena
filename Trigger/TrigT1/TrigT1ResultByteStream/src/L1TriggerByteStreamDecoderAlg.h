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

  // ------------------------- Helper methods ----------------------------------
  /**
   * @brief Copy over ROBFragment pointers from @c in to @c out for ROBs with IDs from the @c ids list
   *
   * @param[in] in The input vector of ROBFragments
   * @param[out] out The output vector of filtered ROBFragments
   * @param[in] ids A list of ROB IDs to filter from @c in to @c out
   * @param[in] toolName Name of the tool requesting the ROB IDs - used for log messages
   * @returns FAILURE if any requested ROB IDs are missing from the @c in vector, otherwise SUCCESS
   */
  StatusCode filterRobs(const IROBDataProviderSvc::VROBFRAG& in,
                        IROBDataProviderSvc::VROBFRAG& out,
                        const std::vector<uint32_t>& ids,
                        std::string_view toolName="UnknownTool") const;

  // ------------------------- Other private members ---------------------------
  /// Vector of ROB IDs to request, filled from all decoder tools in initialize
  std::vector<uint32_t> m_robIds;
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERBYTESTREAMDECODERALG_H
