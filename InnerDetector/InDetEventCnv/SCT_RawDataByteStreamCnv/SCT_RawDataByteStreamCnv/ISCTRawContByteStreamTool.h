/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h
 * @author Susumu Oda
 * @date October 2016
 */

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTBYTESTREAMTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTBYTESTREAMTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

/** 
 * @class ISCTRawContByteStreamTool
 * 
 * @brief Interface for Athena Algorithm Tool to provide conversion from SCT RDO container to ByteStream.
 *
 * Conversion from SCT RDO container to ByteStream, and fill it in RawEvent.
 *
 * The class inherits from IAlgTool.
 */
class ISCTRawContByteStreamTool : virtual public IAlgTool 
{
 public:

  /** Creates the InterfaceID and interfaceID() method */
  DeclareInterfaceID(ISCTRawContByteStreamTool, 1, 0);

  /** Destructor */
  virtual ~ISCTRawContByteStreamTool() = default;

  /** Convert method */
  virtual StatusCode convert(const SCT_RDO_Container* sctRDOCont) const = 0;
};

#endif // SCT_RAWDATABYTESTREAMCNV_ISCTRAWCONTBYTESTREAMTOOL_H
