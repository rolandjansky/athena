/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CNVTOOLS_IMDT_RDOTOBYTESTREAMTOOL_H
#define MUONMDT_CNVTOOLS_IMDT_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h" 

class MdtCsmContainer; 
class MsgStream ; 

static const InterfaceID IID_IMDT_RDOtoByteStreamTool("Muon::IMDT_RDOtoByteStreamTool", 1, 0);

namespace Muon {
  
  
  class IMDT_RDOtoByteStreamTool: virtual public IAlgTool {
    
  public:
    
    //    IMdtCsmContByteStreamTool() {};
    
    typedef MdtCsmContainer CONTAINER ; 
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( );
    
    virtual StatusCode convert(CONTAINER* cont, RawEventWrite* re, MsgStream& log )=0; 
    
  };
  
}
inline const InterfaceID& Muon::IMDT_RDOtoByteStreamTool::interfaceID()
{
  return IID_IMDT_RDOtoByteStreamTool;
}
#endif



