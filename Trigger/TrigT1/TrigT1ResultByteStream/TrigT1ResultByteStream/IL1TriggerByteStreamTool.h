/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H

#include "ByteStreamData/RawEvent.h"
#include "xAODTrigger/TrigComposite.h"
#include "GaudiKernel/IAlgTool.h"

/**
 * @class IL1TriggerByteStreamTool
 * @brief Interface for AlgTools converting ByteStream to xAOD RoI objects and vice versa
 **/
class IL1TriggerByteStreamTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IL1TriggerByteStreamTool, 1, 0);
  virtual ~IL1TriggerByteStreamTool() override {}

  /**
   * @brief Convert BS -> xAOD
   *
   * The implementation should create an xAOD RoI object from the raw data and record it in the event store
   * using a WriteHandle it declares.
   **/
  virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                   const EventContext& eventContext) const = 0;

  /**
   * @brief Convert xAOD -> BS
   *
   * The implementation should take the xAOD RoI object from the event store using a ReadHandle it declares,
   * convert it to raw data, and fill the vrobf vector.
   **/
  virtual StatusCode convertToBS(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                 const EventContext& eventContext) const = 0;

  /**
   * @brief List of IDs of ROBs which the convert methods expect in the vrobf input/output parameter
   *
   * The implementation has to hold a Gaudi::Property<vector<uint32_t>> to declare the ROB IDs it requires/provides
   * and this method has to return the value of this property. There is no easy way to declare a Gaudi::Property here
   * in the interface, so it is delegated to the implementation.
   **/
  virtual const std::vector<uint32_t> robIds() const = 0;
};

#endif // TRIGT1RESULTBYTESTREAM_IL1TRIGGERBYTESTREAMTOOL_H
