// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

// Local include(s):
#include "TrigT1ResultByteStream/MuCTPISrcIdMap.h"

// Forward declaration(s):
class MsgStream;
class MuCTPI_RDO;

/**
 *   @short Tool doing the MuCTPI_RDO <-> ByteStream conversion
 *
 *          The actual work of translating the information in a MuCTPI_RDO object
 *          into a MuCTPI ROB fragment and vice-versa is done with this tool.
 *
 *     @see MuCTPIByteStreamCnv
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2007-12-14 15:33:08 $
 */
class MuCTPIByteStreamTool : public AthAlgTool {

private:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

public:
  /// Default constructor
  MuCTPIByteStreamTool( const std::string& type, const std::string& name,
                        const IInterface* parent );
  /// Default destructor
  virtual ~MuCTPIByteStreamTool();

  /// AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  /// Function to initialise the tool
  virtual StatusCode initialize();
  /// Function to finalise the tool
  virtual StatusCode finalize();

  /// Convert ROBFragment to MuCTPI_RDO
  StatusCode convert( const ROBF* rob, MuCTPI_RDO*& result );
  /// convert MuCTPI_RDO to ByteStream
  StatusCode convert( const MuCTPI_RDO* result, RawEventWrite* re );

private:
  /// Object storing the various IDs of the MuCTPI fragment
  MuCTPISrcIdMap*                      m_srcIdMap;
  /// Object used in creating the MuCTPI ROB fragment
  FullEventAssembler< MuCTPISrcIdMap > m_fea;

}; // class MuCTPIByteStreamTool

#endif // TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMTOOL_H
