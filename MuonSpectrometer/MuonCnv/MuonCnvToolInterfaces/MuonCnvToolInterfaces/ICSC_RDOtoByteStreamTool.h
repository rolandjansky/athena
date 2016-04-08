/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCNVTOOLINTERFACES_ICSC_RDOTOBYTESTREAMTOOL_H
#define MUONCNVTOOLINTERFACES_ICSC_RDOTOBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

class CscRawDataContainer; 
class MsgStream ; 
//class RawEventWrite;

static const InterfaceID IID_ICSC_RDOtoByteStreamTool( "Muon::ICSC_RDOtoByteStreamTool", 1, 0 );

namespace Muon {
/*
  An AlgTool to provide conversion between CSC RDO <---> ByteStream,
  and fill it in RawEvent.
  
  @author Ketevi A. Assamagan BNL December 27 2003
*/
class ICSC_RDOtoByteStreamTool : virtual public IAlgTool 
{

public:


  static const InterfaceID& interfaceID( ) { return IID_ICSC_RDOtoByteStreamTool; };

  /** to read the cosmic data */ 
  virtual bool isCosmic () const =0;

  /** for the old cosmic data before the ROB id=ROD id fix */
  virtual bool isOldCosmic () const =0;

  virtual StatusCode convert(const CscRawDataContainer* cont, RawEventWrite* re, MsgStream& log)=0;
};
}

#endif



