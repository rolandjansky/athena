// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"

// Trigger include(s):
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// Local include(s):
#include "MuCTPISrcIdMap.h"

// Forward declaration(s):
class MuCTPI_RIO;

/**
 *   @short Tool doing the ByteStream -> MuCTPI_RIO conversion
 *
 *          The actual work of translating the information in a MuCTPI ROB fragment
 *          into a MuCTPI_RIO object is done with this tool.
 *
 *     @see CTPByteStreamCnv
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *  @author David Berge
 *  @author Christian Ohm
 */
class RecMuCTPIByteStreamTool : public AthAlgTool {

private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

public:
  /// Default constructor
  RecMuCTPIByteStreamTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize() override;

  /// Convert ROBFragment to MuCTPI_RIO
  StatusCode convert( const ROBF* rob, MuCTPI_RIO*& cont );

private:
  /// Object storing the various IDs of the MuCTPI fragment
  MuCTPISrcIdMap m_srcIdMap;

  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_rpcRoITool {
    this, "RPCRecRoiSvc", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC RoI reconstruction tool"
  };
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_tgcRoITool {
    this, "TGCRecRoiSvc", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC RoI reconstruction tool"
  };
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc {
    this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", "LVL1 Config Service"
  };

}; // class RecMuCTPIByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H
