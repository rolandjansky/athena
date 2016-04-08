// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"

#include "StoreGate/StoreGateSvc.h"

// Trigger include(s):
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

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
 *    @date $Date: 2008-05-08 21:42:35 $
 */
class RecRoIBResultByteStreamTool : public AthAlgTool {

  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType DataType;

public:
  /// Standard constructor
  RecRoIBResultByteStreamTool( const std::string& type, const std::string& name,
                               const IInterface* parent );
  /// Standard destructor
  virtual ~RecRoIBResultByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// Convert ROBFragments to RecRoIBResult
  template< class ROBF > StatusCode convert( const std::vector< ROBF >& robs,
                                             ROIB::RecRoIBResult*& cont );

private:
  /// Handle to the DetectorStore
  ServiceHandle< StoreGateSvc > m_detectorStore;
  /// Handle to the RPC RecRoISvc
  ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoISvc;
  /// Handle to the TGC RecRoISvc
  ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoISvc;
  /// Handle to the Lvl1ConfigSvc
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

  /// Flag telling whether the RecMuonRoI services have been found
  bool m_muRoISvcPresent;

  /// Store the L1 trigger configuration
  std::vector<TrigConf::TriggerThreshold*> m_muonConfig;
  std::vector<TrigConf::TriggerThreshold*> m_emtauConfig;
  std::vector<TrigConf::TriggerThreshold*> m_jetConfig;

}; // class RecRoIBResultByteStreamTool

// Include the implementation:
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.icc"

#endif // TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMTOOL_H
