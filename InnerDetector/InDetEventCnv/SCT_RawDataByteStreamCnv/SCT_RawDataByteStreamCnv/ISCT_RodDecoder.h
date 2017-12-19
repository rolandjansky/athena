/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @file header for SCT_RodDecoder Class
 *      AlgTool class to decode ROB bytestream data into RDO
 *      @author: Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch
 *      @current developer: Kondo Gnanvo, QMUL (London), august 2005
 */

#ifndef INDETRAWDATABYTESTREAM_ISCT_RODDECODER_H 
#define INDETRAWDATABYTESTREAM_ISCT_RODDECODER_H
//STL
#include <vector>

//#include "eformat/Version.h"
#include "GaudiKernel/IAlgTool.h"
//Cannot fwd declare SCT_RDO_Container due to typedef in that file 
#include "InDetRawData/SCT_RDO_Container.h"
//Cannot fwd declare RawEvent, due to typedef in the .h. Needed for OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment
#include "ByteStreamData/RawEvent.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

class IInterface;
class InterfaceID;
class StatusCode;

class IdentifierHash;

class ISCT_RodDecoder : virtual public IAlgTool {
 public: 

  /** destructor  */
  virtual ~ISCT_RodDecoder(){}; 

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** @brief Decode the rob data fragment and fill the collection SCT_RDO_Collection 
   *  with the RDO built by the makeRDO(..) method
   **/
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment&,
                                    ISCT_RDO_Container&,
                                    InDetBSErrContainer* errs,
                                    std::vector<IdentifierHash>* vec=0) = 0;
};

inline const InterfaceID& ISCT_RodDecoder::interfaceID() {
  static const InterfaceID ISCT_RodDecoderIID("ISCT_RodDecoder",1,0);
  return ISCT_RodDecoderIID;
}

#endif //SCT_RAWDATABYTESTREAM_ISCT_RODDECODER_H
