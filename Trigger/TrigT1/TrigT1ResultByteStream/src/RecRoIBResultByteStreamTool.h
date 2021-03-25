/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H

// Trigger includes
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

// Athena includes:
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"

// System includes
#include <vector>

// Forward declaration(s):
namespace ROIB {
  class RecRoIBResult;
}

/**
 *   @short Tool doing the ByteStream -> RecRoIBResult conversion
 *
 *          The actual work of translating the information in a RoIB ROB fragments
 *          into a RecRoIBResult object is done with this tool.
 *
 *     @see RecRoIBResultByteStreamCnv
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 */
class RecRoIBResultByteStreamTool : public AthAlgTool {

public:
  /// Standard constructor
  RecRoIBResultByteStreamTool( const std::string& type, const std::string& name,
                               const IInterface* parent );
  /// Standard destructor
  virtual ~RecRoIBResultByteStreamTool() = default;

  /// Function to initialise the tool
  virtual StatusCode initialize() override;

  /// Convert ROB fragments to RecRoIBResult
  StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                      ROIB::RecRoIBResult& resultToFill ) const;
  /// Convert ROBFragments to RecRoIBResult (legacy Run-2 interface)
  template< class ROBF > [[deprecated("Use the non-template version instead")]]
  StatusCode convert( const std::vector< ROBF >& robs, ROIB::RecRoIBResult*& cont ) const;

  /// Vector of ROB IDs corresponding to the modules configured for decoding
  const std::vector<uint32_t>& configuredROBIds() const {return m_configuredROBIds;}

private:

  // ------------------------- Properties --------------------------------------
  /// @name Properties holding module IDs for L1 RoI ROBs
  /// @{
  /// MUCTPI Module ID to decode
  Gaudi::Property<uint16_t> m_muCTPIModuleID {
    this, "MUCTPIModuleId", 1,
    "Module ID of MUCTPI ROB with RoI information"
  };
  /// Jet Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_jetModuleID {
    this, "JetModuleIds", {0xac, 0xad},
    "Vector of module IDs of Jet RoI ROBs"
  };
  /// EM Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_emModuleID {
    this, "EMModuleIds", {0xa8, 0xa9, 0xaa, 0xab},
    "Vector of module IDs of EM RoI ROBs"
  };
  // CTP and L1Topo ROBs are currently not used in RecRoIBResult
  /// @}

  // ------------------------- Service/Tool handles ---------------------------------
  /// Handle to the Lvl1ConfigSvc interface
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc {
    this, "LVL1ConfigSvc", "TrigConf::TrigConfigSvc/TrigConfigSvc", "LVL1 Trigger configuration service"
  };
  /// Handle to the RPC RecRoITool
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_rpcRoITool {
    this, "RPCRecRoiSvc", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC RoI reconstruction tool"
  };
  /// Handle to the TGC RecRoITool
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_tgcRoITool {
    this, "TGCRecRoiSvc", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC RoI reconstruction tool"
  };

  // ------------------------- Other private members ---------------------------
  /// Vector of ROB IDs corresponding to the modules configured for decoding
  std::vector<uint32_t> m_configuredROBIds;
  /// L1 Muon configuration cache
  std::vector<TrigConf::TriggerThreshold*> m_muonConfig;
  /// L1 EM/Tau configuration cache
  std::vector<TrigConf::TriggerThreshold*> m_emtauConfig;
  /// L1 Jet configuration cache
  std::vector<TrigConf::TriggerThreshold*> m_jetConfig;

}; // class RecRoIBResultByteStreamTool

// Implementation of the legacy template method has to be included in the header file to avoid linking errors
template< class ROBF >
StatusCode RecRoIBResultByteStreamTool::convert(const std::vector< ROBF >& robs,
                                             ROIB::RecRoIBResult*& result ) const {
  ATH_MSG_WARNING("This convert method is deprecated! Use the non-template version instead");
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vrobf;
  for (const ROBF& robf : robs) vrobf.push_back(&robf);
  return convert(vrobf,*result);
}

#endif // TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H
