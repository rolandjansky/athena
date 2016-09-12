/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "InDetRawData/SCT_RDO_Container.h"

class InterfaceID;

/** Interface to an AthAlgTool class providing conversion from SCT RDO container
 *  to ByteStream.
 *  created:  Jan 09 Nick Barlow 
 */

class ISCTRawContByteStreamTool: virtual public IAlgTool {
 public:

  //! destructor 
  virtual ~ISCTRawContByteStreamTool() {} ;
 
  static const InterfaceID& interfaceID( ) ;

  /** convert() method which makes use of the encoder AlgTool */

  virtual StatusCode convert(SCT_RDO_Container* cont, 
			     RawEventWrite* re, MsgStream& log) = 0 ; 
  
};

inline const InterfaceID& ISCTRawContByteStreamTool::interfaceID( )
{ 
  static const InterfaceID ISCTRawContByteStreamToolIID("SCTRawContByteStreamTool", 1, 0);
  return ISCTRawContByteStreamToolIID; 
}

#endif



