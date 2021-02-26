/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H

// Local includes
#include "L1SrcIdMap.h"

// Trigger includes
#include "L1TopoRDO/L1TopoRDO.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

// Athena includes:
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamData/RawEvent.h"

// System includes
#include <vector>

// Forward declaration(s):
namespace ROIB {
  class RoIBResult;
}

/**
 *   @short Tool doing the RoIBResult <-> ByteStream conversion
 *
 *          The actual work of translating the information in an RoIBResult object
 *          into ROB fragments and vice-versa is done with this tool.
 *
 *     @see RoIBResultByteStreamCnv
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2008-03-28 17:25:52 $
 */
class RoIBResultByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {

public:
  /// Default constructor
  RoIBResultByteStreamTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
  /// Default destructor
  virtual ~RoIBResultByteStreamTool() = default;

  /// Function to initialise the tool
  virtual StatusCode initialize() override;

  /// Convert ROB fragments to RoIBResult
  virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                   const EventContext& eventContext) const override;

  /// Convert RoIBResult to ByteStream
  virtual StatusCode convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& vrobf,
                                 const EventContext& eventContext) override;

  /// Vector of ROB IDs corresponding to the modules configured for decoding
  virtual const std::vector<uint32_t>& robIds() const override {return m_configuredROBIds;}

private:

  // ------------------------- Private types -----------------------------------
  /// Structure holding the status words and rob/rod error flags
  struct DataStatus {
    bool rob_error {false};
    bool rod_error {false};
    uint32_t status_word {0};
    uint32_t status_info {0};
  };

  // ------------------------- Private methods ---------------------------------
  /**
   * @brief Helper method to extract ROD header information
   * @in rob ROBFragment from which data are extracted
   * @out dataStatus Structure to flag data errors
   **/
  ROIB::Header roibHeader(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob, DataStatus& dataStatus) const;

  /**
   * @brief Helper method to extract ROD payload
   * @in ndata Number of data words
   * @in data Data words
   **/
  template<typename RoIType> std::vector<RoIType> roibContent(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob) const;

  /**
   * @brief Helper method to extract ROD payload for L1Topo (different interface from other RoI data)
   * @in ndata Number of data words
   * @in data Data words
   **/
  L1TopoRDO l1topoContent(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob) const;

  /**
   * @brief Helper method to extract ROD trailer information
   * @in dataStatus Structure to flag data errors (extracted from the header)
   * @in dataSize Number of ROD data words
   **/
  ROIB::Trailer roibTrailer(const DataStatus& dataStatus, const uint32_t dataSize) const;

  // ------------------------- Properties --------------------------------------
  /// @name Properties holding module IDs for L1 RoI ROBs
  /// @{
  /// CTP Module ID to decode
  Gaudi::Property<uint16_t> m_ctpModuleID {
    this, "CTPModuleId", 1,
    "Module ID of CTP ROB with RoI information. Value 0xFF disables CTP decoding/encoding in this tool."
  };
  /// MUCTPI Module ID to decode
  Gaudi::Property<uint16_t> m_muCTPIModuleID {
    this, "MUCTPIModuleId", 1,
    "Module ID of MUCTPI ROB with RoI information. Value 0xFF disables MUCTPI decoding/encoding in this tool."
  };
  /// Jet Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_jetModuleID {
    this, "JetModuleIds", {0xac, 0xad},
    "Vector of module IDs of Jet RoI ROBs. Empty vector disables Jet/Energy decoding/encoding in this tool."
  };
  /// EM Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_emModuleID {
    this, "EMModuleIds", {0xa8, 0xa9, 0xaa, 0xab},
    "Vector of module IDs of EM RoI ROBs. Empty vector disables EMTau decoding/encoding in this tool."
  };
  /// L1Topo Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_l1TopoModuleID {
    this, "L1TopoModuleIds", {0x81,0x91},
    "Vector of module IDs of L1Topo RoI ROBs. Empty vector disables L1Topo decoding/encoding in this tool."
  };
  /// @}

  /// Detector event type to write when converting to ByteStream
  Gaudi::Property<uint16_t> m_detEvType {
    this, "DetEvType", 1, // RB: It was hard-coded to 1 in the past, no idea why
    "Detector event type to write when converting to ByteStream"
  };

  /// @name Data handles
  /// @{
  SG::WriteHandleKey<ROIB::RoIBResult> m_roibResultWriteKey {
    this, "RoIBResultWriteKey", "",
    "Write handle key to RoIBResult for conversion from ByteStream"
  };
  SG::ReadHandleKey<ROIB::RoIBResult> m_roibResultReadKey {
    this, "RoIBResultReadKey", "",
    "Read handle key to RoIBResult for conversion to ByteStream"
  };
  /// @}

  // ------------------------- Other private members ---------------------------
  /// Vector of ROB IDs corresponding to the modules configured for decoding
  std::vector<uint32_t> m_configuredROBIds;
  /// Interface to get raw event pointer for updating event header when writing to BS
  SmartIF<IByteStreamEventAccess> m_byteStreamEventAccess;
  /// @name Flags to switch decoding/encoding of each system
  /// @{
  bool m_doCTP {true};
  bool m_doMuon {true};
  bool m_doJetEnergy {true};
  bool m_doEMTau {true};
  bool m_doTopo {true};
  /// @}

}; // class RoIBResultByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
