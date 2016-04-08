// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECCTPBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_RECCTPBYTESTREAMTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Local include(s):
#include "TrigT1ResultByteStream/CTPSrcIdMap.h"

// Forward declaration(s):
class CTP_RIO;

/**
 *   @short Tool doing the ByteStream -> CTP_RIO conversion
 *
 *          The actual work of translating the information in a CTP ROB fragment
 *          into a CTP_RIO object is done with this tool.
 *
 *     @see RecCTPByteStreamCnv
 *
 *  @author David Berge
 *    @date $Date: 2007-12-14 15:33:09 $
 */
class RecCTPByteStreamTool : public AthAlgTool {

private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

public:
  /// Default constructor
  RecCTPByteStreamTool( const std::string& type, const std::string& name,
                        const IInterface* parent );
  /// Default destructor
  virtual ~RecCTPByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// Convert ROBFragment to CTP_RIO
  StatusCode convert( const ROBF* rob, CTP_RIO*& result );

private:
  /// Object storing the various IDs of the CTP fragment
  CTPSrcIdMap* m_srcIdMap;

}; // class RecCTPByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_RECCTPBYTESTREAMTOOL_H
