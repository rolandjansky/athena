// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOBYTESTREAM_L1TOPOBYTESTREAMTOOL_H
#define L1TOPOBYTESTREAM_L1TOPOBYTESTREAMTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Local include(s):
#include "L1TopoSrcIdMap.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

// Forward declaration(s):
class L1TopoRDO;
class L1TopoRDOCollection;

/**
 *   @short Tool doing the L1TopoRDO <-> ByteStream conversion
 *
 *          The actual work of translating the information in a L1TopoRDO object
 *          into a L1Topo ROB fragment and vice-versa is done with this tool.
 *          Based on CTPByteStreamTool by David Berge.
 *
 *     @see L1TopoByteStreamCnv
 *
 *  @author Simon George
 *    @date $Date: 2014-11-12 00:00:00 $
 */
class L1TopoByteStreamTool : public AthAlgTool {
 private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

 public:
  /// Default constructor
  L1TopoByteStreamTool(const std::string& type, const std::string& name,
                       const IInterface* parent);
  /// Default destructor
  virtual ~L1TopoByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// return list of L1Topo ROD source IDs to use, defaults to DAQ ROD IDs
  const std::vector<uint32_t>& sourceIDs();
  /// Convert ROBFragment to L1TopoRDO
  StatusCode convert(const std::string& sgKey, L1TopoRDOCollection* result) const;
  /// Convert ROBFragment to L1TopoRDO
  StatusCode convert(const ROBF* rob, L1TopoRDO*& result) const;
  /// convert L1TopoRDO to ByteStream
  StatusCode convert(const L1TopoRDO* result, RawEventWrite* re) const;

 private:
  /// Object to generate and convert between the various IDs of the L1Topo
  /// fragment
  L1TopoSrcIdMap* m_srcIdMap;
  /// Source IDs of L1Topo RODs
  std::vector<uint32_t> m_sourceIDs;

  BooleanProperty m_doDAQROBs;
  BooleanProperty m_doROIROBs;

 private:
  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
};  // class L1TopoByteStreamTool

#endif  // L1TOPOBYTESTREAM_L1TOPOBYTESTREAMTOOL_H
