/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tool for RDO to BS conversion 
 -----------------------------------------
 ***************************************************************************/

#ifndef MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H
# define MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h" 

class RpcPadContainer;

static const InterfaceID IID_IRPC_RDOtoByteStreamTool( "Muon::IRPC_RDOtoByteStreamTool", 1, 0 );

namespace Muon {
/*
  An AlgTool to provide conversion between RPC RDO <---> ByteStream,
  and fill it in RawEvent.
  
*/
class IRPC_RDOtoByteStreamTool : virtual public IAlgTool 
{
public:
  static const InterfaceID& interfaceID( ) { return IID_IRPC_RDOtoByteStreamTool; };

  virtual StatusCode convert(RpcPadContainer* cont, RawEventWrite* re)=0;
};
}

#endif // MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H
