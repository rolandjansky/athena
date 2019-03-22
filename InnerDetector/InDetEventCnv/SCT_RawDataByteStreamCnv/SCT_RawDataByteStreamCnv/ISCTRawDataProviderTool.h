/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h
 * 
 * Athena Algorithm Tool to fill Collections of SCT RDO Containers.
 */

#ifndef SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H

#include "ByteStreamData/RawEvent.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"

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

  /** Main decoding method */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&,
                             ISCT_RDO_Container&,
                             InDetBSErrContainer* errs,
                             SCT_ByteStreamFractionContainer* bsFracCont) const = 0;

  /** Reset list of know ROB IDs */
  virtual void beginNewEvent() const = 0;
};

#endif // SCT_RAWDATABYTESTREAMCNV_ISCTRAWDATAPROVIDERTOOL_H
