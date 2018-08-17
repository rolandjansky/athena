/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "InDetRawData/SCT_RDO_Container.h"

class InterfaceID;

/** Interface to an AthAlgTool class providing conversion from SCT RDO container
 *  to ByteStream.
 *  created:  Oct 16 Susumu Oda
 *  Based on ISCTRawContByteStreamTool.h
 */

class ISCTRawContByteStreamTool: virtual public IAlgTool {
 public:

  //! destructor 
  virtual ~ISCTRawContByteStreamTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCTRawContByteStreamTool, 1, 0);

  /** convert() method */
  virtual StatusCode convert(SCT_RDO_Container* cont, 
                             RawEventWrite* re, MsgStream& log) = 0;
  
};

#endif // SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTRAWEVENTTOOL_H
