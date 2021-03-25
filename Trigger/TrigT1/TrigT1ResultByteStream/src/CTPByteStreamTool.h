// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Local include(s):
#include "CTPSrcIdMap.h"

// Forward declaration(s):
class CTP_RDO;

/**
 *   @short Tool doing the CTP_RDO <-> ByteStream conversion
 *
 *          The actual work of translating the information in a CTP_RDO object
 *          into a CTP ROB fragment and vice-versa is done with this tool.
 *
 *     @see CTPByteStreamCnv
 *
 *  @author David Berge
 *    @date $Date: 2007-12-14 15:33:07 $
 */
class CTPByteStreamTool : public AthAlgTool {

private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

public:
  /// Default constructor
  CTPByteStreamTool( const std::string& type, const std::string& name,
                     const IInterface* parent );

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Convert ROBFragment to CTP_RDO
  StatusCode convert( const ROBF* rob, CTP_RDO*& result );
  /// convert CTP_RDO to ByteStream
  StatusCode convert( const CTP_RDO* result, RawEventWrite* re );

private:
  /// Object storing the various IDs of the CTP fragment
  CTPSrcIdMap                       m_srcIdMap;
  /// Object used in creating the CTP ROB fragment
  FullEventAssembler< CTPSrcIdMap > m_fea;

}; // class CTPByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMTOOL_H
