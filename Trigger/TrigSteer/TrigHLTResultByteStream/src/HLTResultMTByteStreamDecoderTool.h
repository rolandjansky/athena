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
  /// Fills the HLTResultMT object from the ByteStream event header
  StatusCode decodeHeader(const RawEvent* rawEvent, HLT::HLTResultMT& resultToFill) const;
  /// Fills the HLTResultMT object from vector of ROBFragments
  StatusCode decodePayload(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                           HLT::HLTResultMT& resultToFill) const;
};

#endif // TRIGHLTRESULTBYTESTREAM_HLTResultMTByteStreamDecoderTool_H
