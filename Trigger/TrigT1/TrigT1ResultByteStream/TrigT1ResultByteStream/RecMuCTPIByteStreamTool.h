// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamData/RawEvent.h"

// Trigger include(s):
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// Forward declaration(s):
class MuCTPI_RIO;
class MuCTPISrcIdMap;

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
 *    @date $Date: 2008-05-08 21:42:35 $
 */
class RecMuCTPIByteStreamTool : public AthAlgTool {

private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

public:
  /// Default constructor
  RecMuCTPIByteStreamTool( const std::string& type, const std::string& name,
                           const IInterface* parent );
  /// Default destructor
  virtual ~RecMuCTPIByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// Convert ROBFragment to MuCTPI_RIO
  StatusCode convert( const ROBF* rob, MuCTPI_RIO*& cont );

private:
  /// Object storing the various IDs of the MuCTPI fragment
  MuCTPISrcIdMap* m_srcIdMap;
  /// Internal flag showing whether the RecRoI services were found
  bool m_cnvSvcPresent;
  /// Handle to the DetectorStore
  ServiceHandle< StoreGateSvc > m_detectorStore;
  /// Handle to the RPC RecRoISvc
  ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoISvc;
  /// Handle to the TGC RecRoISvc
  ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoISvc;
  /// Handle to the Lvl1ConfigSvc
  bool m_configSvcPresent;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

}; // class RecMuCTPIByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMTOOL_H
