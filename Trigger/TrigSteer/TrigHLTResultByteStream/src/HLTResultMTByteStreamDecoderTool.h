/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderTool_H
#define TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderTool_H

// Trigger includes
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"

/** @class HLTResultMTByteStreamDecoderTool
 *  @brief Tool filling an HLTResultMT from ByteStream event data
 **/
class HLTResultMTByteStreamDecoderTool : virtual public AthAlgTool {
public:
  /// Standard constructor
  HLTResultMTByteStreamDecoderTool(const std::string& type, const std::string& name, const IInterface* parent);

  // ------------------------- AthAlgTool methods ------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- Specific methods of this class ------------------
  /**
   * Retrieves the HLT ROD minor version from vector of ROBFragments, checking it is the same in all of them
   *
   * @param[in] vrobf A vector of HLT ROBFragments to decode (subdetector identifier TDAQ_HLT)
   * @return The decoded minor version number or {0xff,0xff} in case of errors
   */
  HLT::HLTResultMT::RODMinorVersion getHltRodMinorVersion(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf) const;
  /**
   * Fills the HLTResultMT object from the ByteStream event header
   *
   * The decoding may depend on HLT ROD Minor version. The passed HLTResultMT should have a valid version set.
   *
   * @param[in] rawEvent A pointer to the FullEventFragment to decode
   * @param[out] resultToFill The HLTResultMT object to be updated with the decoded information
   * @return FAILURE if anything goes wrong, else SUCCESS
   */
  StatusCode decodeHeader(const RawEvent* rawEvent, HLT::HLTResultMT& resultToFill) const;
  /**
   * Fills the HLTResultMT object from vector of ROBFragments
   *
   * The decoding may depend on HLT ROD Minor version. The passed HLTResultMT should have a valid version set.
   *
   * @param[in] vrobf A vector of HLT ROBFragments to decode (subdetector identifier TDAQ_HLT)
   * @param[out] resultToFill The HLTResultMT object to be updated with the decoded information
   * @return FAILURE if anything goes wrong, else SUCCESS
   */
  StatusCode decodePayload(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                           HLT::HLTResultMT& resultToFill) const;

private:
  // Check if RODMinorVersion is valid and 1.0 or higher
  StatusCode checkRodMinorVersion(const HLT::HLTResultMT::RODMinorVersion version) const;
};

#endif // TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderTool_H
