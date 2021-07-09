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
#include "ByteStreamData/ByteStreamMetadataContainer.h"

/** @class L1TriggerByteStreamDecoderAlg
 *  @brief Algorithm calling tools to convert L1 ByteStream into xAOD collections
 **/
class L1TriggerByteStreamDecoderAlg : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  L1TriggerByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
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

  // ------------------------- Properties --------------------------------------
  /// StoreGate key for the ByteStreamMetadata container to retrieve detector mask
  SG::ReadHandleKey<ByteStreamMetadataContainer> m_bsMetaDataContRHKey {
    this, "ByteStreamMetadataRHKey", "InputMetaDataStore+ByteStreamMetadata",
    "Key of the ByteStreamMetadataContainer to retrieve the detector mask"
  };
  /// Allow some ROBs to be missing
  Gaudi::Property<std::vector<uint32_t>> m_maybeMissingRobsProp {
    this, "MaybeMissingROBs", {},
    "List of ROB IDs allowed to be missing. If a decoder tool requests one of these "
    "and it is not available in the event, no errors will be reported"
  };
  /// Set behaviour for non-zero ROB status words
  Gaudi::Property<std::string> m_robStatusCheckLevel {
    this, "ROBStatusCheckLevel", "Warning",
    "ROB status word check behaviour. Can be 'None' - status is not checked, 'Warning' - only print warnings "
    "for non-zero status, 'Error' - only print errors for non-zero status, 'Fatal' - return FAILURE "
    "from the algorithm if non-zero status is found."
  };

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
  /// Check ROB status word and report if different from zero
  StatusCode checkRobs(const IROBDataProviderSvc::VROBFRAG& robs, std::string_view toolName="UnknownTool") const;

  // ------------------------- Other private members ---------------------------
  /// Vector of ROB IDs to request, filled from all decoder tools in initialize
  std::vector<uint32_t> m_robIds;
  /// Set of ROB IDs allowed to be missing because they are disabled
  std::set<uint32_t> m_maybeMissingRobs;
  /// The behaviour for non-zero ROB status words
  enum class ROBStatusCheck {Undefined=-1, None, Warning, Error, Fatal} m_robStatusCheck{ROBStatusCheck::Undefined};
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERBYTESTREAMDECODERALG_H
