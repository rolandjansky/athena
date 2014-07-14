/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_ITGC_RDOTOBYTESTREAMTOOL_H
#define MUONCNVTOOLINTERFACES_ITGC_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

class TgcRdoContainer; 
class MsgStream ; 

static const InterfaceID IID_ITGC_RDOtoByteStreamTool( "Muon::ITGC_RDOtoByteStreamTool", 1, 0 );

namespace Muon {

  /** An AlgTool class to provide conversion from TgcRdoContainer
   *  to ByteStream, and fill it in RawEvent
   *  
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * 
   * This code was adapted from LAr by Ketevi A. Assamagan
   * on Jan-14-2003.
   */
  class ITGC_RDOtoByteStreamTool: virtual public IAlgTool {
    
  public:
    
    static const InterfaceID& interfaceID() { return IID_ITGC_RDOtoByteStreamTool; };
    
    virtual StatusCode initialize()=0;
    virtual StatusCode finalize()=0;
    
    /** Conversion method, which takes the RDO container and converts it into raw data, filled into RawEventWrite.
     *  @param cont RDO container which will be used to fill the raw event
     *  @param re Raw event to be filled by this method.
     *  @param log MsgStream to be filled by method.
     *  @TODO Do we really need to pass in a logfile? This is a AlgTool and so can provide its own log objects.
     */
    virtual StatusCode convert(const TgcRdoContainer* cont, RawEventWrite* re, MsgStream& log )=0; 
  };
}
#endif



