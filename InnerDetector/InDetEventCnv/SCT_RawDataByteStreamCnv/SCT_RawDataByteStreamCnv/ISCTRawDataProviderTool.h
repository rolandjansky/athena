// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h
 * 
 * Athena Algorithm Tool to fill Collections of SCT RDO Containers.
 */

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H

#include "ByteStreamData/RawEvent.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"

#include "GaudiKernel/IAlgTool.h"

/** 
 * @class ISCTRawDataProviderTool
 *
 * @brief Interface for Athena Algorithm Tool to fill Collections of SCT RDO Containers.
 *
 * The class inherits from IAlgTool.
 */
class ISCTRawDataProviderTool : virtual public IAlgTool
{
 public:

  /** Creates the InterfaceID and interfaceID() method */
  DeclareInterfaceID(ISCTRawDataProviderTool, 1, 0);

  /** Destructor */
  virtual ~ISCTRawDataProviderTool() = default;

  /** Main decoding methods */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&,
                             SCT_RDO_Container&,
                             IDCInDetBSErrContainer& errs) const = 0;

};

#endif // SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H
