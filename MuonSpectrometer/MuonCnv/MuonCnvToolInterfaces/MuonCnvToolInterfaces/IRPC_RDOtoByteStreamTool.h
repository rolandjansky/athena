/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IRPC_RDOTOBYTESTREAMTOOL_H
#define MUONCNVTOOLINTERFACES_IRPC_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"

class RpcPadContainer; 
class MsgStream ; 
class RawEventWrite;

static const InterfaceID IID_IRPC_RDOtoByteStreamTool( "Muon::IRPC_RDOtoByteStreamTool", 1, 0 );

namespace Muon {

/** An AlgTool class to provide conversion from LArRawChannelContainer
  *  to ByteStream, and fill it in RawEvent
  *  created:  Sept 25, 2002, by Hong Ma 
  *  requirements:   typedef for CONTAINER class method 
  *  statusCode convert(CONTAINER* cont, RawEvent* re, MsgStream& log ); 
  *
  * Adapted for Muons by Ketevi A. Assamagan
  * Jan-14-2003, BNL
  * Conversion of Rpc Pad Container to byte stream
  */
class IRPC_RDOtoByteStreamTool: public IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRPC_RDOtoByteStreamTool; };

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** Conversion method, which takes the RDO container and converts it into raw data, filled into RawEventWrite.
  @param cont RDO container which will be used to fill the raw event
  @param re Raw event to be filled by this method.
  @param log MsgStream to be filled by method.
  @TODO Do we really need to pass in a logfile? This is a AlgTool and so can provide its own log objects.
  */
  StatusCode convert(RpcPadContainer* cont, RawEventWrite* re, MsgStream& log )=0; 
  
};
}
#endif



