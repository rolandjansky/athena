// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Local include(s):
#include "L1SrcIdMap.h"

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
  template< class ROBF > StatusCode convert( const std::vector< ROBF >& robs, ROIB::RoIBResult*& cont );
  /// Convert RoIBResult to ByteStream
  StatusCode convert( ROIB::RoIBResult* cont, RawEventWrite* re );

private:
  /// Object used in creating the RoI Builder ROB fragments
  FullEventAssembler<L1SrcIdMap> m_fea;

  typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType DataType;

  /// variables that hold module IDs for L1 RoI ROBs
  uint32_t m_ctpModuleID;
  uint32_t m_muCTPIModuleID;
  std::vector<uint32_t> m_jetModuleID;
  std::vector<uint32_t> m_emModuleID;
  std::vector<uint32_t> m_l1TopoModuleID;

}; // class RoIBResultByteStreamTool

// Include the implementation:
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.icc"

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMTOOL_H
