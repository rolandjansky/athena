// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H

// Local includes
#include "L1SrcIdMap.h"

// Trigger includes
#include "L1TopoRDO/L1TopoRDO.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

// Athena includes:
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
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
class RoIBResultByteStreamTool : public AthAlgTool {

public:
  /// Default constructor
  RoIBResultByteStreamTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
  /// Default destructor
  virtual ~RoIBResultByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// Convert ROB fragments to RoIBResult
  StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                      ROIB::RoIBResult& resultToFill ) const;
  /// Convert ROB fragments to RoIBResult (legacy Run-2 interface)
  template< class ROBF > StatusCode convert( const std::vector< ROBF >& robs, ROIB::RoIBResult*& cont );
  /// Convert RoIBResult to ByteStream
  StatusCode convert( ROIB::RoIBResult* cont, RawEventWrite* re );

  /// Vector of ROB IDs corresponding to the modules configured for decoding
  const std::vector<uint32_t>& configuredROBIds() const {return m_configuredROBIds;}

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
    "Module ID of CTP ROB with RoI information"
  };
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
  /// L1Topo Module IDs to decode
  Gaudi::Property<std::vector<uint16_t>> m_l1TopoModuleID {
    this, "L1TopoModuleIds", {0x81,0x91},
    "Vector of module IDs of L1Topo RoI ROBs"
  };
  /// @}

  // ------------------------- Other private members ---------------------------
  /// Vector of ROB IDs corresponding to the modules configured for decoding
  std::vector<uint32_t> m_configuredROBIds;
  /// Object used in creating the RoI Builder ROB fragments
  FullEventAssembler<L1SrcIdMap> m_fea;

}; // class RoIBResultByteStreamTool

// Implementation of the legacy template method has to be included in the header file to avoid linking errors
template< class ROBF >
StatusCode RoIBResultByteStreamTool::convert(const std::vector< ROBF >& robs,
                                             ROIB::RoIBResult*& result ) {
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vrobf;
  for (const ROBF& robf : robs) vrobf.push_back(&robf);
  return convert(vrobf,*result);
}

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
