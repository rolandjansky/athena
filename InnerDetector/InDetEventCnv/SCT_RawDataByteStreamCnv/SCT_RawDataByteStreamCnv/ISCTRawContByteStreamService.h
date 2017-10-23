/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTSERVICE_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTSERVICE_H

#include "GaudiKernel/IInterface.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "InDetRawData/SCT_RDO_Container.h"

class InterfaceID;

/** Interface to an AthService class providing conversion from SCT RDO container
 *  to ByteStream.
 *  created:  Oct 16 Susumu Oda
 *  Based on ISCTRawContByteStreamTool.h
 */

class ISCTRawContByteStreamService: virtual public IInterface {
 public:

  //! destructor 
  virtual ~ISCTRawContByteStreamService() {} ;
 
  static const InterfaceID& interfaceID( ) ;

  /** convert() method */

  virtual StatusCode convert(SCT_RDO_Container* cont, 
                             RawEventWrite* re, MsgStream& log) = 0 ; 
  
};

inline const InterfaceID& ISCTRawContByteStreamService::interfaceID( )
{ 
  static const InterfaceID ISCTRawContByteStreamServiceIID("ISCTRawContByteStreamService", 1, 0);
  return ISCTRawContByteStreamServiceIID; 
}

#endif
