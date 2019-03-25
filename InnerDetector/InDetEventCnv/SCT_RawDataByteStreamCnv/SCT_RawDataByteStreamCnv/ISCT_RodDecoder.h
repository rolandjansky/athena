/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h
 * @author Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch
 *
 * AlgTool class to decode ROB bytestream data into RDO
 */

#ifndef INDETRAWDATABYTESTREAM_ISCT_RODDECODER_H 
#define INDETRAWDATABYTESTREAM_ISCT_RODDECODER_H

#include "GaudiKernel/IAlgTool.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"

#include <vector>

class StatusCode;
class IdentifierHash;

class ISCT_RodDecoder : virtual public IAlgTool 
{
 public: 

  /** Creates the InterfaceID and interfaceID() method */
  DeclareInterfaceID(ISCT_RodDecoder, 1, 0);

  /** Destructor */
  virtual ~ISCT_RodDecoder() = default;

  /** Fill Collection method */
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment&,
                                    ISCT_RDO_Container&,
                                    InDetBSErrContainer* errs,
                                    SCT_ByteStreamFractionContainer* bsFracCont,
                                    const std::vector<IdentifierHash>* vecHash = nullptr) const = 0;
};

#endif //SCT_RAWDATABYTESTREAM_ISCT_RODDECODER_H
