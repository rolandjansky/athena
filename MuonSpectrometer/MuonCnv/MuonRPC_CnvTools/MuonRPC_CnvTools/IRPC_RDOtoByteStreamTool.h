/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tool for RDO to BS conversion 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IRPC_RDOtoByteStreamTool.h,v 1.1 2009-02-18 16:26:53 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H
# define MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h" 

class RpcPadContainer;
class MsgStream ; 

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

    //  virtual StatusCode initialize();
    //  virtual StatusCode finalize();

  virtual StatusCode convert(RpcPadContainer* cont, RawEventWrite* re, MsgStream& log)=0;
};
}

#endif // MUONRPC_CNVTOOLS_IRPC_RDOTOBYTESTREAMTOOL_H
