/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_IMDT_RDOTOBYTESTREAMTOOL_H
#define MUONCNVTOOLINTERFACES_IMDT_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"

class MdtCsmContainer; 
class MsgStream ; 

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
class IMDT_RDOtoByteStreamTool: virtual public IAlgTool {

public:

  DeclareInterfaceID( IMDT_RDOtoByteStreamTool, 1, 0 );

  /** Conversion method, which takes the RDO container and converts it into raw data, filled into RawEventWrite.
  @param cont RDO container which will be used to fill the raw event
  @param log MsgStream to be filled by method.
  @todo Do we really need to pass in a logfile? This is a AlgTool and so can provide its own log objects.
  */
  virtual StatusCode convert (const MdtCsmContainer* cont, MsgStream& log ) const = 0; 
  
};

} // namespace Muon

#endif



